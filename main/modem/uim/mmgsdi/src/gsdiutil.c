/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G S D I   U T I L I T Y    F U N C T I O N S


GENERAL DESCRIPTION

  This source contains utility functions that support the GSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2010 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/sqa/mmgsdi/rel/07H1_2/src/gsdiutil_c/rev12/gsdiutil.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/10   nmb     DEEPSEC buffer check for copying data when building a cnf
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
05/05/09   js      Fixed warnings
04/29/09   js      Added prototype for static functions, fixed compiler warnings
04/27/09   gg      Added handling of SMS-PP case in gsdi_util_ruim_service_check()
02/19/09   ssr     Set init flag to TRUE for EF-RAT and EF-ActHPLMN file.
02/13/09   tml     Fixed perso retry count return mismatch issue
02/16/09   kk      INIT_FFCN mode sent to clients, to enable re-read all the
                   cached items that are not part of re-init
01/22/09   sun     Removal of old gsdi refresh timer
01/21/09   kk      Fixing featurization of FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/26/08   nb      Restructured featurization flags for Common Modem Interface
                   Feature
10/14/08   xz      Move FEATURE_ENHANCED_NW_SELECTION into
                   gsdi_is_ens_enabled_in_nv()
10/06/08   jk      Allow GSM SIM access in 1X only
09/29/08   xz      Only do TP DL when ENS NV is enabled or protocol is GSM
                   upon REFRESH
09/25/08   kk      Added support for extracting unblock dck retries available
08/27/08   nb      Klockwork fixes
08/08/08   js      Klockwork fixes
06/24/08   sp      Modify messages.
06/23/08   sp      Don't clean up the global that was populated as a result
                   of EF-DIR aid read during the reselection of an app.
05/26/08   kk      Fixed compilation warning
05/23/08   vs      Added support for storing PIN after a GSDI PIN operation
                   using a synchronous call is successful
05/23/08   kk      Fixed Data Assertion case during Refresh - FCN
04/24/08   sun     Close all sessions and inform all clients on a Reset
04/16/08   kk      Added support of get command on sim lock data
01/21/08   sun     Clear CPHS cache on Refresh
11/15/07   sun     Added support for NNA refresh
06/25/07   sp      Added function to delete the path lookup table for refresh
05/14/07   sp      Removed ef SPN fom taking down the call stack during refresh
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
04/26/07   sp      Pass access type instead of enum to cache
04/12/07   sun     Featurized Phonebook
04/03/07   sk      Support EF HPLMN Refresh during call
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/26/07   tml     Fixed refresh reset not prompting for pin issue
03/16/07   sk      Support Refresh-FCN related to callstack while in a call
02/27/07   sun     Removed Global PIN from GSDI
02/27/07   jk      Added featurization for Memory Reduction
02/27/07   sk      Fixed USIM service table
02/15/07   sp      Fix compilation error for 7k builds when
                   FEATURE_VIRTUAL_SIM is defined.
02/09/07   sk      Added support for 1000 phonebook entries.
02/08/07   sp      Set perso_engine_state for refresh
02/01/07   tml     Null ptr check
01/17/07   sp      Removed pin_just_verified flag.
01/08/07   sun     Send Default Client ID for All Pin Events
12/15/06   tml     Added support for GSDI_REFRESH_SUCCESS_BUT_NO_AID_ACTIVE
12/04/06   sp      Set the flag to perform perso operation on various
                   refresh methods
10/17/06   sk      Featurization/Compilation fix.
10/09/06   tml     Compilation fix
10/06/06   sk      Added support to perform gsdi perso refresh on Refresh
                   SIM INIT Req w/wo Full File Change Notification/File Change
                   Notification
09/08/06   jk      Added RUIM support in gsdi_populate_file_attr_cache
09/12/06   jk      Lint errors fix as a result of RPC Convergence
09/12/06   sun     Fixed Lint Error
09/06/06   tml     Add refresh AID notification support
09/07/06   pv      Set the security attributes to Always Allow when
                   gsdi_util_extract_EF_tagged_data fails.  Always return
                   GSDI_SUCCESS from gsdi_util_decode_umts_file_header.
                   Return error/success status from
                   gsdi_util_decode_umts_file_header
09/06/06   tml     Fixed Expand Security Parsing issue
08/30/06   tml     Lint fix
08/25/06   jar     Added Support for FEATURE_MMGSDI_CARD_DRROR_INFO
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/03/06   sun     Do not update perso state
07/26/06   sun     Added support for MultiUSIM App and MMGSDI PIN
07/07/06   sun     Removed Reference to mc.h
06/28/06   sp      Added log messages in gsdi_util_selective_card_cache_delete()
06/15/06   tml     Fixed compilation warning
06/14/06   sun     Fixed Compilation
06/12/06   pv      Add handling code for GSDI_INTERNAL_SIM_RESET and
                   MMGSDI_SESSION_CLOSE_EVT
06/12/06   sun     Clear out the USIM cache on a SIM_INIT refresh
06/04/06   jar     Changed message low to message high and fixed alignment
                   in gsdi_util_is_file_ok_in_sst and
                   gsdi_util_is_file_ok_is_ust
06/01/06   jar     Indentation fixes which resulted in Lint errors.
06/01/06   jar     Removed the the LOCAL Prefix from the function:
                   gsdi_util_need_to_take_down_call_stack() because it is
                   now called in gsdi.c
05/31/06   tml     Fixed MO SMS Control USIM UST check
05/16/06   nk      Fixed compilation warnings
05/15/06   tml     Refresh fix and cleanup
05/10/06   sun     Fixed Featurization
05/08/06   nk      Removed GSDI_UTIL_SEND_PIN_STATUS featuriziation.
04/13/06   sp      Updating new mmgsdi pin status
04/13/06   sun     Fixed Response for Perm_Feat_Ind
04/06/06   sun     Added support for RUIM Lock
04/05/06   tml     Lint and add Card Removed support
03/29/06   sun     Feature Indication should be on if features are in autolock
                   state as well, add null pointer check
03/21/06   tml     added cfis and missing service table support and lint
03/10/06   jar     Lint fixes
03/08/06   jar     Merged SIM Lock support from Branch.
                   Lint Fixes
03/01/06   tml     Fixed compilation warning
02/21/06   sp      Updating gsdi_ust_services
02/09/06   tml     Do not wipe out total len from cache because the write
                   to cache requires this info and compilation warning fix
                   and lint fixes
2/14/06   nk      Merge for: Bring file attribute setting to avoid synchronous
                   file attribute access in the library function that could have
                   caused potential deadlock situation and ARR Caching merge and
                   get fle attribute caching additional support
02/01/06   jar     Fix for improper Read/Write while invalidated
                   flag for GSM SIM
01/26/06   tml     Fixed get EF cache length not returning more than 256 byte
                   issue
01/25/06   tml     fixed file path comparison issue with USIM
12/04/05   sun     Fixed Lint Errors
11/22/05   jk      Changes to support multiprocessor RPC
11/14/05    pv     change signature for functions that needs lookup
                   to return gsdi_returns_T and accept an output parameter
11/14/05   tml     Fixed compilation warning and lint fix
11/09/05   sun     Fixed Lint Errors
11/03/05   sun     Fixed Compiler Warning
10/25/05   tml     Fixed compilation issue for 1X targets
10/14/05   tml     Added utility function for checking if data to be
                   written same as cached data
10/13/05   tml     Refresh FCN cache clearing fix
10/12/05   jar     Acting HPLMN Refresh FCN Fix.
10/07/05   tml     Refresh FCN cache clearing fix
09/26/05   jar     Changed default for RW While Deactivated
                   to TRUE.
09/13/05   sun     Initialized the mask to the right value
08/26/05   sun     Added support for BT SAP
08/22/05   sun     Fixed the order of MSISDN in gsdi_usim_service_table
08/17/05   sk      Fixed problem with gsdi_util_decode_exp_sec_attr()
                   with increase and update sec sttributes.
08/09/05   tml     Fixed expanded security decode issue
07/26/05   sst     Fixed lint errors
07/11/05   sun     Fixed Memory Leaks
06/30/05   sun     Added missing UST service
06/16/05   sk      Fixed potential problems with GSDI_SEND_CHANNEL_DATA_RSP
06/10/05   tml     Added 256 MMGSDI support
05/26/05   sun     Fixed path conversion for multiple DFs.
05/11/05   tml     Fixed offset mismatch
05/11/05   sst     Lint fixes
04/30/05   jar     Fixed bad message length issue
04/28/05   jar     Initial release of the GSDI SIM Lock Architecutre to support
                   Plug In Proprietary Security Libraries driven by a Perso Engine.
03/16/05   jar     Phonebook Fix in the conf
03/11/05   jar     SST Fix and Variable name changes and variable type changes
03/07/05   tml     Fixed mask for SST
03/05/05   jar     Fixed invalid memheap pointer deallocation.
                   Added a special case for EXT1 for USIM
03/04/05   tml     ensure ATR are not being written outside boundary
03/01/05   sun     Fixed mask for SST
02/25/05 jar/ tml  Fixed compilation error
02/25/05   tml     Fixed compilation error
02/25/05   tml     Merged mapping functions for uim and mmgsdi file back in
02/23/05   jar     Merged Cingular Feature Enhancements back in.
02/22/05   jar     Reverted Cingular Fixes...and merged back in UST Shift
02/08/05   sun     Added support for Cingular
02/15/05   jar     Removed Service Table Check for PS Service Domain EFs
02/06/05   jar     Mute MMGSDI when a TASK_STOP_SIG is received.
02/04/05   jar     Set task_stopped to TRUE when the TASK_STOP_SIG is
                   received.
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
01/27/05   jk      Fixed new compiler warning.
01/21/05   sun     Added support for all USIM and GSM services
01/12/05   tml     Fixed compilation error for 1X only target
12/28/04   jar     Added support for GSDI_GET_PBR_TABLE_REQ
12/27/04   jk      Fixed Compiler warnings.
11/15/04   jar     Dual Slot Merges
10/18/04   sk      Added definitions for functions gsdi_util_map_to_ecc
                   and gsdi_proc_get_ecc.
                   Added case for GSDI_GET_ECC_RSP under
                   gsdi_util_send_gsdi_cnf.
09/30/04   sk      Merged in Mainline fix for sending out multiple events
                   Added EF-ELP Caching Support
09/29/04   sk      Added a function gsdi_util_get_pin to get application specific
                   pin information from the application pin information table.
09/24/04   jar     Added generation of GET_CPHSI_INFO CNF RSP
09/09/04   sk      Corrected fields sw1 and sw2 in case GSDI_SEND_CHANNEL_DATA_RSP
                   in function gsdi_util_send_gsdi_cnf
08/31/04   jar     Added FEATURE_MMGSDI_ATCSIM Support for Send Channel Data
07/12/04   jar     Added CPHS ALS and file enum mapping Support
09/01/04   tml     Ensure event is being sent out to client once only
08/20/04   tml     Fixed FCN when the file is not registered by any client issue
08/10/04   jar     Changed fixed array SST and UST in the util functions that
                   check to see if it is ok to access to dynamically allocated
                   arrays of GSDI_MAX_DATA_BLOCK_LEN.  Changed UST to use
                   UIM_USIM_MSISDN instead of UIM_TELECOM_MSISDN.
08/09/04   jar     Changed UIM_TELECOM_SDN to use SDN Mask instead of SPN Mask.
08/02/04   jar     Fixed Issue with mem overwrite in OTA PERSO CNF. Fixed
                   compiler warning.
07/27/04   tml     initialize message length in sanity timer handling
07/22/04   jar     Change feature define from RUIM to CDMA
06/28/04   jar     Reset Perso State on a REFRESH RESET Request.
06/24/04 jar/tml   Added GSDI - UIM Sanity Timer
                   Added additional handling of Perso Confirmations
06/08/04   jar     Added FEATURE_MMGSDI_PERSONALIZATION Support
06/02/04   tml     Added unit test trigger for GSTK
04/28/04   jar     Added Channel Management Support
04/14/04   jar     Added Fix which allows for the Record Length to be reset
                   during the REFRESHING of Dual Slots.
04/13/04   jar     Lint Fix required for gsdi.c linting.
04/12/04   tml     Added Unit test for Increase, Get SIM Capability, Search
                   and Select
04/07/04   jar     Fixed Compile Warning
03/31/04   tml     Added Read/Write/Get File Attribute and Select Unit API Test
                   Change to #define for indexing in get file attributes
03/27/04   tml     Merged from branch
03/27/04   tml     Added cache_ready_2 support
03/18/04   jar     Added additional utility functions.
03/12/04   jar     Added utility functions to support Single Slot Initialization
                   for Dual Slot Targets.
03/11/04   tml     Added Event Tester Support
02/23/04   jar     Compile Fix for New Target Bringup (MSM6500)
02/04/04 jar/tml   Added selectively reset just_invalidated/rehabilitated and
                   just_invalidated_2/rehabilited_2 for REFRESH SIM RESET.
01/29/04   tml     Added event notification to diag interface
01/26/04   jar     Modified gsdi_util_extract_file_attr to check if file is
                   readable and updateable when file is invalidated.  Modified
                   gsdi_util_decode_umts_file_header to always indicate false
                   for readianble updateable when file is deactivated.
01/24/04   tml     Dual slot refresh support
01/15/04   tml     Allowed GSDI to sent the last SIM event to client if
                   client event registration is too slow
12/12/03   jar     Force a context switch in gsdi_wait regardless of number of
                   items pending in gsdi_task_cmd_q.
12/04/03   tml     Linted
11/10/03   jar     Modified data to data_p in gsdi_util_send_gsdi_cnf()
                   for GSDI_SEND_APDU_RSP
11/05/03   jar     Added support for Slot Change RSP/CNF
10/22/03   jar     Added support for SEND APDU Command processing under
                   FEATURE_MMGSDI_TEST_AUTOMATION
10/16/03   jar     Fixed Compile Warning for non dual slot builds.
09/09/03   jar     Removed additional ERR_FATALS
09/07/03   jar     Removed Dual Slot ERR_FATALS and Dual Slot Code Cleanup
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT
06/26/03   jar     Modified gsdi_util_ref_exp_sec_attr() to select correct ARR
                   when a Get File Attributes is performed on an EF_BY_PATH
06/10/03   jar     Added OFFLINE SIG Handling when FEATURE_GSTK is defined.
06/09/03   jar     Provided Recovery Method for Invalid Sigs in GSDI TCB
06/03/03   jar     Fixed REFRESH Request problem when comparing FCN Lists
05/20/03 jar/tml   linted.  Removed EST Check for FDN in USIM Service Table.
05/09/03   jar     Fixed compile error
05/01/03   jar     Added utility functions to support new FCN Modes.
04/22/03   tml     Image confirmation supports
04/15/03   tml     Evaluated ERR usage
04/13/03   tml     removed redundant return gsdi_status statement
03/21/03   tml     Added boolean parameter status in gsdi_util_gstk_tp_dl_
                   complete()
03/19/03   jar     Added Function gsdi_ruim_get_ecc
03/18/03   jar     Added CHV1 Disable check for USIM.  Workaround to allow
                   calling modules to use same gsdi_lib_is_service_available
                   API.
02/28/03  tml/jar  Added Featur GSTK around Toolkit specific functions.
02/04/03   jar     Added GSDI_SIM_REFRESH_RSP to gsdi_util_send_gsdi_cnf()
                   Modified gsdi_util_delete_[SIM|USIM|RUIM]_cache to also
                   memset body to 0x00.  Added function gsdi_util_selective_
                   sim_cache_delete, gsdi_util_preinit_gsdi, gsdi_util_convert_
                   file_paths, gsdi_util_notify_modules.  Added case handling
                   GSDI_MOSMS_CONTROL in GSM services table check
01/02/03   jar     LTK RUIM Support Merge
11/26/02   jar     Removed all RTRE Code.
11/18/02   jar     Removed phone book status_unknown booleans.
11/08/02   jar     Added function gsdi_util_is_gsdi_cache_ready
10/25/02   jar     Backed out file_invalidate boolean fix.
10/23/02  tml/jar  Fix get file attribute's file_invalidate boolean so that
                   it reflects the correct file invalidate status
                   Modified handling of TASK_OFFLINE_SIG and TASK_STOP_SIG
                   to be processed before the GSDI_TASK_CMD_Q_SIG is handled.
10/17/02   jar     Vote UIM off when Offline and Stop Signals are received.
10/15/02   tml     Correctly match the file type and file structure from the
                   standard values to the GSDI enum values in
                   gsdi_util_decode_umts_file_header for umts
                   Changed gsdi_util_send_gsdi_cnf's GET_FILE_ATTRIBUTES_RSP
                   check for transparent/linear/cyclic file structure.
                   Used the GSDI_TRANSPARENT_FILE, GSDI_LINEAR_FILE and
                   GSDI_CYCLIC_FILE as the check since the file_structure
                   is being populated correctly in the
                   gsdi_util_decode_umts_file_header
10/15/02   jar     Cleaned up TASK_STOP_SIG and TASK_OFFLINE_SIG handlers due
                   to task_stop() function.
                   Disable the GSDI from processing requests when the
                   TASK_STOP_SIG or TASK_OFFLINE_SIG signals are received.
10/15/02  jq/jar   Wait for TASK_STOP_SIG and TASK_OFFLINE_SIG.
                   Added task_stop() call when either of these signals is
                   is recieved.  Removed older implementation.
10/14/02   tml     1x, gsm merge
10/07/02   tml     Update pin1_status and pin2_status depending on various pin
                   actions, e.g., Verify pin, enable pin, disable pin etc
09/17/02   tml     Moved gsdi_util_get_sim_ef_cache_length to under FEATURE_MMGSDI_GSM
09/13/02   tml     Fixed Security attr parsing (required future enhancement)
09/03/02   jar     Removed ERR msgs from gsdi_util_get_usim_ef_cache_length() and
                   gsdi_util_get_sim_ef_cache_length().
09/14/02   jar     Made changes per Run Time Enable RUIM Integration in function
                   gsdi_util_send_mmi_sim_ind.
08/30/02 tml/jar   Replace gs_status_T with gsdi_returns_T.  Fixed
                   gsdi_util_send_gsdi_cnf() problem with overwriting union
                   members.  Modified gsdi_util_send_mmi_sim_ind() to remap
                   GSDI_CARD_ERROR base on RTRE config.
08/28/02   jar     Added NVRUIM RTRE support for GSDI Events in gsdi_util_send_
                   mmi_sim_ind.
08/25/02   jar     Added switch in gsdi_util_send_mmi_ind to check the event.
                   For GSDI_CARD_ERROR, disable the GSDILibraries to avoid
                   having a Request sit on the Command Queue longer than
                   25 seconds.
08/21/02   jar     Modified gsdi_util_send_gsdi_cnf() to support gsdi_cnf_T
                   change from struct to union.
08/12/02   tml     Added resp type enum handling for pin related api in order to
08/11/02   jar     Added functions gsdi_util_get_usim_ef_cache_length and
                   gsdi_util_get_sim_ef_cache_length to be called
                   by gsdi_get_ef_length library function to return cached EF
                   length. (Optimization)
08/08/02   jar     Modified gsdi_util_write_sim_cache and gsdi_util_write_usim_cache
                   to adjust the data length of the item in the Queue during gsdi
                   init.  This will remove the dependency of having to know the
                   length of the item in the SIM/USIM before it is cached.
07/16/02   jar     Added SST checks for Service 43 (PLMNWACT) Service 44
                   (OPLMNWACT) Service 45 (HPLMNWACT) and Service 50
                   (RPLMNWACT)
07/13/02   jar     Modified functions gsdi_util_is_file_ok_in_sst()  and
                   gsdi_util_is_file_ok_in_ust() to fix an error when
                   retrieving ust and sst info from cache.  Modified functions to
                   avoid code optimization issues.  Added MSG_LOWs to assist
                   with debugging. Added additional Service Options for ust.
05/27/02   jar     Removed setting gsdi_status to GSDI_ERROR in
                   else in function gsdi_util_decode_exp_sec_attr.
05/19/02   jar     Modified gsdi_util_send_mmi_sim_ind to call
                   gsdi_util_send_sim_events_to_reg_tasks to send events if
                   FEATURE_GSDI_MULTICLIENT_SUPPORT is defined.
05/16/02   jar     Merged in code changes to change FEATURE_MM to FEATURE_NAS.
05/10/02   jar     Modified gsdi_util_decode_exp_sec_attr while loop control
                   to not attempt to Decode TLV Tags of 0x00.
04/02/02   jar     Wrapped 01/23/02 changes around FEATURE_MM
03/23/02   jar     Added the function gsdi_util_parse_atr.
03/11/02   jar     Modified gsdi_util_send_mmi_sim_ind for multiclient support.
                   Added Function gsdi_util_send_sim_events_to_reg_tasks.
02/13/02   jar     Modified gsdi_sim_get_ecc and gsdi_usim_get_ecc to
                   better handle a LINEAR FIXED USIM ECC EF.
02/08/02   jar     Modified gsdi_util_send_gsdi_cnf to not memcpy data on a
                   failed request.  It will also set returned_data_len to 1
                   and the set first byte of data to 0 to represent an error.
02/03/02   jar     Add functionality to allow requests for ECC data retrieved
                   during GSDI Initialization.  Added Phonebook Support
                   Functionality which is featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.
1/23/02    jar     Added a mechanism to bypass gs_send_message for messages
                   directed to the MM.  It calculates  message_length using
                   gsdi_rsphdr_T to avoid losing data.
10/19/01   jc      Add mechanism to use callback function for responses to
                   clients.
10/11/01   jc      Update gsdi_cnf and send_message functions.
09/04/01   jc      Update FEATURE define for watch dog reporting.
08/29/01   jc      Update UST checking utility for USIM EFs.


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "task.h"

#include "msg.h"
#include "memheap.h"
#include "err.h"

#include "gsdi_exp.h"
#include "gsdilib.h"
#include "gsdi.h"
#include "gsdi_nv.h"
#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#include "gsdi_perso.h"
#endif /* !FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
#include "gsdidiag.h"
#endif

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_DOG
#include "dog.h"
#endif /* FEATURE_DOG */

#ifdef FEATURE_NAS
#error code not present
#endif /* FEATURE_NAS */

#ifdef FEATURE_UIM
#include "uim.h"
#endif /* FEATURE_UIM */
#include "mmgsdiutil.h"
#include "mmgsdilib.h"

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
#include "gsdi_event_tester.h"
#endif /*#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#include "gsdi_perso_sec.h"
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define GSDI_SHIFT_TO_BYTE1_OF_WORD 8
#define GSDI_MF                     0
#define DF_EF_1                     1
#define DF_EF_2                     2
#define DF_EF_3                     3
#define DF_EF_4                     4
#define MF_HIGH_BYTE                0x3F

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif

/*-------------------------------------------------------------------
** GSDI_UST_SERVICES
** Maps Directly with USIM Service Table EFUST in 3GPP TS 31.102
---------------------------------------------------------------------*/
typedef enum
{
  GSDI_UST_NONE = 0x00,

  GSDI_UST_LOCAL_PHONE_BOOK,
  GSDI_UST_FDN,
  GSDI_UST_EXT2,
  GSDI_UST_SDN,
  GSDI_UST_EXT3,
  GSDI_UST_BDN,
  GSDI_UST_EXT4,
  GSDI_UST_OCI_OCT,
  GSDI_UST_ICI_ICT,
  GSDI_UST_SMS,

  GSDI_UST_SMSR,
  GSDI_UST_SMSP,
  GSDI_UST_AOC,
  GSDI_UST_CCP,
  GSDI_UST_CBMI,
  GSDI_UST_CBMID_RANGES,
  GSDI_UST_GID1,
  GSDI_UST_GID2,
  GSDI_UST_SPN,
  GSDI_UST_UPLMN_SEL_WACT,

  GSDI_UST_MSISDN,
  GSDI_UST_IMAGE,
  GSDI_UST_SOLSA,
  GSDI_UST_ENH_ML_SVC,
  GSDI_UST_AA_EMLPP,
  GSDI_UST_RFU1, /*lint -esym(749,GSDI_UST_RFU1) */
  GSDI_UST_GSM_ACCESS,
  GSDI_UST_DATA_DL_SMSPP,
  GSDI_UST_DATA_DL_SMSCB,
  GSDI_UST_CALL_CONTROL,

  GSDI_UST_MO_SMS,
  GSDI_UST_RUN_AT_CMD,
  GSDI_UST_SET_TO_ONE, /*lint -esym(749,GSDI_UST_SET_TO_ONE) */
  GSDI_UST_EST,
  GSDI_UST_ACL,
  GSDI_UST_DEPERSON_KEYS,
  GSDI_UST_COOP_NETWK_LIST,
  GSDI_UST_GSM_SECURITY_CONTEXT,
  GSDI_UST_CPBCCH,
  GSDI_UST_INV_SCAN,

  GSDI_UST_MEXE,
  GSDI_UST_OPLMN_SEL_WACT,
  GSDI_UST_HPLMN_WACT,
  GSDI_UST_EXT5,
  GSDI_UST_PLMN,         /*PNN*/
  GSDI_UST_OPLMN_LIST,/*OPL*/
  GSDI_UST_MAILBOX_DIAL,
  GSDI_UST_MSG_WAIT,
  GSDI_UST_CALL_FWD_IND,
  GSDI_UST_RPLMN_LACT,

  GSDI_UST_SP_DISP_INFO,
  GSDI_UST_MMS,
  GSDI_UST_EXT8,
  GSDI_UST_RFU2,
  GSDI_UST_MMS_USR_P,
  GSDI_UST_NETWK_ALTERING_MS,
  GSDI_UST_VGCS_GID_LIST,
  GSDI_UST_VBS_GID_LIST,
  GSDI_UST_PSEUDO,
  GSDI_UST_USR_PLMN_WLAN,

  GSDI_UST_OPT_PLMN_WLAN,
  GSDI_UST_USR_WSID,
  GSDI_UST_OPT_WSID,
  GSDI_UST_VGCS_SEC,
  GSDI_UST_VBS_SEC,
  GSDI_UST_WLAN_REAUTH_ID,
  GSDI_UST_MULTIMEDIA_MSG_STRG,
  GSDI_UST_GBA,
  GSDI_UST_MBMS_SEC,
  GSDI_UST_DATA_DWLD_USSD,

  GSDI_UST_EHPLMN,
  GSDI_UST_TERMINAL_PROFILE_AFTER_UICC_ACT

}gsdi_ust_services;


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEAUTRE_MMGSDI_UMTS*/

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/
#if defined(FEATURE_GSDI_PHONEBOOK_SUPPORT) && defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* (FEATURE_GSDI_PHONEBOOK_SUPPORT) && (FEATURE_MMGSDI_UMTS) */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

static void gsdi_util_selective_card_cache_delete(
  gsdi_app_enum_type    app_type,
  uim_items_enum_type * efs_not_to_clear,
  uint8 num_items,
  gsdi_slot_id_type slot
);

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

static void gsdi_util_send_vote_notification(
  gsdi_refresh_vote_node_type * current_node,
  uim_items_enum_type         * sim_filenames_p,
  uint8                         num_files,
  boolean                       request_to_take_down_callstack
);

static gsdi_returns_T gsdi_util_scan_fcn_node(
    uim_items_enum_type * sim_filenames_p,
    uint8       * num_files_p,
    uint8       * mod_num_files_p,
    uim_items_enum_type * array_of_filenames_p,
    gsdi_refresh_fcn_notify_node_type * current_node_p
);

static void gsdi_set_pin_status (
  int32 *pin_status_ptr,
  int32 pin_set_to
);

static gsdi_returns_T gsdi_check_pin_status(
  int32 pin_status,
  boolean and_operation
);

static gsdi_returns_T gsdi_check_access_condition(
  gsdi_pin_status_info_type *  key_ref_value_p
);

static boolean gsdi_util_send_fcn_notification(
  gsdi_refresh_fcn_notify_node_type * current_node_p
);


/*===========================================================================
FUNCTION GSDI_UTIL_FREE_DATA_PTR_MEMBER

DESCRIPTION
  This will free the data pointer member in the gsdi_task_cmd_data_type

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_util_free_data_ptr_member(gsdi_task_cmd_data_type* cmd_ptr)
{
  if( NULL == cmd_ptr)
  {
    MSG_ERROR("gsdi_util_free_data_ptr_member cmd_ptr is NULL",0,0,0);
    return;
  }

  switch (cmd_ptr->gsdi_image_req_cmd.message_header.message_id )
  {
    case GSDI_GET_IMAGE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_image_req_cmd.path);
      break;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    case GSDI_SIM_SECURE_STORE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_sim_secure_store_req_cmd.data_buf_ptr);
      break;

    case GSDI_PERSO_ROOT_KEY_WRITE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_perso_root_key_write_cmd.data_buf_ptr);
      break;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
    case GSDI_PERSO_VERIFY_PHONE_CODE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_ptr);
      break;

    case GSDI_PERSO_CHANGE_PHONE_CODE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_ptr);
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_ptr);
      break;

    case GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_ptr);
      break;

    case GSDI_PERSO_SET_PHONE_CODE_REQ:
      /* free the path from the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cmd_ptr->gsdi_set_phone_code_cmd.phone_code_ptr);
      break;
#endif /*#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/

    default:
      break;
  }
}

/*===========================================================================
FUNCTION GSDI_UTIL_MALLOC_AND_COPY_DATA_PTR_MEMBER

DESCRIPTION
  This will malloc data pointer member in the gsdi_task_cmd_data_type and copy
  the pointer data into the newly allocated buffer

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_util_malloc_and_copy_data_ptr_member(
  gsdi_task_cmd_data_type * cmd_ptr,
  gsdi_task_cmd_data_type * new_cmd_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status;
  if ((cmd_ptr == NULL) || (new_cmd_ptr == NULL))
    return GSDI_ERROR;

  switch (cmd_ptr->gsdi_image_req_cmd.message_header.message_id )
  {
    case GSDI_GET_IMAGE_REQ:
      if (cmd_ptr->gsdi_image_req_cmd.path != NULL)
      {
          MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_image_req_cmd.path,
            sizeof(word)*cmd_ptr->gsdi_image_req_cmd.path_len, mmgsdi_status);
          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
              memcpy(new_cmd_ptr->gsdi_image_req_cmd.path,
                      cmd_ptr->gsdi_image_req_cmd.path,
                      sizeof(word)*cmd_ptr->gsdi_image_req_cmd.path_len);
          }
          else
          {
            return GSDI_ERROR;
          }
      }
      break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    case GSDI_SIM_SECURE_STORE_REQ:
      if (cmd_ptr->gsdi_sim_secure_store_req_cmd.data_buf_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_sim_secure_store_req_cmd.data_buf_ptr,
            cmd_ptr->gsdi_sim_secure_store_req_cmd.data_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_sim_secure_store_req_cmd.data_buf_ptr,
                  cmd_ptr->gsdi_sim_secure_store_req_cmd.data_buf_ptr,
                  (uint32)cmd_ptr->gsdi_sim_secure_store_req_cmd.data_len);
        }
        else
        {
          return GSDI_ERROR;
        }
      }
      break;

    case GSDI_PERSO_ROOT_KEY_WRITE_REQ:
      if (cmd_ptr->gsdi_perso_root_key_write_cmd.data_buf_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_perso_root_key_write_cmd.data_buf_ptr,
            cmd_ptr->gsdi_perso_root_key_write_cmd.data_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_perso_root_key_write_cmd.data_buf_ptr,
                  cmd_ptr->gsdi_perso_root_key_write_cmd.data_buf_ptr,
                  cmd_ptr->gsdi_perso_root_key_write_cmd.data_len);
        }
        else
        {
          return GSDI_ERROR;
        }
      }
      break;

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
    case GSDI_PERSO_VERIFY_PHONE_CODE_REQ:
      if (cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_ptr,
            cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_ptr,
                  cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_ptr,
                  int32touint32(cmd_ptr->gsdi_verify_phone_code_cmd.phone_code_len));
        }
        else
        {
          return GSDI_ERROR;
        }
      }
      break;

    case GSDI_PERSO_CHANGE_PHONE_CODE_REQ:
      if (cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_ptr,
            cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_ptr,
                  cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_ptr,
                  int32touint32(cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_len));
        }
        else
        {
          return GSDI_ERROR;
        }
      }
      if (cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_ptr,
            cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_ptr,
                  cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_ptr,
                  int32touint32(cmd_ptr->gsdi_change_phone_code_cmd.new_phone_code_len));
        }
        else
        {
          MMGSDIUTIL_TMC_MEM_FREE(new_cmd_ptr->gsdi_change_phone_code_cmd.old_phone_code_ptr);
          return GSDI_ERROR;
        }
      }
      break;

    case GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ:
      if (cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_ptr,
            cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_ptr,
                  cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_ptr,
                  int32touint32(cmd_ptr->gsdi_unblock_phone_code_cmd.master_key_len));
        }
        else
        {
          return GSDI_ERROR;
        }
      }
      break;

    case GSDI_PERSO_SET_PHONE_CODE_REQ:
      if (cmd_ptr->gsdi_set_phone_code_cmd.phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            new_cmd_ptr->gsdi_set_phone_code_cmd.phone_code_ptr,
            cmd_ptr->gsdi_set_phone_code_cmd.phone_code_len, mmgsdi_status);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          memcpy(new_cmd_ptr->gsdi_set_phone_code_cmd.phone_code_ptr,
                  cmd_ptr->gsdi_set_phone_code_cmd.phone_code_ptr,
                  int32touint32(cmd_ptr->gsdi_set_phone_code_cmd.phone_code_len));
        }
        else
        {
          return GSDI_ERROR;
        }
      }
      break;
#endif /*#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/

    default:
      break;
  }
  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_UTIL_SET_CACHE_READY

DESCRIPTION
  This will set the gsdi_data Globals for cache_ready or cache_ready2 for
  a dual slot target.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_util_set_cache_ready(
   gsdi_slot_id_type slot,
   boolean           ready
)
{
  switch ( slot )
  {
  case GSDI_SLOT_1:
    gsdi_data.cache_ready = ready;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    gsdi_data.cache_ready_2 = ready;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    break;
  }
}
/*===========================================================================
FUNCTION GSDI_UTIL_GET_SIM_INSERTED_EVENT

DESCRIPTION
  This function will return the appropriate SIM INSERTED Event based on the
  slot provided.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_sim_events_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_sim_events_T gsdi_util_get_sim_inserted_event(
   gsdi_slot_id_type slot
 )
{
  gsdi_sim_events_T sim_event = GSDI_NO_EVENT;

  switch ( slot )
  {
  case GSDI_SLOT_1:
    /* Check Slot 1...is it present */
    if ( gsdi_data.card_applications.slot1_present )
    {
      /* Does it have an App */
      if ( gsdi_data.card_applications.apps_enabled != GSDI_NO_APP_MASK )
        sim_event = GSDI_SIM_INSERTED;
    }
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    if ( gsdi_data.card_applications.slot2_present )
    {
      /* Does it have an App */
      if ( gsdi_data.card_applications.apps2_enabled != GSDI_NO_APP_MASK )
        sim_event = GSDI_SIM_INSERTED_2;
    }
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("BAD SLOT ID: %x",0,0,0);
    break;
  }
  return sim_event;
}

/*===========================================================================
FUNCTION GSDI_UTIL_GET_REFRESH_EVENT

DESCRIPTION
  This function will return the appropriate REFRESH Event based on the
  REFRESH Type and the slot requested.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_sim_events_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_sim_events_T gsdi_util_get_refresh_event(
   gsdi_slot_id_type slot,
   gsdi_refresh_modes_enum_type refresh_mode
)
{
  gsdi_sim_events_T sim_event = GSDI_NO_EVENT;

  switch ( slot )
  {
  case GSDI_SLOT_1:
    switch ( refresh_mode)
    {
    case GSDI_SIM_INIT_AND_FULL_FILE_CHANGE:
    case GSDI_SIM_INIT_AND_FILE_CHANGE:
      sim_event = GSDI_REFRESH_SIM_INIT_FCN;
      break;

    case GSDI_SIM_INIT:
      sim_event = GSDI_REFRESH_SIM_INIT;
      break;

    case GSDI_SIM_RESET:
      sim_event = GSDI_REFRESH_SIM_RESET;
      break;

    case GSDI_USIM_APP_RESET:
      sim_event = GSDI_REFRESH_APP_RESET;
      break;

    case GSDI_3G_SESSION_RESET:
      sim_event = GSDI_REFRESH_3G_SESSION_RESET;
      break;

    case GSDI_FILE_CHANGE_NOTIFICATION:
      sim_event = GSDI_REFRESH_FCN;
      break;

    default:
      MSG_ERROR("BAD REFRESH MODE: %x",refresh_mode,0,0);
      break;
    }
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    switch (refresh_mode)
    {
    case GSDI_SIM_INIT_AND_FULL_FILE_CHANGE:
    case GSDI_SIM_INIT_AND_FILE_CHANGE:
      sim_event = GSDI_REFRESH_SIM_INIT_FCN_2;
      break;

    case GSDI_SIM_INIT:
      sim_event = GSDI_REFRESH_SIM_INIT_2;
      break;

    case GSDI_SIM_RESET:
      sim_event = GSDI_REFRESH_SIM_RESET_2;
      break;

    case GSDI_USIM_APP_RESET:
      sim_event = GSDI_REFRESH_APP_RESET_2;
      break;

    case GSDI_3G_SESSION_RESET:
      sim_event = GSDI_REFRESH_3G_SESSION_RESET_2;
      break;

    case GSDI_FILE_CHANGE_NOTIFICATION:
      sim_event = GSDI_REFRESH_FCN_2;
      break;

    default:
      MSG_ERROR("BAD REFRESH MODE: %x",refresh_mode,0,0);
      break;
    }
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    break;
  }
  return sim_event;
}
/*===========================================================================
FUNCTION GSDI_UTIL_SET_SIM_PRESENT_AND_READY

DESCRIPTION
  This function will set the SIM boolean, sim_present_and_ready, to either
  true or false as indicated by the boolean provided and the slot requested.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_util_set_sim_present_and_ready(
   gsdi_slot_id_type slot,
   boolean           sim_ready
)
{
  switch ( slot )
  {
  case GSDI_SLOT_1:
    gsdi_data.sim_present_and_ready = sim_ready;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    gsdi_data.sim_present_and_ready_2 = sim_ready;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    break;
  }
}


/*===========================================================================
FUNCTION GSDI_UTIL_GET_APPS_ENABLED

DESCRIPTION
  This function will return the pointer to the gsdi_data global data for
  the apps_enabled or apps2_enabled member of the gsdi_card_apps_T
  structure.

DEPENDENCIES
  gsdi_data and whether this is a Single Slot Solution or a Dual Slot Solution.

RETURN VALUE
  gsdi_card_apps_T  * :  A pointer to either:
                         1)  &gsdi_data.card_applications.apps_enabled
                             - OR -
                         2)  &gsdi_data.card_applications.apps2_enabled

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
uint8 * gsdi_util_get_apps_enabled(
   gsdi_slot_id_type slot
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
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("BAD SLOT ID: %x", slot,0,0);
    break;
  }

  return card_apps_p;
} /* gsdi_util_get_apps_enabled */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION GSDI_DETERMINE_CARD_MODE

DESCRIPTION
  Function called by GSDI to use the slot present information, apps_enabled
  to determine the card mode to service the request with.

  INPUT:
  1.  void * req_p [MANDATORY]  -  Pointer to request

DEPENDENCIES
  Slot Information Requested.

RETURN VALUE
  gsdi_card_mode_capability_T:
    1.  GSDI_CARD_GSM:
    2.  GSDI_CARD_RUIM:
    3.  GSDI_CARD_USIM:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_card_mode_capability_T gsdi_util_determine_card_mode(
  gsdi_slot_id_type  slot
)
{

    switch ( slot )
    {
    case GSDI_SLOT_1:
        if ( gsdi_data.card_applications.slot1_present )
            return gsdi_data.current_card_mode_capability;
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
        if ( gsdi_data.card_applications.slot2_present )
            return gsdi_data.current_card_mode_capability_2;
        break;

#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    default:
        MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
        break;
    }
    return GSDI_CARD_MODE_UNKNOWN;
}

#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_WAIT

DESCRIPTION
  Main wait routine for GSDI task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals).  The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
rex_sigs_type gsdi_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type rex_signals_mask = 0x00;  /* Mask of signals returned by REX     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* --------------------------------------------------------------
   ** Check High Priority Message queue:  If the request mask contains
   ** a check for GSDI_HIGH_PRIORITY_CMD_Q_SIG then we should
   ** check the queue, and set in the queue_mask an indication
   ** that something is waiting to be serviced on that queue.
   ** The rex signal is also cleared regardless of the queue count.
   ** This is to cover the case where there is a signal set but
   ** nothing on the queue.
   ** ------------------------------------------------------------- */

   if (requested_mask & GSDI_HIGH_PRI_CMD_Q_SIG)
   {
      (void)rex_clr_sigs( &gsdi_task_tcb, GSDI_HIGH_PRI_CMD_Q_SIG );

      if (q_cnt( &gsdi_high_pri_cmd_q ))
      {
        (void)rex_set_sigs(&gsdi_task_tcb,GSDI_HIGH_PRI_CMD_Q_SIG);
      }
   }


   /* --------------------------------------------------------------
   ** Check message queue:  If the request mask contains a check for
   ** GSDI_TASK_CMD_Q_SIG then we should check the queue, and set in
   ** the queue_mask an indication that something is waiting to be
   ** serviced on that queue.  The rex signal is also cleared
   ** regardless of the queue count.  This is to cover the case
   ** where there is a signal set but nothing on the queue.
   ** ------------------------------------------------------------- */

   if (requested_mask & GSDI_TASK_CMD_Q_SIG)
   {
      (void)rex_clr_sigs( &gsdi_task_tcb, GSDI_TASK_CMD_Q_SIG );

      if (q_cnt( &gsdi_task_cmd_q ))
      {
         (void)rex_set_sigs(&gsdi_task_tcb,GSDI_TASK_CMD_Q_SIG);;
      }
   }

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {
#ifdef FEATURE_DOG
      /* Kick the watchdog */
      dog_report( DOG_GSDI_RPT );
      (void) rex_set_timer( &gsdi_rpt_timer, DOG_GSDI_RPT_TIME );
#endif

      /* ---------------------------------------------------------------
      ** perform the real wait to wait for a command...or to force a
      ** context switch to allow higher priority tasks to run.
      ** ---------------------------------------------------------------- */
      rex_signals_mask = rex_wait( requested_mask | GSDI_RPT_TIMER_SIG );


      /* ---------------------------------------------------------------
      ** Make sure that if we got a queue signal, that there is actually
      ** an item on the queue indicated
      ** --------------------------------------------------------------- */
      if (rex_signals_mask & GSDI_HIGH_PRI_CMD_Q_SIG )
      {
          if (q_cnt(&gsdi_high_pri_cmd_q) == 0)   /* No elements in queue ? */
          {
              (void) rex_clr_sigs( &gsdi_task_tcb, GSDI_HIGH_PRI_CMD_Q_SIG );
              rex_signals_mask &= ~GSDI_HIGH_PRI_CMD_Q_SIG;  /* Turn off bit */
          }

      }

      if (rex_signals_mask & GSDI_TASK_CMD_Q_SIG)
      {
          if (q_cnt(&gsdi_task_cmd_q) == 0)   /* No elements in queue ? */
          {
              (void) rex_clr_sigs( &gsdi_task_tcb, GSDI_TASK_CMD_Q_SIG );
              rex_signals_mask &= ~GSDI_TASK_CMD_Q_SIG;  /* Turn off bit */
          }
      }

   /* do while mask requested does not match current mask */
   } while ((rex_signals_mask & requested_mask) == 0);

   return (rex_signals_mask);

} /* end gsdi_wait() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UTIL_READ_MESSAGE

DESCRIPTION
  Function used to retrieve messages from the GSDI_TASK_CMD_Q.  Basically,
  this is the main wait for the GSDI Task.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_util_read_message(
  void * message_p,
  rex_sigs_type sigs_rcvd,
  rex_sigs_type sigs
)
{
    gsdi_returns_T                      gsdi_status    = GSDI_ERROR;
    gsdi_task_cmd_type                  *cmd_ptr       = NULL;
    word                                message_length = 0x00;
    gsdi_uim_link_established_T         msg;

    MMGSDI_RETURN_IF_NULL(message_p);

    memset(&msg, 0x00, sizeof(gsdi_uim_link_established_T));

    if (sigs_rcvd & GSDI_UIM_SANITY_TIMER_EXPIRE_SIG)
    {
        MSG_HIGH("GSDI_UIM_SANITY_TIMER_EXPIRE_SIG received",0,0,0);

        /* Clear the signal set in gsdi_send_cmd_to_uim_server() */
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

        /* Set the Message Header Contents */
        msg.message_header.message_set     = MS_GSDI_UIM_SANITY_TIMER_EXPIRE;
        msg.message_header.message_id      = GSDI_ERROR_CONDITION;

        /* Set the Message Length Properties */
        msg.message_header.message_len_msb = (byte)((sizeof(
          gsdi_uim_link_established_T )-sizeof(gsdi_cmdhdr_T))/0x100);
        msg.message_header.message_len_lsb = (byte)((sizeof(
          gsdi_uim_link_established_T)-sizeof(gsdi_cmdhdr_T))%0x100);

        message_length = ( msg.message_header.message_len_lsb +
                         ( 0x100 * msg.message_header.message_len_msb ) );

        /* Now copy the message to the GSDI Main Message Buffer */
        memcpy(message_p,&msg,message_length + sizeof(gsdi_cmdhdr_T));

        /* Indicate success for proper handling in gsdimain */
        gsdi_status = GSDI_SUCCESS;
    }
    else if (sigs_rcvd & GSDI_HIGH_PRI_CMD_Q_SIG)
    {
        if ((cmd_ptr = (gsdi_task_cmd_type*)q_get(&gsdi_high_pri_cmd_q)) != NULL)
        {
            MSG_MED("GSDI_HIGH_PRI_CMD_Q_SIG received",0,0,0);

#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))
            message_length = ( ((IMH_T *)&cmd_ptr->cmd)->message_len_lsb +
                ( 0x100 * ((IMH_T *)&cmd_ptr->cmd)->message_len_msb ) );
#else
            GET_IMH_LEN(message_length,((IMH_T *)&cmd_ptr->cmd));
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))*/

            if ((message_length + sizeof(gsdi_cmdhdr_T)) > GSDI_MAX_MSG_LEN )
            {
              /* Deallocate the cmd queue */
              MMGSDIUTIL_TMC_MEM_FREE( cmd_ptr);
              return GSDI_ERROR;
            }

            /* Copy data from command queue to local buffer */
            memcpy(message_p,
                   &cmd_ptr->cmd,
                   int32touint32(message_length) + sizeof(gsdi_cmdhdr_T));

            /* Deallocate the cmd queue */
            MMGSDIUTIL_TMC_MEM_FREE( cmd_ptr);

            gsdi_status = GSDI_SUCCESS;
        }
        else
        {
            /*  No items on the queue. Clear signal */
            (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_HIGH_PRI_CMD_Q_SIG);

            /* Loop back around in the main task */
        }
    }
    else if (sigs_rcvd & GSDI_TASK_CMD_Q_SIG)
    {
        if ((cmd_ptr = (gsdi_task_cmd_type*)q_get(&gsdi_task_cmd_q)) != NULL)
        {
            MSG_MED("GSDI_TASK_CMD_Q_SIG received",0,0,0);

#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))

            message_length = ( ((IMH_T *)&cmd_ptr->cmd)->message_len_lsb +
                ( 0x100 * ((IMH_T *)&cmd_ptr->cmd)->message_len_msb ) );
#else
            GET_IMH_LEN(message_length,((IMH_T *)&cmd_ptr->cmd));
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))*/

            if ((message_length + sizeof(gsdi_cmdhdr_T)) > GSDI_MAX_MSG_LEN )
            {
              /* Deallocate the cmd queue */
              MMGSDIUTIL_TMC_MEM_FREE(cmd_ptr);
              return GSDI_ERROR;
            }

            memcpy(message_p,
                   &cmd_ptr->cmd,
                   int32touint32(message_length) + sizeof(gsdi_cmdhdr_T));

            /* copy path if it is image request */
            (void)gsdi_util_malloc_and_copy_data_ptr_member(
              &cmd_ptr->cmd, (gsdi_task_cmd_data_type*)message_p);

            gsdi_util_free_data_ptr_member(&cmd_ptr->cmd);

            /* Deallocate the cmd queue */
            MMGSDIUTIL_TMC_MEM_FREE( cmd_ptr);

            gsdi_status = GSDI_SUCCESS;
        }
        else
        {
            /*  No items on the queue. Clear signal */
            (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_TASK_CMD_Q_SIG);

            /* Loop back around in the main task */
        }
    }
    else if (sigs_rcvd & GSDI_UIM_ERROR_SIG)
    {
        MSG_HIGH("GSDI_UIM_ERROR_SIG received",0,0,0);

        /* Clear signal */
        (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_ERROR_SIG);

        msg.message_header.message_set = MS_UIM_SERVER;
        msg.message_header.message_id  = GSDI_ERROR_CONDITION;

        GSDI_PUT_IMH_LEN( sizeof( gsdi_uim_link_established_T ),
                      &msg.message_header );

#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))
        message_length = ( ((IMH_T *)&msg)->message_len_lsb +
            ( 0x100 * ((IMH_T *)&msg)->message_len_msb ) );
#else
        GET_IMH_LEN(message_length,((IMH_T *)(void*)&msg));
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))*/

        memcpy(message_p,
               &msg,
               message_length + sizeof(gsdi_cmdhdr_T));

        gsdi_status = GSDI_SUCCESS;
    }

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
    /* Signals to Handle
       GSDI_API_TESTER_READ_TIMER_SIG
       GSDI_API_TESTER_WRITE_TIMER_SIG
       GSDI_API_TESTER_GFA_TIMER_SIG
       GSDI_API_TESTER_GSC_TIMER_SIG
    */
    else if ( sigs_rcvd & GSDI_API_TESTER_READ_TIMER_SIG)
    {
       rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_READ_TIMER_SIG);
       MSG_ERROR("DEBUG: KICK OFF READ TESTS",0,0,0);
       gsdi_api_tester_kick_off_read_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_WRITE_TIMER_SIG)
    {
       rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_WRITE_TIMER_SIG);
       MSG_ERROR("DEBUG: KICK OFF WRITE TESTS",0,0,0);
       gsdi_api_tester_kick_off_write_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_GPS_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_GPS_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF GET PIN STATUS TESTS",0,0,0);
      gsdi_api_tester_kick_off_pin_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_GFA_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_GFA_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF GET FILE ATTRIBUTE TESTS",0,0,0);
      gsdi_api_tester_kick_off_get_file_attr_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_SELECT_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_SELECT_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF SELECT TESTS",0,0,0);
      gsdi_api_tester_kick_off_select_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_GSC_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_GSC_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF GET SIM CAP TESTS",0,0,0);
      gsdi_api_tester_kick_off_sim_capability_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_INCREASE_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_INCREASE_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF INCREASE TESTS",0,0,0);
      gsdi_api_tester_kick_off_increase_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_SEARCH_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_SEARCH_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF SEARCH TESTS",0,0,0);
//      gsdi_api_tester_kick_off_search_tests();
      gsdi_api_tester_kick_off_stream_apdu_tests();
    }
    else if ( sigs_rcvd & GSDI_API_TESTER_OPEN_CHANNEL_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_OPEN_CHANNEL_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF OPEN CHANNEL TESTS",0,0,0);
      gsdi_api_tester_kick_off_open_channel_tests();
    }
#ifdef FEATURE_GSTK
    else if ( sigs_rcvd & GSDI_API_TESTER_NETWORK_RESEL_ACCESS_ENV_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_API_TESTER_NETWORK_RESEL_ACCESS_ENV_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF Network Resel Acc Env TESTS",0,0,0);
//      gsdi_api_tester_send_location_env_and_read_write_access();
    }
#endif /*FEATURE_GSTK*/
#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
    else if ( sigs_rcvd & GSDI_EVENT_TESTER_KICK_OFF_TIMER_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_EVENT_TESTER_KICK_OFF_TIMER_SIG);
      MSG_ERROR("DEBUG:  KICK OFF event tests",0,0,0);
      gsdi_event_tester_init_virtuim_and_gsdi_global();
    }
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */
#ifdef FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP
    else if (sigs_rcvd & GSDI_GSTK_CMD_UNIT_TESTER_APP_SIG)
    {
      rex_clr_sigs(&gsdi_task_tcb, GSDI_GSTK_CMD_UNIT_TESTER_APP_SIG);
      MSG_ERROR("DEBUG:  Gstk cmd unit TesterApp process",0,0,0);
      gsdi_gstk_cmd_unit_tester_app_process();
    }
#endif /*FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP */
    else
    {
        ERR("GSDI TASK INVALID SIGS: %x",sigs_rcvd,0,0);

        /* Reset the Sigs by Masking out Good Sigs and
        ** Clearing out Bad SIGS
        */
        (void)rex_clr_sigs(&gsdi_task_tcb, (sigs_rcvd & ~sigs) );

        /* Preserve any Signals currently Set and wipe */
        /* out the bad Signals                         */
        sigs_rcvd &= sigs;

        gsdi_status = GSDI_ERROR;
    }

    return gsdi_status;

} /* gsdi_util_read_message */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UTIL_SEND_MESSAGE

DESCRIPTION
  Function is used to send a message.  The Transport mechanism for sending
  the message is the General Services (GS) Task.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_util_send_message (
  gs_queue_id_T   queue_id,
  void          * message_p
)
{
    gsdi_returns_T      gsdi_status      = GSDI_SUCCESS;
    gsdi_lib_cmd_type * gsdi_lib_cmd_ptr = NULL_PTR;
    word                message_length   = 0x00;
#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM */
#ifdef FEATURE_NAS
#error code not present
#endif /*#ifdef FEATURE_NAS*/

    MMGSDI_RETURN_IF_NULL(message_p);
    MSG_LOW("Start of gsdi_lib_send_message",0,0,0);

    /* Initialize return value */
    gsdi_status = GSDI_SUCCESS;

    /* Get command buffer from destination task */
    switch (queue_id) {
    /* Message destination is GSDI Library */
    case GS_QUEUE_GSDI_LIB  :

        /* Get message length */
#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))
            message_length = ( ((IMH_T *)message_p)->message_len_lsb +
                ( 0x100 * ((IMH_T *)message_p)->message_len_msb ) );
#else
            GET_IMH_LEN(message_length,message_p);
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))*/

        /* Add in header size */
        message_length += sizeof(gsdi_rsphdr_T);

        gsdi_lib_cmd_ptr = gsdi_lib_get_cmd_buf();

        if (gsdi_lib_cmd_ptr != NULL)
        {
            /* Copy data to command buffer */
            memcpy(&gsdi_lib_cmd_ptr->cmd,
                   message_p,
                   message_length);

            /* Put message in destination task queue */
            gsdi_lib_put_cmd_buf(gsdi_lib_cmd_ptr);

            /* Set return status */
            gsdi_status = GSDI_SUCCESS;
        }
        else
        {
            /* Set return status */
            gsdi_status = GSDI_ERROR;
        }

        break;
#ifdef FEATURE_NAS
#error code not present
#endif
    default :

        /* Set return status */
#if defined (FEATURE_MMGSDI_UMTS) || defined (FEATURE_MMGSDI_GSM)
#error code not present
#endif

        break;
    }

    if ( gsdi_status != GSDI_SUCCESS )
    {
        MSG_ERROR("GSDI cannot send message",0,0,0);
    }

    return gsdi_status;
} /* gsdi_util_send_message */


#ifdef FEATURE_MMGSDI_DUAL_SLOT
/* <EJECT> */
/*===========================================================================
FUNCTION gsdi_util_update_nv_user_pref

DESCRIPTION
  Function is used to send a message.  The Transport mechanism for sending
  the message is the General Services (GS) Task.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_util_update_nv_user_pref(gsdi_app_enum_type app)
{
    nv_item_type      gsdi_nv_data_buf;      /* NV data buffer */
    nv_stat_enum_type nv_status = NV_DONE_S; /* NV CMD Status */
    gsdi_slot_id_type *user_selected_slot;
    gsdi_slot_id_type *nv_slot;
    nv_items_enum_type user_pref_slot_nv_item;
    nv_uim_slot_type  *nv_uim_slot_p;


    switch (app)
    {
      case GSDI_GSM_APP:
        user_selected_slot = &(gsdi_data.user_slot_prefs.user_switch_gsm_app_pref);
        nv_slot = &(gsdi_data.user_slot_prefs.gsm_app_pref);
        user_pref_slot_nv_item = NV_UIM_GSM_PREF_SLOT_I;
        nv_uim_slot_p = (nv_uim_slot_type*)&(gsdi_nv_data_buf.uim_gsm_pref_slot);
        break;
      case GSDI_CDMA_APP:
        user_selected_slot = &(gsdi_data.user_slot_prefs.user_switch_cdma_app_pref);
        nv_slot = &(gsdi_data.user_slot_prefs.cdma_app_pref);
        user_pref_slot_nv_item = NV_UIM_CDMA_PREF_SLOT_I;
        nv_uim_slot_p = (nv_uim_slot_type*)&(gsdi_nv_data_buf.uim_cdma_pref_slot);
        break;
      default:
        MSG_ERROR("Invalid app 0x%x", app, 0, 0);
        return;
    }
    /* Write to NV and UIM data switch slot if required */
    if (*user_selected_slot != *nv_slot)
    {
      /* Write the new user preferred slot to NV */
      switch (*user_selected_slot) {
      case GSDI_SLOT_NONE:
          *nv_uim_slot_p = NV_UIM_SLOT_NONE;
          break;

      case GSDI_SLOT_1:
          *nv_uim_slot_p = NV_UIM_SLOT_1;
          break;

      case GSDI_SLOT_2:
          *nv_uim_slot_p = NV_UIM_SLOT_2;
          break;

      default:
          MSG_ERROR("BAD SLOT ID: %x",*user_selected_slot,0,0);
          /* reset the user switch slot to the old data */
          *user_selected_slot = *nv_slot;
          return;
      }
      nv_status = gsdi_put_nv( user_pref_slot_nv_item, &gsdi_nv_data_buf );
      if (nv_status != NV_DONE_S)
      {
          MSG_ERROR( "NV PUT FAIL: 0x%x App %x", user_pref_slot_nv_item, nv_status, 0 );
          *user_selected_slot = *nv_slot;
          return;
      }
      *nv_slot = *user_selected_slot;
    }

} /* gsdi_util_update_nv_user_pref */
#endif /*FEATURE_MMGSI_DUAL_SLOT */


#if defined(FEATURE_GSDI_PHONEBOOK_SUPPORT) && defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*#if defined(FEATURE_GSDI_PHONEBOOK_SUPPORT) && defined(FEATURE_MMGSDI_UMTS)  */

/*===========================================================================
FUNCTION gsdi_populate_file_attr_structure

INPUT
 gsdi_slot_id_type       slot,
  gsdi_returns_T          gsdi_status,
  uint8                  *data_p,
  gsdi_file_attributes_T *file_attr_ptr

OUTPUT
  void

DESCRIPTION
  populate the raw data received from UIM and put it into the
  gsdi_file_attributes_T

RETURN VALUE
  void
===========================================================================*/
void gsdi_util_populate_file_attr_structure(
  gsdi_slot_id_type       slot,
  gsdi_returns_T          gsdi_status,
  uint8                  *data_p,
  gsdi_file_attributes_T *file_attr_ptr)
{
  gsdi_card_mode_capability_T card_mode = GSDI_CARD_MODE_UNKNOWN;

  if (data_p == NULL)
    return;
  if (file_attr_ptr == NULL)
    return;

  switch (slot)
  {
    case GSDI_SLOT_1:
      card_mode = gsdi_data.current_card_mode_capability;
      break;
  #ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      card_mode = gsdi_data.current_card_mode_capability_2;
      break;
    case GSDI_SLOT_AUTOMATIC:
      /* the operation has failed since if not, the slot will have been updated to a valid value */
      break;
  #endif /* FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
      card_mode = gsdi_data.current_card_mode_capability;
      break;
  }
  if ( gsdi_status == GSDI_SUCCESS )
  {
    file_attr_ptr->file_size                    = data_p[GSDI_ATTR_FILE_SIZE_MSB_OFFSET] * 16777216 +
                                                        data_p[GSDI_ATTR_FILE_SIZE_MSB_OFFSET+1] * 65536 +
                                                        data_p[GSDI_ATTR_FILE_SIZE_MSB_OFFSET+2] * 256 +
                                                        data_p[GSDI_ATTR_FILE_SIZE_LSB_OFFSET];
    file_attr_ptr->file_id[0]                   = data_p[GSDI_ATTR_FILE_ID_MSB_OFFSET];
    file_attr_ptr->file_id[1]                   = data_p[GSDI_ATTR_FILE_ID_LSB_OFFSET];
    file_attr_ptr->file_type                    = (gsdi_file_types_T)data_p[GSDI_ATTR_FILE_TYPE_OFFSET];
    file_attr_ptr->cyclic_increase_allowed      = data_p[GSDI_ATTR_INCREASE_OFFSET];
    file_attr_ptr->file_protection.read         = data_p[GSDI_ATTR_PROT_READ_OFFSET];
    file_attr_ptr->file_protection.write        = data_p[GSDI_ATTR_PROT_WRITE_OFFSET];
    file_attr_ptr->file_protection.increase     = data_p[GSDI_ATTR_PROT_INCREASE_OFFSET];
    file_attr_ptr->file_protection.invalidate   = data_p[GSDI_ATTR_PROT_INVAL_OFFSET];
    file_attr_ptr->file_protection.rehabilitate = data_p[GSDI_ATTR_PROT_REHAB_OFFSET];
    file_attr_ptr->file_invalidated             = data_p[GSDI_ATTR_VALID_FLAG_OFFSET];
    file_attr_ptr->file_structure               = (gsdi_file_structures_T)data_p[GSDI_ATTR_FILE_STRUCT_OFFSET];
    file_attr_ptr->rw_invalidated_deactivated_allowed = data_p[GSDI_ATTR_RW_WHEN_INV_DACT_OFFSET];

    switch(card_mode)
    {
      case GSDI_CARD_USIM:
        /* Determine File Structure */
        if ( file_attr_ptr->file_structure == GSDI_TRANSPARENT_FILE )
        {
          /* For upper layers to properly function */
          /* Set the Record Length to the File Size */
          file_attr_ptr->rec_len =
            (uint8)file_attr_ptr->file_size;
        }
        else
        {
          /* File is either Cyclic or Linear */
          file_attr_ptr->rec_len = data_p[GSDI_ATTR_REC_LEN_OFFSET];
        }
        file_attr_ptr->num_of_records = data_p[GSDI_ATTR_REC_NUM_OFFSET];
        break;

      case GSDI_CARD_GSM:
      case GSDI_CARD_RUIM:
        if ( file_attr_ptr->file_structure == GSDI_TRANSPARENT_FILE )
        {
          /* For upper layers to properly function */
          /* Set the Record Length to the File Size */
          file_attr_ptr->rec_len =
            (uint8)file_attr_ptr->file_size;
          /* EF Is Binary/Transparent */
          file_attr_ptr->num_of_records = data_p[GSDI_ATTR_REC_NUM_OFFSET];
        }
        else
        {
          /* File is either Cyclic or Linear */
          file_attr_ptr->rec_len = data_p[GSDI_ATTR_REC_LEN_OFFSET];
          if (file_attr_ptr->rec_len > 0)
          {
            /* Calculate Number of Records */
            file_attr_ptr->num_of_records =
              (uint8)(file_attr_ptr->file_size /
              file_attr_ptr->rec_len);
          }
          else
          {
            /* Avoid Divide by 0 Error */
            MSG_ERROR("rec_len = %x", file_attr_ptr->rec_len,0,0);
            file_attr_ptr->num_of_records = data_p[GSDI_ATTR_REC_NUM_OFFSET];
          }
        }
        break;

      default: /* others */
          file_attr_ptr->rec_len = 0;
          file_attr_ptr->num_of_records = data_p[GSDI_ATTR_REC_NUM_OFFSET];
          break;
    }
  }
}
/* <EJECT> */
/*===========================================================================
FUNCTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_util_send_gsdi_cnf (
  gsdi_returns_T  gsdi_status,
  uint32          client_ref,
  gs_queue_id_T   queue_id,
  byte            rec_num_accessed,
  byte            num_records,
  int32           returned_data_len,
  byte *          data_p,
  rex_tcb_type    *return_tcb_p,
  rex_sigs_type   return_wait_sig,
  rex_crit_sect_type *return_crit_sec_p,
  void*           cmd_p,
  gsdi_resp_type* resp_type_p
)
{
  gsdi_slot_id_type        slot                    = GSDI_SLOT_NONE;
  gsdi_returns_T           gsdi_return_status      = GSDI_SUCCESS;
  gsdi_cnf_T               msg                     = {{0}};
  dword                    msg_size                = 0;
  int32                    loop                    = 0;
  boolean                  mem_available           = TRUE;
  mmgsdi_pin_enum_type     pin_id                  = MMGSDI_MAX_PIN_ENUM;
  mmgsdi_static_data_type  pin_data;

  #if defined(FEATURE_GSDI_PHONEBOOK_SUPPORT) && \
      defined(FEATURE_MMGSDI_UMTS)
#error code not present
  #endif /* FEATURE_GSDI_PHONEBOOK_SUPPORT &&
            FEATURE_MMGSDI_UMTS */
  boolean           return_resp = TRUE;

  MMGSDI_RETURN_IF_NULL(cmd_p);
  MMGSDI_RETURN_IF_NULL(resp_type_p);
  /*
  ** THE GSDI_CNF MESSAGE IS VARIABLE LENGTH, BECAUSE ITS LAST
  ** PARAM 'DATA' HAS LEN 0...255.
  */

  /* Fill in header information */
  msg.message_header.message_set     = MS_GSDI;
  msg.message_header.message_id      = GSDI_CNF;

  msg_size = ((sizeof(gsdi_cnf_T) -        /* max msg size    */
             sizeof(gsdi_rsphdr_T)) -     /* msg header len  */
             GSDI_MAX_DATA_BLOCK_LEN) +   /* max data len    */
             int32touint32(returned_data_len); /* actual data len */

  msg.message_header.message_len_lsb = (byte)(msg_size % 256);
  msg.message_header.message_len_msb = (byte)(msg_size / 256);
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr         = return_tcb_p;
  msg.message_header.crit_sec        = return_crit_sec_p;
#endif /* !FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig        = return_wait_sig;
  msg.message_header.gsdi_status     = gsdi_status;
  msg.message_header.client_ref      = client_ref;
  msg.message_header.resp_type       = *(resp_type_p);
  msg.message_header.slot            =((gsdi_cmdhdr_T *)cmd_p)->slot;

  /* Set the Slot ID */
  slot = ((gsdi_cmdhdr_T *)cmd_p)->slot;

  switch(*(resp_type_p))
  {
  case GSDI_INTERNAL_RSP:
    break;

  case GSDI_GET_ECC_RSP:
      if(gsdi_status == GSDI_SUCCESS)
      {
        msg.ecc_cnf.emergency_call_code.num_of_rec = gsdi_ecc_table_p->num_of_rec;
        if(gsdi_ecc_table_p->num_of_rec > 0)
        {
          while((loop < gsdi_ecc_table_p->num_of_rec) &&
                (loop < GSDI_MAX_ECC_RECORDS) &&
                (mem_available == TRUE ))
          {
            /* Allocate memory for the new record */
            msg.ecc_cnf.emergency_call_code.ecc_p[loop] =
              mmgsdi_malloc(sizeof(gsdi_ecc_info_T));
            if(msg.ecc_cnf.emergency_call_code.ecc_p[loop] == NULL)
            {
              MSG_ERROR("NO HEAP SPACE FOR ECC POINTER ALLOC", 0, 0, 0);
              mem_available = FALSE;
            }
            else
            {
              /* Memset the new record pointer */
              memset(msg.ecc_cnf.emergency_call_code.ecc_p[loop],
                     0xFF,
                     sizeof(gsdi_ecc_info_T)
                    );
              memcpy(msg.ecc_cnf.emergency_call_code.ecc_p[loop],
                     gsdi_ecc_table_p->ecc_p[loop],
                     sizeof(gsdi_ecc_info_T)
                    );
              loop++;
            }
          }/* while((loop < ecc_table_p->num_of_rec) && (loop < GSDI_MAX_ECC_RECORDS)) */
        }/* if(ecc_table_p->num_of_rec > 0) */
      }/* if(gsdi_status == GSDI_SUCCESS) */
      else
      {
        msg.ecc_cnf.emergency_call_code.num_of_rec = 0;
        //msg.ecc_cnf.emergency_call_code.ecc_p      = NULL;
      }
    break;

    case GSDI_SIM_READ_RSP:
      /* Clear the buffer with 0xFF.  0xFF represents  */
      /*  uninitialized data                           */
      memset( msg.read_cnf.data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );
      msg.read_cnf.sim_filename = ((gsdi_sim_read_req_T *)cmd_p)->sim_filename;

      if ((msg.read_cnf.sim_filename & (UIM_EF_PATH_START << 8)) == (UIM_EF_PATH_START << 8))
      {
        msg.read_cnf.sim_filename = (word)UIM_EF_BY_PATH;
      }

      if (sizeof(msg.read_cnf.data) < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.read_cnf.data);
      }

      if (gsdi_status == GSDI_SUCCESS)
      {
        MMGSDI_RETURN_IF_NULL(data_p);
        msg.read_cnf.rec_num_accessed  = rec_num_accessed;
        msg.read_cnf.returned_data_len = int32touint8(returned_data_len);
        memcpy( msg.read_cnf.data, data_p, int32touint32(returned_data_len));
      }
      else
      {
        msg.read_cnf.returned_data_len = 0;
        msg.read_cnf.rec_num_accessed  = 0;
      }
      break;

    case GSDI_GET_IMAGE_RSP:
      /* initialize returned data len to 0 */
      msg.image_cnf.returned_data_len = 0;
      msg.image_cnf.data_p = NULL;
      if (gsdi_status == GSDI_SUCCESS)
      { /* malloc data */
        msg.image_cnf.data_p = (uint8*) mmgsdi_malloc(int32touint32(returned_data_len));
        if (msg.image_cnf.data_p != NULL)
        { /* Clear the buffer with 0xFF */
          MMGSDI_RETURN_IF_NULL(data_p);
          memset( msg.image_cnf.data_p, 0xFF, int32touint32(returned_data_len));
          msg.image_cnf.returned_data_len = int32touint8(returned_data_len);
          memcpy( msg.image_cnf.data_p, data_p, int32touint32(returned_data_len));
        }
        else
          MSG_ERROR("NO HEAP SPACE FOR IMAGE ALLOC", 0, 0, 0);
      }
      else
        msg.image_cnf.returned_data_len = 0;

      break;

    case GSDI_SIM_ACCESS_RSP:
      /* initialize returned data len to 0 */
      msg.sim_access_cnf.sim_filename = (word)((gsdi_sim_access_req_T *)cmd_p)->sim_filename;
      msg.sim_access_cnf.returned_data_len = 0;
      msg.sim_access_cnf.data_p = NULL;

      if (gsdi_status == GSDI_SUCCESS)
      { /* malloc data */
        msg.sim_access_cnf.data_p =
          (uint8*) mmgsdi_malloc(int32touint32(returned_data_len));
        if (msg.sim_access_cnf.data_p != NULL)
        { /* Clear the buffer with 0xFF */
          memset( msg.sim_access_cnf.data_p, 0xFF, int32touint32(returned_data_len));
          msg.sim_access_cnf.returned_data_len = returned_data_len;
          memcpy( msg.sim_access_cnf.data_p, data_p, int32touint32(returned_data_len));
          msg.sim_access_cnf.rec_num_accessed  = rec_num_accessed;
        }
        else
          MSG_ERROR("NO HEAP SPACE FOR IMAGE ALLOC", 0, 0, 0);
      }
      else
        msg.sim_access_cnf.returned_data_len = 0;
      break;


    case GSDI_SIM_WRITE_RSP:
      msg.write_cnf.sim_filename = ((gsdi_sim_write_req_T *)cmd_p)->sim_filename;
      if ((msg.write_cnf.sim_filename & (UIM_EF_PATH_START << 8)) == (UIM_EF_PATH_START << 8))
      {
        msg.write_cnf.sim_filename = (word)UIM_EF_BY_PATH;
      }

      msg.write_cnf.rec_num_accessed   = rec_num_accessed;
      break;

    case GSDI_SIM_INCREASE_RSP:
      if (sizeof(msg.increase_cnf.data) < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.increase_cnf.data);
      }

      if (gsdi_status == GSDI_SUCCESS)
      {
        /* Set the Buffer to 0xFF */
        MMGSDI_RETURN_IF_NULL(data_p);
        memset( msg.increase_cnf.data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );
        msg.increase_cnf.new_value_len   = int32touint8(returned_data_len);
        memcpy( msg.increase_cnf.data, data_p, int32touint32(returned_data_len));
      }
      else
      {
        msg.increase_cnf.new_value_len = 0;
        msg.increase_cnf.data[0] = 0x00;
      }
      break;

    case GSDI_RUN_GSM_ALGORITHM_RSP:
      if (sizeof(msg.run_gsm_alg_cnf.data) < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.run_gsm_alg_cnf.data);
      }

      if (gsdi_status == GSDI_SUCCESS)
      {
        /* Clear the buffer with 0xFF */
        MMGSDI_RETURN_IF_NULL(data_p);
        memset( msg.run_gsm_alg_cnf.data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );
        msg.run_gsm_alg_cnf.returned_data_len   = int32touint8(returned_data_len);
        memcpy( msg.run_gsm_alg_cnf.data, data_p, int32touint32(returned_data_len));
      }
      else
      {
        msg.run_gsm_alg_cnf.returned_data_len = 0;
        msg.run_gsm_alg_cnf.data[0] = 0x00;
      }
      break;

    case GSDI_SELECT_RSP:
      if (sizeof(msg.select_cnf.data) < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.select_cnf.data);
      }

      if (gsdi_status == GSDI_SUCCESS)
      {
        MMGSDI_RETURN_IF_NULL(data_p);
        /* Clear the buffer with FF */
        memset( msg.select_cnf.data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );
        msg.select_cnf.returned_data_len   = int32touint8(returned_data_len);
        memcpy( msg.select_cnf.data, data_p, int32touint32(returned_data_len));
      }
      else
      {
        msg.select_cnf.returned_data_len = 0;
        msg.select_cnf.data[0] = 0x00;
      }
      break;

    case GSDI_GET_FILE_ATTRIBUTES_RSP:
      gsdi_util_populate_file_attr_structure(
        slot,
        gsdi_status,
        data_p,
        &msg.gfa_cnf.gfa_data);
      break;

    case GSDI_GET_PIN_STATUS_RSP:
        /* populate pin id, status and num of retries   */
        /* number of retries included for the specific  */
        /* cnf struct as well as the general cnf struct */
        MMGSDI_RETURN_IF_NULL(data_p);
        msg.pin_status_cnf.pin_id = ((gsdi_get_pin_status_req_T *)cmd_p)->pin_id;
        /* Set PIN STATUS */
        msg.pin_status_cnf.status = (gsdi_pin_status_T)data_p[0];

        if (((gsdi_get_pin_status_req_T*)cmd_p)->pin_id == GSDI_PIN1)
        {
          switch(slot)
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.pin_status_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);

              if(msg.pin_status_cnf.num_of_retries == 0)
              {
                mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                             mmgsdi_util_get_sel_app(),
                                             &msg.pin_status_cnf.num_of_retries,
                                             MMGSDI_GET_NUM_UNBLOCK_RETRIES);
              }
              break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed == 0)
                msg.pin_status_cnf.num_of_retries =
                  gsdi_data.pin_status_info_2.puk1_num_retries_allowed;
              else
                msg.pin_status_cnf.num_of_retries =
                  gsdi_data.pin_status_info_2.pin1_num_retries_allowed;
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("UNKNOWN SLOT %x",slot,0,0);
              break;
          }
        }
        else
        {
          switch (slot)
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.pin_status_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);

              if(msg.pin_status_cnf.num_of_retries == 0)
              {
                mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                             mmgsdi_util_get_sel_app(),
                                             &msg.pin_status_cnf.num_of_retries,
                                             MMGSDI_GET_NUM_UNBLOCK_RETRIES);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed == 0)
                msg.pin_status_cnf.num_of_retries =
                  gsdi_data.pin_status_info_2.puk2_num_retries_allowed;
              else
                msg.pin_status_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin2_num_retries_allowed;
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("UNKNOWN SLOT %x",slot,0,0);
              break;
          }
        }
        break;

    case GSDI_VERIFY_PIN_RSP: /* populate pin id, num of retries */
      /* number of retries included for the specific cnf struct as well as the general cnf struct */
      msg.verify_pin_cnf.pin_id = (gsdi_pin_ids_T)((gsdi_verify_pin_req_T *)cmd_p)->pin_id;
      if(data_p != NULL)
      {
       /* Copy UIM status words from data pointer to CNF type */
        msg.verify_pin_cnf.status_words.sw1 = data_p[0];
        msg.verify_pin_cnf.status_words.sw2 = data_p[1];
      }
      else
      {
        MSG_HIGH("Status words not retrieved!  Data pointer is NULL",0,0,0);
        msg.verify_pin_cnf.status_words.sw1 = 0xFF;
        msg.verify_pin_cnf.status_words.sw2 = 0xFF;
      }


      /* pin1 or pin2 status started with GSDI_PIN_ENABLED */
      if (((gsdi_verify_pin_req_T*)cmd_p)->pin_id == (int)GSDI_PIN1)
      {
        switch ( slot )
        {
          case GSDI_SLOT_1:
            mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                         mmgsdi_util_get_sel_app(),
                                         &msg.verify_pin_cnf.num_of_retries,
                                         MMGSDI_GET_NUM_RETRIES);
            if(gsdi_status == GSDI_SUCCESS)
            {
               (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN1, GSDI_PIN_ENABLED);
            }
            break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
          case GSDI_SLOT_2:
            msg.verify_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin1_num_retries_allowed;

            /* update pin status global data */
            if (gsdi_status == GSDI_SUCCESS)
            {
              gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_ENABLED;
              /* Note: need to set go_to_verified_state to MMGSDI_PIN_STATE_VERIFIED
                 when we have support for dual slot in mmgsdi. */
            }
            else
            {
              if (gsdi_data.pin_status_info_2.puk1_num_retries_allowed == 0)
                gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_PERM_BLOCKED;
              else if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed == 0)
                gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
            }
            break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

          default:
            MSG_ERROR("UNKNOWN SLOT %x",slot,0,0);

            break;
        }
      }
      else
      {
        switch ( slot )
        {
          case GSDI_SLOT_1:
            mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                         mmgsdi_util_get_sel_app(),
                                         &msg.verify_pin_cnf.num_of_retries,
                                         MMGSDI_GET_NUM_RETRIES);
            if(gsdi_status == GSDI_SUCCESS)
            {
               (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN2, GSDI_PIN_ENABLED);
            }
            break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
          case GSDI_SLOT_2:
            msg.verify_pin_cnf.num_of_retries =
              gsdi_data.pin_status_info_2.pin2_num_retries_allowed;

            if (gsdi_status == GSDI_SUCCESS)
            {
              gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_ENABLED;

            }
            else
            {
              if (gsdi_data.pin_status_info_2.puk2_num_retries_allowed == 0)
              {
                gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_PERM_BLOCKED;
              }
              else if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed == 0)
              {
                gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
              }
            }
            break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

          default:
            MSG_ERROR("UNKNOWN SLOT %x",slot,0,0);
            break;
        }
      }
      /* If operation was successful store the PIN */
      if (gsdi_status == GSDI_SUCCESS)
      {
        pin_id = mmgsdi_util_convert_from_gsdi_pin_id((gsdi_pin_ids_T)
                                       ((gsdi_verify_pin_req_T *)cmd_p)->pin_id);
        pin_data.data_len = GSDI_PIN_LEN;
        memcpy(pin_data.data_ptr,
               ((gsdi_verify_pin_req_T *)cmd_p)->pin,
               GSDI_PIN_LEN
              );
        mmgsdi_util_store_pin(pin_id,pin_data);
      }
      break;

    case GSDI_CHANGE_PIN_RSP: /* populate pin id, num of retries */
      /* number of retries included for the specific cnf struct as well as the general cnf struct */
        msg.change_pin_cnf.pin_id = (gsdi_pin_ids_T)((gsdi_change_pin_req_T *)cmd_p)->pin_id;

        if(data_p != NULL)
        {
          /* Copy UIM status words from data pointer to CNF type */
          msg.change_pin_cnf.status_words.sw1 = data_p[0];
          msg.change_pin_cnf.status_words.sw2 = data_p[1];
        }
        else
        {
          MSG_HIGH("Status words not retrieved!  Data pointer is NULL",0,0,0);
          msg.change_pin_cnf.status_words.sw1 = 0xFF;
          msg.change_pin_cnf.status_words.sw2 = 0xFF;
        }

        /* pin1 or pin2 status started with GSDI_PIN_ENABLED */

        if (((gsdi_change_pin_req_T*)cmd_p)->pin_id == (int)GSDI_PIN1)
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.change_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN1, GSDI_PIN_ENABLED);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              msg.change_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin1_num_retries_allowed;

              /* update pin status */
              if (gsdi_status == GSDI_SUCCESS)
              {
                gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_ENABLED;
                /* Note: need to set go_to_verified_state to MMGSDI_PIN_STATE_VERIFIED
                 when we have support for dual slot in mmgsdi. */
              }
              else
              {
                if (gsdi_data.pin_status_info_2.puk1_num_retries_allowed == 0)
                {
                    gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_PERM_BLOCKED;
                }
                else
                {
                  if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
                }
              }
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
          }
        } /* end updating pin status */
        else
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.change_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN2, GSDI_PIN_ENABLED);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              msg.change_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin2_num_retries_allowed;

              /* update pin status */
              if (gsdi_status == GSDI_SUCCESS)
              {
                gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_ENABLED;
              }
              else
              {
                if (gsdi_data.pin_status_info_2.puk2_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_PERM_BLOCKED;
                else
                {
                  if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                }
              }
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
          }
        }
        /* If operation was successful store the PIN */
        if (gsdi_status == GSDI_SUCCESS)
        {
          pin_id = mmgsdi_util_convert_from_gsdi_pin_id((gsdi_pin_ids_T)
                                       ((gsdi_change_pin_req_T *)cmd_p)->pin_id);
          pin_data.data_len = GSDI_PIN_LEN;
          memcpy(pin_data.data_ptr,
                 ((gsdi_change_pin_req_T *)cmd_p)->new_pin,
                 GSDI_PIN_LEN
                );
          mmgsdi_util_store_pin(pin_id,pin_data);
        }
        break;

    case GSDI_UNBLOCK_PIN_RSP: /* populate pin id, num of retries */
      /* number of retries included for the specific cnf struct as well as the general cnf struct */
        msg.unblock_pin_cnf.pin_id = (gsdi_pin_ids_T)((gsdi_unblock_pin_req_T *)cmd_p)->pin_id;
        if(data_p != NULL)
        {
          /* Copy UIM status words from data pointer to CNF type */
          msg.unblock_pin_cnf.status_words.sw1 = data_p[0];
          msg.unblock_pin_cnf.status_words.sw2 = data_p[1];
        }
        else
        {
          MSG_HIGH("Status words not retrieved!  Data pointer is NULL",0,0,0);
          msg.unblock_pin_cnf.status_words.sw1 = 0xFF;
          msg.unblock_pin_cnf.status_words.sw2 = 0xFF;
        }

        /* pin1_status or pin2_status is GSDI_PIN_BLOCKED to start with */

        if (((gsdi_unblock_pin_req_T*)cmd_p)->pin_id == (int)GSDI_PIN1)
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.unblock_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_UNBLOCK_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN1, GSDI_PIN_ENABLED);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              msg.unblock_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.puk1_num_retries_allowed;

              /* update pin status */
              if (gsdi_status == GSDI_SUCCESS)
              {
                gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_ENABLED;
                /* Note: need to set go_to_verified_state to MMGSDI_PIN_STATE_VERIFIED
                 when we have support for dual slot in mmgsdi. */
              }
              else
              {
                if (gsdi_data.pin_status_info_2.puk1_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_PERM_BLOCKED;
              }
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
          }
        }
        else
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.unblock_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_UNBLOCK_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN2, GSDI_PIN_ENABLED);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              msg.unblock_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.puk2_num_retries_allowed;

              /* update pin status */
              if (gsdi_status == GSDI_SUCCESS)
                gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_ENABLED;
              else
              {
                if (gsdi_data.pin_status_info_2.puk2_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_PERM_BLOCKED;
              }
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
          }
        }
        /* If operation was successful store the PIN */
        if (gsdi_status == GSDI_SUCCESS)
        {
          pin_id = mmgsdi_util_convert_from_gsdi_pin_id((gsdi_pin_ids_T)
                                      ((gsdi_unblock_pin_req_T *)cmd_p)->pin_id);
          pin_data.data_len = GSDI_PIN_LEN;
          memcpy(pin_data.data_ptr,
                 ((gsdi_unblock_pin_req_T *)cmd_p)->new_pin,
                 GSDI_PIN_LEN
                );
          mmgsdi_util_store_pin(pin_id,pin_data);
        }
        break;

    case GSDI_DISABLE_PIN_RSP: /* populate pin id, num of retries */
      /* number of retries included for the specific cnf struct as well as the general cnf struct */
        msg.disable_pin_cnf.pin_id = (gsdi_pin_ids_T)((gsdi_disable_pin_req_T *)cmd_p)->pin_id;

        if(data_p != NULL)
        {
          /* Copy UIM status words from data pointer to CNF type */
          msg.disable_pin_cnf.status_words.sw1 = data_p[0];
          msg.disable_pin_cnf.status_words.sw2 = data_p[1];
        }
        else
        {
          MSG_HIGH("Status words not retrieved!  Data pointer is NULL",0,0,0);
          msg.disable_pin_cnf.status_words.sw1 = 0xFF;
          msg.disable_pin_cnf.status_words.sw2 = 0xFF;
        }

        if (((gsdi_disable_pin_req_T*)cmd_p)->pin_id == (int)GSDI_PIN1)
        {
        switch ( slot )
        {
          case GSDI_SLOT_1:
            mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                         mmgsdi_util_get_sel_app(),
                                         &msg.disable_pin_cnf.num_of_retries,
                                         MMGSDI_GET_NUM_RETRIES);
            if(gsdi_status == GSDI_SUCCESS)
            {
              (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN1, GSDI_PIN_DISABLED);
            }
            break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
        case GSDI_SLOT_2:
          msg.disable_pin_cnf.num_of_retries =
            gsdi_data.pin_status_info_2.pin1_num_retries_allowed;
            /* update pin status */
            if (gsdi_status == GSDI_SUCCESS)
              gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_DISABLED;
            else
            {
              if (gsdi_data.pin_status_info_2.puk1_num_retries_allowed == 0)
                  gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_PERM_BLOCKED;
              else
              {
                if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed == 0)
                  gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
              }
            }
            break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

           default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);

              break;
          }
        }
        else
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.disable_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN2, GSDI_PIN_DISABLED);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              msg.disable_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin2_num_retries_allowed;

              /* pin status update*/
              if (gsdi_status == GSDI_SUCCESS)
                gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_DISABLED;
              else
              {
                if (gsdi_data.pin_status_info_2.puk2_num_retries_allowed == 0)
                  gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_PERM_BLOCKED;
                else
                {
                  if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                }
              }
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
           }
        }
        /* If operation was successful store the PIN */
        if (gsdi_status == GSDI_SUCCESS)
        {
          pin_id = mmgsdi_util_convert_from_gsdi_pin_id((gsdi_pin_ids_T)
                                      ((gsdi_disable_pin_req_T *)cmd_p)->pin_id);
          pin_data.data_len = GSDI_PIN_LEN;
          memcpy(pin_data.data_ptr,
                 ((gsdi_disable_pin_req_T *)cmd_p)->pin,
                 GSDI_PIN_LEN
                );
          mmgsdi_util_store_pin(pin_id,pin_data);
        }
        break;

    case GSDI_ENABLE_PIN_RSP: /* populate pin id, num of retries */
      /* number of retries included for the specific cnf struct as well as the general cnf struct */
        msg.enable_pin_cnf.pin_id = (gsdi_pin_ids_T)((gsdi_enable_pin_req_T *)cmd_p)->pin_id;

        if(data_p != NULL)
        {
          /* Copy UIM status words from data pointer to CNF type */
          msg.enable_pin_cnf.status_words.sw1 = data_p[0];
          msg.enable_pin_cnf.status_words.sw2 = data_p[1];
        }
        else
        {
          MSG_HIGH("Status words not retrieved!  Data pointer is NULL",0,0,0);
          msg.enable_pin_cnf.status_words.sw1 = 0xFF;
          msg.enable_pin_cnf.status_words.sw2 = 0xFF;
        }

        if (((gsdi_enable_pin_req_T*)cmd_p)->pin_id == (int)GSDI_PIN1)
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN1,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.enable_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN1, GSDI_PIN_ENABLED);
              }
              break;
  #ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              msg.enable_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin1_num_retries_allowed;

              /* update pin status */
              if (gsdi_status == GSDI_SUCCESS)
              {
                gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_ENABLED;
                /* Note: need to set go_to_verified_state to MMGSDI_PIN_STATE_VERIFIED
                 when we have support for dual slot in mmgsdi. */
              }
              else
              {
                if (gsdi_data.pin_status_info_2.puk1_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_PERM_BLOCKED;
                else
                {
                  if (gsdi_data.pin_status_info_2.pin1_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_BLOCKED;
                }
              }
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
          }

        } /* end of updating pin status */
        else
        {
          switch ( slot )
          {
            case GSDI_SLOT_1:
              mmgsdi_util_gsdi_set_retries(MMGSDI_PIN2,
                                           mmgsdi_util_get_sel_app(),
                                           &msg.enable_pin_cnf.num_of_retries,
                                           MMGSDI_GET_NUM_RETRIES);
              if(gsdi_status == GSDI_SUCCESS)
              {
                (void)mmgsdi_util_convert_to_mmgsdi_pin_status(GSDI_PIN2, GSDI_PIN_ENABLED);
              }
              break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case GSDI_SLOT_2:
              /* update pin status */
              msg.enable_pin_cnf.num_of_retries =
                gsdi_data.pin_status_info_2.pin2_num_retries_allowed;
              if (gsdi_status == GSDI_SUCCESS)
                gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_ENABLED;
              else
              {
                if (gsdi_data.pin_status_info_2.puk2_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_PERM_BLOCKED;
                else
                {
                  if (gsdi_data.pin_status_info_2.pin2_num_retries_allowed == 0)
                    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_BLOCKED;
                }
              } /* end of updating in status */
              break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

            default:
              MSG_ERROR("BAD PIN ID: %x",0,0,0);
              break;
          }
        }
        /* If operation was successful store the PIN */
        if (gsdi_status == GSDI_SUCCESS)
        {
          pin_id = mmgsdi_util_convert_from_gsdi_pin_id((gsdi_pin_ids_T)
                                       ((gsdi_enable_pin_req_T *)cmd_p)->pin_id);
          pin_data.data_len = GSDI_PIN_LEN;
          memcpy(pin_data.data_ptr,
                 ((gsdi_enable_pin_req_T *)cmd_p)->pin,
                 GSDI_PIN_LEN
                );
          mmgsdi_util_store_pin(pin_id,pin_data);
        }
        break;

    case GSDI_GET_SIM_CAPABILITIES_RSP: /* get sim cap: fill up sim_capabilities struct using gsdi_data*/
      if ( gsdi_status == GSDI_SUCCESS)
      {
        switch ( slot )
        {
          case GSDI_SLOT_1:
            /* OK To Copy Contents */
            msg.get_sim_cap_cnf.sim_capabilities.fdn_enabled =
              gsdi_data.sim_capability.fdn_enabled;
            msg.get_sim_cap_cnf.sim_capabilities.bdn_enabled =
              gsdi_data.sim_capability.bdn_enabled;
            msg.get_sim_cap_cnf.sim_capabilities.acl_enabled =
              gsdi_data.sim_capability.acl_enabled;
            msg.get_sim_cap_cnf.sim_capabilities.imsi_invalidated =
              gsdi_data.sim_capability.imsi_invalidated;

            /* Copy all SIM Phonebook Related Information */
            msg.get_sim_cap_cnf.sim_capabilities.sim_phone_book.multiple_pbr_entries =
              gsdi_data.sim_capability.sim_phone_book.multiple_pbr_entries;
            msg.get_sim_cap_cnf.sim_capabilities.sim_phone_book.present =
              gsdi_data.sim_capability.sim_phone_book.present;

            /* Copy all USIM Local Phonebook Related Information */
            msg.get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.multiple_pbr_entries =
              gsdi_data.sim_capability.usim_local_phone_book.multiple_pbr_entries;
            msg.get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.present =
              gsdi_data.sim_capability.usim_local_phone_book.present;

            /* Copy all USIM Global Phonebook Related Information */
            msg.get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.multiple_pbr_entries =
              gsdi_data.sim_capability.usim_global_phone_book.multiple_pbr_entries;
            msg.get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.present =
              gsdi_data.sim_capability.usim_global_phone_book.present;
            break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
          case GSDI_SLOT_2:
            /* OK To Copy Contents */
            msg.get_sim_cap_cnf.sim_capabilities.fdn_enabled =
              gsdi_data.sim_capability_2.fdn_enabled;
            msg.get_sim_cap_cnf.sim_capabilities.bdn_enabled =
              gsdi_data.sim_capability_2.bdn_enabled;
            msg.get_sim_cap_cnf.sim_capabilities.acl_enabled =
              gsdi_data.sim_capability_2.acl_enabled;
            msg.get_sim_cap_cnf.sim_capabilities.imsi_invalidated =
              gsdi_data.sim_capability_2.imsi_invalidated;

            /* Copy all SIM Phonebook Related Information */
            msg.get_sim_cap_cnf.sim_capabilities.sim_phone_book.multiple_pbr_entries =
              gsdi_data.sim_capability_2.sim_phone_book.multiple_pbr_entries;
            msg.get_sim_cap_cnf.sim_capabilities.sim_phone_book.present =
              gsdi_data.sim_capability_2.sim_phone_book.present;

            /* Copy all USIM Local Phonebook Related Information */
            msg.get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.multiple_pbr_entries =
              gsdi_data.sim_capability_2.usim_local_phone_book.multiple_pbr_entries;
            msg.get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.present =
              gsdi_data.sim_capability_2.usim_local_phone_book.present;

            /* Copy all USIM Global Phonebook Related Information */
            msg.get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.multiple_pbr_entries =
              gsdi_data.sim_capability_2.usim_global_phone_book.multiple_pbr_entries;
            msg.get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.present =
              gsdi_data.sim_capability_2.usim_global_phone_book.present;
            break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

          default:
            MSG_ERROR("BAD PIN ID: %x",0,0,0);
            break;
        }
      }
      break;

    case GSDI_ENABLE_FDN_RSP:
      break;

    case GSDI_DISABLE_FDN_RSP:
      break;
    case GSDI_ILLEGAL_SIM_RSP:
      break;

    case GSDI_SIM_SEARCH_RSP:
      msg.search_cnf.rec_num_accessed  = rec_num_accessed;
      break;

    case GSDI_USIM_AUTHENTICATION_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);

      if (sizeof(msg.usim_auth_cnf.data) < int32touint32(returned_data_len) )
      {
        returned_data_len = sizeof(msg.usim_auth_cnf.data);
      }
      /* clear up the buffer with FF */
      memset( msg.usim_auth_cnf.data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );

      if (gsdi_status == GSDI_SUCCESS) 
      { 
        msg.usim_auth_cnf.returned_data_len = int32touint8(returned_data_len);
        /* fill with data from uim */
        memcpy(msg.usim_auth_cnf.data, data_p, int32touint32(returned_data_len));
      }
      else 
      { /* return data len of 1 and set the data[0] to 0 to represent error */
           msg.usim_auth_cnf.returned_data_len = 1;
           msg.usim_auth_cnf.data[0] = 0x00;
      }

      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SWITCH_SLOT_PREF_RSP:
        MMGSDI_RETURN_IF_NULL(data_p);
        /* data[0] will contain the boolean result as to whether or
        ** not there is a refresh pending
        */
        msg.switch_slot_pref_cnf.refresh_pending = data_p[0];
        break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
    case GSDI_SEND_APDU_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.send_apdu_cnf.sw1 = data_p[0];
      msg.send_apdu_cnf.sw2 = data_p[1];
      msg.send_apdu_cnf.resp_data_len = 0;
      break;
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */

    case GSDI_SIM_REFRESH_RSP:
      if ((gsdi_status != GSDI_SUCCESS) &&
          (gsdi_status != GSDI_REFRESH_SUCCESS) &&
          (gsdi_status != GSDI_REFRESH_IN_PROGRESS) &&
          (gsdi_status != GSDI_REFRESH_SUCCESS_BUT_NO_AID_ACTIVE))
      {
        (void)gsdi_util_send_mmi_sim_ind(GSDI_REFRESH_FAILED);
      }
      if (gsdi_status == GSDI_REFRESH_SUCCESS &&
          refresh_fcn_notify_list_node.additional_files_read)
      {
        msg.message_header.gsdi_status = GSDI_REFRESH_FCN_ADDITIONAL_FCN_READ;
      }
      msg.refresh_cnf.client_ref = client_ref;
      break;

    case GSDI_REFRESH_COMPLETED_RSP:
    case GSDI_REFRESH_REGISTRATION_VOTE_RSP:
    case GSDI_REFRESH_REGISTRATION_FCN_RSP:
      /* Do Nothing Else */
      break;

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
    case GSDI_OPEN_CHANNEL_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      if (sizeof(msg.open_channel_cnf.data_buffer) < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.open_channel_cnf.data_buffer);
      }

      if ( gsdi_status == GSDI_SUCCESS )
      {
        if ( returned_data_len > 0x02 )
          msg.open_channel_cnf.channel_id = data_p[0];
        else
          msg.open_channel_cnf.channel_id = data_p[2];

        msg.open_channel_cnf.data_len   = returned_data_len;
        memcpy(msg.open_channel_cnf.data_buffer,
               data_p,
               int32touint32(returned_data_len));
      }
      else
      {
        /* Indicate SW Error by setting the following */
        /* Set the returned data Length to 0 */
        --returned_data_len;
        if(returned_data_len >= 0)
        {
          msg.open_channel_cnf.channel_id = data_p[returned_data_len];
        }
        msg.open_channel_cnf.data_len   = returned_data_len;
      }
      break;

    case GSDI_CLOSE_CHANNEL_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      if (sizeof(msg.close_channel_cnf.data_buffer) < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.close_channel_cnf.data_buffer);
      }

      if ( gsdi_status == GSDI_SUCCESS )
      {
        --returned_data_len;
        if(returned_data_len >= 0){
         msg.close_channel_cnf.channel_id = data_p[returned_data_len];
        }
        msg.close_channel_cnf.data_len   = returned_data_len;
        memcpy(msg.close_channel_cnf.data_buffer,
               data_p,
               int32touint32(returned_data_len));
      }
      else
      {
        /* Set the channel ID and return a length of 0 */
        --returned_data_len;
        if(returned_data_len >= 0){
          msg.close_channel_cnf.channel_id = data_p[returned_data_len];
        }
        msg.close_channel_cnf.data_len   = returned_data_len;
      }
      break;

    case GSDI_GET_ATR_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.get_atr_cnf.data_len = returned_data_len;
      if (returned_data_len > 0) {
        if (returned_data_len > GSDI_MAX_ATR_BYTES) {
          returned_data_len = GSDI_MAX_ATR_BYTES;
        }
        memcpy(msg.get_atr_cnf.data_buffer,
               data_p,
               int32touint32(returned_data_len));
      }
      else {
        memset(msg.get_atr_cnf.data_buffer, 0x00, GSDI_MAX_ATR_BYTES);
      }
      break;

    case GSDI_SEND_CHANNEL_DATA_RSP:
      /* Subtract 1 from data to represent channel id len appended
      ** to the data
      */
      MMGSDI_RETURN_IF_NULL(data_p);
      if (sizeof(msg.send_channel_data_cnf.data_buffer) + 3  < int32touint32(returned_data_len))
      {
        returned_data_len = sizeof(msg.send_channel_data_cnf.data_buffer) + 3 ;
      }

      if ( gsdi_status == GSDI_SUCCESS )
      {
        if(returned_data_len >= 3)
        {
          --returned_data_len;
          msg.send_channel_data_cnf.implicit_get_rsp_sw2 = data_p[returned_data_len];
          --returned_data_len;
          msg.send_channel_data_cnf.implicit_get_rsp_sw1 = data_p[returned_data_len];
          --returned_data_len;
          msg.send_channel_data_cnf.channel_id     = data_p[returned_data_len];
          msg.send_channel_data_cnf.apdu_len       = returned_data_len;

          memcpy(msg.send_channel_data_cnf.data_buffer,
                 data_p,
                 int32touint32(returned_data_len));
        }
        else
        {
           MSG_ERROR("RETURNED DATA LEN IS ZERO",0,0,0);
        }
      }
      else
      {
        if (returned_data_len > 0)
        {
          msg.send_channel_data_cnf.channel_id = data_p[--returned_data_len];
        }
        msg.send_channel_data_cnf.apdu_len   = returned_data_len;
      }
      break;
#endif /*FEATURE_MMGSDI_CHANNEL_MANAGEMENT*/

    case GSDI_GET_ALL_PIN_INFO_RSP:
      {
        msg.all_pin_info_cnf.num_pins = GSDI_MAX_PIN_RECORDS;
        /* Allocate memory for putting the conf response */
        msg.all_pin_info_cnf.pin_info_items =
          mmgsdi_malloc(
            msg.all_pin_info_cnf.num_pins * sizeof(gsdi_pin_info_item_T));
        if (msg.all_pin_info_cnf.pin_info_items != NULL )
        {
          if ( data_p != NULL )
          {
            memcpy(msg.all_pin_info_cnf.pin_info_items,
                   data_p,
                   (sizeof(gsdi_pin_info_item_T) * msg.all_pin_info_cnf.num_pins) );
          }
          else
          {
            MSG_ERROR("NULL POINTER:  SEND ERROR",0,0,0);
            msg.message_header.gsdi_status = GSDI_ERROR;
          }
        }
        else
        {
          MSG_ERROR("Unable allocate memory for PIN info items", 0, 0, 0);
          msg.all_pin_info_cnf.num_pins = 0;
          msg.message_header.gsdi_status  = GSDI_ERROR;
        }
      }
      break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION
    case GSDI_PERSO_REG_TASK_RSP:
      break;
    case GSDI_PERSO_ACT_IND_RSP:
    {
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.act_ind_cnf.perso_feature = ((gsdi_perso_act_feature_ind_req_T *)cmd_p)->feature;
      /* Success or fail, data_p[0] must contain the number of retries */
      switch ( ((gsdi_perso_act_feature_ind_req_T *)cmd_p)->feature )
      {
#ifdef FEATURE_PERSO_SIM
      case GSDI_PERSO_NW:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_NW_OFFSET];
        break;
      case GSDI_PERSO_NS:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_NS_OFFSET];
        break;
      case GSDI_PERSO_SP:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_SP_OFFSET];
        break;
      case GSDI_PERSO_CP:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_CP_OFFSET];
        break;
      case GSDI_PERSO_SIM:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_SIM_OFFSET];
        break;
      case GSDI_PERSO_PROP1:
        MSG_ERROR("Not supported perso key GSDI_PERSO_PROP1", 0 , 0, 0);
        break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
      case GSDI_PERSO_RUIM_NW1:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_NW1_OFFSET];
        break;
      case GSDI_PERSO_RUIM_NW2:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_NW2_OFFSET];
        break;
      case GSDI_PERSO_RUIM_HRPD:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_HRPD_OFFSET];
        break;
      case GSDI_PERSO_RUIM_SP:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_SP_OFFSET];
        break;
      case GSDI_PERSO_RUIM_CP:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_CP_OFFSET];
        break;
      case GSDI_PERSO_RUIM_RUIM:
        msg.act_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_RUIM_OFFSET];
        break;
#endif /* FEATURE_PERSO_RUIM */
      case GSDI_PERSO_NONE:
      default:
        break;
      }
    }
    break;
    case GSDI_PERSO_DEACT_IND_RSP:
    {
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.dact_ind_cnf.perso_feature = ((gsdi_perso_deact_feature_ind_req_T *)cmd_p)->feature;
      /* Success or fail, data_p[0] must contain the number of retries */
      switch ( ((gsdi_perso_deact_feature_ind_req_T *)cmd_p)->feature )
      {
#ifdef FEATURE_PERSO_SIM
      case GSDI_PERSO_NW:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_NW_OFFSET];
        break;
      case GSDI_PERSO_NS:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_NS_OFFSET];
        break;
      case GSDI_PERSO_SP:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_SP_OFFSET];
        break;
      case GSDI_PERSO_CP:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_CP_OFFSET];
        break;
      case GSDI_PERSO_SIM:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_SIM_OFFSET];
        break;
      case GSDI_PERSO_PROP1:
        MSG_ERROR("Not supported perso key GSDI_PERSO_PROP1", 0 , 0, 0);
        break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
      case GSDI_PERSO_RUIM_NW1:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_NW1_OFFSET];
        break;
      case GSDI_PERSO_RUIM_NW2:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_NW2_OFFSET];
        break;
      case GSDI_PERSO_RUIM_HRPD:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_HRPD_OFFSET];
        break;
      case GSDI_PERSO_RUIM_SP:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_SP_OFFSET];
        break;
      case GSDI_PERSO_RUIM_CP:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_CP_OFFSET];
        break;
      case GSDI_PERSO_RUIM_RUIM:
        msg.dact_ind_cnf.num_retries   = data_p[GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_RUIM_OFFSET];
        break;
#endif /* FEATURE_PERSO_RUIM */
      case GSDI_PERSO_NONE:
      default:
        break;

      }
    }
    break;
    case GSDI_PERSO_GET_IND_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      if ( gsdi_status == GSDI_SUCCESS )
      {
        #ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#ifdef FEATURE_PERSO_SIM
        if ( data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.nw_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.nw_ind_status = FALSE;
        }

        if ( data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ns_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ns_ind_status = FALSE;
        }

        if ( data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.sp_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.sp_ind_status = FALSE;
        }

        if ( data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.cp_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.cp_ind_status = FALSE;

        }

        if ( data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.sim_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.sim_ind_status = FALSE;
        }
        #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

        if ( data_p[GSDI_PERSO_PROP1_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
                   data_p[GSDI_PERSO_PROP1_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.prop1_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.prop1_ind_status = FALSE;
        }

        #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
        if ( data_p[GSDI_PERSO_RUIM_NW1_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_RUIM_NW1_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ruim_nw1_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ruim_nw1_ind_status = FALSE;
        }

        if ( data_p[GSDI_PERSO_RUIM_NW2_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_RUIM_NW2_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ruim_nw2_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ruim_nw2_ind_status = FALSE;
        }

        if ( data_p[GSDI_PERSO_RUIM_SP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_RUIM_SP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ruim_sp_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ruim_sp_ind_status = FALSE;
        }

        if ( data_p[GSDI_PERSO_RUIM_CP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_RUIM_CP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ruim_cp_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ruim_cp_ind_status = FALSE;

        }

        if ( data_p[GSDI_PERSO_RUIM_RUIM_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_RUIM_RUIM_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ruim_ruim_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ruim_ruim_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_RUIM_HRPD_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_ACTIVE_STATE ||
             data_p[GSDI_PERSO_RUIM_HRPD_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_AUTOLOCK_STATE)
        {
          msg.get_ind_cnf.ruim_hrpd_ind_status = TRUE;
        }
        else
        {
          msg.get_ind_cnf.ruim_hrpd_ind_status = FALSE;
        }
#endif /*FEATURE_PERSO_RUIM*/
        #else

        msg.get_ind_cnf.nw_ind_status = data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET];
        msg.get_ind_cnf.ns_ind_status = data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET];
        msg.get_ind_cnf.sp_ind_status = data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET];
        msg.get_ind_cnf.cp_ind_status = data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET];
        msg.get_ind_cnf.sim_ind_status =data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET];

        #endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
      }
      break;
    case GSDI_PERSO_SET_DATA_RSP:
      break;
    case GSDI_PERSO_GET_DATA_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.get_data_cnf.num_bytes       = returned_data_len;
      msg.get_data_cnf.data_buffer_ptr = (uint8 *)data_p;
      break;
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_OTA_DEPERSO_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.ota_cnf.num_bytes   = GSDI_PERSO_OTA_DEPERSO_RSP_LEN;
      msg.ota_cnf.data_buffer = (uint8 *)data_p;
      break;
#endif /* FEATURE_PERSO_SIM */
    case GSDI_PERSO_INTERNAL_PROC_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
      msg.int_init_cnf.feature = ((gsdi_perso_int_init_procedure_req_T*)cmd_p)->feature;
      if ( msg.int_init_cnf.feature == GSDI_PERSO_NONE )
      {
        msg.int_init_cnf.int_init_s       = (gsdi_perso_self_perso_state_enum_type)data_p[0];
        msg.int_init_cnf.int_init_feature = (gsdi_perso_enum_type)data_p[1];
      }
#else
      return_resp = FALSE;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

      break;
    case GSDI_PERSO_UNBLOCK_IND_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.unblock_ind_cnf.perso_feature = ((gsdi_perso_unblock_feature_ind_req_T *)cmd_p)->feature;
      /* Success or fail, data_p[0] must contain the number of retries */
      switch ( ((gsdi_perso_unblock_feature_ind_req_T *)cmd_p)->feature )
      {
#ifdef FEATURE_PERSO_SIM
      case GSDI_PERSO_NW:
        msg.unblock_ind_cnf.num_retries   = data_p[0];
        break;
      case GSDI_PERSO_NS:
        msg.unblock_ind_cnf.num_retries   = data_p[1];
        break;
      case GSDI_PERSO_SP:
        msg.unblock_ind_cnf.num_retries   = data_p[2];
        break;
      case GSDI_PERSO_CP:
        msg.unblock_ind_cnf.num_retries   = data_p[3];
        break;
      case GSDI_PERSO_SIM:
        msg.unblock_ind_cnf.num_retries   = data_p[4];
        break;
      case GSDI_PERSO_PROP1:
          break;
#endif
#ifdef FEATURE_PERSO_RUIM
      case GSDI_PERSO_RUIM_NW1:
        msg.unblock_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES];
        break;
      case GSDI_PERSO_RUIM_NW2:
        msg.unblock_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+1];
        break;
      case GSDI_PERSO_RUIM_HRPD:
        msg.unblock_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+2];
        break;
      case GSDI_PERSO_RUIM_SP:
        msg.unblock_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+3];
        break;
      case GSDI_PERSO_RUIM_CP:
        msg.unblock_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+4];
        break;
      case GSDI_PERSO_RUIM_RUIM:
        msg.unblock_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+5];
        break;
#endif /* FEATURE_PERSO_RUIM */
      case GSDI_PERSO_NONE:
      default:
        break;
      }
      break;
    case GSDI_PERSO_PERM_DISABLE_IND_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      msg.disable_ind_cnf.perso_feature = ((gsdi_perso_perm_disabled_feature_req_T *)cmd_p)->perso_feature;
      /* Success or fail, data_p[0] must contain the number of retries */
      switch ( ((gsdi_perso_perm_disabled_feature_req_T *)cmd_p)->perso_feature )
      {
#ifdef FEATURE_PERSO_SIM
      case GSDI_PERSO_NW:
        msg.disable_ind_cnf.num_retries   = data_p[0];
        break;
      case GSDI_PERSO_NS:
        msg.disable_ind_cnf.num_retries   = data_p[1];
        break;
      case GSDI_PERSO_SP:
        msg.disable_ind_cnf.num_retries   = data_p[2];
        break;
      case GSDI_PERSO_CP:
        msg.disable_ind_cnf.num_retries   = data_p[3];
        break;
      case GSDI_PERSO_SIM:
        msg.disable_ind_cnf.num_retries   = data_p[4];
        break;
      case GSDI_PERSO_PROP1:
        break;
#endif /* FEATURE_PERSO_SIM */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#ifdef FEATURE_PERSO_RUIM
      case GSDI_PERSO_RUIM_NW1:
        msg.disable_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES];
        break;
      case GSDI_PERSO_RUIM_NW2:
        msg.disable_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+1];
        break;
      case GSDI_PERSO_RUIM_HRPD:
        msg.disable_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+2];
        break;
      case GSDI_PERSO_RUIM_SP:
        msg.disable_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+3];
        break;
      case GSDI_PERSO_RUIM_CP:
        msg.disable_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+4];
        break;
      case GSDI_PERSO_RUIM_RUIM:
        msg.disable_ind_cnf.num_retries   = data_p[NUM_OF_PERSO_SIM_FEATURES+5];
        break;
#endif /* FEATURE_PERSO_RUIM */
#endif /*FEATURE_MMGSDI_PERSONALIZATION_ENGINE*/

      case GSDI_PERSO_NONE:
      default:
        break;
      }
      break;

    case GSDI_PERSO_GET_FEATURE_KEY_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      if ( gsdi_status == GSDI_SUCCESS )
      {
        msg.get_ind_key.feature = ((gsdi_perso_get_feature_key_req_T *)cmd_p)->feature;
        msg.get_ind_key.key_len = returned_data_len;
        memcpy(msg.get_ind_key.key, data_p, int32touint32(returned_data_len));
      }
      else
      {
        msg.get_ind_key.key_len = 0;
      }
      break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
    case GSDI_PERSO_GET_PERM_IND_RSP:
      MMGSDI_RETURN_IF_NULL(data_p);
      if ( gsdi_status == GSDI_SUCCESS )
      {
#ifdef FEATURE_PERSO_SIM
        if ( data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_nw_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_nw_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ns_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ns_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_sp_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_sp_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_cp_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_cp_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_sim_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_sim_ind_status = FALSE;
        }
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
       if ( data_p[GSDI_PERSO_RUIM_NW1_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ruim_nw1_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ruim_nw1_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_RUIM_NW2_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ruim_nw2_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ruim_nw2_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_RUIM_HRPD_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ruim_hrpd_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ruim_hrpd_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_RUIM_SP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ruim_sp_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ruim_sp_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_RUIM_CP_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ruim_cp_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ruim_cp_ind_status = FALSE;
        }
        if ( data_p[GSDI_PERSO_RUIM_RUIM_STATUS_IND_OFFSET] == (byte)GSDI_PERSO_ENG_DISABLED_STATE)
        {
          msg.get_perm_ind_cnf.perm_ruim_ruim_ind_status = TRUE;
        }
        else
        {
          msg.get_perm_ind_cnf.perm_ruim_ruim_ind_status = FALSE;
        }
#endif /* FEATURE_PERSO_RUIM */
      }
      break;

    case GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_RSP:
      if ( gsdi_status == GSDI_SUCCESS )
      {
#ifdef FEATURE_PERSO_SIM
        msg.dck_unblock_num_retries_cnf.unblock_nw_num_retries  = data_p[0];
        msg.dck_unblock_num_retries_cnf.unblock_ns_num_retries  = data_p[1];
        msg.dck_unblock_num_retries_cnf.unblock_sp_num_retries  = data_p[2];
        msg.dck_unblock_num_retries_cnf.unblock_cp_num_retries  = data_p[3];
        msg.dck_unblock_num_retries_cnf.unblock_sim_num_retries = data_p[4];
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
        msg.dck_unblock_num_retries_cnf.ruim_unblock_nw1_num_retries  = data_p[0];
        msg.dck_unblock_num_retries_cnf.ruim_unblock_nw2_num_retries  = data_p[1];
        msg.dck_unblock_num_retries_cnf.ruim_unblock_hrpd_num_retries = data_p[2];
        msg.dck_unblock_num_retries_cnf.ruim_unblock_sp_num_retries   = data_p[3];
        msg.dck_unblock_num_retries_cnf.ruim_unblock_cp_num_retries   = data_p[4];
        msg.dck_unblock_num_retries_cnf.ruim_unblock_ruim_num_retries = data_p[5];
#endif /*FEATURE_PERSO_RUIM*/
      }
      break;

    case GSDI_PERSO_GET_DCK_NUM_RETRIES_RSP:
      if ( gsdi_status == GSDI_SUCCESS )
      {
#ifdef FEATURE_PERSO_SIM
        msg.dck_num_retries_cnf.nw_num_retries  = data_p[0];
        msg.dck_num_retries_cnf.ns_num_retries  = data_p[1];
        msg.dck_num_retries_cnf.sp_num_retries  = data_p[2];
        msg.dck_num_retries_cnf.cp_num_retries  = data_p[3];
        msg.dck_num_retries_cnf.sim_num_retries = data_p[4];
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
        msg.dck_num_retries_cnf.ruim_nw1_num_retries  = data_p[0];
        msg.dck_num_retries_cnf.ruim_nw2_num_retries  = data_p[1];
        msg.dck_num_retries_cnf.ruim_hrpd_num_retries = data_p[2];
        msg.dck_num_retries_cnf.ruim_sp_num_retries  = data_p[3];
        msg.dck_num_retries_cnf.ruim_cp_num_retries  = data_p[4];
        msg.dck_num_retries_cnf.ruim_ruim_num_retries = data_p[5];
#endif /*FEATURE_PERSO_RUIM*/
       #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

       msg.dck_num_retries_cnf.prop1_num_retries = data_p[5];

       #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
      }
      break;

    #ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

    case GSDI_SIM_SECURE_STORE_RSP:
      break;

    case GSDI_SIM_SECURE_READ_RSP:
      #ifdef FEATURE_MMGSDI_DEBUG_SIM_PARTITION

      MMGSDI_RETURN_IF_NULL(data_p);
      msg.sim_secure_read_cnf.data = NULL;
      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* last byte is the feature */
        msg.sim_secure_read_cnf.data_len = returned_data_len;
        msg.sim_secure_read_cnf.data = mmgsdi_malloc(
                                                  returned_data_len);
        if (msg.sim_secure_read_cnf.data != NULL)
        {
          /* Move the data Pointer by 4 bytes to skip the HDR Information */
          memcpy(msg.sim_secure_read_cnf.data,
                 data_p,
                 returned_data_len);
        }
        else
        {
          MSG_ERROR("Unable to perform memory allocation", 0, 0, 0);
          msg.sim_secure_read_cnf.data_len = 0;
        }
      }
      else
      {
        msg.sim_secure_read_cnf.data_len = 0;
      }

      #else

      msg.sim_secure_read_cnf.data_len = 0;
      msg.sim_secure_read_cnf.data = NULL;

      #endif /* FEATURE_MMGSDI_DEBUG_SIM_PARTITION */

      break;

    case GSDI_PERSO_ROOT_KEY_WRITE_RSP:
      msg.root_key_write_cnf.root_key = NULL;  /* To be allocated if successful */
      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* last byte is the feature */
        msg.root_key_write_cnf.root_key_len = returned_data_len;
        msg.root_key_write_cnf.root_key = mmgsdi_malloc(
                                                     returned_data_len);
        if (msg.root_key_write_cnf.root_key != NULL)
        {
          memcpy(msg.root_key_write_cnf.root_key,
                 data_p,
                 returned_data_len);
        }
        else
        {
          MSG_ERROR("Unable to perform memory allocation", 0, 0, 0);
          msg.root_key_write_cnf.root_key_len = 0;
        }
      }
      else
      {
        msg.root_key_write_cnf.root_key_len = 0;
      }
      break;

    case GSDI_PERSO_ROOT_KEY_READ_RSP:
      msg.root_key_read_cnf.root_key = NULL;
      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* last byte is the feature */
        msg.root_key_read_cnf.root_key_len = returned_data_len;
        msg.root_key_read_cnf.root_key = mmgsdi_malloc(returned_data_len);
        if (msg.root_key_read_cnf.root_key != NULL)
        {
          memcpy(msg.root_key_read_cnf.root_key, data_p, returned_data_len);
        }
        else
        {
          MSG_ERROR("Unable to perform memory allocation", 0, 0, 0);
          msg.root_key_read_cnf.root_key_len = 0;
        }
      }
      else
      {
        msg.root_key_read_cnf.root_key_len = 0;
      }
      break;

    case GSDI_PERSO_GET_LIB_VERSION_RSP:
      msg.get_lib_version_cnf.lib_version_ptr = NULL;
      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* last byte is the feature */
        msg.get_lib_version_cnf.lib_version_len = returned_data_len;
        msg.get_lib_version_cnf.lib_version_ptr = mmgsdi_malloc(returned_data_len);
        if (msg.get_lib_version_cnf.lib_version_ptr != NULL)
        {
          memcpy(msg.get_lib_version_cnf.lib_version_ptr, data_p, returned_data_len);
        }
        else
        {
          MSG_ERROR("Unable to perform memory allocation", 0, 0, 0);
          msg.get_lib_version_cnf.lib_version_len = 0;
        }
      }
      else
      {
        msg.get_lib_version_cnf.lib_version_len = 0;
      }
      break;

    case GSDI_SECURE_DEVICE_INFO_RSP:
      break;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

    case GSDI_PERSO_VERIFY_PHONE_CODE_RSP:
      break;

    case GSDI_PERSO_CHANGE_PHONE_CODE_RSP:
      break;

    case GSDI_PERSO_UNBLOCK_PHONE_CODE_RSP:
      break;

    case GSDI_PERSO_SET_PHONE_CODE_RSP:
      break;

    case GSDI_PERSO_GET_PHONE_CODE_STATUS_RSP:
      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* last byte is the feature */
        if ( data_p[0] == 0x03 )
        {
           msg.get_phone_code_cnf.num_retries = 0xFF;
           msg.get_phone_code_cnf.status      = GSDI_PERSO_PHONECODE_NOT_INIT;
        }
        else
        {
           msg.get_phone_code_cnf.num_retries = data_p[1];
           msg.get_phone_code_cnf.status      = GSDI_PERSO_PHONECODE_INIT;
        }
      }
      break;

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

#ifdef FEATURE_MMGSDI_CPHS
    case GSDI_GET_CPHS_RSP:
      /* Copy the Data out of the Global Structure into the
      ** Conformation type
      */
      /* Whether Card Inserted supports CPHS */
      msg.get_cphs_info_cnf.cphs_capable = gsdi_data.cphs_info.cphs_capable;

      /* The CPHS Services Supported */
      memcpy(&msg.get_cphs_info_cnf.services,
             &gsdi_data.cphs_info.services,
             sizeof(gsdi_cphs_service_table_type));

      /* The CSP Settings */
      memcpy(&msg.get_cphs_info_cnf.csp,
             &gsdi_data.cphs_info.csp,
             sizeof(gsdi_cphs_cust_service_profile_type));
      break;
#endif /* FEATURE_MMGSDI_CPHS */

    case GSDI_GET_PRB_TABLE_RSP:
#if defined(FEATURE_GSDI_PHONEBOOK_SUPPORT) && defined(FEATURE_MMGSDI_UMTS)
#error code not present
#else
      MSG_HIGH("Command not supported in non UMTS target", 0, 0, 0);
      msg.get_pbr_table_cnf.num_rows = 0;
      break;
#endif /*#if defined(FEATURE_GSDI_PHONEBOOK_SUPPORT) && defined(FEATURE_MMGSDI_UMTS)*/

    default:
      break;
    }
  if ( return_resp == TRUE )
  {
    if (((gsdi_cmdhdr_T*)cmd_p)->gsdi_async_callback == NULL)
    {
       gsdi_return_status = gsdi_util_send_message(queue_id, (void *)&msg);
    }
    else
    {
     (((gsdi_cmdhdr_T*)cmd_p)->gsdi_async_callback)(&msg);
    }
  }
  else
  {
    MSG_HIGH("RSP: 0x%x Not Returned for REQ",*(resp_type_p),0,0);
  }

  /* --------------------------------------------------------------------------
     FREE UP ANY MEMORY ALLOCATED WHILE BUILDING THE GSDI CNF
     The assumption is, the memory needed in the callback
     was already copied, used, etc. while in the callback function.
     Once at this point, the data is deemed to be no longer needed and can
     thus be freed.
     ------------------------------------------------------------------------*/
  switch ( *resp_type_p )
  {
    case GSDI_GET_IMAGE_RSP:
      MMGSDIUTIL_TMC_MEM_FREE(msg.image_cnf.data_p);
      break;

    case GSDI_SIM_ACCESS_RSP:
      MMGSDIUTIL_TMC_MEM_FREE(msg.sim_access_cnf.data_p);
      break;

    case GSDI_GET_ECC_RSP:
      loop = 0;
      while ( loop < msg.ecc_cnf.emergency_call_code.num_of_rec )
      {
        /* Free Up each ECC Code */
        MMGSDIUTIL_TMC_MEM_FREE(msg.ecc_cnf.emergency_call_code.ecc_p[loop]);
        loop++;
      }
      break;

    case GSDI_GET_PRB_TABLE_RSP:
      if (  msg.get_pbr_table_cnf.num_rows > 0 &&
            msg.get_pbr_table_cnf.message_header.gsdi_status == GSDI_SUCCESS )
      {
        loop = 0;

        while ( loop < msg.get_pbr_table_cnf.num_rows )
        {
          /* Free Up each PBR Row */
          MMGSDIUTIL_TMC_MEM_FREE(msg.get_pbr_table_cnf.pbr_tbl_p[loop].path_p);
          loop++;
        }

        /* Now free up the entire Table allocated */
        MMGSDIUTIL_TMC_MEM_FREE(msg.get_pbr_table_cnf.pbr_tbl_p);
      }
      break;

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

    case GSDI_PERSO_ROOT_KEY_WRITE_RSP:
      MMGSDIUTIL_TMC_MEM_FREE(msg.root_key_write_cnf.root_key );
      break;

    case GSDI_PERSO_ROOT_KEY_READ_RSP:
      MMGSDIUTIL_TMC_MEM_FREE(msg.root_key_read_cnf.root_key );
      break;

    case GSDI_PERSO_GET_LIB_VERSION_RSP:
      MMGSDIUTIL_TMC_MEM_FREE(msg.get_lib_version_cnf.lib_version_ptr );
      break;

    case GSDI_SIM_SECURE_READ_RSP:
      MMGSDIUTIL_TMC_MEM_FREE(msg.sim_secure_read_cnf.data );
      break;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

#endif /* FEATURE_MMGSDI_PERONALIZATION_ENGINE */
    case GSDI_GET_ALL_PIN_INFO_RSP:
      MMGSDIUTIL_TMC_MEM_FREE( msg.all_pin_info_cnf.pin_info_items );
      break;
    default:
      /* Do Nothing */
      break;
  }

  return gsdi_return_status;
} /* gsdi_util_send_gsdi_cnf */ /*lint !e715 */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UTIL_SEND_MMI_SIM_IND_EXT

DESCRIPTION
  Will update the GSDI DATA Variables based on the event generated.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gsdi_returns_T gsdi_util_send_mmi_sim_ind_ext (
  gsdi_sim_events_T  sim_event
)
{

  gsdi_returns_T    gsdi_status = GSDI_SUCCESS;
  boolean           slot_1_event = FALSE;
  boolean           slot_2_event = FALSE;

  /* Determine whether or not the Task has
  ** received the task stop sig.  If it did,
  ** MUTE the task (i.e. do not send any more
  ** events)
  */
  if ( gsdi_data.task_stopped == TRUE )
  {
    return GSDI_SUCCESS;
  }

  /* Determine whether or not to allow the */
  /* GSDI Libraries to be used             */
  switch ( sim_event )
  {
    case GSDI_CARD_ERROR:
    case GSDI_CARD_ERR_POLL_ERROR:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE:
      /* Don't service any more requests */
      gsdi_data.sim_present_and_ready = FALSE;
      gsdi_data.gsdi_state = GSDI_CARD_ERROR_S;
      slot_1_event = TRUE;
      break;

    case GSDI_SIM_INIT_COMPLETED:
      /* OK to Service Requests */
      gsdi_data.sim_present_and_ready = TRUE;
      gsdi_data.gsdi_state = GSDI_SIM_INIT_COMPLETED_S;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
      slot_1_event = TRUE;
      break;

    case GSDI_SIM_INSERTED:
      gsdi_data.gsdi_state = GSDI_SIM_INSERTED_S;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
      slot_1_event = TRUE;
      break;

    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
      /* OK TO Service Requests for SLOT 1 */
      gsdi_data.sim_present_and_ready = TRUE;
      gsdi_data.gsdi_state = GSDI_SIM_INIT_COMPLETED_NO_PROV_S;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
      slot_1_event = TRUE;
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SIM_INSERTED_2:
      gsdi_data.gsdi_state_2 = GSDI_SIM_INSERTED_S;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

      slot_2_event = TRUE;
      break;

    case GSDI_CARD_ERROR_2:
    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH_2:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR_2:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2:
    case GSDI_CARD_ERR_POLL_ERROR_2:
      /* Don't service any more requests for SLOT 2*/
      gsdi_data.sim_present_and_ready_2 = FALSE;
      gsdi_data.gsdi_state_2 = GSDI_CARD_ERROR_S;
      slot_2_event = TRUE;
      break;

    case GSDI_SIM_INIT_COMPLETED_NO_PROV_2:
      gsdi_data.sim_present_and_ready_2 = TRUE;
      gsdi_data.gsdi_state_2 = GSDI_SIM_INIT_COMPLETED_NO_PROV_S;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
      slot_2_event = TRUE;
      break;

    case GSDI_SIM_INIT_COMPLETED_2:
      /* OK to Service Requests for SLOT 2*/
      gsdi_data.sim_present_and_ready_2 = TRUE;
      gsdi_data.gsdi_state_2 = GSDI_SIM_INIT_COMPLETED_S;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
      slot_2_event = TRUE;
      break;

#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    default:
        /* Do nothing for now */
        break;
    }

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
    /* First task who has a Call Back Pointer in this array */
    /* will always be the UI.                               */
    gsdi_util_send_sim_events_to_reg_tasks(sim_event, slot_1_event, slot_2_event);
#else

    if (gsdi_data.gsdi_callback_info.gsdi_callback_ptr == NULL)
    {
      ERR("MMI_SIM_IND ui ptr NULL",0,0,0);
      return GSDI_ERROR;
    }
    (gsdi_data.gsdi_callback_info.gsdi_callback_ptr)(sim_event);
    if (slot_1_event)
    {
      gsdi_data.gsdi_callback_info.event_sent = TRUE;
    }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (slot_2_event)
    {
      gsdi_data.gsdi_callback_info.event2_sent = TRUE;
    }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT */

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
    gsdidiag_event_notification(sim_event);
#endif /*FEATURE_MMGSDI_TEST_AUTOMATION*/

  return gsdi_status;
} /* gsdi_util_send_mmi_sim_ind_ext */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UTIL_SEND_MMI_SIM_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_util_send_mmi_sim_ind (
  gsdi_sim_events_T  sim_event
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_event_data_type  *evt_info_ptr  = NULL;

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    evt_info_ptr,
    sizeof(mmgsdi_event_data_type),
    mmgsdi_status);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_event(FALSE, evt_info_ptr, &sim_event);

    if (mmgsdi_status != MMGSDI_SUCCESS)
      MSG_ERROR("Unable to build MMGSDI events", 0, 0, 0);
    else
      mmgsdi_status = mmgsdi_util_notify_client_evt(evt_info_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
    return GSDI_SUCCESS;
  else
    return GSDI_ERROR;


} /* gsdi_util_send_mmi_sim_ind */

/*===========================================================================
FUNCTION GSDI_UTIL_MAP_PIN_STATUS_TO_SIM_EVENT

DESCRIPTION
  Maps the pin status to sim event status

DEPENDENCIES
  None

RETURN VALUE
  gsdi_sim_events_T.
===========================================================================*/
static gsdi_sim_events_T gsdi_util_map_pin_status_to_sim_event(gsdi_pin_status_T pin_status)
{
  gsdi_sim_events_T sim_event = GSDI_NO_EVENT;

  switch(pin_status)
  {
  case GSDI_PIN_DISABLED:
    sim_event = GSDI_PIN1_DISABLED;
    break;
  case GSDI_PIN_ENABLED:
    sim_event = GSDI_PIN1_ENABLED;
    break;
  case GSDI_PIN_BLOCKED:
    sim_event = GSDI_PIN1_BLOCKED;
    break;
  case GSDI_PIN_PERM_BLOCKED:
    sim_event = GSDI_PIN1_PERM_BLOCKED;
    break;
  case GSDI_PIN_NOT_INITIALISED:
  case GSDI_MAX_PIN_STATUS:
    sim_event = GSDI_NO_EVENT;
    break;
  default:
    sim_event = GSDI_NO_EVENT;
    MSG_ERROR("Should never come here, all the cases are handled", 0,0,0);
    break;
  }
  return sim_event;
}

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
/*===========================================================================
FUNCTION GSDI_UTIL_SEND_SIM_EVENTS_TO_REG_TASKS

DESCRIPTION
   This function will be used to send sim events to all
   tasks who have callback pointers registered with the
   GSDI

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gsdi_util_send_sim_events_to_reg_tasks (
    gsdi_sim_events_T  sim_event,
    boolean            slot1_event,
    boolean            slot2_event
)
{
  byte           cb_ptr_index = 0;

  if ( sim_event != GSDI_NO_EVENT )
  {
      /* Send out Events */
      for ( cb_ptr_index = 0; cb_ptr_index < GSDI_MAX_REGISTERED_TASK_CBS; cb_ptr_index++)
      {

          if ( gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr != NULL)
          {
              /* Call the Task's SIM EVENT Callback Function */
              MSG_LOW("Calling Call Back Ptr",0,0,0);
              (gsdi_data.gsdi_callback_info[cb_ptr_index].gsdi_callback_ptr)(sim_event);
              if (slot1_event)
              {
                gsdi_data.gsdi_callback_info[cb_ptr_index].event_sent = TRUE;
              }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
              if (slot2_event)
              {
                gsdi_data.gsdi_callback_info[cb_ptr_index].event2_sent = TRUE;
              }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

          }
          else
          {
              /* Rest of the indexes have Callback Ptrs set to NULL */
              MSG_LOW("Breaking Loop, cb_ptr_index = %x",cb_ptr_index,0,0);
              break;
          }
      }
  }

} /* gsdi_util_send_sim_events_to_reg_tasks */ /*lint !e715 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
#endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT */


#if defined (FEATURE_GSDI_PHONEBOOK_SUPPORT) && defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*FEATURE_GSDI_PHONEBOOK_SUPPORT && MMGSDI_UMTS */



/*===========================================================================
FUNCTION GSDI_UTIL_POPULATE_SIM_CAPABILITY

DESCRIPTION
  populate the synchronized user sim capability pointer with gsdi_data

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_utils_populate_sim_capability(gsdi_slot_id_type slot,
                                        sim_capabilities_T * sim_capabilities)
{
   sim_capabilities_T *gsdi_sim_cap_p = NULL_PTR;

   switch (slot)
   {
   case GSDI_SLOT_1:
      gsdi_sim_cap_p = &(gsdi_data.sim_capability);
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
   case GSDI_SLOT_2:
      gsdi_sim_cap_p = &(gsdi_data.sim_capability_2);
      break;

#endif /* FEATURE_MMGSDI_DUAL_SLOT */

   default:
      MSG_ERROR("Unkown slot", 0, 0, 0);
      /* default to SLOT 1 */
      gsdi_sim_cap_p = &(gsdi_data.sim_capability);
      break;
   }

   if(sim_capabilities == NULL)
     return;

   sim_capabilities->fdn_enabled = gsdi_sim_cap_p->fdn_enabled;
   sim_capabilities->bdn_enabled = gsdi_sim_cap_p->bdn_enabled;
   sim_capabilities->acl_enabled = gsdi_sim_cap_p->acl_enabled;
   sim_capabilities->imsi_invalidated =
                                     gsdi_sim_cap_p->imsi_invalidated;

   /* Copy all SIM Phonebook Related Information */
   sim_capabilities->sim_phone_book.multiple_pbr_entries =
                                     gsdi_sim_cap_p->sim_phone_book.multiple_pbr_entries;
   sim_capabilities->sim_phone_book.present =
                                     gsdi_sim_cap_p->sim_phone_book.present;

   /* Copy all USIM Local Phonebook Related Information */
   sim_capabilities->usim_local_phone_book.multiple_pbr_entries =
                                          gsdi_sim_cap_p->usim_local_phone_book.multiple_pbr_entries;
   sim_capabilities->usim_local_phone_book.present =
                                          gsdi_sim_cap_p->usim_local_phone_book.present;

   /* Copy all USIM Global Phonebook Related Information */
   sim_capabilities->usim_global_phone_book.multiple_pbr_entries =
                                          gsdi_sim_cap_p->usim_global_phone_book.multiple_pbr_entries;
   sim_capabilities->usim_global_phone_book.present =
                                          gsdi_sim_cap_p->usim_global_phone_book.present;
} /*gsdi_utils_populate_sim_capability*/

/* <EJECT> */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*#ifdef FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_CDMA)
/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UTIL_EXTRACT_FILE_ATTR

DESCRIPTION:
  This function is used to decode and map the Header information from the SIM
  format as define in GSM 11.11 as follows:

  byte 01:    rfu
  byte 02:    rfu
  byte 03:    file size MSB
  byte 04:    file size LSB
  byte 05:    file id MSB
  byte 06:    file id LSB
  byte 07:    type of file
              00 --> RFU
              01 --> MF
              02 --> DF
              04 --> EF
  byte 08:    RFU for Linear and Transparent Files
              bit7 = 1 Indicates Cyclic Increase allowed
  byte 09:    B8-B5 Read/Seek Permissions
              B4-B1 Update Permissions
  byte 10:    B8-B5 Increase Permissions
              B4-B1 RFU
  byte 11:    B8-B5 RehabilitatePermissions
              B4-B1 Invalidate Permissions
  byte 12:    File Status
              B8-B4 RFU
              B3 = 0 Not Readable/Writeable when invalidate
              B3 = 1 Readable/Writeable when invalidated
              B2 RFU
              B1 = 0 (Invalidated)
              B1 = 1 (Not Invalideated - Valid)
  byte 13:    Length of the following Information (Byte 14 to end]
  byte 14:    Structure of EF
              00 --> TRANSPARENT
              01 --> LINEAR FIXED
              03 --> CYCLIC
  byte 15:    Length of Record.  For cyclic and linear fixed, this denotes
              the length of the record.  For a transparent file it is 0x00.
  byte 16+:   (optional when defined)


  to the gsdi format so that it can be eventually placed in the
  gsdi_file_attributes_T format.

  byte 00:  file size (msb)
  byte 01:  file size
  byte 02:  file size
  byte 03:  file size (lsb)
  byte 04:  file id
  byte 05:  file id
  byte 06:  file type
  byte 07:  cyclic increase allowed
  byte 08:  file protection (READ)
  byte 09:  file protection (WRITE)
  byte 10:  file protection (INCREASE)
  byte 11:  file protection (INVALIDATE)
  byte 12:  file protection (REHABILITATE)
  byte 13:  file valid (not invalidated)
  byte 14:  file structure
  byte 15:  rec len
  byte 16:  RESERVED (FOR USIM - Number of Records)
  byte 17:  file is updateable/readable when invalidated


DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gsdi_util_extract_file_attr (
  gsdi_slot_id_type slot,
  byte *            sim_format_attr,
  byte              sim_format_attr_len,
  byte *            gsdi_format_attr
)
{
    uint8 *card_apps_p = NULL;
    byte phase = 0;

    if(NULL == gsdi_format_attr || NULL == sim_format_attr)
    {
      MSG_ERROR("gsdi_util_extract_file_attr NULL pointer",0,0,0);
      return;
    }
    memset( gsdi_format_attr, 0xFF, sizeof(gsdi_file_attributes_T) );

    gsdi_format_attr[GSDI_ATTR_FILE_SIZE_MSB_OFFSET]   = 0x00;
    gsdi_format_attr[GSDI_ATTR_FILE_SIZE_MSB_OFFSET+1] = 0x00;
    gsdi_format_attr[GSDI_ATTR_FILE_SIZE_MSB_OFFSET+2] = sim_format_attr[2];
    gsdi_format_attr[GSDI_ATTR_FILE_SIZE_LSB_OFFSET]   = sim_format_attr[3];
    gsdi_format_attr[GSDI_ATTR_FILE_ID_MSB_OFFSET]     = sim_format_attr[4];
    gsdi_format_attr[GSDI_ATTR_FILE_ID_LSB_OFFSET]     = sim_format_attr[5];
    gsdi_format_attr[GSDI_ATTR_FILE_TYPE_OFFSET]       = sim_format_attr[6];
    if ( sim_format_attr[13] == (int)GSDI_CYCLIC_FILE )
    {
      gsdi_format_attr[GSDI_ATTR_INCREASE_OFFSET]    = ( sim_format_attr[7]
                                                        & 0x40 ) / 0x40;
    }
    else
    {
      gsdi_format_attr[GSDI_ATTR_INCREASE_OFFSET]    = FALSE;
    }
    gsdi_format_attr[GSDI_ATTR_PROT_READ_OFFSET]       = sim_format_attr[8]
                                                         / 0x10;
    gsdi_format_attr[GSDI_ATTR_PROT_WRITE_OFFSET]      = sim_format_attr[8]
                                                         & 0x0F;
    gsdi_format_attr[GSDI_ATTR_PROT_INCREASE_OFFSET]   = sim_format_attr[9]
                                                         / 0x10;
    gsdi_format_attr[GSDI_ATTR_PROT_INVAL_OFFSET]      = sim_format_attr[10]
                                                         & 0x0F;
    gsdi_format_attr[GSDI_ATTR_PROT_REHAB_OFFSET]      = sim_format_attr[10]
                                                         / 0x10;
    switch (slot)
    {
      case GSDI_SLOT_1:
        phase = gsdi_data.sim_phase;
        card_apps_p = &gsdi_data.card_applications.apps_enabled;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
        phase       = gsdi_data.sim_phase_2;
        card_apps_p = &gsdi_data.card_applications.apps2_enabled;
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/
      default:
        MSG_ERROR("Unknown slot %d ", slot, 0, 0);
        return;
    }

    /* ------------------------------------------------------------------------
       For an RUIM Only Card, (i.e. CDMA DF with no GSM DF) we must force the
       phase to be a Phase 2 since this is the phase these cards are modeled
       after
       ----------------------------------------------------------------------*/
    if ((!(*card_apps_p & GSDI_GSM_SIM_MASK )) &&
          (*card_apps_p & GSDI_RUIM_APP_MASK))
    {
      phase = 2;
    }

    /* ------------------------------------------------------------------------
       For a P2 or P2+, the appropriate offsets can now be checked
       ----------------------------------------------------------------------*/
    if ( phase >= 2 )
    {
      gsdi_format_attr[GSDI_ATTR_VALID_FLAG_OFFSET]      = sim_format_attr[11]
                                                           & 0x01;
      /* ----------------------------------------------------------------------
         Check the access condition for invalidation using the following
     table:
           File status
             b8 b7 b6 b5 b4 b3 b2 b1
             b1=0: invalidated;
             b1=1: not invalidated
             b3=0: not readable or updatable when invalidated
             b3=1: readable and updatable when invalidated
     --------------------------------------------------------------------*/
      if ((sim_format_attr[11] & 0x04) == 0x04)
      {
          gsdi_format_attr[GSDI_ATTR_RW_WHEN_INV_DACT_OFFSET] = TRUE;
      }
      else
      {
          gsdi_format_attr[GSDI_ATTR_RW_WHEN_INV_DACT_OFFSET] = FALSE;
      }
    }
    else
    {
      /* To handle a Phase 1 or lower GSM SIM Card       */
      gsdi_format_attr[GSDI_ATTR_VALID_FLAG_OFFSET]      = FALSE;
      gsdi_format_attr[GSDI_ATTR_RW_WHEN_INV_DACT_OFFSET] = FALSE;
    }

    gsdi_format_attr[GSDI_ATTR_FILE_STRUCT_OFFSET]     = sim_format_attr[13];
    gsdi_format_attr[GSDI_ATTR_REC_LEN_OFFSET]         = sim_format_attr[14];

    /* initialize the number of record to 0 */
    gsdi_format_attr[GSDI_ATTR_REC_NUM_OFFSET]         = 0;

} /* gsdi_util_extract_file_attr */ /*lint !e715 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
#endif /*#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_CDMA)*/


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/


/* ----------------------------------------------------------------------------
   FUNCTION  GSDI_UTIL_SEND_PIN_STATUS

   DESCRIPTION:
     This function will send the pin status event

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     gsdi_returns_T

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
gsdi_returns_T gsdi_util_send_pin_status(void)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  gsdi_sim_events_T       sim_event     = GSDI_NO_EVENT;
  mmgsdi_event_data_type *evt_info_ptr  = NULL;
  gsdi_pin_status_T      pin_status     = GSDI_PIN_NOT_INITIALISED;
  gsdi_pin_info_item_T   pin_info;

  pin_info.pin_id = GSDI_PIN1;
  if(mmgsdi_util_convert_to_gsdi_pin_info(&pin_info) != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Could not get pin status from MMGSDI",0,0,0);
    return GSDI_ERROR;
  }

  sim_event = gsdi_util_map_pin_status_to_sim_event(pin_status);
  if(sim_event == GSDI_NO_EVENT)
    return GSDI_ERROR;

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    evt_info_ptr,
    sizeof(mmgsdi_event_data_type),
    mmgsdi_status);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_event(FALSE, evt_info_ptr, &sim_event);
    if (mmgsdi_status != MMGSDI_SUCCESS)
      MSG_ERROR("Unable to build MMGSDI events", 0, 0, 0);
    else
      mmgsdi_status = mmgsdi_util_notify_client_evt(evt_info_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
    return GSDI_SUCCESS;
  else
    return GSDI_ERROR;

} /* gsdi_util_send_pin_status */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */


/*===========================================================================

FUNCTION GSDI_RUIM_GET_ECC

DESCRIPTION
  This function will be used to retrieve the Emergency Call Codes retrieved
    by the GSDI during the GSDI & RUIM Initialization Procedures.

    This function will populate a gsdi_ecc_data_T structure passed in
    as a parameter.

DEPENDENCIES
   Dependency on Global Variable gsdi_ef_ecc_data_T ruim_ecc_data.  This
   structure must before this function can be used.

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
gsdi_returns_T gsdi_ruim_get_ecc(gsdi_ef_ecc_data_T * ruim_ecc_structure)
{
#ifdef FEATURE_MMGSDI_CDMA
    return gsdi2_ruim_get_ecc(GSDI_SLOT_1, ruim_ecc_structure);
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA */
} /* gsdi_ruim_get_ecc */ /* lint !e715 */


/*===========================================================================
FUNCTION GSDI2_RUIM_GET_ECC

DESCRIPTION
  This function will be used to get the Emergency Call Codes retrieved
  by the GSDI during the GSDI & RUIM Initialization Procedures.

  This function will populate a gsdi_ecc_data_T structure base on the slot
  information provided

    Parameters
    slot [ INPUT ] indicates which slot the ecc is to be obtained from.
    ruim_ecc [ INPUt/OUTPUT ] pointer to the structure where the
                              RUIM ecc information is going to be set to.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
    GSDI_SUCCESS
    GSDI_NOT_FOUND
    GSDI_ERROR

SIDE EFFECTS
  None

===========================================================================*/
gsdi_returns_T gsdi2_ruim_get_ecc(
    gsdi_slot_id_type slot,
    gsdi_ef_ecc_data_T * ruim_ecc )
{
#ifdef FEATURE_MMGSDI_CDMA
    gsdi_ef_ecc_data_T * psim_ecc;

    /* Check for NULL Pointer */
    if (ruim_ecc == NULL)
    {
        ERR("ERROR:  Got Null Pointer",0,0,0);
        return GSDI_ERROR;
    }

    switch (slot)
    {
    case GSDI_SLOT_1:
        psim_ecc = &ruim_ecc_data;
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
        psim_ecc = &ruim2_ecc_data;
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

    default:
        MSG_ERROR ("Unknown slot %d", slot, 0, 0);
        return GSDI_ERROR;
    }

    if (psim_ecc->ef_exists != TRUE)
    {
        MSG_ERROR("ERROR:  ECC FILE Not Read",0,0,0);
        return GSDI_NOT_FOUND;
    }
    else if (psim_ecc->len > 15)
    {
        MSG_ERROR("ERROR:  Len exceeds GSDI_MAX_ECC_LEN",0,0,0);
        return GSDI_NOT_SUPPORTED;
    }
    else
    {
        ruim_ecc->len       = psim_ecc->len;
        ruim_ecc->ef_exists = psim_ecc->ef_exists;

        memcpy( ruim_ecc->data,
                psim_ecc->data,
                psim_ecc->len );
    }

    return GSDI_SUCCESS;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA */
} /* gsdi2_ruim_get_ecc */ /* lint !e715 */


#ifdef FEATURE_MMGSDI_CDMA
/*===========================================================================
FUNCTION GSDI_UTIL_DELETE_RUIM_CACHE

DESCRIPTION:

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
void gsdi_util_delete_ruim_cache (
  gsdi_slot_id_type slot
)
{
    word file_num = 0;
    boolean pref_slot = TRUE;
    gsdi_slot_id_type new_slot;

    pref_slot = gsdi_util_is_slot_pref( slot,
                                        GSDI_CDMA,
                                        &new_slot);

    for ( file_num = 0; file_num < GSDI_NUM_FILES_IN_RUIM_CACHE; file_num++ )
    {
      if (bool_equal(gsdi_ruim_cache[file_num].pref_slot, pref_slot)) {
        gsdi_ruim_cache[file_num].init = FALSE;
        memset(gsdi_ruim_cache[file_num].body,
               0x00,
               GSDI_CACHE_MAX_FILE_LEN);
        gsdi_ruim_cache[file_num].in_synch = FALSE;
        if (gsdi_ruim_cache[file_num].add_data != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE(gsdi_ruim_cache[file_num].add_data);
        }
        MSG_LOW("RUIM CACHE DELETE %x",gsdi_ruim_cache[file_num].filename,0,0);
      }
    }
} /* gsdi_util_delete_ruim_cache */

/*===========================================================================
FUNCTION GSDI_UTIL_READ_RUIM_CACHE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_util_read_ruim_cache (
  uim_items_enum_type   sim_filename,
  boolean               pref_slot,
  byte                  requested_data_len,
  byte                  data_offset,
  byte *                actual_data_len_p,
  byte *                data_p
)
{
    boolean         file_found = FALSE;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

    /* Find requested file in ruim cache */
    for ( file_num = 0; file_num < GSDI_NUM_FILES_IN_RUIM_CACHE; file_num++ ) {
        if ( gsdi_ruim_cache[file_num].filename == sim_filename ) {
            if ( bool_equal(gsdi_ruim_cache[file_num].pref_slot, pref_slot) ) {
                file_found = TRUE;
                break;
            }
        }
    }

    if ( file_num >= GSDI_NUM_FILES_IN_RUIM_CACHE)
    {
        return GSDI_NOT_SUPPORTED;
    }


    /* Read file */
    if ( file_found == FALSE )
    {
        gsdi_status = GSDI_NOT_SUPPORTED;
    }
    else if ( gsdi_ruim_cache[file_num].init == FALSE )
    {
        gsdi_status = GSDI_NOT_INIT;
    }
    else if ( data_offset + requested_data_len >
                gsdi_ruim_cache[file_num].rec_len )
    {
        gsdi_status = GSDI_INCORRECT_PARAMS;
    } else
    {
        if ( requested_data_len == 0 ) {
            actual_data_len_p[0] = gsdi_ruim_cache[file_num].rec_len;
        } else {
            actual_data_len_p[0] = requested_data_len;
        }
        (void) memcpy( data_p, (gsdi_ruim_cache[file_num].body) + data_offset,
                actual_data_len_p[0] );
        gsdi_status = GSDI_SUCCESS;
    }

    return gsdi_status;

} /* gsdi_util_read_ruim_cache */

/*===========================================================================
FUNCTION GSDI_UTIL_GET_RUIM_EF_CACHE_LENGTH

DESCRIPTION
    This function is called to retrieve the length of an EF stored in GSDI
    USIM Cache.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - EF from cache length found and copied
                   GSDI_NOT_FOUND - EF not found in cache
                   GSDI_NOT_INIT  - EF item was not read during init procs.
===========================================================================*/
gsdi_returns_T gsdi_util_get_ruim_ef_cache_length(
    /* EF in cache */
    uim_items_enum_type     sim_filename,
    boolean                 pref_slot,
    /* Byte Pointer to store length in */
    int*                    length_p
)
{
    boolean         file_found = FALSE;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

    /* Find requested file in ruim cache */
    for ( file_num = 0; file_num < GSDI_NUM_FILES_IN_RUIM_CACHE; file_num++ ) {
        if ( gsdi_ruim_cache[file_num].filename == sim_filename ) {
            if ( bool_equal(gsdi_ruim_cache[file_num].pref_slot, pref_slot) ) {
                file_found = TRUE;
                break;
            }
        }
    }

    if (file_num >= GSDI_NUM_FILES_IN_RUIM_CACHE)
    {
      return GSDI_NOT_FOUND;
    }

    /* Was the file found in Cache */
    if ( file_found != TRUE )
    {
        MSG_ERROR("RUIM Cache Get EF Length Not Found",0,0,0);
        gsdi_status = GSDI_NOT_FOUND;
    }

    /* Check to see if Cache item was "init" */
    if ( gsdi_status == GSDI_SUCCESS )
    {
        if ( gsdi_ruim_cache[file_num].init == FALSE )
        {
            MSG_ERROR("RUIM Cache Get EF Not Init",0,0,0);
            gsdi_status = GSDI_NOT_INIT;
        }
        else
        {
            /* Copy Length */
            length_p[0] = (int)gsdi_ruim_cache[file_num].total_len;
            MSG_HIGH("EF %x Length %x",gsdi_ruim_cache[file_num].filename,
                                       gsdi_ruim_cache[file_num].total_len,
                                       0);

        }
    }

    return gsdi_status;
} /* gsdi_util_get_ruim_ef_cache_length */

/*===========================================================================
FUNCTION gsdi_util_is_file_ok_in_cst

DESCRIPTION
    This function will be used to check the EF in the request against
    the CDMA Service Table.

DEPENDENCIES
  Must be used in conjuction with an RUIM or a GSM/RUIM Dualmode SIM

RETURN VALUE
  boolean:   1) TRUE - OK to service request
             2) FALSE - Reject request.

SIDE EFFECTS
  None
===========================================================================*/
boolean gsdi_util_is_file_ok_in_cst(
   uim_items_enum_type  sim_filename,
   boolean              pref_slot)
{
    boolean         ok = FALSE;
    byte            cst[GSDI_CACHE_CDMA_CST_LEN];
    gsdi_returns_T  gsdi_status = GSDI_NOT_SUPPORTED;
    byte            offset=0x00;
    byte            mask=0x00;
    byte            actual_data_len = 0;
    uim_items_enum_type read_cst_item;

    /* Is the specified file allocated & activated in the [RUIM-CACHE] CST ? */
    read_cst_item = UIM_CDMA_CDMA_SVC_TABLE;
    gsdi_status = gsdi_util_read_ruim_cache(read_cst_item,
                                            pref_slot,
                                            0,
                                            0,
                                            &actual_data_len,
                                            cst );

    if ( gsdi_status == GSDI_SUCCESS )
    {
        switch ( sim_filename )
        {
            /* Need to add services */

        default:
            ok = TRUE;
            break;
        }

        /* Verify with SST */
        if ( ok == FALSE )
        {
            MSG_LOW("OFFSET = %x",offset,0,0);
            MSG_LOW("CHECKING:  %x & %x for %x",cst[offset],mask,sim_filename);
            /* Check Mask and Offset to determine */
            /* if service is Allocated            */
            if ( ( cst[offset] & mask ) == mask )
            {
                /* Service Allocated...allow */
                /* request to completed      */
                MSG_HIGH("SERVICE IS AVAILABLE %x, %x, %x", cst[offset],
                                                           mask,
                                                           cst[offset]&mask);
                ok = TRUE;
            }
            else
            {
                /* Service is not Allocated  */
                /* do not allow request to   */
                /* be completed              */
                MSG_HIGH("SERVICE NOT AVAILABLE %x, %x, %x",cst[offset],
                                                            mask,
                                                            cst[offset]&mask);
                ok = FALSE;
            }
        }
    }
    else
    {
        /* SST may not be init */
        ok = TRUE;
    }
    return ok;

} /* gsdi_util_is_file_ok_in_cst */


/*===========================================================================
FUNCTION GSDI_UTIL_WRITE_RUIM_CACHE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_util_write_ruim_cache (
  uim_items_enum_type       sim_filename,
  boolean                   pref_slot,
  byte                      data_len,
  byte                      data_offset,
  byte *                    data_p,
  gsdi_data_from_enum_type  data_fr_card
)
{
    boolean         file_found = FALSE;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    boolean         init_completed;

    init_completed = gsdi_data.init_completed;

    /* Find requested file in ruim cache */
    for ( file_num = 0; file_num < GSDI_NUM_FILES_IN_RUIM_CACHE; file_num++ )
    {
        if ( gsdi_ruim_cache[file_num].filename == sim_filename )
        {
            if ( bool_equal(gsdi_ruim_cache[file_num].pref_slot, pref_slot) ) {
                file_found = TRUE;
                break;
            }
        }
    }

    if (file_num >= GSDI_NUM_FILES_IN_RUIM_CACHE)
    {
      return GSDI_NOT_SUPPORTED;
    }

    /* Handle the special case of updating the */
    /* data len in cache during init when GSDI */
    /* is caching information                  */
    /* During init, GSDI caches whole records. */
    /* partial records or offsets of records   */
    /* are not cached during init              */
    if ( ( gsdi_data.init_completed   == FALSE
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        || gsdi_data.init_completed_2 == FALSE
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
         ) && ( file_found            == TRUE  ) )
    {
        /* Need to first update the Length of the record */
        /* Is this the first time reading the file ?     */
        if ( gsdi_ruim_cache[file_num].init == FALSE )
        {
            /* OK to update to length returned */
            /* Need to avoid updating to length 0 */
            if ( data_len != 0 )
            {
                gsdi_ruim_cache[file_num].rec_len = data_len;
                gsdi_ruim_cache[file_num].total_len = data_len;
                gsdi_ruim_cache[file_num].add_data = NULL;
                gsdi_status = GSDI_SUCCESS;
            }
        }

    }

    /* Was file found in Cache ? */
    if ( !file_found )
    {
        /* Item is not in RUIM Cache */
        gsdi_status = GSDI_NOT_SUPPORTED;
    }
    /* Is the requested offset and length of offset     */
    /* exceeding the amount of space allocated for rec? */
    else if ( data_offset + data_len > gsdi_ruim_cache[file_num].rec_len )
    {
        gsdi_status = GSDI_INCORRECT_PARAMS;
    }
    /* OK to update cache at this point */
    else
    {
        /* Data len of 0 indicates the max number */
        /* of bytes are to be written             */
        if ( data_len == 0 )
        {
             data_len = gsdi_ruim_cache[file_num].rec_len;
        }

        /* Copy contents to be written into cache */
        memcpy( (gsdi_ruim_cache[file_num].body) + data_offset, data_p, data_len );
        gsdi_status = GSDI_SUCCESS;


        if ( ( data_len == gsdi_ruim_cache[file_num].rec_len ) && ( init_completed ) )
        {
            gsdi_ruim_cache[file_num].init = TRUE;
        }
        else
        {
            /* Don't initialize the cache */
            /* item due to a data len     */
            /* problem during GSDI init   */
            gsdi_ruim_cache[file_num].init = TRUE;

        }
        switch(data_fr_card)
        {
        case GSDI_DATA_FROM_CARD:
          gsdi_ruim_cache[file_num].in_synch = TRUE;
          break;
        case GSDI_DATA_FROM_CLIENT:
        default:
          gsdi_ruim_cache[file_num].in_synch = FALSE;
          break;
        }
    }

    return gsdi_status;

} /* gsdi_util_write_ruim_cache */
#endif /* FEATURE_MMGSDI_CDMA */


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*#ifdef FEATURE_MMGSDI_UMTS*/

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*#ifdef FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_CDMA
/*===========================================================================
FUNCTION GSDI_UTIL_RUIM_SERVICE_CHECK

DESCRIPTION
  This function check whether the service is available in the cdma service
  table.  It checks if it is availabe in the sst.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_svr_rsp_type
===========================================================================*/
gsdi_returns_T gsdi_util_ruim_service_check(
    gsdi_slot_id_type               slot,
    gsdi_svr_rsp_type               *svr_rsp,
    gsdi_services_type              service
)
{
  gsdi_returns_T                  gsdi_status = GSDI_SUCCESS;
  byte                            svr_avail_sst;
  gsdi_slot_id_type               new_slot;
  int                             cst_index;


  /* Change to looping method of searching for file
  ** in case the Index is changed by adding files above
  ** CST
  */
  if (gsdi_util_is_slot_pref(slot, GSDI_CDMA, &new_slot))
  {
      cst_index = 2;
  }
  else {
      cst_index = 5;
  }

  if (cst_index >= GSDI_NUM_FILES_IN_RUIM_CACHE) {
    return GSDI_SUCCESS;
  }
  if  (gsdi_ruim_cache[cst_index].init == FALSE) {
    gsdi_status = GSDI_NOT_INIT;
    return gsdi_status;
  }

  switch(service) {


  case GSDI_CHV1_DISABLE:   /* CHV1 disabled function           */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_CHV1_DISABLE_OFFSET])&GSDI_SST_CHV1_DISABLE_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_CHV1_DISABLE_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_ADN:            /* GSM Abbreviated Dialling Numbers */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_ADN_OFFSET])&GSDI_SST_ADN_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_ADN_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_FDN:            /* Forbidden Dialling Numbers       */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_FDN_OFFSET])&GSDI_SST_FDN_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_FDN_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_SMS:            /* Short Message Services           */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_SMS_OFFSET])&GSDI_SST_SMS_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_SMS_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;



    /* No 5 - 9 RFU */


  case GSDI_EXT1:           /* Extension 1                      */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_EXT1_OFFSET])&GSDI_SST_EXT1_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_EXT1_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_EXT2:           /* Extension 2                      */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_EXT2_OFFSET])&GSDI_SST_EXT2_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_EXT2_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_SMSP:           /* SMS Parameters                   */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_SMSP_OFFSET])&GSDI_SST_SMSP_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_SMSP_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_LND:            /* Last Number Dialled              */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_LND_OFFSET])&GSDI_SST_LND_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_LND_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  /* No 14-16 RFU */

  case GSDI_SPN:            /* Service Provider Name            */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_SPN_OFFSET])&GSDI_SST_SPN_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_SPN_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_SDN:            /* Service Dialling Numbers         */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_SDN_OFFSET])&GSDI_SST_SDN_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_SDN_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  case GSDI_EXT3:           /* Extension 3                      */
    /* Get the info from the service table cache */
    svr_avail_sst = (gsdi_ruim_cache[cst_index].body[GSDI_SST_EXT3_OFFSET])&GSDI_SST_EXT3_MASK;

    /* shift the info to bit 2 (RUIM bit in service bitmap) */
    if (svr_avail_sst == GSDI_SST_EXT3_MASK) {
      svr_avail_sst =  0x01;
    }
    else {
      svr_avail_sst =  0x00;
    }
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  /* No 20 RFU */

  case GSDI_DATA_DL_SMSPP:           /* Data download via SMS-PP         */
    /* Get the info from the service table cache */

    svr_avail_sst =
      (gsdi_ruim_cache[cst_index].body[GSDI_SST_DATA_DL_SMSPP_OFFSET])& GSDI_SST_DATA_DL_SMSPP_MASK;

    /* shift the info to the least significant bit */
    if (svr_avail_sst == GSDI_SST_DATA_DL_SMSPP_MASK)
    {
      svr_avail_sst =  0x01;
    }
    else
    {
      svr_avail_sst =  0x00;
    }

    /* shift the info to bit 2 (RUIM bit in service bitmap) */
    svr_rsp->svr_bitmap = (uint8)(svr_rsp->svr_bitmap | (svr_avail_sst << 2));
    break;

  default:
    gsdi_status = GSDI_INCORRECT_PARAMS;
    break;
  }

  return gsdi_status;
 } /*gsdi_util_ruim_service_check*/

#endif /*#ifdef FEATURE_MMGSDI_CDMA*/

/*===========================================================================
FUNCTION GSDI_UTIL_IS_GSDI_CACHE_READY

DESCRIPTION

  This utility function is a called by the gsdi Library to check if the GSDI
  has cached all necessary data.  This function is a necessity for
  GSDI Optimization and will be used to make sure cached items are not accessed
  until they are all ready.

  When called, this function (which executes in the context of the caller)
  checks the gsdi_data.cache_ready  Boolean.


DEPENDENCIES
  None

RETURN VALUE
  Boolean:
    if gsdi_data.cache_ready = TRUE, the function returns TRUE
    if gsdi_data.cache_ready = FALSE, the function returns FALSE

===========================================================================*/
boolean gsdi_util_is_gsdi_cache_ready (
    void
)
{
#ifdef FEATURE_GSDI_OPTIMIZATION
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    return gsdi_data.cache_ready;
#else
    return (gsdi_data.cache_ready | gsdi_data.cache_ready_2);
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#else
    return FALSE;
#endif /* FEATURE_GSDI_OPTIMIZATION */
}

/*===========================================================================
FUNCTION GSDI_UTIL_GSTK_TP_DL_COMPLETE

DESCRIPTION

  This function is a utility function called by GSTK to quickly notify GSDI
  that it has completed the TERMINAL PROFILE DONWLOAD Procedures.

  When called, this function (which executes in the context of the caller) will
  set the GSDI SIGNAL GSDI_WAIT_GSTK_TP_DL_SIG for further processing.

DEPENDENCIES
  None

RETURN VALUE

  void

===========================================================================*/
void gsdi_util_gstk_tp_dl_complete (
  void
)
{
  /* Set Signal to GSDI so that it can continue
  ** with the SIM Initialization procedures
  */
  (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_WAIT_GSTK_TP_DL_COMPLETED_SIG);
  (void)rex_set_sigs(&gsdi_task_tcb, GSDI_WAIT_GSTK_TP_DL_COMPLETED_SIG);
}
#if (defined FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA) || defined (FEATURE_MMGSDI_UMTS)
/*===========================================================================
FUNCTION GSDI_UTIL_SELECTIVE_CARD_CACHE_DELETE
DESCRIPTION
  This function is called to delete all the cache from the given protocol.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
LOCAL void gsdi_util_selective_card_cache_delete(
  gsdi_app_enum_type    app_type,
  uim_items_enum_type * efs_not_to_clear,
  uint8 num_items,
  gsdi_slot_id_type slot)
{
  boolean ok_to_clear   = FALSE;
  word file_num         = 0;
  word index                    = 0;
  gsdi_slot_id_type  new_slot;
  sim_cache_element_T   *cache_ptr;
  int                    max_cache_size;
  gsdi_technology_type   tech;

  switch (app_type)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
#ifdef FEATURE_MMGSDI_CDMA
  case GSDI_CDMA_APP:
    cache_ptr = gsdi_ruim_cache;
    max_cache_size = GSDI_NUM_FILES_IN_RUIM_CACHE;
    MSG_HIGH("Deleting Cache for CDMA R-UIM",0,0,0);
    tech = GSDI_CDMA;
    break;
#endif /*FEATURE_MMGSDI_CDMA */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS */

  default:
    MMGSDI_DEBUG_MSG_ERROR("App Type is neither GSM nor CDMA",0,0,0);
    return;
  }
  if (num_items == 0)
  {
    MSG_ERROR("USE GSDI_UTIL_DELETE_SIM_CACHE",0,0,0);
    return;
  }

  if ( efs_not_to_clear == NULL )
  {
    ERR("NULL POINTER",0,0,0);
    return;
  }

  /* First Go Through Cache */
  for ( file_num = 0; file_num < max_cache_size; file_num++)
  {
    /* Now go through ef_not_to_clear for
    ** each file_num to ensure we don't
    ** clear it out
    */
    ok_to_clear = TRUE;
    for ( index = 0; index < num_items; index++)
    {
      /* Verify that a Cache Item to to clear is not
      ** to be cleared
      */
      if ( cache_ptr[file_num].filename == efs_not_to_clear[index] )
      {
        ok_to_clear = FALSE;
        break;
      }
    }
    if ( ok_to_clear )
    {
      if (gsdi_util_is_slot_pref(slot, tech, &new_slot) ==
              cache_ptr[file_num].pref_slot) {
        cache_ptr[file_num].init = FALSE;
        memset(cache_ptr[file_num].body,
             0x00,
             GSDI_CACHE_MAX_FILE_LEN);
        cache_ptr[file_num].in_synch = FALSE;
        if (cache_ptr[file_num].add_data != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE(cache_ptr[file_num].add_data);
        }
        MSG_HIGH("App %x cache delete %x",app_type, cache_ptr[file_num].filename, 0);
      }
    }
  }
} /* gsdi_util_selective_card_cache_delete */

#endif /* (defined FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA) */

/*===========================================================================
FUNCTION GSDI_UTIL_SELECTIVE GLOBAL_DATA_INIT

DESCRIPTION
  This function is called to put the GSDI into a state where it is ready
  to accept the COM LINK ESTABLISHED Message from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_selective_global_data_init(
  gsdi_slot_id_type slot,
  gsdi_refresh_modes_enum_type refresh_mode,
  boolean reselect_app
)
{
  MSG_HIGH("GSDI_UTIL_SELECTIVE_GLOBAL_DATA_INIT slot 0x%x, refresh mode 0x%x",
           slot, refresh_mode, 0);

  if ((refresh_mode != GSDI_SIM_RESET) &&
      (refresh_mode != GSDI_USIM_APP_RESET) &&
      (refresh_mode != GSDI_3G_SESSION_RESET))
  {
    MSG_ERROR("invalid refresh mode 0x%x", refresh_mode, 0, 0);
    return;
  }

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
  gsdi_data.ruim_dfs_present = GSDI_NO_DIR_PRESENT;
#endif
  gsdi_data.uim_error_condition = UIM_NO_SUCH_STATUS_S;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

  if ( slot == GSDI_SLOT_1)
  {
    if (refresh_mode == GSDI_SIM_RESET)
    {
      memset(&mmgsdi_data_slot1.applications, 0x00, sizeof(mmgsdi_apps_type));
      memset(&mmgsdi_data_slot1.operational_apps, 0x00, sizeof(mmgsdi_apps_type));
      gsdi_data.gsdi_gstk_tp_done = FALSE;
      gsdi_data.card_applications.apps_enabled = 0;
      gsdi_data.card_applications.operational_slot &= 0xF0;
      gsdi_data.current_card_mode_capability = GSDI_CARD_MODE_UNKNOWN;
      memset(&gsdi_data.hist_bytes, 0x00, sizeof(gsdi_hist_byte_type));
      if(mmgsdi_data_slot1.app_info_ptr != NULL)
      {
        mmgsdi_util_reset_pin_status();
        /* Don't reset the data for reselecting the app */
        if(!reselect_app)
        {
          memset(&mmgsdi_data_slot1.app_info_ptr[mmgsdi_data_slot1.sel_aid_index].app_data,
                 0x00,
                 sizeof(mmgsdi_aid_type));
          mmgsdi_util_close_all_sessions_cleanup_and_notify();
        }
      }
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
    }
    if(mmgsdi_data_slot1.app_info_ptr != NULL)
    {

      mmgsdi_util_reset_pin_status();
      mmgsdi_data_slot1.app_info_ptr[mmgsdi_data_slot1.sel_aid_index].pin1 = NULL;
      mmgsdi_data_slot1.app_info_ptr[mmgsdi_data_slot1.sel_aid_index].universal_pin = NULL;
    }
    gsdi_data.cache_ready = FALSE;
    gsdi_data.sim_phase = 0x00;
    gsdi_data.sim_present_and_ready = FALSE;
    gsdi_data.init_completed = FALSE;
    gsdi_data.illegal_sim = FALSE;

    gsdi_data.sim_capability.fdn_enabled = FALSE;
    gsdi_data.sim_capability.bdn_enabled = FALSE;
    gsdi_data.sim_capability.acl_enabled = FALSE;
    gsdi_data.sim_capability.imsi_invalidated = FALSE;
    gsdi_data.sim_capability.unknown_capabilities = FALSE;
    gsdi_data.sim_capability.sim_phone_book.multiple_pbr_entries = FALSE;
    gsdi_data.sim_capability.sim_phone_book.present = FALSE;
    gsdi_data.disable_chv1_allowed = FALSE;
    gsdi_data.just_invalidated = TRUE;
    gsdi_data.just_rehabilitated = TRUE;
    gsdi_data.registered_for_fcns = FALSE;

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CPHS
     memset(&gsdi_data.cphs_info, 0, sizeof(gsdi_cphs_information_type));
#endif /*FEATURE_MMGSDI_CPHS*/
    gsdi_data.gsdi_state = GSDI_NOT_INIT_S;
  }
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  else if ( slot == GSDI_SLOT_2)
  {
    if (refresh_mode == GSDI_SIM_RESET)
    {
      memset(&mmgsdi_data_slot2.applications, 0x00, sizeof(mmgsdi_apps_type));
      memset(&mmgsdi_data_slot2.operational_apps, 0x00, sizeof(mmgsdi_apps_type));
      gsdi_data.gsdi_gstk_tp_done_2 = FALSE;
      gsdi_data.card_applications.apps2_enabled = 0;
      gsdi_data.card_applications.operational_slot &= 0x0F;
      gsdi_data.current_card_mode_capability_2 = GSDI_CARD_MODE_UNKNOWN;
      memset(&gsdi_data.hist_bytes_2, 0x00, sizeof(gsdi_hist_byte_type));
      if(mmgsdi_data_slot2.app_info_ptr != NULL)
      {
        memset(&mmgsdi_data_slot2.app_info_ptr[mmgsdi_data_slot2.sel_aid_index].app_data,
               0x00,
               sizeof(mmgsdi_aid_type));
        mmgsdi_util_close_all_sessions_cleanup_and_notify();
      }
    }
    if(mmgsdi_data_slot2.app_info_ptr != NULL)
    {
      // TODO, to properly clear the pin1 and pin2 info
      mmgsdi_data_slot2.app_info_ptr[mmgsdi_data_slot2.sel_aid_index].pin_cache = FALSE;
      mmgsdi_data_slot2.app_info_ptr[mmgsdi_data_slot2.sel_aid_index].pin1 = NULL;
      memset(&mmgsdi_data_slot2.app_info_ptr[mmgsdi_data_slot2.sel_aid_index].pin2,
             0x00,
             sizeof(mmgsdi_app_pin_info_type));
      mmgsdi_data_slot2.app_info_ptr[mmgsdi_data_slot2.sel_aid_index].universal_pin = NULL;
    }
    gsdi_data.cache_ready_2 = FALSE;
    gsdi_data.sim_phase_2 = 0x00;
    gsdi_data.sim_present_and_ready_2 = FALSE;
    gsdi_data.init_completed_2 = FALSE;
    gsdi_data.illegal_sim_2 = FALSE;
    gsdi_data.pin_status_info_2.PIN1_enabled = FALSE;
    gsdi_data.pin_status_info_2.PIN1_key_ref = 0;
    gsdi_data.pin_status_info_2.PIN2_enabled = FALSE;
    gsdi_data.pin_status_info_2.PIN2_key_ref = 0;
    gsdi_data.pin_status_info_2.UNIV_enabled = FALSE;
    gsdi_data.pin_status_info_2.UNIV_key_ref = 0;
    gsdi_data.pin_status_info_2.pin1_num_retries_allowed = 0xFF;
    gsdi_data.pin_status_info_2.puk1_num_retries_allowed = 0xFF;
    gsdi_data.pin_status_info_2.pin2_num_retries_allowed = 0xFF;
    gsdi_data.pin_status_info_2.puk2_num_retries_allowed = 0xFF;
    gsdi_data.pin_status_info_2.pin1_status = GSDI_PIN_NOT_INITIALISED;
    gsdi_data.pin_status_info_2.pin2_status = GSDI_PIN_NOT_INITIALISED;
    gsdi_data.pin_status_info_2.pin_status_cached = FALSE;
    gsdi_data.sim_capability_2.fdn_enabled = FALSE;
    gsdi_data.sim_capability_2.bdn_enabled = FALSE;
    gsdi_data.sim_capability_2.acl_enabled = FALSE;
    gsdi_data.sim_capability_2.imsi_invalidated = FALSE;
    gsdi_data.sim_capability_2.unknown_capabilities = FALSE;
    gsdi_data.sim_capability_2.sim_phone_book.multiple_pbr_entries = FALSE;
    gsdi_data.sim_capability_2.sim_phone_book.present = FALSE;
    gsdi_data.disable_chv1_allowed_2 = FALSE;
    gsdi_data.just_invalidated_2 = TRUE;
    gsdi_data.just_rehabilitated_2 = TRUE;

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
    gsdi_data.gsdi_state_2 = GSDI_NOT_INIT_S;
  }
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi_util_selective_global_data_init */

/*===========================================================================
FUNCTION GSDI_UTIL_PRE_INIT_GSDI

DESCRIPTION
  This function is called to put the GSDI into a state where it is ready
  to accept the COM LINK ESTABLISHED Message from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_preinit_gsdi(
  gsdi_refresh_modes_enum_type refresh_mode,
  gsdi_slot_id_type slot,
  boolean reselect_app
)
{
    uim_items_enum_type     items_not_deleted[5];
    uint8                   num_items            = 0;
    uint8                   apps_enable          = 0;
    gsdi_protocol_enum_type protocol             = GSDI_PROTOCOL_NONE;


    /*
    ** Disable GSDI from processing
    ** anymore requests
    */
    MSG_HIGH("PRE INIT GSDI",0,0,0);

    /*
    ** Get protocol of the slot
    */
    if (gsdi_get_protocol(slot, &protocol) !=  GSDI_SUCCESS)
    {
      MSG_ERROR("failed to get protocol of slot 0x%x!", slot, 0, 0);
    }

    /*
    ** Don't Allow Cache Accesses
    */
    switch (slot)
    {
      case GSDI_SLOT_1:
        gsdi_data.cache_ready = FALSE;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
        gsdi_data.cache_ready_2 = FALSE;
        break;
#endif/*FEATURE_MMGSDI_DUAL_SLOT*/
      default:
        MSG_ERROR("Unknown slot in preinit gsdi %x", slot, 0, 0);
        break;
    }


    switch ( refresh_mode)
    {
    case GSDI_SIM_RESET:
    case GSDI_USIM_APP_RESET:
    case GSDI_3G_SESSION_RESET:

#if defined (FEATURE_MMGSDI_PERSONALIZATION)
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE)
      gsdi_data.perso_engine_state    = GSDI_PERSO_ENG_NOT_INIT_S;
#else
      gsdi_data.perso_state           = GSDI_PERSO_NOT_INIT_STATE;
#endif  /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* !FEATURE_MMGSDI_PERSONALIZATION  */

      /* Perso refresh procedures required */
      gsdi_data.gstk_data.perso_refresh_needed = TRUE;

      switch (slot)
      {
      case GSDI_SLOT_1:
        apps_enable = gsdi_data.card_applications.apps_enabled;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      case GSDI_SLOT_2:
        apps_enable = gsdi_data.card_applications.apps2_enabled;
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      default:
        MSG_ERROR("Unknown slot", 0, 0, 0);
        return;
      }

      /* Invalidate the Cache */
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
      if ( apps_enable & GSDI_RUIM_APP_MASK)
      {
         gsdi_util_delete_ruim_cache(slot);
      }
#endif /* FEATURE_MMGSDI_CDMA */
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /*FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY*/

      gsdi_util_selective_global_data_init(slot, refresh_mode, reselect_app);
      break;

    case GSDI_SIM_INIT:
    case GSDI_SIM_INIT_AND_FILE_CHANGE:
    case GSDI_SIM_INIT_AND_FULL_FILE_CHANGE:

       /* Perso refresh procedures required */
        gsdi_data.gstk_data.perso_refresh_needed = TRUE;

        switch ( slot )
        {
        case GSDI_SLOT_1:
          apps_enable = gsdi_data.card_applications.apps_enabled;
          gsdi_data.just_invalidated      = TRUE;
          gsdi_data.just_rehabilitated    = TRUE;
          gsdi_data.sim_present_and_ready = FALSE;
          gsdi_data.init_completed        = FALSE;
          gsdi_data.illegal_sim           = FALSE;
          if (gsdi_is_ens_enabled_in_nv() ||
              (protocol == GSDI_PROTOCOL_GSM))
          {
            gsdi_data.gsdi_gstk_tp_done     = FALSE;
          }
#ifdef FEATURE_MMGSDI_CPHS
          memset(&gsdi_data.cphs_info, 0, sizeof(gsdi_cphs_information_type));
#endif /*FEATURE_MMGSDI_CPHS*/
#if defined (FEATURE_MMGSDI_PERSONALIZATION)
#if defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE)
          gsdi_data.perso_engine_state    = GSDI_PERSO_ENG_NOT_INIT_S;
#else
          gsdi_data.perso_state           = GSDI_PERSO_NOT_INIT_STATE;
#endif  /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* !FEATURE_MMGSDI_PERSONALIZATION  */
          break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        case GSDI_SLOT_2:
          apps_enable = gsdi_data.card_applications.apps2_enabled;
          gsdi_data.just_invalidated_2      = TRUE;
          gsdi_data.just_rehabilitated_2    = TRUE;
          gsdi_data.sim_present_and_ready_2 = FALSE;
          gsdi_data.init_completed_2        = FALSE;
          gsdi_data.illegal_sim_2           = FALSE;
          if (gsdi_is_ens_enabled_in_nv() ||
              (protocol == GSDI_PROTOCOL_GSM))
          {
            gsdi_data.gsdi_gstk_tp_done_2     = FALSE;
          }
          break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

        default:
          MSG_ERROR("Unknown slot", 0, 0, 0);
          return;
        }

        /* Invalidate the Cache */
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
#ifdef FEATURE_MMGSDI_CDMA
        if ( apps_enable & GSDI_RUIM_APP_MASK)
        {
          /* Don't want to clear out entire Cache */
          /* Should selectivly clear cache        */
          /* Clear all files except the following */
          items_not_deleted[0] = UIM_CDMA_PREF_LANG;
          items_not_deleted[1] = UIM_CDMA_ECC;
          num_items = 2;
          gsdi_util_selective_card_cache_delete(GSDI_CDMA_APP, items_not_deleted,num_items, slot);
        }
#endif /* FEATURE_MMGSDI_CDMA */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/
        /* Now build an internal Message to notify */
        /* GSDI to run Post Pin1 Procedures */
        break;

    case GSDI_FILE_CHANGE_NOTIFICATION:
        /* Do Nothing */
        break;

    default:
        MSG_ERROR("UNKNOWN REFRESH MODE: %x",refresh_mode,0,0);
        break;
    }

}/* gsdi_util_preinit_gsdi */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

/*===========================================================================
FUNCTION GSDI_UTIL_CONVERT_FILE_PATHS

DESCRIPTION
  This function is called convert the File Paths in the provided data_p
  3F,00,7F,20,FF,FF into a uim_items_enum_type


DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS Indicated ME doe keep file imaged
                   GSDI_ERROR   Indicates ME does not keep file imaged
===========================================================================*/
void gsdi_util_convert_file_paths(
    uint8 data_len,
    uint8 * data_p,
    uim_items_enum_type * sim_filenames_p,
    uint8 * num_files_p
)
{
    uint8 file_num = 0;
    word path[UIM_MAX_PATH_ELEMENTS+1];
    byte index = 0;
    boolean found = FALSE;
    byte path_index = 0;

    if( NULL == num_files_p || NULL == data_p)
    {
      return;
    }
    memset(path,
           0xFF,
           (sizeof(word)*(UIM_MAX_PATH_ELEMENTS+1)));
    /* First Byte Contains Number of
    ** files in the file list
    */
    *num_files_p = data_p[index++];
    data_len = data_len - 1;

    while ( index < data_len )
    {
        MSG_LOW("FILE NUM: %x", file_num,0,0);
        path_index = 0;

        do
        {
          path[path_index] = data_p[index++] << GSDI_SHIFT_TO_BYTE1_OF_WORD;
          path[path_index] = path[path_index] | data_p[index++];
          path_index +=1;
          MSG_HIGH("File path info - Ox%x", path[path_index-1],0,0);
        }while (index < data_len && data_p[index] != MF_HIGH_BYTE);

#ifdef FEATURE_VIRTUAL_SIM
        /* Note: The below code inside the switch statements could cause
           lint or warning for 6k since its not casted. But casting will
           break compilation for 7k. */
        switch ( file_num )
        {
          case 1:
            sim_filenames_p[file_num] = UIM_GSM_IMSI;
            break;
          case 2:
            sim_filenames_p[file_num] = UIM_GSM_PLMN;
            break;
          case 3:
            sim_filenames_p[file_num] = UIM_GSM_ACM_MAX;
            break;
        }
#else
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
        if ( !found )
        {
          if(NULL ==sim_filenames_p)
          {
            MSG_ERROR("gsdi_util_convert_file_paths NULL sim_filenames_p",0,0,0);
            return;
          }
          sim_filenames_p[file_num] = (uim_items_enum_type)uim_get_enum_value_for_path(path);
        }
#endif /* FEATURE_VIRTUAL_SIM */
        file_num++;
    } /* While Loop */

    /* file_num has the no of files that are decoded from file_path buffer */
    *num_files_p = file_num;
} /* gsdi_util_convert_file_paths */ /*lint !e715 */


/*===========================================================================
FUNCTION GSDI_UTIL_SEND_VOTE_NOTIFICATION

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
LOCAL void gsdi_util_send_vote_notification(
  gsdi_refresh_vote_node_type * current_node,
  uim_items_enum_type         * sim_filenames_p,
  uint8                         num_files,
  boolean                       request_to_take_down_callstack
)
{
  boolean                   ok_to_take_down_callstack      = FALSE;
  boolean                   ok_to_do_file_notify           = FALSE;

  if ( current_node != NULL )
  {
    /* Notify Clients to Vote...i.e. call their
    ** voting callbacks they provided
    */
    ((gsdi_refresh_vote_cb_type *)current_node->vote_data.callback)(
        sim_filenames_p,
        num_files,
        request_to_take_down_callstack,
        &ok_to_take_down_callstack,
        &ok_to_do_file_notify);

    /* Save the Return Data to Link List */
    current_node->vote_data.ok_to_take_down_stack = ok_to_take_down_callstack;
    current_node->vote_data.ok_to_file_notify     = ok_to_do_file_notify;
  }
}
/*===========================================================================
FUNCTION GSDI_UTIL_NOTIFY_CLIENTS_TO_VOTE

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_notify_clients_to_vote(
    uim_items_enum_type * sim_filenames_p,
    uint8                 num_files,
    boolean               request_to_take_down_callstack)
{
    gsdi_refresh_vote_node_type * current_node;
    gsdi_refresh_vote_node_type * next_node;

    /* Set the Current Node to be the Head of
    ** the link list
    */
    current_node = (void *)refresh_vote_list_node.head_p;

    while ( current_node != NULL)
    {

      gsdi_util_send_vote_notification(current_node,
                           sim_filenames_p,
                           num_files,
                           request_to_take_down_callstack);

      next_node    = (void *)current_node->next_p;
      current_node = (void *)next_node;

    }
}
/*===========================================================================
FUNCTION GSDI_UTIL_CAN_GSDI_TAKE_DOWN_STACK

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_can_gsdi_take_down_stack(
  gsdi_refresh_vote_node_type * head_node,
  boolean * reset_allowed_p
)
{
  gsdi_refresh_vote_node_type * next_node;
  gsdi_refresh_vote_node_type * current_node;
  current_node = head_node;
  /*lint -esym(613,reset_allowed_p)*/
  while ( current_node != NULL )
  {
    *reset_allowed_p &= current_node->vote_data.ok_to_take_down_stack;
    next_node = (void *)current_node->next_p;
    current_node = next_node;
  }
  /*lint +esym(613,reset_allowed_p)*/
}
/*===========================================================================
FUNCTION GSDI_UTIL_CAN_GSDI_DO_FILE_NOTIFYS

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_can_gsdi_do_file_notify(
    gsdi_refresh_vote_node_type * head_node_p,
    boolean                     * ok_to_do_file_notify_p
)
{
    gsdi_refresh_vote_node_type * current_node_p = NULL_PTR;

    current_node_p = (void *)head_node_p;
    /*lint -esym(613,ok_to_do_file_notify_p)*/
    while ( current_node_p != NULL )
    {
        *ok_to_do_file_notify_p &=  (boolean)current_node_p->vote_data.ok_to_file_notify;
        current_node_p  = (void *)current_node_p->next_p;
    }
}

/*===========================================================================
FUNCTION GSDI_UTIL_NEED_TO_TAKE_DOWN_CALL_STACK

DESCRIPTION
    This function will be used to check if the file list provided requires the
  GSDI Task to take down the call stack.

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  The callstack needs to be taken down.
            FALSE: The callstack does not need to be taken down.
===========================================================================*/
 boolean gsdi_util_need_to_take_down_call_stack(
    uint8 num_files,
    uim_items_enum_type * file_list_p
)
{
    uint8 index = 0;

    if(NULL == file_list_p)
    {
      return FALSE;
    }

    while (index < num_files )
    {
        switch(file_list_p[index])
        {
        /* GSM Related Files which
        ** Require call stack to
        ** be taken down and then
        ** brought back up
        ** No need to take down call stack for EF SPN
        */
        case UIM_GSM_AD:
        case UIM_GSM_AAEM:
        case UIM_GSM_ACC:
        case UIM_GSM_BCCH:
        case UIM_GSM_EMLPP:
        case UIM_GSM_IMSI:
        case UIM_GSM_KC:
        case UIM_GSM_KCGPRS:
        case UIM_GSM_LOCI:
        case UIM_GSM_LOCIGPRS:
        case UIM_GSM_LP:
        case UIM_GSM_PHASE:
        case UIM_GSM_SST:

        /* USIM Related Files which
        ** Require call stack to
        ** be taken down and then
        ** brought back up
        ** No need to take down call stack for EF SPN
        */
        case UIM_USIM_AAEM:
        case UIM_USIM_EMLPP:
        case UIM_USIM_EST:
        case UIM_USIM_IMSI:
        case UIM_USIM_KC:
        case UIM_USIM_KEYS:
        case UIM_USIM_KEYSPS:
        case UIM_USIM_UST:

        /* RUIM Related Files which
        ** Require call stack to
        ** be taken down and then
        ** brought back up
        */

          return TRUE;

        default:
          /* Do Noting...continue in loop
          ** and check next file in file
          ** list
          */
          break;

        }

        index++;
    }
    /* Getting this FAR Indicates that
    ** the files in the File List do
    ** not require a take down and
    ** bring up of the stack
    */
    return FALSE;
} /* gsdi_util_need_to_take_down_call_stack */

/*===========================================================================
FUNCTION GSDI_UTIL_CLEAR_FILE_FROM_CACHE

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_clear_file_from_cache(
    uint8                 num_files,
    uim_items_enum_type * sim_filenames_p,
    gsdi_slot_id_type     slot
)
{
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
    uint8 cache_file_num = 0;
    uint8 file_num = 0;
    gsdi_slot_id_type new_slot = GSDI_SLOT_NONE;
    uint8 *apps_enabled_ptr = NULL;

    if ( num_files == 0
        || num_files >= GSDI_NUM_FILES_IN_SIM_CACHE
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
       )
    {
        return;
    }

    switch (slot) {
      case GSDI_SLOT_1:
      apps_enabled_ptr = &gsdi_data.card_applications.apps_enabled;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      apps_enabled_ptr = &gsdi_data.card_applications.apps2_enabled;
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MSG_ERROR("Unknown Slot", 0, 0, 0);
      return;
    }

    /* Go through SIM Cache and see if
    ** a match is made
    */

    if(((*apps_enabled_ptr) & GSDI_GSM_SIM_MASK) == GSDI_GSM_SIM_MASK)
    {

      while (file_num < num_files )
      {
        /* Go through File List and
        ** try to match with SIM Cache
        */
        for ( cache_file_num = 0;
              cache_file_num < GSDI_NUM_FILES_IN_SIM_CACHE;
              cache_file_num++ )
        {
          if(NULL == sim_filenames_p)
          {
            return;
          }
          if ( sim_filenames_p[file_num] == gsdi_sim_cache[cache_file_num].filename )
          {
            if (gsdi_util_is_slot_pref(slot, GSDI_GSM, &new_slot) ==
                gsdi_sim_cache[cache_file_num].pref_slot)
            {
              if(gsdi_util_is_file_from_do_not_clear_cache_list(
                   sim_filenames_p[file_num], GSDI_GSM) != TRUE)
              {
                gsdi_sim_cache[cache_file_num].init = FALSE;
                memset(gsdi_sim_cache[cache_file_num].body,
                       0x00,
                       GSDI_CACHE_MAX_FILE_LEN);
                MSG_LOW( "GSM SIM CACHE DELETE %x",
                          gsdi_sim_cache[file_num].filename,
                          0,
                          0);
                gsdi_sim_cache[cache_file_num].in_synch = FALSE;
                MMGSDIUTIL_TMC_MEM_FREE( gsdi_sim_cache[cache_file_num].add_data);
              }/* if(gsdi_util_is_file_from_do_not_clear_cache_list(
                  sim_filenames_p[file_num], GSDI_GSM) != TRUE)*/
            }
          }
#ifdef FEATURE_MMGSDI_CPHS
          if(sim_filenames_p[file_num] == UIM_GSM_CPHSI)
          {
            gsdi_data.cphs_info.cphs_capable = FALSE;
            memset(&gsdi_data.cphs_info.services,0,sizeof(gsdi_cphs_service_table_type));
            break;
          }
          else if(sim_filenames_p[file_num] == UIM_GSM_CSP)
          {
            memset(&gsdi_data.cphs_info.csp, 0 ,sizeof(gsdi_cphs_cust_service_profile_type));
            break;
          }
#endif/*FEATURE_MMGSDI_CPHS*/
        }
        file_num++;
      }
    }

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS || FEATURE_UIM_RUIM_W_GSM_ACCESS*/

}


/*===========================================================================
FUNCTION GSDI_UTIL_IS_REFRESH_ALLOWED

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_is_refresh_allowed(
    gsdi_sim_refresh_req_T * req_p,
    uint8                  * num_files_p,
    uim_items_enum_type    * sim_filenames_p,
    boolean                * reset_allowed_p,
    boolean                * fcn_allowed_p
)
{
    boolean                   request_to_take_down_callstack = FALSE;
    mmgsdi_int_app_info_type *sel_app_ptr                    = NULL;
    mmgsdi_aid_type           aid;
    mmgsdi_return_enum_type   mmgsdi_status                  = MMGSDI_SUCCESS;

    if(NULL == req_p)
    {
      MSG_ERROR("gsdi_util_is_refresh_allowed NULL req_p",0,0,0);
      return;
    }
    /* Convert the files in the file change
    ** to uim_items_enum_type
    */
    /* Are there Bytes for EFs Present? */
    if ( req_p->data_len > 0 )
    {
        /* Have to Decode a PATH and Convert
        ** to uim_items_enum_types
        */
        gsdi_util_convert_file_paths( req_p->data_len,
                                      req_p->data,
                                      sim_filenames_p,
                                      num_files_p);
    }

    /* Do we need to take down
    ** the call stack ?
    */
    switch ( req_p->refresh_mode)
    {
    case GSDI_SIM_RESET:
        request_to_take_down_callstack = TRUE;
        break;

    case GSDI_SIM_INIT:
    case GSDI_SIM_INIT_AND_FULL_FILE_CHANGE:
    case GSDI_SIM_INIT_AND_FILE_CHANGE:
        request_to_take_down_callstack = TRUE;
        break;

    case GSDI_FILE_CHANGE_NOTIFICATION:
        /* Are files in the File Change Notification
        ** requiring GSDI to refresh Call Stack
        ** related files
        */
        if(NULL == num_files_p)
        {
          MSG_ERROR("gsdi_util_is_refresh_allowed NULL num_files_p",0,0,0);
          return;
        }
        request_to_take_down_callstack =
                gsdi_util_need_to_take_down_call_stack(*num_files_p,
                                                       sim_filenames_p);
        break;

    case GSDI_USIM_APP_RESET:
    case GSDI_3G_SESSION_RESET:
        sel_app_ptr = mmgsdi_util_get_sel_app();
        if (sel_app_ptr == NULL)
        {
          *reset_allowed_p = FALSE;
          *fcn_allowed_p   = FALSE;
          return;
        }

        request_to_take_down_callstack = FALSE;

        if (req_p->app_id.aid_len == 0)
        {
          /* Default channel app */
          request_to_take_down_callstack = TRUE;
        }
        else
        {
          aid.aid.data_len = req_p->app_id.aid_len;
          memcpy(aid.aid.data_ptr, req_p->app_id.aid_data, req_p->app_id.aid_len);

          /* check if it is default app */
          mmgsdi_status = mmgsdi_util_cmp_aid(&sel_app_ptr->app_data,
                                              &aid);
          if (mmgsdi_status == MMGSDI_SUCCESS)
          {
            /* Default channel app */
            request_to_take_down_callstack = TRUE;
          }
        }
        break;

    default:
        MSG_ERROR("UNKNOWN REFRESH MODE %x", req_p->refresh_mode,0,0);
        break;
    }
    if( NULL == num_files_p)
    {
      MSG_ERROR("gsdi_util_is_refresh_allowed NULL num_files_p",0,0,0);
      return;
    }
    /* Call Callbacks for Voting */
    gsdi_util_notify_clients_to_vote(
                                  sim_filenames_p,
                                  *num_files_p,
                                  request_to_take_down_callstack);

    /* What did the Clients Vote?
    ** Can GSDI Take down the Call Stack
    */
    gsdi_util_can_gsdi_take_down_stack(
                              &refresh_vote_list_node,
                              reset_allowed_p);

    /* What did the Clients Vote?
    ** Can GSDI Send Out the File
    ** Change Notifications
    */
    gsdi_util_can_gsdi_do_file_notify(
                              &refresh_vote_list_node,
                              fcn_allowed_p);


} /* gsdi_util_is_refresh_allowed */

/*===========================================================================
FUNCTION GSDI_UTIL_SCAN_FCN_NODE

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
static gsdi_returns_T gsdi_util_scan_fcn_node(
    uim_items_enum_type * sim_filenames_p,
    uint8       * num_files_p,
    uint8       * mod_num_files_p,
    uim_items_enum_type * array_of_filenames_p,
    gsdi_refresh_fcn_notify_node_type * current_node_p
)
{
    uint8 index = 0;
    uint8 fcn_notify_index = 0;
    int num_files = 0;
    MMGSDI_RETURN_IF_NULL(current_node_p);
    MMGSDI_RETURN_IF_NULL(num_files_p);
    MMGSDI_RETURN_IF_NULL(sim_filenames_p);
    /* Need to match Files provided in sim_filenames_p
    ** with files the client registered for
    */
    while ( ( fcn_notify_index < GSDI_MAX_FILE_CHANGE_NOTIFICATIONS ) &&
            ( fcn_notify_index < current_node_p->fcn_notify_data.num_files ) )
    {
        while ( index < *num_files_p )
        {
            if ( sim_filenames_p[index] == current_node_p->fcn_notify_data.file_list[fcn_notify_index])
            {
                MMGSDI_RETURN_IF_NULL(array_of_filenames_p);
                /* Match Made */
                array_of_filenames_p[num_files] =
                    current_node_p->fcn_notify_data.file_list[fcn_notify_index];
                num_files++;
                break;
            }
            else
            {
                index++;
            }
        }
        /* Reset index */
        index = 0;

        /* Increase Index through current node
        ** file list
        */
        fcn_notify_index++;
    }
    if ( num_files > 0 )
    {
        /* Created a array of filenames
        ** that matches current clients
        ** registration for filenames.
        ** Store this information in link list
        */
        if ( num_files <= GSDI_MAX_FILE_CHANGE_NOTIFICATIONS )
        {
            MMGSDI_RETURN_IF_NULL(array_of_filenames_p);
            current_node_p->fcn_notify_data.num_files_in_notify_file_list = (uint8)num_files;
            memcpy(current_node_p->fcn_notify_data.notify_file_list,
                   array_of_filenames_p,
                   (sizeof(uim_items_enum_type) *
                    current_node_p->fcn_notify_data.num_files_in_notify_file_list ) );
        }
        else
        {
            /* For some reason, num_files exceeded MAX Number
            ** of Files allowed.
            */
            return GSDI_ERROR;
        }
    }

    return GSDI_SUCCESS;

} /* gsdi_util_scan_fcn_node */ /*lint !e715 */

/*===========================================================================
FUNCTION GSDI_UTIL_SEND_FTN_NOTIFICATION

DESCRIPTION
  This function is used to call the callback for the File Change notification

DEPENDENCIES
  None

RETURN VALUE
  boolean  TRUE:   Node has a notification that was sent out
           FALSE:  Notification not sent for node.
===========================================================================*/
LOCAL boolean gsdi_util_send_fcn_notification(
  gsdi_refresh_fcn_notify_node_type * current_node_p
)
{
    boolean sent_notification = FALSE;
    if ( current_node_p != NULL )
    {
        if ( current_node_p->fcn_notify_data.num_files_in_notify_file_list > 0)
        {
            /* Notify Clients to Vote...i.e. call their
            ** voting callbacks they provided
            */
            ((gsdi_task_notify_file_list_cb_type *)current_node_p->fcn_notify_data.callback)(
                current_node_p->fcn_notify_data.notify_file_list,
                current_node_p->fcn_notify_data.num_files_in_notify_file_list);

            /* Set the Flag which indicates a client was notified */
            current_node_p->fcn_notify_data.client_notified = TRUE;
            sent_notification = TRUE;

            MSG_HIGH("Notifying client with callback 0x%x for refresh ",
                     current_node_p->fcn_notify_data.callback,0,0);
        }
    }

    return sent_notification;
}
/*===========================================================================
FUNCTION GSDI_UTIL_NOTIFY_CLIENTS_FCN

DESCRIPTION
  This function is used to notify the clients of the File Change Notification
  based on whether or not the files match their registration.


DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_notify_clients_fcn(
    uim_items_enum_type * sim_filenames_p,
    uint8       * num_files_p)
{
    gsdi_refresh_fcn_notify_node_type  * current_node_p;
    gsdi_refresh_fcn_notify_node_type  * next_node_p;
    uint8                                mod_num_files;
    uim_items_enum_type                  array_of_filenames[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
    gsdi_returns_T                       gsdi_status = GSDI_SUCCESS;
    boolean                              sent_notification = FALSE;
    boolean                              refresh_in_progress = FALSE;
    /* Start with the First Node
    ** in the linked list
    */
    current_node_p = &refresh_fcn_notify_list_node;

    while ( current_node_p != NULL )
    {
        gsdi_status =  gsdi_util_scan_fcn_node( sim_filenames_p,
                                                num_files_p,
                                                &mod_num_files,
                                                array_of_filenames,
                                                current_node_p);

        next_node_p = (void *)current_node_p->next_p;
        current_node_p = next_node_p;
    }
    if ( gsdi_status == GSDI_SUCCESS )
    {
        /* Reset Current Node to point to head of fcn_list */
        current_node_p = &refresh_fcn_notify_list_node;
        while ( current_node_p != NULL )
        {
            /* Send Out the File Change Notifications
            ** to only the clients that have files
            ** that match current file change notification
            ** with only the files they registered for.
            */
            sent_notification = gsdi_util_send_fcn_notification(
                (gsdi_refresh_fcn_notify_node_type*)current_node_p);
            refresh_in_progress |= sent_notification;
            next_node_p = (void *)current_node_p->next_p;
            current_node_p = next_node_p;
        }
        if ( refresh_in_progress )
        {
          MSG_HIGH("GSDI Refresh in progress",0,0,0);
        }
        else
        {
          /* refresh is done */
          MSG_HIGH("GSDI Refresh has done successfully",0,0,0);
        }
    }
} /* gsdi_util_notify_clients_fcn */
/*===========================================================================
FUNCTION GSDI_UTIL_CLEAR_CLIENT_NOTIFY

DESCRIPTION
  This Function is used to clear the client notify information for each
  client as the client requests it when it has completed REFRESH Procedures.


DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_util_clear_client_notify(
    boolean                pass_fail,
    uint8                  num_files,
    uim_items_enum_type *  file_list_p,
    boolean              * pass_fail_p)
{
    gsdi_refresh_fcn_notify_node_type * node_p;
    boolean      found_node = FALSE;

    /* Assign Next Node to Head of Linked List */
    MMGSDI_RETURN_IF_NULL(file_list_p);
    MMGSDI_RETURN_IF_NULL(pass_fail_p);

    *pass_fail_p = TRUE;

    node_p = &refresh_fcn_notify_list_node;
    while ( node_p != NULL )
    {
        /* First Check if Node has correct
        ** number of files
        */
        if ( node_p->fcn_notify_data.num_files_in_notify_file_list == num_files &&
             found_node == FALSE)
        {
            /* Try to compare the filenames */
            if (!(memcmp(node_p->fcn_notify_data.notify_file_list,
                file_list_p,
                (sizeof(uim_items_enum_type) * num_files))))
            {
              /* Got a Match, clear client_notified bool */
              node_p->fcn_notify_data.client_notified = FALSE;

              /* Indicate whether the refresh attempt passed or failed */
              if ( pass_fail == TRUE )
              {
                  node_p->fcn_notify_data.client_refresh_attempt_failed = FALSE;
              }
              else
              {
                  node_p->fcn_notify_data.client_refresh_attempt_failed = TRUE;
              }

              /* Wipe out the File List used in the notification */
              memset(node_p->fcn_notify_data.notify_file_list,
                     0x00,
                    (sizeof(uim_items_enum_type) *
                    node_p->fcn_notify_data.num_files_in_notify_file_list));

              /* Wipe out the number of files in notify_file_list */
              node_p->fcn_notify_data.num_files_in_notify_file_list = 0;

              /* Indicate node found and modified */
              found_node = TRUE;
            }
        }
        if(!node_p->fcn_notify_data.client_refresh_attempt_failed)
        {
          *pass_fail_p = TRUE;
        }

        node_p = (void *)node_p->next_p;
    }
    if ( found_node == TRUE )
    {
        return GSDI_SUCCESS;
    }
    else
    {
        return GSDI_ERROR;
    }

} /* gsdi_util_clear_client_notify */

/*===========================================================================
FUNCTION GSDI_UTIL_CLEAR_CLIENT_NOTIFY

DESCRIPTION
  This function continues to return TRUE as soon as it detects that there
  is a client with a FCN Outstanding.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
boolean gsdi_util_notifications_still_outstanding( void )
{
    gsdi_refresh_fcn_notify_node_type * current_node_p = &refresh_fcn_notify_list_node;

    while ( current_node_p != NULL )
    {
        if ( current_node_p->fcn_notify_data.client_notified == TRUE)
        {
            MSG_HIGH("outstanding notification callback is 0x%x",
                      current_node_p->fcn_notify_data.callback,0,0);
            return TRUE;
        }
        current_node_p = (void*)current_node_p->next_p;
    }
    return FALSE;
} /* gsdi_util_notifications_still_outstanding */

/*===========================================================================
FUNCTION GSDI_UTIL_IS_SLOT_PREF

DESCRIPTION
  This function looks at the technology type and determines if the slot
  passed in is the operational slot for that particular technology.
  If it is, the function returns TRUE, else, it returns FALSE.
  The function also populates the new slot pointer to the correct slot info.
  This is required for automatic slot mode since the uim request will require
  the specification of the slot info

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE
===========================================================================*/
boolean gsdi_util_is_slot_pref ( gsdi_slot_id_type slot,
                                 gsdi_technology_type tech,
                                 gsdi_slot_id_type *new_slot)
{
    uint32 slot1_mask = 0;
    uint32 slot2_mask = 0;

    if (new_slot == NULL)
    {
        MSG_ERROR("Null input", 0, 0, 0);
        return FALSE;
    }

    /* set the new slot equals the old slot */
    *new_slot = slot;

    switch (tech)
    {
    case GSDI_GSM:
        slot1_mask = GSDI_GSM_SLOT_1_MASK;
        slot2_mask = GSDI_GSM_SLOT_2_MASK;
        break;
    case GSDI_CDMA:
        slot1_mask = GSDI_CDMA_SLOT_1_MASK;
        slot2_mask = GSDI_CDMA_SLOT_2_MASK;
        break;
    case GSDI_UMTS:
        slot1_mask = GSDI_USIM_SLOT_1_MASK;
        slot2_mask = GSDI_USIM_SLOT_2_MASK;
        break;
    default:
        break;
    }

    switch (slot)
    {
    case GSDI_SLOT_1:
        if ( ( gsdi_data.card_applications.operational_slot & slot1_mask ) ==
               slot1_mask )
            return TRUE;
        else
            return FALSE;

    case GSDI_SLOT_2:
        if ( ( gsdi_data.card_applications.operational_slot & slot2_mask ) ==
               slot2_mask )
            return TRUE;
        else
            return FALSE;

    case GSDI_SLOT_AUTOMATIC:
        if ( ( gsdi_data.card_applications.operational_slot & slot1_mask ) ==
               slot1_mask )
        {
            *new_slot = GSDI_SLOT_1;
        }
        else if ( ( gsdi_data.card_applications.operational_slot & slot2_mask )==
                    slot2_mask)
        {
            *new_slot = GSDI_SLOT_2;
        }
        else
        {
            MSG_ERROR("No operational slot for tech %d", tech, 0, 0);
            *new_slot = GSDI_SLOT_NONE;
        }
        return TRUE;

    default:
        return FALSE;
    }
} /* gsdi_util_is_slot_pref */

/*===========================================================================
FUNCTION GSDI_UTIL_SEND_CARD_ERROR

DESCRIPTION
  This function clear the slot present, apps enabled and operational slot
  information based on which error event is being passed in.
  The input event parameter should be limited to:
  GSDI_CARD_ERROR
  GSDI_CARD_ERROR_2
  After clearing the card_applications info, this function will notify the
  client with the card error event

DEPENDENCIES
  None

RETURN VALUE
  GSDI_SUCCESS
  GSDI_ERROR
===========================================================================*/
gsdi_returns_T gsdi_util_send_card_error(gsdi_slot_id_type slot)
{
    gsdi_sim_events_T event = GSDI_NO_SIM_EVENT;

    switch (slot)
    {
    case GSDI_SLOT_1:
        event = GSDI_CARD_ERROR;
        gsdi_data.card_applications.slot1_present = FALSE;
        gsdi_data.card_applications.apps_enabled = GSDI_NO_APP_MASK;
        gsdi_data.card_applications.operational_slot &= ~GSDI_GSM_SLOT_1_MASK;
        gsdi_data.card_applications.operational_slot &= ~GSDI_CDMA_SLOT_1_MASK;
        gsdi_data.card_applications.operational_slot &= ~GSDI_USIM_SLOT_1_MASK;
        gsdi_data.sim_present_and_ready = FALSE;
        gsdi_data.current_card_mode_capability = GSDI_CARD_MODE_UNKNOWN;
        break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
        event = GSDI_CARD_ERROR_2;
        gsdi_data.card_applications.slot2_present = FALSE;
        gsdi_data.card_applications.apps2_enabled = GSDI_NO_APP_MASK;
        gsdi_data.card_applications.operational_slot &= ~GSDI_GSM_SLOT_2_MASK;
        gsdi_data.card_applications.operational_slot &= ~GSDI_CDMA_SLOT_2_MASK;
        gsdi_data.card_applications.operational_slot &= ~GSDI_USIM_SLOT_2_MASK;
        gsdi_data.sim_present_and_ready_2 = FALSE;
        gsdi_data.current_card_mode_capability_2 = GSDI_CARD_MODE_UNKNOWN;
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
        MSG_ERROR("Unhandled error event: %d", event, 0, 0);
        return GSDI_ERROR;
    }
    /* Send Event and return status of sending event */
    return gsdi_util_send_mmi_sim_ind(event);

} /* gsdi_util_send_card_error */


/*===========================================================================
FUNCTION: gsdi_util_map_file_enum_to_uim_enum

DESCRIPTION
  Map MMGSDI File Enum to UIM File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
gsdi_returns_T gsdi_util_map_file_enum_to_uim_enum(
    gsdi_file_enum_type gsdi_file_item,
    uim_items_enum_type *uim_file_item_p )
{
  int i = 0;

  if (uim_file_item_p == NULL) {
    return GSDI_ERROR;
  }

  for ( i = 0; i < GSDI_FILE_ENUM_TABLE_SIZE; i++ ) {
    if (gsdi_file_item == gsdi_file_enum_table[i].gsdi_file) {
      *uim_file_item_p = gsdi_file_enum_table[i].uim_file;
      return GSDI_SUCCESS;
    }
  }
  return GSDI_ERROR;
}

/*===========================================================================
FUNCTION: gsdi_util_map_uim_enum_to_file_enum

DESCRIPTION
  Maps UIM File Enum to MMGSDI File Enum

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gsdi_returns_T gsdi_util_map_uim_enum_to_file_enum(
   uim_items_enum_type uim_file_item ,
   gsdi_file_enum_type *gsdi_file_item_p
 )
{
  int i = 0;

  if (gsdi_file_item_p == NULL) {
    return GSDI_ERROR;
  }

  for ( i = 0; i < GSDI_FILE_ENUM_TABLE_SIZE; i++ ) {
    if (uim_file_item == gsdi_file_enum_table[i].uim_file) {
      *gsdi_file_item_p = gsdi_file_enum_table[i].gsdi_file;
      return GSDI_SUCCESS;
    }
  }
  return GSDI_ERROR;
}/* gsdi_util_map_uim_enum_to_file_enum */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION: gsdi_util_map_file_enum_to_gsdi_service_type

DESCRIPTION
  Maps MMGSDI File Enum to GSDI Service

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gsdi_returns_T gsdi_util_map_file_enum_to_gsdi_service_type(
    gsdi_file_enum_type gsdi_file_item ,
    gsdi_services_type *gsdi_services_item_p)
{
  int i = 0;

  if (gsdi_services_item_p == NULL)
  {
    return GSDI_ERROR;
  }

  /* Initialize the gsdi_services_item_p */
  *gsdi_services_item_p = GSDI_NONE;

  for ( i = 0; i < GSDI_FILE_SERVICE_TABLE_SIZE; i++ ) {
    if (gsdi_file_item == gsdi_file_service_table[i].gsdi_file) {
      *gsdi_services_item_p = gsdi_file_service_table[i].gsdi_service;
      return GSDI_SUCCESS;
    }
  }
  return GSDI_NOT_FOUND;
}/* gsdi_util_map_file_enum_to_gsdi_service_type */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION: gsdi_util_get_pin

DESCRIPTION
  Get PIN information for all applications from GSDI cache.

INPUT
  - Application Id
  - Pin Id
  - Pointer to pin_info

RETURN VALUE
  none

OUTPUT
  - Pin_info for particular a application are populated with data from the
    GSDI table.

===========================================================================*/
gsdi_returns_T gsdi_util_get_pin(
    gsdi_app_id_type *app_id,
    gsdi_pin_ids_T    pin_id,
    gsdi_pin_info_T  *pin_info
)
{
  uint32 iter = 0;

  if ( pin_info == NULL )
  {
    return GSDI_ERROR;
  }

  if(pin_id == GSDI_PIN1)
  {
    pin_info->pin_id = GSDI_USIM_APP1_PIN1;
  }
  else if (pin_id == GSDI_PIN2)
  {
    pin_info->pin_id = GSDI_USIM_APP1_PIN2;
  }
  else if(pin_id == GSDI_USIM_UNIV_PIN)
  {
    return GSDI_ERROR;
  }

  while(iter < GSDI_MAX_APP_COUNT)
  {
    if(pin_id == gsdi_app_pin_info_table[iter].pin1.pin_id)
    {
      if(gsdi_app_pin_info_table[iter].pin1.status)
      {
        memcpy(pin_info->pin,gsdi_app_pin_info_table[iter].pin1.pin,
               GSDI_PIN_LEN);
        pin_info->status = gsdi_app_pin_info_table[iter].pin1.status;
        pin_info->is_pin_available = gsdi_app_pin_info_table[iter].pin1.is_pin_available;
        return GSDI_SUCCESS;
      }
      else
      {
        return GSDI_ERROR;
      }
    }
    else if(pin_id == gsdi_app_pin_info_table[iter].pin2.pin_id)
    {
      if(gsdi_app_pin_info_table[iter].pin2.status)
      {
        memcpy(pin_info->pin,gsdi_app_pin_info_table[iter].pin2.pin,
               GSDI_PIN_LEN);
        pin_info->status = gsdi_app_pin_info_table[iter].pin2.status;
        pin_info->is_pin_available = gsdi_app_pin_info_table[iter].pin2.is_pin_available;
        return GSDI_SUCCESS;
      }
      else
      {
        return GSDI_ERROR;
      }
    }/* else if(pin_info->pin_id == gsdi_app_pin_info_table[iter].pin2->pin_id)  */

    iter++;

  }/* end of while(iter < GSDI_MAX_APP_COUNT) */
  return GSDI_ERROR;
}/* gsdi_returns_T gsdi_util_get_pin */ /*lint !e715 */

/*===========================================================================
FUNCTION GSDI_UTIL_WRITE_COMMON_CACHE

DESCRIPTION
  Function to read EF information from the common cache table

INPUT
  uim_items_enum_type  sim_filename,         - EF- FileName
  boolean              pref_slot,            - TRUE/FALSE
  byte                 data_len,             - Length of Data to be written
  byte                 data_offset,          - Offset to start writing data from
  byte *               data_p                - Data

OUTPUT
  None

RETURN VALUE
  gsdi_returns_T - GSDI_SUCCESS
                 - GSDI_ERROR
===========================================================================*/
gsdi_returns_T gsdi_util_write_common_cache (
  uim_items_enum_type       sim_filename,
  boolean                   pref_slot,
  byte                      data_len,
  byte                      data_offset,
  byte *                    data_p,
  gsdi_data_from_enum_type  data_fr_card
)
{
    boolean         file_found  = FALSE;
    word            file_num    = 0;

    /* Find requested file in sim cache */
    for ( file_num = 0; file_num < GSDI_NUM_FILES_IN_COMMON_CACHE; file_num++ )
    {
      if ( gsdi_common_cache[file_num].filename == sim_filename )
      {
         if ( bool_equal(gsdi_common_cache[file_num].pref_slot, pref_slot) )
         {
            file_found = TRUE;
            break;
         }
      }
    }

    if (file_num >= GSDI_NUM_FILES_IN_COMMON_CACHE)
    {
        return GSDI_NOT_SUPPORTED;
    }
    /* Is the requested offset and length of offset     */
    /* Handle the special case of updating the */
    /* data len in cache during init when GSDI */
    /* is caching information                  */
    /* During init, GSDI caches whole records. */
    /* partial records or offsets of records   */
    /* are not cached during init              */
    if ( ( !gsdi_data.init_completed ) && ( file_found ) )
    {
        /* Need to first update the Length of the record */
        /* Is this the first time reading the file ?     */
        if ( gsdi_common_cache[file_num].init == FALSE )
        {
            /* OK to update to length returned */
            /* Need to avoid updating to length 0 */
            if ( data_len != 0 )
            {
                gsdi_common_cache[file_num].rec_len = data_len;
                gsdi_common_cache[file_num].total_len = data_len;
                gsdi_common_cache[file_num].add_data = NULL;
            }
        }
    }

    /* Was file found in Cache ? */
    if ( !file_found )
    {
        /* Item is not in Cache */
        return GSDI_NOT_SUPPORTED;
    }
    /* exceeding the amount of space allocated for rec? */
    if ( data_offset + data_len > gsdi_common_cache[file_num].rec_len )
    {
        return GSDI_INCORRECT_PARAMS;
    }
    /* OK to update cache at this point */

    /* Data len of 0 indicates the max number */
    /* of bytes are to be written             */
    if ( data_len == 0 )
    {
      data_len = gsdi_common_cache[file_num].rec_len;
    }
    MMGSDI_RETURN_IF_NULL(data_p);
    /* Copy contents to be written into cache */
    memcpy( (gsdi_common_cache[file_num].body) + data_offset, data_p, data_len );
    gsdi_common_cache[file_num].init = TRUE;
    switch(data_fr_card)
    {
    case GSDI_DATA_FROM_CARD:
        gsdi_common_cache[file_num].in_synch = TRUE;
        break;
    case GSDI_DATA_FROM_CLIENT:
    default:
        gsdi_common_cache[file_num].in_synch = FALSE;
        break;
    }

    return GSDI_SUCCESS;

} /* gsdi_util_write_usim_cache */

/*===========================================================================
FUNCTION GSDI_UTIL_READ_COMMON_CACHE

DESCRIPTION
  Function to read EF information from the common cache table

INPUT
  uim_items_enum_type  sim_filename,         - EF- FileName
  boolean              pref_slot,            - TRUE/FALSE
  byte                 requested_data_len,   - Length of Data Requested
  byte                 data_offset,          - Offset to start reading data from
  byte *               actual_data_len_p,    - Actucal Length of Data returned
  byte *               data_p                - Data

OUTPUT
  Pointers Data_p and actual_data_len_p are  populated

RETURN VALUE
  gsdi_returns_T - GSDI_SUCCESS
                 - GSDI_ERROR
===========================================================================*/
gsdi_returns_T gsdi_util_read_common_cache (
  uim_items_enum_type  sim_filename,
  boolean              pref_slot,
  byte                 requested_data_len,
  byte                 data_offset,
  byte *               actual_data_len_p,
  byte *               data_p
)
{
    boolean         file_found  = FALSE;
    word            file_num    = 0;


    /* Find requested file in sim cache */
    for ( file_num = 0; file_num < GSDI_NUM_FILES_IN_COMMON_CACHE; file_num++ )
    {
      if ( gsdi_common_cache[file_num].filename == sim_filename )
      {
        if ( bool_equal(gsdi_common_cache[file_num].pref_slot, pref_slot) )
        {
          file_found = TRUE;
          break;
        }
      }
    }

    /* Read file */
    if ( file_found == FALSE )
    {
        return GSDI_NOT_SUPPORTED;
    }
    if (file_num >= GSDI_NUM_FILES_IN_COMMON_CACHE)
    {
        return GSDI_NOT_SUPPORTED;
    }
    if ( gsdi_common_cache[file_num].init == FALSE )
    {
        return GSDI_NOT_INIT;
    }
    if ( data_offset + requested_data_len >
                gsdi_common_cache[file_num].rec_len )
    {
        return GSDI_INCORRECT_PARAMS;
    }

    MMGSDI_RETURN_IF_NULL(actual_data_len_p);
    if ( requested_data_len == 0 )
    {
      actual_data_len_p[0] = gsdi_common_cache[file_num].rec_len;
    }
    else
    {
      actual_data_len_p[0] = requested_data_len;
    }

    MMGSDI_RETURN_IF_NULL(data_p);
    (void) memcpy( data_p, (gsdi_common_cache[file_num].body) + data_offset,
              actual_data_len_p[0] );

    return GSDI_SUCCESS;

} /* gsdi_util_read_usim_cache */

/*===========================================================================

FUNCTION GSDI_UTIL_MAP_TO_ECC_TABLE

DESCRIPTION
  - This function will populate a global gsdi_ecc_info_table_type structure.

INPUT
  - gsdi_ef_ecc_data_T * ecc_data_p

DEPENDENCIES
  Dependency on Global Variable gsdi_ef_ecc_data_T ecc_data.  This
  structure must be popultaed before this function can be used.

RETURN VALUE
  gsdi_returns_T.

OUTPUT
  None

===========================================================================*/
gsdi_returns_T gsdi_util_map_to_ecc_table(
  gsdi_ef_ecc_data_T *ecc_data_p
  )
{
  uint32 loop = 0;

  /* Allocate memory for the ecc table pointer if it is NULL */
  if(gsdi_ecc_table_p == NULL)
  {
    gsdi_ecc_table_p = mmgsdi_malloc(sizeof(gsdi_ecc_info_table_type));
    if(gsdi_ecc_table_p == NULL)
    {
      MSG_ERROR("NO HEAP SPACE FOR ECC TABLE POINTER ALLOC", 0, 0, 0);
      return GSDI_ERROR;
    }
    memset(gsdi_ecc_table_p, 0x00, sizeof(gsdi_ecc_info_table_type));
  }/* if(ecc_table_p == NULL) */

  if((gsdi_data.protocol != GSDI_PROTOCOL_GSM) &&
     (gsdi_data.protocol != GSDI_PROTOCOL_UICC)
    )
  {
    MMGSDIUTIL_TMC_MEM_FREE(gsdi_ecc_table_p);
    return GSDI_ERROR;
  }

  MMGSDI_RETURN_IF_NULL(ecc_data_p);
  gsdi_ecc_table_p->num_of_rec = ecc_data_p->num_of_recs;
  if(ecc_data_p->num_of_recs > 0)
  {
    while((loop < ecc_data_p->num_of_recs) && (loop < GSDI_MAX_ECC_RECORDS))
    {
      /* Allocate memory for the new record */
      gsdi_ecc_table_p->ecc_p[loop] = mmgsdi_malloc(sizeof(gsdi_ecc_info_T));
      if(gsdi_ecc_table_p->ecc_p[loop] == NULL)
      {
        MSG_ERROR("NO HEAP SPACE FOR ECC POINTER ALLOC", 0, 0, 0);
        MMGSDIUTIL_TMC_MEM_FREE(gsdi_ecc_table_p);
        return GSDI_ERROR;
      }
      /* Memset the new record pointer */
      memset(gsdi_ecc_table_p->ecc_p[loop], 0xFF, sizeof(gsdi_ecc_info_T));

      /* Perform copy of emergency call code and emergency service category ONLY
          in case of GSM protocol */

      if(gsdi_data.protocol == GSDI_PROTOCOL_GSM)
      {
        /* Copy only the emergency call code and the emergency service category */
        memcpy(gsdi_ecc_table_p->ecc_p[loop]->emer_call_code,
                &(ecc_data_p->data[loop][0]),
                GSDI_ECC_LEN
              );
        memcpy(&(gsdi_ecc_table_p->ecc_p[loop]->emer_srvc_category),
                &(ecc_data_p->data[loop][ecc_data_p->len-1]),
                sizeof(byte)
              );
      }

      /* Perform copy of emergency call code, aplha identifier and emergency service
          category in case of UICC protocol */

      if(gsdi_data.protocol == GSDI_PROTOCOL_UICC)
      {
        /* Copy the emergency call code,the alpha identifier and the emergency service
            category
        */
        memcpy(gsdi_ecc_table_p->ecc_p[loop]->emer_call_code,
                &(ecc_data_p->data[loop][0]),
                GSDI_ECC_LEN
                );
        memcpy(&(gsdi_ecc_table_p->ecc_p[loop]->emer_srvc_category),
                &(ecc_data_p->data[loop][ecc_data_p->len-1]),
                sizeof(byte)
              );
        memcpy(gsdi_ecc_table_p->ecc_p[loop]->alpha_id,
                &(ecc_data_p->data[loop][3]),
                GSDI_ALPHA_ID_LEN
              );
      }
      loop++;
    } /* while(loop <= ecc_data_p->num_of_recs) */
  }/* if(ecc_data_p->num_of_recs > 0) */
  return GSDI_SUCCESS;
}


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_MAP_GSDI_MMGSDI_SLOT

  DESCRIPTION:
    This function map the gsdi slot to mmgsdi slot if find_old == FALSE, and
    map mmgsdi slot to gsdi if find_old == TRUE

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_map_gsdi_mmgsdi_slot(
  boolean find_old,
  gsdi_slot_id_type *slot,
  mmgsdi_slot_id_enum_type* new_slot
)
{
  MMGSDIUTIL_RETURN_IF_NULL(slot);
  MMGSDIUTIL_RETURN_IF_NULL(new_slot);
  if (find_old)
  {
    switch (*new_slot)
    {
    case MMGSDI_SLOT_1:
      *slot = GSDI_SLOT_1;
      return MMGSDI_SUCCESS;
    case MMGSDI_SLOT_2:
      *slot = GSDI_SLOT_2;
      return MMGSDI_SUCCESS;
    case MMGSDI_SLOT_AUTOMATIC:
      *slot = GSDI_SLOT_AUTOMATIC;
      return MMGSDI_SUCCESS;
    case MMGSDI_MAX_SLOT_ID_ENUM:
      *slot = GSDI_SLOT_NONE;
      return MMGSDI_ERROR;
    }
  }
  else
  {
    switch (*slot)
    {
    case GSDI_SLOT_1:
      *new_slot = MMGSDI_SLOT_1;
      return MMGSDI_SUCCESS;
    case GSDI_SLOT_2:
      *new_slot = MMGSDI_SLOT_2;
      return MMGSDI_SUCCESS;
    case GSDI_SLOT_AUTOMATIC:
      *new_slot = MMGSDI_SLOT_AUTOMATIC;
      return MMGSDI_SUCCESS;
    case GSDI_SLOT_NONE:
    case GSDI_SLOT_MAX:
      *new_slot = MMGSDI_MAX_SLOT_ID_ENUM;
      return MMGSDI_ERROR;
    }
  }
  return MMGSDI_ERROR;
}/* mmgsdi_map_gsdi_mmgsdi_slot */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_MAP_GSDI_MMGSDI_EVENT

  DESCRIPTION:
    This function map the gsdi event to mmgsdi event if find_old == FALSE, and
    map mmgsdi event to gsdi if find_old == TRUE

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                            within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                            but the service table indicates the card does not
                            have the support

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_map_gsdi_mmgsdi_event(
  boolean                 find_old,
  mmgsdi_event_data_type *new_evt_info,
  gsdi_sim_events_T      *old_evt)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(new_evt_info);
  MMGSDIUTIL_RETURN_IF_NULL(old_evt);
  if (find_old)
  {
    switch (new_evt_info->evt)
    {
    case MMGSDI_CARD_INSERTED_EVT:
      if (new_evt_info->data.card_inserted.slot == MMGSDI_SLOT_1)
      {
        *old_evt = GSDI_SIM_INSERTED;
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.card_inserted.slot == MMGSDI_SLOT_2)
      {
        *old_evt = GSDI_SIM_INSERTED_2;
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

    case MMGSDI_CARD_ERROR_EVT:
      if (new_evt_info->data.card_error.slot == MMGSDI_SLOT_1)
      {
        #ifdef FEATURE_MMGSDI_CARD_ERROR_INFO

        /* Convert the MMGSDI Card Error Event information
        ** into an old GSDI Event
        */
        mmgsdi_util_convert_to_error_info_event (new_evt_info->data.card_error.slot,
                                                 GSDI_CARD_ERROR,
                                                 gsdi_data.uim_error_condition,
                                                 old_evt);
        #else

        *old_evt = GSDI_CARD_ERROR;

        #endif /* FEATURE_MMGSDI_CARD_ERROR_INFO */

        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.card_error.slot == MMGSDI_SLOT_2)
      {
        #if defined(FEATURE_MMGDSI_DUAL_SLOT) && defined(FEATURE_MMGSDI_CARD_ERROR_INFO)

        /* Convert the MMGSDI Card Error Event information
        ** into an old GSDI Event
        */
        mmgsdi_util_convert_to_error_info_event (new_evt_info->data.card_error.slot,
                                                 GSDI_CARD_ERROR,
                                                 gsdi_data.uim_error_condition_2,
                                                 old_evt);
        #else

        *old_evt = GSDI_CARD_ERROR_2;

        #endif /* FEATURE_MMGSDI_DUAL_SLOT && FEATURE_MMGSDI_CARD_ERROR_INFO */

        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

    case MMGSDI_SESSION_CLOSE_EVT:
      MSG_HIGH("No corresponding session close event in old GSDI event list ", 0, 0, 0);
      return MMGSDI_ERROR;

    case MMGSDI_CARD_REMOVED_EVT:
      if (new_evt_info->data.card_removed.slot == MMGSDI_SLOT_1)
      {
        *old_evt = GSDI_SIM_REMOVED;
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.card_removed.slot == MMGSDI_SLOT_2)
      {
        *old_evt = GSDI_SIM_REMOVED_2;
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

    case MMGSDI_CARD_INIT_COMPLETED_EVT:
      if (new_evt_info->data.card_init_completed.slot == MMGSDI_SLOT_1)
      {
        *old_evt = GSDI_SIM_INIT_COMPLETED;
/*TODO  if (new_evt_info->data.card_init_completed.is_operational_slot)
        else
          *old_evt = GSDI_SIM_INIT_COMPLETED_NO_PROV;*/
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.card_init_completed.slot == MMGSDI_SLOT_2)
      {
        *old_evt = GSDI_SIM_INIT_COMPLETED_2;

/*TODO  if (new_evt_info->data.card_init_completed.is_operational_slot)
          *old_evt = GSDI_SIM_INIT_COMPLETED_2;
        else
          *old_evt = GSDI_SIM_INIT_COMPLETED_NO_PROV_2;*/
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

    case MMGSDI_PIN1_EVT:
      if (new_evt_info->data.pin.slot == MMGSDI_SLOT_1)
      {
        switch (new_evt_info->data.pin.pin_info.status)
        {
        case MMGSDI_PIN_ENABLED_VERIFIED:
          *old_evt = GSDI_PIN1_VERIFIED;
          break;
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
          *old_evt = GSDI_PIN1_ENABLED;
          break;
        case MMGSDI_PIN_DISABLED:
          *old_evt = GSDI_PIN1_DISABLED;
          break;
        case MMGSDI_PIN_BLOCKED:
          *old_evt = GSDI_PIN1_BLOCKED;
          break;
        case MMGSDI_PIN_PERM_BLOCKED:
          *old_evt = GSDI_PIN1_PERM_BLOCKED;
          break;
        case MMGSDI_PIN_CHANGED:
          *old_evt = GSDI_PIN1_CHANGED;
          break;
        case MMGSDI_PIN_UNBLOCKED:
          *old_evt = GSDI_PIN1_UNBLOCKED;
           break;
        default:
          MSG_ERROR("MMGSDI Pin1, slot 1 State is %x",new_evt_info->data.pin.pin_info.status,0,0);
          return MMGSDI_ERROR;
        }
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.pin.slot == MMGSDI_SLOT_2)
      {
        switch (new_evt_info->data.pin.pin_info.status)
        {
        case MMGSDI_PIN_ENABLED_VERIFIED:
          *old_evt = GSDI_PIN1_VERIFIED_2;
          break;
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
          *old_evt = GSDI_PIN1_ENABLED_2;
          break;
        case MMGSDI_PIN_DISABLED:
          *old_evt = GSDI_PIN1_DISABLED_2;
          break;
        case MMGSDI_PIN_BLOCKED:
          *old_evt = GSDI_PIN1_BLOCKED_2;
          break;
        case MMGSDI_PIN_PERM_BLOCKED:
          *old_evt = GSDI_PIN1_PERM_BLOCKED_2;
          break;
        case MMGSDI_PIN_CHANGED:
          *old_evt = GSDI_PIN1_CHANGED_2;
          break;
        case MMGSDI_PIN_UNBLOCKED:
          *old_evt = GSDI_PIN1_UNBLOCKED_2;
           break;
        default:
          MSG_ERROR("MMGSDI Pin1, slot 2 State is %x",new_evt_info->data.pin.pin_info.status,0,0);
          return MMGSDI_ERROR;
        }
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

      case MMGSDI_PIN2_EVT:
      if (new_evt_info->data.pin.slot == MMGSDI_SLOT_1)
      {
        switch (new_evt_info->data.pin.pin_info.status)
        {
        case MMGSDI_PIN_ENABLED_VERIFIED:
          *old_evt = GSDI_PIN2_VERIFIED;
          break;
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
          *old_evt = GSDI_PIN2_ENABLED;
          break;
        case MMGSDI_PIN_DISABLED:
          *old_evt = GSDI_PIN2_DISABLED;
          break;
        case MMGSDI_PIN_BLOCKED:
          *old_evt = GSDI_PIN2_BLOCKED;
          break;
        case MMGSDI_PIN_PERM_BLOCKED:
          *old_evt = GSDI_PIN2_PERM_BLOCKED;
          break;
        case MMGSDI_PIN_CHANGED:
          *old_evt = GSDI_PIN2_CHANGED;
          break;
        case MMGSDI_PIN_UNBLOCKED:
          *old_evt = GSDI_PIN2_UNBLOCKED;
           break;
        default:
          MSG_ERROR("MMGSDI Pin2, slot 2 State is %x",new_evt_info->data.pin.pin_info.status,0,0);
          return MMGSDI_ERROR;
        }
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.pin.slot == MMGSDI_SLOT_2)
      {
        switch (new_evt_info->data.pin.pin_info.status)
        {
        case MMGSDI_PIN_ENABLED_VERIFIED:
          *old_evt = GSDI_PIN2_VERIFIED_2;
          break;
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
          *old_evt = GSDI_PIN2_ENABLED_2;
          break;
        case MMGSDI_PIN_DISABLED:
          *old_evt = GSDI_PIN2_DISABLED_2;
          break;
        case MMGSDI_PIN_BLOCKED:
          *old_evt = GSDI_PIN2_BLOCKED_2;
          break;
        case MMGSDI_PIN_PERM_BLOCKED:
          *old_evt = GSDI_PIN2_PERM_BLOCKED_2;
          break;
        case MMGSDI_PIN_CHANGED:
          *old_evt = GSDI_PIN2_CHANGED_2;
          break;
        case MMGSDI_PIN_UNBLOCKED:
          *old_evt = GSDI_PIN2_UNBLOCKED_2;
          break;
        default:
          MSG_ERROR("MMGSDI Pin2, slot 2 State is %x",new_evt_info->data.pin.pin_info.status,0,0);
          return MMGSDI_ERROR;
        }
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;
      /* Verified, Blocked, Unblocked, Enabled, Disabled, Changed*/

    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
      if (new_evt_info->data.terminal_profile.slot == MMGSDI_SLOT_1)
      {
        if (new_evt_info->data.terminal_profile.ok_dl)
          *old_evt = GSDI_OK_FOR_TERMINAL_PROFILE_DL;
        else
          *old_evt = GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL;
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.terminal_profile.slot == MMGSDI_SLOT_2)
      {
        if (new_evt_info->data.terminal_profile.ok_dl)
          *old_evt = GSDI_OK_FOR_TERMINAL_PROFILE_DL_2;
        else
          *old_evt = GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2;
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

    case MMGSDI_REFRESH_EVT:
      if (new_evt_info->data.refresh.slot == MMGSDI_SLOT_1)
      {
        if (new_evt_info->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_FAILED)
        {
          *old_evt = GSDI_REFRESH_FAILED;
          return MMGSDI_SUCCESS;
        }
        else if (new_evt_info->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
        {
          switch (new_evt_info->data.refresh.mode)
          {
          case MMGSDI_REFRESH_RESET:
          case MMGSDI_REFRESH_RESET_AUTO:
            *old_evt = GSDI_REFRESH_SIM_RESET;
            break;
          case MMGSDI_REFRESH_NAA_INIT:
          case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
            *old_evt = GSDI_REFRESH_SIM_INIT;
            break;
          case MMGSDI_REFRESH_NAA_INIT_FCN:
            *old_evt = GSDI_REFRESH_SIM_INIT_FCN;
            break;
          case MMGSDI_REFRESH_NAA_FCN:
            *old_evt = GSDI_REFRESH_FCN;
            break;
          case MMGSDI_REFRESH_NAA_APP_RESET:
            *old_evt = GSDI_REFRESH_APP_RESET;
            break;
          case MMGSDI_REFRESH_3G_SESSION_RESET:
            *old_evt = GSDI_REFRESH_3G_SESSION_RESET;
            break;
          default:
            return MMGSDI_ERROR;
          }
          return MMGSDI_SUCCESS;
        }
      }
      else if (new_evt_info->data.terminal_profile.slot == MMGSDI_SLOT_2)
      {
        if (new_evt_info->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_FAILED)
        {
          *old_evt = GSDI_REFRESH_FAILED;
          return MMGSDI_SUCCESS;
        }
        else if (new_evt_info->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
        {
          switch (new_evt_info->data.refresh.mode)
          {
          case MMGSDI_REFRESH_RESET:
            *old_evt = GSDI_REFRESH_SIM_RESET_2;
            break;
          case MMGSDI_REFRESH_NAA_INIT:
          case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
            *old_evt = GSDI_REFRESH_SIM_INIT_2;
            break;
          case MMGSDI_REFRESH_NAA_INIT_FCN:
            *old_evt = GSDI_REFRESH_SIM_INIT_FCN_2;
            break;
          case MMGSDI_REFRESH_NAA_FCN:
            *old_evt = GSDI_REFRESH_FCN_2;
            break;
          case MMGSDI_REFRESH_NAA_APP_RESET:
            *old_evt = GSDI_REFRESH_APP_RESET_2;
            break;
          case MMGSDI_REFRESH_3G_SESSION_RESET:
            *old_evt = GSDI_REFRESH_3G_SESSION_RESET_2;
            break;
          default:
            return MMGSDI_ERROR;
          }
          return MMGSDI_SUCCESS;
        }
      }
      return MMGSDI_ERROR;

    case MMGSDI_FDN_EVT:
      if (new_evt_info->data.fdn.slot == MMGSDI_SLOT_1)
      {
        if (new_evt_info->data.fdn.enabled)
          *old_evt = GSDI_FDN_ENABLE;
        else
          *old_evt = GSDI_FDN_DISABLE;
        return MMGSDI_SUCCESS;
      }
      else if (new_evt_info->data.fdn.slot == MMGSDI_SLOT_2)
      {
        if (new_evt_info->data.fdn.enabled)
          *old_evt = GSDI_FDN_ENABLE_2;
        else
          *old_evt = GSDI_FDN_DISABLE_2;
        return MMGSDI_SUCCESS;
      }
      return MMGSDI_ERROR;

    case MMGSDI_SWITCH_SLOT_EVT:
      if (new_evt_info->data.switch_slot.start)
        *old_evt = GSDI_START_SWITCH_SLOT;
      else
        *old_evt = GSDI_FINISH_SWITCH_SLOT;
      return MMGSDI_SUCCESS;

    case MMGSDI_ILLEGAL_CARD_EVT:
      if (new_evt_info->data.illegal_card.slot == MMGSDI_SLOT_1)
        *old_evt = GSDI_SIM_ILLEGAL;
      else
        *old_evt = GSDI_SIM_ILLEGAL_2;
      return MMGSDI_SUCCESS;

    case MMGSDI_SELECT_AID_EVT:
      if (new_evt_info->data.select_aid.slot == MMGSDI_SLOT_1)
        *old_evt = GSDI_APP_SELECTED;
      else
        *old_evt = GSDI_APP_SELECTED_2;
      return MMGSDI_SUCCESS;

    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_SAP_DISCONNECT_EVT:
    case MMGSDI_SAP_PIN_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
      *old_evt = GSDI_MAX_SIM_EVENTS;
      return MMGSDI_ERROR;
    case MMGSDI_MAX_EVT_ENUM:
      *old_evt = GSDI_MAX_SIM_EVENTS;
      return MMGSDI_SUCCESS;
    }
  }
  else
  {
    switch (*old_evt)
    {
    case GSDI_SIM_INSERTED:
      mmgsdi_status = mmgsdi_util_build_card_inserted_evt(MMGSDI_SLOT_1, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_REMOVED:
      mmgsdi_status = mmgsdi_util_build_card_removed_evt(MMGSDI_SLOT_1, new_evt_info);
      return mmgsdi_status;

    case GSDI_DRIVER_ERROR:
    case GSDI_CARD_ERROR:
    case GSDI_NO_SIM:
    case GSDI_CARD_ERR_POLL_ERROR:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE:
      mmgsdi_status = mmgsdi_util_build_card_error_evt(MMGSDI_SLOT_1, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_INIT_COMPLETED:
      mmgsdi_status = mmgsdi_util_build_card_init_completed_evt(MMGSDI_SLOT_1,new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
      mmgsdi_status = mmgsdi_util_build_card_init_completed_evt(MMGSDI_SLOT_1,new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_VERIFIED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_ENABLED_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_UNBLOCKED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_UNBLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_BLOCKED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_ENABLED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_ENABLED_NOT_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_DISABLED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_DISABLED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_CHANGED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_CHANGED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_PERM_BLOCKED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN1, MMGSDI_PIN_PERM_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_VERIFIED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_ENABLED_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_UNBLOCKED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_UNBLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_BLOCKED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_ENABLED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_ENABLED_NOT_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_DISABLED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_DISABLED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_CHANGED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_CHANGED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_PERM_BLOCKED:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_1, MMGSDI_PIN2, MMGSDI_PIN_PERM_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_APP_SELECTED:
      mmgsdi_status = mmgsdi_util_build_select_aid_evt(MMGSDI_SLOT_1, new_evt_info);
      return mmgsdi_status;

    case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
      mmgsdi_status = mmgsdi_util_build_tp_dl_evt(MMGSDI_SLOT_1, TRUE, new_evt_info);
      return mmgsdi_status;

    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL:
      mmgsdi_status = mmgsdi_util_build_tp_dl_evt(MMGSDI_SLOT_1, FALSE, new_evt_info);
      return mmgsdi_status;

    case GSDI_FDN_ENABLE:
      mmgsdi_status = mmgsdi_util_build_fdn_evt(MMGSDI_SLOT_1, TRUE, new_evt_info);
      return mmgsdi_status;

    case GSDI_FDN_DISABLE:
      mmgsdi_status = mmgsdi_util_build_fdn_evt(MMGSDI_SLOT_1, FALSE, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_INSERTED_2:
      mmgsdi_status = mmgsdi_util_build_card_inserted_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_REMOVED_2:
      mmgsdi_status = mmgsdi_util_build_card_removed_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_CARD_ERROR_2:
    case GSDI_CARD_ERR_POLL_ERROR_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2:
    case GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2:
    case GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2:
    case GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2:
    case GSDI_CARD_ERR_VOLTAGE_MISMATCH_2:
    case GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2:
    case GSDI_CARD_ERR_MAXED_PARITY_ERROR_2:
    case GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2:
    case GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2:
    case GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2:
    case GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2:
    case GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2:
    case GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2:
    case GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2:
      mmgsdi_status = mmgsdi_util_build_card_error_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_INIT_COMPLETED_2:
      mmgsdi_status = mmgsdi_util_build_card_init_completed_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_INIT_COMPLETED_NO_PROV_2:
      mmgsdi_status = mmgsdi_util_build_card_init_completed_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_VERIFIED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_ENABLED_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_UNBLOCKED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_UNBLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_BLOCKED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_ENABLED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_ENABLED_NOT_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_DISABLED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_DISABLED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_CHANGED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_CHANGED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN1_PERM_BLOCKED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN1, MMGSDI_PIN_PERM_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_VERIFIED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_UNBLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_UNBLOCKED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_ENABLED_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_BLOCKED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_ENABLED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_ENABLED_NOT_VERIFIED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_DISABLED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_DISABLED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_CHANGED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_CHANGED, new_evt_info);
      return mmgsdi_status;

    case GSDI_PIN2_PERM_BLOCKED_2:
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_generic_data.client_id,
                                                MMGSDI_SLOT_2, MMGSDI_PIN2, MMGSDI_PIN_PERM_BLOCKED, new_evt_info);
      return mmgsdi_status;

    case GSDI_APP_SELECTED_2:
      mmgsdi_status = mmgsdi_util_build_select_aid_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_FDN_ENABLE_2:
      mmgsdi_status = mmgsdi_util_build_fdn_evt(MMGSDI_SLOT_2, TRUE, new_evt_info);
      return mmgsdi_status;

    case GSDI_FDN_DISABLE_2:
      mmgsdi_status = mmgsdi_util_build_fdn_evt(MMGSDI_SLOT_2, FALSE, new_evt_info);
      return mmgsdi_status;

    case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
      mmgsdi_status = mmgsdi_util_build_tp_dl_evt(MMGSDI_SLOT_2, TRUE, new_evt_info);
      return mmgsdi_status;

    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2:
      mmgsdi_status = mmgsdi_util_build_tp_dl_evt(MMGSDI_SLOT_2, TRUE, new_evt_info);
      return mmgsdi_status;

    case GSDI_START_SWITCH_SLOT:
      mmgsdi_status = mmgsdi_util_build_switch_slot_evt(TRUE, new_evt_info);
      return mmgsdi_status;

    case GSDI_FINISH_SWITCH_SLOT:
      mmgsdi_status = mmgsdi_util_build_switch_slot_evt(FALSE, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_ILLEGAL:
      mmgsdi_status = mmgsdi_util_build_illegal_card_evt(MMGSDI_SLOT_1, new_evt_info);
      return mmgsdi_status;

    case GSDI_SIM_ILLEGAL_2:
      mmgsdi_status = mmgsdi_util_build_illegal_card_evt(MMGSDI_SLOT_2, new_evt_info);
      return mmgsdi_status;

    case GSDI_REFRESH_SIM_RESET:
    case GSDI_REFRESH_SIM_INIT:
    case GSDI_REFRESH_SIM_INIT_FCN:
    case GSDI_REFRESH_FCN:
    case GSDI_REFRESH_APP_RESET:
    case GSDI_REFRESH_3G_SESSION_RESET:
    case GSDI_REFRESH_SIM_RESET_2:
    case GSDI_REFRESH_SIM_INIT_2:
    case GSDI_REFRESH_SIM_INIT_FCN_2:
    case GSDI_REFRESH_FCN_2:
    case GSDI_REFRESH_APP_RESET_2:
    case GSDI_REFRESH_3G_SESSION_RESET_2:
    case GSDI_REFRESH_FAILED:
    case GSDI_MEMORY_WARNING:
    case GSDI_NO_SIM_EVENT:
    case GSDI_NO_EVENT:
    case GSDI_MEMORY_WARNING_2:
    case GSDI_NO_SIM_EVENT_2:
    case GSDI_PERSO_INIT_COMPLETED:
    case GSDI_PERSO_EVENT_GEN_PROP1:
    case GSDI_SIM_UHZI_V2_COMPLETE:
    case GSDI_SIM_UHZI_V1_COMPLETE:
    case GSDI_PERSO_EVENT_GEN_PROP2:
    case GSDI_INTERNAL_SIM_RESET:
    case GSDI_MAX_SIM_EVENTS:
      new_evt_info->evt = MMGSDI_MAX_EVT_ENUM;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
}/* mmgsdi_map_gsdi_mmgsdi_event */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ----------------------------------------------------------------------------
  FUNCTION:      GSDI_UTIL_IS_SAME_CACHE_SYNCH_DATA

  DESCRIPTION:
    This function checks if the cached data is the same as the data passed
    in and whether this data is synchronous with the data in the card.

  DEPENDENCIES:

  LIMITATIONS:


  RETURN VALUE:
    gsdi_returns_T: GSDI_SUCCESS, GSDI_ERROR, GSDI_NOT_FOUND
    same_data_ptr will also be populated to indicate if the data passed it is
    the same as the one in the cache or not

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
gsdi_returns_T gsdi_util_is_same_cache_synch_data (
  gsdi_cache_enum_type  cache_type,
  uim_items_enum_type   sim_filename,
  boolean               pref_slot,
  int32                 data_len,
  byte                  data_offset,
  byte *                data_p,
  boolean *             same_data_ptr
)
{
  sim_cache_element_T *cache_ptr   = NULL;
  int                  cache_size  = 0;
  boolean              file_found  = FALSE;
  word                 file_num    = 0;
  gsdi_returns_T       gsdi_status = GSDI_SUCCESS;

  if (same_data_ptr == NULL)
    return GSDI_ERROR;

  switch (cache_type)
  {
  case GSDI_CACHE_COMMON:
    cache_ptr = gsdi_common_cache;
    cache_size = GSDI_NUM_FILES_IN_COMMON_CACHE;
    break;

  case GSDI_CACHE_GSM:
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
    break;

  case GSDI_CACHE_UMTS:
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
    break;

  case GSDI_CACHE_CDMA:
#ifdef FEATURE_MMGSDI_CDMA
    cache_ptr = gsdi_ruim_cache;
    cache_size = GSDI_NUM_FILES_IN_RUIM_CACHE;
#endif /* #ifdef FEATURE_MMGSDI_CDMA */
    break;

  case GSDI_CACHE_MAX:
    MSG_ERROR("Incorrect Cache Type, no need to update", 0, 0, 0);
    return GSDI_ERROR;
  }

  if (cache_ptr == NULL)
  {
    MSG_ERROR("Null Cache Ptr", 0, 0, 0);
    return GSDI_ERROR;
  }

  /* Find requested file in sim cache */
  for ( file_num = 0; file_num < cache_size; file_num++ )
  {
    if ( cache_ptr[file_num].filename == sim_filename )
    {
      if ( bool_equal(cache_ptr[file_num].pref_slot, pref_slot) )
      {
        file_found = TRUE;
        break;
      }
    }
  }

  if (file_found == FALSE)
  {
    /* No file found */
    MSG_HIGH("File 0x%x not in Cache", sim_filename, 0, 0);
    return GSDI_NOT_FOUND;
  }

  if (file_num >= cache_size)
  {
    /* No file found */
    MSG_HIGH("File 0x%x not in Cache", sim_filename, 0, 0);
    return GSDI_NOT_FOUND;
  }
  if ( cache_ptr[file_num].init == FALSE )
  {
    /* Hasn't been initialized yet, data not the same */
    MSG_MED("File 0x%x not init", sim_filename, 0, 0);
    *same_data_ptr = FALSE;
    return gsdi_status;
  }

  if(cache_ptr[file_num].total_len < (data_len + data_offset))
  {
    /* Cache len smaller, definitely not same data */
    MSG_MED("File 0x%x total len 0x%x < data_len+data_offset 0x%x, 0x%x",
      sim_filename, cache_ptr[file_num].total_len, (data_len + data_offset));
    *same_data_ptr = FALSE;
    return gsdi_status;
  }

  /* compare the content of the data */
  if (data_offset >= GSDI_CACHE_MAX_FILE_LEN)
  {
    /* Trying to write to the additional data portion of the cache */

    /* If additional data is NULL => no data to compare, the data to be written
        is therefore different from the data in the cache */
    if (cache_ptr[file_num].add_data == NULL)
    {
      *same_data_ptr = FALSE;
      return gsdi_status;
    }

    MMGSDI_RETURN_IF_NULL(data_p);
    if(memcmp(&cache_ptr[file_num].add_data[data_offset-GSDI_CACHE_MAX_FILE_LEN],
      data_p, (uint32)data_len))
    {
      /* Data not the same */
      *same_data_ptr = FALSE;
      return gsdi_status;
    }

    /* Data the same */
    if (cache_ptr[file_num].in_synch)
    {
      /* Already in synch, no need to write to cache and SIM */
      *same_data_ptr = TRUE;
    }
    else
    {
      *same_data_ptr = FALSE;
    }
    return gsdi_status;
  }
  else
  {
    MMGSDI_RETURN_IF_NULL(data_p);
    /* data offset starts within the body */
    if ((data_len + data_offset) < GSDI_CACHE_MAX_FILE_LEN)
    {
      /* the entire data lies within the body portion of the cache */
      if (memcmp(&cache_ptr[file_num].body[data_offset],
          data_p, (uint32)data_len))
      {
        *same_data_ptr = FALSE;
        return gsdi_status;
      }

      /* data the same */
      if (cache_ptr[file_num].in_synch)
      {
        /* Already in synch, no need to write to cache and SIM */
        *same_data_ptr = TRUE;
      }
      else
      {
        *same_data_ptr = FALSE;
      }
      return gsdi_status;
    }
    else
    {
      MMGSDI_RETURN_IF_NULL(data_p);
      /* part of the data in the body, and part in the add_data */
      /* the entire data lies within the body portion of the cache */
      if(data_offset < GSDI_CACHE_MAX_FILE_LEN)
      {
        if (memcmp(&cache_ptr[file_num].body[data_offset],
                   data_p, (GSDI_CACHE_MAX_FILE_LEN-data_offset)))
        {
          *same_data_ptr = FALSE;
          return gsdi_status;
        }
      }
      else
      {
        MSG_ERROR("data offset greater than/equal to max file len", 0, 0, 0);
        *same_data_ptr = FALSE;
        return GSDI_ERROR;
      }
      /* compare the rest of the data */
      if (cache_ptr[file_num].add_data == NULL)
      {
        *same_data_ptr = FALSE;
        return gsdi_status;
      }

      if (memcmp(cache_ptr[file_num].add_data,
          &data_p[GSDI_CACHE_MAX_FILE_LEN-data_offset],
          int32touint32(((data_len-GSDI_CACHE_MAX_FILE_LEN)+data_offset))))
      {
        *same_data_ptr = FALSE;
        return gsdi_status;
      }

      /* data the same */
      if (cache_ptr[file_num].in_synch)
      {
        /* Already in synch, no need to write to cache and SIM */
        *same_data_ptr = TRUE;
      }
      else
      {
        *same_data_ptr = FALSE;
      }
      return gsdi_status;
    }
  }
} /* gsdi_util_is_same_cache_synch_data */

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
/*===========================================================================
FUNCTION gsdi_set_pin_status

DESCRIPTION
  Set the pin status depending on the current status.

INPUT
  int32 *pin_status : Current Pin Status
  int32 pin_set_to : PIN Status to overwrite or Or the current PIN Status to

OUTPUT
  void

RETURN VALUE
  void
===========================================================================*/
static void gsdi_set_pin_status(
  int32 *pin_status_ptr,
  int32 pin_set_to)
{
  /* If the Current PIN status is unknown then overwrite it with the PIN status
     sent in.  If the status is always or never do nothing else if neither of
     the 3 condition or the current status with that sent in*/
  if (*pin_status_ptr == MMGSDI_PIN_STATUS_UNKNOWN)
  {
    *pin_status_ptr = pin_set_to;
  }
  else if (*pin_status_ptr == MMGSDI_PIN_STATUS_ALWAYS  ||
           *pin_status_ptr == MMGSDI_PIN_STATUS_NEVER)
  {
    return;
  }
  else
  {
    *pin_status_ptr |= pin_set_to;
  }
  return;
}
/*===========================================================================
FUNCTION gsdi_populate_file_attr_cache

INPUT
  byte pin_status_input,
  gsdi_pin_status_info_type *pin_status_output_ptr

OUTPUT
  void

DESCRIPTION

RETURN VALUE
  void
===========================================================================*/
void gsdi_populate_file_attr_cache(
  byte pin_status_input,
  gsdi_pin_status_info_type *pin_status_output_ptr)
{
  if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)
       == GSDI_USIM_APP_MASK)
  {
    switch (pin_status_input)
    {
    case PIN_APPL_1:
      gsdi_set_pin_status(&pin_status_output_ptr->and_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN1);
      gsdi_set_pin_status(&pin_status_output_ptr->or_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN1);
      break;
    case SEC_PIN_APPL_1:
      gsdi_set_pin_status(&pin_status_output_ptr->and_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN2);
      gsdi_set_pin_status(&pin_status_output_ptr->or_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN2);
      break;
    case KEYREF_ALWAYS:
      pin_status_output_ptr->and_pin_status = MMGSDI_PIN_STATUS_ALWAYS;
      pin_status_output_ptr->or_pin_status = MMGSDI_PIN_STATUS_ALWAYS;
      break;
    default:
      /* becomes never, this includes: NEVER, ADMs */
      pin_status_output_ptr->and_pin_status = MMGSDI_PIN_STATUS_NEVER;
      pin_status_output_ptr->or_pin_status = MMGSDI_PIN_STATUS_NEVER;
      break;
    }
    return;
  }

if (((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
      == GSDI_GSM_SIM_MASK) ||
      ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
      == GSDI_RUIM_APP_MASK))
  {
    switch (pin_status_input)
    {
    case CHV1:
      gsdi_set_pin_status(&pin_status_output_ptr->and_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN1);
      gsdi_set_pin_status(&pin_status_output_ptr->or_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN1);
      break;
    case CHV2:
      gsdi_set_pin_status(&pin_status_output_ptr->and_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN2);
      gsdi_set_pin_status(&pin_status_output_ptr->or_pin_status,
                          MMGSDI_PIN_STATUS_APP1_PIN2);
      break;
    case CHV_NONE:
      pin_status_output_ptr->and_pin_status = MMGSDI_PIN_STATUS_ALWAYS;
      pin_status_output_ptr->or_pin_status = MMGSDI_PIN_STATUS_ALWAYS;
      break;
    default:
      /* becomes never, this includes: NEVER, ADMs */
      pin_status_output_ptr->and_pin_status = MMGSDI_PIN_STATUS_NEVER;
      pin_status_output_ptr->or_pin_status = MMGSDI_PIN_STATUS_NEVER;
      break;
    }
    return;
  }
}

/*===========================================================================
FUNCTION gsdi_get_cached_attributes

INPUT
       uim_items_enum_type        item
OUTPUT
       gsdi_attr_cache*           attr

DESCRIPTION

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_attr_cache *  gsdi_get_cached_attributes( uim_items_enum_type item)
{
   gsdi_attr_cache        * cache_attr      = NULL;
   uint32                   table_index     = ((uint32)item >> 8) & 0x0F;
   uint32                   item_index      = ((uint32)item) & 0xFF;


   switch(table_index)
   {
#ifdef FEATURE_MMGSDI_CDMA
     case UIM_RUIM_EF_START:
       if (item_index >= MMGSDI_CDMA_DF_NUM_FILE_ENUM)
       {
         MSG_ERROR("Invalid Index for RUIM_EF 0x%x", item_index, 0, 0);
         return NULL;
       }
       cache_attr = &gsdi_ruim_cache_tbl[item_index];
       break;
#endif
#if defined ( FEATURE_MMGSDI_GSM ) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
     case UIM_EF_UNDER_MF_START:
       if (item_index >= MMGSDI_MF_NUM_FILE_ENUM)
       {
         MSG_ERROR("Invalid Index for MF_EF 0x%x", item_index, 0, 0);
         return NULL;
       }
       cache_attr = &gsdi_mf_cache_tbl[item_index];
       break;
     case UIM_TELECOM_EF_START:
       if (item_index >= MMGSDI_TELECOM_DF_NUM_FILE_ENUM)
       {
         MSG_ERROR("Invalid Index for TELECOM_EF 0x%x", item_index, 0, 0);
         return NULL;
       }
       cache_attr = &gsdi_telecom_cache_tbl[item_index];
       break;
     case UIM_EF_PATH_START:
       if (item_index >= MMGSDI_EF_BY_PATH_NUM_FILE_ENUM)
       {
         MSG_ERROR("Invalid Index for BY_PATH_EF 0x%x", item_index, 0, 0);
         return NULL;
       }
       cache_attr = &gsdi_ef_cache_tbl[item_index];
       break;
     default:
       return NULL;
   }

   if (cache_attr->file_name != item)
   {
     MSG_ERROR("item request 0x%x != item in cache 0x%x", item, cache_attr->file_name, 0);
     return NULL;
   }

   return cache_attr;
}

/*===========================================================================
FUNCTION gsdi_check_pin_status

INPUT
  int32 pin_status
OUTPUT
  None

DESCRIPTION

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static gsdi_returns_T gsdi_check_pin_status(
  int32 pin_status,
  boolean and_operation
)
{
  boolean                   error_occured          = FALSE;
  boolean                   additional_error_occur = FALSE;
  boolean                   can_proceed            = FALSE;
  mmgsdi_int_app_info_type *sel_app_ptr            = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();

  MMGSDI_RETURN_IF_NULL(sel_app_ptr);

  if (pin_status == MMGSDI_PIN_STATUS_ALWAYS)
    return GSDI_SUCCESS;

  if (pin_status == MMGSDI_PIN_STATUS_NEVER)
  {
    MSG_HIGH("Pin status is MMGSDI_PIN_STATUS_NEVER",0,0,0);
    return GSDI_ERROR;
  }

  if (pin_status == MMGSDI_PIN_STATUS_UNKNOWN)
    return GSDI_SUCCESS;

  if ((pin_status & MMGSDI_PIN_STATUS_APP1_PIN1)
         ==  MMGSDI_PIN_STATUS_APP1_PIN1 )
  {
    MSG_HIGH("EF is PIN1 protected",0,0,0);
    can_proceed = mmgsdi_util_can_proceed_to_post_pin1(MMGSDI_SLOT_1);
    if( can_proceed == FALSE)
    {
        MSG_ERROR("APP1 PIN1 blocked or not verified", 0, 0, 0);
        error_occured = TRUE;
    }
  }

  /* if an AND operation, and error occurs, no need to continue, this is not accessible */
  if (and_operation && error_occured)
  {
    MSG_HIGH("and_operation: 0X%x error_occured: 0X%x",
             and_operation,error_occured,0);
    return GSDI_ERROR;
  }

  /* if it comes here, it means no error so far */

  additional_error_occur = FALSE;
  if ((pin_status & MMGSDI_PIN_STATUS_APP1_PIN2)
       ==  MMGSDI_PIN_STATUS_APP1_PIN2 )
  {
    MSG_HIGH("EF is PIN2 protected",0,0,0);
    if (sel_app_ptr->pin2.status == MMGSDI_PIN_BLOCKED ||
         sel_app_ptr->pin2.status == MMGSDI_PIN_PERM_BLOCKED)
    {
        MSG_HIGH("APP1 PIN2 block", 0, 0, 0);
        additional_error_occur = TRUE;
    }
  }

  if (!additional_error_occur)
  {
    return GSDI_SUCCESS;
  }

  /* there is additional error */
  if (and_operation)
  {
    MSG_HIGH("and_operation: 0X%x",and_operation,0,0);
    return GSDI_ERROR;
  }
  else /* or operation */
  {
    if (error_occured)
      return GSDI_SUCCESS;
    else
      return GSDI_ERROR;
  }
}

/*===========================================================================
FUNCTION gsdi_check_access_condition

INPUT
    gsdi_pin_status_info_type *  key_ref_value

DESCRIPTION

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static gsdi_returns_T gsdi_check_access_condition(
    gsdi_pin_status_info_type *  key_ref_value_p)
{
    gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;

    if (key_ref_value_p == NULL)
      return GSDI_ERROR;

    if (((key_ref_value_p->and_pin_status & MMGSDI_PIN_STATUS_UNKNOWN)
         == MMGSDI_PIN_STATUS_UNKNOWN ) &&
        ((key_ref_value_p->or_pin_status & MMGSDI_PIN_STATUS_UNKNOWN)
         == MMGSDI_PIN_STATUS_UNKNOWN ))
    {
      MSG_ERROR("Unknown PIN Status when not expected", 0, 0, 0);
      return GSDI_ERROR;
    }
    if ((key_ref_value_p->and_pin_status == MMGSDI_PIN_STATUS_ALWAYS)
         &&
        (key_ref_value_p->or_pin_status == MMGSDI_PIN_STATUS_ALWAYS ))
    {
      MSG_HIGH("Always allow access", 0, 0, 0);
      return GSDI_SUCCESS;
    }
    if ((key_ref_value_p->and_pin_status == MMGSDI_PIN_STATUS_NEVER)
          &&
        (key_ref_value_p->or_pin_status == MMGSDI_PIN_STATUS_NEVER ))
    {
      MSG_HIGH("Never allow access", 0, 0, 0);
      return GSDI_ERROR;
    }
    /* Check the And status */
    gsdi_status = gsdi_check_pin_status(key_ref_value_p->and_pin_status, TRUE);
    if (gsdi_status != GSDI_SUCCESS)
      return gsdi_status;

    /* Check the Or status */
    gsdi_status = gsdi_check_pin_status(key_ref_value_p->or_pin_status, FALSE);
    return gsdi_status;

}


/*===========================================================================
FUNCTION GSDI_EF_ACCESS_CONDITIONS_CHECK

INPUT
    uim_items_enum_type  filename :
    byte                 msg_id   :

DESCRIPTION

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_ef_access_conditions_check(uim_items_enum_type filename,
                                               byte                msg_id
)
{
  gsdi_attr_cache           * file_attributes_ptr  = NULL;
  gsdi_returns_T              gsdi_status          = GSDI_SUCCESS;
  gsdi_pin_status_info_type * access_req_type_ptr  = NULL;

  /* ------------------------------------------------------------------------
     Check the file_attributes_ptr.  If this is NULL, this means that the
     item to be checked is not maintained.  Therefore, allow it to go to
     the card.
     ----------------------------------------------------------------------*/
  file_attributes_ptr = gsdi_get_cached_attributes(filename);

  if ( file_attributes_ptr == NULL )
  {
    MSG_HIGH("ITEM: 0x%x - Not in Cache", filename,0,0);
    return GSDI_SUCCESS;
  }

  /* -------------------------------------------------------------------------
     If any of the PIN Status members in the cache indicate an UNKNOWN,
     we must return a SUCCESS to allow Task to populate the cache further
     in case it hasn't been populated.  The Task will enforce the PIN Status
     ------------------------------------------------------------------------*/
  if((((file_attributes_ptr->read.and_pin_status     == MMGSDI_PIN_STATUS_UNKNOWN)
    && (file_attributes_ptr->read.or_pin_status      == MMGSDI_PIN_STATUS_UNKNOWN)) ||
      ((file_attributes_ptr->write.and_pin_status    == MMGSDI_PIN_STATUS_UNKNOWN)
    &&  file_attributes_ptr->write.or_pin_status     == MMGSDI_PIN_STATUS_UNKNOWN)) ||
      ((file_attributes_ptr->increase.and_pin_status == MMGSDI_PIN_STATUS_UNKNOWN)
    && (file_attributes_ptr->increase.or_pin_status  == MMGSDI_PIN_STATUS_UNKNOWN)))
  {
    MSG_HIGH("ITEM: 0x%x Han an unknown PIN Status",filename,0,0);
    return GSDI_SUCCESS;
  }

  /* -------------------------------------------------------------------------
     Switch on the msg_id and check the respective access PIN Status
     indication.
     -----------------------------------------------------------------------*/
  switch(msg_id)
  {
    case GSDI_SIM_READ_REQ:
      access_req_type_ptr = &file_attributes_ptr->read;
      break;

    case GSDI_SIM_WRITE_REQ:
      access_req_type_ptr = &file_attributes_ptr->write;
      break;

    case GSDI_SIM_INCREASE_REQ:
      access_req_type_ptr = &file_attributes_ptr->increase;
      if (file_attributes_ptr->cyclic_increase_allowed != TRUE)
      {
        MSG_ERROR("NOT ALLOWED INCREASE",0,0,0);
        return GSDI_ERROR;
      }
      break;

    default:
        MSG_ERROR("UNKNOWN REQUEST",0,0,0);
        return GSDI_ERROR;
  }

  gsdi_status = gsdi_check_access_condition(access_req_type_ptr);

  return gsdi_status;
}
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/


#ifdef FEATURE_USIM_1000_PBM
/*===========================================================================

FUNCTION GSDI_UTIL_ASSIGN_ANR_TO_ADN

INPUT
    int *  anr_count_ptr       : count of ANR assigned to ADN
    int    pb_global_flag   : flag to indicate local or global phonebook

DESCRIPTION
  This function helps to assign a base ANR file for an ADN in a single
  phonebook record in the PBR file.


DEPENDENCIES
  None


RETURN VALUE
  uim_items_enum_type - returns the corresponding ANR file name
  In case of error, returns UIM_NO_SUCH_ITEM
===========================================================================*/
uim_items_enum_type gsdi_util_assign_anr_to_adn (
  int *anr_count_ptr,
  int pb_global_flag
)
{
  uim_items_enum_type filename = UIM_NO_SUCH_ITEM;

  if(anr_count_ptr == NULL)
  {
    MSG_ERROR("anr_count_ptr NULL ptr",0,0,0);
    return (filename);
  }

  if(pb_global_flag == 0)
  {
    switch(*anr_count_ptr)
    {
    case 1:
      filename = UIM_USIM_ANR;
      break;
    case 2:
      filename = UIM_USIM_ANRA;
      break;
    case 3:
      filename = UIM_USIM_ANRB;
      break;
    case 4:
      filename = UIM_USIM_ANRC;
      break;
    default:
      MSG_HIGH("No ANR file for Record 0x%x",*anr_count_ptr,0,0);
      break;
    }
  }
  else
  {
    switch(*anr_count_ptr)
    {
    case 1:
      filename = UIM_TELECOM_ANR;
      break;
    case 2:
      filename = UIM_TELECOM_ANRA;
      break;
    case 3:
      filename = UIM_TELECOM_ANRB;
      break;
    case 4:
      filename = UIM_TELECOM_ANRC;
      break;
    default:
      MSG_HIGH("No ANR file for Record 0x%x",*anr_count_ptr,0,0);
      break;
    }
  }

  if(filename != UIM_NO_SUCH_ITEM)
  {
    *anr_count_ptr += 1;
  }
  return (filename);
}


/*===========================================================================

FUNCTION GSDI_UTIL_GET_PHONEBOOK_FILENAME_FROM_TAG

INPUT
    int    pb_global_flag   : flag to indicate local or global phonebook
    byte   ef_tag              : EF Tag from PBR record.
    int *  anr_count_ptr       : count of ANR assigned to ADN

DESCRIPTION
  This function helps to assign a base Phonebook filename from the tag value
  specified in the record of EF PBR


DEPENDENCIES
  None


RETURN VALUE
  uim_items_enum_type - returns the corresponding Phonebook file name
  In case of error, returns UIM_NO_SUCH_ITEM
===========================================================================*/
uim_items_enum_type gsdi_util_get_phonebook_filename_from_tag (
  byte pb_global_flag,
  byte ef_tag,
  int  *anr_count_ptr
)
{
    uim_items_enum_type filename = UIM_NO_SUCH_ITEM;
    if (pb_global_flag == 0)
    {
      /* USIM ADF EFs */
      switch(ef_tag)
      {
      case 0xC0:
        filename = UIM_USIM_ADN;
        break;
      case 0xC1:
        filename = UIM_USIM_IAP;
        break;
      case 0xC2:
        filename = UIM_USIM_EXT1;
        break;
      case 0xC3:
        filename = UIM_USIM_SNE;
        break;
      case 0xC4:
        filename = gsdi_util_assign_anr_to_adn(anr_count_ptr, pb_global_flag);
        break;
      case 0xC5:
        filename = UIM_USIM_PBC;
        break;
      case 0xC6:
        filename = UIM_USIM_GRP;
        break;
      case 0xC7:
        /* Type 3 Files:  Can be shared between Entries */
        filename = UIM_USIM_AAS;
        break;
      case 0xC8:
        filename = UIM_USIM_GAS;
        break;
      case 0xC9:
        filename = UIM_USIM_UID;
        break;
      case 0xCA:
        filename = UIM_USIM_EMAIL;
        break;
      case 0xCB:
        filename = UIM_USIM_CCP1;
        break;
      default:
        filename = UIM_NO_SUCH_ITEM;
        MSG_ERROR(" -- ERROR:  Filename can't be determined",0,0,0);
        break;
      }
    }
    else
    {
      /* TELECOM EFs */
      switch(ef_tag) {
      case 0xC0:
        filename = UIM_TELECOM_ADN;
        break;
      case 0xC1:
        filename = UIM_TELECOM_IAP;
        break;
      case 0xC2:
        filename = UIM_TELECOM_EXT1;
        break;
      case 0xC3:
        filename = UIM_TELECOM_SNE;
        break;
      case 0xC4:
        filename = gsdi_util_assign_anr_to_adn(anr_count_ptr, pb_global_flag);
        break;
      case 0xC5:
        filename = UIM_TELECOM_PBC;
        break;
      case 0xC6:
        filename = UIM_TELECOM_GRP;
        break;
      case 0xC7:
        /* Type 3 Files:  Can be shared between Entries */
        filename = UIM_TELECOM_AAS;
        break;
      case 0xC8:
        filename = UIM_TELECOM_GAS;
        break;
      case 0xC9:
        filename = UIM_TELECOM_UID;
        break;
      case 0xCA:
        filename = UIM_TELECOM_EMAIL;
        break;
      case 0xCB:
        filename = UIM_TELECOM_CCP1;
        break;
      default:
        filename = UIM_NO_SUCH_ITEM;
        MSG_ERROR(" -- WRITE:  Filename can't be determined %x",filename,0,0);
        break;
      }
    }
    return (filename);
}
#endif /*FEATURE_USIM_1000_PBM*/


/*===========================================================================
FUNCTION GSDI_UTIL_IS_FILE_FROM_DO_NOT_CLEAR_CACHE_LIST

INPUT
    uim_items_enum_type  filename    : filename
    gsdi_technology_type tech_type   : technology type

LIMITATION
    The files passed to this function should be files that are present in the
    sim / usim cache that needs to be checked against the "do not clear cache"
    list.
DESCRIPTION

RETURN VALUE
  boolean TRUE  - file should not be cleared from the cache
          FALSE - file can be cleared from the cache.
===========================================================================*/
boolean gsdi_util_is_file_from_do_not_clear_cache_list(
  uim_items_enum_type  uim_filename,
  gsdi_technology_type tech_type)
{
  /* If invalid filename or tech type is provided , FALSE will be returned */

  if(tech_type == GSDI_GSM)
  {
    switch (uim_filename)
    {
    case UIM_GSM_HPLMN:
    case UIM_GSM_FPLMN:
    case UIM_GSM_HPLMNACT:
    case UIM_GSM_PLMNWACT:
    case UIM_GSM_OPLMNWACT:
      return TRUE;
    default:
      MSG_HIGH("filename 0x%x not in do-not-clear cache list",uim_filename,0,0);
      return FALSE;
    }
  }
  else if(tech_type == GSDI_UMTS )
  {
    switch (uim_filename)
    {
    case UIM_USIM_HPLMN:
    case UIM_USIM_FPLMN:
    case UIM_USIM_HPLMNWACT:
    case UIM_USIM_RPLMNACT:
    case UIM_USIM_PLMNWACT:
    case UIM_USIM_OPLMNWACT:
    case UIM_USIM_START_HFN:
    case UIM_USIM_THRESHOLD:
    case UIM_USIM_EHPLMN:
      return TRUE;
    default:
      MSG_HIGH("filename 0x%x not in do-not-clear cache list",uim_filename,0,0);
      return FALSE;
    }
  }
  MSG_HIGH("unhandled tech_type -- 0x%x",tech_type,0,0);
  return FALSE;
}/* gsdi_util_is_file_from_do_not_clear_cache_list */
