/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

                            M A I N     T A S K


GENERAL DESCRIPTION

  This source file contains the main GSDI task along with supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2011 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/mmgsdi/src/gsdi.c#5 $$ $DateTime: 2011/04/28 23:26:10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/09   mib     mmgsdi_init_cache_binary checks if cache already inited
03/22/11   ssr     Fixed the MEID cache
03/08/11   ssr     Add non 3gpd cdma card check
03/08/11   ssr     Setting NVRUIM control flags inside of 
                   gsdi_cdma_post_pin1_init
02/24/11   nmb     memset allocated cmd buffer space in gsdi_get_uim_buf
01/18/10   ssr     Fixed len for store_esn command
07/02/09   ssr     Operator MCC Check for 3GPD
05/22/09   js      Added gsdi support for SELECT command for RUIM
05/15/09   tml     Fixed compilation warnings
05/14/09   kp      Added compiler directive for demand Paging Changes
05/12/09   kk      Fixed passing correct parameter to refresh_complete API
05/11/09   kp      Demand Paging Changes
05/05/09   js      Fixed warning
04/29/09   kk      Updated EF file path info for non-spec defined telecom EFs
04/29/09   js      Added prototypes for static functions
04/14/09   mib     Added flag to ignore errors in mmgsdi_proc_post_pin1_init
04/09/09   kk      Updated 5th byte in content that is written into EF-TST
03/25/09   sun     Added support for Virtuim
03/04/09   mib     Get perso keys if FEATURE_MMGSDI_PERSO_GET_KEY not defined
02/12/09   kk      Add files to refresh file list for re-reading during FCN
02/03/09   ssr     Read from NV_MEID_ME_I to ensure that MEID value,
                   should be come form NV memory, not from the card.
01/22/09   sun     Removed GSDI Refresh timer
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/26/08   nb      Removed Featurization Flags for Common Modem Interface
                   and moved gsdi_get_card_mode() function to gsdilib.c
11/26/08   kk      Updated handling gsdi_cache_pin_status for CDMA & GSM mode
11/12/08   ssr     Removed compiler warnings
11/10/08   sun     Removed early returns from sim_inserted function
10/14/08   xz      Move FEATURE_ENHANCED_NW_SELECTION into
                   gsdi_is_ens_enabled_in_nv()
10/10/08   sun     Replaced gsdi_sim_refresh with mmgsdi_naa_refresh
10/09/08   tml     Fixed compilation warnings
10/06/08   jk      Changes to support GSM SIM Access in 1X mode
10/01/08   js      Set default value for gsdi_gprs_anite_nv_status to FALSE
09/29/08   xz      Expose gsdi_is_ens_enabled_in_nv() as external
09/25/08   kk      Added support for extracting unblock dck retries available
09/22/08   js      Fixed Lint Errors
08/27/08   tml     Fixed compilation warning in refresh
08/01/08   tml     Remove duplicate select req to uim
07/24/08   sp      Remove debug message
06/23/08   sp      Don't clean up the global that was populated as a result
                   of EF-DIR aid read during the reselection of an app.
06/22/08   tml     Fixed debug message hex print
06/21/08   kk      Added PKCS15 Support
06/04/08   tml     Fixed debug message print out
05/01/08   nk      Added Support to allow initialization to complete when in FTM
04/30/08   kk      Added support for MMS EFs
04/16/08   kk      Added support of get command on sim lock data
03/20/08   nk      Added OMH EF Support
03/18/08   vs      Changed write access condition of EF PRL
03/14/08   sk      Reverted the change for usage of signals during
                   sim initialization
02/26/08   js      Support for updating GPRS Anite NV by reading IMSI
02/21/08   sun     Added new parameters for CPHS read
02/14/08   kk      Removed the Usage of signals during sim initialization
02/04/07   js      Added fix to set UIM Proactive to true by default for UICC card
01/29/08   sun     Added new state for refresh - this state helps post pin1
                   procedure not to be carried out again and again when
                   a refresh reset is in progress.
12/12/07   tml     Fixed compilation warnings and lint fixes
11/19/07   js      Radio Access Technology Balancing support
11/15/07   sun     Added support for NAA Refresh
11/15/07   tml     Remove extra client registration signal
11/12/07   vs      Fixed compiler warnings
11/08/07   vs/nk   Added EUIMID and OMH feature support
10/02/07   tml     Added proprietary app session support
10/01/07   sk      Fixed warning
09/23/07   sk      Moved reading of EF PL to process link establish stage
09/10/07   tml     Fixed 1000 PBM and MBMS files support
07/26/07   sp      Increment the counter inside while loop
07/11/07   sun     No need to wait for any clients for Perso in LTK
06/25/07   sp      Delete the path_lookup_table for refresh modes
05/31/07   sp      Fixed compile error
05/30/07   sp      Send status command for app termination in uicc reset
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for MBMS
04/12/07   sun     Featurized Phonebook
04/03/07   sp      Send status command before app termination
04/03/07   sun     If some client is making duplicate registrations, then GSDI
                   should use the same index instead of creating a new entry.
03/26/07   sun     Fixed the response type for refresh
03/16/07   sk      Support Refresh-FCN related to callstack while in a call
03/19/07   sun     Fixed Lint Error
03/15/07   nk      Fixed malloc of cmd pointer in gsdi_cmd
02/21/07   sun     Remove GSDI Global PIN
02/27/07   jk      Added featurization for Memory Reduction
02/09/07   sk      Added support for 1000 phonebook entries.
02/01/07   tml     Null pointer check
02/01/07   jk      Changes to prevent warnings when FEATURE_GSTK is off
01/29/07   tml     Only remove from GSDI FCN for HPLMN
01/19/07   jk      Featurized function call that should only be made with GSTK
01/23/07   sp      Fix issues with ThinUI build and removed
                   pin_just_verified flag
01/18/07   sun     Fixed services type for CBMI
01/16/07   pv      Use allocated memory instead of using the stack for
                   mmgsdi_event_data_type in gsdi_send_sim_inserted_event()
01/15/07   pv      Initialize gsdi_uim_sanity_timer_value to a default value
                   which would later be changed to the actual value provided
                   by UIM upon link establishment.
12/27/06   sp      Back out pin event changes.
12/15/06   tml     Aded support for GSDI_REFRESH_SUCCESS_BUT_NO_AID_ACTIVE
12/14/06   tml     Allow correct TR to be sent to card when app is inactive
                   or unknown AID
12/12/06   tml     Update session_build_select_aid with additional param
12/14/06   sp      Fixed compilation warning
12/04/06   sp      Perform perso operation for various refresh commands.
11/28/06   tml     refresh fcn max num file check
11/06/06   sp      Proceed with post pin functions only after pin event
                   is sent
10/20/06   sun     Do not wait for Card Selection if NV item is set
10/19/06   sp      Fixed Lint errors.
10/17/06   sp      Fixed warning and lint errors.
10/13/06   sp      Read ens flag from nv before sending for tp download
10/13/06   tml     Return Error for 3G Session and APP Reset refresh in a
                   SIM Card
10/05/06   pv      Add UIM_TELECOM_BY_PATH_ANRC and UIM_TELECOM_BY_PATH_ANRC1
                   to the cache table.  Correct the location of UIM_GSM_CFIS
                   in gsdi_gsm_cache_tbl cache table.
10/02/06   sp      Read the me support for fdn, bdn and acl from nv
09/28/06   sk      Featurized gsdi_gstk_terminal_profile_download under
                   FEATURE_GSTK
09/06/06   tml     Add refresh AID notification support
08/28/06   jar     Fix to allow a Terminal Profile Download when Feature
                   is not defined after a USIM Application activation.
08/25/06   jar     Added Support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/16/06   jar     Added UIM_USIM_ANRC and UIM_USIM_ANRC1 to the PB Path
                   path lookup table.
08/01/06   tml     boundary check for refresh notify list
07/28/06   sun     Added support for Multi App and Lint fixes
07/24/06   tml     compilation fix for L4 build
07/05/06   jar     Fixes to allow for more than 4 ANRs to be used.
06/15/06   tml     Fixed compilation warning
06/12/06   pv      Check if cnf_ptr is NULL before accessing.
06/12/06   tml     fixed compilation issue
06/08/06   tml     Fixed featurization
05/31/06   sk      Added messages for Refresh FCNs.
06/02/06   jar     Added entries into GSDI_FILE_SERVICE_TABLE for
                   MMGSDI_GSM_ACM and MMGSDI_GSM_PUCT
06/01/06   jar     Fixed Logic Error where GSDI was not allowing Refresh FCN
                   when in a PS or CS Call.
05/31/06   tml     Fixed compilation error
05/22/06   tml     Fixed sending refresh TR issue
05/09/06   tml     Refresh fix and cleanup
05/05/06   pv      Always reset the card before trying GSM instruction class
05/03/06   sp      Send pin status after sim init.
04/21/06   su      Allow different number of client reg based on feature for
                   perso
04/20/06   nk      back out pin_status support and add featurization
04/18/06   sp      Added support to send pin_status evt after sim_insert.
04/05/06   tml     Make sure TR is sent if can't get command for
                   SIM INIT + FCN
03/30/06   tml     ensure return ERROR for gsdi_cmd_ext if high priority
                   and normal priority total count is GSDI_CMD_QUEUE_MAX_SIZE
03/30/06   tml     Cap max command support to GSDI_CMD_QUEUE_MAX_SIZE
03/21/06   tml     added cfis and missing service table support and lint
03/11/06   tml     correctly assign req type of request processing
03/10/06   tml     Fixed compilation error
03/10/06   tml     Do not add another callback for registration FCN if
                   the callback is already existed
03/09/06   tml     Clear UIM Get Response cache when refresh
03/08/06   jar     Merged in Security Library Solutions for SIM Lock
02/21/06   sp      Updating gsdi_file_service_table for EHPLMN
02/15/06   tml     Lint fix
16/02/06   sp      Support for EF_EHPLMN
2/14/06    nk      Merge for: Bring file attribute setting to avoid synchronous
                   file attribute access and ARR Caching merge and
                   get fle attribute caching additional support
02/08/06   jar     Reset the Instruction class to force GSM Like operation
                   when USIM ECC is not found.  Updated copywrite info
01/30/06   jar     Modifications to print out a few post pin1 init files
01/30/06   pv      Handle all specific error reasons reported by UIM
01/09/06   tml     Fixed compilation error
01/05/06   tml     Fixed memory overwrite issue
11/22/05   jk      Changes to support multiprocessor RPC
11/17/05   tml     Default uim_cmd channel to channel 0
11/14/05   tml     Fixed compilation warning
11/10/05   tml     Added Lint fixes
11/10/05   tml     Revert Lint Fixes
11/09/05  sun/tml  Fixed lint errors and allow more than 6 clients to
                   register successfully
11/08/05   pv      Added code to send a status command to UIM indicating that
                   the ME is ready for a  3G session.
11/02/05   tml     Added L4 fixes
10/30/05   tml     Compilation fix
10/28/05   sun     Featurized support for cache file attributes
10/20/05   sun     Added support to cache file attributes
10/18/05   tml     Added in_synch info to the cache
10/18/05   tml     Send terminal profile first before UICC application check
10/13/05   tml     Refresh FCN, call stack update global variable reset
10/13/05   tml     Refresh FCN fix for files required Call Stack update
10/07/05   tml     Refresh HPLMN fix
09/23/05   sun     Regardless of card type, set the 4 byte to 1 in EF-TST
8/30/05    jar     Register MMGSDI as a Client
                   Send a Status Command after the PIN is verified.
08/23/05   tml     Fixed SIM Init with (Full) File change Notification issue
08/16/05   jk      Send out card error if there is still unknown card app after
                   get card mode capabilities
08/02/05   tml     Send out card error if perso post pin1 init failed
07/29/05   sun     Fixed TST Length for USIM
07/27/05   jk      Moved gsdi_data atr and sanity timer set under
                   link_established if condition to fix Dual Slot issue when
                   one card is pulled
07/25/05   pv      Use cache_record for MMGSDI_GSM_ACM
07/18/05   sun     Added support for USIM files in ENS
07/19/05   tml     Removed consortium code
07/18/05   jk      Fixed Compile Issues for 1x Targets
07/15/05   sun     Added checks for BDN and FDN  in USIM
07/14/05   tml     Added original request type
07/12/05   tml     Lint fixes
07/11/05   sun     Fixed Memory Leaks. Added NV Item Checks around ENS applications
07/05/05   tml     Assign phase before populating global structure
06/30/05   wli     Fixed error of MMGSDI_CDMA_ECC
06/17/05   jar     Activate Onchip SIM Fix and Change to QCT Values
06/17/05   sun     Fixed GSDI Enums
06/16/05   tml     Prototype change for transparent and record access
06/15/05   jk      Read EF Phase at proper time according to Spec
03/15/05   jar     Fix to allow noncompliant Cards with Empty CDMA DFs to
                   operate with the target.
06/14/05   jar     Featurization Fix.
05/10/05   jar     Branched from MMGSDI 3.2 Branch.
                   Fix to decline number of registrations expected.
02/24/05   jk      Added 256+ MMGSDI support
06/10/05   sun     Set GSTK flag to FALSE before EF-TST is updated and set the
                   FLAG to  TRUE once update is complete
05/16/05   sun     Only Allow Perso init to go through if enough registrations
                   have not gone through.
06/03/05   pv      Report an illegal sim event when setting the sim to be an
                   illegal sim and always return GSDI_SUCCESS even for an
                   illegal sim
05/27/05   jk      Send Terminal Profile Download before EF IMSI and LOCI
                   select.  Also, allow Read and Get File Attributes for
                   Language files even if sim_present_and_ready is not TRUE
05/16/05   tml     fixed USIM file service check
05/08/05   tml     Remove GSM default channel support
05/11/05   sst     Lint fixes
05/10/05   jar     Fix for "Invalid EF_protocol" error in gsdi_get_EF_protocol()
05/05/05   sun     Added support for Perso Refresh
05/05/05   sun     Fixed Compiler Error
04/30/05   jar     Bad Featurization Fix.
                   Fix to avoid Multiple Terminal Profile Downloads
04/28/05   jar     Added New GSDI SIM Lock Architecutre to support Plug In
                   Proprietary Security Libraries driven by a Perso Engine.
04/22/05   yz      Added Consortium support for Sirius.
04/04/05   tml     Added GSM open channel supports
03/16/05   wli     Added 820B EFs
03/16/05   jk      Corrected gsdi_proc_change_pref_slot so that before
                   processing slot change, user_switch_cdma/gsm_app_pref is set
                   instead of cdma_gsm_app_pref.  This ensures proper slot pref
                   change processing
03/11/05   jar     Fixed file service mapping for USIM_KC,USIM_KCGPRS
02/28/05   sun     Added File - EF MWIS
02/28/05   sun     Added EF Mapping for Cingular
02/24/05   jar     Fixed compiler error
02/15/05    pv     Added code to re-set the UIM when currently running in USIM
                   instruction class and a check for gsm returned invalid
                   instruction class.  Also added additional parameter to
                   gsdi_uim_server_sim_reset to indicate the protocol to
                   start with.
02/14/05   sun     Added support for Cingular Wireless
02/15/05   jar     Fixed Channel Management Memory Leak.
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/05/04   jar     Added GET PBR Support.  Add Channel Management Fixes.
12/27/04   jk      Fixed compiler warnings.
12/16/04    jk     Removed GSM Orange EF references
12/04/04   jar     Fix compile for 1X/GSM DUALSLOT
12/03/04 jar/tml   Fix to keep gsdi from getting stuck in an infinite loop
                   due to missing SIM Refreshing Check lost during DS Merge.
11/29/04   tml     Fixed insert cb
11/15/04   jar     MMGSDI Merge for DUAL_SLOT
                   Code cleanup to use GSDI MACROS for MEMFREE and ERRORS
11/01/04   pv      Added code to compute sanity timer value based on the uim
                   work waiting time reported while link_established command
10/18/04   sk      Added case GSDI_GET_ECC_REQ.
                   Added gsdi_ecc_table pointer declaration.
                   Added UIM_GSM_ECC to gsdi_sim_cache.
                   Added definition for funtion gsdi_proc_get_ecc().
10/18/04   sk      Added suppport for UIM_CDMA under GSDI_SIM_SEARCH_REQ message.
10/08/04   tml     Cached CBMID
10/07/04   sk      Removed FEATURE_MMGSDI_UMTS for gsdi_app_pin_info_table initialization.
09/30/04   sk      Added support for Caching EF-ELP
09/29/04   sk      Initialized table gsdi_app_pin_info_table,added definitions
                   for functions gsdi_uim_notify_reset() and
                   gsdi_add_to_app_pin_info_table(), added a call to funtion
                   gsdi_add_to_app_pin_info_table() from function gsdi_cache_pin_status()
09/23/04   jar     Further Support of CPHS and Initialization when present.
09/04/04   jar     Orange ALS Support and fixed date.
09/03/04   tml     Added file enum table
09/02/04   jar     Added file enum table
09/01/04   tml     Ensure event is being sent out to client once only
08/20/04   tml     Refresh clean up
07/17/04 jar/tml   Added Depersonalization Checks for REFRESH
                   Added fixes to avoid infinite loop of card errors in the
                   Perso Init Procedures.
07/13/04  jar/tml  Fixed compilation errors/warnings
07/06/04   jar     Merged in Memory Leak Fix found as a result of too many
                   refreshes.
06/29/04   jar     Modified status check for gsdi_proc_perso_internal_init
                   to only check for an Error.
06/24/04 jar/tml   Handling of addition Personalization Commands
06/16/04   jar     Added params to gsdi_perso_init to support moving of
                   generation of keys into gsdi_perso_init()
06/16/04   tml     Added Language Preference caching
06/15/04   tml     Cleaned up process link establish
06/10/04   tml     Removed GSDI auto generated link establish message for RESET
                   Refresh
06/08/04   jar     Added FEATURE_MMGSDI_PERSONALIZATION Support
06/02/04   tml     Added gstk unit test trigger
05/05/04   jar     Added UIM_USIM_AD to the GSDI_USIM_CACHE.
                   Merged in code for High Priority Queue.
04/13/04   jar     Code Cleanup, Lint Fixes
04/12/04   tml     Added Unit test for Increase, Get SIM Capability, Search
                   and Select
04/07/04   jar     Removed setting of gsdi_data.gsdi_state in gsdimain() when
                   receiving a Link Established Message.
04/07/04   jar     Fixed gsdi_get_card_mode() issue of setting incorrect
                   card_mode_capability.
04/02/04   tml     Fixed disable chv1 for CDMA only card
04/01/04   jar     Fixed Disable PIN1 on RUIM Card with Invalid GSM DF
                   Subscription.
03/31/04   tml     Added Read/Write/Get File Attribute and Select Unit API Test
03/25/04   tml     Removed obsolete functions
03/27/04   tml     Added cache_ready_2
03/25/04   tml     Fix to properly set card mode during get card mode capability
                   and sends card error for card that has no valid app
03/25/04   jar     Single Slot Featurization fix.
03/25/04   jar     Fix Pointer derefrence issue.
03/25/04   jar     Fix to allow a Dual Mode Card with a Valid CDMA Subscription
                   to continue init when a GSM Subscription is invalid.
03/25/04   ck      Changed the app type from GSDI_USIM_APP to GSD_CDMA_APP
                   in the call to the function gsdi_cache_pin_status for
                   second slot.
03/23/04   jar     Compile fix, wrong file picked up.
03/23/04   jar     Avoid running REFRESH_SIM_INIT_2 multiple times.
03/23/04   jar     Removed the checks on the GSDI Callback for handling the
                   refreshing as a result the switch slot procedures.  Make sure
                   Global for Cache Ready is properly set.
03/18/04   jar     Refresh Procedure Fixes
                   Merged in Duplo SIM Fix from Branch.
03/12/04   jar     Dual Slot Support for Single Slot Initializations
                   Merged in High Priority Queue Support.
02/21/04   jar     Check for NULL Pointer card_apps in function
                   gsdi_get_apps_available().
02/04/04 jar/tml   Added support for just_rehabilitated_2 and just_invalidated_2.
02/04/04 jar/tml   Fix for setting proactive uim bit for dual slot
01/28/04   jar     Added Function gsdi_cm_in_traffic_channel_w_mode to support
                   Polling on/off for 1X RUIM.
01/26/04   tml     Featurized Link establish message for refresh reset under
                   !FEATURE_MMGSDI_DUAL_SLOT
01/25/04   tml     Dual slot refresh fixes
01/26/04   jar     Fixed Single slot issue where we get stuck in function
                   gsdi_proc_post_pin1_init() due to invalid gsdi2_status.
01/20/04   tml     Added refresh support for dual slot
01/15/04   tml     Allowed GSDI to sent the last SIM event to client if
                   client event registration is too slow
01/14/04   tml     Sent card error when the inserted card has no apps
01/14/04   jar     Added gsdidiag_init()
01/07/04   tml     Preferred slot none fix
01/05/04   jar     Vote UIM Clock off before sending out SIM Inserted event and
                   vote it back on during post pin1 init procedures.
12/20/03   tml     Added FDN support for RUIM
12/18/03  jar/tml  Allowed Get Sim capability to go to global data immediately
                   if there is no need to synch up the global data.
12/12/03   jar     Added fix to for message_header.slot to be GSDI_SLOT_1 for
                   non dual slot builds, regardless of slot number provided.
12/10/03   jar     lint
12/04/03   tml     lint
11/19/03  jar/tml  USIM ECC fix
11/05/03  jar/tml  Added Dual Slot Support for Slot Pref Changes
10/30/03   tml     Compilation fix for CDMA only target
10/28/03   jar     Forced RUN_GSM_ALGORITHM to use GSDI_SLOT_AUTOMATIC so that
                   the request is sent to the proper subscription.
10/16/03   jar     Added Send APDU API Processing
10/20/03   jar     Removed references to my_cs member.
10/16/03   jar     Fixed Compile warning in gsdi_process_link_establish().
09/18/03   jar     Changed bad casting in gsdi_proc_handle_client_req for
                   GSDI_GET_PIN_STATUS.
09/05/03   jar     GSDI GET CARD MODE Workaround.  Removed ERR_FATALS.
                   Dual Slot Code Cleanup.
09/04/03   tml     Dual slot TK support
08/25/30   jar     FEATURE_MMGSDI_DUAL_SLOT SUPPORT
08/21/03   jar     Added protection against corruption of First Inst NV Item.
08/14/03   jar     Fixed issue where UIM was told not to poll eventhough Card
                   inserted was a proactive card.
07/31/03   tml     Checked for return value from SST before accessing the
                   data
07/02/03   tml     Added messages for start and stop polling during traffic
06/22/03   jar     Create Global gsdi_crit_sec and initialize it.
06/23/03   jar     Fixed bad CDMA RUIM Check.
06/12/03   jar     Put DCS1800 DF under FEATURE_UIM_RUIM_W_GSM_ACCESS due to
                   SIM Simulator Limiation/Issue found.
06/03/03   jar     Refresh Request fix for Stack Related FCNs.  Updated
                   Function information.
06/02/03   tml     Do not enable GSM app access if the card has USIM app
05/28/03   jar     Removed Extra Break.  Fixed Compile Warning.  Ignore any
                   UIM Errors when running with Virtual SIM.
05/19/03   jar     Lint Fixes
05/17/03   jar     Added additional support for FEATURE_UIM_RUIM_W_GSM_ACCESS
05/10/03   jar     Fix Compiler Warning seen in function gsdi_register_for_fcns
                   Fix uim_set_proactive_uim linking errors for LTK.
05/01/03   jar     Added Support for REFRESH File Change Notification Handling
04/21/03   tml     add image supports
04/21/03   tml     compilation fixes and read request fixes for SAT/USAT/UTK
04/15/03   tml     Evaluated ERR usage
04/07/03   tml     Added UIM_USIM_KCGPRS to the usim cache
           jar     Fixed Feature problem which keeps GSM Post Pin1 procedures
                   from running.
04/10/03   jar     Modified Initialization procedures to use DFs as indicators
                   as to what initialization procedures are followed.
03/21/03   tml     Allowed mobile to poll when the  card is usim.
                   And turned off polling if terminal profile download fails
03/19/03   jar     Added Features around ECC Buffers.
                   Added ECC Buffer for RUIM
                   Added Caching of ECCs to ECC Buffer for RUIM
03/19/03   jar     Fixed incorrect settings for the Available Card Applications.
                   Allow GSDI to complete initialization as long as a valid
                   GSM/CDMA/USIM DF/ADF is available.
                   Removed unused declaration of variable msg.
03/18/03   jar     Added Handling for Virtual SIM when in USIM Mode.
03/14/03   jar     GSDI will notify UIM that a proactive SIM is available
                   after it notifies the GSTK.
03/13/03   jar     Added Function gsdi_get_apps_available to be used to phase
                   out gsdi_get_card_mode.
03/13/03   jar     Added FEATURE_GSTK around case for GSDI_SIM_REFRESH_REQ.
02/28/03   jar     Added FEATURE_GSTK around toolkit functions.
02/27/03   jar     Added gsdi_gstk_timer to time FCN ACKs.  Added Handling
                   for request GSDI_REFRESH_REQ.  Added Timer to avoid
                   from Hanging ME Init if GSTK Task does not ack for TP DL.
                   Added LOCAL function gsdi_file_change_notification for
                   FCNs needed with SIM_INIT_AND_FCN.  Added handling of different
                   Message Sets.  Return GSDI_NOT_SUPPORTED for REFRESH USIM/RUIM.
02/26/03   jar     Added Fix which Allows for RUIM EFs to be requested when the
                   Card Mode is set to GSDI_CARD_GSM for Dualmode Cards.
02/13/03   jar     Send GSDI_NOT_OK_FOR_TP_DL Event if Proactive Command/Sim
                   is not allocated in the Service Table (FEATURE_GSTK)
02/11/03   jar     Added a timer to gsdi_wait_for_tasks_to_register to ensure
                   GSDI doesn't hang waiting on all required tasks to register.
02/04/03   jar     Added GSTK Task Support.  Changed signals in gsdi_wait_for
                   tasks_to_register().
01/29/03   jar     Added check for Telecom EFs for RUIM to set uim_protocol to
                   UIM_CDMA in gsdi_get_EF_protocol.  Added write handling for
                   RUIM Writes.
01/24/03   jar     Added support to Cache RUIM Lang Pref and Emergency Call Codes.
01/21/03   jar     Changed FEATURE_UIM_UMTS to FEATURE_UIM_USIM in
                   gsdi_uim_error_condition to ensure call teardown.
01/16/03   jar     Added Reading and Caching of Language Preference/Indicator
                   Moved CACHE from gsdi.h to this file.  Added UIM_GSM_LP and
                   UIM_USIM_LI to CACHE.
01/07/03   jar     Fixed Featurization problem for LTK RUIM Support.
01/03/03   jar     Fixed record,array offset problem in
                   gsdi_get_card_mode_capability_usim.
01/02/03   jar     LTK RUIM Support Merge
12/17/02   jar     Changed FEATURE_MMGSDI_USIM to FEATURE_MMGSDI_UMTS
12/03/02   jar     Added workaround for 1X/GSM Initialization for Post Pin1
                   Init Procedures.  Merge 1X/GSM workaround for card_mode.
11/25/02   jar     Removed RTRE Code. Featurized gsdi_get_first_inst_class().
                   Added Feature FEATURE_MMGSDI_UMTS around gsdi_get_card_
                   mode capability_usim.  FEATURE_MMGSDI_GSM around gsdi_
                   get_card_mode capability_sim.  Fixed compile issues in
                   gsdi_get_card_mode_capability_ruim and now returns a status.
                   Added code to better handle 1X/GSM Handling in gsdi_get_
                   card_mode_capability.  Featurized use of UIM_POLL_ERR_S in
                   gsdi_uim_error_condition().  Featurized handling of CM
                   registration in gsdi_wait_for_tasks_to_register().
11/14/02   jar     Added case BY_PATH to gsdi_get_ef_protocol() to handle EFs
                   accessed by PATH.  Removed Phone book status_unknown
11/08/02   jar     Set gsdi_data.cache_ready to FALSE in gsdi_main_task_init and
                   set it to TRUE once Initialization has completed.
11/07/02   jar     Added Else Case for Historical Characters check to handle
                   case where USIM is First Inst Class when Virtual SIM is in
                   GSM Mode to allow Fallback to GSM Inst Class.
10/24/02   jar     Added handling for UIM_POLL_ERROR_S so that a CARD_ERROR
                   event can be generated and sent to all client.
10/16/02 tml/jar   Send GSDI_CARD_ERROR in case gsdi_post_pin1_init fails
                   Removed prior method of requesting card polling when
                   in traffic channel due to new method of cm calling
                   the gsdi_cm_in_traffic_channel function when in/out of
                   traffic channel.
10/14/02   jar     Wait for CM to register a callback during gsdi init in
                   function gsdi_wait_for_tasks_to_register().
                   Broke gsdi_get_card_mode_capability into 3 functions:
                   gsdi_get_card_mode_capability_sim,
                   gsdi_get_card_mode_capability_usim,
                   gsdi_get_card_mode_capability_ruim.
                   Modified gsdi_get_card_mode_capability to read from NV
                   NV_UIM_FIRST_INST_CLASS_I using the function:
                   gsdi_get_first_inst_class.
                   Added Function gsdi_get_first_inst_class to retrieve
                   NV_UIM_FIRST_INST_CLASS_I and populate gsdi_data.
10/14/02   tml     1x, gsm changes
10/08/02   tml     PIN Status cached during initialization:
                   call gsdi_cache_pin_status after receiving
                   GSDI_LINK_ESTABLISHED_MSG event, added gsdi_cache_pin_status()
09/31/02   jar     Added the function gsdi_cm_in_traffic_channel(boolean in_TC)
                   to be used by CM to notify GSDI when a Call in the Traffic
                   channel or when all calls have been released.  This
                   implementation will replace the previous method of GSDI
                   registering for CM Events.
09/24/02   tml     Added disable_chv1_allowed check before sending
                   GSDI_DISABLE_PIN_REQ to uim driver.
09/15/02   jar     Changed FEATURE_GSDI_UMTS to FEATURE_MMGSDI_UMTS
09/14/02   jar     Made changes per Run Time Enable RUIM Integration.
08/29/02  jar/ck   Added Run Time Enable RUIM Support.  Removed all ESN Usage
                   support but still set NVRUIM Items.  Added internal Feature
                   MMGSDI_RTRE.
08/21/02   jar     Added PIN Unblock Fix for RUIM.
08/12/02   tml     Add resp enum type
07/28/02   jar     Register with CM for CM_CALL_EVENT_CONNECT and
                   CM_CALL_EVENT_END events.  Added callback for cm to call
                   which calls uim_power_control.
07/24/02   tml     Added Select api
07/23/02   jar     Added a function to handle rsps from asynchronous rqsts made
                   as a result of optimization.  It will handle the response
                   from the SIM/USIM as a result of updating the SIM/USIM to
                   match the updated cached item (gsdi_handle_async_lib_rsp).
07/17/02   jar     Added a Function the function definition to handle Async
                   SIM/USIM Resp's for Asynch Write Accesses used to Synch
                   the SIM/USIM to cache (gsdi_handle_async_lib_rsp).
07/13/02   jar     Uncommented code to Vote the USIM Clock off
05/21/02   jar     Enhanced methods used to retrieve the AID from the USIM.
                   Now perform an enhanced search on part of AID, and then
                   perform a read.
05/21/02   jar     Changed FEATURE_DATA to FEATURE_DATA_ETSI_PIN to allow for
                   Data to exist without having the need to register a
                   callback during GSDI Initialization.
05/21/02   jar     Added Check to make sure length of ECC Record fits within
                   GSDI_MAX_ECC_LEN.
05/19/02   jar     Modified to only generate signals for callbacks registered
                   during init.  CBs registered with library will not need
                   to generate a signal to the GSDI.  Modified signals
                   generated and waited on for CBs registered.  Only use
                   gsdi_util_send_mmi_sim_indication in gsdimain.
05/18/02   jar     Corrected stack corruption issues.
05/17/02   jar     Commented out code for GSDI To Vote off USIM Clock after
                   initialization completes.
05/13/02   jar     Modified Loop Control for reading ECCs.
05/01/02   jar     Merged in Branched Code for GS Unification
04/30/02   jar     Added to gsdi_main_task_init to initialize phonebook related
                   data.
04/24/02   jar     Removed previous methods of power control. Now vote to stop
                   USIM Clock after GSDI Init has completed.
04/04/02   jar     Modified code in gsdimain not to execute gsdi_proc_post_pin1
                   _init for a gsdi_get_pin_status lib request after GSDI
                   Initialization has completed.
04/02/02   jar     Added gsdi_wait_for_tasks_to_register.  Moved code that
                   waits for tasks to register into this new function.
                   gsdi_insert_cb_into_cb_array will generate a new signal
                   for each task that registers.  Use gsdi_util_send_sim_
                   events_to_reg_tasks to send the event GSDI_SIM_INIT_COMPLETED.
03/24/02   jar     Modified case MS_UIM_SERVER within gsdimain() to wait for
                   a registration from UI and/or ATCOP if the proper
                   features are defined.  Modified gsdi_reg_callback
                   to support this change.  Removed Else for No Historical
                   Characters.  GSM SIMs usually don't have Bytes where
                   Historical Characters usually exist for USIMs.
03/23/02   jar     Removed ATR Dependency for determining Card Type (SIM/USIM)
                   in gsdi_get_card_mode_capability.  Removed the dependency
                   Emergency Call Codes retrieval had on
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.
03/08/02   jar     Added new Request GSDI_REGISTER_CALLBACK_REQ
                   Added functions: gsdi_insert_cb_into_cb_array,
                   gsdi_proc_handle_callback_request.  Modified
                   gsdi_reg_callback to follow multclient support structure.
                   All changes need feature define FEATURE_GSDI_MULTICLIENT_SUPPORT
02/09/02   jar     Added Retrieval of ECCs as specified in TS 31.102 5.1.1.2
                   to comply with initialization procedures.
                   Added a select of MF for TELECOM-PBR Reads.  This is to
                   handle occasional failures.
02/03/02   jar     Added functionality to allow requests for ECC data retrieved
                   during GSDI Initialization.  Added Phonebook Support
                   Functionality which is featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.  Added a case to handle
                   uim_server error condition 5.
11/30/01   jar     Featurized code using FEATURE_UI to be used for KOVU
                   Development and KOVU the Unit Testing Environment.
10/19/01   jc      Add message_p param to gsdi_util_send_gsdi_cnf calls.
10/11/01   jc      Add passing of calling task info in request message.
                   Code cleanup.
09/13/01   jc      F3 message cleanup
09/04/01   jc      Update FEATURE define for watch dog reporting.


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "rex.h"
#include "task.h"
#ifdef FEATURE_DOG
   #include "dog.h"
#endif /* FEATURE_DOG */
#include "msg.h"
#include "err.h"
#include "queue.h"
#include "dsm.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"
#ifdef FEATURE_GSTK
#include "gstk.h"
#endif /* FEATURE_GSTK */

#ifdef FEATURE_UIM
   #include "uim.h"
#endif /* FEATURE_UIM */

#include "gsdi_exp.h"
#include "gsdi.h"
#include "gsdi_nv.h"

#ifdef FEATURE_MMGSDI_CPHS
#include "gsdi_cphs.h"
#endif /* FEATURE_MMGSDI_CPHS */
#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#include "gsdi_perso_engine.h"
#else
#include "gsdi_perso.h"
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PEROSNALIZATION */

#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_session.h"
#include "mmgsdi_refresh.h"
#include "gsdi_convert.h"
#include "gsdilib.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
#ifndef FEATURE_MMGSDI_LTK
#include "flash_otp.h"
#else
#include "mmgsdiltk.h"
#endif /* FEATURE_MMGSDI_LTK */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#include "tmc.h"

#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
  #include "target.h"
  #include "queue.h"
  #include "uim.h"
  #include "nvruimi.h"
  #include "diag.h"
#endif /* FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_VIRTUAL_SIM
  #include "virtuim.h"
#endif  /* FEATURE_VIRTUAL_SIM */

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
#include "gsdi_api_tester.h"
#endif /* FEATURE_GSDI_ENABLE_API_TESTER */

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
#include "gsdi_event_tester.h"
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
#include "gsdi_chnl_mgt.h"
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
#include "gsdi_gstk_cmd_unit_tester.h"
#endif /* #ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER  */

#ifdef FEATURE_MMGSDI_CPHS
#include "gsdi_cphs.h"
#endif /* FEATURE_MMGSDI_CPHS */

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
/* Members in the table has to follow the uim.h and uim.c enum ordering */
gsdi_attr_cache gsdi_mf_cache_tbl[MMGSDI_MF_NUM_FILE_ENUM] =
{
  /* 0 */
  {UIM_ICCID,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_ELP,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_DIR, // 102.221
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_ARR, // 102.221
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_NO_SUCH_ITEM,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}}

};
#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS ) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
/* Members in the table has to follow the uim.h and uim.c enum ordering */
gsdi_attr_cache   gsdi_gsm_cache_tbl[MMGSDI_GSM_DF_NUM_FILE_ENUM] =
{
  /* 0 */
  {UIM_GSM_LP,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMSI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_KC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_PLMN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_HPLMN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ACM_MAX,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_SST,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ACM,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   TRUE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1}},
  {UIM_GSM_GID1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_GID2,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 10 */
  {UIM_GSM_SPN,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_PUCT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CBMI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_BCCH,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ACC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_FPLMN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_LOCI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_AD,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_PHASE,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_VGCS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 20 */
  {UIM_GSM_VGCSS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_VBS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_VBSS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_EMLPP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_AAEM,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CBMID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ECC,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CBMIR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_DCK,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CNL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 30 */
  {UIM_GSM_NIA,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_KCGPRS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_LOCIGPRS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_SUME,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_PLMNWACT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_OPLMNWACT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_HPLMNACT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CPBCCH,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_INVSCAN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_RPLMNAT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 40 cphs spec */
  {UIM_GSM_VMWI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_SVC_STR_TBL,  // phase 1 CPHS, unable to find in spec
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_GSM_CFF,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ONS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CSP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CPHSI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_MN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* can't find in gsm spec, appear in 51.011 Rel 4
      using usim spec value */
  {UIM_GSM_PNN,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
   /* can't find in gsm spec, appear in 51.011 Rel 4
      using usim spec value */
  {UIM_GSM_OPL,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_SAI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 50 */
  {UIM_GSM_SLL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_MEXE_ST,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ORPK,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_ARPK,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_TPRPK,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* can't find in gsm spec, appear in 51.011 Rel 4
      using usim spec value */
  {UIM_GSM_MBDN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_EXT6,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_MBI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_MWIS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
   // ext1 can't be found under gsm df, using telecom value
  {UIM_GSM_EXT1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 60 */
  {UIM_GSM_SPDI,  // spdi can't be found in gsm, using usim value
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_CFIS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* these are not present in the uim.c */
   /*
  {UIM_GSM_IMG,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG1INST1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG1INST2,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG1INST3,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG2INST1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG2INST2,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG2INST3,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG3INST1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   // 60
  {UIM_GSM_IMG3INST2,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_GSM_IMG3INST3,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
   */
};
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */


#if defined (FEATURE_MMGSDI_CDMA)
/* Members in the table has to follow the uim.h and uim.c enum ordering */
gsdi_attr_cache   gsdi_ruim_cache_tbl[MMGSDI_CDMA_DF_NUM_FILE_ENUM] =
{
  /* 0 */
  {UIM_CDMA_CC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   TRUE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1}},
  {UIM_CDMA_IMSI_M,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_IMSI_T,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_TMSI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ANALOG_HOME_SID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ANALOG_OP_PARAMS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ANALOG_LOCN_AND_REGN_IND,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_HOME_SID_NID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ZONE_BASED_REGN_IND,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SYS_REGN_IND,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 10 */
  {UIM_CDMA_DIST_BASED_REGN_IND,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ACCOLC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_CALL_TERM_MODE_PREF,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SCI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ANALOG_CHAN_PREF,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_PRL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_RUIM_ID,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_CDMA_SVC_TABLE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SPC,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_OTAPA_SPC_ENABLE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 20 */
  {UIM_CDMA_NAM_LOCK,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_OTASP_OTAPA_FEATURES,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SERVICE_PREF,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ESN_ME,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_RUIM_PHASE,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER,  MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_PREF_LANG,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_APP1_PIN1,  MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_UNASSIGNED_1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_SMS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SMS_PARAMS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SMS_STATUS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 30 */
  {UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_UNASSIGNED_2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_HOME_SVC_PVDR_NAME,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_UIM_ID_USAGE_IND,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ADM_DATA,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MSISDN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MAXIMUM_PRL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER,  MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SPC_STATUS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ECC,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_3GPD_ME3GPDOPC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 40 */
  {UIM_CDMA_3GPD_3GPDOPM,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_SIPCAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER,  MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_MIPCAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_SIPUPP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_MIPUPP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_SIPSP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_MIPSP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_SIPPAPSS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_UNASSIGNED_3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_UNASSIGNED_4,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 50 */
  {UIM_CDMA_PUZL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER,  MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MAXPUZL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MECRP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_HRPDCAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_HRPDUPP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_CSSPR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_ATC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_EPRL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BCSMS_CONFIG,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BCSMS_PREF,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 60 */
  {UIM_CDMA_BCSMS_TABLE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BCSMS_PARAMS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MMS_NOTIF,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MMS_EXT8,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MMS_ICP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MMS_UP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_SMS_CAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_IPV6CAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_MIPFLAGS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_TCPCONFIG,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 70 */
  {UIM_CDMA_3GPD_DGC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BROWSER_CP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BROWSER_BM,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_SIPUPPEXT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MMS_CONFIG,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_JAVA_DURL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_3GPD_MIPUPPEXT,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BREW_DLOAD,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BREW_TSID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BREW_SID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 80 */
  {UIM_CDMA_LBS_XCONFIG,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_LBS_XSURL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_LBS_V2CONFIG,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_LBS_V2PDEADDR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_LBS_V2MPCADDR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_BREW_AEP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_MODEL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_RC,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_CDMA_APP_LABELS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  /* KDDI Related */
  {UIM_CDMA_USER_AGENT_STRING,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 90 */
  {UIM_CDMA_GID2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_GID1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_GID_REFERENCES,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_IMSI_STATUS,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_ME_DOWNLOADABLE_DATA,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_ME_SETTING_DATA,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_ME_USER_DATA,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_RESERVED2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_RESERVED1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_UIM_SVC_TABLE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 100 */
  {UIM_CDMA_UIM_FEATURES,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_UIM_VERSION,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_HRPD_HRPDUPP,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_CDMA_SF_EUIM_ID,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}}
};
#endif
/* Members in the table has to follow the uim.h and uim.c enum ordering */
gsdi_attr_cache   gsdi_telecom_cache_tbl[MMGSDI_TELECOM_DF_NUM_FILE_ENUM] =
{
  {UIM_TELECOM_ADN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_FDN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN2, MMGSDI_PIN_STATUS_APP1_PIN2},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_SMS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_CCP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_ECCP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_MSISDN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_SMSP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_SMSS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_LND,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   TRUE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_SDN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 10 */
  {UIM_TELECOM_EXT1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_EXT2,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN2, MMGSDI_PIN_STATUS_APP1_PIN2},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_EXT3,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BDN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN2, MMGSDI_PIN_STATUS_APP1_PIN2},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_EXT4,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN2, MMGSDI_PIN_STATUS_APP1_PIN2},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_SMSR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_CMI,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  /* usim sume in telecom ef define in 102.222, unable
     to find it */
  {UIM_TELECOM_SUME,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_ARR, // can't find ARR under MF, using ARR under USIM ADF
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_IMAGE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  /* 20 */
   /*  PBR, CCP1, UID, PSC, CC, PUID,
       can't find in gsm spec, using usim value */
  {UIM_TELECOM_PBR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_PSC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_CC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_PUID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 26 */
  {UIM_NO_SUCH_ITEM,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_NO_SUCH_ITEM,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_NO_SUCH_ITEM,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_NO_SUCH_ITEM,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}}

};

/* Members in the table has to follow the uim.h and uim.c enum ordering */
gsdi_attr_cache   gsdi_ef_cache_tbl[MMGSDI_EF_BY_PATH_NUM_FILE_ENUM] =
{
  /* 0 */
  {UIM_EF_BY_PATH,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_IAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_PBC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_GRP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_AAS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_GAS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_ANR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_SNE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_EMAIL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
   // unknown... spec does not have specific access condition
   // listed for these files from ANRA to IAP1
  {UIM_TELECOM_BY_PATH_ANRA,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 10 */
  {UIM_TELECOM_BY_PATH_ANRB,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRC,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANR1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRA1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRB1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRC1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ADN1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_PBC1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_GRP1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_SNE1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 20 */
  {UIM_TELECOM_BY_PATH_UID1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_EMAIL1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_IAP1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ADN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_EXT1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_EXT2,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN2, MMGSDI_PIN_STATUS_APP1_PIN2},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
   /* gsm telecom uid/ccp1 and usim uid/ccp1 has diff write */
  {UIM_TELECOM_BY_PATH_UID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_TELECOM_BY_PATH_CCP1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 1000 PBM Support starts here */
  {UIM_TELECOM_BY_PATH_ADN2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_IAP2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 30 */
  {UIM_TELECOM_BY_PATH_PBC2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_GRP2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_SNE2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_UID2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_EMAIL2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANR2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRA2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRB2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRC2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ADN3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 40 */
  {UIM_TELECOM_BY_PATH_IAP3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_PBC3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_GRP3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_SNE3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_UID3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_EMAIL3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANR3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRA3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRB3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_TELECOM_BY_PATH_ANRC3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  /* 1000 USIM Phonebook ends at UIM_TELECOM_BY_PATH_ANRC3 */

  /* 50 */
  {UIM_USIM_ADN,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_IAP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_PBC,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_EXT1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_GRP,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_AAS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_AAS1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_GAS,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_GAS1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANR,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

   /* 60 */
  {UIM_USIM_SNE,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_EMAIL,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
   /* unknown, similar to telecom_by_path */
  {UIM_USIM_ANRA,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRB,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRC,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANR1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRA1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRB1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRC1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ADN1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

   /* 70 */
  {UIM_USIM_PBC1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_GRP1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_SNE1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_UID1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_EMAIL1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_IAP1,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

 /* can't find act_hplmn in spec, there is one for cingular found
    in uim.c... */
  {UIM_USIM_ACT_HPLMN,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_RAT,
   {MMGSDI_PIN_STATUS_ALWAYS, MMGSDI_PIN_STATUS_ALWAYS},
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_UID,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},
  {UIM_USIM_CCP1,
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   {MMGSDI_PIN_STATUS_APP1_PIN1, MMGSDI_PIN_STATUS_APP1_PIN1},
   FALSE,
   {MMGSDI_PIN_STATUS_NEVER, MMGSDI_PIN_STATUS_NEVER}},

  /* 80 */
  /* USIM 1000 support starts here */
  {UIM_USIM_ANR2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  {UIM_USIM_ANRA2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRB2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRC2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ADN2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_PBC2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_GRP2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_SNE2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_UID2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_EMAIL2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 90 */
  {UIM_USIM_IAP2,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANR3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRA3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRB3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ANRC3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_ADN3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_PBC3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_GRP3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_SNE3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_UID3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},

  /* 100 */
  {UIM_USIM_EMAIL3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_USIM_IAP3,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  /* 1000 PBM support ends at UIM_USIM_IAP3 */
  {UIM_PKCS15_PRKDF,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_PKCS15_PUKDF,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_PKCS15_SKDF,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_PKCS15_CDF,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_PKCS15_DODF,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}},
  {UIM_PKCS15_AODF,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN},
   FALSE,
   {MMGSDI_PIN_STATUS_UNKNOWN, MMGSDI_PIN_STATUS_UNKNOWN}}
};
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
mmgsdi_uim_report_rsp_type *mmgsdi_internal_synch_uim_rpt_rsp;


#ifdef FEATURE_MMGSDI_PERSONALIZATION
static void gsdi_perform_post_pin1_perso_refresh_if_needed(
  gsdi_slot_id_type     slot,
  uint8                *data_ptr
);
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
static gsdi_returns_T gsdi_post_pin1_init_perso(
  gsdi_slot_id_type slot,
  int32           * len_p,
  uint8           * data_p
);
#endif  /* FEATURE_MMGSDI_PERSONALIZATION*/

static gsdi_returns_T gsdi_hzi_init
(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PROC_POST_PIN1_INIT

DESCRIPTION
  Based on the mobiles capability, call the appropriate initialization
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T mmgsdi_proc_post_pin1_init(
  gsdi_slot_id_type slot,
  int32             *returned_data_len,
  uint8             *data_ext_p
);

/*===========================================================================
FUNCTION GSDI_CDMA_SVC_NVRUIM_INIT

DESCRIPTION
  This function initiates the control flags in nvruim

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdi_cdma_svc_nvruim_init (
  gsdi_slot_id_type slot
);

#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS or FEATURE_MMGSDI_GSM */


static mmgsdi_data_type        *mmgsdi_init_data;

/* Queue to hold high priority items for GSDI to Process */
q_type gsdi_high_pri_cmd_q;

/* Queue to hold commands for gsdi */
q_type gsdi_task_cmd_q;

/* Global structure to general info for gsdi task */
gsdi_data_T gsdi_data;

gsdi_resp_type  gsdi_resp;  /* store the response type for gsdi cmd */

rex_timer_type gsdi_int_timer;

rex_timer_type gsdi_gstk_timer;

/* GSDI UIM Interface Sanity Timer */
rex_timer_type gsdi_uim_sanity_timer;


#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
/* Test SIM Reads Timer Expire */
rex_timer_type gsdi_read_api_test_timer;

/* Test SIM Write Timer Expire */
rex_timer_type gsdi_write_api_test_timer;

/* Test Get PIN Status Timer Exipre */
rex_timer_type gsdi_gps_api_test_timer;

/* Test SIM Get File Attributes Timer Expire */
rex_timer_type gsdi_gfas_api_test_timer;

/* Test SIM Get SIM Capabilities Timer Exipire */
rex_timer_type gsdi_gscs_api_test_timer;

/* Test SIM Select Timer Exipire */
rex_timer_type gsdi_select_api_test_timer;

/* Test SIM Increase Timer Exipire */
rex_timer_type gsdi_increase_api_test_timer;

/* Test SIM Search Timer Exipire */
rex_timer_type gsdi_search_api_test_timer;

/* Test SIM Open Channel */
rex_timer_type gsdi_open_channel_api_test_timer;

/* Test SIM Reads subsequent timer expire */
rex_timer_type gsdi_read_api_next_test_timer;

/* Test SIM Writes subsequent timer expire */
rex_timer_type gsdi_write_api_next_test_timer;

/* Test SIM Get PIN Status subsequent timer expire */
rex_timer_type gsdi_pin_api_next_test_timer;

/* Test SIM Get File Attribute subsequent timer expire */
rex_timer_type gsdi_file_attr_api_next_test_timer;

/* Test SIM Select subsequent timer expire */
rex_timer_type gsdi_select_api_next_test_timer;

/* Test SIM get sim capability subsequent timer expire */
rex_timer_type gsdi_sim_cap_api_next_test_timer;

/* Test SIM increase subsequent timer expire */
rex_timer_type gsdi_increase_api_next_test_timer;

/* Test SIM search subsequent timer expire */
rex_timer_type gsdi_search_api_next_test_timer;

#ifdef FEATURE_GSTK
rex_timer_type gsdi_gstk_network_resel_access_env_test_timer;
#endif

rex_timer_type gsdi_open_channel_api_next_test_timer;

rex_timer_type gsdi_stream_apdu_api_next_test_timer;
#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
rex_timer_type gsdi_event_test_kick_off_timer;
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
rex_timer_type gsdi_gstk_cmd_unit_tester_timer;
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */

/* Buffers for Emergency Call Codes */
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
gsdi_ef_ecc_data_T ruim_ecc_data;
#endif /* FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
gsdi_ef_ecc_data_T ruim2_ecc_data;
#endif /* FEATURE_MMGSDI_CDMA */
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

/* ECC table pointer */
gsdi_ecc_info_table_type *gsdi_ecc_table_p;

#ifdef FEATURE_UIM_HOT_SWAP
boolean gsdi_uim_card_inserted = FALSE;
#endif /* FEATURE_UIM_HOT_SWAP */


word path_array_from_lkup_table[GSDI_LKUP_TABLE_LEN];

gsdi_file_enum_table_type gsdi_file_enum_table[GSDI_FILE_ENUM_TABLE_SIZE] =
{
  /* 0 */
  /* EFs at the MF level */
  {UIM_ICCID,                            GSDI_ICCID},                            /* ICCID */
  {UIM_ELP,                              GSDI_ELP},                              /* Extended Language Preference */
  {UIM_DIR,                              GSDI_DIR},                              /* EF DIR for USIM */
  {UIM_ARR,                              GSDI_ARR},                              /* Access rule reference */
  /* EFs in CDMA DF*/
  {UIM_CDMA_CC,                          GSDI_CDMA_CC},                          /* Call Count */
  {UIM_CDMA_IMSI_M,                      GSDI_CDMA_IMSI_M},                      /* IMSI_M */
  {UIM_CDMA_IMSI_T,                      GSDI_CDMA_IMSI_T},                      /* IMSI_T */
  {UIM_CDMA_TMSI,                        GSDI_CDMA_TMSI},                        /* TMSI */
  {UIM_CDMA_ANALOG_HOME_SID,             GSDI_CDMA_ANALOG_HOME_SID},             /* Analog Home SID */
  {UIM_CDMA_ANALOG_OP_PARAMS,            GSDI_CDMA_ANALOG_OP_PARAMS},            /* Analog Operational Parameters */

  /* 10 */
  {UIM_CDMA_ANALOG_LOCN_AND_REGN_IND,    GSDI_CDMA_ANALOG_LOCN_AND_REGN_IND},    /* Analog Locn and Regn Indicators */
  {UIM_CDMA_HOME_SID_NID,                GSDI_CDMA_HOME_SID_NID},                /* CDMA Home SID and NID */
  {UIM_CDMA_ZONE_BASED_REGN_IND,         GSDI_CDMA_ZONE_BASED_REGN_IND},         /* CDMA Zone Based Regn Indicators */
  {UIM_CDMA_SYS_REGN_IND,                GSDI_CDMA_SYS_REGN_IND},                /* CDMA System/Network Regn Indicators */
  {UIM_CDMA_DIST_BASED_REGN_IND,         GSDI_CDMA_DIST_BASED_REGN_IND},         /* CDMA Distance Based Regn Indicators */
  {UIM_CDMA_ACCOLC,                      GSDI_CDMA_ACCOLC},                      /* Access Overload Class */
  {UIM_CDMA_CALL_TERM_MODE_PREF,         GSDI_CDMA_CALL_TERM_MODE_PREF},         /* Call Termination Mode Preferences */
  {UIM_CDMA_SCI,                         GSDI_CDMA_SCI},                         /* Suggested Slot Cycle Index */
  {UIM_CDMA_ANALOG_CHAN_PREF,            GSDI_CDMA_ANALOG_CHAN_PREF},            /* Analog Channel Preferences */
  {UIM_CDMA_PRL,                         GSDI_CDMA_PRL},                         /* Preferred Roaming List */

   /* 20 */
  {UIM_CDMA_RUIM_ID,                     GSDI_CDMA_RUIM_ID},                     /* Removable UIM ID */
  {UIM_CDMA_CDMA_SVC_TABLE,              GSDI_CDMA_CDMA_SVC_TABLE},              /* CDMA Service Table */
  {UIM_CDMA_SPC,                         GSDI_CDMA_SPC},                         /* Service Programming Code */
  {UIM_CDMA_OTAPA_SPC_ENABLE,            GSDI_CDMA_OTAPA_SPC_ENABLE},            /* OTAPA/SPC-Enable */
  {UIM_CDMA_NAM_LOCK,                    GSDI_CDMA_NAM_LOCK},                    /* NAM-Lock */
  {UIM_CDMA_OTASP_OTAPA_FEATURES,        GSDI_CDMA_OTASP_OTAPA_FEATURES},        /* OTASP-OTAPA Parameters */
  {UIM_CDMA_SERVICE_PREF,                GSDI_CDMA_SERVICE_PREF},                /* Service Preferences */
  {UIM_CDMA_ESN_ME,                      GSDI_CDMA_ESN_ME},                      /* ESN_ME */
  {UIM_CDMA_RUIM_PHASE,                  GSDI_CDMA_RUIM_PHASE},                  /* R-UIM Phase */
  {UIM_CDMA_PREF_LANG,                   GSDI_CDMA_PREF_LANG},                   /* Preferred Languages */

  /* 30 */
  {UIM_CDMA_UNASSIGNED_1,                GSDI_CDMA_UNASSIGNED_1},                /* Not assigned yet */
  {UIM_CDMA_SMS,                         GSDI_CDMA_SMS},                         /* Short Messages */
  {UIM_CDMA_SMS_PARAMS,                  GSDI_CDMA_SMS_PARAMS},                  /* Short Message Service Parameters */
  {UIM_CDMA_SMS_STATUS,                  GSDI_CDMA_SMS_STATUS},                  /* SMS Status */
  {UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, GSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE}, /* Sup Services Feature Code Table */
  {UIM_CDMA_UNASSIGNED_2,                GSDI_CDMA_UNASSIGNED_2},                /* Not assigned yet */
  {UIM_CDMA_HOME_SVC_PVDR_NAME,          GSDI_CDMA_HOME_SVC_PVDR_NAME},          /* CDMA Home Service Provider Name */
  {UIM_CDMA_UIM_ID_USAGE_IND,            GSDI_CDMA_UIM_ID_USAGE_IND},            /* R-UIM ID usage indicator */
  {UIM_CDMA_ADM_DATA,                    GSDI_CDMA_ADM_DATA},                    /* CDMA Administrative Data */
  {UIM_CDMA_MSISDN,                      GSDI_CDMA_MSISDN},                      /* Mobile Directory Number */

  /* 40 */
  {UIM_CDMA_MAXIMUM_PRL,                 GSDI_CDMA_MAXIMUM_PRL},                 /* Max size of the PRL */
  {UIM_CDMA_SPC_STATUS,                  GSDI_CDMA_SPC_STATUS},                  /* SPC Status */
  {UIM_CDMA_ECC,                         GSDI_CDMA_ECC},                         /* Emergency Call Codes */
  {UIM_CDMA_3GPD_ME3GPDOPC,              GSDI_CDMA_3GPD_ME3GPDOPC},              /* Operational Capabilities */
  {UIM_CDMA_3GPD_3GPDOPM,                GSDI_CDMA_3GPD_3GPDOPM},                /* Operational Mode */
  {UIM_CDMA_3GPD_SIPCAP,                 GSDI_CDMA_3GPD_SIPCAP},                 /* Simple IP Capabilities */
  {UIM_CDMA_3GPD_MIPCAP,                 GSDI_CDMA_3GPD_MIPCAP},                 /* Mobile IP Capabilities */
  {UIM_CDMA_3GPD_SIPUPP,                 GSDI_CDMA_3GPD_SIPUPP},                 /* Simple IP User Profile Parameters */
  {UIM_CDMA_3GPD_MIPUPP,                 GSDI_CDMA_3GPD_MIPUPP},                 /* Mobile IP User Profile Parameters */
  {UIM_CDMA_3GPD_SIPSP,                  GSDI_CDMA_3GPD_SIPSP},                  /* Simple IP Status Parameters */

  /* 50 */
  {UIM_CDMA_3GPD_MIPSP,                  GSDI_CDMA_3GPD_MIPSP},                  /* Mobile IP Status Parameters */
  {UIM_CDMA_3GPD_SIPPAPSS,               GSDI_CDMA_3GPD_SIPPAPSS},               /* Simple IP PAP SS Parameters */
  {UIM_CDMA_UNASSIGNED_3,                GSDI_CDMA_UNASSIGNED_3},                /* Reserved */
  {UIM_CDMA_UNASSIGNED_4,                GSDI_CDMA_UNASSIGNED_4},                /* Reserved */
  {UIM_CDMA_PUZL,                        GSDI_CDMA_PUZL},                        /* Preffered User Zone List */
  {UIM_CDMA_MAXPUZL,                     GSDI_CDMA_MAXPUZL},                     /* Maximum PUZL */
  {UIM_CDMA_MECRP,                       GSDI_CDMA_MECRP},                       /* ME-specific Config REquest Param */
  {UIM_CDMA_HRPDCAP,                     GSDI_CDMA_HRPDCAP},                     /* HRPD Access Auth Capability Param */
  {UIM_CDMA_HRPDUPP,                     GSDI_CDMA_HRPDUPP},                     /* HRPD Access Auth User Profile Param */
  {UIM_CDMA_CSSPR,                       GSDI_CDMA_CSSPR},                       /* CUR_SSPR_P_REV */

  /* 60 */
  {UIM_CDMA_ATC,                         GSDI_CDMA_ATC},                         /* Acceess Terminal Class */
  {UIM_CDMA_EPRL,                        GSDI_CDMA_EPRL},                        /* Extended Preffered Roaming List */
  {UIM_CDMA_BCSMS_CONFIG,                GSDI_CDMA_BCSMS_CONFIG},                /* Broadcast SMS Configuration */
  {UIM_CDMA_BCSMS_PREF,                  GSDI_CDMA_BCSMS_PREF},                  /* Broadcast SMS Preferences */
  {UIM_CDMA_BCSMS_TABLE,                 GSDI_CDMA_BCSMS_TABLE},                 /* Broadcast SMS Table */
  {UIM_CDMA_BCSMS_PARAMS,                GSDI_CDMA_BCSMS_PARAMS},                /* Broadcast SMS Paramaters */
  {UIM_CDMA_MMS_NOTIF,                   GSDI_CDMA_MMS_NOTIF},                   /* MMS Notification */
  {UIM_CDMA_MMS_EXT8,                    GSDI_CDMA_MMS_EXT8},                    /* MMS Extension 8 */
  {UIM_CDMA_MMS_ICP,                     GSDI_CDMA_MMS_ICP},                     /* MMS Issuer Connectivity Parameters */
  {UIM_CDMA_MMS_UP,                      GSDI_CDMA_MMS_UP},                      /* MMS User Preferences */

  /* 70 */
  {UIM_CDMA_SMS_CAP,                     GSDI_CDMA_SMS_CAP},                     /* SMS Capabilities */
  {UIM_CDMA_3GPD_IPV6CAP,                GSDI_CDMA_3GPD_IPV6CAP},                /* IPv6 Capabilities */
  {UIM_CDMA_3GPD_MIPFLAGS,               GSDI_CDMA_3GPD_MIPFLAGS},               /* Mobile IP Flags */
  {UIM_CDMA_3GPD_TCPCONFIG,              GSDI_CDMA_3GPD_TCPCONFIG},              /* TCP Configurations */
  {UIM_CDMA_3GPD_DGC,                    GSDI_CDMA_3GPD_DGC},                    /* Data Generic Configurations */
  {UIM_CDMA_BROWSER_CP,                  GSDI_CDMA_BROWSER_CP},                  /* Browser Connectivity Parameters */
  {UIM_CDMA_BROWSER_BM,                  GSDI_CDMA_BROWSER_BM},                  /* Browser Bookmarks */
  {UIM_CDMA_3GPD_SIPUPPEXT,              GSDI_CDMA_3GPD_SIPUPPEXT},              /* Simple IP User Profile Parameters Extension */
  {UIM_CDMA_MMS_CONFIG,                  GSDI_CDMA_MMS_CONFIG},                  /* MMS Configuration */
  {UIM_CDMA_JAVA_DURL,                   GSDI_CDMA_JAVA_DURL},                   /* Java Download URL */

  /* 80 */
  {UIM_CDMA_3GPD_MIPUPPEXT,              GSDI_CDMA_3GPD_MIPUPPEXT},              /* Mobile IP User Profile Parameters Extension */
  {UIM_CDMA_BREW_DLOAD,                  GSDI_CDMA_BREW_DLOAD},                  /* BREW Download */
  {UIM_CDMA_BREW_TSID,                   GSDI_CDMA_BREW_TSID},                   /* BREW Teleservice ID */
  {UIM_CDMA_BREW_SID,                    GSDI_CDMA_BREW_SID},                    /* BREW Subscriber ID */
  {UIM_CDMA_LBS_XCONFIG,                 GSDI_CDMA_LBS_XCONFIG},                 /* LBS XTRA Configuration */
  {UIM_CDMA_LBS_XSURL,                   GSDI_CDMA_LBS_XSURL},                   /* LBS XTRA Server URLs */
  {UIM_CDMA_LBS_V2CONFIG,                GSDI_CDMA_LBS_V2CONFIG},                /* LBS V2 Configuration */
  {UIM_CDMA_LBS_V2PDEADDR,               GSDI_CDMA_LBS_V2PDEADDR},               /* LBS V2 PDE Address */
  {UIM_CDMA_LBS_V2MPCADDR,               GSDI_CDMA_LBS_V2MPCADDR},               /* LBS V2 MPC Address */
  {UIM_CDMA_BREW_AEP,                    GSDI_CDMA_BREW_AEP},                    /* BREW Application Exec. Policy */

  /* 90 */
  {UIM_CDMA_MODEL,                       GSDI_CDMA_MODEL},                       /* Model Info */
  {UIM_CDMA_RC,                          GSDI_CDMA_RC},                          /* Root Certificate */
  {UIM_CDMA_APP_LABELS,                  GSDI_CDMA_APP_LABELS},                  /* Application Labels*/
  {UIM_CDMA_USER_AGENT_STRING,           GSDI_CDMA_USER_AGENT_STRING},           /* KDDI Agent String */
  {UIM_CDMA_GID2,                        GSDI_CDMA_GID2},                        /* KDDI GID2 */
  {UIM_CDMA_GID1,                        GSDI_CDMA_GID1},                        /* KDDI GID1 */
  {UIM_CDMA_GID_REFERENCES,              GSDI_CDMA_GID_REFERENCES},              /* KDDI GID References */
  {UIM_CDMA_IMSI_STATUS,                 GSDI_CDMA_IMSI_STATUS},                 /* KDDI IMSI Status */
  {UIM_CDMA_ME_DOWNLOADABLE_DATA,        GSDI_CDMA_ME_DOWNLOADABLE_DATA},        /* KDDI ME Downloadable Data */
  {UIM_CDMA_ME_SETTING_DATA,             GSDI_CDMA_ME_SETTING_DATA},             /* KDDI ME Setting Date */

  /* 100 */
  {UIM_CDMA_ME_USER_DATA,                GSDI_CDMA_ME_USER_DATA},                /* KDDI User Data */
  {UIM_CDMA_RESERVED2,                   GSDI_CDMA_RESERVED2},                   /* KDDI Reserved */
  {UIM_CDMA_RESERVED1,                   GSDI_CDMA_RESERVED1},                   /* KDDI Reserved */
  {UIM_CDMA_UIM_SVC_TABLE,               GSDI_CDMA_UIM_SVC_TABLE},               /* KDDI UIM Service Table */
  {UIM_CDMA_UIM_FEATURES,                GSDI_CDMA_UIM_FEATURES},                /* KDDI UIM Features */
  {UIM_CDMA_UIM_VERSION,                 GSDI_CDMA_UIM_VERSION},                 /* KDDI UIM Version */
  {UIM_CDMA_HRPD_HRPDUPP,                GSDI_CDMA_HRPD_HRPDUPP},                /* HRPD User Profile Parameters */
  {UIM_CDMA_SF_EUIM_ID,                  GSDI_CDMA_SF_EUIM_ID},                  /* Short Form EUIMID */
  /* EFs in DF GSM */
  {UIM_GSM_LP,                           GSDI_GSM_LP},                           /* Language Preference */
  {UIM_GSM_IMSI,                         GSDI_GSM_IMSI},                         /* IMSI */

  /* 110 */
  {UIM_GSM_KC,                           GSDI_GSM_KC},                           /* Ciphering Key Kc */
  {UIM_GSM_PLMN,                         GSDI_GSM_PLMN},                         /* PLMN selector */
  {UIM_GSM_HPLMN,                        GSDI_GSM_HPLMN},                        /* HPLMN search period */
  {UIM_GSM_ACM_MAX,                      GSDI_GSM_ACM_MAX},                      /* ACM Maximum value */
  {UIM_GSM_SST,                          GSDI_GSM_SST},                          /* SIM Service table */
  {UIM_GSM_ACM,                          GSDI_GSM_ACM},                          /* Accumulated call meter */
  {UIM_GSM_GID1,                         GSDI_GSM_GID1},                         /* Group Identifier Level 1 */
  {UIM_GSM_GID2,                         GSDI_GSM_GID2},                         /* Group Identifier Level 2 */
  {UIM_GSM_SPN,                          GSDI_GSM_SPN},                          /* Service Provider Name */
  {UIM_GSM_PUCT,                         GSDI_GSM_PUCT},                         /* Price Per Unit and currency table */

  /* 120 */
  {UIM_GSM_CBMI,                         GSDI_GSM_CBMI},                         /* Cell broadcast message identifier selection */
  {UIM_GSM_BCCH,                         GSDI_GSM_BCCH},                         /* Broadcast control channels */
  {UIM_GSM_ACC,                          GSDI_GSM_ACC},                          /* Access control class */
  {UIM_GSM_FPLMN,                        GSDI_GSM_FPLMN},                        /* Forbidden PLMNs */
  {UIM_GSM_LOCI,                         GSDI_GSM_LOCI},                         /* Location information */
  {UIM_GSM_AD,                           GSDI_GSM_AD},                           /* Administrative data */
  {UIM_GSM_PHASE,                        GSDI_GSM_PHASE},                        /* Phase identification */
  {UIM_GSM_VGCS,                         GSDI_GSM_VGCS},                         /* Voice Group Call service */
  {UIM_GSM_VGCSS,                        GSDI_GSM_VGCSS},                        /* Voice Group Call service status */
  {UIM_GSM_VBS,                          GSDI_GSM_VBS},                          /* Voice Broadcast service  */

  /* 130 */
  {UIM_GSM_VBSS,                         GSDI_GSM_VBSS},                         /* Voice Broadcast service status */
  {UIM_GSM_EMLPP,                        GSDI_GSM_EMLPP},                        /* Enhanced multi level pre-emption and priority */
  {UIM_GSM_AAEM,                         GSDI_GSM_AAEM},                         /* Automatic Answer for eMLPP service */
  {UIM_GSM_CBMID,                        GSDI_GSM_CBMID},                        /* Cell Broadcast Message id for data dload */
  {UIM_GSM_ECC,                          GSDI_GSM_ECC},                          /* Emergency Call Codes */
  {UIM_GSM_CBMIR,                        GSDI_GSM_CBMIR},                        /* Cell Broadcast Message id range selection */
  {UIM_GSM_DCK,                          GSDI_GSM_DCK},                          /* De-personalization control keys */
  {UIM_GSM_CNL,                          GSDI_GSM_CNL},                          /* Co-operative network list */
  {UIM_GSM_NIA,                          GSDI_GSM_NIA},                          /* Network's indication of alerting */
  {UIM_GSM_KCGPRS,                       GSDI_GSM_KCGPRS},                       /* GPRS ciphering key */

  /* 140 */
  {UIM_GSM_LOCIGPRS,                     GSDI_GSM_LOCIGPRS},                     /* GPRS location information */
  {UIM_GSM_SUME,                         GSDI_GSM_SUME},                         /* Setup Menu elements */
  {UIM_GSM_PLMNWACT,                     GSDI_GSM_PLMNWACT},                     /* PLMN Selector with Access technology */
  {UIM_GSM_OPLMNWACT,                    GSDI_GSM_OPLMNWACT},                    /* Operator controlled PLMNWACT */
  {UIM_GSM_HPLMNACT,                     GSDI_GSM_HPLMNACT},                     /* HPLNMN Access technology */
  {UIM_GSM_CPBCCH,                       GSDI_GSM_CPBCCH},                       /* CPBCCH information */
  {UIM_GSM_INVSCAN,                      GSDI_GSM_INVSCAN},                      /* Investigation PLMN Scan */
  {UIM_GSM_RPLMNAT,                      GSDI_GSM_RPLMNAT},                      /* RPLMN  Last used Access Technology */
  {UIM_GSM_VMWI,                         GSDI_GSM_VMWI},                         /* CPHS: Voice Mail Waiting Indicator */
  {UIM_GSM_SVC_STR_TBL,                  GSDI_GSM_SVC_STR_TBL},                  /* CPHS: Service String Table */

  /* 150 */
  {UIM_GSM_CFF,                          GSDI_GSM_CFF},                          /* CPHS: Call Forwarding Flag */
  {UIM_GSM_ONS,                          GSDI_GSM_ONS},                          /* CPHS: Operator Name String */
  {UIM_GSM_CSP,                          GSDI_GSM_CSP},                          /* CPHS: Customer Service Profile */
  {UIM_GSM_CPHSI,                        GSDI_GSM_CPHSI},                        /* CPHS: CPHS Information */
  {UIM_GSM_MN,                           GSDI_GSM_MN},                           /* CPHS: Mailbox Number */
  {UIM_GSM_PNN,                          GSDI_GSM_PNN},
  {UIM_GSM_OPL,                          GSDI_GSM_OPL},
  {UIM_GSM_CFIS,                         GSDI_GSM_CFIS},
  /* EFs at SoLSA */
  {UIM_GSM_SAI,                          GSDI_GSM_SAI},                          /* SoLSA access indicator */
  {UIM_GSM_SLL,                          GSDI_GSM_SLL},                          /* SoLSA LSA list */

  /* 160 */
  /* EFs at MExE Level */
  {UIM_GSM_MEXE_ST,                      GSDI_GSM_MEXE_ST},                      /* MExE Service table */
  {UIM_GSM_ORPK,                         GSDI_GSM_ORPK},                         /* Operator Root Public Key */
  {UIM_GSM_ARPK,                         GSDI_GSM_ARPK},                         /* Administrator Root Public Key */
  {UIM_GSM_TPRPK,                        GSDI_GSM_TPRPK},                        /* Third party Root public key */
  {UIM_GSM_MBDN,                         GSDI_GSM_MBDN},                         /* Mail Box Dialing Number*/
  {UIM_GSM_EXT6,                         GSDI_GSM_EXT6},                          /* Extension 6*/
  {UIM_GSM_MBI,                          GSDI_GSM_MBI},                          /* Mail Box Identifier*/
  {UIM_GSM_EXT1,                         GSDI_GSM_EXT1},                         /* Extension 1*/
  {UIM_GSM_MWIS,                         GSDI_GSM_MWIS},                         /* Message Waiting Indication Status*/
  {UIM_GSM_SPDI,                         GSDI_GSM_SPDI},                         /* Service Provider Display Information*/

  /* 170 */
  /* image file */
  {UIM_GSM_IMG,                          GSDI_GSM_IMG},
  {UIM_GSM_IMG1INST1,                    GSDI_GSM_IMG1INST1},
  {UIM_GSM_IMG1INST2,                    GSDI_GSM_IMG1INST2},
  {UIM_GSM_IMG1INST3,                    GSDI_GSM_IMG1INST3},
  {UIM_GSM_IMG2INST1,                    GSDI_GSM_IMG2INST1},
  {UIM_GSM_IMG2INST2,                    GSDI_GSM_IMG2INST2},
  {UIM_GSM_IMG2INST3,                    GSDI_GSM_IMG2INST3},
  {UIM_GSM_IMG3INST1,                    GSDI_GSM_IMG1INST1},
  {UIM_GSM_IMG3INST2,                    GSDI_GSM_IMG2INST2},
  {UIM_GSM_IMG3INST3,                    GSDI_GSM_IMG3INST3},

  /* 180 */
  /* EFs at the telecom DF in GSM and CDMA */
  {UIM_TELECOM_ADN,                      GSDI_TELECOM_ADN},                      /* Abbrev Dialing Numbers */
  {UIM_TELECOM_FDN,                      GSDI_TELECOM_FDN},                      /* Fixed dialling numbers */
  {UIM_TELECOM_SMS,                      GSDI_TELECOM_SMS},                      /* Short messages */
  {UIM_TELECOM_CCP,                      GSDI_TELECOM_CCP},                      /* Capability Configuration Parameters */
  {UIM_TELECOM_ECCP,                     GSDI_TELECOM_ECCP},                     /* Extended CCP */
  {UIM_TELECOM_MSISDN,                   GSDI_TELECOM_MSISDN},                   /* MSISDN */
  {UIM_TELECOM_SMSP,                     GSDI_TELECOM_SMSP},                     /* SMS parameters */
  {UIM_TELECOM_SMSS,                     GSDI_TELECOM_SMSS},                     /* SMS Status */
  {UIM_TELECOM_LND,                      GSDI_TELECOM_LND},                      /* Last number dialled */
  {UIM_TELECOM_SDN,                      GSDI_TELECOM_SDN},                      /* Service Dialling numbers */

  /* 190 */
  {UIM_TELECOM_EXT1,                     GSDI_TELECOM_EXT1},                     /* Extension 1 */
  {UIM_TELECOM_EXT2,                     GSDI_TELECOM_EXT2},                     /* Extension 2 */
  {UIM_TELECOM_EXT3,                     GSDI_TELECOM_EXT3},                     /* Extension 3 */
  {UIM_TELECOM_BDN,                      GSDI_TELECOM_BDN},                      /* Barred Dialing Numbers */
  {UIM_TELECOM_EXT4,                     GSDI_TELECOM_EXT4},                     /* Extension 4 */
  {UIM_TELECOM_SMSR,                     GSDI_TELECOM_SMSR},                     /* SMS reports */
  {UIM_TELECOM_CMI,                      GSDI_TELECOM_CMI},                      /* Comparison Method Information */
  {UIM_TELECOM_SUME,                     GSDI_TELECOM_SUME},                     /* Setup Menu elements */
  {UIM_TELECOM_ARR,                      GSDI_TELECOM_ARR},                      /* Access Rule reference */
  /* EFs at DF GRAPHICS under DF Telecom */
  {UIM_IMAGE,                            GSDI_IMAGE_FILE},                       /* Image instance data files */

  /* 200 */
   /* EFs at the DF PHONEBOOK under  DF Telecom */
  {UIM_TELECOM_PBR,                      GSDI_TELECOM_PBR},                      /* Phone book reference file */
  {UIM_TELECOM_CCP1,                     GSDI_TELECOM_CCP1},                     /* Capability Configuration parameters 1 */
  {UIM_TELECOM_UID,                      GSDI_TELECOM_UID},                      /* Unique Identifier */
  {UIM_TELECOM_PSC,                      GSDI_TELECOM_PSC},                      /* Phone book synchronization center */
  {UIM_TELECOM_CC,                       GSDI_TELECOM_CC},                       /* Change counter */
  {UIM_TELECOM_PUID,                     GSDI_TELECOM_PUID},                     /* Previous Unique Identifier */
  /* EFs of USIM ADF */
  {UIM_USIM_LI,                          GSDI_USIM_LI},                          /* Language Indication */
  {UIM_USIM_IMSI,                        GSDI_USIM_IMSI},                        /* IMSI */
  {UIM_USIM_KEYS,                        GSDI_USIM_KEYS},                        /* Ciphering and Integrity keys */
  {UIM_USIM_KEYSPS,                      GSDI_USIM_KEYSPS},                      /* C and I keys for packet switched domain */

  /* 210 */
  {UIM_USIM_PLMNWACT,                    GSDI_USIM_PLMNWACT},                    /* User controlled PLMN selector with access tech */
  {UIM_USIM_UPLMNSEL,                    GSDI_USIM_UPLMNSEL},                    /* UPLMN selector */
  {UIM_USIM_HPLMN,                       GSDI_USIM_HPLMN},                       /* HPLMN search period */
  {UIM_USIM_ACM_MAX,                     GSDI_USIM_ACM_MAX},                     /* ACM maximum value */
  {UIM_USIM_UST,                         GSDI_USIM_UST},                         /* USIM Service table */
  {UIM_USIM_ACM,                         GSDI_USIM_ACM},                         /* Accumulated Call meter */
  {UIM_USIM_GID1,                        GSDI_USIM_GID1},                        /* Group Identifier Level  */
  {UIM_USIM_GID2,                        GSDI_USIM_GID2},                        /* Group Identifier Level 2 */
  {UIM_USIM_SPN,                         GSDI_USIM_SPN},                         /* Service Provider Name */
  {UIM_USIM_PUCT,                        GSDI_USIM_PUCT},                        /* Price Per Unit and Currency table */

  /* 220 */
  {UIM_USIM_CBMI,                        GSDI_USIM_CBMI},                        /* Cell Broadcast Message identifier selection */
  {UIM_USIM_ACC,                         GSDI_USIM_ACC},                         /* Access control class */
  {UIM_USIM_FPLMN,                       GSDI_USIM_FPLMN},                       /* Forbidden PLMNs */
  {UIM_USIM_LOCI,                        GSDI_USIM_LOCI},                        /* Location information */
  {UIM_USIM_AD,                          GSDI_USIM_AD},                          /* Administrative data */
  {UIM_USIM_CBMID,                       GSDI_USIM_CBMID},                       /* Cell Broadcast msg identifier for data download */
  {UIM_USIM_ECC,                         GSDI_USIM_ECC},                         /* Emergency call codes */
  {UIM_USIM_CBMIR,                       GSDI_USIM_CBMIR},                       /* Cell broadcast msg identifier range selection */
  {UIM_USIM_PSLOCI,                      GSDI_USIM_PSLOCI},                      /* Packet switched location information */
  {UIM_USIM_FDN,                         GSDI_USIM_FDN},                         /* Fixed dialling numbers */

  /* 230 */
  {UIM_USIM_SMS,                         GSDI_USIM_SMS},                         /* Short messages */
  {UIM_USIM_MSISDN,                      GSDI_USIM_MSISDN},                      /* MSISDN */
  {UIM_USIM_SMSP,                        GSDI_USIM_SMSP},                        /* SMS parameters */
  {UIM_USIM_SMSS,                        GSDI_USIM_SMSS},                        /* SMS Status */
  {UIM_USIM_SDN,                         GSDI_USIM_SDN},                         /* Service dialling numbers */
  {UIM_USIM_EXT2,                        GSDI_USIM_EXT2},                        /* Extension 2 */
  {UIM_USIM_EXT3,                        GSDI_USIM_EXT3},                        /* Extension 3 */
  {UIM_USIM_SMSR,                        GSDI_USIM_SMSR},                        /* SMS reports */
  {UIM_USIM_ICI,                         GSDI_USIM_ICI},                         /* Incoming call information */
  {UIM_USIM_OCI,                         GSDI_USIM_OCI},                         /* Outgoing call information */

  /* 240 */
  {UIM_USIM_ICT,                         GSDI_USIM_ICT},                         /* Incoming call timer */
  {UIM_USIM_OCT,                         GSDI_USIM_OCT},                         /* Outgoing call timer */
  {UIM_USIM_EXT5,                        GSDI_USIM_EXT5},                        /* Extension 5 */
  {UIM_USIM_CCP2,                        GSDI_USIM_CCP2},                        /* Capability Configuration Parameters 2 */
  {UIM_USIM_EMLPP,                       GSDI_USIM_EMLPP},                       /* Enhanced Multi Level Precedence and Priority */
  {UIM_USIM_AAEM,                        GSDI_USIM_AAEM},                        /* Automatic answer for eMLPP service */
  {UIM_USIM_GMSI,                        GSDI_USIM_GMSI},                        /* Group identity */
  {UIM_USIM_HIDDENKEY,                   GSDI_USIM_HIDDENKEY},                   /* key for hidden phonebook entries */
  {UIM_USIM_BDN,                         GSDI_USIM_BDN},                         /* Barred dialling numbers */
  {UIM_USIM_EXT4,                        GSDI_USIM_EXT4},                        /* Extension 4 */

  /* 250 */
  {UIM_USIM_CMI,                         GSDI_USIM_CMI},                         /* Comparison Method information */
  {UIM_USIM_EST,                         GSDI_USIM_EST},                         /* Enabled services table */
  {UIM_USIM_ACL,                         GSDI_USIM_ACL},                         /* Access Point Name Control List */
  {UIM_USIM_DCK,                         GSDI_USIM_DCK},                         /* De-personalization Control Keys */
  {UIM_USIM_CNL,                         GSDI_USIM_CNL},                         /* Co-operative network list */
  {UIM_USIM_START_HFN,                   GSDI_USIM_START_HFN},                   /* Init values for Hyper-frame number */
  {UIM_USIM_THRESHOLD,                   GSDI_USIM_THRESHOLD},                   /* Max value of START */
  {UIM_USIM_OPLMNWACT,                   GSDI_USIM_OPLMNWACT},                   /* Operator controlled PLMN sel with access tech */
  {UIM_USIM_OPLMNSEL,                    GSDI_USIM_OPLMNSEL},                    /* OPLMN selector */
  {UIM_USIM_HPLMNWACT,                   GSDI_USIM_HPLMNWACT},                   /* HPLMN selector with access tech */

  /* 260 */
  {UIM_USIM_ARR,                         GSDI_USIM_ARR},                         /* Access Rule reference */
  {UIM_USIM_RPLMNACT,                    GSDI_USIM_RPLMNACT},                    /* RPLMN last used access tech */
  {UIM_USIM_NETPAR,                      GSDI_USIM_NETPAR},                      /* Network parameters */
  {UIM_USIM_VMWI,                        GSDI_USIM_VMWI},                        /* CPHS: Voice Mail Waiting Indicator */
  {UIM_USIM_SVC_STR_TBL,                 GSDI_USIM_SVC_STR_TBL},                 /* CPHS: Service String Table */
  {UIM_USIM_CFF,                         GSDI_USIM_CFF},                         /* CPHS: Call Forwarding Flag */
  {UIM_USIM_ONS,                         GSDI_USIM_ONS},                         /* CPHS: Operator Name String */
  {UIM_USIM_CSP,                         GSDI_USIM_CSP},                         /* CPHS: Customer Service Profile */
  {UIM_USIM_CPHSI,                       GSDI_USIM_CPHSI},                       /* CPHS: CPHS Information */
  {UIM_USIM_MN,                          GSDI_USIM_MN},                          /* CPHS: Mailbox Number */

  /* 270 */
  {UIM_USIM_PNN,                         GSDI_USIM_PNN},                          /* CPHS: Mailbox Number */
  {UIM_USIM_OPL,                         GSDI_USIM_OPL},                          /* CPHS: Mailbox Number */
  /* EFs of ORANGE DF under USIM ADF */
  {UIM_USIM_ORANGE_DFS,                  GSDI_USIM_ORANGE_DFS},                  /* Dynamic Flags Status */
  {UIM_USIM_ORANGE_D2FS,                 GSDI_USIM_ORANGE_D2FS},                 /* Dynamic2 Flag Setting */

  {UIM_USIM_ORANGE_CSP2,                 GSDI_USIM_ORANGE_CSP2},                 /* Customer Service Profile Line2*/  /*EFs at DF PHONEBOOK under USIM ADF */
  {UIM_USIM_ORANGE_PARAMS,               GSDI_USIM_ORANGE_CSP2},
  /*EFs at DF PHONEBOOK under USIM ADF */
  {UIM_USIM_PBR,                         GSDI_USIM_PBR},                         /* Phone book reference file */
  {UIM_USIM_CCP1,                        GSDI_USIM_CCP1},                        /* Capability Configuration parameters 1 */
  {UIM_USIM_UID,                         GSDI_USIM_UID},                         /* Unique Identifier */
  {UIM_USIM_PSC,                         GSDI_USIM_PSC},                         /* Phone book synchronization center */

  /* 280 */
  {UIM_USIM_CC,                          GSDI_USIM_CC},                          /* Change counter */
  {UIM_USIM_PUID,                        GSDI_USIM_PUID},                        /* Previous Unique Identifier */
  /*EFs at DF GSM under USIM ADF */
  {UIM_USIM_KC,                          GSDI_USIM_KC},                          /* GSM ciphering key Kc */
  {UIM_USIM_KCGPRS,                      GSDI_USIM_KCGPRS},                      /* GPRS ciphering key */
  {UIM_USIM_CPBCCH,                      GSDI_USIM_CPBCCH},                      /* CPBCCH information */
  {UIM_USIM_INVSCAN,                     GSDI_USIM_INVSCAN},                     /* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
  {UIM_USIM_MEXE_ST,                     GSDI_USIM_MEXE_ST},                     /* MExE Service table */
  {UIM_USIM_ORPK,                        GSDI_USIM_ORPK},                        /* Operator Root Public Key */
  {UIM_USIM_ARPK,                        GSDI_USIM_ARPK},                        /* Administrator Root Public Key */
  {UIM_USIM_TPRPK,                       GSDI_USIM_TPRPK},                       /* Third party Root public key */

  /* 290 */
  {UIM_USIM_MBDN,                        GSDI_USIM_MBDN},                        /* Mailbox Dialing Number*/
  {UIM_USIM_EXT6,                        GSDI_USIM_EXT6},                        /* Extension 6 */
  {UIM_USIM_MBI,                         GSDI_USIM_MBI},                         /* Mailbox Identifier*/
  {UIM_USIM_MWIS,                        GSDI_USIM_MWIS},                        /* Message Waiting Indication Status */
  {UIM_USIM_SPDI,                        GSDI_USIM_SPDI},                        /* Service Provider Display Information*/
  {UIM_USIM_EHPLMN,                      GSDI_USIM_EHPLMN},                      /* Equivalent HPLMN */
  {UIM_USIM_CFIS,                        GSDI_USIM_CFIS},                        /* Call forwarding indication status */
  /* EFs of DCS1800 DF */
  {UIM_DCS1800_IMSI,                     GSDI_DCS1800_IMSI},                     /* Imsi */
  {UIM_DCS1800_KC,                       GSDI_DCS1800_KC},                       /* Ciphering Key Kc */
  {UIM_DCS1800_PLMN,                     GSDI_DCS1800_PLMN},                     /* PLMN selector */

   /* 300 */
  {UIM_DCS1800_SST,                      GSDI_DCS1800_SST},                      /* SIM Service table */
  {UIM_DCS1800_BCCH,                     GSDI_DCS1800_BCCH},                     /* Broadcast control channels */
  {UIM_DCS1800_ACC,                      GSDI_DCS1800_ACC},                      /* Access control class */
  {UIM_DCS1800_FPLMN,                    GSDI_DCS1800_FPLMN},                    /* Forbidden PLMNs */
  {UIM_DCS1800_LOCI,                     GSDI_DCS1800_LOCI},                     /* Location information */
  {UIM_DCS1800_AD,                       GSDI_DCS1800_AD},                       /* Administrative data */
  /* EFs selected by path at the telecom DF in GSM and CDMA */
  {UIM_TELECOM_IAP,                      GSDI_TELECOM_IAP},                      /* Index Administration File */
  {UIM_TELECOM_PBC,                      GSDI_TELECOM_PBC},                      /* Phone Book Control */
  {UIM_TELECOM_GRP,                      GSDI_TELECOM_GRP},                      /* Grouping File */
  {UIM_TELECOM_AAS,                      GSDI_TELECOM_AAS},                      /* Additional Number Alpha String */

  /* 310 */
  {UIM_TELECOM_GAS,                      GSDI_TELECOM_GAS},                      /* Grouping Information Alpha String */
  {UIM_TELECOM_ANR,                      GSDI_TELECOM_ANR},                      /* Additional Number */
  {UIM_TELECOM_SNE,                      GSDI_TELECOM_SNE},                      /* Secondary Name Entry */
  {UIM_TELECOM_EMAIL,                    GSDI_TELECOM_EMAIL},                    /* Email */
  {UIM_TELECOM_ANRA,                     GSDI_TELECOM_ANRA},                     /* Additional Number A */
  {UIM_TELECOM_ANRB,                     GSDI_TELECOM_ANRB},                     /* Additional Number B */
  {UIM_TELECOM_ANR1,                     GSDI_TELECOM_ANR1},                     /* Additional Number C */
  {UIM_TELECOM_ANRA1,                    GSDI_TELECOM_ANRA1},                    /* Additional Number A1 */
  {UIM_TELECOM_ANRB1,                    GSDI_TELECOM_ANRB1},                    /* Additional Number B1 */
  {UIM_TELECOM_ADN1,                     GSDI_TELECOM_ADN1},                     /* Abbreviated Dialing Numbers 1 */

  /* 320 */
  {UIM_TELECOM_PBC1,                     GSDI_TELECOM_PBC1},                     /* Phone Book Control 1 */
  {UIM_TELECOM_GRP1,                     GSDI_TELECOM_GRP1},                     /* Grouping File 1 */
  {UIM_TELECOM_SNE1,                     GSDI_TELECOM_SNE1},                     /* Secondary Name Entry 1 */
  {UIM_TELECOM_UID1,                     GSDI_TELECOM_UID1},                     /* Unique Identifier 1 */
  {UIM_TELECOM_EMAIL1,                   GSDI_TELECOM_EMAIL1},                   /* Email 1 */
  {UIM_TELECOM_IAP1,                     GSDI_TELECOM_IAP1},                     /* Index Administration File 1 */
  /* EFs selected by path of USIM ADF */
  {UIM_USIM_ADN,                         GSDI_USIM_ADN},                         /* Abbreviated Dialing Numbers */
  {UIM_USIM_IAP,                         GSDI_USIM_IAP},                         /* Index Administration File */
  {UIM_USIM_PBC,                         GSDI_USIM_PBC},                         /* Phone Book Control */
  {UIM_USIM_EXT1,                        GSDI_USIM_EXT1},                        /* Extension 1 */

  /* 330 */
  {UIM_USIM_GRP,                         GSDI_USIM_GRP},                         /* Grouping File */
  {UIM_USIM_AAS,                         GSDI_USIM_AAS},                         /* Additional Number Alpha String */
  {UIM_USIM_AAS1,                        GSDI_USIM_AAS1},                        /* Additional Number Alpha String 1 */
  {UIM_USIM_GAS,                         GSDI_USIM_GAS},                         /* Grouping Information Alpha String */
  {UIM_USIM_GAS1,                        GSDI_USIM_GAS1},                        /* Grouping Information Alpha String */
  {UIM_USIM_ANR,                         GSDI_USIM_ANR},                         /* Additional Number */
  {UIM_USIM_SNE,                         GSDI_USIM_SNE},                         /* Secondary Name Entry */
  {UIM_USIM_EMAIL,                       GSDI_USIM_EMAIL},                       /* Email */
  {UIM_USIM_ANRA,                        GSDI_USIM_ANRA},                        /* Additional Number A */
  {UIM_USIM_ANRB,                        GSDI_USIM_ANRB},                        /* Additional Number B */

  /* 340 */
  {UIM_USIM_ANRC,                        GSDI_USIM_ANRC},                        /* Additional Number C */
  {UIM_USIM_ANR1,                        GSDI_USIM_ANR1},                        /* Additional Number 1 */
  {UIM_USIM_ANRA1,                       GSDI_USIM_ANRA1},                       /* Additional Number A1 */
  {UIM_USIM_ANRB1,                       GSDI_USIM_ANRB1},                       /* Additional Number B1 */
  {UIM_USIM_ANRC1,                       GSDI_USIM_ANRC1},                       /* Additional Number C1 */
  {UIM_USIM_ADN1,                        GSDI_USIM_ADN1},                        /* Abbreviated Dialing Numbers 1 */
  {UIM_USIM_PBC1,                        GSDI_USIM_PBC1},                        /* Phone Book Control 1 */
  {UIM_USIM_GRP1,                        GSDI_USIM_GRP1},                        /* Grouping File 1 */
  {UIM_USIM_SNE1,                        GSDI_USIM_SNE1},                        /* Secondary Name Entry 1 */
  {UIM_USIM_UID1,                        GSDI_USIM_UID1},                        /* Unique Identifier 1 */

  /* 350 */
  {UIM_USIM_EMAIL1,                      GSDI_USIM_EMAIL1},                      /* Email 1 */
  {UIM_USIM_IAP1,                        GSDI_USIM_IAP1},                        /* Index Administration File 1 */
  /* EFs of WIM DF */
  {UIM_WIM_ODF,                          GSDI_WIM_ODF},                          /* WIM ODF */
  {UIM_WIM_TOKEN_INFO,                   GSDI_WIM_TOKEN_INFO},                   /* Generic token info and capabilities */
  {UIM_WIM_UNUSED_SPACE,                 GSDI_WIM_UNUSED_SPACE},                 /* Unused Space */
  /* EFs of ORANGE DF */
  {UIM_ORANGE_DFS,                       GSDI_ORANGE_DFS},                       /* Dynamic Flags Status */
  {UIM_ORANGE_D2FS,                      GSDI_ORANGE_D2FS},                      /* Dynamic2 Flag Setting */
  {UIM_ORANGE_CSP2,                      GSDI_ORANGE_CSP2},                      /* Customer Service Profile Line2*/
  {UIM_ORANGE_PARAMS,                    GSDI_ORANGE_PARAMS},                    /* Welcome Message */
  /* EFS of Cingular */
  {UIM_CINGULAR_ACT_HPLMN,               GSDI_CINGULAR_ACT_HPLMN},                /* Acting HPLMN*/

  /* 360 */
  {UIM_CINGULAR_SPT_TABLE,               GSDI_CINGULAR_SPT_TABLE},                /* Support Table*/
  /*EFs of HZ*/
  {UIM_HZI_HZ,                           GSDI_HZI_HZ},
  {UIM_HZI_CACHE1,                       GSDI_HZI_CACHE1},                         /*UHZI-Cache1*/
  {UIM_HZI_CACHE2,                       GSDI_HZI_CACHE2},                         /*UHZI-Cache2*/
  {UIM_HZI_CACHE3,                       GSDI_HZI_CACHE3},                         /*UHZI-Cache3*/
  {UIM_HZI_CACHE4,                       GSDI_HZI_CACHE4},                         /*UHZI-Cache4*/
  {UIM_HZI_SUBSCRIBED_LAC_CI_1,          GSDI_HZI_SUBSCRIBED_LAC_CI_1},            /*UHZI-SUBSCRIBED_LAC_CI_1*/
  {UIM_HZI_SUBSCRIBED_LAC_CI_2,          GSDI_HZI_SUBSCRIBED_LAC_CI_2},            /*UHZI-SUBSCRIBED_LAC_CI_2*/
  {UIM_HZI_SUBSCRIBED_LAC_CI_3,          GSDI_HZI_SUBSCRIBED_LAC_CI_3},            /*UHZI-SUBSCRIBED_LAC_CI_3*/

  /* 370 */
  {UIM_HZI_SUBSCRIBED_LAC_CI_4,          GSDI_HZI_SUBSCRIBED_LAC_CI_4},            /*UHZI-SUBSCRIBED_LAC_CI_4*/
  {UIM_HZI_TAGS,                         GSDI_HZI_TAGS},                           /*UHZI-Tags*/
  {UIM_HZI_UHZI_SETTINGS,                GSDI_HZI_UHZI_SETTINGS},                  /*UHZI-Settings*/
  /* EFs given by path */
  {UIM_EF_BY_PATH,                       GSDI_EF_BY_PATH},                       /* File selection by path */
  /* EFs for MMS Notification */
  {UIM_USIM_MMSN,                        GSDI_USIM_MMSN},                          /* MMS Notification */
  {UIM_USIM_MMSICP,                      GSDI_USIM_MMSICP},                        /* MMS Issuer connectivity parameters */
  {UIM_USIM_MMSUP,                       GSDI_USIM_MMSUP},                         /* MMS User Preferences */
  {UIM_USIM_MMSUCP,                      GSDI_USIM_MMSUCP},                        /* MMS User connectivity parameters */
  {UIM_USIM_EXT8,                        GSDI_USIM_EXT8},                          /* Extension 8 */

  {UIM_NO_SUCH_ITEM,                     GSDI_NO_SUCH_ITEM},
  /* No such item */
  {UIM_NO_SUCH_ITEM,                     GSDI_NO_SUCH_ITEM}
  /* No such item */
};

/*
**gsdi_file_service_table
** Maps GSDI Files Name to GSDI Service
*/

gsdi_file_service_table_type gsdi_file_service_table[GSDI_FILE_SERVICE_TABLE_SIZE] =
{
  {GSDI_TELECOM_ADN,                     GSDI_ADN},
  {GSDI_TELECOM_FDN,                     GSDI_FDN},
  {GSDI_USIM_FDN,                        GSDI_FDN},
  {GSDI_TELECOM_SMS,                     GSDI_SMS},
  {GSDI_TELECOM_SMSS,                    GSDI_SMS},
  {GSDI_USIM_SMS,                        GSDI_SMS},
  {GSDI_USIM_SMSS,                       GSDI_SMS},
  {GSDI_GSM_ACM,                         GSDI_AOC},
  {GSDI_USIM_ACM,                        GSDI_AOC},
  {GSDI_GSM_ACM_MAX,                     GSDI_AOC},
  {GSDI_USIM_ACM_MAX,                    GSDI_AOC},
  {GSDI_GSM_PUCT,                        GSDI_AOC},
  {GSDI_USIM_PUCT,                       GSDI_AOC},
  {GSDI_TELECOM_CCP,                     GSDI_CCP},            /* Configuration Capability Params  */
  {GSDI_GSM_PLMN,                        GSDI_PLMN_SELECTOR},  /* PLMN Selector                    */
  {GSDI_TELECOM_MSISDN,                  GSDI_MSISDN},         /* MSISDN                           */
  {GSDI_USIM_MSISDN,                     GSDI_MSISDN},         /* MSISDN                           */
  {GSDI_TELECOM_EXT1,                    GSDI_EXT1},           /* Extension 1                      */
  {GSDI_TELECOM_EXT2,                    GSDI_EXT2},           /* Extension 2                      */
  {GSDI_USIM_EXT2,                       GSDI_EXT2},           /* Extension 2                      */
  {GSDI_TELECOM_SMSP,                    GSDI_SMSP},           /* SMS Parameters                   */
  {GSDI_USIM_SMSP,                       GSDI_SMSP},
  {GSDI_TELECOM_SMSR,                    GSDI_SMSR},           /* Short Message Status Reports     */
  {GSDI_USIM_SMSR,                       GSDI_SMSR},
  {GSDI_TELECOM_LND,                     GSDI_LND},            /* Last Number Dialled              */
  {GSDI_GSM_CBMID,                       GSDI_DATA_DL_SMSCB},    /* Cell Broadcast Message Identifier for Data Download */
  {GSDI_GSM_CBMI,                        GSDI_CBMI},  /* Cell Broadcast Message Identifier */
  {GSDI_GSM_GID1,                        GSDI_GID1},           /* Group Identifier 1               */
  {GSDI_USIM_GID1,                       GSDI_GID1},           /* Group Identifier 1               */
  {GSDI_GSM_GID2,                        GSDI_GID2},           /* Group Identifier 2               */
  {GSDI_USIM_GID2,                       GSDI_GID2},           /* Group Identifier 2               */
  {GSDI_GSM_SPN,                         GSDI_SPN},            /* Service Provider Name            */
  {GSDI_USIM_SPN,                        GSDI_SPN},            /* Service Provider Name            */
  {GSDI_TELECOM_SDN,                     GSDI_SDN},            /* Service Dialling Numbers         */
  {GSDI_USIM_SDN,                        GSDI_SDN},            /* Service Dialling Numbers         */
  {GSDI_TELECOM_EXT3,                    GSDI_EXT3},           /* Extension 3                      */
  {GSDI_USIM_EXT3,                       GSDI_EXT3},           /* Extension 3                      */
  {GSDI_GSM_VGCS,                        GSDI_VGCS_GID_LIST},  /* VGCS Group Identifier List       */
  {GSDI_GSM_VBS,                         GSDI_VBS_GID_LIST},   /* VBS Group Identifier List        */
  {GSDI_GSM_EMLPP,                       GSDI_ENH_ML_SVC},     /* Enhanced Multi Level precedence and Premption Service */
  {GSDI_USIM_EMLPP,                      GSDI_ENH_ML_SVC},     /* Enhanced Multi Level precedence and Premption Service */
  {GSDI_GSM_AAEM,                        GSDI_AA_EMLPP},       /* Automatic Answer for eMLPP       */
  {GSDI_USIM_AAEM,                       GSDI_AA_EMLPP},       /* Automatic Answer for eMLPP       */
  {GSDI_USIM_CBMIR,                      GSDI_CBMID_RANGES},   /* Cell Broadcast Message Identifiers */
  {GSDI_USIM_BDN,                        GSDI_BDN},            /* Barred Dialing Numbers           */
  {GSDI_TELECOM_EXT4,                    GSDI_EXT4},           /* Extension 4                      */
  {GSDI_USIM_EXT4,                       GSDI_EXT4},           /* Extension 4                      */
  {GSDI_GSM_DCK,                         GSDI_DEPERSON_KEYS},  /* Depersonalization Keys           */
  {GSDI_USIM_DCK,                        GSDI_DEPERSON_KEYS},  /* Depersonalization Keys           */
  {GSDI_GSM_CNL,                         GSDI_COOP_NETWK_LIST},/* Co-operative Network List        */
  {GSDI_USIM_CNL,                        GSDI_COOP_NETWK_LIST},/* Co-operative Network List        */
  {GSDI_GSM_NIA,                         GSDI_NETWK_ALTERING_MS}, /* Network's Indication of alertin in the MS */
  {GSDI_GSM_KCGPRS,                      GSDI_GPRS},           /* GPRS                             */
  {GSDI_GSM_LOCIGPRS,                    GSDI_GPRS},           /* GPRS                             */
  {GSDI_USIM_KC,                         GSDI_GSM_ACCESS_IN_USIM},/* GSM Access in USIM            */
  {GSDI_USIM_KCGPRS,                     GSDI_GSM_ACCESS_IN_USIM},/* GSM Access in USIM            */
  {GSDI_GSM_IMG,                         GSDI_IMAGE},          /* Image (IMG)                      */
  {GSDI_IMAGE_FILE,                      GSDI_IMAGE},          /* Image (IMG)                      */
  {GSDI_GSM_PLMNWACT,                    GSDI_UPLMN_SEL_WACT},  /* User controlled PLMN Selector with Access Technology */
  {GSDI_USIM_PLMNWACT,                   GSDI_UPLMN_SEL_WACT},  /* User controlled PLMN Selector with Access Technology */
  {GSDI_USIM_UPLMNSEL,                   GSDI_UPLMN_SEL_WACT},  /* User controlled PLMN Selector with Access Technology */
  {GSDI_GSM_OPLMNWACT,                   GSDI_OPLMN_SEL_WACT},  /* Operator controlled PLMN Selector with Access Technology */
  {GSDI_USIM_OPLMNWACT,                  GSDI_OPLMN_SEL_WACT},  /* Operator controlled PLMN Selector with Access Technology */
  {GSDI_GSM_HPLMNACT,                    GSDI_HPLMN_WACT},      /* HPLMN Selector with Access Technology */
  {GSDI_USIM_HPLMNWACT,                  GSDI_HPLMN_WACT},      /* HPLMN Selector with Access Technology */
  {GSDI_GSM_CPBCCH,                      GSDI_CPBCCH},          /* CP BCCH */
  {GSDI_USIM_CPBCCH,                     GSDI_CPBCCH},          /* CP BCCH */
  {GSDI_GSM_INVSCAN,                     GSDI_INV_SCAN},        /* Investigation Scan */
  {GSDI_USIM_INVSCAN,                    GSDI_INV_SCAN},        /* Investigation Scan */
  {GSDI_USIM_MEXE_ST,                    GSDI_MEXE},            /* MExE info */
  {GSDI_GSM_RPLMNAT,                     GSDI_RPLMN_LACT},      /* RPLMN Last used Access Technology */
  {GSDI_USIM_RPLMNACT,                   GSDI_RPLMN_LACT},      /* RPLMN Last used Access Technology */
  {GSDI_GSM_PNN,                         GSDI_PLMN_NTWRK_NAME}, /* PLMN Network Name */
  {GSDI_USIM_PNN,                        GSDI_PLMN_NTWRK_NAME}, /* PLMN Network Name */
  {GSDI_GSM_OPL,                         GSDI_OPLMN_LIST},      /* Operator PLMN list */
  {GSDI_USIM_OPL,                        GSDI_OPLMN_LIST},      /* Operator PLMN list */
  {GSDI_USIM_PBR,                        GSDI_LOCAL_PHONEBOOK}, /* Local Phonebook, UADF/Phonebook/ADN */
  {GSDI_USIM_CCP1,                       GSDI_LOCAL_PHONEBOOK}, /* Local Phonebook, UADF/Phonebook/ADN */
  {GSDI_USIM_UID,                        GSDI_LOCAL_PHONEBOOK}, /* Local Phonebook, UADF/Phonebook/ADN */
  {GSDI_USIM_PSC,                        GSDI_LOCAL_PHONEBOOK}, /* Local Phonebook, UADF/Phonebook/ADN */
  {GSDI_USIM_PUID,                       GSDI_LOCAL_PHONEBOOK}, /* Local Phonebook, UADF/Phonebook/ADN */
  {GSDI_USIM_OCI,                        GSDI_OCI_OCT},         /* Outgoing Call information/timer */
  {GSDI_USIM_OCT,                        GSDI_OCI_OCT},         /* Outgoing Call information/timer */
  {GSDI_USIM_ICI,                        GSDI_ICI_ICT},         /* Incoming call information/timer */
  {GSDI_USIM_ICT,                        GSDI_ICI_ICT},         /* Incoming call information/timer */
  {GSDI_USIM_CBMI,                       GSDI_CBMI},           /* Cell Broadcast Message Identifier */
  {GSDI_USIM_CBMID,                      GSDI_DATA_DL_SMSCB},           /* Cell Broadcast Message Identifier for Data Download */
  {GSDI_USIM_EST,                        GSDI_EST},             /* Enable Service Table in USIM */
  {GSDI_USIM_ACL,                        GSDI_ACL},             /* APN Control list */
  {GSDI_USIM_EXT5,                       GSDI_EXT5},            /* Ext 5 */
  {GSDI_USIM_EHPLMN,                     GSDI_EHPLMN},          /* Equivalent HPLMN */
  {GSDI_GSM_MBDN,                        GSDI_MAILBOX_DIAL},    /* Mailbox dialling numbers */
  {GSDI_USIM_MBDN,                       GSDI_MAILBOX_DIAL},    /* Mailbox dialling numbers */
  {GSDI_GSM_CFIS,                        GSDI_CALL_FWD_IND},    /* Call Forward indication status */
  {GSDI_USIM_CFIS,                       GSDI_CALL_FWD_IND},    /* Call Forward indication status */
  {GSDI_USIM_MMSN,                       GSDI_MMS},             /* MMS Notification */
  {GSDI_USIM_MMSICP,                     GSDI_MMS},             /* MMS Issuer connectivity parameters */
  {GSDI_USIM_MMSUP,                      GSDI_MMS},             /* MMS User Preferences */
  {GSDI_USIM_MMSUCP,                     GSDI_MMS},             /* MMS User connectivity parameters */
  {GSDI_USIM_EXT8,                       GSDI_EXT8}             /* Extension 8 */

};

/*
**USIM APPLICATION-PIN TABLE CACHE
*/
gsdi_app_pin_info_type gsdi_app_pin_info_table[GSDI_MAX_APP_COUNT] =
{
  {{0,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}}},
  {{0,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}}},
  {{0,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}}},
  {{0,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}}},
  {{0,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}},
   {GSDI_PIN1,GSDI_PIN_NOT_INITIALISED,FALSE,{0}}}
};

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS or FEATURE_MMGSDI_GSM */

gsdi_refresh_vote_node_type       refresh_vote_list_node;
gsdi_refresh_fcn_notify_node_type refresh_fcn_notify_list_node;

#ifdef FEATURE_HOME_ZONE_SERVICE
gsdi_returns_T (*sim_read_hzi)(
  gsdi_sim_read_req_T * req_p,
  byte *                rec_num_accessed_p,
  byte *                num_records_p,
  byte *                returned_data_len_p,
  byte *                data_p
);

gsdi_returns_T (*sim_write_hzi)(
  gsdi_sim_write_req_T * req_p,
  byte *                returned_data_len_p
 );
#endif /*FEATURE_HOME_ZONE_SERVICE*/


/*===========================================================================
  S T A T I C   P R O T O T Y P E S
===========================================================================*/

LOCAL void gsdi_sim_init_completed(
  gsdi_slot_id_type slot
);

static gsdi_returns_T gsdi_handle_secure_device_info(
  const gsdi_secure_device_info_req_T *req_p
);

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA)
static gsdi_returns_T gsdi_init_ecc_cache(
  gsdi_slot_id_type      slot,
  mmgsdi_file_enum_type  mmgsdi_file_name,
  gsdi_ef_ecc_data_T   * sim_ecc_p,
  const byte           * data_p
);
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_CDMA */

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

static void gsdi_remove_apps_from_apps_enabled(
  gsdi_slot_id_type slot,
  uint8             card_app
);

static uim_protocol_type gsdi_get_EF_protocol (
  gsdi_slot_id_type   slot,
  uim_items_enum_type requested_EF
);

static boolean gsdi_is_sim_present_and_ready(
  gsdi_slot_id_type slot
);

static gsdi_returns_T gsdi_proc_handle_all_pin_info(
  gsdi_slot_id_type slot,
  gsdi_pin_info_item_T * pin_info_item
);

static boolean gsdi_is_language_check(
  uim_items_enum_type item
);

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
static gsdi_returns_T  gsdi_proc_send_apdu(
    gsdi_send_apdu_req_T * req_p,
    uint8                * ret_data_len_p,
    uint8                * data_p
);
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */

static gsdi_returns_T gsdi_proc_activate_onchip_sim(
  gsdi_slot_id_type                      slot,
  const gsdi_activate_onchip_sim_req_T * req_p,
  const uint8 *                          data_p
);

static gsdi_returns_T gsdi_proc_notify_client(
  const gsdi_notify_client_req_T *req
);

static gsdi_slot_id_type gsdi_get_slot_from_req(
  void * req_p
);

static boolean gsdi_is_sim_in_slot_available(
  gsdi_slot_id_type slot
);

static gsdi_returns_T gsdi_proc_sim_access (
  gsdi_slot_id_type       slot,
  gsdi_sim_access_req_T * req_p,
  byte *                  rec_num_accessed_p,
  int32*                  returned_data_length,
  uint8 *                 data_p
);

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
static gsdi_returns_T gsdi_populate_cached_attributes_if_needed(
  gsdi_slot_id_type   slot,
  uim_items_enum_type item,
  uim_protocol_type   EF_protocol,
  uint8               *data_p
);
#endif /* FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */

static gsdi_returns_T gsdi_proc_handle_client_req (
  byte*          rec_num_accessed_p,
  byte*          num_records_p,
  gs_queue_id_T* return_queue_id_p,
  uint32*        client_ref_p,
  void*          req_p,
  int32*         returned_data_len_p,
  byte*          data_p,
  gsdi_resp_type* resp_type_p
);

static gsdi_returns_T gsdi_populate_card_app_slot_info(
  gsdi_slot_id_type             slot,
  gsdi_uim_link_established_T   *message_p,
  byte *                        data_p
);

static gsdi_returns_T gsdi_process_link_establish(
    gsdi_uim_link_established_T *   message_p,
    byte *                          data_p
);

static void gsdi_send_sim_inserted_event(void);

static gsdi_sim_events_T gsdi_determine_sim_init_completed_event (
  gsdi_slot_id_type slot,
  uint8             op_slot,
  uint8             apps_enabled
);

static gsdi_slot_id_type gsdi_get_slot_available_for_post_pin1_init(
  void
);

#ifdef FEATURE_MMGSDI_PERSONALIZATION
static gsdi_returns_T gsdi_perso_refresh(
  gsdi_slot_id_type slot,
  int32 * len_p,
  uint8  * data_p
);
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */


/*===========================================================================
FUNCTION   GSDI_HANDLE_SECURE_DEVICE_INFO

DESCRIPTION
  Function is called to hanle the processing request to secure the device
  information.  This device information consist of the IMEI and Serial
  number of the device.

  This device is stored in NV and then transferred to the secure area
  as defined.

DEPENDENCIES
  None

RETURNS
  void

SIDE EFFECTS
  None

===========================================================================*/
static gsdi_returns_T gsdi_handle_secure_device_info(const gsdi_secure_device_info_req_T *req_p)
{
  #ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  gsdi_returns_T     gsdi_status = GSDI_SUCCESS;
  uint8              imei_data_p[GSDI_PERSO_MAX_IMEI_LEN*2+1];
  uint32             imei_len = 0;

  #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

  FlashOTP_ReturnType flash_status = FLASH_OTP_IO_SUCCESS;

  #endif /*FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

  /* ------------------------------------------------------------------------
  Set IMEI data buffer to 0x00
  Get the IMEI and convert to ASCII format
  Null Terminate the IMEI data buffer
  ----------------------------------------------------------------------*/
  memset(imei_data_p, 0x00, GSDI_PERSO_MAX_IMEI_LEN*2+1);
  gsdi_status = gsdi_perso_engine_get_imei(FALSE,imei_data_p, &imei_len);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not get IMEI",0,0,0);
    return gsdi_status;
  }
  if (imei_len > GSDI_PERSO_MAX_IMEI_LEN*2)
  {
    MMGSDI_DEBUG_MSG_ERROR("Potential Scripple of Memory 0x%x",imei_len,0,0);
    return GSDI_ERROR;
  }
  imei_data_p[imei_len] = 0x00;

  /* ------------------------------------------------------------------------
  Write to Flash OTP Block of the NAND Device
  ----------------------------------------------------------------------*/
  #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  #ifdef FEATURE_MMGSDI_LTK

  flash_status = mmgsdi_ltk_flash_otp_operation(FLASH_OTP_OP_IMEI_WRITE, imei_data_p);

  #else /* FEATURE_MMGSDI_LTK */

  flash_status = flash_otp_operation(FLASH_OTP_OP_IMEI_WRITE, imei_data_p);

  #endif /* FEATURE_MMGSDI_LTK */

  if (flash_status != FLASH_OTP_IO_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not Write IMEI",0,0,0);
    return GSDI_ERROR;
  }

  #ifdef FEATURE_MMGSDI_LTK

  flash_status = mmgsdi_ltk_flash_otp_enable_authentication();

  #else /* FEATURE_MMGSDI_LTK */

  flash_status = flash_otp_enable_authentication();

  #endif /* FEATURE_MMGSDI_LTK */

  if (flash_status != FLASH_OTP_IO_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not Write Serial Number",0,0,0);
    return GSDI_ERROR;
  }

  #ifdef FEATURE_MMGSDI_LTK

  flash_status = mmgsdi_ltk_flash_otp_activate();

  #else /* FEATURE_MMGSDI_LTK */

  flash_status = flash_otp_activate();

  #endif /* FEATURE_MMGSDI_LTK */

  if (flash_status != FLASH_OTP_PROTECT_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not Activate",0,0,0);
    return GSDI_ERROR;
  }

  #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

  return gsdi_status;

  #else /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

  return GSDI_NOT_SUPPORTED;

  #endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

} /* gsdi_handle_activate */ /*lint !e715 */

#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_DISCARD_CNF

DESCRIPTION
  Function will be used to provide a callback for GSDI to call upon when
  complets servicing a REQUEST that is generated internally by GSDI.

  Not much is done with this function at the time.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_discard_cnf ( gsdi_cnf_T * gsdi_cnf)
{
    gsdi_returns_T gsdi_status = GSDI_ERROR;

    if(gsdi_cnf != NULL)
    {
      gsdi_status = gsdi_cnf->message_header.gsdi_status;
    }

    if ( gsdi_status == GSDI_SUCCESS )
        MSG_MED("DISCARDED MSG STATUS SUCCESS: %x",gsdi_status,0,0);
    else
        MSG_MED("DISCARDED MSG STATUS FAILURE: %x",gsdi_status,0,0);

} /* gsdi_discard_cnf */


#ifdef FEATURE_GSTK
/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LINK_VOTING_TO_LIST_TAIL

DESCRIPTION
  Function called to add the link list structure to the end "tail" of the list.
  The node added to the tail is for voting only.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_link_to_voting_list_tail (
    gsdi_refresh_vote_node_type * list_node_head,
    gsdi_refresh_vote_node_type * list_tail_new
)
{
    gsdi_refresh_vote_node_type * node_p;
    if(list_node_head == NULL)
    {
      return;
    }
    node_p = (gsdi_refresh_vote_node_type *)list_node_head->tail_p;


    if (node_p == NULL)
    {
      MSG_ERROR("List node head's tail is NULL", 0, 0, 0);
      return;
    }

    /* Update Old Tail Information to
    ** use the next_p since it is no
    ** longer at the tail
    */
    node_p->next_p = (void *)list_tail_new;

    /* Update New Tail Information to
    ** indicate it is the final element
    */
    if(list_tail_new == NULL)
    {
      return;
    }

    list_tail_new->head_p = (void *)list_node_head;
    list_tail_new->prev_p = (void *)node_p;
    list_tail_new->next_p = NULL;

    /* Reassin temporary node_p to start
    ** from head and set all tail_p to
    ** new tail.
    */
    node_p = (void *)list_node_head;

    do
    {
        node_p->tail_p = (void *)list_tail_new;
        node_p         = (void *)node_p->next_p;
    }while ( node_p != NULL );

} /* gsdi_link_to_voting_list_tail */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LINK_TO_FCN_LIST_TAIL

DESCRIPTION
  Function called to add the link list structure to the end "tail" of the list.
  The node added to the tail is for File Change Notifications only.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_link_to_fcn_list_tail (
    gsdi_refresh_fcn_notify_node_type * list_node_head,
    gsdi_refresh_fcn_notify_node_type * list_tail_new
)
{
    gsdi_refresh_vote_node_type * node_p ;

    if(list_node_head == NULL)
    {
      return;
    }
    node_p = (void *)list_node_head->tail_p;

    /* Update Old Tail Information to
    ** use the next_p since it is no
    ** longer at the tail
    */
    node_p->next_p = (void *)list_tail_new;

    /* Update New Tail Information to
    ** indicate it is the final element
    */
    if(list_tail_new == NULL)
    {
      return;
    }
    list_tail_new->head_p = (void *)list_node_head;
    list_tail_new->prev_p = (void *)node_p;
    list_tail_new->next_p = NULL;

    /* Reassin temporary node_p to start
    ** from head and set all tail_p to
    ** new tail.
    */
    node_p = (void *)list_node_head;
    do
    {
      node_p->tail_p = (void *)list_tail_new;
      node_p         = (void *)node_p->next_p;
    }while ( node_p != NULL );

} /* gsdi_link_to_fcn_list_tail */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_REFRESH_SCAN_VOTE_LIST_FOR_DUPS

DESCRIPTION
  Function is called to scan the Refresh Vote list of refresh voting
  registrations for possible duplicate registrations.

  To be considered a duplicate registration the following criteria must
  be met.
  1.  The registration callback provided must be the same

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - Added to Link List Successfully
                   GSDI_ERROR     - No Memory Heap
                   GSDI_NOT_FOUND - Not found in Scan

SIDE EFFECTS
  Allocates memory for a gsdi_refresh_fcn_notify_node_type
===========================================================================*/
static gsdi_returns_T gsdi_refresh_scan_vote_list_for_dups(
  const gsdi_refresh_registration_req_T * req_p
)
{
    gsdi_refresh_vote_node_type *     node_p    = NULL;

    MSG_LOW("GSDI_SCAN_REFRESH_VOTE_LINK_LIST_FOR_DUPS",0,0,0);

    /* Check to see if the req_p is NULL */
    if ( req_p == NULL )
    {
      /* Null Pointer received...end the prococedure */
        return GSDI_ERROR;
    }

    /* Next perform the scan
    ** Start by checking the global FCN List head Pointer to see if it is
    ** NULL
    */
    if ( refresh_vote_list_node.head_p == NULL )
    {
        return GSDI_NOT_FOUND;
    }

    /* The Head Pointer is set...go ahead and make the Node
    ** Pointer the Head Pointer
    */
    node_p = (gsdi_refresh_vote_node_type*)refresh_vote_list_node.head_p;

    while ( node_p != NULL  )
    {
      /* First Check the Callback to see if we are registering
      ** the same_callback
      */
      if ( (gsdi_refresh_vote_cb_type*)(node_p->vote_data.callback) ==
             req_p->reg_vote_cb )
      {
        return GSDI_SUCCESS;
      }
      node_p = (gsdi_refresh_vote_node_type*)node_p->next_p;
    }
    return GSDI_NOT_FOUND;
} /* gsdi_refresh_scan_vote_list_for_dups */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_ADD_TO_VOTE_REFRESH_LINK_LIST

DESCRIPTION
  Function called from gsdi_proc_handle_client_req to add the item to the
  linked list.  The first element of the link list is not a pointer to heap.
  The first element is a structure.  This will be used to add a voting
  request to the link list.

  If the item added to the link list is being added to the first time,
  it will be automatically to the head.  Each subsequent call to this
  function will get a pointer from head.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Added to Link List Successfully
                    GSDI_ERROR   - No Memory Heap

SIDE EFFECTS
  Allocates Memory for a gsdi_refresh_vote_node_type
===========================================================================*/
static gsdi_returns_T gsdi_add_to_vote_refresh_link_list(
    gsdi_refresh_registration_req_T * req_p
)
{
    gsdi_returns_T gsdi_status = GSDI_ERROR;
    mmgsdi_return_enum_type mmgsdi_status;

    MSG_LOW("GSDI_ADD_TO_REFRESH_LINK_LIST",0,0,0);
    MMGSDI_RETURN_IF_NULL(req_p);

    /* Determine whether or not the client requesting registration has
    ** already registered for RESETs.  This will handle cases where a
    ** GSDI_SIM_INIT_COMPLETED Event is generated (i.e. REFRESH) and the
    ** client keys off of this to register again
    */
    gsdi_status = gsdi_refresh_scan_vote_list_for_dups(req_p);

    if ( gsdi_status != GSDI_NOT_FOUND )
    {
      /* RESET Registration Request is either a duplicate (i.e.
      ** gsdi_status = GSDI_SUCCESS or there was a link list
      ** operation that failed in which case gsdi_status is
      ** as GSDI_ERROR;
      */
      return gsdi_status;
    }

    /* Determine whether this is the first add
    ** to the refresh voting list table
    */
    if ( refresh_vote_list_node.head_p == NULL )
    {
        /* Request Made will be added to
        ** the beggining of this linked list
        */
        refresh_vote_list_node.head_p = (void *)&refresh_vote_list_node;
        refresh_vote_list_node.tail_p = (void *)&refresh_vote_list_node;
        refresh_vote_list_node.next_p = NULL;
        refresh_vote_list_node.prev_p = NULL;

        refresh_vote_list_node.vote_data.callback = req_p->reg_vote_cb;
        refresh_vote_list_node.vote_data.ok_to_file_notify = FALSE;
        refresh_vote_list_node.vote_data.ok_to_take_down_stack = FALSE;
    }
    else
    {
        gsdi_refresh_vote_node_type * refresh_next_node;
        /* Not the first time we add to
        ** link list, need to add as
        ** next element
        */

        /* First get memory */
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(refresh_next_node,
            sizeof(gsdi_refresh_vote_node_type), mmgsdi_status);
        if ((mmgsdi_status != MMGSDI_SUCCESS) ||
            (refresh_next_node == NULL))
        {
            MMGSDI_DEBUG_MSG_ERROR("FAILED TO GET LIST MEMORY",0,0,0);
            return GSDI_ERROR;
        }

        refresh_next_node->vote_data.callback = req_p->reg_vote_cb;
        refresh_next_node->vote_data.ok_to_take_down_stack     = FALSE;
        refresh_next_node->vote_data.ok_to_file_notify         = FALSE;

        gsdi_link_to_voting_list_tail(&refresh_vote_list_node,
                                      refresh_next_node);

    }
    return GSDI_SUCCESS;

}/* gsdi_add_to_vote_refresh_link_list */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_REFRESH_SCAN_FCN_LINK_LIST_FOR_DUPS

DESCRIPTION
  Function is called to scan the FCN Link List of FCN Registrations for
  possible duplicate registrations.

  To be considered a duplicate registration the following criteria must
  be met.
  1.  The registration callback provided must be the same
  2.  The files requested in the FCN must be a subset or an exact match
      of the list of files in the List.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - Added to Link List Successfully
                   GSDI_ERROR     - No Memory Heap
                   GSDI_NOT_FOUND - Not found in Scan

SIDE EFFECTS
  Allocates memory for a gsdi_refresh_fcn_notify_node_type
===========================================================================*/
static gsdi_returns_T gsdi_refresh_scan_link_list_for_dups(
  gsdi_refresh_registration_req_T * req_p
)
{
    gsdi_refresh_fcn_notify_node_type  *node_p                = NULL;
    gsdi_task_notify_file_list_cb_type *l_callback            = NULL;


    MSG_LOW("GSDI_SCAN_FCN_LINK_LIST_FOR_DUPS",0,0,0);

    /* Check to see if the req_p is NULL */
    MMGSDI_RETURN_IF_NULL(req_p);

    if (req_p->num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS)
    {
      MSG_ERROR("Client FCN # 0x%x > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS, cap at GSDI #define",
                 req_p->num_files, 0, 0);
      return GSDI_ERROR;
    }

    /* Next perform the scan
    ** Start by checking the global FCN List head Pointer to see if it is
    ** NULL
    */
    if ( refresh_fcn_notify_list_node.head_p == NULL )
    {
        return GSDI_NOT_FOUND;
    }

    /* The Head Pointer is set...go ahead and make the Node
    ** Pointer the Head Pointer
    */
    node_p = (gsdi_refresh_fcn_notify_node_type*)refresh_fcn_notify_list_node.head_p;

    while ( node_p != NULL  )
    {
      /* First Check the Callback to see if we are registering
      ** the same_callback and the number of files registered are the
      ** same.
      */
      l_callback = node_p->fcn_notify_data.callback;

      if ( l_callback == req_p->reg_file_notify_cb )
      {
        /* This will overwrite the current registration with the new
        ** new registration found in this request.
        */
        node_p->fcn_notify_data.num_files = req_p->num_files;
        memcpy(node_p->fcn_notify_data.file_list,
               ((gsdi_refresh_registration_req_T *)req_p)->file_list_p,
               sizeof(uim_items_enum_type) * req_p->num_files);
        return GSDI_SUCCESS;
      }
      else
      {
        if (node_p->fcn_notify_data.num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS)
        {
          MSG_ERROR("Node_p FCN # 0x%x > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS, cap at GSDI #define",
                    node_p->fcn_notify_data.num_files, 0, 0);
          return GSDI_ERROR;
        }

        if (node_p->fcn_notify_data.num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS)
        {
          MSG_ERROR("Node_p FCN # 0x%x > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS, cap at GSDI #define",
                    node_p->fcn_notify_data.num_files, 0, 0);
          return GSDI_ERROR;
        }
      }
      node_p = node_p->next_p;
    }
    return GSDI_NOT_FOUND;
} /* gsdi_refresh_scan_link_list_for_dups */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_ADD_TO_REFRESH_FCN_LINK_LIST

DESCRIPTION
  Function called from gsdi_proc_handle_client_req to add the item to the
  linked list.  The first element of the link list is not a pointer to heap.
  The first element is a structure.  This will be used to add a file
  change notification request to the link list.

  If the item added to the link list is being added to the first time,
  it will be automatically to the head.  Each subsequent call to this
  function will get a pointer from head.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Added to Link List Successfully
                    GSDI_ERROR   - No Memory Heap

SIDE EFFECTS
  Allocates memory for a gsdi_refresh_fcn_notify_node_type
===========================================================================*/
static gsdi_returns_T gsdi_add_to_refresh_fcn_link_list(
    gsdi_refresh_registration_req_T * req_p
)
{
    gsdi_returns_T                      gsdi_status;
    mmgsdi_return_enum_type             mmgsdi_status;
    gsdi_refresh_fcn_notify_node_type * node_p  = NULL;
    mmgsdi_refresh_file_list_type       refresh_files = {0};
    uint8                               i,j            = 0;
    mmgsdi_option_type                  option        = {0};

    MSG_LOW("GSDI_ADD_TO_REFRESH_FCN_LINK_LIST",0,0,0);
    MMGSDI_RETURN_IF_NULL(req_p);

    if (req_p->num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS)
    {
      MSG_ERROR("Client FCN # 0x%x > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS, cap at GSDI #define",
                 req_p->num_files, 0, 0);
      return GSDI_ERROR;
    }

    /* Determine whether or not the client requesting registration has
    ** already registered for FCNs.  This will handle cases where a
    ** GSDI_SIM_INIT_COMPLETED Event is generated (i.e. REFRESH) and the
    ** client keys off of this to register again
    */

    gsdi_status = gsdi_refresh_scan_link_list_for_dups(req_p);

    if ( gsdi_status != GSDI_NOT_FOUND )
    {
      /* FCN Registration Request is either a duplicate (i.e.
      ** gsdi_status = GSDI_SUCCESS or there was a link list
      ** operation that failed in which case gsdi_status is
      ** as GSDI_ERROR;
      */
      return gsdi_status;
    }

    /* Determine whether this is the first add
    ** to the refresh voting list table
    */
    if ( refresh_fcn_notify_list_node.head_p == NULL )
    {
        /* Request Made will be added to
        ** the beggining of this linked list
        */
        refresh_fcn_notify_list_node.head_p = (void *)&refresh_fcn_notify_list_node;
        refresh_fcn_notify_list_node.tail_p = (void *)&refresh_fcn_notify_list_node;
        refresh_fcn_notify_list_node.next_p = NULL;
        refresh_fcn_notify_list_node.prev_p = NULL;

        refresh_fcn_notify_list_node.fcn_notify_data.callback =
          req_p->reg_file_notify_cb;
        refresh_fcn_notify_list_node.fcn_notify_data.num_files = req_p->num_files;
        memcpy(refresh_fcn_notify_list_node.fcn_notify_data.file_list,
              ((gsdi_refresh_registration_req_T *)req_p)->file_list_p,
              sizeof(uim_items_enum_type) * req_p->num_files);

    }
    else
    {
        gsdi_refresh_fcn_notify_node_type * refresh_fcn_next_node;
        /* Not the first time we add to
        ** link list, need to add as
        ** next element
        */

        /* First get memory */
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(refresh_fcn_next_node,
            sizeof(gsdi_refresh_fcn_notify_node_type), mmgsdi_status);
        if ((mmgsdi_status != MMGSDI_SUCCESS) ||
            (refresh_fcn_next_node == NULL))
        {
            MMGSDI_DEBUG_MSG_ERROR("FAILED TO GET LIST MEMORY",0,0,0);
            return GSDI_ERROR;
        }

        refresh_fcn_next_node->fcn_notify_data.callback  =
          req_p->reg_file_notify_cb;
        refresh_fcn_next_node->fcn_notify_data.num_files = req_p->num_files;
        memcpy(refresh_fcn_next_node->fcn_notify_data.file_list,
              req_p->file_list_p,
              sizeof(uim_items_enum_type) * req_p->num_files);

        gsdi_link_to_fcn_list_tail(&refresh_fcn_notify_list_node,
                                    refresh_fcn_next_node);
        node_p = &(refresh_fcn_notify_list_node);

        while ( node_p != NULL )
        {
          MSG_HIGH("Callback funct ptr is %x",
                   node_p->fcn_notify_data.callback,0,0);
          node_p         = node_p->next_p;
        }
    }
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(refresh_files.file_list_ptr,
      req_p->num_files * sizeof(mmgsdi_file_enum_type),
      mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }

    j=0;
    for(i=0;i<req_p->num_files;i++)
    {
      mmgsdi_status =mmgsdi_util_convert_from_uim_items_enum(req_p->file_list_p[i],
                                                             &refresh_files.file_list_ptr[j]);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        j++;
      }
    }
    refresh_files.num_files = j;
    mmgsdi_status = mmgsdi_register_for_refresh(gsdi_data.client_id,
                                                MMGSDI_SLOT_1,
                                                refresh_files,
                                                TRUE,
                                                option,
                                                mmgsdi_util_internal_cb,
                                                0);
    if(MMGSDI_SUCCESS != mmgsdi_status)
    {
      MSG_ERROR("mmgsdi_register_for_refresh is failed for fcn link list",
               0, 0, 0);
    }

    if(MMGSDI_SUCCESS != mmgsdi_util_convert_to_gsdi_status(mmgsdi_status,
                                                            &gsdi_status))
    {
      MSG_ERROR("mmgsdi status 0x%x convert to gsdi status failed",
                 mmgsdi_status,0,0);
      gsdi_status = GSDI_ERROR;
    }
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    return gsdi_status;

} /* gsdi_add_to_refresh_fcn_link_list */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CLIENT_COMPLETED_REFRESH

DESCRIPTION
  Function is called by gsdi_proc_handle_client_req when a client notifies
  GSDI that it has completed reading the REFRESH Files.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Processed Completed Refresh Request
                    GSDI_ERROR   - Could not get Heap

SIDE EFFECTS
  Can Trigger a REFRESH Completed Terminal Response to GSTK
===========================================================================*/
static gsdi_returns_T gsdi_client_completed_refresh(
    gsdi_refresh_complete_req_T * req_p )
{
    gsdi_returns_T          gsdi_status               = GSDI_SUCCESS;
    mmgsdi_return_enum_type mmgsdi_status             = MMGSDI_ERROR;
    boolean                 notifications_outstanding = FALSE;
    mmgsdi_option_type      option                    = {0};
    boolean                 pass_fail                 = FALSE;
    gsdi_async_cb           refresh_complete_client_cb;

    MSG_LOW("GSDI_CLIENT_COMPLETED_REFRESH",0,0,0);

    gsdi_data.cache_ready = TRUE;

    if ( req_p == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("COMPLETE REFRESH NULL PTR: %x",req_p,0,0);
        return GSDI_ERROR;
    }

    MSG_HIGH("PASS OR FAIL    %x", req_p->pass_fail,0,0);
    MSG_HIGH("NUMBER OF FILES %x", req_p->num_files,0,0);

    /* save this callback since in the function gsdi_send_refresh_cnf,
       the previously saved async callback from the original refresh
       command will be assigned to the req_p */
    refresh_complete_client_cb = req_p->message_header.gsdi_async_callback;

    if (req_p->refresh_mode == GSDI_FILE_CHANGE_NOTIFICATION)
    {
      gsdi_status = gsdi_util_clear_client_notify(req_p->pass_fail,
                                                  req_p->num_files,
                                                  req_p->file_list_p,
                                                  &pass_fail);
      MSG_HIGH("status from gsdi_util_clear_client_notify() is %d",gsdi_status,0,0);

      if ( gsdi_status == GSDI_SUCCESS )
      {
          /* Scan to see if it is ok to generat a terminal
          ** response message to GSTK via callback function
          ** provided in gsdi_sim_refresh()
          */
          notifications_outstanding = gsdi_util_notifications_still_outstanding();

          MSG_HIGH("notification outstanding  is %d",notifications_outstanding ,0,0);

          if ( notifications_outstanding == FALSE )
          {
              /* Use Callback Saved in Global Data
              ** Structure gsdi_data.gstk_data to
              ** send a gsdi_cnf
              */

              mmgsdi_status = mmgsdi_refresh_complete(gsdi_data.client_id,
                                                      MMGSDI_SLOT_1,
                                                      req_p->pass_fail,
                                                      option,
                                                      mmgsdi_util_internal_cb,
                                                      0);
              if (MMGSDI_SUCCESS != mmgsdi_status)
              {
                MSG_ERROR("Unable to send Refresh Complete 0x%x", mmgsdi_status,
                           0, 0);
              }
              if(MMGSDI_SUCCESS != mmgsdi_util_convert_to_gsdi_status(mmgsdi_status,
                                                                      &gsdi_status))
              {
                MSG_ERROR("mmgsdi status 0x%x convert to gsdi status failed",
                           mmgsdi_status,0,0);
                gsdi_status = GSDI_ERROR;
              }

          } /* notifications_outstanding */
      } /* gsdi_status */
      else
      {
          MMGSDI_DEBUG_MSG_ERROR("Could not send gstk refresh status",0,0,0);
      }
    }
    /* Reset async callback back to the refresh complete async callback
       so that we can notify clients correctly */
    req_p->message_header.gsdi_async_callback = refresh_complete_client_cb;
    return gsdi_status;
}/* gsdi_client_completed_refresh */


/*===========================================================================
FUNCTION GSDI_PERFORM_POST_PIN_FUNCTION

DESCRIPTION
  This function is used to perform post pin functions.

DEPENDENCIES

RETURN VALUE
 gsdi_status

SIDE EFFECTS

===========================================================================*/
static gsdi_returns_T gsdi_perform_post_pin_function(
  gsdi_slot_id_type                     slot,
  int32                               * returned_data_len_ptr,
  byte                                * data_ptr,
  const gsdi_update_call_stack_req_T  * req_ptr,
  boolean                             *pass_fail_ptr
)
{
  gsdi_sim_refresh_req_T  sim_req;

  gsdi_returns_T     gsdi_status     = GSDI_SUCCESS;
  boolean           *cache_ready_ptr = NULL;
  gsdi_sim_events_T  sim_err_event   = GSDI_NO_SIM_EVENT;

  if(pass_fail_ptr == NULL)
  {
    MSG_ERROR("pass_fait_ptr is NULL",0,0,0);
    return GSDI_ERROR;
  }

  *pass_fail_ptr = FALSE;

  if(slot == GSDI_SLOT_1)
  {
     /* reset global variable so that get sim capability can be re-run if needed */
     gsdi_data.just_invalidated      = TRUE;
     gsdi_data.just_rehabilitated    = TRUE;
     gsdi_data.sim_present_and_ready = FALSE;
     gsdi_data.init_completed        = FALSE;
     gsdi_data.illegal_sim           = FALSE;
     cache_ready_ptr                 = &gsdi_data.cache_ready;
     sim_err_event                   = GSDI_CARD_ERROR;
     (void) gsdi_util_send_mmi_sim_ind(GSDI_REFRESH_SIM_INIT);

  }//GSDI_SLOT_1

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if (slot == GSDI_SLOT_2)
  {
     /* reset global variable so that get sim capability can be re-run if needed */
     gsdi_data.just_invalidated_2      = TRUE;
     gsdi_data.just_rehabilitated_2    = TRUE;
     gsdi_data.sim_present_and_ready_2 = FALSE;
     gsdi_data.init_completed_2        = FALSE;
     gsdi_data.illegal_sim_2           = FALSE;
     sim_err_event                     = GSDI_CARD_ERROR_2;
     cache_ready_ptr                   = &gsdi_data.cache_ready_2;
     (void) gsdi_util_send_mmi_sim_ind(GSDI_REFRESH_SIM_INIT_2);
  } // slot 2
#endif
  else
  {
    MMGSDI_DEBUG_MSG_ERROR("Wrong Slot: %d",slot,0,0);
    return GSDI_ERROR;
  } //error, neither slot1 or slot2

  /* Start the perso procedures */
  gsdi_util_set_sim_present_and_ready(slot,FALSE);

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perform_post_pin1_perso_refresh_if_needed(slot, data_ptr);

  /* Continue with Intialization procedures which
     require a PIN to be verified. */
  gsdi_status = gsdi_post_pin1_init_perso(slot,
                                          returned_data_len_ptr,
                                          (uint8 *)data_ptr);

  if ( gsdi_status == GSDI_ERROR )
  {
     memcpy(&sim_req.message_header, &req_ptr->message_header,
            sizeof(req_ptr->message_header));

    /* Build refresh complete req*/
    mmgsdi_refresh_send_complete(MMGSDI_SLOT_1, FALSE);
    MMGSDI_DEBUG_MSG_ERROR("PERSO POST PIN1 INTI FAILURE",0,0,0);
    return gsdi_status;
  } /* post pin1_init_perso failure*/
#endif

  if ( gsdi_is_perso_completed() )
  {
    (void)gsdi_hzi_init();
    gsdi_data.gstk_data.gsdi_refresh_fcn_data.perso_op_done = TRUE;

    gsdi_status = mmgsdi_proc_post_pin1_init(slot,
                                             returned_data_len_ptr,
                                             data_ptr);
    if ( gsdi_status == GSDI_SUCCESS )
    {
     *cache_ready_ptr = TRUE;

      MSG_HIGH("POST PIN1 INIT SUCCESS for slot 1",0,0,0);
      gsdi_sim_init_completed(slot);
     *pass_fail_ptr = TRUE;
    }/* post_pin1_init succesful*/

    else
    {
      MMGSDI_DEBUG_MSG_ERROR("POST PIN1 INIT FAILED for slot 1",0,0,0);
     *pass_fail_ptr = FALSE;
      memcpy(&sim_req.message_header, &req_ptr->message_header,
            sizeof(req_ptr->message_header));

      /* Build refresh complete req*/
      mmgsdi_refresh_send_complete(MMGSDI_SLOT_1, FALSE);
      (void)gsdi_util_send_mmi_sim_ind(sim_err_event);
      return GSDI_ERROR;
    } /*  post pin init failure */
  } /* perso completed*/
  return gsdi_status;
} /* gsdi_perform_post_pin_function */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_PROC_UPDATE_CALL_STACK

DESCRIPTION
  This function is used by the GSDI Task.  It is used to take down the Call
  Stack, recache the necessary files, and then bring the call stack back
  up.

DEPENDENCIES
  gsdi_util_can_gsdi_take_down_stack - Will query the clients of MMGSDI to
  determine whether or not GSDI Can take down the call stack.

RETURN VALUE
  None

SIDE EFFECTS
  Takes down and brings back up the UMTS/GSM Call Stacks
===========================================================================*/
static gsdi_returns_T gsdi_proc_update_call_stack(
  const gsdi_update_call_stack_req_T * req_p,
  int32                              * returned_data_len_p,
  byte                               * data_p
)
{
    boolean                     take_down_call_stack_allowed = TRUE;
    gsdi_returns_T              gsdi_status                  = GSDI_SUCCESS;
    boolean                     pass_fail                    = TRUE;
    gsdi_slot_id_type           slot                         = GSDI_SLOT_NONE;

   MMGSDI_RETURN_IF_NULL(data_p);
   MMGSDI_RETURN_IF_NULL(req_p);
    slot = req_p->message_header.slot;

    gsdi_util_can_gsdi_take_down_stack(&refresh_vote_list_node,
                                       &take_down_call_stack_allowed);
    if( take_down_call_stack_allowed == TRUE )
    {
        /* Proceed with procedures */
        /* check for operational slot for GSM/UMTS */
        if (((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_1_MASK) ==
              GSDI_GSM_SLOT_1_MASK) ||
            ((gsdi_data.card_applications.operational_slot & GSDI_USIM_SLOT_1_MASK) ==
              GSDI_USIM_SLOT_1_MASK))
        {
           gsdi_status =  gsdi_perform_post_pin_function(slot,
                                                          returned_data_len_p,
                                                          data_p,
                                                          req_p,
                                                          &pass_fail );
        }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        else if (((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_2_MASK) ==
                     GSDI_GSM_SLOT_2_MASK) ||
                   ((gsdi_data.card_applications.operational_slot & GSDI_USIM_SLOT_2_MASK) ==
                     GSDI_USIM_SLOT_2_MASK))
        {
          gsdi_status =  gsdi_perform_post_pin_function(slot,
                                                        returned_data_len_p,
                                                        data_p,
                                                        req_p
                                                        &pass_fail );
        }
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

        else
        {
          return GSDI_ERROR;
        }
        if(gsdi_status != GSDI_SUCCESS)
        {
          MMGSDI_DEBUG_MSG_ERROR("Post pin functions failed",0,0,0);
          return gsdi_status;
        }
        if(gsdi_data.gstk_data.gsdi_refresh_fcn_data.perso_op_done == TRUE)
        {
          mmgsdi_refresh_send_complete(MMGSDI_SLOT_1, pass_fail);
        }
        /* perso_op_done is not TRUE*/
        else
        {
          //copy the file no & data to the global data
          gsdi_data.gstk_data.gsdi_refresh_fcn_data.num_files = req_p->num_files;
          memcpy(gsdi_data.gstk_data.gsdi_refresh_fcn_data.file_list_p,
                 req_p->file_list_p,
                 (sizeof(uim_items_enum_type) * req_p->num_files));
        }
    }
   return GSDI_SUCCESS;
} /* gsdi_proc_update_call_stack */
#endif /* FEATURE_GSTK */


#if defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA)
/*===========================================================================
FUNCTION GSDI_INIT_ECC_CACHE

DESCRIPTION
  This function is used generically to populate the ECC Global Data Structures
  maintained by GSDI.  It is based on the slot input parameter and populates the
  corresponding global structure: sim_ecc_data or sim2_ecc_data

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
    GSDI_SUCCESS - GSDI was able to do the following:
                   1.  Successfully Read and Cache ECCs.
                   2.  Failure to read Optional ECCs will return SUCCESS
                       by structures will remain empty.
    GSDI_ERROR   - GSDI function failed due to:
                   1.  Could not Allocate memory for the read_req
                   2.  Request for Phase Failed

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_init_ecc_cache(
    gsdi_slot_id_type      slot,
    mmgsdi_file_enum_type  mmgsdi_file_name,
    gsdi_ef_ecc_data_T   * sim_ecc_p,
    const byte           * data_p)
{
    mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_data_type             mmgsdi_data_buf;

    memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));

    /* check for input parameters */
    if ( ( sim_ecc_p == NULL ) ||
         ( data_p    == NULL ) )
    {
        return GSDI_ERROR;
    }

    MSG_MED("Reading GSM EF-ECC",0,0,0);

    /* performed the ECC read */
    mmgsdi_status = mmgsdi_init_cache_binary(slot, mmgsdi_file_name, &mmgsdi_data_buf);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
        /* Careful not to overshoot ECC */
        /* Array boundaries             */
      if ( mmgsdi_data_buf.data_len > GSDI_MAX_ECC_LEN )
      {
        sim_ecc_p->len = GSDI_MAX_ECC_LEN;
      }
      else
      {
        sim_ecc_p->len = int32touint8(mmgsdi_data_buf.data_len);
      }

      /*
      ** Populate the ecc_data_T Structure Variable
      ** sim_ecc_data with ECC Info
      */
      sim_ecc_p->ef_exists   = TRUE;;
      sim_ecc_p->num_of_recs = 1;

      memcpy( &sim_ecc_p->data[0][0],
        mmgsdi_data_buf.data_ptr,
        sim_ecc_p->len);
    }
    else
    {
        MSG_HIGH("ECC Not in SIM. NOT a GSDI_ERROR",0,0,0);
        sim_ecc_p->ef_exists = FALSE;
    }

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_data_buf.data_ptr);

    return GSDI_SUCCESS;
} /* gsdi_init_ecc_cached */
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_CDMA */

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_CDMA

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_CARD_MODE_CAPABILITY_RUIM

DESCRIPTION
  Will be used to get the card Mode Capabilities for the Card Inserted.
  Called by gsdi_get_card_mode_capability if FEATURE_MMGSDI_CDMA is
  defined.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
LOCAL gsdi_returns_T gsdi_get_card_mode_capability_ruim(
  gsdi_uim_link_established_T *req_p,
  byte*                        data_p
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  gsdi_slot_id_type   slot;

  MMGSDI_RETURN_IF_NULL(req_p);

  /* Set the slot for this function */
  slot = req_p->message_header.slot;

  /* Read and Cache RUIM ID */
  MSG_MED("Reading CDMA RUIM ID",0,0,0);
  mmgsdi_status = mmgsdi_init_cache_binary( slot, MMGSDI_CDMA_RUIM_ID, NULL);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    return GSDI_ERROR;
  }

  /* Get the Emergency Call Codes         */
  /* Optional File - Initialization will  */
  /* not fail if EF is not present        */

  /* attempt to get both slot */

  /* slot 1 is present and has CDMA app */
  if (((gsdi_data.card_applications.slot1_present) &&
        (gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK) == GSDI_RUIM_APP_MASK))
  {
      (void)gsdi_init_ecc_cache( GSDI_SLOT_1,
                                 MMGSDI_CDMA_ECC,
                                 &ruim_ecc_data,
                                 data_p);
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* slot 2 is present and has CDMA app */
  if (((gsdi_data.card_applications.slot2_present) &&
        (gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK) == GSDI_RUIM_APP_MASK))
  {
      (void)gsdi_init_ecc_cache( GSDI_SLOT_2,
                                 MMGSDI_CDMA_ECC,
                                 &ruim2_ecc_data,
                                 data_p);
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

  /* Get the Language Preference Information */
  /* Mandatory File - Initialization will    */
  /* fail if EF is not present               */
  MSG_MED("Reading UIM_CDMA_PREF_LANG",0,0,0);
  mmgsdi_status = mmgsdi_init_cache_binary( slot, MMGSDI_CDMA_PREF_LANG, NULL);

  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
      ERR("Could not Read UIM_CDMA_PREF_LANG",0,0,0);
      return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_CDMA */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_CARD_MODE_CAPABILITY

DESCRIPTION
  Determine whether the card inserted contains a USIM ADF or GSM DF.  For
  UMTS, a GSM DF can be used if a USIM ADF is not available.

DEPENDENCIES
  Received link established message from UIM server.  All dependent on the
  features for UMTS, GSM and/or CDMA

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_returns_T gsdi_get_card_mode_capability(
    gsdi_uim_link_established_T *req_p,
    byte*                        data_p
)
{
    gsdi_returns_T        gsdi_status    = GSDI_SUCCESS;
    mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
    uint8               *apps_enabled_p  = NULL_PTR;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
    uint32               ret_data_len    = 0;
    mmgsdi_data_type     elp_data;

    memset(&elp_data, 0x00, sizeof(mmgsdi_data_type));


    MMGSDI_RETURN_IF_NULL(req_p);
    if(gsdi_data.protocol == GSDI_PROTOCOL_GSM)
    {
        /* Check the Apps Enabled to call the correct
        ** CDMA or GSM function is called
        */
        mmgsdi_status = mmgsdi_init_cache_binary(GSDI_SLOT_1, MMGSDI_ELP, &elp_data);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR("Unknown gsdi_data.protocol 0x%x",gsdi_data.protocol,0,0);
    }

    /* If gsdi_status is a SUCCESS, cache information in the gsdi_common_cache */
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      gsdi_status = gsdi_util_write_common_cache ( UIM_ELP,
                                                    TRUE,
                                                    uint32touint8(ret_data_len), /*data_len*/
                                                    0,            /* data_offset */
                                                    elp_data.data_ptr,
                                                    GSDI_DATA_FROM_CARD);
    }
    else if (mmgsdi_status == MMGSDI_NOT_SUPPORTED)
    {
      gsdi_status = GSDI_NOT_SUPPORTED;
    }
    else if (mmgsdi_status == MMGSDI_NOT_FOUND)
    {
      gsdi_status = GSDI_NOT_FOUND;
    }
    else
    {
      gsdi_status = GSDI_ERROR;
    }
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(elp_data.data_ptr);

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif  /* FEATURE_MMGSDI_UMTS */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_CDMA
    /* Need To Determine Operational Slot for CDMA  */
    /* No need to get CDMA Card Mode Capabilities if */
    /* CDMA Is not Present                                                  */
    if ( ( ( gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_1_MASK ) ==
             GSDI_CDMA_SLOT_1_MASK ) &&
          ( ( gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK ) ==
              GSDI_RUIM_APP_MASK ) )
    {
        req_p->message_header.slot = GSDI_SLOT_1;
        apps_enabled_p = &gsdi_data.card_applications.apps_enabled;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    else if ( ( ( gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_2_MASK ) ==
                  GSDI_CDMA_SLOT_2_MASK ) &&
              ( ( gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK ) ==
                  GSDI_RUIM_APP_MASK ) )
    {
        req_p->message_header.slot = GSDI_SLOT_2;
        apps_enabled_p = &gsdi_data.card_applications.apps2_enabled;
    }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    else
    {
        req_p->message_header.slot = GSDI_SLOT_NONE;
        gsdi_status = GSDI_NOT_SUPPORTED;
    }

    if ( gsdi_status != GSDI_NOT_SUPPORTED )
    {
        gsdi_status = gsdi_get_card_mode_capability_ruim( req_p, data_p);
        if (gsdi_status != GSDI_SUCCESS)
        {
            /* Remove CDMA from the Apps Enabled BITMAP */
            if (apps_enabled_p == NULL) {
              gsdi_status = GSDI_ERROR;
            }
            else
            {
                *apps_enabled_p &= ~GSDI_RUIM_APP_MASK;

                switch ( req_p->message_header.slot )
                {
                case GSDI_SLOT_1:
                  gsdi_data.card_applications.operational_slot &= ~GSDI_CDMA_SLOT_1_MASK;
                  break;
  #ifdef FEATURE_MMGSDI_DUAL_SLOT
                case GSDI_SLOT_2:
                  gsdi_data.card_applications.operational_slot &= ~GSDI_CDMA_SLOT_2_MASK;
                  break;
  #endif /* FEATURE_MMGSDI_DUAL_SLOT */
                default:
                  MMGSDI_DEBUG_MSG_ERROR("Incorrect slot: %x",req_p->message_header.slot,0,0);
                  break;
                }

                /* To fix an issue with non compliant cards where the CDMA DF
                ** is empty, we will reset the status to GSDI Success only if
                ** the apps_enabled_p != NULL && the apps_enabled_p has a valid
                ** application residing in it
                */
                if ( gsdi_status     == GSDI_NOT_FOUND  &&
                     *apps_enabled_p != 0x00 )
                {
                  gsdi_status = GSDI_SUCCESS;
                }
            }
        }
    }
    else
    {
        /* Need to reset status */
        gsdi_status = GSDI_SUCCESS;
    }

#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

#endif /* FEATURE_MMGSDI_CDMA */

    /* reset card mode capability if needed */
    if (gsdi_data.current_card_mode_capability == GSDI_CARD_GSM )
    {
      if ( (gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK ) != GSDI_GSM_SIM_MASK )
      {
        if ( (gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK ) == GSDI_RUIM_APP_MASK )
        {
          gsdi_data.current_card_mode_capability = GSDI_CARD_RUIM;
        }
        else
        {
          gsdi_data.current_card_mode_capability = GSDI_CARD_MODE_UNKNOWN;
        }
      }
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    /* reset card mode capability if needed */
    if (gsdi_data.current_card_mode_capability_2 == GSDI_CARD_GSM )
    {
      if ( (gsdi_data.card_applications.apps2_enabled & GSDI_GSM_SIM_MASK ) != GSDI_GSM_SIM_MASK )
      {
        if ( (gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK ) == GSDI_RUIM_APP_MASK )
        {
          gsdi_data.current_card_mode_capability_2 = GSDI_CARD_RUIM;
        }
        else
        {
          gsdi_data.current_card_mode_capability_2 = GSDI_CARD_MODE_UNKNOWN;
        }
      }
    }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    return gsdi_status;

} /* gsdi_get_card_mode_capability */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_REMOVE_APPS_FROM_APPS_ENABLED

DESCRIPTION
  When GSDI determines that application is invalid, it will call this function
  to remove it from the gsdi_data.card_applications Global Structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_remove_apps_from_apps_enabled(
  gsdi_slot_id_type slot,
  uint8             card_app
)
{
  uint8 * card_apps_p = NULL;

  switch ( slot )
  {
  case GSDI_SLOT_1:
    card_apps_p = &gsdi_data.card_applications.apps_enabled;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    card_apps_p = &gsdi_data.card_applications.apps2_enabled;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    break;
  }

  if ( card_apps_p == NULL )
  {
    return;
  }

  *card_apps_p = *card_apps_p & ~card_app;

}

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_PROC_ILLEGAL_SIM

DESCRIPTION
  Function is called by GSDI as a result of a task calling the Illegal SIM
  API

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void gsdi_proc_illegal_sim (
  const gsdi_illegal_sim_req_T * gsdi_illegal_sim_req
)
{
    gsdi_sim_events_T illegal_sim_evt;
    /*
    ** THIS FUNC IS VERY SIMPLE,
    ** JUST STORE THE illegal_sim FLAG
    */
    if(gsdi_illegal_sim_req == NULL)
    {
      MSG_ERROR("gsdi_illegal_sim_req is NULL",0,0,0);
      return;
    }

    switch ( gsdi_illegal_sim_req->message_header.slot )
    {
      case GSDI_SLOT_1:
        gsdi_data.illegal_sim = gsdi_illegal_sim_req->illegal_sim;
        illegal_sim_evt       = GSDI_SIM_ILLEGAL;
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
        gsdi_data.illegal_sim_2 = gsdi_illegal_sim_req->illegal_sim;
        illegal_sim_evt       = GSDI_SIM_ILLEGAL_2;
        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

      default:
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID %x",gsdi_illegal_sim_req->message_header.slot,0,0);
        return;
    }

    MSG_HIGH("SIM MARKED AS ILLEGAL BY REQUEST",0,0,0);

    /* Send the appropriate notification */
    if(gsdi_util_send_mmi_sim_ind( illegal_sim_evt ) != GSDI_SUCCESS)
        MSG_HIGH("Not able to send warning %d", illegal_sim_evt, 0, 0);
    else
        MSG_HIGH("Warning indication %d",illegal_sim_evt,0,0);

} /* gsdi_proc_illegal_sim */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_UIM_SLOT_ERROR_CONDITION

DESCRIPTION
  Function is called by UIM Server to report an Error. This will signal GSDI
  to process an Error.  Errors generated by the UIM are ignored when running
  in Virtual SIM Mode.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  Triggers GSDI to signal itself to generate a GSDI_CARD_ERROR Event to
  the rest of the ME
===========================================================================*/
void gsdi_uim_slot_error_condition (
  uim_status_type   error_condition,
  uim_slot_type     slot
)
{
    gsdi_notified_error_req_T * msg = NULL_PTR;
    mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg,
              sizeof(gsdi_notified_error_req_T), mmgsdi_status);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
        ERR_FATAL("NO HEAP SPACE...",0,0,0);
    }

    /* Start populating the error message */
    /* Message Header                     */
    msg->message_header.message_set = MS_UIM_SERVER;

    switch ( slot )
    {
        case UIM_SLOT_1:
            msg->message_header.message_id  = GSDI_ERROR_CONDITION;
            gsdi_data.uim_error_condition   = error_condition;
            break;

        case UIM_SLOT_2:
            msg->message_header.message_id  = GSDI_ERROR_CONDITION_2;
            gsdi_data.uim_error_condition_2 = error_condition;
            break;

        default:
            MMGSDI_DEBUG_MSG_ERROR("UNKNOWN UIM SLOT %x",slot,0,0);
            return;
    }

#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))
    ((IMH_T *)(&msg.message_header))->message_len_msb =
      (unsigned char)((sizeof( gsdi_notified_error_req_T ) -
      sizeof(gsdi_cmdhdr_T)) / 0x100);
    ((IMH_T *)(&msg.message_header))->message_len_lsb =
      (unsigned char)((sizeof( gsdi_notified_error_req_T ) -
      sizeof(gsdi_cmdhdr_T)) % 0x100);
#else
    PUT_IMH_LEN( sizeof( gsdi_notified_error_req_T ) -
      sizeof(gsdi_cmdhdr_T), &msg->message_header );
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))*/
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.slot = slot;
    msg->error_condition     = error_condition;

    (void)gsdi_cmd_ext(msg);

    MMGSDIUTIL_TMC_MEM_FREE(msg);

} /* gsdi_uim_slot_error_condition */
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UIM_ERROR_CONDITION

DESCRIPTION
  Function is called by UIM Server to report an Error. This will signal GSDI
  to process an Error.  Errors generated by the UIM are ignored when running
  in Virtual SIM Mode.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  Triggers GSDI to signal itself to generate a GSDI_CARD_ERROR Event to
  the rest of the ME
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gsdi_uim_error_condition (
  uim_status_type error_condition
)
{
        MSG_HIGH("GSDI Notified by UIM: %x",error_condition,0,0);

    switch (error_condition) {
        case UIM_POLL_ERR_S:
            /* This is a faulty condition from the UIM server.
                Error is generated when the UIM detects a Polling
                Error.  Execute same procedures in UIM_ERR_S to handle
                this error.
              */
        case UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S:
        case UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S:
        case UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S:
        case UIM_PPS_TIMED_OUT_MAX_TIMES_S:
        case UIM_VOLTAGE_MISMATCH_S:
        case UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S:
        case UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S:
        case UIM_MAXED_PARITY_ERROR_S:
        case UIM_MAXED_RX_BREAK_ERROR_S:
        case UIM_MAXED_OVERRUN_ERROR_S:
        case UIM_TRANSACTION_TIMER_EXPIRED_S:
        case UIM_POWER_DOWN_CMD_NOTIFICATION_S:
        case UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S:
        case UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S:
        case UIM_MAX_PARITY_IN_PASSIVE_S:
        case UIM_MAX_RXBRK_IN_PASSIVE_S:
        case UIM_MAX_OVERRUN_IN_PASSIVE_S:
        case UIM_ERR_S:
            /* This is a faulty condition from the UIM server.
                Send an error signal to the gsdi task which will notify
                UI.
            */
            gsdi_data.uim_error_condition = error_condition;
            (void)rex_set_sigs(&gsdi_task_tcb, GSDI_UIM_ERROR_SIG);

            break;

        case UIM_INITIALIZED_S:
            /* Occasionally get this condition from UIM Server */

            /* Do Nothing. */
            break;

        default:
            /* An unexpected error_condition received. Raise an error.*/
            ERR("GSDI received invalid condition %d",error_condition,0,0);
            break;
    }


} /* gsdi_uim_error_condition */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION GSDI_UIM_NOTIFY_RESET

DESCRIPTION
  Function is called by UIM Server to report reset status of the card. This
  will signal GSDI to wait for a Link Established message in case of reset init
  or to proceed if reset complete.

INPUT
  reset_status - uninitialized/reset_complete

RETURN VALUE
  Void

OUTPUT
  Triggers GSDI to signal itself to go into waiting state if reset_status is
  uninitialized or to signal itself to proceed in case of the reset_status being
  initialized.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gsdi_uim_notify_reset(
  uim_status_type reset_status
)
{
  switch(reset_status)
  {
    case UIM_RESET_COMPLETE_S:
      (void)rex_set_sigs(&gsdi_task_tcb, GSDI_UIM_RESET_COMPLETE_SIG);
      break;

    case UIM_ERR_S:
      break;

    default:
      break;
  }
}/* gsdi_uim_notify_reset */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_EF_PROTOCOL

DESCRIPTION
  Function called to determine what protocol should be used for the requested
  file.

DEPENDENCIES
  None

RETURN VALUE
  uim_protocol_type

SIDE EFFECTS
  None
===========================================================================*/
LOCAL uim_protocol_type gsdi_get_EF_protocol (
  gsdi_slot_id_type   slot,
  uim_items_enum_type requested_EF
)
{
    gsdi_item_category_T EF_category;
    uim_protocol_type    protocol = UIM_NO_SUCH_MODE;
    gsdi_card_mode_capability_T  * capability_of_card_p = NULL_PTR;

    /* By shifting the EF enum by 8 bits, */
    /* gives us the category of the       */
    /* requested id. Where category is:   */
    /*     MF       = 0,                  */
    /*     CDMA     = 1,                  */
    /*     GSM      = 2,                  */
    /*     TELECOM  = 3,                  */
    /*     USIM     = 4                   */
    /*     ORANGE   = 7,                  */
    /*     CINGULAR = 8,                  */
    /*     HZI      = 0xA                 */
    EF_category = (gsdi_item_category_T)(((word)requested_EF >> 8) & 0x0F);

    /* Get the capability of the card     */
    /* itself (GSM DF, USIM ADF, etc..)   */
    switch ( slot )
    {
      case GSDI_SLOT_1:
        capability_of_card_p = &gsdi_data.current_card_mode_capability;
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
        capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
        break;

      case GSDI_SLOT_AUTOMATIC:
        if( gsdi_data.card_applications.slot1_present &&
            gsdi_data.card_applications.slot2_present)
        {
          switch (EF_category)
          {
            case GSM:
              if ((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_1_MASK) ==
                   GSDI_GSM_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_2_MASK) ==
                   GSDI_GSM_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }
              MMGSDI_DEBUG_MSG_ERROR("GSM Request fails when there is no GSM app", 0, 0, 0);
              break;

            case CDMA:
              if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_1_MASK) ==
                   GSDI_CDMA_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_2_MASK) ==
                   GSDI_CDMA_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }
              MMGSDI_DEBUG_MSG_ERROR("CDMA Request fails when there is no CDMA app", 0, 0, 0);
              break;

            case USIM:
              if ((gsdi_data.card_applications.operational_slot & GSDI_USIM_SLOT_1_MASK) ==
                   GSDI_USIM_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_USIM_SLOT_2_MASK) ==
                   GSDI_USIM_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }
              MMGSDI_DEBUG_MSG_ERROR("USIM Request fails when there is no USIM app", 0, 0, 0);
              break;

            case ROOT:
            case TELECOM:
            case BY_PATH:
              /* default to operational GSM! */
              if ((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_1_MASK) ==
                   GSDI_GSM_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_2_MASK) ==
                   GSDI_GSM_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }
              /* try CDMA now */
              if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_1_MASK) ==
                   GSDI_CDMA_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_2_MASK) ==
                   GSDI_CDMA_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }
              MMGSDI_DEBUG_MSG_ERROR("USIM Request fails when there is no USIM app", 0, 0, 0);
              break;

          case CINGULAR:
          case ORANGE:
          case HZI:
              if ((gsdi_data.card_applications.operational_slot & GSDI_USIM_SLOT_1_MASK) ==
                   GSDI_USIM_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_USIM_SLOT_2_MASK) ==
                   GSDI_USIM_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }

              /* USIM is not operational...try GSM */
              if ((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_1_MASK) ==
                   GSDI_GSM_SLOT_1_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability;
                  break;
              }
              if ((gsdi_data.card_applications.operational_slot & GSDI_GSM_SLOT_2_MASK) ==
                   GSDI_GSM_SLOT_2_MASK)
              {
                  capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
                  break;
              }
              MMGSDI_DEBUG_MSG_ERROR("GSM Request fails when there is no GSM app", 0, 0, 0);
              break;
          }

        }
        if (!gsdi_data.card_applications.slot1_present &&
            gsdi_data.card_applications.slot2_present)
        {
          capability_of_card_p = &gsdi_data.current_card_mode_capability_2;
        }
        if (gsdi_data.card_applications.slot1_present &&
            !gsdi_data.card_applications.slot2_present)
        {
          capability_of_card_p = &gsdi_data.current_card_mode_capability;
        }
        if (!gsdi_data.card_applications.slot1_present &&
            !gsdi_data.card_applications.slot2_present)
        {
          MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
        }
        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

      default:
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
        break;
    }

    /* Now validate the category of the   */
    /* EF against the capability of the   */
    /* card and the type of mobile we are */
    /* using.                             */
    if (capability_of_card_p == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("Null capability_of_card_p",0,0,0);
        return protocol;
    }
    if (*capability_of_card_p != GSDI_CARD_MODE_UNKNOWN) {

        gsdi_data.current_EF_category = EF_category;

        switch (EF_category) {

        case ROOT:      /* Root or MF directory on the card */
        case TELECOM:   /* Telecom DF area on the card      */

            /* UMTS supports using both a USIM    */
            /* or GSM based card.  USIM will      */
            /* always be used first if available. */
            /* Otherwise, GSM is used if it is    */
            /* available.                         */
            if ( ( *capability_of_card_p == GSDI_CARD_USIM ) ||
                 ( *capability_of_card_p == GSDI_CARD_GSM_USIM ) )
            {
                protocol = UIM_WCDMA;
            }
            else if ( ( *capability_of_card_p == GSDI_CARD_GSM ) ||
                      ( *capability_of_card_p == GSDI_CARD_GSM_RUIM ) )
            {
                protocol = UIM_GSM;
            }
            /* RUIM Card Has Support for Telecom DF */
            else if ( *capability_of_card_p == GSDI_CARD_RUIM )
            {
                protocol = UIM_CDMA;
            }
            else
            {
                ERR("Unknown capability_of_card %x",*capability_of_card_p,0,0);
            }

            break;

        case GSM:       /* GSM DF on the card     */

            /* UMTS supports using both USIM      */
            /* or GSM EFs.  Therefore, we must    */
            /* use the correct EF for such a case */
            if ( *capability_of_card_p == GSDI_CARD_GSM)
            {
                protocol = UIM_GSM;
            }
            else if ( *capability_of_card_p == GSDI_CARD_USIM )
            {
                protocol = UIM_WCDMA;
            }
            break;

        case USIM:      /* USIM ADF on the card   */
            if ( ( *capability_of_card_p == GSDI_CARD_USIM ) ||
                 ( *capability_of_card_p == GSDI_CARD_GSM_USIM ) )
            {
                protocol = UIM_WCDMA;
            }
            break;

        case BY_PATH:   /* USIM ADF or TELECOM DF for using PATH on USIM */

            if ( ( *capability_of_card_p == GSDI_CARD_USIM ) ||
                 ( *capability_of_card_p == GSDI_CARD_GSM_USIM ) )
            {
                protocol = UIM_WCDMA;
            }
            else if ( *capability_of_card_p == GSDI_CARD_GSM)
            {
                protocol = UIM_GSM;
            }
            else if ( *capability_of_card_p == GSDI_CARD_RUIM )
            {
                protocol = UIM_CDMA;
            }
            break;

        case CDMA:

            if ( *capability_of_card_p == GSDI_CARD_RUIM ||
                  /* For Dualmode CDMA/GSM DF Cards */
                 *capability_of_card_p == GSDI_CARD_GSM )
            {
                protocol = UIM_CDMA;
            }
            break;

        case CINGULAR:
        case ORANGE:
        case HZI:
            if ( *capability_of_card_p == GSDI_CARD_GSM )
            {
                protocol = UIM_GSM;
            }
            else if ( *capability_of_card_p == GSDI_CARD_USIM )
            {
                protocol = UIM_WCDMA;
            }
            else
            {
              protocol = UIM_NO_SUCH_MODE;
            }
            break;


        default:
            MMGSDI_DEBUG_MSG_ERROR("Invalid EF_catagory %d",EF_category,0,0);
            break;
        }
    }
    else
    {
        ERR("Invalid card mode",0,0,0);
    }

    return(protocol);

} /* gsdi_get_EF_protocol */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_IS_SIM_PRESENT_AND_READY

DESCRIPTION
  Function called by GSDI to determine whether or not the SIM is PRESENT
  and READY to accept requests.

  INPUT PARAMETERS:
  1.  gsdi_slot_id_type slot [MANDATORY]
      GSDI_SLOT_1
      GSDI_SLOT_2

DEPENDENCIES
  GSDI Global Data

  gsdi_data.card_applications.slot1_present
  gsdi_data.sim_present_and_ready

  If FEATURE_MMGSDI_DUAL_SLOT
  gsdi_data.card_applications.slot2_present
  gsdi_data.sim_present_and_ready_2

RETURN VALUE
  boolean
    TRUE:  Card is present and ready

SIDE EFFECTS
  None
============================================================================ */
LOCAL boolean gsdi_is_sim_present_and_ready(
  gsdi_slot_id_type slot
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  switch ( slot )
  {
    case GSDI_SLOT_1:
      if ( gsdi_data.card_applications.slot1_present )
          return gsdi_data.sim_present_and_ready;
      break;

    case GSDI_SLOT_2:
      if ( gsdi_data.card_applications.slot2_present )
          return gsdi_data.sim_present_and_ready_2;
      break;

    case GSDI_SLOT_AUTOMATIC:
      if ( !gsdi_data.card_applications.slot1_present &&
           !gsdi_data.card_applications.slot2_present )
          return FALSE;

      if ( gsdi_data.card_applications.slot1_present &&
           !gsdi_data.card_applications.slot2_present )
          return gsdi_data.sim_present_and_ready;

      if ( !gsdi_data.card_applications.slot1_present &&
           gsdi_data.card_applications.slot2_present )
          return gsdi_data.sim_present_and_ready_2;

      if ( gsdi_data.card_applications.slot1_present &&
           gsdi_data.card_applications.slot2_present )
          return (gsdi_data.sim_present_and_ready || gsdi_data.sim_present_and_ready_2);

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
      return FALSE;
  }
  return FALSE;
#else
  if ( slot != GSDI_SLOT_1 )
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);

  return gsdi_data.sim_present_and_ready;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi_is_sim_present_and_ready */

/*===========================================================================
FUNCTION   GSDI_HANDLE_ALL_PIN_INFO

DESCRIPTION
  Function is called to hanle the processing request to get all pin info
  request.  This function pulls out all the information from the
  gsdi_data global structure.

DEPENDENCIES
  None

RETURNS
  gsdi_returns_T

SIDE EFFECTS
  None

===========================================================================*/
static gsdi_returns_T gsdi_proc_handle_all_pin_info(
  gsdi_slot_id_type slot,
  gsdi_pin_info_item_T * pin_info_item
)
{
  boolean sim_present;

  MMGSDI_RETURN_IF_NULL(pin_info_item);

  sim_present = gsdi_is_sim_present_and_ready( slot );

  if ( (slot == GSDI_SLOT_1) &&  sim_present)
  {
    pin_info_item[0].pin_id      = GSDI_PIN1;
    pin_info_item[1].pin_id      = GSDI_PIN2;

    if(mmgsdi_util_convert_to_gsdi_pin_info(&pin_info_item[0])
       != MMGSDI_SUCCESS)
    {
       MSG_ERROR("Could not convert PIN1 status from MMGSDI",0,0,0);
       return GSDI_ERROR;
    }
    if (mmgsdi_util_convert_to_gsdi_pin_info(&pin_info_item[1])
        != MMGSDI_SUCCESS)
    {
       MSG_ERROR("Could not convert PIN2 status from MMGSDI",0,0,0);
       return GSDI_ERROR;
    }
    return GSDI_SUCCESS;
  }
  else
  {
    return GSDI_NOT_INIT;
  }
}


/* <EJECT> */
/*===========================================================================
FUNCTION gsdi_is_language_check

DESCRIPTION
  Function to check if the item being read is a EF which contains language
  information

  INPUT PARAMETERS:
    item  - A uim_items_enum_type

DEPENDENCIES


RETURN VALUE
  boolean
    TRUE: item is an EF which contains language information

SIDE EFFECTS
  None
============================================================================ */
LOCAL boolean gsdi_is_language_check(
  uim_items_enum_type item
)
{
  switch (item)
  {
  case UIM_USIM_LI:
  case UIM_GSM_LP:
  case UIM_ELP:
  case UIM_CDMA_PREF_LANG:
    return TRUE;
  default:
    return FALSE;
  }
} /* gsdi_is_language_check */


#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
/*===========================================================================
FUNCTION GSDI_PROC_SEND_APDU

DESCRIPTION
  Function is called by GSDI function:  gsdi_proc_handl_client_req to
  extract the relevant parameters to be passed to the GSDI - UIM Interface
  for additinal processing.

  This function is applicable to all 3 technolgies and combination of:
  1.  GSM SIM CARDS
  2.  UMTS UICC CARDS WITH USIM APP
  3.  CDMA RUIM CARDS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_returns_T  gsdi_proc_send_apdu(
    gsdi_send_apdu_req_T * req_p,
    uint8                * ret_data_len_p,
    uint8                * data_p
)
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;

    /* ----------------------------------------------------------------------
       Perform relevant parameter checking
       Verify Pointer passed in are not NULL.
       ---------------------------------------------------------------------- */
    if ( req_p == NULL ||
         ret_data_len_p == NULL ||
         data_p == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL PARAMETERS: %x %x %x",req_p, ret_data_len_p, data_p);
        return GSDI_ERROR;
    }

    /* ----------------------------------------------------------------------
       Verify Parameters passed to function are valid
       1.  Valid APDU DATA BUFFER PTR
       2.  apdu_len is less than Max
       ---------------------------------------------------------------------- */
    if ( req_p->apdu_len > GSDI_MAX_DATA_BLOCK_LEN )
    {
        MMGSDI_DEBUG_MSG_ERROR("SEND APDU BAD REQ PARAMS",0,0,0);
        return GSDI_ERROR;
    }

    /* ----------------------------------------------------------------------
       Send relevant information to GSDI - UIM Interface
       a) slot_number
       b) APDU Length
       c) APDU Data
       d) Pointer to store returned DATA Len
       e) Pointer to store returned DATA
       ---------------------------------------------------------------------- */
    gsdi_status = gsdi_uim_server_send_apdu(req_p->message_header.slot,
                                            req_p->apdu_len,
                                            req_p->apdu_data,
                                            ret_data_len_p,
                                            data_p);

    return gsdi_status;

} /* gsdi_proc_send_apdu */

#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_IS_REFRESH_REQUIRED

DESCRIPTION
  Function is called to determine whether or not the GSDI has to perform a
  REFRESH as a result of changing the Slot information.

  Criteria for determing whether or not a REFRESH is performed depends on
  whether or not the UIM Operational Slot differs from the GSDI Operational
  Slot Information.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_returns_T gsdi_is_refresh_required(
  gsdi_app_enum_type app,
  gsdi_slot_id_type  slot,
  byte *             data_p
)
{
    uim_slot_type uim_slot = UIM_SLOT_NONE;
    uint32        new_mask = GSDI_SLOT_NO_APP_MASK;
    uint32        old_mask = GSDI_SLOT_NO_APP_MASK;

    /* initialized to refresh not required */
    data_p[0] = 0;

    /* ---------------------------------------------------------------------
       Map the gsdi_app_enum_type to a uim_app_type
       --------------------------------------------------------------------- */
    switch ( app )
    {
    case GSDI_GSM_APP:
        /* ASK UIM Where corresponding APP Is */
        uim_slot = uim_get_slot_for_app(UIM_GSM_APP);
        /* compare with existing operational slot */
        /* and Update Operational Slot Information for Higer Layers */
        switch ( uim_slot )
        {
        case UIM_SLOT_NONE:
            /* Jump out of function...no need to refresh */
            return GSDI_SUCCESS;

        case UIM_SLOT_1:
            new_mask = GSDI_GSM_SLOT_1_MASK;
            old_mask = GSDI_GSM_SLOT_2_MASK;
            break;

        case UIM_SLOT_2:
            new_mask = GSDI_GSM_SLOT_2_MASK;
            old_mask = GSDI_GSM_SLOT_1_MASK;
            break;

        default:
            MMGSDI_DEBUG_MSG_ERROR("NO APP FOR SLOTS AVAIL",0,0,0);
            break;
        }
        break;

    case GSDI_CDMA_APP:
        /* ASK UIM Where corresponding APP Is */
        uim_slot = uim_get_slot_for_app(UIM_CDMA_APP);
        /* Update Operational Slot Information for Higer Layers */
        switch ( uim_slot )
        {
        case UIM_SLOT_NONE:
            /* Jump out of function...no need to refresh */
            return GSDI_SUCCESS;

        case UIM_SLOT_1:
            new_mask = GSDI_CDMA_SLOT_1_MASK;
            old_mask = GSDI_CDMA_SLOT_2_MASK;
            break;

        case UIM_SLOT_2:
            new_mask = GSDI_CDMA_SLOT_2_MASK;
            old_mask = GSDI_CDMA_SLOT_1_MASK;
            break;

        default:
            MMGSDI_DEBUG_MSG_ERROR("NO APP FOR SLOTS AVAIL",0,0,0);
            break;
        }
        break;

    default:
        MMGSDI_DEBUG_MSG_ERROR("Bad app enum: %x for slot: %x", app,0,0);
        return GSDI_ERROR;
    }
    /* ---------------------------------------------------------------------
       Criteria for performing a REFRESH:
       Slot Information changes
       --------------------------------------------------------------------- */
    if ((gsdi_data.card_applications.operational_slot & new_mask) != new_mask) {
        /* needs to perform refresh */
        gsdi_data.card_applications.operational_slot &= ~old_mask;
        gsdi_data.card_applications.operational_slot |= new_mask;
        data_p[0] = 1;
    }
    return GSDI_SUCCESS;

}

/*===========================================================================
FUNCTION GSDI_PREF_SLOT_COMPLETED_CB

DESCRIPTION
  Function is a callback called by the GSDI Task to notify itself that
  the REFRESHING on Slot 2 has completed in some way:
  1.  GSDI_SUCCESS
  2.  GSDI_REFRESH_SUCCESS
  3.  GSDI_CAN_NOT_REFRESH
  4.  GSDI_CARD_ERROR
  5.  etc.


  This will allow for single slot operation regardless of f

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_pref_slot_completed_cb( gsdi_cnf_T *gsdi_cnf)
{
    gsdi_returns_T gsdi_status;

    if (gsdi_data.gstk_data.refreshing_slot_2 )
    {
      return;
    }
    gsdi_data.gstk_data.gsdi_refresh_status_slot2 =
              gsdi_cnf->message_header.gsdi_status;


    if  (gsdi_data.user_slot_prefs.user_switch_gsm_app_pref !=
          gsdi_data.user_slot_prefs.gsm_app_pref)
    {
      /* GSM slot pref different */
      if (gsdi_data.user_slot_prefs.user_switch_gsm_app_pref == GSDI_SLOT_1 )
      {
      /* Want to change to slot 1 */
        if  (gsdi_data.gstk_data.gsdi_refresh_status_slot1 == GSDI_REFRESH_SUCCESS)
        {
          /* update the pref slot */
          /* GSM */
          gsdi_util_update_nv_user_pref(GSDI_GSM_APP);
        }
      }
      else if (gsdi_data.user_slot_prefs.user_switch_gsm_app_pref == GSDI_SLOT_2 )
      {
        /* Want to change to slot 2 */
        if  (gsdi_data.gstk_data.gsdi_refresh_status_slot2 == GSDI_REFRESH_SUCCESS)
        {
          /* update the pref slot */
          /* GSM */
          gsdi_util_update_nv_user_pref(GSDI_GSM_APP);
        }
      }
      else
      {
        /* User select NONE */
        gsdi_util_update_nv_user_pref(GSDI_CDMA_APP);
      }
    }

    /* CDMA */
    if  (gsdi_data.user_slot_prefs.user_switch_cdma_app_pref !=
          gsdi_data.user_slot_prefs.cdma_app_pref)
    {
      /* CDMA slot pref different */
      if (gsdi_data.user_slot_prefs.user_switch_cdma_app_pref == GSDI_SLOT_1 )
      {
        /* Want to change to slot 1 */
        if  (gsdi_data.gstk_data.gsdi_refresh_status_slot1 == GSDI_REFRESH_SUCCESS)
        {
          /* update the pref slot */
          /* CDMA */
          gsdi_util_update_nv_user_pref(GSDI_CDMA_APP);
        }
      }
      else if (gsdi_data.user_slot_prefs.user_switch_cdma_app_pref == GSDI_SLOT_2 )
      {
        /* Want to change to slot 2 */
        if  (gsdi_data.gstk_data.gsdi_refresh_status_slot2 == GSDI_REFRESH_SUCCESS)
        {
          /* update the pref slot */
          /* CDMA */
          gsdi_util_update_nv_user_pref(GSDI_CDMA_APP);
        }
      }
      else
      {
        /* User select NONE */
        gsdi_util_update_nv_user_pref(GSDI_CDMA_APP);
      }
    }


    /* 1 of the refresh fails, need to revert the data */
    if (gsdi_data.user_slot_prefs.user_switch_gsm_app_pref !=
          gsdi_data.user_slot_prefs.gsm_app_pref)
    {
      /* User switched to original GSM slot */
      gsdi_data.user_slot_prefs.user_switch_gsm_app_pref = gsdi_data.user_slot_prefs.gsm_app_pref;
      gsdi_status = gsdi_uim_server_pref_slot_change(gsdi_data.user_slot_prefs.user_switch_gsm_app_pref,
                                                     GSDI_GSM_APP);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MMGSDI_DEBUG_MSG_ERROR("UIM Did not reverting accepted Slot Pref for GSM",0,0,0);
      }
    }
    if (gsdi_data.user_slot_prefs.user_switch_cdma_app_pref !=
        gsdi_data.user_slot_prefs.cdma_app_pref)
    {
      /* User switched to original CDMA slot */
      gsdi_data.user_slot_prefs.user_switch_cdma_app_pref = gsdi_data.user_slot_prefs.cdma_app_pref;
      gsdi_status = gsdi_uim_server_pref_slot_change(gsdi_data.user_slot_prefs.user_switch_cdma_app_pref,
                                                     GSDI_CDMA_APP);
      if (gsdi_status != GSDI_SUCCESS)
      {
        MMGSDI_DEBUG_MSG_ERROR("UIM Did not reverting accepted Slot Pref for CDMA",0,0,0);
      }
    }

    /* Send event to client to notify the end of the
       switch slot procedure.  */
    (void) gsdi_util_send_mmi_sim_ind ( GSDI_FINISH_SWITCH_SLOT );

} /* gsdi_pref_slot_completed_cb */

/*===========================================================================
FUNCTION GSDI_PREF_SLOT_COMPLETED_SLOT_1_REFRESH

DESCRIPTION
  Function is a callback called by the GSDI Task to notify itself that
  the REFRESHING on Slot 1 has completed in some way:
  1.  GSDI_SUCCESS
  2.  GSDI_REFRESH_SUCCESS
  3.  GSDI_CAN_NOT_REFRESH
  4.  GSDI_CARD_ERROR
  5.  etc.

  Basically, regardless of the status of Slot 1, we must continue with slot2.
  This will allow for single slot operation regardless of f

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void gsdi_pref_slot_completed_slot_1_refresh ( gsdi_cnf_T * gsdi_cnf )
{

  if ( !gsdi_data.gstk_data.refreshing_slot_1 )
  {
    /* save the status */
    gsdi_data.gstk_data.gsdi_refresh_status_slot1 =
        gsdi_cnf->message_header.gsdi_status;
    (void) gsdi2_sim_refresh( GSDI_SLOT_2,
                              GSDI_SIM_INIT,
                              0,
                              0,
                              NULL,
                              0,
                              gsdi_pref_slot_completed_cb);
  }
}

/*===========================================================================
FUNCTION GSDI_PROC_CHANGE_PREF_SLOT

DESCRIPTION
  Function is called by GSDI function:  gsdi_proc_handl_client_req to
  extract the relevant parameters to be passed to the GSDI - UIM Interface
  to change the preferred slot.

  This function is applicable to GSM/CDMA technolgies and combination of:
  1.  GSM SIM CARDS
  2.  CDMA RUIM CARDS
  3.  Combination Cards (GSM/CDMA)

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_returns_T gsdi_proc_change_pref_slot(
   gsdi_switch_slot_preference_req_T * req_p,
   byte * data_p
)
{
    gsdi_returns_T    gsdi_status = GSDI_SUCCESS;
    nv_item_type      gsdi_nv_data_buf;      /* NV data buffer */
    nv_stat_enum_type nv_status = NV_DONE_S; /* NV CMD Status */

    /* ---------------------------------------------------------------------
       Perform some parameter checking and extract relevant parameters
       Check Slot Information
       --------------------------------------------------------------------- */
    MMGSDI_RETURN_IF_NULL(req_p);

    if ( req_p->message_header.slot != GSDI_SLOT_NONE &&
         req_p->message_header.slot != GSDI_SLOT_1    &&
         req_p->message_header.slot != GSDI_SLOT_2 )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x", req_p->message_header.slot,0,0);
        return GSDI_ERROR;
    }
    /* ---------------------------------------------------------------------
       Map the GSDI Slot ID to the correct NV UIM Slot Type
       Default case will handle invalid APPs.
       Write to NV and check status.
       --------------------------------------------------------------------- */
    switch ( req_p->app )
    {
    case GSDI_CDMA_APP:
        switch ( req_p->message_header.slot )
        {
        case GSDI_SLOT_NONE:
            gsdi_nv_data_buf.uim_cdma_pref_slot = NV_UIM_SLOT_NONE;
            break;

        case GSDI_SLOT_1:
            gsdi_nv_data_buf.uim_cdma_pref_slot = NV_UIM_SLOT_1;
            break;

        case GSDI_SLOT_2:
            gsdi_nv_data_buf.uim_cdma_pref_slot = NV_UIM_SLOT_2;
            break;

        default:
            MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_p->message_header.slot,0,0);
            return GSDI_ERROR;
        }
        nv_status = gsdi_put_nv( NV_UIM_CDMA_PREF_SLOT_I, &gsdi_nv_data_buf );
        if (nv_status != NV_DONE_S)
        {
            MMGSDI_DEBUG_MSG_ERROR( "NV PUT FAIL: %x App %x", nv_status, req_p->app, 0 );
            return GSDI_ERROR;
        }
        /* update the gloabl variable */
        gsdi_data.user_slot_prefs.user_switch_cdma_app_pref = gsdi_nv_data_buf.uim_cdma_pref_slot;
        break;

    case GSDI_GSM_APP:
        switch ( req_p->message_header.slot )
        {
        case GSDI_SLOT_NONE:
            gsdi_nv_data_buf.uim_gsm_pref_slot = NV_UIM_SLOT_NONE;
            break;

        case GSDI_SLOT_1:
            gsdi_nv_data_buf.uim_gsm_pref_slot = NV_UIM_SLOT_1;
            break;

        case GSDI_SLOT_2:
            gsdi_nv_data_buf.uim_gsm_pref_slot = NV_UIM_SLOT_2;
            break;

        default:
            MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_p->message_header.slot,0,0);
            return GSDI_ERROR;
        }
        nv_status = gsdi_put_nv( NV_UIM_GSM_PREF_SLOT_I, &gsdi_nv_data_buf );
        if (nv_status != NV_DONE_S)
        {
            MMGSDI_DEBUG_MSG_ERROR( "NV PUT FAIL: %x App %x", nv_status, req_p->app, 0 );
            return GSDI_ERROR;
        }
        gsdi_data.user_slot_prefs.user_switch_gsm_app_pref = gsdi_nv_data_buf.uim_gsm_pref_slot;
        break;

    default:
        MMGSDI_DEBUG_MSG_ERROR("Invalid Application DF Provided: %x",req_p->app,0,0);
        return GSDI_ERROR;
    }


    /* ---------------------------------------------------------------------
      Notify UIM of Slot Preference Change
      --------------------------------------------------------------------- */
    gsdi_status = gsdi_uim_server_pref_slot_change(req_p->message_header.slot,
                                                   req_p->app);
    if ( gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("UIM Did not accept Slot Pref",0,0,0);
        return GSDI_ERROR;
    }

    gsdi_status = gsdi_is_refresh_required(req_p->app,
                                             req_p->message_header.slot,
                                             data_p);
    if ( gsdi_status == GSDI_SUCCESS )
    {
        if ( data_p[0] == 0x01 )
        {
            /* Send event o client to notify the beginning of the
               switch slot procedure.  2 refreshes are expected
               upon receiving this event */
            (void) gsdi_util_send_mmi_sim_ind ( GSDI_START_SWITCH_SLOT );

            gsdi_status = gsdi2_sim_refresh( GSDI_SLOT_1,
                                             GSDI_SIM_INIT,
                                             0,
                                             0,
                                             NULL,
                                             0,
                                             gsdi_pref_slot_completed_slot_1_refresh);
        }
        else
        {
            /* No refresh is required, set the slot for the app */
            gsdi_util_update_nv_user_pref(req_p->app);
        }
    }
    if ( gsdi_status != GSDI_SUCCESS )
    {
        return GSDI_ERROR;
    }
    return GSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_UIM_HOT_SWAP

/*===========================================================================

FUNCTION GSDI_PROCESS_CARD_SWAP

DESCRIPTION
  This function is called when a card inserted or removed.


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Card will be no longer valid when GSDI_CARD_REMOVED passed in.
  Card will be powered up and become valid when GSDI_CARD_INSERTED passed in.

===========================================================================*/
void gsdi_process_card_swap(gsdi_slot_id_type slot,
  gsdi_uim_swap_enum_type status )
{
  mmgsdi_aid_type          aid           = {0};
  mmgsdi_data_type         refresh_files = {0};
  mmgsdi_option_type       option        = {0};
  mmgsdi_slot_id_enum_type slot          = MMGSDI_SLOT_1;

  switch (status)
  {
  case GSDI_UIM_CARD_REMOVED:
    break;

  case GSDI_UIM_CARD_INSERTED:
    /* Card inserted call refresh with reset */
    gsdi_uim_card_inserted  = TRUE;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    slot = MMGSDI_SLOT_2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

    (void)mmgsdi_naa_refresh(mmgsdi_generic_data.client_id,
                             slot,
                             aid,
                             MMGSDI_REFRESH_RESET,
                             refresh_files,
                             0,
                             option,
                             mmgsdi_util_internal_cb,
                             0);

    break;

  default:
    break;
  }
}
#endif /* FEATURE_UIM_HOT_SWAP */


/*===========================================================================
FUNCTION gsdi_proc_activate_onchip_sim

DESCRIPTION
  Function is called by GSDI function:  gsdi_proc_handl_client_req when
  the GSDI_ACTIVATE_ONCHIP_SIM_REQ is called.

  This function is used to populate the GSDI Cache (SIM or USIM).

DEPENDENCIES
  Allows the GSDI Task to service the rest of the Call Stack.  Will trigger
  the initialization procedures using a Simulated SIM.

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_proc_activate_onchip_sim(
  gsdi_slot_id_type                      slot,
  const gsdi_activate_onchip_sim_req_T * req_p,
  const uint8 *                          data_p
)
{

  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* --------------------------------------------------------------------------
     Only available for Slot 1
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1 )
  {
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_p);
  MMGSDI_RETURN_IF_NULL(data_p);

  /* --------------------------------------------------------------------------
     Use the REFRESH SIM RESET Command to trigger the UI to get into
     the correct state so that a GSDI_SIM_INSERTED can be handled
     ------------------------------------------------------------------------*/
  MSG_HIGH("Sending out GSDI_REFRESH_SIM_RESET",0,0,0);
  (void)gsdi_util_send_mmi_sim_ind(GSDI_REFRESH_SIM_RESET);


  /* --------------------------------------------------------------------------
     Start by setting the GSDI Global Data
     ------------------------------------------------------------------------*/
  gsdi_data.cache_ready                         = GSDI_GSM_SIM_MASK;
  gsdi_data.card_atr.atr_len                    = 0x02;
  gsdi_data.card_atr.atr_buffer[0]              = 0x3B;
  gsdi_data.card_atr.atr_buffer[1]              = 0x00;
  gsdi_data.card_applications.slot1_present     = TRUE;
  gsdi_data.card_applications.apps_enabled      = GSDI_GSM_SIM_MASK;
  gsdi_data.card_applications.operational_slot  = (int)GSDI_SLOT_1;
  gsdi_data.current_card_mode_capability        = GSDI_CARD_GSM;
  gsdi_data.first_inst_class                    = (nv_uim_first_inst_class_type)NV_UIM_FIRST_INST_CLASS_GSM_SIM;
  gsdi_data.gsdi_gstk_tp_done                   = TRUE;
  gsdi_data.gsdi_state                          = GSDI_SIM_INIT_COMPLETED_S;
  gsdi_data.gsdi_uim_sanity_timer_value         = 0xFFFFFFFF;
  gsdi_data.init_completed                      = TRUE;

  #ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_data.perso_engine_state                  = GSDI_PERSO_ENG_INIT_COMPLETED_S;
  gsdi_data.perso_post_pin1_init                = TRUE;
  #endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
  mmgsdi_util_preset_retries(MMGSDI_SLOT_1);

  gsdi_data.protocol                            = GSDI_PROTOCOL_GSM;
  gsdi_data.cache_ready                         = TRUE;
  gsdi_data.onchip_sim_activated                = TRUE;

#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */
  /* --------------------------------------------------------------------------
     Now Notify the tasks that we have a SIM Inserted.
     With an immediate notification that the SIM Init has Completed.
     ------------------------------------------------------------------------*/
  if ( gsdi_status == GSDI_SUCCESS )
  {
    MSG_HIGH("GSDI_SIM_INSERTED",0,0,0);
    (void)gsdi_util_send_mmi_sim_ind(GSDI_SIM_INSERTED);

    MSG_HIGH("GSDI_SIM_INIT_COMPLETED",0,0,0);
    (void)gsdi_util_send_mmi_sim_ind(GSDI_SIM_INIT_COMPLETED);
  }
  /* --------------------------------------------------------------------------
     From this point on, the GSDI Task should go back to the Main Loop.
     Access to the ONCHIP SIM Cache shall succeed.  The Call Stacks won't know
     or need to know that this is occurring.
     ------------------------------------------------------------------------*/
  return gsdi_status;
}


/*===========================================================================
FUNCTION gsdi_proc_notify_client

DESCRIPTION
  Function is called by GSDI function:  gsdi_proc_handl_client_req to
  send the event notification to the client registered event callback functions.

  This function is applicable during initialization when the client registers
  the callback.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_proc_notify_client(
  const gsdi_notify_client_req_T *req
)
{
  uint32 cb_ptr_index;

  MMGSDI_RETURN_IF_NULL(req);

  cb_ptr_index = req->cb_table_index;
  if (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr == NULL) {
    MMGSDI_DEBUG_MSG_ERROR("Try to notify Null Event Cb", 0, 0, 0);
    return GSDI_ERROR;
  }

  if (!gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent)
  {
    switch(gsdi_data.gsdi_state)
    {
      case GSDI_NOT_INIT_S:
        /* do nothing */
        break;
      /* GSDI has already received some events, need to send
        * them to the client */
      case GSDI_CARD_ERROR_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_CARD_ERROR);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent = TRUE;
        break;
      case GSDI_SIM_INSERTED_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_SIM_INSERTED);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent = TRUE;
        break;
      case GSDI_SIM_INIT_COMPLETED_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_SIM_INIT_COMPLETED);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent = TRUE;
        break;
      case GSDI_SIM_INIT_COMPLETED_NO_PROV_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_SIM_INIT_COMPLETED_NO_PROV);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent = TRUE;
        break;
      case GSDI_NO_SIM_S:
      case GSDI_LINK_ESTABLISHED_S:
      case GSDI_PIN_VERIFIED_S:
      default:
        MMGSDI_DEBUG_MSG_ERROR("BAD GSDI STATE: %x",gsdi_data.gsdi_state,0,0);
        break;
    }
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT

  if (!gsdi_data.gsdi_callback_info[cb_ptr_index].event2_sent)
  {
    switch(gsdi_data.gsdi_state_2)
    {
      case GSDI_NOT_INIT_S:
        /* do nothing */
        break;
      /* GSDI has already received some events, need to send
        * them to the client */
      case GSDI_CARD_ERROR_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_CARD_ERROR_2);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event2_sent = TRUE;
        break;
      case GSDI_SIM_INSERTED_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_SIM_INSERTED_2);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event2_sent = TRUE;
        break;
      case GSDI_SIM_INIT_COMPLETED_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_SIM_INIT_COMPLETED_2);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event2_sent = TRUE;
        break;
      case GSDI_SIM_INIT_COMPLETED_NO_PROV_S:
        (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(GSDI_SIM_INIT_COMPLETED_NO_PROV_2);
        gsdi_data.gsdi_callback_info[cb_ptr_index].event2_sent = TRUE;
        break;
      case GSDI_NO_SIM_S:
      case GSDI_LINK_ESTABLISHED_S:
      case GSDI_PIN_VERIFIED_S:
      default:
        MMGSDI_DEBUG_MSG_ERROR("BAD GSDI STATE: %x",gsdi_data.gsdi_state,0,0);
        break;
    }
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_GET_SLOT_FROM_REQ

DESCRIPTION
  Function called to extract the slot from the req_p.  The behavior of this
  function is as follows:

  #ifndef FEATURE_MMGSDI_DUAL_SLOT
    Slot is extracted
    Slot is checked
    if slot is != GSDI_SLOT_1, the the req_p is modified to slot GSDI_SLOT_1.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_slot_id_type

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_slot_id_type gsdi_get_slot_from_req(
  void * req_p
)
{
  if ( req_p == NULL )
  {
    ERR_FATAL("NULL REQ PTR: %x",req_p,0,0);
  }
#ifndef FEATURE_MMGSDI_DUAL_SLOT
  /*
  ** Ensure that only GSDI_SLOT_1 is the
  ** value in the slot
  */
  if (((gsdi_cmdhdr_T *)req_p)->slot != GSDI_SLOT_1)
  {
    ((gsdi_cmdhdr_T *)req_p)->slot = GSDI_SLOT_1;
  }
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */
  return ((gsdi_cmdhdr_T *)req_p)->slot;
}
/*===========================================================================
FUNCTION GSDI_IS_SIM_IN_SLOT_AVAILABLE

DESCRIPTION
  Function is called to determine whether or not the SIM in the SLOT
  requested is available

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE  - SIM In Slot is available
            FALSE - SIM In Slot is not available

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean gsdi_is_sim_in_slot_available(
  gsdi_slot_id_type slot
)
{
  boolean sim_present = FALSE;

  switch ( slot )
  {
  case GSDI_SLOT_1:
    sim_present = gsdi_data.card_applications.slot1_present;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    sim_present = gsdi_data.card_applications.slot2_present;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x", slot,0,0);
    sim_present = FALSE;
    break;
  }
  return sim_present;
} /* gsdi_is_sim_in_slot_available */

/*===========================================================================
FUNCTION GSDI_PROC_SIM_ACCESS

DESCRIPTION
  get sim access data in the GSM/UMTS/RUIM card

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_proc_sim_access (
    gsdi_slot_id_type       slot,
    gsdi_sim_access_req_T * req_p,
    byte *                  rec_num_accessed_p,
    int32*                  returned_data_length,
    uint8 *                 data_p
)
{
    gsdi_returns_T       gsdi_status = GSDI_SUCCESS;
    uint8                num_records;
    gsdi_slot_id_type    new_slot;
    uim_protocol_type    EF_protocol;

    MMGSDI_RETURN_IF_NULL(req_p);
    MMGSDI_RETURN_IF_NULL(returned_data_length);
    MMGSDI_RETURN_IF_NULL(data_p);
    MMGSDI_RETURN_IF_NULL(rec_num_accessed_p);

    EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)req_p->sim_filename);

    req_p->path = NULL;

    switch(EF_protocol)
    {
#ifdef FEATURE_MMGSDI_CDMA
      case UIM_CDMA:
        (void) gsdi_util_is_slot_pref( req_p->message_header.slot,
                                       GSDI_CDMA,
                                       &new_slot);

       /* update the slot in case the original slot was AUTOMATIC */
       req_p->message_header.slot = new_slot;

       /* Read the EF */
        gsdi_status = gsdi_uim_server_cdma_ruim_access (req_p,
                                                        rec_num_accessed_p,
                                                        &num_records,
                                                        returned_data_length,
                                                        data_p );
        break;
#endif
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif
      default:
        MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
        return GSDI_ERROR;
    }

    MSG_MED("GSDI proc sim access",0,0,0);

    /* check if the slot requested is the preferred slot */

    if (gsdi_status != GSDI_SUCCESS)
       MMGSDI_DEBUG_MSG_ERROR("Get SIM Access status %d",gsdi_status,0,0);

    MSG_MED("GSDI SIM Access proc status %d",gsdi_status,0,0);

    return gsdi_status;
} /* gsdi_proc_sim_access */

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
/*===========================================================================
FUNCTION GSDI_POPULATE_CACHED_ATTRIBUTES_IF_NEEDED

DESCRIPTION
  Check the Get File Attribute Cache, if there is any unknown security
  access condition, it will perform a get file attributes on that file

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_populate_cached_attributes_if_needed(
  gsdi_slot_id_type   slot,
  uim_items_enum_type item,
  uim_protocol_type   EF_protocol,
  uint8               *data_p
)
{
  gsdi_attr_cache                 *cache_attr_ptr = NULL;
  gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;
  gsdi_get_file_attributes_req_T  *get_file_attr_ptr = NULL;
  gsdi_file_attributes_T           file_attributes;

  cache_attr_ptr = gsdi_get_cached_attributes(item);
  if (cache_attr_ptr == NULL)
  {
    MSG_ERROR("Unable to get cached attributes 0x%x", item, 0, 0);
    return GSDI_ERROR;
  }
  if (((cache_attr_ptr->read.and_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)
      && (cache_attr_ptr->read.or_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)) ||
      ((cache_attr_ptr->write.and_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)
      && (cache_attr_ptr->write.or_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)) ||
      ((cache_attr_ptr->increase.and_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)
      && (cache_attr_ptr->increase.or_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)))
  {
    /* there is at least 1 unknown security access condition */
    MSG_HIGH("Unknown Security Attr, Get File Attribute will be performed 0x%x",
              item, 0, 0);
    get_file_attr_ptr = (gsdi_get_file_attributes_req_T*)mmgsdi_malloc(
      sizeof(gsdi_get_file_attributes_req_T));
    if (get_file_attr_ptr == NULL)
    {
      MSG_ERROR("Unable to memmalloc GFA pointer 0x%x", item, 0, 0);
      return GSDI_ERROR;
    }
    if (data_p == NULL)
    {
      MSG_ERROR("data_p null", 0, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_ptr);
      return GSDI_ERROR;
    }
    get_file_attr_ptr->message_header.slot = slot;
    get_file_attr_ptr->sim_filename        =(word)item;
    switch (EF_protocol)
    {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
    case UIM_CDMA:
      gsdi_status = gsdi_cdma_proc_get_file_attributes(
        get_file_attr_ptr,data_p );
      break;
#endif /* FEATURE_MMGSDI_CDMA */

    default:
      MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
      gsdi_status = GSDI_ERROR;
      break;

    }
    if(gsdi_status == GSDI_SUCCESS)
    {
      /* populate it into the file_attributes_T */
      gsdi_util_populate_file_attr_structure(
          slot,
          gsdi_status,
          data_p,
          &file_attributes);
      gsdi_populate_file_attr_cache(file_attributes.file_protection.read,
                                     &cache_attr_ptr->read);
      gsdi_populate_file_attr_cache(file_attributes.file_protection.write,
                                     &cache_attr_ptr->write);
      gsdi_populate_file_attr_cache(file_attributes.file_protection.increase,
                                     &cache_attr_ptr->increase);
      cache_attr_ptr->cyclic_increase_allowed = file_attributes.cyclic_increase_allowed;
    }
    else
    {
      MSG_ERROR("Unable to populate file attr cache for 0x%x, status 0x%x",
                 item, gsdi_status, 0);
    }
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_ptr);
  }
  return gsdi_status;
}
#endif /* FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */

/*===========================================================================
FUNCTION GSDI_PROC_HANDLE_CLIENT_REQ

DESCRIPTION
  Function called by GSDI to Case through the requests for proper processing

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  Return value is based on a large number of things.

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_proc_handle_client_req (
  byte*          rec_num_accessed_p,
  byte*          num_records_p,
  gs_queue_id_T* return_queue_id_p,
  uint32*        client_ref_p,
  void*          req_p,
  int32*         returned_data_len_p,
  byte*          data_p,
  gsdi_resp_type* resp_type_p
)
{
  gsdi_card_mode_capability_T current_card_mode;
  gsdi_returns_T              gsdi_status = GSDI_SUCCESS;
  gsdi_slot_id_type           slot = GSDI_SLOT_NONE;
  uim_protocol_type           EF_protocol;
  word                        EF_item;
  byte                        msg_id;
  byte                        returned_data_len_b = 0;
  boolean                     ok_to_read = FALSE;

  /*lint -esym(613,rec_num_accessed_p) */
  /*lint -esym(613,return_queue_id_p)*/
  /*lint -esym(613,client_ref_p)*/
  /*lint -esym(613,resp_type_p)*/
  /*lint -esym(613,returned_data_len_p)*/

  /* --------------------------------------------------------------------------
  INITIALIZE VARIABLES AS NEEDED
  --------------------------------------------------------------------------- */
  *rec_num_accessed_p   = 0;
  current_card_mode = gsdi_data.current_card_mode_capability;

  /* --------------------------------------------------------------------------
  Extract the slot number
  --------------------------------------------------------------------------- */
  MMGSDI_RETURN_IF_NULL(req_p);
  slot = gsdi_get_slot_from_req( req_p );
  /* --------------------------------------------------------------------------
  WORK OUT WHAT HAS BEEN REQUESTED.
  1.  DETERMINE APPROPRIATE SIM IF APPLICABLE
  2.  DETERMINE IF SIM IS PRESENT AND READY
  3.  USE MESSAGE_ID TO DETERMINE WHICH FUNCTION TO CALL.
  --------------------------------------------------------------------------- */
  msg_id = ((IMH_T *)req_p)->message_id;
  switch ( msg_id )
  {
    case GSDI_GET_ECC_REQ:

      gsdi_status = gsdi_proc_get_ecc(slot);
      *return_queue_id_p = ((gsdi_get_ecc_req_T *)req_p)->queue_id;
      *client_ref_p      = ((gsdi_get_ecc_req_T *)req_p)->client_ref;
      *resp_type_p       = GSDI_GET_ECC_RSP;
    break;

    case GSDI_GET_SIM_CAPABILITIES_REQ:
      MSG_HIGH ("GSDI_GET_SIM_CAPABILITIES_REQ",0,0,0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_get_ruim_capabilities(slot);
            break;
#endif
          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown:  %x", current_card_mode,0,0);
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY %x",slot,0,0);
      }
      *return_queue_id_p  = ((gsdi_get_sim_capabilities_req_T *)req_p)
                            ->queue_id;
      *client_ref_p       = ((gsdi_get_sim_capabilities_req_T *)req_p)
                            ->client_ref;
      *returned_data_len_p = GSDI_SIM_CAPABILITIES_LEN;
      *resp_type_p         = GSDI_GET_SIM_CAPABILITIES_RSP;
      break;

    case GSDI_SIM_SEARCH_REQ:
      MSG_HIGH ("GSDI_SIM_SEARCH_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        /* SELECT PROTOCOL FOR REQUEST ITEM */
        EF_item     = ((gsdi_sim_search_req_T *)req_p)->sim_filename;
        EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);
        switch (EF_protocol)
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif

#ifdef FEATURE_MMGSDI_CDMA
          case UIM_CDMA:
            gsdi_status = gsdi_cdma_proc_ruim_search(
              (gsdi_sim_search_req_T*)req_p, rec_num_accessed_p);
            break;
#endif
          default:
            MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
            gsdi_status = GSDI_ERROR;
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY",0,0,0);
      }
      *return_queue_id_p   = ((gsdi_sim_search_req_T *)req_p)->queue_id;
      *client_ref_p        = ((gsdi_sim_search_req_T *)req_p)->client_ref;
      *returned_data_len_p = 0x00;
      *resp_type_p         = GSDI_SIM_SEARCH_RSP;
      break;

    case GSDI_SIM_READ_REQ:
      MSG_HIGH ("GSDI_SIM_READ_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      EF_item = ((gsdi_sim_read_req_T *)req_p)->sim_filename;

      /* Select the protocol to use for the requested item */
      EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);
      gsdi_status = GSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
      gsdi_status = gsdi_populate_cached_attributes_if_needed(
        ((gsdi_sim_read_req_T *)req_p)->message_header.slot,
        (uim_items_enum_type)EF_item,
        EF_protocol,
        data_p);
      if (gsdi_status == GSDI_SUCCESS)
      {
        /* Was able to get cached attributes */
        /* check for pin security attributes before going to the card if needed */
        gsdi_status = gsdi_ef_access_conditions_check((uim_items_enum_type)EF_item,
                                                     msg_id);
      }
      else
      {
        /* Was not able to get the cached attributes, so go the card
        for further processing and have the card handle any failure */
        gsdi_status = GSDI_SUCCESS;
      }
#endif /* FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */
      if (gsdi_status == GSDI_SUCCESS)
      {
        ok_to_read = FALSE;
        ok_to_read = gsdi_is_sim_present_and_ready(slot);
        if(!ok_to_read)
        {
          ok_to_read = gsdi_is_language_check((uim_items_enum_type)EF_item);
        }

        if ( ok_to_read )
        {
          switch (EF_protocol)
          {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case UIM_CDMA:
            gsdi_status = gsdi_cdma_proc_ruim_read(
              (gsdi_sim_read_req_T *)req_p, rec_num_accessed_p, num_records_p,
              &returned_data_len_b, data_p );
            *returned_data_len_p = returned_data_len_b;
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
            gsdi_status = GSDI_ERROR;
            break;
          }
        }
        else
        {
          gsdi_status = GSDI_ERROR;
          MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
        } /* sim_present_and_ready */
      }
      else
      {
        MMGSDI_DEBUG_MSG_ERROR("Access Condition not fullfill: %x, file 0x%x",
          slot,(uim_items_enum_type)EF_item,0);
      }
      *return_queue_id_p = ((gsdi_sim_read_req_T *)req_p)->queue_id;
      *client_ref_p      = ((gsdi_sim_read_req_T *)req_p)->client_ref;
      *resp_type_p       = GSDI_SIM_READ_RSP;
      break;

    case GSDI_GET_IMAGE_REQ:
      MSG_HIGH ("GSDI_GET_IMAGE_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        /* Select the protocol to use for the requested item */
        EF_item = ((gsdi_sim_access_req_T *)req_p)->sim_filename;
        EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);

        switch (EF_protocol)
        {
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
            case UIM_CDMA:
              gsdi_status = gsdi_cdma_proc_get_image(
              (gsdi_get_image_req_T *)req_p, returned_data_len_p,
                  data_p );
              break;
#endif /* FEATURE_MMGSDI_CDMA */
            default:
              MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
              gsdi_status = GSDI_ERROR;
              break;
          }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY %x",slot,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p = ((gsdi_get_image_req_T *)req_p)->queue_id;
      *client_ref_p      = ((gsdi_get_image_req_T *)req_p)->client_ref;
      *resp_type_p       = GSDI_GET_IMAGE_RSP;
      break;

    case GSDI_SIM_ACCESS_REQ:
      MSG_HIGH ("GSDI_SIM_ACCESS_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot )  && data_p != NULL)
      {
        gsdi_status = gsdi_proc_sim_access(slot,
                                           (gsdi_sim_access_req_T *)req_p,
                                           rec_num_accessed_p,
                                           returned_data_len_p,
                                           data_p );
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY %x",slot,0,0);
      } /* sim_present_and_ready */

      *return_queue_id_p = ((gsdi_sim_access_req_T *)req_p)->queue_id;
      *client_ref_p      = ((gsdi_sim_access_req_T *)req_p)->client_ref;
      *resp_type_p       = GSDI_SIM_ACCESS_RSP;
      break;

    case GSDI_SIM_WRITE_REQ:
      MSG_HIGH ("GSDI_SIM_WRITE_REQ", 0, 0, 0);
      slot = ((gsdi_sim_write_req_T  *)req_p)->message_header.slot;
      /* Select the protocol to use for the requested item */
      EF_item = ((gsdi_sim_write_req_T *)req_p)->sim_filename;
      EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);
      gsdi_status = GSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
      gsdi_status = gsdi_populate_cached_attributes_if_needed(
        ((gsdi_sim_write_req_T *)req_p)->message_header.slot,
        (uim_items_enum_type)EF_item,
        EF_protocol,
        data_p);
      if (gsdi_status == GSDI_SUCCESS)
      {
        /* Was able to get cached attributes */
        /* check for pin security attributes before going to the card if needed */
        gsdi_status = gsdi_ef_access_conditions_check((uim_items_enum_type)EF_item,
                                                     msg_id);
      }
      else
      {
        /* Was not able to get the cached attributes, so go the card
        for further processing and have the card handle any failure */
        gsdi_status = GSDI_SUCCESS;
      }
#endif /*  FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */
      if (gsdi_status == GSDI_SUCCESS)
      {
        /* IS SIM PRESENT AND READY FOR REQUEST? */
        if ( gsdi_is_sim_present_and_ready( slot ) )
        {
          switch (EF_protocol)
          {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case UIM_CDMA:
            gsdi_status = gsdi_cdma_proc_ruim_write(
              (gsdi_sim_write_req_T *)req_p, rec_num_accessed_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
            gsdi_status = GSDI_ERROR;
            break;
          }
        }
        else
        {
          gsdi_status = GSDI_ERROR;
          MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
        } /* sim_present_and_ready */
      }
      else
      {
        MMGSDI_DEBUG_MSG_ERROR("Access Condition not fullfill: %x, file 0x%x",
          slot,(uim_items_enum_type)EF_item,0);
      }
      *return_queue_id_p   = ((gsdi_sim_write_req_T *)req_p)->queue_id;
      *client_ref_p        = ((gsdi_sim_write_req_T *)req_p)->client_ref;
      *returned_data_len_p = 0x00;
      *resp_type_p         = GSDI_SIM_WRITE_RSP;
      break;

    case GSDI_SIM_INCREASE_REQ:
      MSG_HIGH ("GSDI_SIM_INCREASE_REQ", 0, 0, 0);
      /* Select the protocol to use for the requested item */
      EF_item = ((gsdi_sim_increase_req_T *)req_p)->sim_filename;
      EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);

      gsdi_status = GSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
      gsdi_status = gsdi_populate_cached_attributes_if_needed(
        ((gsdi_sim_increase_req_T *)req_p)->message_header.slot,
        (uim_items_enum_type)EF_item,
        EF_protocol,
        data_p);
      if (gsdi_status == GSDI_SUCCESS)
      {
        /* Was able to get cached attributes */
        /* check for pin security attributes before going to the card if needed */
        gsdi_status = gsdi_ef_access_conditions_check((uim_items_enum_type)EF_item,
                                                     msg_id);
      }
      else
      {
        /* Was not able to get the cached attributes, so go the card
        for further processing and have the card handle any failure */
        gsdi_status = GSDI_SUCCESS;
      }
#endif /* FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */
      if (gsdi_status == GSDI_SUCCESS)
      {
        /* IS SIM PRESENT AND READY FOR REQUEST? */
        if ( gsdi_is_sim_present_and_ready( slot ) )
        {
           switch (EF_protocol)
           {
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
            /* Not yet supported
            ** case UIM_CDMA:
            */
#endif /* FEATURE_MMGSDI_CDMA */
             default:
               MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
               gsdi_status = GSDI_ERROR;
             break;
           }
        }
        else
        {
          gsdi_status = GSDI_ERROR;
          MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: x",slot,0,0);
        } /* sim_present_and_ready */
      }
      else
      {
        MMGSDI_DEBUG_MSG_ERROR("Access Condition not fullfill: %x, file 0x%x",
          slot,(uim_items_enum_type)EF_item,0);
      }
      *return_queue_id_p = ((gsdi_sim_increase_req_T *)req_p)->queue_id;
      *client_ref_p      = ((gsdi_sim_increase_req_T *)req_p)->client_ref;
      *resp_type_p       = GSDI_SIM_INCREASE_RSP;
      break;

#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */

    case GSDI_GET_FILE_ATTRIBUTES_REQ:
      MSG_HIGH ("GSDI_GET_FILE_ATTRIBUTES_REQ", 0, 0, 0);
      /* Select the protocol to use for the requested item */
      EF_item = ((gsdi_get_file_attributes_req_T *)req_p)->sim_filename;
      ok_to_read = FALSE;
      ok_to_read = gsdi_is_sim_present_and_ready(slot);

      if(!ok_to_read)
      {
        ok_to_read = gsdi_is_language_check((uim_items_enum_type)EF_item);
      }

      if ( ok_to_read )
      {

        EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);
        switch (EF_protocol)
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case UIM_CDMA:
            gsdi_status = gsdi_cdma_proc_get_file_attributes(
              (gsdi_get_file_attributes_req_T *)req_p,data_p );
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
            gsdi_status = GSDI_ERROR;
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY",0,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p   = ((gsdi_get_file_attributes_req_T *)req_p)
                              ->queue_id;
      *client_ref_p        = ((gsdi_get_file_attributes_req_T *)req_p)
                              ->client_ref;
      *returned_data_len_p = sizeof( gsdi_file_attributes_T );
      *resp_type_p         = GSDI_GET_FILE_ATTRIBUTES_RSP;
      break;

    case GSDI_GET_PIN_STATUS_REQ:
      MSG_HIGH ("GSDI_GET_PIN_STATUS_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_in_slot_available( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_get_pin_status(
                                (gsdi_get_pin_status_req_T *)req_p,
                                (gsdi_pin_status_T *)data_p);
              break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p    = ((gsdi_get_pin_status_req_T *)req_p)->
                              queue_id;
      *client_ref_p         = ((gsdi_get_pin_status_req_T *)req_p)->
                              client_ref;
      *returned_data_len_p  = GSDI_PIN_STATUS_LEN;
      *resp_type_p          = GSDI_GET_PIN_STATUS_RSP;
      break;

    case GSDI_VERIFY_PIN_REQ:
      MSG_HIGH ("GSDI_VERIFY_PIN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_in_slot_available( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_verify_pin(
              (gsdi_verify_pin_req_T *)req_p, data_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p    = ((gsdi_verify_pin_req_T *)req_p)->queue_id;
      *client_ref_p         = ((gsdi_verify_pin_req_T *)req_p)->client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_VERIFY_PIN_RSP;
      break;

    case GSDI_CHANGE_PIN_REQ:
      MSG_HIGH ("GSDI_CHANGE_PIN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_in_slot_available( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_change_pin(
              (gsdi_change_pin_req_T *)req_p, data_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */
            default:
              gsdi_status = GSDI_ERROR;
              MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
              break;
          }
        }
        else
        {
          gsdi_status = GSDI_ERROR;
          MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
        } /* sim_present_and_ready */
        *return_queue_id_p    = ((gsdi_change_pin_req_T *)req_p )->queue_id;
        *client_ref_p         = ((gsdi_change_pin_req_T *)req_p )->client_ref;
        *returned_data_len_p  = 0x00;
        *resp_type_p          = GSDI_CHANGE_PIN_RSP;
        break;

    case GSDI_UNBLOCK_PIN_REQ:
      MSG_HIGH ("GSDI_UNBLOCK_PIN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_in_slot_available( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_unblock_pin(
              (gsdi_unblock_pin_req_T *)req_p, data_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY",0,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p    = ((gsdi_unblock_pin_req_T *)req_p)->
                              queue_id;
      *client_ref_p         = ((gsdi_unblock_pin_req_T *)req_p)->
                              client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_UNBLOCK_PIN_RSP;
      break;

    case GSDI_DISABLE_PIN_REQ:
      MSG_HIGH ("GSDI_DISABLE_PIN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_in_slot_available( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
            {
              gsdi_status = gsdi_cdma_proc_disable_pin(
                (gsdi_disable_pin_req_T *)req_p, data_p);
            }
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /*#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS*/
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p    = ((gsdi_disable_pin_req_T *)req_p )->
                              queue_id;
      *client_ref_p         = ((gsdi_disable_pin_req_T *)req_p )->
                              client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_DISABLE_PIN_RSP;
      break;

    case GSDI_ENABLE_PIN_REQ:
      MSG_HIGH ("GSDI_ENABLE_PIN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_in_slot_available( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_enable_pin(
              (gsdi_enable_pin_req_T *)req_p, data_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p    = ((gsdi_enable_pin_req_T *)req_p )->
                              queue_id;
      *client_ref_p         = ((gsdi_enable_pin_req_T *)req_p )->
                              client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_ENABLE_PIN_RSP;
      break;


    case GSDI_ENABLE_FDN_REQ:
      MSG_HIGH ("GSDI_ENABLE_FDN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_enable_fdn(
               (gsdi_enable_fdn_req_T *)req_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */

          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
      } /* sim_present_and_ready */
      *return_queue_id_p    = ((gsdi_enable_fdn_req_T *)req_p )->queue_id;
      *client_ref_p         = ((gsdi_enable_fdn_req_T *)req_p )->client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_ENABLE_FDN_RSP;
      break;

    case GSDI_DISABLE_FDN_REQ:
      MSG_HIGH ("GSDI_DISABLE_FDN_REQ", 0, 0, 0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        /* IS THE CARD MODE KNOWN? */
        switch ( gsdi_util_determine_card_mode( slot ) )
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case GSDI_CARD_RUIM:
            gsdi_status = gsdi_cdma_proc_disable_fdn
                                        ((gsdi_disable_fdn_req_T *)req_p);
            break;
#endif /* FEATURE_MMGSDI_CDMA */
          default:
            gsdi_status = GSDI_ERROR;
            MMGSDI_DEBUG_MSG_ERROR("Card Mode Unknown",0,0,0);
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
        MMGSDI_DEBUG_MSG_ERROR("GSDI NOT READY: %x",slot,0,0);
      } /* sim_present_and_ready */

      *return_queue_id_p    = ((gsdi_disable_fdn_req_T *)req_p )->
                              queue_id;
      *client_ref_p         = ((gsdi_disable_fdn_req_T *)req_p )->
                              client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_DISABLE_FDN_RSP;
      break;

    case GSDI_ILLEGAL_SIM_REQ:
      MSG_HIGH ("GSDI_ILLEGAL_SIM_REQ", 0, 0, 0);
      gsdi_proc_illegal_sim( (gsdi_illegal_sim_req_T *)req_p );
      gsdi_status         = GSDI_SUCCESS;
      *resp_type_p         = GSDI_ILLEGAL_SIM_RSP;
      *return_queue_id_p   = 0x00;  /* no return for this request */
      *client_ref_p        = 0x00;  /* no return for this request */
      *returned_data_len_p = 0x00;  /* no return for this request */
      break;

#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
    case GSDI_REGISTER_CALLBACK_REQ:
      MSG_HIGH("GSDI_REGISTER_CALLBACK_REQ",0,0,0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        gsdi_status = gsdi_proc_handle_callback_request(
          (gsdi_register_callback_req_T*)req_p);
      }
      break;
#endif /* FEATURE_GSDI_MUTLICLIENT_SUPPORT */

    case GSDI_SELECT_REQ:
      MSG_HIGH("GSDI_SELECT_REQ",0,0,0);
      /* IS SIM PRESENT AND READY FOR REQUEST? */
      if ( gsdi_is_sim_present_and_ready( slot ) )
      {
        /*Select the protocol to use for the requested item */
        EF_item = ((gsdi_select_req_T *)req_p)->sim_filename[0];
        EF_protocol = gsdi_get_EF_protocol(slot,(uim_items_enum_type)EF_item);
        switch (EF_protocol)
        {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
          case UIM_CDMA:
            gsdi_status = gsdi_cdma_proc_select(
              (gsdi_select_req_T *)req_p, &returned_data_len_b,data_p);
            *returned_data_len_p = returned_data_len_b;
            break;

#endif /* FEATURE_MMGSDI_CDMA */
          default:
            MMGSDI_DEBUG_MSG_ERROR("Invalid EF_protocol %d",EF_protocol,0,0);
            gsdi_status = GSDI_ERROR;
            break;
        }
      }
      else
      {
        gsdi_status = GSDI_ERROR;
      } /* sim_present_and_ready */
      *resp_type_p       = GSDI_SELECT_RSP;
      *return_queue_id_p = ((gsdi_select_req_T *)req_p)
                            ->queue_id;
      *client_ref_p      = ((gsdi_select_req_T *)req_p)
                            ->client_ref;
      break;

#ifdef FEATURE_GSTK
    case GSDI_SIM_REFRESH_REQ:
      gsdi_status = GSDI_ERROR;
      MMGSDI_DEBUG_MSG_ERROR("USE MMGSDI COMMAND",0,0,0);
      *return_queue_id_p = ((gsdi_sim_refresh_req_T *)req_p )->queue_id;
      *client_ref_p      = ((gsdi_sim_refresh_req_T *)req_p )->client_ref;
      *resp_type_p       = GSDI_SIM_REFRESH_RSP;
        break;

    case GSDI_REG_FOR_REFRESH_VOTE_REQ:
      /* Handling of this message is not based
      ** on any preconditions.  No need to check
      ** card applications or GSDI READY
      */
      MSG_HIGH("GSDI_REG_FOR_REFRESH_VOTE_REQ: %x",msg_id,0,0);
      gsdi_status = gsdi_add_to_vote_refresh_link_list(
        ((gsdi_refresh_registration_req_T *)req_p));
      *return_queue_id_p = ((gsdi_refresh_registration_req_T *)req_p )->
                            queue_id;
      *client_ref_p      = ((gsdi_refresh_registration_req_T *)req_p )->
                            client_ref;
      *resp_type_p       = GSDI_REFRESH_REGISTRATION_VOTE_RSP;
      break;

    case GSDI_REG_FOR_FILE_NOTIFY_REQ:
      /* Handling of this message is not based
      ** on any preconditions.  No need to check
      ** card applications or GSDI READY
      */
      MSG_HIGH("GSDI_REG_FOR_FILE_NOTIFY_REQ: %x",msg_id,0,0);
      gsdi_status = gsdi_add_to_refresh_fcn_link_list(
        (gsdi_refresh_registration_req_T *)req_p);
      *return_queue_id_p = ((gsdi_refresh_registration_req_T *)req_p )->
                            queue_id;
      *client_ref_p      = ((gsdi_refresh_registration_req_T *)req_p )->
                            client_ref;
      *resp_type_p       = GSDI_REFRESH_REGISTRATION_FCN_RSP;
      break;

    case GSDI_SIM_REFRESH_COMPLETE_REQ:
      /* Handling of this message is not based
      ** on any preconditions.  No need to check
      ** card applications or GSDI READY
      */
      MSG_HIGH("GSDI_SIM_REFRESH_COMPLETE_REQ: %x",msg_id,0,0);
      gsdi_status = gsdi_client_completed_refresh(
        (gsdi_refresh_complete_req_T *)req_p);
      *return_queue_id_p = ((gsdi_refresh_complete_req_T *)req_p )->
                            queue_id;
      *client_ref_p      = ((gsdi_refresh_complete_req_T *)req_p )->
                            client_ref;
      *resp_type_p       = GSDI_REFRESH_COMPLETED_RSP;
      break;

    case GSDI_REFRESH_UPDATE_CALL_STACK_REQ:
      MSG_HIGH("GSDI_REFRESH_UPDATE_CALL_STACK_REQ: %x",msg_id,0,0);
      gsdi_status = gsdi_proc_update_call_stack(
        (gsdi_update_call_stack_req_T *)req_p, returned_data_len_p, data_p);
      *return_queue_id_p       = ((gsdi_update_call_stack_req_T *)req_p )->
                                queue_id;
      *client_ref_p            = ((gsdi_update_call_stack_req_T *)req_p )->
                                client_ref;
      *resp_type_p             = GSDI_REFRESH_COMPLETED_RSP;
      break;
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
    case GSDI_SEND_APDU_REQ:
        MSG_HIGH("GSDI_SEND_APDU_REQ",0,0,0);
        gsdi_status  = gsdi_proc_send_apdu((gsdi_send_apdu_req_T *)req_p,
                                           &returned_data_len_b,
                                           data_p);
        *returned_data_len_p = returned_data_len_b;
        *return_queue_id_p       = ((gsdi_send_apdu_req_T *)req_p )->
                                   queue_id;
        *client_ref_p            = ((gsdi_send_apdu_req_T *)req_p )->
                                   client_ref;
        *resp_type_p             = GSDI_SEND_APDU_RSP;
        break;
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SWITCH_SLOT_PREF_REQ:
        gsdi_status = gsdi_proc_change_pref_slot((gsdi_switch_slot_preference_req_T *)req_p,
                                                  data_p);
        *return_queue_id_p       = ((gsdi_switch_slot_preference_req_T *)req_p )->
                                   queue_id;
        *client_ref_p            = ((gsdi_switch_slot_preference_req_T *)req_p )->
                                   client_ref;
        *resp_type_p             = GSDI_SWITCH_SLOT_PREF_RSP;
        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT

    case GSDI_GET_ATR_REQ:
        MSG_HIGH("GSDI_GET_ATR_REQ SLOT: %x",
          ((gsdi_get_atr_req_T *)req_p)->message_header.slot,0,0);

        if ( gsdi_is_sim_present_and_ready( slot ) )
        {
          gsdi_status = gsdi_proc_chnl_mgt_get_atr((gsdi_get_atr_req_T *)req_p,
                                          (int32 *)returned_data_len_p,
                                          data_p);
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR("GSDI IS NOT INIT OR ABLE TO PROCESS COMMANDS",0,0,0);
          gsdi_status = GSDI_NOT_INIT;
        }
        *return_queue_id_p  = ((gsdi_get_atr_req_T *)req_p)->queue_id;
        *client_ref_p       = ((gsdi_get_atr_req_T *)req_p)->client_ref;
        *resp_type_p         = GSDI_GET_ATR_RSP;
      break;

    case GSDI_OPEN_CHANNEL_REQ:
        MSG_HIGH("GSDI_OPEN_CHANNEL_REQ SLOT: %x",
          ((gsdi_open_channel_req_T *)req_p)->message_header.slot,0,0);
        if ( gsdi_is_sim_in_slot_available( slot ) )
        {
          gsdi_status = gsdi_proc_chnl_mgt_open_channel(
                              (gsdi_open_channel_req_T *)req_p,
                              (int32 *)returned_data_len_p,
                              data_p);
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR("GSDI IS NOT INIT OR ABLE TO PROCESS COMMANDS",0,0,0);
          gsdi_status = GSDI_NOT_INIT;
        }
        *return_queue_id_p  = ((gsdi_open_channel_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_open_channel_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_OPEN_CHANNEL_RSP;
        break;

    case GSDI_CLOSE_CHANNEL_REQ:
        MSG_HIGH("GSDI_CLOSE_CHANNEL_REQ SLOT: %x",
          ((gsdi_close_channel_req_T *)req_p)->message_header.slot,0,0);

        if ( gsdi_is_sim_in_slot_available( slot ) )
        {
          gsdi_status = gsdi_proc_chnl_mgt_close_channel(
                              (gsdi_close_channel_req_T *)req_p,
                              (int32 *)returned_data_len_p,
                              data_p);
        }
        else
        {
          gsdi_status = GSDI_ERROR;
        }
        *return_queue_id_p  = ((gsdi_close_channel_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_close_channel_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_CLOSE_CHANNEL_RSP;
        break;

    case GSDI_SEND_CHANNEL_DATA_REQ:
        MSG_HIGH("GSDI_SEND_CHANNEL_DATA_REQ SLOT: %x",
          ((gsdi_send_channel_data_req_T *)req_p)->message_header.slot,0,0);

        if ( gsdi_is_sim_in_slot_available( slot ) )
        {
          gsdi_status = gsdi_proc_chnl_mgt_send_channel_data(
                              (gsdi_send_channel_data_req_T *)req_p,
                              (int32 *)returned_data_len_p,
                              data_p);
        }
        else
        {
          gsdi_status = GSDI_ERROR;
        }
        *return_queue_id_p  = ((gsdi_send_channel_data_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_send_channel_data_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_SEND_CHANNEL_DATA_RSP;
        break;

#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
    case GSDI_PERSO_REGISTER_TASK_REQ:
        MSG_HIGH("GSDI_PERSO_REGISTER_TASK_REQ SLOT: %x",
          ((gsdi_perso_reg_task_req_T *)req_p)->message_header.slot,0,0);
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
        gsdi_status = gsdi_proc_perso_engine_reg_task((gsdi_perso_reg_task_req_T *)req_p);
#else
        gsdi_status = gsdi_proc_perso_reg_task((gsdi_perso_reg_task_req_T *)req_p);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Register Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_reg_task_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_reg_task_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_REG_TASK_RSP;
        break;

#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
    case GSDI_PERSO_ACT_FEATURE_IND_REQ:
        MSG_HIGH("GSDI_PERSO_ACT_FEATURE_IND_REQ SLOT: %x",
            ((gsdi_perso_act_feature_ind_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_activate_feature_ind(
            (gsdi_perso_act_feature_ind_req_T *)req_p,
             returned_data_len_p,
             data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Activate Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_act_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_act_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_ACT_IND_RSP;
        break;
    case GSDI_PERSO_DEACT_FEATURE_IND_REQ:
        MSG_HIGH("GSDI_PERSO_DEACT_FEATURE_IND_REQ SLOT: %x",
            ((gsdi_perso_deact_feature_ind_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_deactivate_feature_ind(
            (gsdi_perso_deact_feature_ind_req_T *)req_p,
            returned_data_len_p,
            data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Deactivate Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_deact_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_deact_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_DEACT_IND_RSP;
        break;
    case GSDI_PERSO_GET_FEATURE_IND_REQ:
        MSG_HIGH("GSDI_PERSO_GET_FEATURE_IND SLOT SLOT: %x",
          ((gsdi_perso_get_feature_ind_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_get_feature_ind(
                          (gsdi_perso_get_feature_ind_req_T *)req_p,
                           returned_data_len_p,
                           data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Feature Indicators Request Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_IND_RSP;

        break;
    case GSDI_PERSO_SET_FEATURE_DATA_REQ:
        MSG_HIGH("GSDI_PERSO_SET_FEATURE_DATA_REQ SLOT: %x",
          ((gsdi_perso_set_feature_data_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_set_feature_data(
                           (gsdi_perso_set_feature_data_req_T *)req_p,
                           returned_data_len_p,
                           data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Set Feature Indicators Request Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_set_feature_data_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_set_feature_data_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_SET_DATA_RSP;
        break;
    case GSDI_PERSO_OTA_DERPERSO_REQ:
        MSG_HIGH("GSDI_PERSO_OTA_DEPERSO_REQ SLOT: %x",
          ((gsdi_perso_ota_deperso_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_ota_deperso(
                          (gsdi_perso_ota_deperso_req_T *)req_p,
                           returned_data_len_p,
                           data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso OTA Deperso Request Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_ota_deperso_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_ota_deperso_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_OTA_DEPERSO_RSP;
        break;

    case GSDI_PERSO_INT_PROC_REQ:
        MSG_HIGH("GSDI_PERSO_INIT_PROC: %x",
          ((gsdi_perso_int_init_procedure_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_internal_init_procedures(
                          (gsdi_perso_int_init_procedure_req_T *)req_p,
                           returned_data_len_p,
                           data_p);

        if ( gsdi_status == GSDI_ERROR )
        {
          MMGSDI_DEBUG_MSG_ERROR("Internal Perso Inti Procedures Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_int_init_procedure_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_int_init_procedure_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_INTERNAL_PROC_RSP;
        break;
    case GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ:
        MSG_HIGH("GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ SLOT: %x",
            ((gsdi_perso_unblock_feature_ind_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_unblock_feature_ind(
            (gsdi_perso_unblock_feature_ind_req_T *)req_p,
            returned_data_len_p,
            data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Unblock Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_unblock_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_unblock_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_UNBLOCK_IND_RSP;
        break;

    case GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ:
        MSG_HIGH("GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ SLOT: %x",
            ((gsdi_perso_perm_disabled_feature_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_perm_disable_feature_ind(
            (gsdi_perso_perm_disabled_feature_req_T *)req_p,
            returned_data_len_p,
            data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Perm Disable Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_perm_disabled_feature_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_perm_disabled_feature_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_PERM_DISABLE_IND_RSP;
        break;

    case GSDI_PERSO_GET_PERM_FEATURE_IND_REQ:
        MSG_HIGH("GSDI_PERSO_GET_PERM_FEATURE_IND_REQ SLOT: %x",
            ((gsdi_perso_get_perm_feature_ind_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_get_perm_feature_ind(
            (gsdi_perso_get_perm_feature_ind_req_T *)req_p,
            returned_data_len_p,
            data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Perm Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_perm_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_perm_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_PERM_IND_RSP;
        break;
    case GSDI_PERSO_GET_FEATURE_KEY_REQ:
      MSG_HIGH("GSDI_PERSO_GET_FEATURE_KEY_REQ SLOT: %x",
            ((gsdi_perso_get_feature_key_req_T *)req_p)->message_header.slot,0,0);

        gsdi_status = gsdi_proc_perso_get_feature_key(
            (gsdi_perso_get_feature_key_req_T *)req_p,
            returned_data_len_p,
            data_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Feature Key Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_feature_key_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_feature_key_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_FEATURE_KEY_RSP;
        break;

#else /*FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/
      case GSDI_PERSO_ACT_FEATURE_IND_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Activate Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_act_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_act_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_ACT_IND_RSP;
        break;

      case GSDI_PERSO_DEACT_FEATURE_IND_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Deactivate Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_deact_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_deact_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_DEACT_IND_RSP;
        break;

      case GSDI_PERSO_SET_FEATURE_DATA_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Set Feature Indicators Request Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_set_feature_data_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_set_feature_data_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_SET_DATA_RSP;
        break;

      case GSDI_PERSO_GET_FEATURE_IND_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Feature Indicators Request Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_IND_RSP;
        break;

      case GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Unblock Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_unblock_feature_ind_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_unblock_feature_ind_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_UNBLOCK_IND_RSP;
        break;


      case GSDI_PERSO_OTA_DERPERSO_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso OTA Deperso Request Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_ota_deperso_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_ota_deperso_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_OTA_DEPERSO_RSP;
        break;

      case GSDI_PERSO_VERIFY_PHONE_CODE_REQ:
        MSG_HIGH("GSDI_PERSO_VERIFY_PHONE_CODE_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                                             req_p,
                                             msg_id,
                                             data_p,
                                             returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Verify Phone Code Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_verify_phone_code_req_T *)req_p)
                            ->queue_id;
        *client_ref_p       = ((gsdi_perso_verify_phone_code_req_T *)req_p)
                            ->client_ref;
        *resp_type_p         = GSDI_PERSO_VERIFY_PHONE_CODE_RSP;
        break;

     case GSDI_PERSO_CHANGE_PHONE_CODE_REQ:
        MSG_HIGH("GSDI_PERSO_CHANGE_PHONE_CODE_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                                             req_p,
                                             msg_id,
                                             data_p,
                                             returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Change Phone Code Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_change_phone_code_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_change_phone_code_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_CHANGE_PHONE_CODE_RSP;
        break;

    case GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ:
        MSG_HIGH("GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                                             req_p,
                                             msg_id,
                                             data_p,
                                             returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Unblock Phone Code Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_unblock_phone_code_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_unblock_phone_code_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_UNBLOCK_PHONE_CODE_RSP;
        break;

    case GSDI_PERSO_SET_PHONE_CODE_REQ:
        MSG_HIGH("GSDI_PERSO_SET_PHONE_CODE_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                                             req_p,
                                             msg_id,
                                             data_p,
                                             returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Set Phone Code Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_set_phone_code_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_set_phone_code_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_SET_PHONE_CODE_RSP;
        break;

      case GSDI_PERSO_INT_PROC_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status == GSDI_ERROR )
        {
          MMGSDI_DEBUG_MSG_ERROR("Internal Perso Inti Procedures Failed: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_internal_command_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_internal_command_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_INTERNAL_PROC_RSP;
        break;

      case GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Perm Disable Feature Ind Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_perm_disabled_feature_req_T *)req_p)
                            ->queue_id;
        *client_ref_p       = ((gsdi_perso_perm_disabled_feature_req_T *)req_p)
                            ->client_ref;
        *resp_type_p         = GSDI_PERSO_PERM_DISABLE_IND_RSP;
        break;

      case GSDI_PERSO_GET_PERM_FEATURE_IND_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                                             req_p,
                                             msg_id,
                                             data_p,
                                             returned_data_len_p);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Feature Key Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_perm_feature_ind_req_T *)req_p)
                                ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_perm_feature_ind_req_T *)req_p)
                                ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_PERM_IND_RSP;
        break;

      case GSDI_PERSO_GET_FEATURE_KEY_REQ:
        MSG_HIGH("GSDI_PERSO_GET_FEATURE_KEY_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                               req_p,
                               msg_id,
                               data_p,
                               returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Feature Key Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_feature_key_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_feature_key_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_FEATURE_KEY_RSP;
        break;

      case GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ:
        MSG_HIGH("GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                       req_p,
                       msg_id,
                       data_p,
                       returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get DCK Num Retries Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_dcks_num_retries_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_dcks_num_retries_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_DCK_NUM_RETRIES_RSP;
        break;

      case GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ:
        MSG_HIGH("GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                       req_p,
                       msg_id,
                       data_p,
                       returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get DCK unblock Num Retries Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_dck_unblock_num_retries_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_dck_unblock_num_retries_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_RSP;
        break;

     case GSDI_PERSO_GET_PHONE_CODE_STATUS_REQ:
       MSG_HIGH("GSDI_PERSO_GET_PHONE_CODE_STATUS_REQ",0,0,0);
       gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                          req_p,
                          msg_id,
                          data_p,
                          returned_data_len_p);
       if ( gsdi_status != GSDI_SUCCESS )
       {
         MMGSDI_DEBUG_MSG_ERROR("Perso Get Phone Code Request Failed: %x",gsdi_status,0,0);
       }
       *return_queue_id_p  = ((gsdi_perso_get_phonecode_status_req_T *)req_p)
                             ->queue_id;
       *client_ref_p       = ((gsdi_perso_get_phonecode_status_req_T *)req_p)
                             ->client_ref;
       *resp_type_p         = GSDI_PERSO_GET_PHONE_CODE_STATUS_RSP;
       break;

      case GSDI_PERSO_GET_FEATURE_DATA_REQ:
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                                             req_p,
                                             msg_id,
                                             data_p,
                                             returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Feature Ind Data Request Fail: %x",
                     gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_feature_data_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_feature_data_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_DATA_RSP;
        break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
      case GSDI_SIM_SECURE_STORE_REQ:
        MSG_HIGH("GSDI_SIM_SECURE_STORE_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_sim_secure_store(req_p,
                                                         returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("SIM Secure Store Req Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_sim_secure_store_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_sim_secure_store_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_SIM_SECURE_STORE_RSP;
        break;

      case GSDI_SIM_SECURE_READ_REQ:
        MSG_HIGH("GSDI_SIM_SECURE_READ_REQ",0,0,0);
        #ifdef FEATURE_MMGSDI_DEBUG_SIM_PARTITION

        gsdi_status = gsdi_perso_engine_sim_secure_read(req_p,
                                                        returned_data_len_p,
                                                        data_p);

        #else /* FEATURE_MMGSDI_DEBUG_SIM_PARTITION */

        gsdi_status = GSDI_NOT_SUPPORTED;

        #endif /* FEATURE_MMGSDI_DEBUG_SIM_PARTITION */

        if ( gsdi_status != GSDI_SUCCESS )/*lint !e774 */
        {
          MMGSDI_DEBUG_MSG_ERROR("SIM Secure Read Req Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_sim_secure_read_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_sim_secure_read_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_SIM_SECURE_READ_RSP;
        break;

      case GSDI_PERSO_ROOT_KEY_WRITE_REQ:
        MSG_HIGH("GSDI_PERSO_ROOT_KEY_WRITE_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_root_key_write(req_p,
                                                       data_p,
                                                       returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Root Key Write: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_root_key_write_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_root_key_write_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_ROOT_KEY_WRITE_RSP;
        break;

      case GSDI_PERSO_ROOT_KEY_READ_REQ:
        MSG_HIGH("GSDI_PERSO_ROOT_KEY_READ_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_root_key_read(req_p,
                                                      data_p,
                                                      returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Root Key READ: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_root_key_read_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_root_key_read_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_ROOT_KEY_READ_RSP;
        break;

     case GSDI_PERSO_GET_LIB_VERSION_REQ:
        MSG_HIGH("GSDI_PERSO_GET_LIB_VERSION_REQ",0,0,0);
        gsdi_status = gsdi_perso_engine_main(GSDI_SLOT_1,
                           req_p,
                           msg_id,
                           data_p,
                           returned_data_len_p);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Perso Get Library Version Request Failed: %x",gsdi_status,0,0);
        }
        *return_queue_id_p  = ((gsdi_perso_get_lib_version_req_T *)req_p)
                              ->queue_id;
        *client_ref_p       = ((gsdi_perso_get_lib_version_req_T *)req_p)
                              ->client_ref;
        *resp_type_p         = GSDI_PERSO_GET_LIB_VERSION_RSP;
        break;

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#endif/*FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/
#endif /* FEATURE_MMGSDI_PEROSNALIZATION */

    case GSDI_ACTIVATE_ONCHIP_SIM_REQ:
       MSG_HIGH("GSDI_ACTIVATE_ONCHIP_SIM_REQ",0,0,0);
       gsdi_status = gsdi_proc_activate_onchip_sim(GSDI_SLOT_1,
                                                   (gsdi_activate_onchip_sim_req_T*)req_p,
                                                   data_p);

       if ( gsdi_status != GSDI_SUCCESS )
       {
         MMGSDI_DEBUG_MSG_ERROR("Perso Activate ONCHIP SIM Req Failed: 0x%x",gsdi_status,0,0);
       }
       *return_queue_id_p  = ((gsdi_activate_onchip_sim_req_T *)req_p)
                              ->queue_id;
       *client_ref_p       = ((gsdi_activate_onchip_sim_req_T *)req_p)
                              ->client_ref;
       *resp_type_p         = GSDI_ACTIVATE_ONCHIP_SIM_RSP;
       break;

    case GSDI_SECURE_DEVICE_INFO_REQ:
      gsdi_status =       gsdi_handle_secure_device_info((gsdi_secure_device_info_req_T*)req_p);

      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR("Secure Device Info Request Failed: %x",gsdi_status,0,0);
      }
      *return_queue_id_p  = ((gsdi_secure_device_info_req_T *)req_p)
                            ->queue_id;
      *client_ref_p       = ((gsdi_secure_device_info_req_T *)req_p)
                            ->client_ref;
      *resp_type_p         = GSDI_SECURE_DEVICE_INFO_RSP;
      break;
#ifdef FEATURE_MMGSDI_CPHS
    case GSDI_SIM_GET_CPHS_INFORMATION_REQ:
      MSG_HIGH ("GSDI_SIM_GET_CPHS_INFORMATION_REQ SLOT: 0x%x", slot, 0, 0);

      /* No need to call a function.  The information is copied in the
      ** gsdi_util_send_gsdi_cnf() function.  This is done to ensure a task is not
      ** directly accessing MMGSDI Global memory
      */

      gsdi_status = GSDI_SUCCESS;

      *return_queue_id_p    = ((gsdi_get_cphs_information_req_T *)req_p)->queue_id;
      *client_ref_p         = ((gsdi_get_cphs_information_req_T *)req_p)->client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_GET_CPHS_RSP;
      break;
#endif /* FEATURE_MMGSDI_CPHS */

    case GSDI_GET_PBR_TABLE_REQ:
      MSG_HIGH("GSDI GET PBR TABLE REQ: 0x%x", slot,0,0);

      /* No need to call a function.  The information is retrieved during the
      ** gsdi_util_send_gsdi_cnf() function.
      */

      *return_queue_id_p    = ((gsdi_get_pbr_table_req_T *)req_p)->queue_id;
      *client_ref_p         = ((gsdi_get_pbr_table_req_T *)req_p)->client_ref;
      *returned_data_len_p  = 0x00;
      *resp_type_p          = GSDI_GET_PRB_TABLE_RSP;
      break;

    case GSDI_GET_ALL_PIN_INFO_REQ:
      MSG_HIGH("GSDI GET ALL PIN INFO REQ: 0x%x", slot,0,0);
      gsdi_status = gsdi_proc_handle_all_pin_info(
                      slot,
                      (gsdi_pin_info_item_T *) data_p);

      *return_queue_id_p    = ((gsdi_get_all_pin_info_req_T *)req_p)->queue_id;
      *client_ref_p         = ((gsdi_get_all_pin_info_req_T *)req_p)->client_ref;
      *resp_type_p          = GSDI_GET_ALL_PIN_INFO_RSP;
      break;

    case GSDI_INTERNAL_REQ:
      MSG_HIGH("GSDI INTERNAL REQ: 0x%x", slot, 0, 0);
      *client_ref_p         = ((gsdi_internal_req_T *)req_p)->client_ref;
      *resp_type_p          = GSDI_INTERNAL_RSP;
      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR ("GSDI Msg Handler - Message not Supported", 0, 0, 0);
      gsdi_status = GSDI_NOT_SUPPORTED;
      return_queue_id_p[0] = 0x00;
      client_ref_p[0]      = 0x00;
      returned_data_len_p[0] = 0x00;
      break;
    }

    return gsdi_status;
  /*lint +esym(613,rec_num_accessed_p) */
  /*lint +esym(613,return_queue_id_p)*/
  /*lint +esym(613,client_ref_p)*/
  /*lint +esym(613,resp_type_p)*/
  /*lint +esym(613,returned_data_len_p)*/
} /* gsdi_proc_handle_client_req */

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_INSERT_CB_INTO_CB_ARRAY

DESCRIPTION
  This function is a utility used to insert the callback pointers into
  the callback array maintained by the GSDI.  This will only be available
  when the GSDI is running in Multi-Client Support Mode.

  Once the callback is successfully stored in the Callback Array,
  the proper signal is called.

DEPENDENCIES
  NONE

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS or ERROR(s)

SIDE EFFECTS
  None
===========================================================================*/

gsdi_returns_T gsdi_insert_cb_into_cb_array(
    gsdi_event_callback_funct_type *gsdi_callback_funct )
{

    gsdi_returns_T gsdi_status  = GSDI_ERROR;
    byte           cb_ptr_index = 0;
    gsdi_task_cmd_type *gsdi_task_cmd_p;
    byte           empty_index  = 0;

    /* Search for the first NULL'ed Element */
    for ( cb_ptr_index=0; cb_ptr_index<GSDI_MAX_REGISTERED_TASK_CBS; cb_ptr_index++)
    {
        if ( gsdi_status != GSDI_SUCCESS &&
             gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr == NULL
           )
        {
            empty_index = cb_ptr_index;
            /* Found an empty slot to store call back pointer */
            MSG_LOW("Store %x in index %x",gsdi_callback_funct,cb_ptr_index,0);
            gsdi_status = GSDI_SUCCESS;
        }
        else if(gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr ==
                gsdi_callback_funct)
        {
          /*Duplicate Registration - there is no deregistration in GSDI*/
          gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent = FALSE;
          gsdi_status = GSDI_SUCCESS;
          empty_index = cb_ptr_index;
          break;
        }

    }

    if (( gsdi_status == GSDI_SUCCESS )
        && (empty_index < GSDI_MAX_REGISTERED_TASK_CBS))
    {
        /* Store Callback Pointer */
        gsdi_data.gsdi_callback_info[empty_index].gsdi_callback_ptr = gsdi_callback_funct;
        if ( ( gsdi_data.gsdi_state != GSDI_NOT_INIT_S )
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        || ( gsdi_data.gsdi_state_2 != GSDI_NOT_INIT_S )
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
           )
        {

          gsdi_task_cmd_p = gsdi_task_get_cmd_buf((GSDI_MAX_MSG_LEN - 1));
          if (gsdi_task_cmd_p == NULL) {
            MMGSDI_DEBUG_MSG_ERROR("Memory error", 0, 0, 0);
            return GSDI_ERROR;
          }

          memset(&(gsdi_task_cmd_p->cmd), 0x00, sizeof(gsdi_notify_client_req_T));

          gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.message_header.message_set = MS_GSDI_NOTIFY_CLIENT_EVT;
          gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.message_header.message_id = GSDI_NOTIFY_CLIENT_REQ;
          GSDI_PUT_IMH_LEN( sizeof( gsdi_notify_client_req_T ) - sizeof( gsdi_cmdhdr_T ),
                                &(gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.message_header) );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
          gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
          gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.message_header.wait_sig = NULL;
          gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.message_header.gsdi_async_callback = gsdi_discard_cnf;

          /* Message Contents */
          gsdi_task_cmd_p->cmd.gsdi_notify_client_req_cmd.cb_table_index = empty_index;

          /* Put message in destination task queue */
          gsdi_task_put_cmd_buf(gsdi_task_cmd_p);
        }
    }
    else
    {
        ERR("Callback Ptr Array Full",0,0,0);
        gsdi_status = GSDI_ERROR;
    }

    /* Set the Appropriate Signal ... Only applicable */
    /* for initialization.  After initialization, it  */
    /* is no longer necessary to set a signal.        */
    if ( gsdi_data.init_completed != TRUE )
    {
        if ( gsdi_status == GSDI_SUCCESS)
        {
            switch (empty_index)
            {
                case 0:

                    /* Set the Signal to indicate the 1st Task has */
                    /* Registered Successfully                     */
                    MSG_HIGH("Call Back Pointer Saved Task 1",0,0,0);
                    (void) rex_set_sigs(&gsdi_task_tcb,
                                        GSDI_CALLBACK_FUNC_REG_SIG);

                    break;

                case 1:
                    /* Set the Signal to indicate the 2nd Task has */
                    /* Registered Successfully                     */
                    MSG_HIGH("Call Back Pointer Saved Task 2",0,0,0);
                    (void) rex_set_sigs(&gsdi_task_tcb,
                                        GSDI_CALLBACK_FUNC_REG_SIG_1);

                    break;

                case 2:
                    /* Set the Signal to indicate the 3rd Task has */
                    /* Registered Successfully                     */
                    MSG_HIGH("Call Back Pointer Saved Task 3",0,0,0);
                    (void) rex_set_sigs(&gsdi_task_tcb,
                                        GSDI_CALLBACK_FUNC_REG_SIG_2);

                    break;

                case 3:
                    /* Set the Signal to indicate the 4th Task has */
                    /* Registered Successfully                     */
                    MSG_HIGH("Call Back Pointer Saved Task 4",0,0,0);
                    (void) rex_set_sigs(&gsdi_task_tcb,
                                        GSDI_CALLBACK_FUNC_REG_SIG_3);

                    break;

                case 4:
                    /* Set the Signal to indicate the 5th Task has */
                    /* Registered Successfully                     */
                    MSG_HIGH("Call Back Pointer Saved Task 5",0,0,0);
                    (void) rex_set_sigs(&gsdi_task_tcb,
                                        GSDI_CALLBACK_FUNC_REG_SIG_4);

                    break;

                default:

                    /* Index is too high, too low, or corrupted */
                    ERR(" Invalid index: %d",empty_index,0,0);
                    gsdi_status = GSDI_SUCCESS;

                    break;

            } /* Switch index */

        } /* GSDI_SUCCESS */

    } /* gsdi_data.sim_present_and_ready */

    return gsdi_status;

} /* gsdi_insert_cb_into_cb_array */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_PROC_HANDLE_CALLBACK_REQUEST

DESCRIPTION
    This function will be used to handle all client requests to register
    a callback function.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_handle_callback_request(
    const gsdi_register_callback_req_T *cb_req_ptr
)
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    gsdi_event_callback_funct_type *gsdi_callback_function_ptr = NULL;

    /* Do we have a NULL PTR */
    if ( cb_req_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL Pointer from GSDI_PROC_HANDLE_CLIENT_REQ",0,0,0);
        return GSDI_ERROR;
    }
    else
    {
        /* Extract Pointer from Message */

        gsdi_callback_function_ptr = (gsdi_event_callback_funct_type *)cb_req_ptr->
                                      gsdi_callback_to_register;

        /* Check Call Back Function PTR is not NULL */
        if (gsdi_callback_function_ptr == NULL)
        {
            gsdi_status = GSDI_ERROR;
            ERR_FATAL("NULL Pointer from GSDI Lib",0,0,0);

        }

        /* Perform the necessary Register Procedures */
        gsdi_status = gsdi_insert_cb_into_cb_array(gsdi_callback_function_ptr);

        if ( gsdi_status != GSDI_SUCCESS )
        {
            MMGSDI_DEBUG_MSG_ERROR("Error from GSDI_REG_CALLBACK %x",gsdi_status,0,0);
        }
    }

    return gsdi_status;
}

#endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT */
/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_REG_CALLBACK

DESCRIPTION
  This function registers a callback function to send SIM events.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECT
  None
===========================================================================*/
gsdi_returns_T gsdi_reg_callback (
  gsdi_event_callback_funct_type *gsdi_callback_funct
)
{
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

    /* Check that function pointer is valid */
    if (gsdi_callback_funct == NULL) {
        gsdi_status = GSDI_ERROR;
        ERR_FATAL("Registered callback pointer is NULL",0,0,0);
    }

    /* After successful setting, send a signal to the main
        gsdi task */
#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
    gsdi_status = gsdi_insert_cb_into_cb_array( gsdi_callback_funct );
#else
    gsdi_data.gsdi_callback_info.gsdi_callback_ptr = gsdi_callback_funct;

    MSG_HIGH("gsdi_callback ptr set",0,0,0);
    (void) rex_set_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG);

#endif /*FEATURE_GSDI_MULTICLIENT_SUPPORT */

    return gsdi_status;

} /* gsdi_reg_callback */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_APPS_AVAILABLE

DESCRIPTION
  This API is used by Calling Task to determine which Applications or DFs are
  available on the Card Inserted in the Mobile.  This function is to replace
  the function "gsdi_get_card_mode()" due to the limitations when the card
  inserted has different combinations of GSM DFs, CDMA DFs, USIM ADFs, EMV DF.

  This function will populate the BYTE or UINT8 passed in with the following
  information BIT MAP Information.

  0x00 :  NO APPS Available.  APP/DF not available or Provision info
          is not complete.  (i.e. Missing IMSI)

  0x01 :  TELECOM APP/DF Available.
          Common to GSM SIM
          Common to UICC for UMTS
          Common to CDMA RUIM
          Shared in cards with multiple APPs.

  0x02 :  GSM APP/DF Available.
          Common to GSM SIM
          Common to CDMA RUIM with GSM APP/DF
          Applicable to UICC with GSM App

  0x04 :  USIM APP/ADF Available
          Common to UICC only

  0x08 :  CDMA APP/DF Available
          Common to CDMA RUIM
          Common to GSM SIM with CDMA APP/DF
          Applicable to UICC with RUIM APP/ADF

  0x10 :  Banking Application Available
          Common to GSM SIM
          Applicable to UICC with EMV APP/DF

  When a BIT is set, the assumption can be made that the provision information
  contained within the DF/ADF meets minimal provision requirements.

  This function is executed in the context of the calling task.  There is no
  context switch and executed relatively quickly.

DEPENDENCIES
  None

RETURN VALUE
  void - No Return

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_get_apps_available( gsdi_card_apps_T * card_apps )
{

    if ( card_apps == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("NULL PARAMETER: card_apps provided by caller: %x",card_apps,0,0);
      return;
    }
    memcpy(card_apps,&gsdi_data.card_applications,sizeof(gsdi_card_apps_T));

} /* gsdi_get_apps_available */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_UIM_BUF

DESCRIPTION
  This function returns a pointer to heap for buffer allocation for UIM
  Server interface.

DEPENDENCIES
  None

RETURN VALUE
  *uim_cmd_type allocated by mmgsdi_malloc

SIDE EFFECTS
  None
===========================================================================*/
uim_cmd_type *gsdi_get_uim_buf (
  void
)
{
  uim_cmd_type *cmd_ptr = NULL;
    /* Allocate command buffer space */
    /* from TMC Heap   */
  cmd_ptr = (uim_cmd_type*)mmgsdi_malloc(sizeof(uim_cmd_type));
  if (cmd_ptr != NULL)
  {
    memset(cmd_ptr, 0x00, sizeof(uim_cmd_type));
    cmd_ptr->hdr.channel  = UIM_CHANNEL0;
  }
  return cmd_ptr;
}

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_TASK_GET_CMD_BUF

DESCRIPTION
  This function returns a pointer to heap for buffer allocation for UIM
  Server interface.

DEPENDENCIES
  None

RETURN VALUE
  *gsdi_task_cmd_type allocated by mmgsdi_malloc
===========================================================================*/
gsdi_task_cmd_type *gsdi_task_get_cmd_buf (
  word len
)
{
  /* Will eventually get Queued to High Priority Queue */
  if ((gsdi_high_pri_cmd_q.cnt + gsdi_task_cmd_q.cnt) >= GSDI_CMD_QUEUE_MAX_SIZE)
  {
    /* GSDI Command queue is full, reject the command */
    MMGSDI_DEBUG_ERR("GSDI Command Queue is full, rejecting command",0,0,0);
    return NULL;
  }

  if ( len < GSDI_MAX_MSG_LEN &&
       len > sizeof(gsdi_task_cmd_type))
  {
    return(gsdi_task_cmd_type*)mmgsdi_malloc((sizeof(uint8) * len));
  }
  else
  {
    return(gsdi_task_cmd_type*)mmgsdi_malloc(sizeof(gsdi_task_cmd_type));
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_TASK_PUT_HIGH_PRI_CMD_BUF

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_task_put_high_pri_cmd_buf (
  gsdi_task_cmd_type *cmd_ptr
)
{
    /* Check for Null pointer */
    if (cmd_ptr != NULL)
    {
        (void)q_link(cmd_ptr, &cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&gsdi_high_pri_cmd_q, &cmd_ptr->link);

        /* Set the command queue signal for */
        /* receive task                     */
        (void) rex_set_sigs(&gsdi_task_tcb, GSDI_HIGH_PRI_CMD_Q_SIG);
    }
    else
    {
        ERR ("put cmd buf NULL cmd_ptr",0,0,0);
    }
}

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_TASK_PUT_CMD_BUF

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_task_put_cmd_buf (
  gsdi_task_cmd_type *cmd_ptr
)
{
    /* Check for Null pointer */
    if (cmd_ptr != NULL) {

        (void)q_link(cmd_ptr, &cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&gsdi_task_cmd_q, &cmd_ptr->link);

        /* Set the command queue signal for */
        /* receive task                     */
        (void) rex_set_sigs(&gsdi_task_tcb, GSDI_TASK_CMD_Q_SIG);
    } else {
        ERR ("put cmd buf NULL cmd_ptr",0,0,0);
    }
}

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CMD_EXT

DESCRIPTION

This function will check the gsdi_cmd_q count to ensure that there are not
more than 30 commands pending.  If there are, the command is rejected with
the status GSDI_CMD_QUEUE_FULL.  This ensures that the gsdi task does not
consume large amounts of memory, queueing many commands when the task is
blocked for a long time, or "stuck".

DEPENDENCIES
  None

RETURN VALUE
  gsdi_status

SIDE EFFECTS
  Calls gsdi_cmd to queue a command
===========================================================================*/
gsdi_returns_T gsdi_cmd_ext (
  void * cmd
)
{

  /* Check for Null pointer */
  if (cmd == NULL)
  {
    MMGSDI_DEBUG_ERR ("Attempted to queue NULL command",0,0,0);
    return GSDI_ERROR;
  }

  /* Check both queues size.  If it is larger than
    GSDI_CMD_QUEUE_MAX_SIZE, then don't queue the command */
  if ((gsdi_high_pri_cmd_q.cnt + gsdi_task_cmd_q.cnt) >= GSDI_CMD_QUEUE_MAX_SIZE)
  {
    /* GSDI Command queue is full, reject the command */
    MMGSDI_DEBUG_ERR("GSDI Command Queue is full, rejecting command",0,0,0);
    return GSDI_CMD_QUEUE_FULL;
  }

  /* msg_set = MS_UIM_SERVER,
     GSDI_RUN_GSM_ALGORITHM_REQ,
     GSDI_USIM_AUTHENTICATION_REQ,
     GSDI_SIM_INIT_REQ,
    Will eventually get Queued to High Priority Queue */
  gsdi_cmd(cmd);
  return GSDI_SUCCESS;

}

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CMD

DESCRIPTION
  Function used to put an Item into the GSDI Command Queue.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_cmd (
  void *cmd
)
{
  gsdi_task_cmd_type * cmd_ptr        = NULL;
  word                 message_length = 0x00;
  byte                 msg_id         = 0x00;
  byte                 msg_set        = 0x00;
#if defined(FEATURE_MMGSDI_UMTS) && !defined(FEATURE_L4)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

    if(cmd == NULL)
    {
      MSG_ERROR("gsdi_cmd, cmd is NULL",0,0,0);
      return;
    }
    /* ------------------------------------------------------------------------
       Get the Message length from the cmd provided.  The message
       length is broken down into an MSB and a LSB.
       ----------------------------------------------------------------------*/
    GSDI_GET_MSG_LEN(message_length,cmd);

    /* ------------------------------------------------------------------------
       The total length is composed of the current length by the
       additional bytes as a result of the gsdi_cmdHdr_T
       ----------------------------------------------------------------------*/
    GSDI_GET_TOTAL_MSG_LEN(message_length);

    /* Get a ptr from heap */
    cmd_ptr = gsdi_task_get_cmd_buf(message_length + sizeof(q_link_type));

    if (cmd_ptr != NULL) {

        /* Copy data to command buffer */
        memcpy(&cmd_ptr->cmd,
                cmd,
                message_length);

        msg_set = ((gsdi_cmdhdr_T *)cmd)->message_set;
        msg_id  = ((gsdi_cmdhdr_T *)cmd)->message_id;

        if ( msg_set == MS_UIM_SERVER
#if defined(FEATURE_MMGSDI_UMTS) && !defined(FEATURE_L4)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
             )
        {
          gsdi_task_put_high_pri_cmd_buf(cmd_ptr);
          return;
        }

        switch ( msg_id )
        {
        case GSDI_RUN_GSM_ALGORITHM_REQ:
        case GSDI_USIM_AUTHENTICATION_REQ:
        case GSDI_SIM_INIT_REQ:
#ifdef FEATURE_MMGSDI_PERSONALIZATION
        case GSDI_PERSO_INT_PROC_REQ:
#endif
          gsdi_task_put_high_pri_cmd_buf(cmd_ptr);
          break;

        default:
            /* copy path if it is get image request */
          (void) gsdi_util_malloc_and_copy_data_ptr_member((gsdi_task_cmd_data_type*)cmd,
                                                    &cmd_ptr->cmd);

            /* Put message in destination task queue */
            gsdi_task_put_cmd_buf(cmd_ptr);
            break;
        }

    } else {
        /* Set return status */
        ERR("NULL cmd_ptr",0,0,0);
    }

} /* gsdi_cmd */

/*===========================================================================
FUNCTION GSDI_WAIT_FOR_TASKS_TO_REGISTER

DESCRIPTION
    This function when called will wait for all tasks to register.  The
    number of tasks to be registered will be dependent on which FEATURES
    are defined

DEPENDENCIES
    FEATURE_GSDI_MULTICLIENT_SUPPORT, FEATURE_UI, FEATURE_DATA

RETURN VALUE
    Void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_wait_for_tasks_to_register( void )
{
    rex_sigs_type             gsdi_wait_sig;
    boolean                   gsdi_can_wait = TRUE;

    /* Start Timer...give all Tasks 1 Second to Register */
    (void) rex_set_timer(&gsdi_int_timer, GSDI_WAIT_FOR_TASKS_TO_REGISTER);

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM && FEATURE_MMGSDI_UMTS */

#if defined(FEATURE_GSDI_MULTICLIENT_SUPPORT) && defined (FEATURE_ICARD_GSDI_REGISTER_CALLBACK)
    if ( gsdi_can_wait )
    {
        /* Wait for Client to Register */
        gsdi_wait_sig = gsdi_wait(GSDI_CALLBACK_FUNC_REG_SIG_2 |
                                  GSDI_REX_TIMER_EXPIRE_SIG );

        /* Check to See if the we timed out */
        if ( gsdi_wait_sig & GSDI_REX_TIMER_EXPIRE_SIG )
        {
            gsdi_can_wait = FALSE;
            MSG_HIGH("TO CALLBACK SIG 2",0,0,0);

            /* Clear Timer Sig and Timer */
            (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_REX_TIMER_EXPIRE_SIG);
        }
        /* Check for proper callback signal */
        if ( ( gsdi_can_wait == TRUE ) &&
              ( gsdi_wait_sig & GSDI_CALLBACK_FUNC_REG_SIG_2 ) )
        {
            (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG_2);
        }
    }
#endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT && FEATURE_ICARD_GSDI_REGISTER_CALLBACK */

#if defined(FEATURE_GSDI_MULTICLIENT_SUPPORT) && defined (FEATURE_CM) \
  && defined (FEATURE_MMGSDI_UMTS) && defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif /* FEATURE_MULTICLIENT_SUPPORT & FEATURE_CM */

#ifdef FEATURE_GSTK
    if ( gsdi_can_wait )
    {
        /* Wait for Client to Register */
        gsdi_wait_sig = gsdi_wait(GSDI_CALLBACK_FUNC_REG_SIG_4 |
                                  GSDI_REX_TIMER_EXPIRE_SIG);
        /* Check to See if the we timed out */
        if ( gsdi_wait_sig & GSDI_REX_TIMER_EXPIRE_SIG )
        {
            gsdi_can_wait = FALSE;
            MSG_HIGH("TO CALLBACK SIG 4",0,0,0);

            /* Clear Timer Sig and Timer */
            (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_REX_TIMER_EXPIRE_SIG);
        }
        /* Check for proper callback signal */
        if ( ( gsdi_can_wait == TRUE ) &&
              ( gsdi_wait_sig & GSDI_CALLBACK_FUNC_REG_SIG_4 ) )

        {
            (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG_4);
        }
    }
#endif /* FEATURE_GSTK */

    /* Able to Register all Tasks, clear timer */
    if ( gsdi_can_wait )
    {
        (void) rex_clr_timer(&gsdi_int_timer);
    }
    else
    {
        /* Clean Up any SIGS still set as a result of Timing Out */
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG);
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG_1);
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG_2);
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG_3);
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_CALLBACK_FUNC_REG_SIG_4);
    }
}


#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION   GSDI_UTIL_IS_TOOLKIT_ALLOWED

DESCRIPTION
  This function is used by GSDI to determine whether or not toolkit is allowed.

DEPENDENCIES
  slot -> GSDI_SLOT_1
       -> GSDI_SLOT_2

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean gsdi_util_is_toolkit_allowed(
  gsdi_slot_id_type  slot,
  gsdi_app_enum_type app,
  byte *             data_p
)
{
  gsdi_sim_read_req_T * read_req        = NULL;
  gsdi_returns_T        gsdi_status     = GSDI_ERROR;
  boolean               toolkit_allowed = FALSE;
  byte                  svc_table_ind   = 0x00;
  byte                  rec_num_accessed= 0;
  byte                  num_records     = 0;
  byte                  ret_data_len    = 0;
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */
  mmgsdi_return_enum_type mmgsdi_status;

  switch ( app )
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */

#ifdef FEATURE_MMGSDI_CDMA
  case GSDI_CDMA_APP:
    /* Get Memory for the read request and initialize it to 0*/
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(read_req,
      sizeof(gsdi_sim_read_req_T), mmgsdi_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      ERR_FATAL("NO MEMORY FOR READ REQ ",0,0,0);
    }


    /* Make a read request for GSM Service Table */
    read_req->sim_filename = (word)UIM_CDMA_CDMA_SVC_TABLE;
    read_req->message_header.slot = slot;
    gsdi_status = gsdi_cdma_proc_ruim_read(read_req,
                                         &rec_num_accessed,
                                         &num_records,
                                         &ret_data_len,
                                         data_p );
    if ( gsdi_status == GSDI_SUCCESS )
    {
      if (ret_data_len > GSDI_CDMA_SST_TOOLKIT_OFFSET)
      {
        /* Get the Service Table Indicaiton */
        svc_table_ind = data_p[GSDI_CDMA_SST_TOOLKIT_OFFSET] & GSDI_CDMA_SST_TOOLKIT_MASK;

        /* Determine if Toolkit is allowed */
        if ( svc_table_ind )
          toolkit_allowed = TRUE;
      }
    }
    /* Free the memory allocated for the Service Table Read */
    MMGSDIUTIL_TMC_MEM_FREE(read_req);
    break;
#endif /*FEATURE_MMGSDI_CDMA */

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD APP PROVIDED: %x",app,0,0);
    toolkit_allowed = FALSE;
    break;
  }

  return toolkit_allowed;
} /* gsdi_util_is_toolkit_allowed */


/* <EJECT> */
/*===========================================================================
FUNCTION   GSDI_GSTK_DETERMINE_TERM_PROFILE_DL_EVENT

DESCRIPTION
  This function is used by GSDI to determine which event to send to the GSTK
  task.

DEPENDENCIES
  slot -> GSDI_SLOT_1
       -> GSDI_SLOT_2

RETURNS
  None

SIDE EFFECTS
  Triggers the GSTK Task to send down a Terminal Profile Download
===========================================================================*/
static gsdi_sim_events_T gsdi_gstk_determine_term_profile_dl_event (
  gsdi_slot_id_type   slot,
  gsdi_app_enum_type  app,
  byte *              data_p
)
{
  gsdi_sim_events_T sim_event       = GSDI_NO_EVENT;
  boolean           toolkit_allowed = FALSE;

  /* ------------------------------------------------------------------------
     Call the Utility Function which caches the Service
     Table for the First Time.  All other utilities are not
     valid until the service table is cached
  --------------------------------------------------------------------------*/
  toolkit_allowed = gsdi_util_is_toolkit_allowed(slot,app, data_p);

  /* ------------------------------------------------------------------------
     Set the appropriate event.
  -------------------------------------------------------------------------*/
  switch ( slot )
  {
  case GSDI_SLOT_1:
    if ( toolkit_allowed )
      sim_event = GSDI_OK_FOR_TERMINAL_PROFILE_DL;
    else
      sim_event = GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL;
    break;

  case GSDI_SLOT_2:
    if ( toolkit_allowed )
      sim_event = GSDI_OK_FOR_TERMINAL_PROFILE_DL_2;
    else
      sim_event = GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2;
    break;

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    break;
  }
  return sim_event;
} /* gsdi_gstk_determine_term_profile_dl_event */


/*===========================================================================
FUNCTION GSDI_SET_SUPPORT_FLAG

DESCRIPTION
  Check to see if HPPLMN Scan feature is supported.
  Set the support flag to 1
  Scan Control feature is indicated by the first byte in  EF-Terminal Support Table
  Network Selection menu is indicated by the second byte in the body of the EF-Terminal Support Table
  Acting HPLMN Rescan feature is indicated by the third byte in the body of the EF-Terminal Support Table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_set_support_flag(
  gsdi_slot_id_type slot
)
{

#if defined (FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif
} /*lint !e715 */
/*lint -e715 slot not being used */
/*lint -e715 data_p not being used */


/*===========================================================================
FUNCTION   GSDI_GSTK_TERMINAL_PROFILE_DOWNLOAD

DESCRIPTION
  This function is used by GSDI to hold initialization until GSTK
  has to opportunity to send a Terminal Profile Download to the UIM.

  Upon completion or timeout...GSDI will continue initialization.

  This function is now designed to send a single Event to GSTK.

DEPENDENCIES
  If FEATURE_GSTK defined :
     GSDI to signal GSTK to perform TP dl
  If FEATURE_GSTK not defined:
     function returns immediately without sending any  TP dl indication to GSTK

  slot -> GSDI_SLOT_1
       -> GSDI_SLOT_2

       Based on the slot passed into the function, the appropriate
       Service Tables are used.

RETURNS
  None

SIDE EFFECTS
  Triggers the GSTK Task to send down a Terminal Profile Download

NOTE
  Performed lint suppression for unused parameters to the function
  due to Featurization within the function
===========================================================================*/
static void gsdi_gstk_terminal_profile_download (
  gsdi_slot_id_type slot,
  byte * data_p )
{
  gsdi_sim_events_T           sim_event         = GSDI_NO_EVENT;
  rex_sigs_type               wait_sigs         = 0x00;
  rex_sigs_type               sigs              = 0x00;
  uint8 *                     card_apps_p       = NULL;
  uint8                       card_apps         = 0x00;
#ifdef FEATURE_APPS_IMAGE_WINMOB
  nv_item_type *              nv_ftm_data_p     = NULL;
  nv_stat_enum_type           nv_status         = NV_DONE_S;
#endif /* FEATURE_APPS_IMAGE_WINMOB */

  /* --------------------------------------------------------------------------
  Use the slot parameter to determine which card appplications on
  the card should be used.
  ---------------------------------------------------------------------------*/
  switch ( slot )
  {
  case GSDI_SLOT_1:
    card_apps_p = &gsdi_data.card_applications.apps_enabled;
     break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    card_apps_p = &gsdi_data.card_applications.apps2_enabled;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",0,0,0);
    /* Not much we can do with a bad slot id */
    return;
  }

  /* --------------------------------------------------------------------------
  Use the Card Applications and the appropriate mask to
  determine whether or not the card supports the application
  currently sought out.  The Feature Defines indicate which
  cards are supported by the target.
  ----------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */

#ifdef FEATURE_APPS_IMAGE_WINMOB
  nv_ftm_data_p = mmgsdi_malloc(sizeof(nv_item_type));

  if(nv_ftm_data_p == NULL)
  {
    /* Continue with waiting for GSTK Event */
    MMGSDIUTIL_DEBUG_MSG_ERR("TMC Heap Space Not available, Not Checking for FTM", 0, 0, 0);
  }
  else
  {
    memset((unsigned char*)(nv_ftm_data_p), 0x00, sizeof(nv_item_type));
    nv_status = gsdi_get_nv(NV_FTM_MODE_I, nv_ftm_data_p);
    if (nv_status != NV_DONE_S)
    {
      /* Continue with waiting for GSTK Event */
      MMGSDI_DEBUG_MSG_ERROR("NV Read for NV_FTM_MODE_I Failed status: %x",
                             nv_status,0,0);
    }
    else if (nv_ftm_data_p->ftm_mode)
    {
      sim_event = GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL;
      MSG_HIGH("Change event to GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL",0,0,0);
    }
    mmgsdi_free( (unsigned char*) nv_ftm_data_p );
  }
#endif /* FEATURE_APPS_IMAGE_WINMOB */

#ifdef FEATURE_MMGSDI_CDMA
  /* --------------------------------------------------------------------------
  Careful not to change the SIM Event if it was already set by
  a card that had a GSM Application and the appropriate service
  table options for Toolkit Services.
  ---------------------------------------------------------------------------*/
  if ( sim_event == GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL   ||
       sim_event == GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2 ||
       sim_event == GSDI_NO_EVENT )
  {
    /* ------------------------------------------------------------------------
    Only here for the following scenarios
    1.  Target Only Support CDMA RUIM
    2.  Target Support GSM DF and CDAM DF but GSM DF is not present.
    3.  Target support GSM SIM and RUIM but GSM DF did not indicate
        services for Toolkit.
    -------------------------------------------------------------------------*/
    card_apps = *card_apps_p & GSDI_RUIM_APP_MASK;
    if ( card_apps )
    {
      sim_event = gsdi_gstk_determine_term_profile_dl_event(slot,
                                                            GSDI_CDMA_APP,
                                                            data_p);
    }
  }
#endif /* FEATURE_MMGSDI_CDMA */

  switch ( sim_event )
  {
  case GSDI_NO_EVENT:
    /* Bail out of function without indication to GSTK */
    return;

  case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL:
  case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
    gsdi_data.gsdi_gstk_tp_done = TRUE;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2:
  case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
    gsdi_data.gsdi_gstk_tp_done_2 = TRUE;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    break;
  }
  /* SEND Event
  ** Send out the Event to all Tasks.
  ** GSTK should be the only
  ** task responding to this signal
  */
  wait_sigs = ( GSDI_REX_TIMER_EXPIRE_SIG          | /*GSTK took too long*/
                GSDI_WAIT_GSTK_TP_DL_COMPLETED_SIG   /*GSTK DL Completed */
               );

  /* Clear Out all Signals relevant to the
  ** Terminal Profile Download Event
  */
  (void) rex_clr_sigs(&gsdi_task_tcb, wait_sigs);

  /* Set Timer for wait on GSTK */
  (void) rex_set_timer(&gsdi_gstk_timer, GSDI_WAIT_FOR_GSTK_TO_TP_DL);

  /* Notify GSTK that it is OK or Not OK to
  ** perform Terminal Profile Downalod
  */
  (void) gsdi_util_send_mmi_sim_ind ( sim_event );

#ifdef TEST_FRAMEWORK
  /* Wait for indication from GSTK that
  ** the Terminal Profile Download
  ** has completed.  PASS or FAIL
  ** it doesn't matter to GSDI
  Set the signal here (GSDI_REX_TIMER_EXPIRE_SIG)  */
  (void) rex_set_sigs( &gsdi_task_tcb, GSDI_REX_TIMER_EXPIRE_SIG );
#endif/*TEST_FRAMEWORK*/

  /* Wait for indication from GSTK that
  ** the Terminal Profile Download
  ** has completed.  PASS or FAIL
  ** it doesn't matter to GSDI
  */
  sigs = gsdi_wait (wait_sigs);

  /* Clear out any signals generated as
  ** a result of these procedures
  */
  (void) rex_clr_sigs(&gsdi_task_tcb, wait_sigs);
  if ( sigs & GSDI_REX_TIMER_EXPIRE_SIG )
    MMGSDI_DEBUG_MSG_ERROR("GSTK DID NOT RESPOND TO GSDI FOR TP DL",0,0,0);

  gsdi_set_support_flag(slot);
}  /* gsdi_gstk_terminal_profile_download */
/*lint +e715 slot not being used */
/*lint +e715 data_p not being used */
#endif /* FEATURE_GSTK */


/*===========================================================================
FUNCTION gsdi_populate_card_app_slot_info

DESCRIPTION
  This function will populate the gsdi_data global structure based on
  the informatio passed in from the gsdi_link_established message.

  Functions as follows:
  1)  Populate the local variables with the corresponding slot gsdi_data info
  2)  Initialize slot present to FALSE, apps enabled to 0 and card mode to
      CARD_MODE_UNKNOWN
  3)  For Protocol == GSM
       a) Check for CDMA df presented => populate data struct
                                         check DF 1800 presented or not
       b) Check fo GSM df presented => populate data struct
  4)  For Protocol == UMTS
       a) Check for USIM ADF presented
       b) Check for GSM DF presented or not if USIM ADF is absented

DEPENDENCIES
  UIM Task needs to notify GSDI when a Link has been established.  Only then
  is this function called.

RETURN VALUE
  gsdi_returns_T:
  1) GSDI_SUCCESS:  processing of the link established message was successful
  2) GSDI_ERROR:    1) No apps are available even though the UIM determined
                        the slot to be present.
                    2) Card is not in the particular slot

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_populate_card_app_slot_info(
  gsdi_slot_id_type             slot,
  gsdi_uim_link_established_T   *message_p,
  byte *                        data_p
)
{
    gsdi_returns_T               gsdi_status              = GSDI_ERROR;
    gsdi_protocol_enum_type      protocol                 = GSDI_PROTOCOL_NONE;
    gsdi_slot_status_type        slot_status              = GSDI_SLOT_ERROR;
    gsdi_dfs_present_type        *dfs_info_p              = NULL;
    int32                        i                        = 0;
    boolean                      *slot_present_p          = NULL;
    uint8                        *apps_enabled_p          = NULL;
    gsdi_card_mode_capability_T  *card_mode_p             = NULL;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif
    MMGSDI_RETURN_IF_NULL(message_p);

    mmgsdi_data_slot1.sel_aid_index = 0;

    if(data_p == NULL)
    {
      MSG_HIGH("data_p is NULL",0,0,0);
      return GSDI_ERROR;
    }

    switch (slot) {
    case GSDI_SLOT_1:
        slot_status        = message_p->gsdi_status_slot1;
        protocol           = message_p->protocol_slot1;
        dfs_info_p         = &(message_p->gsdi_dfs_present_slot1);
        slot_present_p     = &gsdi_data.card_applications.slot1_present;
        apps_enabled_p     = &gsdi_data.card_applications.apps_enabled;
        card_mode_p        = &gsdi_data.current_card_mode_capability;
        gsdi_data.protocol = message_p->protocol_slot1;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
        slot_status    = message_p->gsdi_status_slot2;
        protocol       = message_p->protocol_slot2;
        dfs_info_p     = &(message_p->gsdi_dfs_present_slot2);
        slot_present_p = &gsdi_data.card_applications.slot2_present;
        apps_enabled_p = &gsdi_data.card_applications.apps2_enabled;
        card_mode_p    = &gsdi_data.current_card_mode_capability_2;
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
        MMGSDI_DEBUG_MSG_ERROR("Unknown slot 0x%x", slot, 0, 0);
        /* Return success such that no card error will be sent out for
           a Unknown slot */
        return GSDI_SUCCESS;
    }

    /* Initialize data to "zero" */
    *slot_present_p = FALSE;
    *apps_enabled_p = 0;
    *card_mode_p = GSDI_CARD_MODE_UNKNOWN;

    /* Is slot present? */
    if ( slot_status == GSDI_SLOT_PRESENT )
    {
        /* Set the Global Data To Indicate so */
        *slot_present_p = TRUE;
        /* Determine the Protocol used at the SIM - ME Interface */
        switch ( protocol )
        {
#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA)

        case GSDI_PROTOCOL_GSM:

#ifdef FEATURE_MMGSDI_CDMA

            /* IS CDMA DF/APP Available */
            if ( dfs_info_p->cdma_present )
            {
              MSG_HIGH("CDMA DF PRESENT",0,0,0);
              *apps_enabled_p |= GSDI_RUIM_APP_MASK;
              *card_mode_p     = GSDI_CARD_RUIM;
              mmgsdi_data_slot1.app_info_ptr[i].app_data.app_type = MMGSDI_APP_RUIM;
              mmgsdi_data_slot1.app_info_ptr[i].app_selected      = TRUE;
              mmgsdi_data_slot1.app_info_ptr[i].proprietary_app   = FALSE;
              i++;
            }

            /* Look for DCS 1800 DF only for 1X/GSM Targets */
            gsdi_status = gsdi_cdma_is_icc_dcs1800_df_present( data_p );
            if ( gsdi_status == GSDI_SUCCESS )
            {
              MSG_HIGH("DCS 1800 DF PRESENT",0,0,0);
              *apps_enabled_p |= GSDI_DCS1800_APP_MASK;
              // TODO - What app is this?
            }

#endif /* FEATURE_MMGSDI_CDMA */

#if defined(FEATURE_MMGSDI_GSM) || defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM  || FEATURE_UIM_RUIM_W_GSM_ACCESS */
            break;

#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

        default:
            /* A Protocol was not established
            ** at the UIM Interface
            */
            MSG_HIGH("Invalid Protocol 0x%x for slot %d", protocol, slot, 0);
            break;
        }
        if ( *apps_enabled_p != 0x00)
        {
            /* A Valid applications was found */
            gsdi_status = GSDI_SUCCESS;
        }
    } /* slot status present */
    return gsdi_status;
} /*lint !e818 */

/*===========================================================================
FUNCTION GSDI_PROCESS_LINK_EST

DESCRIPTION
  This function will process the Link Established Message.  This function
  will encapsulate all functions executed as a result of receiving the
  link established message.

  Functions as follows:
  FEATURE_MMGSDI_DUAL_SLOT
  1)  Initialize the Illegal SIM Flag to False
  2)  Check to see which Slots are Present (Slot 1 and/or Slot 2)
  3)  When slot is determined to be present determine which apps
      reside on this Card.
  4)  Next...it needs to be determined where the GSM App will reside
  5)  Then it needs to be determined where the CDMA App will reside
  6)  A Card Error Event will not be generated from this function.  The UIM
      will have already generated the appropriate Card Error Event prior
          to sending out the Link Established Message.

  !FEATURE_MMGSDI_DUAL_SLOT
  1)  GSDI will test for all available applications.  Single Slot
      Solution in this case, so only apps_enabled is updated

  This function will set the Card Mode based on the Applications present.
  This is done to support older software that has not yet adopted the
  card application method.  The card mode set is as follows:
  1.  Applications Present
  GSM     CDMA      CARD MODE
  ---     ----      ---------
    1        1       GSDI_CARD_GSM
    1        0       GSDI_CARD_GSM
    0        1       GSDI_CARD_CDMA
    0        0       GSDI_CARD_MODE_UNKNOWN

DEPENDENCIES
  UIM Task needs to notify GSDI when a Link has been established.  Only then
  is this function called.

RETURN VALUE
  gsdi_returns_T:
  1) GSDI_SUCCESS:  processing of the link established message was successful
  2) GSDI_ERROR:    1) No apps are available even though the UIM determined
                        the slot to be present.

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_process_link_establish(
    gsdi_uim_link_established_T *   message_p,
    byte *                          data_p
)
{
    /*lint -esym(715,data_p)         */
    /*data_p is accessed during non  */
    /*dualslot builds.  Not used for */
    /*a dualslot build               */

    gsdi_returns_T gsdi_status    = GSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    uim_slot_type  slot           = 0;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    /* At This Point...don't know if SIM Inserted is
    ** Illegal or not.  Default to Not Illegal
    */
    gsdi_data.illegal_sim = FALSE;

    gsdi_status = gsdi_populate_card_app_slot_info(GSDI_SLOT_1, message_p, data_p);
    if (gsdi_status == GSDI_UIM_RST_INSTN_CHG_SUCCESS)
    {
      /* Let us not bother processing further since we will get
       * a new link established for the reset process just started
       */
      return GSDI_UIM_RST_INSTN_CHG_SUCCESS;
    }
    else if (gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_1);
    }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_status = gsdi_populate_card_app_slot_info(GSDI_SLOT_2, message_p, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_2);
    }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    /* If a Slot is Present (i.e. a Card is inserted somewhere)
    ** and there is a GSM App Somewhere, we need to ask UIM
    ** where it plans to get it from
    */
    if ( ( gsdi_data.card_applications.slot1_present ||
            gsdi_data.card_applications.slot2_present ) &&
          ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK ) ||
          ( gsdi_data.card_applications.apps2_enabled & GSDI_GSM_SIM_MASK ) )
    {
        /* ASK UIM Where GSM APP Is */
        slot = uim_get_slot_for_app(UIM_GSM_APP);
        /* Update Operational Slot Information for Higer Layers */
        switch ( slot )
        {
            case UIM_SLOT_1:
                gsdi_data.card_applications.operational_slot |= GSDI_GSM_SLOT_1_MASK;
                break;
            case UIM_SLOT_2:
                gsdi_data.card_applications.operational_slot |= GSDI_GSM_SLOT_2_MASK;
                break;
            default:
                MMGSDI_DEBUG_MSG_ERROR("NO GSM APP FOR SLOTS AVAIL",0,0,0);
                break;
        }
    }
    else
    {
        MSG_HIGH("NO GSM PROVISIONING INFO AVAIL",0,0,0);
    }
    /* If a Slot is Present (i.e. a Card is inserted somewhere)
    ** and there is a CDMA App Somewhere, we need to ask UIM
    ** where it plans to get it from
    */
    if ( ( gsdi_data.card_applications.slot1_present ||
            gsdi_data.card_applications.slot2_present ) &&
          ( gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK ) ||
          ( gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK ) )
    {
        /* Need to Determine Operational Slots for CDMA*/
        slot = uim_get_slot_for_app(UIM_CDMA_APP);
        switch ( slot )
        {
            case UIM_SLOT_1:
                gsdi_data.card_applications.operational_slot |= GSDI_CDMA_SLOT_1_MASK;
                break;
            case UIM_SLOT_2:
                gsdi_data.card_applications.operational_slot |= GSDI_CDMA_SLOT_2_MASK;
                break;
            default:
                MMGSDI_DEBUG_MSG_ERROR("NO CDMA APP FOR SLOTS AVAIL",0,0,0);
                break;
        }
    }
    else
    {
        MSG_HIGH("NO CDMA PROVISIONING INFO AVAIL",0,0,0);
    }
#else /*FEATURE_MMGSDI_DUAL_SLOT */
    if (gsdi_data.card_applications.slot1_present)
    {
#ifdef FEATURE_MMGSDI_CDMA
        if (gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK) {
            gsdi_data.card_applications.operational_slot |= GSDI_CDMA_SLOT_1_MASK;
        }
#endif /*FEATURE_MGMSDI_CDMA */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS */
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
    }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    return gsdi_status;
}
/* <EJECT> */


/*===========================================================================
FUNCTION GSDI_SEND_SIM_INSERTED_EVENT

DESCRIPTION
  Wrapper Function for sending the SIM Inserted event.  Used to simplify
  processing for FEATURE_MMGSDI_DUAL_SLOT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void gsdi_send_sim_inserted_event(void)
{
  gsdi_sim_events_T             sim_event          = GSDI_NO_EVENT;
  mmgsdi_event_data_type       *evt_info_ptr       = NULL;
  mmgsdi_return_enum_type       evt_malloc_status  = MMGSDI_ERROR;

#ifndef FEATURE_VIRTUAL_SIM
    /* Vote the UIM Clock Off Prior to Sending
    ** the SIM INSERTED Event
    */
    (void)uim_power_control ( UIM_USIM_SESSION, FALSE );
#endif /* FEATURE_VIRTUAL_SIM */

#ifdef FEATURE_MMGSDI_GSTK_TEST_AUTOMATION
#error code not present
#endif /*FEATURE_MMGSDI_GSTK_TEST_AUTOMATION */
    sim_event = gsdi_util_get_sim_inserted_event(GSDI_SLOT_1);
    if ( sim_event != GSDI_NO_EVENT )
    {
      (void)gsdi_util_send_mmi_sim_ind(sim_event);
      gsdi_data.sim_present_and_ready = TRUE;
    }


#ifdef FEATURE_MMGSDI_DUAL_SLOT
    sim_event = gsdi_util_get_sim_inserted_event(GSDI_SLOT_2);
    if ( sim_event != GSDI_NO_EVENT )
    {
      gsdi_util_send_mmi_sim_ind(sim_event);
      gsdi_data.sim_present_and_ready_2 = TRUE;
    }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    /*App is already selected, Send out Select AID Event*/
    if(mmgsdi_util_get_sel_app() != NULL)
    {
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        evt_info_ptr,
        sizeof(mmgsdi_event_data_type),
        evt_malloc_status);
      if (evt_malloc_status == MMGSDI_SUCCESS)
      {
        if(mmgsdi_util_build_select_aid_evt(MMGSDI_SLOT_1,evt_info_ptr) == MMGSDI_SUCCESS)
        {
          (void)mmgsdi_util_notify_client_evt(evt_info_ptr);
        }
        MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
      }
      else
      {
        MMGSDI_DEBUG_MSG_ERROR("malloc to send MMGSDI_SELECT_AID_EVT failed",
                               0, 0, 0);
      } /* evt_malloc_status */
    } /* mmgsdi_util_get_sel_app != NULL */
} /* gsdi_send_sim_inserted_event */


/*===========================================================================
FUNCTION GSDI_DETERMINE_SIM_INIT_COMPLETED_EVENT

DESCRIPTION
  Wrapper Function for sending the SIM Inserted event.  Used to simplify
  processing for FEATURE_MMGSDI_DUAL_SLOT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_sim_events_T gsdi_determine_sim_init_completed_event (
  gsdi_slot_id_type slot,
  uint8             op_slot,
  uint8             apps_enabled
)
{
  gsdi_sim_events_T sim_event = GSDI_NO_EVENT;

  switch ( slot )
  {
  case GSDI_SLOT_1:
    if (op_slot != 0 && apps_enabled != 0)
      sim_event = GSDI_SIM_INIT_COMPLETED;
    else
      sim_event = GSDI_SIM_INIT_COMPLETED_NO_PROV;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    if (op_slot != 0 && apps_enabled != 0)
      sim_event = GSDI_SIM_INIT_COMPLETED_2;
    else
      sim_event = GSDI_SIM_INIT_COMPLETED_NO_PROV_2;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    break;
  }
  return sim_event;
}

/*===========================================================================
FUNCTION GSDI_SEND_SIM_INIT_COMPLETED_EVENT

DESCRIPTION
  Wrapper Function for sending the SIM Inserted event.  Used to simplify
  processing for FEATURE_MMGSDI_DUAL_SLOT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void gsdi_sim_init_completed(
  gsdi_slot_id_type slot
)
{
  gsdi_sim_events_T sim_event         = GSDI_NO_EVENT;
  boolean           slot_present      = FALSE;
  uint8             apps_enabled      = 0x00;
  uint8             op_slot           = 0x00;
  uint8             op_slot_used      = 0x00;
  uint8             GSM_SLOT_MASK     = 0x00;
  uint8             CDMA_SLOT_MASK    = 0x00;
  uint8             USIM_SLOT_MASK    = 0x00;
  switch ( slot )
  {
  case GSDI_SLOT_1:
    slot_present   = gsdi_data.card_applications.slot1_present;
    apps_enabled   = gsdi_data.card_applications.apps_enabled;
    op_slot        = gsdi_data.card_applications.operational_slot;
    GSM_SLOT_MASK  = GSDI_GSM_SLOT_1_MASK;
    CDMA_SLOT_MASK = GSDI_CDMA_SLOT_1_MASK;
    USIM_SLOT_MASK = GSDI_USIM_SLOT_1_MASK;
    break;

  #ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    slot_present   = gsdi_data.card_applications.slot2_present;
    apps_enabled   = gsdi_data.card_applications.apps2_enabled;
    op_slot        = gsdi_data.card_applications.operational_slot;
    GSM_SLOT_MASK  = GSDI_GSM_SLOT_2_MASK;
    CDMA_SLOT_MASK = GSDI_CDMA_SLOT_2_MASK;
    USIM_SLOT_MASK = GSDI_USIM_SLOT_2_MASK;
    break;
  #endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    /* Bail out of function */
    return;
  }

  if ( slot_present )
  {
    /* Was the slot used for the operational slot */
    op_slot_used = (op_slot & GSM_SLOT_MASK)  |
                   (op_slot & CDMA_SLOT_MASK) |
                   (op_slot & USIM_SLOT_MASK);

    sim_event = gsdi_determine_sim_init_completed_event(slot,
                                                        op_slot_used,
                                                        apps_enabled);
    (void)gsdi_util_send_mmi_sim_ind(sim_event);

    switch ( slot )
    {
    case GSDI_SLOT_1:
      gsdi_data.sim_present_and_ready = TRUE;
      gsdi_data.init_completed        = TRUE;
      MSG_HIGH("GSDI_SIM_INIT_COMPLETED",0,0,0);
      MSG_MED(" Card1 Type = %x",gsdi_data.current_card_mode_capability,0,0);
      MSG_MED(" Card1 GSM Phase = %x",gsdi_data.sim_phase,0,0);
      (void)uim_power_control ( UIM_USIM_SESSION, FALSE );
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      gsdi_data.sim_present_and_ready_2 = TRUE;
      gsdi_data.init_completed_2        = TRUE;
      MSG_MED(" Card2 Type = %x",gsdi_data.current_card_mode_capability_2,0,0);
      MSG_MED(" Card2 GSM Phase = %x",gsdi_data.sim_phase_2,0,0);
      (void)uim_power_control ( UIM_USIM_SESSION, FALSE );
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x - Somehow corrupted",slot,0,0);
      break;
    }
  }
}


/*===========================================================================
FUNCTION GSDI_GET_SLOT_AVAILABLE_FOR_POST_PIN1_INIT

DESCRIPTION
  Function will determine which slot can run post pin1 init.  This will allow
  the task to seperately initialize Slot 1 and Slot 2.

  Function will run everytime a command is processed.

DEPENDENCIES
  REFRESH In progress.  This function will always return GSDI_SLOT_NONE
  whenever Slot 1 (SIM1) or Slot 2 (SIM2) is in the process of performing
  the Refresh procedures.  Determined by evaluating:
  mmgsdi_refresh_info_ptr - this will be populate only if Refresh is in
  progress

  If we are not in the process of a refresh, then the rest of the global
  data is evaluated to determine whether or not to perform the Post PIN1
  procedures.

  SLOT1
  gsdi_data.card_applications.slot1_present
  PIN1 Verified for Slot1

  SLOT2
  gsdi_data.card_applications.slot2_present
  gsdi_data.pin_status_info_2.pin1_just_verified

RETURN VALUE
  gsdi_slot_id_type:

  GSDI_SLOT_1:  If and only if the following are true:
                1.  gsdi_data.card_applications.slot1_present    = TRUE
                2.  PIN1 verified
                3.  gsdi_data.sim_init_completed                 = FALSE

  GSDI_SLOT_2:  if and only if the following are true:
                1.  gsdi_data.card_applications.slot2_present      = TRUE
                2.  gsdi_data.pin_status_info_2.pin1_just_verified = TRUE
                3.  gsdi_data.sim_init_completed_2                 = FALSE

  GSDI_SLOT_NONE:  If any of the conditions do not match.
                :  If slot1 or slot2 are in the middle of the REFRESH
                   procedures.
                   1. gsdi_data.gstk_data.refreshing_slot1         = TRUE;
                   - OR -
                   2. gsdi_data.gstk_data.refreshing_slot2         = TRUE;

  When both slots are available for Post PIN1 procedures,  GSDI_SLOT_1 will
  be returned before before GSDI_SLOT_2.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_slot_id_type gsdi_get_slot_available_for_post_pin1_init(
  void
)
{
  boolean can_proceed = FALSE;
  mmgsdi_slot_id_enum_type refresh_slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
  boolean reset_in_progress = FALSE;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* check if the pin is verified/disabled on the selected app */
  can_proceed = mmgsdi_util_can_proceed_to_post_pin1(MMGSDI_SLOT_2);
#else
  /* check if the pin is verified/disabled on the selected app */
  can_proceed = mmgsdi_util_can_proceed_to_post_pin1(MMGSDI_SLOT_1);
#endif

  if(mmgsdi_refresh_info_ptr != NULL)
  {
    refresh_slot_id = mmgsdi_refresh_info_ptr->request_header.slot_id;
    if ( mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_RESET ||
         mmgsdi_refresh_info_ptr->mode  == MMGSDI_REFRESH_NAA_APP_RESET||
         mmgsdi_refresh_info_ptr->mode  == MMGSDI_REFRESH_3G_SESSION_RESET ||
         mmgsdi_refresh_info_ptr->mode  == MMGSDI_REFRESH_RESET_AUTO)
    {
      reset_in_progress = TRUE;
    }
  }

  /* If the mode is Reset, then allow the Command to go through,
     since this is may be the first command after reset*/
  if (refresh_slot_id    == MMGSDI_SLOT_1 &&
      reset_in_progress == TRUE)
  {
    if ( mmgsdi_data_slot1.refresh_state >
           MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE  &&
         mmgsdi_data_slot1.mmgsdi_state >= MMGSDI_STATE_PIN_EVT_SENT &&
         can_proceed)
    {
      return GSDI_SLOT_1;
    }
    else
    {
      return GSDI_SLOT_NONE;
    }
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Do Not proceed with Post Pin1 Init Procedures if */
  /* a toolkit Session is in progress */
  if (refresh_slot_id   == MMGSDI_SLOT_2 &&
      reset_in_progress == TRUE)
  {
    if ( mmgsdi_data_slot2.refresh_state >
           MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE &&
           mmgsdi_data_slot2.mmgsdi_state >= MMGSDI_STATE_PIN_EVT_SENT &&
           can_proceed)
    {
      return GSDI_SLOT_2;
    }
    else
    {
      return GSDI_SLOT_NONE;
    }
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  /* Determine if Slot 1 is ready for Post Pin1 Init  */
  if (( gsdi_data.card_applications.slot1_present == TRUE ) &&
      (can_proceed                                == TRUE) &&
      ( mmgsdi_data_slot1.mmgsdi_state            == MMGSDI_STATE_PIN_EVT_SENT ))
  {
    if ( gsdi_data.gsdi_state != GSDI_CARD_ERROR_S ) {
      return GSDI_SLOT_1;
    }
    else
    {
      return GSDI_SLOT_NONE;
    }
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Determine if Slot 2 is ready for Post Pin1 Init  */
  if (( gsdi_data.card_applications.slot2_present  == TRUE ) &&
      ( can_proceed                                ==  TRUE) &&
      ( mmgsdi_data_slot2.mmgsdi_state             == MMGSDI_STATE_PIN_EVT_SENT ))
  {
    if ( gsdi_data.gsdi_state_2 != GSDI_CARD_ERROR_S ) {
      return GSDI_SLOT_2;
    }
    else
    {
      return GSDI_SLOT_NONE;
    }
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  /* Default...don't run Post PIN1 Init */
  return GSDI_SLOT_NONE;
} /* gsdi_get_slot_available_for_post_pin1_init */


#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION GSDI_PERSO_REFRESH

DESCRIPTION
  This function will perform the Personalization Initialization procedures
  for SIM Lock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_perso_refresh(
  gsdi_slot_id_type slot,
  int32 * len_p,
  uint8  * data_p
)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  gsdi_perso_internal_command_req_T req;

  req.message_header.slot                = GSDI_SLOT_1;
  req.message_header.message_id          = GSDI_PERSO_ENGINE_SIM_REFRESH_REQ;

  return gsdi_perso_engine_main(slot,
                                &req,
                                GSDI_PERSO_ENGINE_SIM_REFRESH_REQ,
                                data_p,
                                len_p);
#else
  return gsdi_perso_refresh_procedures(slot,
                                       len_p,
                                       (uint8 *)data_p);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
}
#endif /* FEATURE_MMGSDI_PERSONALIZATION */


#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION GSDI_CHECK_EF_PHASE

DESCRIPTION
  Will turn off the init bit in the GSDI_SIM_CACHE for UIM_GSM_PHASE.  To
  comply with 11.11 SIM Initialization specifications, it is required that
  the EF Phase Request Procedures are followed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_check_ef_phase(
  gsdi_slot_id_type slot,
  byte * rec_num_accessed_p,
  byte * num_records_p,
  byte * returned_data_len_p,
  byte * data_p
)
{
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */

} /* gsdi_check_ef_phase */
#endif /* FEATURE_GSTK */


#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION GSDI_PERFORM_POST_PIN1_PERSO_REFRESH_IF_NEEDED

DESCRIPTION
  Perform the Personalization Refresh if required

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_perform_post_pin1_perso_refresh_if_needed(
  gsdi_slot_id_type     slot,
  uint8                *data_ptr)
{
  int32           returned_data_len = 0;

  if (!gsdi_data.gstk_data.perso_refresh_needed)
    return;

  switch (slot)
  {
  case GSDI_SLOT_1:
    if (mmgsdi_refresh_info_ptr == NULL ||
        mmgsdi_refresh_info_ptr->request_header.slot_id != MMGSDI_SLOT_1)
      return;
    break;
  case GSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (mmgsdi_refresh_info_ptr == NULL ||
        mmgsdi_refresh_info_ptr->request_header.slot_id != MMGSDI_SLOT_2)
      return;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("Known slot 0x%x", slot, 0, 0);
    return;
  }
  (void)gsdi_perso_refresh(slot,&returned_data_len, data_ptr);

  gsdi_data.gstk_data.perso_refresh_needed = FALSE;
}
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION GSDI_PERFORM_POST_PIN1_TP_IF_NEEDED

DESCRIPTION
  Send Terminal Profile Download if required.

DEPENDENCIES
  FEATURE_GSDI_TP_BEFORE_USIM_ACTIVATION:

  1.  When defined, a TP DL was performed after a UICC Acivation will not
      occur after the USIM Application procedures.
  2.  When not defined, a TP DL was not doing after UICC Activation, and thus
      a TP DL is required after USIM Application Selection.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdi_perform_post_pin1_tp_if_needed(
  gsdi_slot_id_type   slot,
  uint8              *data_ptr)
{
  uint8   *apps_enabled_ptr = NULL;
  boolean *tp_done_ptr      = NULL;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
  byte     rec_num_accessed = 0;
  byte     num_records      = 0;
  byte     return_data_len  = 0;

  switch (slot)
  {
  case GSDI_SLOT_1:
    apps_enabled_ptr = &gsdi_data.card_applications.apps_enabled;
    tp_done_ptr = &gsdi_data.gsdi_gstk_tp_done;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* #ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
    break;
  case GSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    apps_enabled_ptr = &gsdi_data.card_applications.apps2_enabled;
    tp_done_ptr = &gsdi_data.gsdi_gstk_tp_done_2;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("Unknown Slot ID 0x%x", slot, 0, 0);
    return;
  }
  if (*tp_done_ptr)
  {
    MSG_HIGH("TP For slot 0x%x done", slot, 0, 0);
    return;
  }
  if(*apps_enabled_ptr & GSDI_USIM_APP_MASK)
  {
    MSG_HIGH("Slot 0x%x App: USIM, Sending Terminal Profile to UICC", slot, 0, 0);
    gsdi_gstk_terminal_profile_download(slot, data_ptr );
    return;
  }

  gsdi_check_ef_phase( slot,
                      &rec_num_accessed,
                      &num_records,
                      &return_data_len,
                      data_ptr);

  MSG_HIGH("GSDI - GSTK READY FOR TP DL PROCS",0,0,0);
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
  {
    gsdi_gstk_terminal_profile_download(slot, data_ptr );
  }

#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
}/* gsdi_perform_post_pin1_tp_if_needed */
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
/*===========================================================================
FUNCTION GSDI_POST_PIN1_INIT_PERSO

DESCRIPTION
  This function will perform the Personalization Initialization procedures
  for SIM Lock.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_post_pin1_init_perso(
  gsdi_slot_id_type slot,
  int32           * len_p,
  uint8           * data_p
)
{
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

#if (!defined(TEST_FRAMEWORK) && !defined (FEATURE_UI_CORE_REMOVED))
  int32 i=0;
  uint8 max_clients = 2;
#endif /* (!defined(TEST_FRAMEWORK) && !defined (FEATURE_UI_CORE_REMOVED) */

  if ( gsdi_data.perso_engine_state == GSDI_PERSO_ENG_NOT_INIT_S )
  {
#if (!defined(TEST_FRAMEWORK) && !defined (FEATURE_UI_CORE_REMOVED))

#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#else
   max_clients = 1;
#endif /* defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)*/

#ifdef PLATFORM_LTK
#error code not present
#endif /*PLATFORM_LTK*/

    /*Do not continue if the number of clients is less than 2*/
    for(i=0;i<max_clients && i<GSDI_PERSO_ENGINE_MAX_REG_CLIENTS; i++)
    {
      if(gsdi_perso_event_handler_table[i] == NULL)
      {
        return GSDI_SUCCESS;
      }
    }

#endif /* (!defined(TEST_FRAMEWORK) && !defined (FEATURE_UI_CORE_REMOVED) */
    gsdi_status = gsdi_perso_engine_main(slot,
                                NULL,
                                0,
                                data_p,
                                len_p);
    if (gsdi_status != GSDI_SUCCESS)
    {
      if ( slot == GSDI_SLOT_1 )
      {
        (void)gsdi_util_send_mmi_sim_ind(GSDI_CARD_ERROR);
      }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      else if ( slot == GSDI_SLOT_2 )
      {
        (void)gsdi_util_send_mmi_sim_ind(GSDI_CARD_ERROR_2);
      }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    }
  }
  return GSDI_SUCCESS;
#else
  return gsdi_perso_post_pin1(slot);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
}  /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */


/*===========================================================================
FUNCTION GSDI_IS_PERSO_COMPLETED

DESCRIPTION
  Simply indicates whether or not the GSDI Personalization Initialization
  procedures have been completed.

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  ok to proceed with Post Pin1 Init
            FALSE: not ok to proceed with Post Pin1 Init procedures.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
boolean gsdi_is_perso_completed(
  void
)
{
  boolean result = FALSE;

#if defined (FEATURE_MMGSDI_PERSONALIZATION)
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE)
  if ( gsdi_data.perso_engine_state == GSDI_PERSO_ENG_INIT_COMPLETED_S )
  {
    result = TRUE;
  }
  else
  {
    result = FALSE;
  }

#else
  if ( gsdi_data.perso_state == GSDI_PERSO_COMPLETE_STATE )
  {
    result = TRUE;
  }
  else
  {
    result = FALSE;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

  return result;
}/* gsdi_is_perso_completed */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION GSDI_HZI_INIT

DESCRIPTION
  Initializes files for homezone

DEPENDENCIES
  None

RETURN VALUE
  GSDI_SUCCESS: if the files are updated properly.

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_hzi_init
(
  void
)
{
  #ifdef FEATURE_HOME_ZONE_SERVICE
  gsdi_sim_read_req_T * read_req_p        = NULL;
  gsdi_returns_T        gsdi_status       = GSDI_SUCCESS;
  byte                  digit1            = 0;
  byte                  digit2            = 0;
  uim_items_enum_type   file_name         = UIM_NO_SUCH_ITEM;
  int32                 settings_field    = 0x00;
  byte                  version_field     = 0x00;
  byte                  returned_data_len = 0x00;
  boolean               continue_uhzi_init = FALSE;
  boolean               uiccv1 = FALSE;
  byte  data_p[GSDI_MAX_DATA_BLOCK_LEN];
  nv_item_type          gsdi_homezone_enabled;
  nv_stat_enum_type     nv_status         = NV_DONE_S;

  /*First Check for the NV Item. If present, then proceed.*/
  nv_status = gsdi_get_nv(NV_HOMEZONE_ENABLED_I,
                          &gsdi_homezone_enabled);

  if ( nv_status != NV_DONE_S )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not get Homezone",0,0,0);
    return GSDI_ERROR;
  }

  if (gsdi_homezone_enabled.homezone_enabled == FALSE)
  {
    MSG_HIGH("Homezone not enabled",0,0,0);
    return GSDI_SUCCESS;
  }


  read_req_p = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));
  if ( read_req_p == NULL )
  {
    ERR_FATAL("Could not get memory for read request",0,0,0);
  }

  memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));
  read_req_p->message_header.slot   = GSDI_SLOT_1;
  read_req_p->index_mode            = GSDI_NO_INDEX;
  read_req_p->rec_num               = 0;
  read_req_p->data_offset           = 0;
  read_req_p->required_data_len     = 0;      /* Return the Max Possible */
  read_req_p->search_string_len     = 0x00;


  MMGSDI_SELECT_FILE(file_name, UIM_USIM_IMSI,UIM_GSM_IMSI);
  read_req_p->sim_filename        = (word)file_name;

  MMGSDI_GET_FUNCTION_PTR(sim_read_hzi,
                          gsdi_umts_proc_usim_read,
                          gsdi_gsm_proc_sim_read);

  gsdi_status = (*sim_read_hzi)( read_req_p,
                                 &digit1,
                                 &digit2,
                                 (byte *)&returned_data_len,
                                 (byte *)data_p);

  if(gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("IMSI could not be read",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
    return GSDI_ERROR;
  }


  /*HZI SIM*/
  if(((data_p[1] & GSDI_HZI_IMSI_FIRST_BYTE) == data_p[1]) &&
       data_p[2] == GSDI_HZI_IMSI_SECOND_BYTE &&
      (data_p[3] == GSDI_HZI_IMSI_THIRD_BYTE ||
       data_p[3] == GSDI_HZI_IMSI_THIRD_BYTE_ALT))
  {
    MSG_HIGH("HZI IMSI Present",0,0,0);
    continue_uhzi_init = TRUE;

    read_req_p->sim_filename = (word)UIM_HZI_UHZI_SETTINGS;
    gsdi_status = (*sim_read_hzi)( read_req_p,
                                   &digit1,
                                   &digit2,
                                   (byte *)&returned_data_len,
                                   (byte *)data_p);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);

    if(GSDI_SUCCESS == gsdi_status && returned_data_len != 0)
    {
      /*Check the Activation bit. IF the bit is FALSE, do not do anything -
      UICC Requirements - UHZI- Files*/
      if(( data_p[0] & GSDI_HZI_ACTIVATED ) == FALSE)
      {
        continue_uhzi_init = FALSE;
      }
      else if (returned_data_len >= 2)
      {
        gsdi_sim_write_req_T * write_req_p    = NULL;

        settings_field = (int32)data_p[0];
        version_field = data_p[1];

        /*----------------------------------------------------------------------
          For Dual Mode Phones, add 1
          For Single Mode Phones, add 2
          ----------------------------------------------------------------------*/
#if defined(FEATURE_WCDMA) && defined(FEATURE_GSM)
#error code not present
#elif defined(FEATURE_GSM)
#error code not present
#endif
        /*Settings Field could overflow*/
        version_field = settings_field & 0xFF;
        /*update Version fields of U HZI-UE Settings with "Settings"*/
        data_p[1] = version_field;

      /* --------------------------------------------------------------------------
         Allocate Memory for the Write Request and set the parameters to prepare it
         for the read and verify loop.
        ------------------------------------------------------------------------*/
        write_req_p = mmgsdi_malloc(sizeof(gsdi_sim_write_req_T));
        if ( write_req_p == NULL )
        {
           ERR_FATAL("Could not allocate memory for the write Request",0,0,0);
        }
        memset(write_req_p,0xFF,sizeof(gsdi_sim_write_req_T));
       /* --------------------------------------------------------------------------
         Build the write request based on the information provided in the
         call.
         ------------------------------------------------------------------------*/
        write_req_p->message_header.slot  = GSDI_SLOT_1;
        write_req_p->index_mode          = (byte)GSDI_NO_INDEX;
        write_req_p->rec_num             = 0;
        write_req_p->search_string_len   = 0;
        write_req_p->data_len            = (byte)2;
        write_req_p->data_offset         = 0;
        write_req_p->sim_filename        = (word)UIM_HZI_UHZI_SETTINGS;
        memcpy(write_req_p->data_buffer, data_p,write_req_p->data_len);

       /* --------------------------------------------------------------------------
         Determine which function should be called for write
         ------------------------------------------------------------------------*/
        MMGSDI_GET_FUNCTION_PTR(sim_write_hzi,
                                gsdi_umts_proc_usim_write,
                                gsdi_gsm_proc_sim_write);

        gsdi_status = (*sim_write_hzi)( write_req_p,
                             (byte *)&returned_data_len);

        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Could not write to HZI Settings",0,0,0);
        }
        MMGSDIUTIL_TMC_MEM_FREE(write_req_p);
      }
    }/*Read Setting File*/
    else
    {
      uiccv1 = TRUE;
    }
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
  }

  if(continue_uhzi_init)
  {
    /*Send Completed Event to the Client*/
    if(uiccv1)
    {
      gsdi_util_send_sim_events_to_reg_tasks(GSDI_SIM_UHZI_V1_COMPLETE, TRUE, FALSE);
    }
    else
    {
      gsdi_util_send_sim_events_to_reg_tasks(GSDI_SIM_UHZI_V2_COMPLETE, TRUE, FALSE);
    }
  }

  return gsdi_status;
#else
  return GSDI_ERROR;
#endif
}

/*===========================================================================
FUNCTION GSDIMAIN_LEGACY

DESCRIPTION
  Entry point for the GSDI task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gsdimain_legacy (
  rex_sigs_type                       sigs_rcvd,
  rex_sigs_type                       sigs
)
{
    byte                      msg_set;
    byte                      msg_id;
    rex_tcb_type              *msg_tcb_p = NULL;
    rex_sigs_type             msg_wait_sig;
    rex_crit_sect_type        *msg_crit_sect_p = NULL;
    byte                      message_p[GSDI_MAX_MSG_LEN];
    byte                      rec_num_accessed = 0;
    byte                      num_records = 0;
    byte                      data_p[GSDI_MAX_DATA_BLOCK_LEN];
    uint32                    client_ref = 0;
    byte                      return_queue_id = 0;
    gsdi_returns_T            gsdi_status = GSDI_SUCCESS;
    int32                     returned_data_len = 0;
    gsdi_resp_type *          resp_type_p;
    gsdi_slot_id_type         slot = GSDI_SLOT_NONE;
    mmgsdi_return_enum_type   mmgsdi_status;

    /* Required Variables to support 256+ Bytes */
    uint8 *                   data_ext_p       = NULL;
    boolean                   use_data_ext     = FALSE;
    resp_type_p = &gsdi_resp;

    /* ------------------------------------------------------------------------
    ** Allocate memory for return buffer
    **  ---------------------------------------------------------------------*/
    (void) memset( data_p, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );

    /* ------------------------------------------------------------------------
    ** Main Task Loop
    ** ----------------------------------------------------------------------*/
    /* Check for an incoming message */

    gsdi_status = gsdi_util_read_message((void*)message_p, sigs_rcvd, sigs);
    if ( gsdi_status == GSDI_SUCCESS )
    {

      /* Get the message and message set */
      msg_set         = ((gsdi_cmdhdr_T *)message_p)->message_set;
      msg_id          = ((gsdi_cmdhdr_T *)message_p)->message_id;

      /* Store calling task reference data */
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
      msg_tcb_p       = ((gsdi_cmdhdr_T *)message_p)->tcb_ptr;
      msg_crit_sect_p = ((gsdi_cmdhdr_T *)message_p)->crit_sec;
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
      msg_wait_sig    = ((gsdi_cmdhdr_T *)message_p)->wait_sig;

      /* Where is the message comming from */
      switch ( msg_set )
      {
      /*
        ** MESSAGE-SET = MS_GSDI.  Generated by an External Source
        ** and directed to GSDI.
      */
      case MS_GSDI:
        /* Peek at the command...it is necessary to distinguish
        ** which data buffer Pointer to use
        */
        switch ( msg_id )
        {
          /* The following all require the extend data buffer
          ** pointer with a length that can very.
          ** Memory for data_ext_p is allocated now, and released
          ** after the confirmation is sent to the client
          */
#if defined(FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || defined(FEATURE_MMGSDI_ATCSIM)
          case GSDI_SEND_CHANNEL_DATA_REQ:
            /* Allocate the max memory associated with the response.  The memory
            ** allocated has to be the bigger between the REQ and RSP.  In which
            ** case the RESP is greater, so it is used.
            */
            use_data_ext = TRUE;
            MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(data_ext_p,
            GSDI_MAX_CHANNEL_DATA_RSP_LEN, mmgsdi_status);
            if ((mmgsdi_status != MMGSDI_SUCCESS) || (data_ext_p == NULL))
            {
              MSG_ERROR("Can Process SEND CHANNEL DATA REQ:  No Memory",0,0,0);
              gsdi_status = GSDI_ERROR;
            }
            break;
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT || FEATURE_MMGSDI_ATCSIM */

#if defined (FEATURE_MMGSDI_PERSONALIZATION) && defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE)
          case GSDI_PERSO_GET_FEATURE_KEY_REQ:
          case GSDI_PERSO_ACT_FEATURE_IND_REQ:
          case GSDI_PERSO_DEACT_FEATURE_IND_REQ:
          case GSDI_PERSO_GET_FEATURE_IND_REQ:
          case GSDI_PERSO_SET_FEATURE_DATA_REQ:
          case GSDI_PERSO_OTA_DERPERSO_REQ:
          case GSDI_PERSO_INT_PROC_REQ:
          case GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ:
          case GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ:
          case GSDI_PERSO_GET_PERM_FEATURE_IND_REQ:
          case GSDI_SIM_SECURE_READ_REQ:
          case GSDI_PERSO_ROOT_KEY_WRITE_REQ:
          case GSDI_PERSO_ROOT_KEY_READ_REQ:
            /* Allocate the max memory associated with the response.
            */
            use_data_ext = TRUE;
            MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(data_ext_p,
              600, mmgsdi_status);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              MSG_ERROR("Can't process PERSO REQs:  No Memory",0,0,0);
              gsdi_status = GSDI_ERROR;
            }
            break;
#endif /* FEATURE_MMGSDI_PERSONALIZATION && FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
          case GSDI_GET_ALL_PIN_INFO_REQ:
            /* Allocate the max memory associated with the response.
            */
            use_data_ext = TRUE;
            data_ext_p = mmgsdi_malloc(GSDI_MAX_PIN_RECORDS *
                                    sizeof(gsdi_pin_info_item_T));
            if ( data_ext_p ==  NULL )
            {
              MSG_ERROR("Can't process ALL PIN INFO REQ:  No Memory",0,0,0);
              gsdi_status = GSDI_ERROR;
            }
            break;

          case GSDI_SIM_ACCESS_REQ:
            use_data_ext = TRUE;
            data_ext_p = mmgsdi_malloc(GSDI_RSP_DATA_SIZE);
            if ( data_ext_p ==  NULL )
            {
              MSG_ERROR("Can't Process GSDI_SIM_ACCESS_REQ:  No Memory",0,0,0);
              gsdi_status = GSDI_ERROR;
            }
            break;
          default:
            use_data_ext = FALSE;
            break;
        }

        /* If we can't get memory for the request,
        ** don't process the command
        */
        if ( gsdi_status != GSDI_ERROR )
        {
          returned_data_len = 0;  /* Clear out the returned_data_len information */
          gsdi_status = gsdi_proc_handle_client_req(&rec_num_accessed,
                                                    &num_records,
                                                    &return_queue_id,
                                                    &client_ref,
                                                    message_p,
                                                    &returned_data_len,
                                                    (byte *)((use_data_ext)?data_ext_p:data_p),
                                                    resp_type_p );
        }

        /* Send a response back to the requesting task */
        if ( ( msg_id != GSDI_INIT_REQ )        &&
              ( msg_id != GSDI_ILLEGAL_SIM_REQ ) &&
              ( msg_id != GSDI_CALL_STATE_REQ ) )
        {
            gsdi_status = gsdi_util_send_gsdi_cnf(gsdi_status,
                                                  client_ref,
                                                  return_queue_id,
                                                  rec_num_accessed,
                                                  num_records,
                                                  returned_data_len,
                                                  (byte *)((use_data_ext)?data_ext_p:data_p),
                                                  msg_tcb_p,
                                                  msg_wait_sig,
                                                  msg_crit_sect_p,
                                                  message_p,
                                                  resp_type_p);
          if ( gsdi_status != GSDI_SUCCESS )
              MSG_HIGH("Sent cnf invalid gsdi_status %d",gsdi_status,0,0);
        } /* msg_id */

        if ( ( gsdi_status == GSDI_SUCCESS ) ||
             ( gsdi_status == GSDI_SUCCESS_BUT_ILLEGAL_SIM ) )
        {
          /* Which Slot is OK for POST PIN1 INIT Procedures */
          slot = gsdi_get_slot_available_for_post_pin1_init();
          if ( slot != GSDI_SLOT_NONE )
          {
            /* check if the post pin functions were done for refresh_fcn*/
            if(gsdi_data.gstk_data.gsdi_refresh_fcn_data.perso_op_done != TRUE)
            {
              /* Turn off the indication for the appropriate
              ** SIM to avoid processing commands while that
              ** SIM is initializing
              */
              gsdi_util_set_sim_present_and_ready(slot,FALSE);

#ifdef FEATURE_GSTK
              gsdi_perform_post_pin1_tp_if_needed(slot, data_p);
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
              gsdi_perform_post_pin1_perso_refresh_if_needed(slot, data_p);

              /* Continue with Intialization procedures which
                require a PIN to be verified. */
              gsdi_status = gsdi_post_pin1_init_perso(slot,
                                                    &returned_data_len,
                                                    (uint8 *)data_p);

              if ( gsdi_status == GSDI_ERROR )
              {
                /* Send confirmation because of Sanity Error */
                mmgsdi_refresh_send_complete(MMGSDI_SLOT_1,
                                             FALSE);
                MMGSDI_DEBUG_MSG_ERROR("PERSO POST PIN1 INTI FAILURE",0,0,0);
              }

              if ( gsdi_is_perso_completed() )

#elif defined(FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY)
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY || FEATURE_MMGSDI_PERSONALIZATION */
              {
                /* Perform specified initialization procedures */
                if ((use_data_ext == FALSE) && (data_ext_p == NULL))
                {
                  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(data_ext_p,
                   600, mmgsdi_status);
                  if (mmgsdi_status != MMGSDI_SUCCESS)
                  {
                    MSG_ERROR("Can't process PERSO REQs:  No Memory",0,0,0);
                    gsdi_status = GSDI_ERROR;
                    break;
                  }
                  else
                  {
                    use_data_ext = TRUE;
                  }
                }
                (void)gsdi_hzi_init();

                /* Perform specified initialization procedures */
                gsdi_status = mmgsdi_proc_post_pin1_init( slot,
                                                        &returned_data_len,
                                                        data_ext_p);

                if( gsdi_status == GSDI_SUCCESS )
                {
                  switch (slot)
                  {
                    case GSDI_SLOT_1:
                      gsdi_data.cache_ready = TRUE;
                      break;
                    case GSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                      gsdi_data.cache_ready_2 = TRUE;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
                      break;
                    case GSDI_SLOT_NONE:
                    case GSDI_SLOT_AUTOMATIC:
                    case GSDI_SLOT_MAX:
                      break;
                  }
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
                  {
                    gsdi_util_set_sim_present_and_ready(slot,TRUE);
                    gsdi_sim_init_completed(slot);
                  }
#ifndef FEATURE_VIRTUAL_SIM
                  (void)uim_power_control ( UIM_USIM_SESSION, FALSE );
#endif /* FEATURE_VIRTUAL_SIM */

#ifdef FEATURE_GSTK
                  /*Send Refresh Complete*/
                  mmgsdi_refresh_send_complete(MMGSDI_SLOT_1,TRUE);
#endif /* FEATURE_GSTK */
                }
                else
                {
#ifdef FEATURE_GSTK
                  /*Send Error for Refresh Complete FCN*/
                  mmgsdi_refresh_send_complete(MMGSDI_SLOT_1, FALSE);
#endif /* FEATURE_GSTK */
                 if ( slot == GSDI_SLOT_1 )
                 {
                   (void)gsdi_util_send_mmi_sim_ind(GSDI_CARD_ERROR);
                 }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                 else if ( slot == GSDI_SLOT_2 )
                 {
                   (void)gsdi_util_send_mmi_sim_ind(GSDI_CARD_ERROR_2);
                 }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
                }
              } /* pin1_just_verified */
            } /* perso_op_done*/
          } /* Slot != GSDI_SLOT_NONE */
          /* if perso operation is already done, set it to FALSE */
          if(gsdi_data.gstk_data.gsdi_refresh_fcn_data.perso_op_done == TRUE)
          {
             gsdi_data.gstk_data.gsdi_refresh_fcn_data.perso_op_done = FALSE;
          }
        } /* GSDI_SUCCESS || GSDI_SUCCESS_BUT_ILLEGAL */
        else
        {
          MSG_HIGH("Invalid gsdi_status %d",gsdi_status,0,0);
        } /* gsdi_status */
        break;

      /* MESSAGE-SET = UIM Server */
      case MS_UIM_SERVER:
#ifdef FEATURE_UIM_HOT_SWAP

  if (msg_id == GSDI_CARD_SWAP_MSG) {

    gsdi_process_card_swap(((gsdi_uim_swap_T *)message_p)->slot,
                           ((gsdi_uim_swap_T *)message_p)->status);

    break;
  }
#endif /* FEATURE_UIM_HOT_SWAP  */


#ifdef FEATURE_GSTK
      case MS_GSDI_RESET_COMPLETE:
#endif /* FEATURE_GSTK */
          /* UIM Server indication that a com link has been
                    established with the SIM card */
          if ( msg_id == GSDI_LINK_ESTABLISHED_MSG )
          {
           /* Process the Link Established Message.
              ** Either Set the DF/App information provided by UIM
              ** or test for it
              */
           /* Save ATR info into the GSDI_DATA Global Structure             */
            gsdi_data.card_atr.atr_len = ((gsdi_uim_link_established_T *)message_p)->length_atr_bytes;
            if ( ( gsdi_data.card_atr.atr_len > 0 ) &&
                  ( gsdi_data.card_atr.atr_len < GSDI_MAX_ATR_BYTES ))
            {
                memcpy(gsdi_data.card_atr.atr_buffer,
                      ((gsdi_uim_link_established_T *)message_p)->atr_bytes,
                      ((gsdi_uim_link_established_T *)message_p)->length_atr_bytes);
            }
            /* Save Historic Byte info into the GSDI_DATA Global Structure             */
            gsdi_data.hist_bytes.hist_len = ((gsdi_uim_link_established_T *)message_p)->length_historical_bytes;
            if ( ( gsdi_data.hist_bytes.hist_len > 0 ) &&
                  ( gsdi_data.hist_bytes.hist_len < GSDI_MAX_HIST_BYTES ))
            {
              memcpy(gsdi_data.hist_bytes.hist_buffer,
                ((gsdi_uim_link_established_T *)message_p)->historical_bytes,
                ((gsdi_uim_link_established_T *)message_p)->length_historical_bytes);
            }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
            gsdi_data.card_atr_2.atr_len = ((gsdi_uim_link_established_T *)message_p)->length_atr_2_bytes;
            if ( ( gsdi_data.card_atr_2.atr_len > 0 )&&
                  ( gsdi_data.card_atr_2.atr_len < GSDI_MAX_ATR_BYTES ))
            {
                memcpy(gsdi_data.card_atr_2.atr_buffer,
                      ((gsdi_uim_link_established_T *)message_p)->atr_2_bytes,
                      ((gsdi_uim_link_established_T *)message_p)->length_atr_2_bytes);
            }
            /* Save Historic Byte info into the GSDI_DATA Global Structure             */
            gsdi_data.hist_bytes_2.hist_len = ((gsdi_uim_link_established_T *)message_p)->length_historical_bytes_2;
            if ( ( gsdi_data.hist_bytes_2.hist_len > 0 ) &&
                  ( gsdi_data.hist_bytes_2.hist_len < GSDI_MAX_HIST_BYTES ))
            {
              memcpy(gsdi_data.hist_bytes_2.hist_buffer,
                ((gsdi_uim_link_established_T *)message_p)->historical_bytes_2,
                ((gsdi_uim_link_established_T *)message_p)->length_historical_bytes_2);
            }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
            gsdi_data.gsdi_uim_sanity_timer_value =
              ((gsdi_uim_link_established_T *)message_p)->work_waiting_time *
              GSDI_MAX_BYTES_PER_CMD_FROM_UIM * GSDI_MAX_UIM_CMDS_PENDING;

            MSG_MED("MS_UIM_SERVER message set received",0,0,0);

              gsdi_status = gsdi_process_link_establish(
                        (gsdi_uim_link_established_T *)message_p,data_p);
              if (gsdi_status == GSDI_UIM_RST_INSTN_CHG_SUCCESS)
              {
                /* Ignore this link established and continue waiting
                  * we will eventually get another link established
                  */
                break;
              }
              if ( gsdi_data.card_applications.slot1_present
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                || gsdi_data.card_applications.slot2_present
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
                )
              {
                  /* Determine if this is the first time we go through this
                  ** or if we have gone through this before
                  */
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

/* Cache the MEID value from NV memory */
#if defined (FEATURE_MMGSDI_CDMA) || defined (FEATURE_CSIM)
              mmgsdi_cache_meid_data();
#endif /* FEATURE_MMGSDI_CDMA || FEATURE_CSIM */

#ifdef FEATURE_MMGSDI_NV_ME_CONFIG
                  /* Read the nv parameter */
                  mmgsdi_status = mmgsdi_get_me_config_params_from_nv();
                  if (mmgsdi_status != MMGSDI_SUCCESS)
                  {
                    if ( gsdi_data.card_applications.slot1_present )
                    {
                      (void)gsdi_util_send_card_error(GSDI_SLOT_1);
                    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    if ( gsdi_data.card_applications.slot2_present )
                    {
                      (void)gsdi_util_send_card_error(GSDI_SLOT_2);
                    }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
                    break;
                  }
#endif

                  /*Check to See if more than 1 app is present.*/
                  if(mmgsdi_util_wait_for_card_select() == TRUE)
                  {
                    /*Wait for App Selection*/
                    break;
                  }
                  /* Determine the type of SIM card present */
                  /* this function handle updating the global structure
                      based on each return value from the SIM/USIM/RUIM
                      card mode capability function */
                  gsdi_status = gsdi_get_card_mode_capability(
                                (gsdi_uim_link_established_T *)message_p,data_p);

                  if ( gsdi_status == GSDI_UIM_RST_INSTN_CHG_SUCCESS )
                  {
                      /* This is a badly configured USIM.  Since we forced a UICC RESET to
                      ** restart with a GSM Instruction Class to force it as a GSM SIM
                      ** Card, a New Link Established Message will be sent by the UIM.
                      ** Now need to wait for it.
                      */
                      break;
                  }

                  if(gsdi_data.current_card_mode_capability == GSDI_CARD_MODE_UNKNOWN)
                  {
                    (void)gsdi_util_send_card_error(GSDI_SLOT_1);
                  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                  if(gsdi_data.current_card_mode_capability_2 == GSDI_CARD_MODE_UNKNOWN)
                  {
                    gsdi_util_send_card_error(GSDI_SLOT_2);
                  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

                  /* This function will send out card error event if pin caching is
                  ** unsuccessful
                  */
                  gsdi_status = gsdi_cache_pin_status( );
                  if ( gsdi_status != GSDI_SUCCESS )
                        MMGSDI_DEBUG_MSG_ERROR("COULD NOT CACHE PIN STATUS %x",gsdi_status,0,0);

                  /* Send a card inserted event via the registered callback */
                  if (gsdi_status == GSDI_SUCCESS)
                  {
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
                    {
                      MSG_HIGH("GSDI_SIM_INSERTED",0,0,0);
                      gsdi_send_sim_inserted_event();
                    }
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    /* Get NV slot Preferences and set
                    ** accordingly in gsdi_data global structure
                    ** for use by gsdi_get_user_preferred_slot
                    */
                    (void) gsdi_get_nv_slot_preferences();
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
                  }
                  else
                  {
                    gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_1);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                    gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_2);
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
                  }
              /* Forward an error condition from the UIM Server */
              }
          }
          else if ( msg_id == GSDI_ERROR_CONDITION )
          {
              gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_1);
          }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
          else if ( msg_id == GSDI_ERROR_CONDITION_2)
          {
              gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_2);
          }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
          else
          {
              MSG_HIGH("MS_UIM_SERVER invalid MSG ID %x",msg_id,0,0);
          }
          break;

      case MS_GSDI_NOTIFY_CLIENT_EVT:
          gsdi_status              = gsdi_proc_notify_client((gsdi_notify_client_req_T*)message_p);
          break;

        /*
        ** MESSAGE-SET UNEXPECTED
        */
      default:
          MMGSDI_DEBUG_MSG_ERROR("GSDI task received invalid MS SET",0,0,0);
          break;

      } /* switch */

      gsdi_util_free_data_ptr_member((gsdi_task_cmd_data_type*)message_p);

      /* Perform any clean up */
      if ( use_data_ext == TRUE &&
            data_ext_p   != NULL  )
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data_ext_p);
        use_data_ext = FALSE;
      }
    } /* GSDI_SUCCESS */
    else
    {
      MSG_ERROR("GSDI Task Received an error while trying to process a command",0,0,0);
    }

    /* ------------------------------------------------------------------
    ** End Main Task Loop
    ** ------------------------------------------------------------------ */

} /* gsdimain_legacy */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_UMTS */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_MAIN_TASK_INIT

DESCRIPTION
  Function called to define Necessary Timers and initialize the gsdi_data
  global variable.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void gsdi_main_task_init (
 void
)
{
    /* ---------------------------------------------------------
        DEFINE NECESSARY TIMERS
     ---------------------------------------------------------*/
    /* Initialize Internal Timer */
    rex_def_timer (&gsdi_int_timer, &gsdi_task_tcb,GSDI_REX_TIMER_EXPIRE_SIG);

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
    rex_def_timer (&gsdi_read_api_test_timer, &gsdi_task_tcb,
    GSDI_API_TESTER_READ_TIMER_SIG);

    rex_def_timer (&gsdi_gps_api_test_timer, &gsdi_task_tcb,
    GSDI_API_TESTER_GPS_TIMER_SIG);

    rex_def_timer (&gsdi_write_api_test_timer, &gsdi_task_tcb,
    GSDI_API_TESTER_WRITE_TIMER_SIG);

    rex_def_timer (&gsdi_gfas_api_test_timer, &gsdi_task_tcb,
    GSDI_API_TESTER_GFA_TIMER_SIG);

    rex_def_timer (&gsdi_select_api_test_timer, &gsdi_task_tcb,
    GSDI_API_TESTER_SELECT_TIMER_SIG);

    rex_def_timer (&gsdi_gscs_api_test_timer, &gsdi_task_tcb,
      GSDI_API_TESTER_GSC_TIMER_SIG);

    rex_def_timer (&gsdi_increase_api_test_timer, &gsdi_task_tcb,
      GSDI_API_TESTER_INCREASE_TIMER_SIG);

    rex_def_timer (&gsdi_search_api_test_timer, &gsdi_task_tcb,
      GSDI_API_TESTER_SEARCH_TIMER_SIG);

        rex_def_timer (&gsdi_open_channel_api_test_timer, &gsdi_task_tcb,
      GSDI_API_TESTER_OPEN_CHANNEL_TIMER_SIG);

    rex_def_timer (&gsdi_open_channel_api_test_timer, &gsdi_task_tcb,
      GSDI_API_TESTER_OPEN_CHANNEL_TIMER_SIG);

    rex_def_timer_ex(&gsdi_read_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_read_test_table);

    rex_def_timer_ex(&gsdi_write_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_write_test_table);

    rex_def_timer_ex(&gsdi_pin_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_pin_test_table);

    rex_def_timer_ex(&gsdi_file_attr_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_file_attr_test_table);

    rex_def_timer_ex(&gsdi_select_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_select_test_table);

    rex_def_timer_ex(&gsdi_sim_cap_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_sim_capability_test_table);

    rex_def_timer_ex(&gsdi_increase_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_increase_test_table);

    rex_def_timer_ex(&gsdi_search_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_search_test_table);

    rex_def_timer_ex(&gsdi_open_channel_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_open_channel_test_table);

    rex_def_timer_ex(&gsdi_stream_apdu_api_next_test_timer, gsdi_api_tester_start_next_api_test,
      (unsigned long)gsdi_api_tester_generic_stream_apdu_test_table);

#ifdef FEATURE_GSTK
    rex_def_timer (&gsdi_gstk_network_resel_access_env_test_timer, &gsdi_task_tcb,
    GSDI_API_TESTER_NETWORK_RESEL_ACCESS_ENV_TIMER_SIG);

#endif /*FEATURE_GSTK */
#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
    rex_def_timer (&gsdi_event_test_kick_off_timer, &gsdi_task_tcb,
        GSDI_EVENT_TESTER_KICK_OFF_TIMER_SIG);
#endif /* FEATURE_MMGSDI_ENABLE_EVENT_TESTER */


#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
    rex_def_timer_ex(&gsdi_gstk_cmd_unit_tester_timer, gsdi_gstk_cmd_unit_tester_next_action,
      (unsigned long)0);
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */

#ifdef FEATURE_GSTK
    /* Initialize GSDI - GSTK Timers */
    rex_def_timer (&gsdi_gstk_timer, &gsdi_task_tcb, GSDI_REX_TIMER_EXPIRE_SIG);
#endif /* FEATURE_GSTK */

    /* ---------------------------------------------------------
        INITIALIZE ALL QUEUES
    --------------------------------------------------------- */
    (void) q_init(&gsdi_high_pri_cmd_q);
    (void) q_init(&gsdi_task_cmd_q);

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_DGSM || FEATURE_MMGSDI_UMTS */

    /* --------------------------------------------------------
       INITIALIZE GLOBALS
    -------------------------------------------------------- */

#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

    /* Initialize gsdi_data global variables */
    memset(&gsdi_data,0x00, sizeof(gsdi_data_T));

    /* Need to set to TRUE in order for the first get sim capability to
       be processed correctly */
    gsdi_data.just_invalidated   = TRUE;
    gsdi_data.just_rehabilitated = TRUE;
    /* Have a default value for sanity timer in case commands are
       sent to UIM before receiving a link established */
    gsdi_data.gsdi_uim_sanity_timer_value = GSDI_UIM_SANITY_TIMER_VALUE;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_data.just_invalidated_2   = TRUE;
    gsdi_data.just_rehabilitated_2 = TRUE;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* initialize the card_applications structure to zero */
    memset(&gsdi_data.card_applications, 0x00, sizeof(gsdi_card_apps_T));

    /* UIM Related Globals */
    gsdi_data.uim_error_condition = UIM_NO_SUCH_STATUS_S;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
    gsdi_reg_callback(gsdi_api_tester_event_handler_cb);
#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
    gsdi_reg_callback(gsdi_event_tester_event_result_verification_cb);
    rex_set_timer (&gsdi_event_test_kick_off_timer, GSDI_EVENT_TEST_KICK_OFF_TIME);
#endif /* FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

}

/*===========================================================================
FUNCTION GSDI_HANDLE_ASYNC_LIB_REQ

DESCRIPTION
  This function will serve as the Callback Function from the GSDI to
  itself.  The purpose of this function is to discard the Message
  generated when Synch'ing the CACHE and SIM/USIM.

  Current Implementation can not keep track of the item that failed.

DEPENDENCIES
  FEATURE_GSDI_OPTIMIZATION

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_handle_async_lib_rsp( gsdi_cnf_T *gsdi_cnf )
{
#ifdef FEATURE_GSDI_OPTIMIZATION
   /* Don't do anything */
   if(gsdi_cnf == NULL)
   {
     MSG_ERROR("gsdi_handle_async_lib_rsp, gsdi_cnf is NULL",0,0,0);
     return;
   }

   MSG_HIGH("COMPLETED SYNC QUEUE AND CACHE ITEM",0,0,0);

   if ( gsdi_cnf->message_header.gsdi_status != GSDI_SUCCESS )
   {
       ERR("SYNC SIM AND CACHE FAILED",0,0,0);
   }
#else
   (void) gsdi_cnf;
   MSG_ERROR("gsdi_handle_async_lib_rsp, GSDI NOT SUPPORTED",0,0,0);
#endif /* FEATURE_GSDI_OPTIMIZATION */
}

/*===========================================================================
FUNCTION GSDI_CM_IN_TRAFFIC_CHANNEL

DESCRIPTION

  This function is a utility function for CM to notify GSDI when the Mobile is
  in a traffic channel.  It will also be used to indicate when the Mobile
  is no longer in a traffic channel.

  When called, this function (which executes in the context of the caller) will
  vote the TC Bit in the UIM Drivers On or Off.

  When TC is Voted on in uim_power_control, the UIM Drivers will POLL the SIM.
  When TC is Voted off in uim_power_control, the UIM will not POLL the SIM.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_cm_in_traffic_channel (
  boolean in_TC
)
{
#ifndef FEATURE_VIRTUAL_SIM
    if ( in_TC )
    {
        MSG_HIGH("Start Polling", 0, 0, 0);
        /* A Call has been established  */
        /* Vote UIM to POLL the Card    */
        (void)uim_power_control(UIM_TC, TRUE);
    }
    else
    {
        MSG_HIGH("Stop Polling", 0, 0, 0);
        /* (All) Call(s) has been Torn Down   */
        /* Vote UIM to not Poll Card          */
        (void)uim_power_control(UIM_TC, FALSE);
    }
#endif /* FEATURE_VIRTUAL_SIM */
}

/*===========================================================================
FUNCTION GSDI_CM_IN_TRAFFIC_CHANNEL_W_MODE

DESCRIPTION

  This function is a utility function for CM to notify GSDI when the Mobile is
  in a traffic channel.  It will also be used to indicate when the Mobile
  is no longer in a traffic channel.

  When called, this function (which executes in the context of the caller) will
  vote the TC Bit in the UIM Drivers On or Off.

  When TC is Voted on in uim_power_control, the UIM Drivers will POLL the SIM.
  When TC is Voted off in uim_power_control, the UIM will not POLL the SIM.

  The mode will control with TC Bit is voted on / off.

  GSDI_IN_TC_GW:  Poll a GSM/USIM Card
  GSDI_IN_TC_1X:  Poll a RUIM Card

DEPENDENCIES
  Require UIM to poll the correct slot based on the Mask set.

RETURN VALUE
  void

===========================================================================*/
void gsdi_cm_in_traffic_channel_w_mode (
    boolean in_TC,
    gsdi_in_tc_mode_enum_type mode
)
{
    uim_voter_type mask = UIM_NONE;

    switch ( mode )
    {
    case GSDI_IN_TC_GW:
      mask = UIM_TC;
      break;

#ifndef FEATURE_VIRTUAL_SIM
    case GSDI_IN_TC_1X:
      mask = UIM_CDMA_TC;
      break;
#endif /* FEATURE_VIRTUAL_SIM */

    case GSDI_NOT_IN_TC:
    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD MODE RECVD: %x",mode,0,0);
      return;
    }


    MSG_HIGH("POLLING: %x  MASK: %x  MODE: %x",in_TC,mask,mode);
    (void)uim_power_control(mask,in_TC);

} /* gsdi_cm_in_traffic_channel_w_mode */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_CACHE_PIN_STATUS_SET_GLOBALS

DESCRIPTION
  Will perform the get_pin_status information based on information passed
  into the function

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - PINS Successfully cached
                   GSDI_ERROR     - General Failure Condition
SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_returns_T gsdi_cache_pin_status_set_globals(
  gsdi_get_pin_status_req_T * request,
  gsdi_app_enum_type  app
)
{
  gsdi_returns_T      gsdi_status       = GSDI_ERROR;
  pin_status_info_T * pin_status_info_p = NULL;
  boolean             dummy[2]          = {FALSE,FALSE};
  gsdi_pin_status_T   pin_status        = GSDI_PIN_NOT_INITIALISED;
  pin_status_info_T   slot1_pin_info;

  MMGSDI_RETURN_IF_NULL(request);

  switch ( request->message_header.slot)
  {
  case GSDI_SLOT_2:
    pin_status_info_p = &gsdi_data.pin_status_info_2;
    break;

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x", request->message_header.slot,0,0);
    return GSDI_ERROR;
  }

  switch ( app )
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_CDMA
  case GSDI_CDMA_APP:
    gsdi_status = gsdi_cdma_proc_get_pin_status( request,
                                                &pin_status,
                                                dummy);
    break;
#endif /* FEATURE_MMGSDI_CDMA */

  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Application: %x",app,0,0);
    gsdi_status = GSDI_ERROR;
    break;
  }

  if (gsdi_status == GSDI_SUCCESS)
  {
    switch ( request->pin_id )
    {
    case GSDI_PIN1:
      /* save pin1_status to the global data */
      pin_status_info_p->pin1_status = pin_status;

      /* store pin status information in the gsdi_app_pin_info table */
      (void)gsdi_add_to_app_pin_info_table( NULL,
                                      GSDI_PIN1,
                                      NULL,
                                      pin_status);
      break;

    case GSDI_PIN2:
      pin_status_info_p->pin2_status       = pin_status;

      /* store pin status information in the gsdi_app_pin_info table */
      (void)gsdi_add_to_app_pin_info_table( NULL,
                                      GSDI_PIN2,
                                      NULL,
                                      pin_status);

      pin_status_info_p->pin_status_cached = TRUE;
      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD PIN ID: %x",request->pin_id,0,0);
      break;
    }
  }
  return gsdi_status;
} /* gsdi_cache_pin_status_set_globals */
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

/*===========================================================================
FUNCTION GSDI_CACHE_PIN_STATUS

DESCRIPTION
  This function is called to cache the Status of the following PINs:
  USIM:  PIN1 (Global PIN) & USIM Application PIN2
  GSM:   CHV1 & CHV2
  RUIM:  CHV1 & CHV2

  Will utilize the Local Function GSDI_CACHE_PIN_STATUS_SET_GLOBALS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - PINS Successfully cached
                   GSDI_ERROR     - General Failure Condition
SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_cache_pin_status( void )
{
  gsdi_returns_T                  gsdi_status = GSDI_SUCCESS;

  /* Call Various Functions based on the Card Applications
  ** Bit Map
  */

  /* Is there a Card in Slot 1 */
  if ( gsdi_data.card_applications.slot1_present )
  {
    gsdi_status                 = GSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS) || defined(FEATURE_MMGSDI_CDMA)
    if (!mmgsdi_util_is_sel_app_pin_cached())
    {
      /* Is there a GSM APP Present ? */
      if (  ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK ) ==
              GSDI_GSM_SIM_MASK  ||
            ( gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK ) ==
             GSDI_RUIM_APP_MASK )
      {
        if(mmgsdi_icc_build_get_pin_status() != MMGSDI_SUCCESS)
        {
          MSG_ERROR("Could not Queue PIN Status Command",0,0,0);
          return GSDI_ERROR;
        }
      }
    }/* pin status hasn't been cached */
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS || FEATURE_MMGSDI_CDMA */
  }
  else
  {
    MSG_HIGH("NOT CACHING PIN STATUS FOR SLOT1/CARD1",0,0,0);
  } /* if  gsdi_data.card_applications.slot1_present */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Slot two PIN STATUS Information is cached in
  ** pin_status_info_2
  */
  if ( gsdi_data.card_applications.slot2_present )
  {
    gsdi_get_pin_status_req_T       request;

    gsdi_status                 = GSDI_SUCCESS;
    request.pin_id              = GSDI_PIN1;
    request.message_header.slot = GSDI_SLOT_2;

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */

#ifdef FEATURE_MMGSDI_CDMA
    if (!gsdi_data.pin_status_info_2.pin_status_cached)
    {
      if ( ( gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK ) ==
             GSDI_RUIM_APP_MASK )
      {
        /* NO GSM APP...ONLY CDMA APP PRESENT */
        /* pin1 status */
        gsdi_status = gsdi_cache_pin_status_set_globals(&request,
                                                        GSDI_CDMA_APP);
        if (gsdi_status == GSDI_SUCCESS)
        {
          /* proceed with PIN2 */
          request.pin_id = GSDI_PIN2;
          gsdi_status = gsdi_cache_pin_status_set_globals(&request,
                                                          GSDI_CDMA_APP);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MMGSDI_DEBUG_MSG_ERROR("Could not Cache PIN2 Status: %x",gsdi_status,0,0);
          }
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR("Could not Cache PIN1 Status: %x",gsdi_status,0,0);
        }
      }
    } /* pin hasn't been cached yet */
#endif /*FEATURE_MMGSDI_CDMA*/
    if (gsdi_status != GSDI_SUCCESS)
    {
      /* have difficulties caching pin */
      /* Send a card error event via the registered callback */
      MMGSDI_DEBUG_MSG_ERROR("NO GSM/ CDMA/ USIM APP PRESENT ",0,0,0);
      gsdi_status = gsdi_util_send_card_error(GSDI_SLOT_2);
    }
  }
  else
  {
    MSG_HIGH("NOT CACHING PIN STATUS FOR SLOT2/CARD2",0,0,0);
  } /* if gsdi_data.card_applications.slot2_present */
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_ADD_TO_APP_PIN_INFO_TABLE

DESCRIPTION
  Function is called by all the PIN functions () to add the application-pin info
  to the gsdi_app_pin_info_table.

INPUT
  uint8             *app_id,
  gsdi_pin_ids_T    pin_id1,
  uint8             *pin,
  gsdi_pin_status_T status

RETURN VALUE
  gsdi_returns_T

OUTPUT
  An entry is added to the  gsdi_app_pin_info_table.In case of a return value
  of GSDI_ERROR, no return value is added.
===========================================================================*/
gsdi_returns_T gsdi_add_to_app_pin_info_table(
  const gsdi_app_id_type *app_id,
  gsdi_pin_ids_T          pin_id,
  const uint8            *pin,
  gsdi_pin_status_T       status)
{
    if(pin_id == GSDI_PIN1)
    {
      if (app_id != NULL)
      {
        memcpy(&gsdi_app_pin_info_table[0].app_id, app_id,
             sizeof(gsdi_app_id_type));
      }
      gsdi_app_pin_info_table[0].pin1.pin_id = pin_id;
      gsdi_app_pin_info_table[0].pin1.status = status;
      if(pin != NULL)
      {
        memcpy(gsdi_app_pin_info_table[0].pin1.pin, pin,
             GSDI_PIN_LEN);
        gsdi_app_pin_info_table[0].pin1.is_pin_available = TRUE;

      }
      return GSDI_SUCCESS;
    }
    else if(pin_id == GSDI_PIN2)
    {
      if (app_id != NULL)
      {
        memcpy(&gsdi_app_pin_info_table[0].app_id, app_id,
             sizeof(gsdi_app_id_type));
      }
      gsdi_app_pin_info_table[0].pin2.pin_id = pin_id;
      gsdi_app_pin_info_table[0].pin2.status = status;
      if(pin != NULL)
      {
        memcpy(gsdi_app_pin_info_table[0].pin2.pin, pin,
             GSDI_PIN_LEN);
        gsdi_app_pin_info_table[0].pin2.is_pin_available = TRUE;
      }
      return GSDI_SUCCESS;
    }
    return GSDI_ERROR;
}

/*===========================================================================

FUNCTION GSDI_PROC_GET_ECC

DESCRIPTION
  - This function will be used to retrieve the Emergency Call Codes retrieved
    by the GSDI during the GSDI & SIM/USIM/RUIM Initialization Procedures.
  - This function will populate a gsdi_ecc_data_T structure passed in
    as a parameter.

INPUT
  gsdi_slot_id_type slot
  gsdi_ef_ecc_data_T * ecc_p

DEPENDENCIES
  Dependency on Global Variable gsdi_ef_ecc_data_T ecc_data.  This
  structure must before this function can be used.

RETURN VALUE
  gsdi_returns_T.

OUTPUT
  None

===========================================================================*/

gsdi_returns_T gsdi_proc_get_ecc(
    gsdi_slot_id_type slot
 )
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. ecc_p cannot be null
  ----------------------------------------------------------------------------*/

  switch (slot)
  {
    case GSDI_SLOT_1:
      if(gsdi_data.protocol == GSDI_PROTOCOL_GSM)
      {
        if(gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
        {
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */
        }
        if(gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
        {
#ifdef FEATURE_MMGSDI_CDMA
          gsdi_status = gsdi_util_map_to_ecc_table(&ruim_ecc_data);
#endif /* FEATURE_MMGSDI_CDMA */
        }
      }
      else if (gsdi_data.protocol == GSDI_PROTOCOL_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
      }
      else
      {
        ERR("SLOT 1 BAD GSDI PROTOCOL: 0x%x",gsdi_data.protocol,0,0);
        return GSDI_ERROR;
      }
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      if(gsdi_data.protocol == GSDI_PROTOCOL_GSM)
      {
        if(gsdi_data.card_applications.apps2_enabled & GSDI_GSM_SIM_MASK)
        {
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
        }
        if(gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK)
        {
#ifdef FEATURE_MMGSDI_CDMA
          gsdi_status  = gsdi_util_map_to_ecc_table(&ruim2_ecc_data);
#endif /* FEATURE_MMGSDI_CDMA */
        }
      }
      else if (gsdi_data.protocol == GSDI_PROTOCOL_UICC)
      {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
      }
      else
      {
        ERR("SLOT 2 BAD GSDI PROTOCOL: 0x%x",gsdi_data.protocol,0,0);
        return GSDI_ERROR;
      }
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
      MMGSDI_DEBUG_MSG_ERROR ("Unknown slot %d", slot, 0, 0);
      return GSDI_ERROR;
  }
  return gsdi_status;
}


/*=============================================================================
FUNCTION MMGSDI_INIT_CACHE_BINARY

DESCRIPTION
  This function will initialize data in the cache during the initialization
  process. The third argument data_ptr is used to retrieve the value of
  the file: this function allocates the required memory and the caller is
  responsible for freeing it. data_ptr can be NULL in case the value is not
  required.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_init_cache_binary(gsdi_slot_id_type     gsdi_slot,
                                                 mmgsdi_file_enum_type file,
                                                 mmgsdi_data_type     *data_ptr)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_read_req_type               *req_ptr              = NULL;
  mmgsdi_read_cnf_type               *cnf_ptr              = NULL;
  mmgsdi_access_type                  access;
  uim_items_enum_type                 uim_file_items       = UIM_NO_SUCH_ITEM;
  boolean                             file_ok_in_svr_table = TRUE;
  mmgsdi_slot_id_enum_type            slot                 = MMGSDI_SLOT_1;
  uint32                              data_len             = 0;

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&access,
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_slot(FALSE, &gsdi_slot, &slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Check status of EF in cached SST or CST */
  file_ok_in_svr_table = mmgsdi_util_is_file_ok_in_svc_table( &slot,
                                                              &access );

  if (file_ok_in_svr_table)
  {
    /* Check if the file is already cached. In this case, there is no need
       to read it again from the card */
    mmgsdi_access_type file_access;
    mmgsdi_len_type    file_len    = 0;

    memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
    file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = file;
      
    mmgsdi_status = mmgsdi_util_read_cache_len(slot,
                                               &file_access,
                                               &file_len,
                                               0);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      data_len = int32touint32(file_len);
      if (data_len > 0 && data_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(data_ptr->data_ptr,
                                               data_len,
                                               mmgsdi_status);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          data_ptr->data_len = file_len;
          mmgsdi_status = mmgsdi_util_read_cache(slot,
                                                 &file_access,
                                                 data_ptr,
                                                 0);
          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            /* In case of error, free memory */
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data_ptr->data_ptr);
            data_ptr->data_len = 0;
          }
        }
      }
    }
    /* If cache was already initialized and is read correctly
       return success now, without proceeding. Otherwise, continue
       with the normal code */
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      MSG_HIGH("Cache already inited for file 0x%x", file, 0, 0);
      return mmgsdi_status;
    }

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(req_ptr, sizeof(mmgsdi_read_req_type));

    req_ptr->request_header.client_id = mmgsdi_generic_data.client_id;
    req_ptr->request_header.request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.request_len = uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.payload_len = req_ptr->request_header.request_len -
                                          uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.request_id_ptr = rex_self();
    req_ptr->request_header.slot_id = slot;
    req_ptr->request_header.client_data = 0;
    req_ptr->request_header.response_cb = NULL;
    req_ptr->access.access_method = MMGSDI_EF_ENUM_ACCESS;
    req_ptr->access.file.file_enum = file;
    req_ptr->rec_num = 0;
    req_ptr->offset = 0;
    req_ptr->file_type = MMGSDI_TRANSPARENT_FILE;

    if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK) ||
        (gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK))
    {
      mmgsdi_status = mmgsdi_uim_icc_read_transparent (req_ptr,
                                           TRUE,
                                           &cnf_ptr);
    }
    /* Is this okay to be featurized what happens if mmgsdi_status is not set? */
    #ifdef FEATURE_MMGSDI_UMTS
#error code not present
    #endif /* FEATURE_MMGSDI_UMTS */

    if (cnf_ptr == NULL)
    {
      MSG_ERROR("Cnf Ptr Null after read", 0, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
      return MMGSDI_ERROR;
    }
    if( (mmgsdi_status == MMGSDI_SUCCESS) &&
        (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      if (data_ptr != NULL)
      {
        data_ptr->data_len = cnf_ptr->read_data.data_len;
        data_len           = int32touint32(data_ptr->data_len);

        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(data_ptr->data_ptr,
                                               data_len,
                                               mmgsdi_status);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(data_ptr->data_ptr, cnf_ptr->read_data.data_ptr,
            int32touint32(cnf_ptr->read_data.data_len));
        }
        else
        {
          data_ptr->data_len = 0;
          mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
          /* do not need to free req_ptr because it has been handled by
             the synchronous portion of the code */
          return mmgsdi_status;
        }
      }
    }
    if ( mmgsdi_status != MMGSDI_UIM_INTERNALLY_RESET)
    {
      mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
    }
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
    /* do not need to free req_ptr because it has been handled by
       the synchronous portion of the code */

    return mmgsdi_status;
  }
  else
  {
    MSG_HIGH("Service Table Bit not Set for file 0x%x", file, 0, 0);
    return MMGSDI_NOT_SUPPORTED;
  }
} /* mmgsdi_init_cache_binary */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#if defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
/*=============================================================================
FUNCTION MMGSDI_INIT_CACHE_RECORD

DESCRIPTION
  This function will initialize data in the cache during the initialization
  process for record file

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_init_cache_record(
  gsdi_slot_id_type     gsdi_slot,
  mmgsdi_file_enum_type file,
  mmgsdi_rec_num_type   rec_num,
  mmgsdi_data_type     *data_ptr)
{
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_read_req_type              *req_ptr = NULL;
  mmgsdi_read_cnf_type              *cnf_ptr = NULL;
  mmgsdi_access_type                access;
  uim_items_enum_type               uim_file_items = UIM_NO_SUCH_ITEM;
  boolean                           file_ok_in_svr_table = TRUE;
  mmgsdi_slot_id_enum_type          slot     = MMGSDI_SLOT_1;
  uint32                            data_len = 0;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(&access,
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_slot(FALSE, &gsdi_slot, &slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Check status of EF in cached SST or CST */
  file_ok_in_svr_table = mmgsdi_util_is_file_ok_in_svc_table( &slot,
                                                              &access );

  if (file_ok_in_svr_table)
  {


    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(req_ptr, sizeof(mmgsdi_read_req_type));

    req_ptr->request_header.client_id = mmgsdi_generic_data.client_id;
    req_ptr->request_header.request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.request_len = uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.payload_len = req_ptr->request_header.request_len -
                                          uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.request_id_ptr = rex_self();
    req_ptr->request_header.slot_id = MMGSDI_SLOT_1;
    req_ptr->request_header.client_data = 0;
    req_ptr->request_header.response_cb = NULL;
    req_ptr->access.access_method = MMGSDI_EF_ENUM_ACCESS;
    req_ptr->access.file.file_enum = file;
    req_ptr->rec_num = rec_num;
    req_ptr->offset = 0;
    req_ptr->file_type = MMGSDI_LINEAR_FIXED_FILE;

    if (gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
    {
      mmgsdi_status = mmgsdi_uim_icc_read_record (req_ptr,
                                           TRUE,
                                           &cnf_ptr);
    }
    #ifdef FEATURE_MMGSDI_UMTS
#error code not present
    #endif /* FEATURE_MMGSDI_UMTS */
    if (cnf_ptr == NULL)
    {
      MSG_ERROR("Cnf Ptr Null after read", 0, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
      return MMGSDI_ERROR;
    }
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      if (data_ptr != NULL)
      {
        data_ptr->data_len = cnf_ptr->read_data.data_len;
        data_len           = int32touint32(data_ptr->data_len);

        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(data_ptr->data_ptr,
                                               data_len,
                                               mmgsdi_status);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(data_ptr->data_ptr, cnf_ptr->read_data.data_ptr,
            int32touint32(cnf_ptr->read_data.data_len));
        }
        else
        {
          data_ptr->data_len = 0;
          mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
          /* do not need to free req_ptr because it has been handled by
             the synchronous portion of the code */
          return mmgsdi_status;
        }
      }
    }
    mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
    /* do not need to free req_ptr because it has been handled by
       the synchronous portion of the code */
    return mmgsdi_status;
  }
  else
  {
    MSG_HIGH("Service Table Bit not Set for file 0x%x", file, 0, 0);
    return MMGSDI_NOT_SUPPORTED;
  }
} /* mmgsdi_init_cache_record */
#endif /* FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */


#ifdef FEATURE_MMGSDI_CDMA
/*===========================================================================
FUNCTION GSDI_ICC_CDMA_POST_PIN1_INIT

DESCRIPTION
  This function is used to cache all Call Stack Related Files from the ICC.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:
===========================================================================*/
gsdi_returns_T gsdi_icc_cdma_post_pin1_init (
  gsdi_slot_id_type slot
)
{
    gsdi_returns_T          gsdi_status = GSDI_SUCCESS;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
    mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
    boolean                 slot_present = FALSE;
    uint8                   enabled_apps = 0x00;
    boolean                 initializing_operational_slot = FALSE;
    gsdi_store_esn_req_T    store_esn_req;
    nv_stat_enum_type       nv_return_staus;
    nv_item_type            nv_data;
  
#ifdef FEATURE_UIM_EUIMID
    nvruim_esn_usage_type   ruim_usage_type;
    boolean                 is_meid_available = FALSE;
#endif /* FEATURE_UIM_EUIMID */
    MSG_MED("ICC post pin1 proc",0,0,0);

    /* Determine GSM or CDMA technology */

    switch (slot)
    {
      case GSDI_SLOT_1:
        slot_present = gsdi_data.card_applications.slot1_present;
        enabled_apps = gsdi_data.card_applications.apps_enabled;
        store_esn_req.message_header.slot = slot;
        if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_1_MASK)
                                                         == GSDI_CDMA_SLOT_1_MASK)
        {
          initializing_operational_slot = TRUE;
        }
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
        slot_present = gsdi_data.card_applications.slot2_present;
        enabled_apps = gsdi_data.card_applications.apps2_enabled;
        store_esn_req.message_header.slot = slot;
        if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_2_MASK)
                                                         == GSDI_CDMA_SLOT_2_MASK)
        {
          initializing_operational_slot = TRUE;
        }
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

      case GSDI_SLOT_AUTOMATIC:
        if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_1_MASK)
                                                         == GSDI_CDMA_SLOT_1_MASK)
        {
          /* go to slot 1 */
          slot_present = gsdi_data.card_applications.slot1_present;
          enabled_apps = gsdi_data.card_applications.apps_enabled;
          slot = GSDI_SLOT_1;
          store_esn_req.message_header.slot = slot;
          initializing_operational_slot = TRUE;
        }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        else if ((gsdi_data.card_applications.operational_slot & GSDI_CDMA_SLOT_2_MASK)
                                                         == GSDI_CDMA_SLOT_2_MASK)
        {
          /* go to slot 2 */
          slot_present = gsdi_data.card_applications.slot2_present;
          enabled_apps = gsdi_data.card_applications.apps2_enabled;
          slot = GSDI_SLOT_2;
          store_esn_req.message_header.slot = slot;
          initializing_operational_slot = TRUE;
        }
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
        else {
          MMGSDI_DEBUG_MSG_ERROR("Unknown operational slot for USIM", 0, 0, 0);
          return GSDI_ERROR;
        }
        break;

      case GSDI_SLOT_NONE:
        MMGSDI_DEBUG_MSG_ERROR("Unknown GSDI Slot %x", slot, 0, 0);
        return GSDI_ERROR;
    }

    if ( slot_present &&
        ((enabled_apps & GSDI_RUIM_APP_MASK) == GSDI_RUIM_APP_MASK))
    {
        gsdi_status = gsdi_cdma_proc_get_ruim_capabilities(slot);
        if (gsdi_status != GSDI_SUCCESS)
        {
            MMGSDI_DEBUG_ERR("Invalid get ruim capabilities", 0, 0, 0);
        }
    }

    if (!initializing_operational_slot) {
      /* No need to continue with the rest of the initialization */
      return gsdi_status;
    }

    if ( gsdi_status != GSDI_SUCCESS)
    {
       ERR("Invalid phase2 init status %d",gsdi_status,0,0);
    }


#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
    store_esn_req.esn_usage  = GSDI_RUIM_STORE_ESN_USE_UIM_ID;
    store_esn_req.esn_length = GSDI_RUIM_STORE_ESN_LENGTH;
#ifdef FEATURE_UIM_EUIMID
    store_esn_req.meid_available = FALSE;
    /* Attempt to read MEID, if successful and CST n9 is activated then
       store_esn command should be sent with MEID.  If CST n8 is on and ME
       provisioned with MEID then usage indicator bit2 should also be
       interperted. */
    nv_return_staus = gsdi_get_nv(NV_MEID_ME_I, (nv_item_type *) &nv_data);
    if (nv_return_staus == NV_DONE_S)
    {
      MSG_HIGH("Read MEID from NV", 0,0,0);
      is_meid_available = TRUE;
    }
    /* check if service 9 is supported and activated */
    if ((gsdi_is_meid_svc_activated(slot)) && (TRUE == is_meid_available))
    {
      store_esn_req.esn_usage  = GSDI_RUIM_STORE_MEID_USE_EUIM_ID;
      store_esn_req.esn_length = GSDI_RUIM_STORE_MEID_LENGTH;
      qw_equ(store_esn_req.meid, nv_data.meid);
      store_esn_req.meid_available = TRUE;
    }
    if (!store_esn_req.meid_available)
#endif /* FEATURE_UIM_EUIMID */
    {
#ifndef FEATURE_VIRTUAL_SIM
      /* Need to get ESN Using TMC Interface */
      store_esn_req.esn = tmc_get_stored_esn_me();
#else
      store_esn_req.esn = 1111;
#endif /* FEATURE_VIRTUAL_SIM */
    }

    gsdi_status = gsdi_uim_server_store_esn(&store_esn_req);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("STORE_ESN_USE_UIM_ID Failed: %x",gsdi_status,0,0);
      return gsdi_status;
    }

    /* EF UIM_ID Usage Indicator */
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(mmgsdi_init_data,
              sizeof(mmgsdi_data_type), mmgsdi_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      ERR("gsdi_icc_cdma_post_pin1_init: mmgsdi_malloc Failed", 0, 0, 0);
      return GSDI_ERROR;
    }
    mmgsdi_status = mmgsdi_init_cache_binary(slot,MMGSDI_CDMA_UIM_ID_USAGE_IND,mmgsdi_init_data);

    if ( mmgsdi_status == MMGSDI_SUCCESS )
    {
#ifndef FEATURE_VIRTUAL_SIM
#ifdef FEATURE_UIM_EUIMID
      if (is_meid_available)
      {
        gsdi_get_meid_usage_type(slot, mmgsdi_init_data->data_ptr[0],
                                 &ruim_usage_type);
        nvruim_set_esn_usage( ruim_usage_type);
      }
      else
#endif /* FEATURE_UIM_EUIMID */
      {
        /* Check usage indicator bit */
        if (mmgsdi_init_data->data_ptr[0] & GSDI_RUIM_ESN_USAGE_MASK )
        {
          /* Set the ESN Usage to read the RUIM ID for ESN */
          nvruim_set_esn_usage( NV_RUIM_USE_RUIM_ID);
        }
        else
        {
          nvruim_set_esn_usage( NV_RUIM_USE_ESN);
        }
      }
#endif /* FEATURE_VIRTUAL_SIM */
#if defined (FEATURE_OTASP) || defined (FEATURE_UIM_RUIM)
      /* Add Code to call Diag Function to update Cache */
      diag_otasp_update();
#endif /* FEATURE_OTASP || FEATURE_UIM_RUIM */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_init_data->data_ptr);
#ifndef CUST_EDITION
#if defined (FEATURE_UIM_TOOLKIT_UTK) && !defined (FEATURE_GSTK)
      /* Need to execute the Toolkit Download */
      /* Read CDMA Service Table */
      mmgsdi_status = mmgsdi_init_cache_binary(slot,
                                               MMGSDI_CDMA_SVC_TABLE,
                                               mmgsdi_init_data);

      if ( mmgsdi_status == MMGSDI_SUCCESS )
      {
        /* Determine if Service is Allocated and Activated */
        if ( ( (mmgsdi_init_data->data_ptr[GSDI_CDMA_SST_TOOLKIT_OFFSET]) & GSDI_CDMA_SST_TOOLKIT_MASK ) == GSDI_CDMA_SST_TOOLKIT_MASK )
        {
          /* Tookit is available */

          /* Set the info on Pro-active UIM */
          uim_set_proactive_uim ( TRUE );

          /* Need to send Terminal Profile */
          gsdi_status = gsdi_uim_server_terminal_profile(slot);
          if (gsdi_status != GSDI_SUCCESS)
            mmgsdi_status = MMGSDI_ERROR;
        }
      }
#endif /*FEATURE_UIM_TOOLKIT_UTK && !FEATURE_GSTK */
#endif
    }
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
    gsdi_cdma_svc_nvruim_init(slot);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_init_data->data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_init_data);
    if (mmgsdi_status != MMGSDI_SUCCESS)
      return GSDI_ERROR;
    else
      return GSDI_SUCCESS;
} /* gsdi_icc_cdma_post_pin1_init */
#endif /* FEATURE_MMGSDI_CDMA */

/*===========================================================================
FUNCTION MMGSDI_PROC_POST_PIN1_INIT

DESCRIPTION
  Based on the mobiles capability, call the appropriate initialization
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T mmgsdi_proc_post_pin1_init(
  gsdi_slot_id_type slot,
  int32             *returned_data_len,
  uint8             *data_ext_p
)
{
    gsdi_returns_T   gsdi_status    = GSDI_ERROR;
    uint8 *          card_apps_p    = NULL;
    uint8            card_apps      = 0x00;
    mmgsdi_int_app_info_type  *sel_app_ptr   = NULL;

    card_apps_p = gsdi_util_get_apps_enabled(slot);

    /* ------------------------------------------------------------------------
    Verify the card_apps_p was properly set.  If not set, (i.e.
    set to NULL), abort the Post PIN1 Procedures
    -------------------------------------------------------------------------*/
    if ( card_apps_p == NULL )
      return GSDI_ERROR;

    /* ------------------------------------------------------------------------
    Vote the UIM Clock On before starting the Post PIN1 Procedures.
    Will keep the clock from being continously turned off / on for
    each one of the requests that follows
    --------------------------------------------------------------------------*/
    (void)uim_power_control ( UIM_USIM_SESSION, TRUE );

#ifdef FEATURE_MMGSDI_CPHS
    /* ------------------------------------------------------------------------
    POST PIN1 Initialization procedures for CPHS.  The determination of which
    card type to use, GSM or USIM is handled within the cphs post pin1 init
    function.
    -------------------------------------------------------------------------*/
    gsdi_status = gsdi_proc_cphs_post_pin1_init(slot,
                                                returned_data_len,
                                                data_ext_p,
                                                TRUE, /*Read CPHS*/
                                                TRUE);/*Read CSP*/
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("CPHS POST PIN1 INIT FUNCTION FAILURE: 0x%x",gsdi_status,0,0);
      /* Reset the gsdi_status - No CPHS does not mean
      ** we need to go to NO Card
      */
      gsdi_status = GSDI_SUCCESS;
    }
#endif /* FEATURE_MMGSDI_CPHS */

    /* ------------------------------------------------------------------------
    POST PIN1 Initialization procedures are now based on the applications
    present on the card and the slot for which these procedures should be
    followed on.
    -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
    card_apps = *card_apps_p & GSDI_RUIM_APP_MASK;
    if ( card_apps )
      gsdi_status = gsdi_icc_cdma_post_pin1_init(slot);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("ICC CDMA POST PIN1 INIT FAILED: 0x%x SLOT: 0x%x",gsdi_status,slot,0);
      gsdi_remove_apps_from_apps_enabled(slot, GSDI_RUIM_APP_MASK);
    }
#endif /* FEATURE_MMGSDI_CDMA */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

    sel_app_ptr = mmgsdi_util_get_sel_app();
    if(sel_app_ptr == NULL)
    {
      MSG_ERROR("sel_app_ptr is NULL",0,0,0);
      return GSDI_ERROR;
    }
    /* ------------------------------------------------------------------------
       Queue in a command to send out a status command to indicate that the
       ME is ready for a 3G session If this is an UICC.  This also solves the
       purpose of sending a case 2 command with le > luic for testing
       GCF test case 7.2.3 if this is an UICC.  For a sim this would translate
       to a regular status command.
       ----------------------------------------------------------------------*/
    (void) mmgsdi_3G_ready_status_ind(MMGSDI_SLOT_1);

    return gsdi_status;
} /* mmgsdi_proc_post_pin1_init */


/*===========================================================================
FUNCTION GSDI_GET_PROTOCOL

DESCRIPTION
  This function returns the protocol for the particular slot.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_get_protocol (
  gsdi_slot_id_type        slot,
  gsdi_protocol_enum_type *protocol_ptr
)
{
  if (protocol_ptr == NULL)
    return GSDI_ERROR;

  switch(slot)
  {
  case GSDI_SLOT_1:
    *protocol_ptr = gsdi_data.protocol;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    MSG_ERROR("Unable to Handle slot 2 protocol now", 0, 0, 0);
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    return GSDI_ERROR;
  }
  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_IS_ENS_ENABLED_IN_NV

DESCRIPTION
  This function checks if ens is enabled in NV.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
===========================================================================*/
boolean gsdi_is_ens_enabled_in_nv(void)
{
#ifdef FEATURE_ENHANCED_NW_SELECTION
  nv_item_type       gsdi_nv_ens_enabled;
  nv_stat_enum_type  nv_status;

  nv_status = gsdi_get_nv(NV_ENS_ENABLED_I,
                          &gsdi_nv_ens_enabled);

  if( (nv_status == NV_DONE_S) && (gsdi_nv_ens_enabled.ens_enabled))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
#else
  return FALSE;
#endif /* FEATURE_ENHANCED_NW_SELECTION */
}

/*lint -esym(714,gsdi2_get_card_mode)*/
/*lint -esym(526,rex_curr_task)*/


#if defined (FEATURE_UIM_EUIMID) && defined (FEATURE_MMGSDI_CDMA)
/*===========================================================================
FUNCTION GSDI_IS_SF_EUIMID_ACTIVATED

DESCRIPTION
  This function checks the service table to determine if SF_EUIMID is allocated
  and activated.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
===========================================================================*/
boolean gsdi_is_sf_euimid_activated(
  gsdi_slot_id_type slot
)
{
   gsdi_sim_read_req_T    *read_req_ptr     = NULL;
   byte                    rec_num_accessed = 0;
   byte                    num_records      = 0;
   byte                    ret_data_len     = 0;
   byte                    data_p[GSDI_CDMA_SST_SF_EUIMID_OFFSET + 1];
   gsdi_returns_T          gsdi_status      = GSDI_SUCCESS;
   boolean                 euimid_supported = FALSE;
   mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_SUCCESS;

   MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
      sizeof(gsdi_sim_read_req_T), mmgsdi_status);
   if (mmgsdi_status != MMGSDI_SUCCESS)
   {
     MSG_ERROR("Could Not Allocate Memory for read_req_ptr pointer", 0,0,0);
     return euimid_supported;
   }

   read_req_ptr->sim_filename = (word)UIM_CDMA_CDMA_SVC_TABLE;
   read_req_ptr->message_header.slot = slot;
   read_req_ptr->required_data_len = GSDI_CDMA_SST_MEID_OFFSET + 1;
   gsdi_status = gsdi_cdma_proc_ruim_read(read_req_ptr,
                                          &rec_num_accessed,
                                          &num_records,
                                          &ret_data_len,
                                          data_p );

   if (gsdi_status != GSDI_SUCCESS)
   {
     MSG_ERROR("Could Not read CDMA Service Table gsdi_status: %d", gsdi_status,
                0,0);
   }
   else if (ret_data_len < GSDI_CDMA_SST_SF_EUIMID_OFFSET + 1)
   {
     MSG_ERROR("CDMA Service Table data length: %d returned is less than offset %d",
               ret_data_len,GSDI_CDMA_SST_SF_EUIMID_OFFSET,0);
   }
   else if (data_p[GSDI_CDMA_SST_SF_EUIMID_OFFSET] & GSDI_CDMA_SST_SF_EUIMID_MASK)
   {
     MSG_HIGH("EUIMID support is allocated and activated", 0,0,0);
     euimid_supported = TRUE;
   }
   else
   {
     MSG_HIGH("EUIMID support is not allocated and activated", 0,0,0);
   }
   MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
   return euimid_supported;
} /* gsdi_is_sf_euimid_activated */


/*===========================================================================
FUNCTION GSDI_GET_MEID_USAGE_TYPE

DESCRIPTION
  This function returns esn usage type

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_get_meid_usage_type(
  gsdi_slot_id_type      slot,
  byte                   data,
  nvruim_esn_usage_type *usage_type_ptr
)
{
   byte usage_ind;
   if (usage_type_ptr == NULL)
   {
     MSG_ERROR("NULL Input Paramter usage_type_ptr",0,0,0);
     return;
   }
   if (gsdi_is_sf_euimid_activated(slot))
   {
     usage_ind = (data & GSDI_RUIM_SF_EUIMID_USAGE_MASK);
     switch (usage_ind)
     {
       case GSDI_RUIM_MEID_UIMID_USAGE_IND:
         /* Use MEID for identification and UIMID for authentication */
         *usage_type_ptr = NV_RUIM_USE_MEID_UIMID;
         break;
       case GSDI_RUIM_SF_EUIMID_ESN_USAGE_IND:
         /* Use SF_EUIMID for identification and ESN for authentication */
         *usage_type_ptr = NV_RUIM_USE_SF_EUIMID_ESN;
         break;
       case GSDI_RUIM_SF_EUIMID_USAGE_IND:
         /* Use SF_EUIMID for identification and UIMID for authentication */
         *usage_type_ptr = NV_RUIM_USE_SF_EUIMID;
         break;
       default:
         /* Use MEID for identification and ESN for authentication */
        *usage_type_ptr = NV_RUIM_USE_MEID;
         break;
     } /* switch usage_ind */
   } /* if sf_euimid activated */
   else
   {
     /* Handle LF_EUIMID/RUIMID card */
     if (data & GSDI_RUIM_ESN_USAGE_MASK )
     {
       *usage_type_ptr = NV_RUIM_USE_RUIM_ID;
     }
     else
     {
       *usage_type_ptr = NV_RUIM_USE_ESN;
     }
   }
} /* gsdi_get_meid_usage_type */


/*===========================================================================
FUNCTION GSDI_IS_MEID_SVC_ACTIVATED

DESCRIPTION
  This function returns if MEID Service is supported and activated

DEPENDENCIES
  None

RETURN VALUE
  Boolean
===========================================================================*/
boolean gsdi_is_meid_svc_activated(
  gsdi_slot_id_type slot
)
{
   gsdi_sim_read_req_T       *read_req_ptr       = NULL;
   byte                      rec_num_accessed = 0;
   byte                      num_records      = 0;
   byte                      ret_data_len     = 0;
   byte                      data_p[GSDI_CDMA_SST_MEID_OFFSET + 1];
   mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_SUCCESS;
   gsdi_returns_T            gsdi_status      = GSDI_SUCCESS;
   boolean                   meid_supported   = FALSE;

   MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
      sizeof(gsdi_sim_read_req_T), mmgsdi_status);
   if (mmgsdi_status != MMGSDI_SUCCESS)
   {
     MSG_ERROR("Could Not Allocate Memory for read_req_ptr pointer", 0,0,0);
     return meid_supported;
   }

   read_req_ptr->sim_filename = (word)UIM_CDMA_CDMA_SVC_TABLE;
   read_req_ptr->message_header.slot = slot;
   read_req_ptr->required_data_len = GSDI_CDMA_SST_MEID_OFFSET + 1;
   gsdi_status = gsdi_cdma_proc_ruim_read(read_req_ptr,
                                          &rec_num_accessed,
                                          &num_records,
                                          &ret_data_len,
                                          data_p );

   /* Check if n9 MEID Support is enabled on the card */
   if (gsdi_status != GSDI_SUCCESS)
   {
     MSG_ERROR("Could Not Read UIM_CDMA_SVC_TABLE gsdi_status returned: %d",
               gsdi_status, 0,0);
   }
   else if (ret_data_len < GSDI_CDMA_SST_MEID_OFFSET+1)
   {
     MSG_ERROR("UIM Service Table read data length: %d is less than offset %d",
               ret_data_len,GSDI_CDMA_SST_MEID_OFFSET,0);
   }
   else if (data_p[GSDI_CDMA_SST_MEID_OFFSET] & GSDI_CDMA_SST_MEID_MASK)
   {
     MSG_HIGH("MEID Service Option activated and allocated",0,0,0);
     meid_supported = TRUE;
   }
   else
   {
     MSG_HIGH("MEID Service Option Not activated and allocated",0,0,0);
   }
   MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
   return meid_supported;
} /* gsdi_is_meid_svc_activated */
#endif /* FEATURE_UIM_EUIMID && FEATURE_MMGSDI_CDMA*/


#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS or FEATURE_MMGSDI_GSM */


#ifdef FEATURE_MMGSDI_MCC_VERIFICATION
/*===========================================================================
FUNCTION GSDI_IS_GCF_TEST_MODE_ACTIVATED

DESCRIPTION
  This function returns status of GPRS_ANITE_GCF NV item

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
boolean gsdi_is_gcf_test_mode_activated(void)
{
  return gsdi_data.gsdi_gprs_anite_nv_status;
}

#endif /*  FEATURE_MMGSDI_MCC_VERIFICATION */


/*===========================================================================
FUNCTION GSDI_PERSO_GET_KEY

DESCRIPTION
  Retrieves the key for the specified perso feature

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_get_key (
  gsdi_slot_id_type    slot,
  gsdi_perso_enum_type feature_ind,
  boolean              is_unblock_key,
  uint32               client_ref,
  gsdi_async_cb        async_cb)
{
  gsdi_returns_T  gsdi_status = GSDI_NOT_SUPPORTED;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_get_feature_key_req_T   *msg_ptr = NULL;

  MSG_HIGH("gsdi_perso_get_key",0,0,0);

  /* support async at the moment */
  if (async_cb == NULL)
  {
    MSG_ERROR("Input parameter: async cb ptr null",0,0,0);
    return GSDI_ERROR;
  }

  /* get memory for access request type */
  msg_ptr = mmgsdi_malloc(sizeof(gsdi_perso_get_feature_key_req_T));
  if(msg_ptr == NULL)
  {
    MSG_ERROR("Could not allocate memory for msg ptr",0,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
    Fill In Message Header Contents
    ------------------------------------------------------------------------*/
  msg_ptr->message_header.message_set = MS_GSDI;
  msg_ptr->message_header.message_id  = GSDI_PERSO_GET_FEATURE_KEY_REQ;
  msg_ptr->message_header.slot        = slot;
  msg_ptr->message_header.gsdi_async_callback = async_cb;
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_get_feature_key_req_T),
                   &msg_ptr->message_header);
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg_ptr->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg_ptr->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
  msg_ptr->queue_id                = GS_QUEUE_GSDI_LIB;

  /* --------------------------------------------------------------------------
    Fill In Remainder of Message Contents
    ------------------------------------------------------------------------*/
  msg_ptr->feature        = feature_ind;
  msg_ptr->is_unblock_key = is_unblock_key;
  msg_ptr->client_ref     = client_ref;

  /* --------------------------------------------------------------------------
    Queue Command with GSDI Task.
    Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
    ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(msg_ptr);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MSG_ERROR("gsdi_perso_get_key queing failed - %x",gsdi_status,0,0);
  }
  else
  {
    MSG_HIGH("gsdi_perso_get_key queued successfully",0,0,0);
  }

  MMGSDIUTIL_TMC_MEM_FREE( msg_ptr);
#else
  (void)slot;
  (void)feature_ind;
  (void)is_unblock_key;
  (void)client_ref;
  (void)async_cb;
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_CDMA_SVC_NVRUIM_INIT

DESCRIPTION
  This function initiates the control flags in nvruim

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdi_cdma_svc_nvruim_init(
  gsdi_slot_id_type slot
)
{
#ifdef FEATURE_NV_RUIM
  gsdi_sim_read_req_T       *read_req_ptr           = NULL;
  byte                      rec_num_accessed        = 0;
  byte                      num_records             = 0;
  byte                      ret_data_len            = 0;
  mmgsdi_return_enum_type   mmgsdi_status           = MMGSDI_SUCCESS;
  gsdi_returns_T            gsdi_status             = GSDI_SUCCESS;
  uim_svc_table_return_type support;
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN  
  boolean                   support_an_hrpd         = FALSE;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

#ifdef FEATURE_UIM_SUPPORT_3GPD
  boolean                   support_mip             = FALSE;
  boolean                   support_sip             = FALSE;
  boolean                   service_3gpd_extensions = FALSE;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_LBS
  boolean                   support_lbs             = FALSE;
#endif /* FEATURE_UIM_SUPPORT_LBS */

  boolean                   support_bcms            = FALSE;
  boolean                   support_smscap          = FALSE;
  byte                      data_ptr[GSDI_CDMA_SST_LBS_OFFSET + 1];

#if defined(FEATURE_UIM_SUPPORT_3GPD_NV) && defined (FEATURE_UIM_SUPPORT_3GPD)
  boolean                   non_3gpd_cdma_card      = FALSE;
  mmgsdi_data_type          mmgsdi_data_buf;
  mmgsdi_protocol_enum_type protocol                = MMGSDI_NO_PROTOCOL;  
  uint16                    imsi_mcc                = 0;
#endif /* FEATURE_UIM_SUPPORT_3GPD_NV && FEATURE_UIM_SUPPORT_3GPD */  

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
     sizeof(gsdi_sim_read_req_T), mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could Not Allocate Memory for read_req_ptr pointer", 0,0,0);
    return;
  }

  memset(data_ptr, 0x00, (GSDI_CDMA_SST_LBS_OFFSET + 1));
  read_req_ptr->sim_filename = (word)UIM_CDMA_CDMA_SVC_TABLE;
  read_req_ptr->message_header.slot = slot;
  read_req_ptr->required_data_len = GSDI_CDMA_SST_LBS_OFFSET + 1;
  gsdi_status = gsdi_cdma_proc_ruim_read(read_req_ptr,
                                         &rec_num_accessed,
                                         &num_records,
                                         &ret_data_len,
                                         data_ptr );
  if (gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("Could Not Read UIM_CDMA_SVC_TABLE gsdi_status returned: 0x%x",
               gsdi_status, 0,0);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);
    return;
  }

  /* initiate the nvruim flags for corresponding services */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
  /* an_hrpd_prefrence */
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_HRPD,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    support_an_hrpd = TRUE;
  }
  uim_init_an_hrpd_preference(support_an_hrpd);
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

  /* 3gpd preference */
#ifdef FEATURE_UIM_SUPPORT_3GPD
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_SIP,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    support_sip = TRUE;
  }

  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MIP,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    support_mip = TRUE;
  }

  /* 3gpd extension support */
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MSG_EXT,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    service_3gpd_extensions = TRUE;
  }

  uim_init_3gpd_preference(support_sip, support_mip);
  nvruim_data_3gpd_init_extensions_support(service_3gpd_extensions);
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  /* lbs */
#ifdef FEATURE_UIM_SUPPORT_LBS
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_LBS,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    support_lbs = TRUE;
  }
  nvruim_lbs_init_support(support_lbs);
#endif /* FEATURE_UIM_SUPPORT_LBS */
  /* wms services */
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MSG_EXT,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    support_smscap = TRUE;
  }
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_BCSMS,
                                               data_ptr);
  if(support.allocated && support.activated)
  {
    support_bcms = TRUE;
  }
  nvruim_init_wms_svc_items( support_bcms, support_smscap );

  MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);

#if defined(FEATURE_UIM_SUPPORT_3GPD_NV) && defined (FEATURE_UIM_SUPPORT_3GPD)
  /* check for non 3gpd cdma card */
  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));
  (void) mmgsdi_util_determine_protocol(&protocol);
  /* UICC card always have the 3GPD service, set non 3GPD card to false*/
  if (protocol != MMGSDI_UICC)
  {  
    /* Read the MCC code for icc card */
    mmgsdi_status = mmgsdi_init_cache_binary(slot,
                                             MMGSDI_CDMA_IMSI_M,
                                             &mmgsdi_data_buf);
    if ((mmgsdi_status == MMGSDI_SUCCESS) && ( mmgsdi_data_buf.data_ptr != NULL)
       && (mmgsdi_data_buf.data_len == MMGSDI_CDMA_IMSI_LEN))
    {
      imsi_mcc = (mmgsdi_data_buf.data_ptr[MMGSDI_CDMA_IMSI_LEN - 1] << 8);  // MSB
      imsi_mcc |= mmgsdi_data_buf.data_ptr[MMGSDI_CDMA_IMSI_LEN - 2];  // LSB
    }
    /* check the MCC code for non 3GPD cdma card */
    if (NVRUIM_NON_3GPD_CDMA_CARD_MCC == imsi_mcc)
    {
      non_3gpd_cdma_card = TRUE;
      /* Read the SIPUPPEXT file */
      mmgsdi_status = mmgsdi_init_cache_binary(slot,
                                               MMGSDI_CDMA_3GPD_SIPUPPEXT,
                                               &mmgsdi_data_buf);
      if ((mmgsdi_status == MMGSDI_SUCCESS) && (mmgsdi_data_buf.data_ptr != NULL))
      {
        /* if SIPUPPEXT file is present and have valid value (any value other 
           than 0x00, 0xFF) then 3GDP NV support not required */
        if((mmgsdi_data_buf.data_len > 0) && 
           (0xFF != mmgsdi_data_buf.data_ptr[0]) &&
           (0x00 != mmgsdi_data_buf.data_ptr[0]))
        {
          /* card has the 3gpd ext credential*/
          non_3gpd_cdma_card =  FALSE;
        }
      }
    }
    /* Free the memory allocated for read request */
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
  }
  nvruim_init_non_3gpd_cdma_card(non_3gpd_cdma_card);
#endif /* FEATURE_UIM_SUPPORT_3GPD_NV && FEATURE_UIM_SUPPORT_3GPD */   
#endif /* FEATURE_NV_RUIM */
  (void) slot;
} /* gsdi_cdma_svc_nvruim_init */


