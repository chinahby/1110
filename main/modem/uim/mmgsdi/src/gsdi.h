#ifndef GSDI_H
#define GSDI_H
/*===========================================================================


            G S D I   T A S K   D E F I N I T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2010 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/mmgsdi/src/gsdi.h#2 $ $DateTime: 2011/03/09 23:17:08 $

when       who     what, where, why
-------------------------------------------------------------------------------
06/07/10   ssr     Setting NVRUIM control flags inside of 
                   gsdi_cdma_post_pin1_init
06/08/09   js      Added prototype for gsdi_cdma_proc_select
06/03/09   xz      Extend GSDI_WAIT_FOR_GSTK_TO_TP_DL value
06/03/09   yb      Changing GSDI_WAIT_FOR_GSTK_TO_TP_DL timer value for win mob to
                   2 minutes 1 sec
04/07/09   xz      Change TP wait time with one extra sec
03/13/09   yb      Made changes for Strip/Pack information
03/13/09   yb      Fixed value of GSDI_WAIT_FOR_GSTK_TO_TP_DL when feature
                   FEATURE_GSTK_TEST_AUTOMATION or FEATURE_GSTK_STATE_TESTER are
                   defined.
03/04/09   mib     Get perso keys if FEATURE_MMGSDI_PERSO_GET_KEY not defined
02/24/09   kk      Updated Secrnd timer name
02/13/09   ssr     Removing compilation warning for
                   gsdi_util_clear_file_from_cache function.
02/12/09   kk      Add files to refresh file list for re-reading during FCN
02/05/09   jar     Moved Perso SEC RND Timer from gsdi_perso_sec.h to here
01/22/09   sun     Removal of Old GSDI refresh timer and addition of new
                   GSDI_PERSO_SECRND_READY_SIG sig.
12/01/08   ssr     Handled error condition for some MMGSDI Functions
10/14/08   xz      Move FEATURE_ENHANCED_NW_SELECTION into
                   gsdi_is_ens_enabled_in_nv()
10/06/08   jk      Support GSM SIM Access in 1X only mode
10/01/08   sk      Increased Terminal Profile download wait time if
                   FEATURE_ESTK is turned on
09/24/08   xz      Add gsdi_is_ens_enabled_in_nv()
06/23/08   sp      Add additional function parameter to gsdi_util_preinit_gsdi()
                   gsdi_util_selective_global_data_init().
05/01/08   nk      Updated featurization to include "nvruimi.h"
04/30/08   kk      Added support for MMS EFs
03/26/08   kk      Buffer allocation for select response made dynamic
03/20/08   nk      Added OMH EF Support
03/14/08   sk      Reverted the changes for the usage of signals during
                   sim initialization
02/26/08   js      Support for updating GPRS Anite NV by reading IMSI
01/08/08   kk      Removed the Usage of signals during sim initialization
12/18/07   kk      Fixed compilation warnings, redefinition of NULL_PTR
11/19/07   js      Radio Access Technology Balancing support
11/15/07   sun     Added support for NAA Refresh
11/29/07   sk      Moved gsdi_get_protocol() to gsdi_exp.h
11/15/07   tml     Remove extra client registration signal
11/08/07   vs/nk   Added EUIMID and OMH feature support
10/18/07   sk      Fixed flag for multiple pb support
10/17/07   sun     Pulled the critical section change back in
10/17/07   sun     Backed out critical section change
10/11/07   sun     Fixed usage of global signals
09/23/07   sk      Put a lock around mmgsdi_task_rsp
06/25/07   sp      Function to delete the path_lookup_table
05/30/07   sp      Add function to build status command for app termination
05/21/07   jk      Change sanity timer value to 5 minutes
05/07/07   sp      Removed gsdi macros for mem allocation/free
05/01/07   sun     Fixed Compile Warning
04/12/07   sun     Featurized Phonebook
03/26/07   sun     Added flag for additional files read during refresh
03/15/07   sk      Added new function to check files that should not be cleared
                   from the cache list during FCN.
02/21/07   sun     Remove GSDI Global PIN
02/27/07   jk      Added featurization for Memory Reduction
02/19/07   tml     Increased GSTK TP wait time for WIN Mobile to match the
                   value in GSTK due to the long power up time in WINCE
02/09/07   sk      Added support for 1000 phonebook entries.
01/23/07   sp      Fix compiler error
01/17/07   sp      Removed pin_just_verified flag
01/18/07   sun     Fixed services type for CBMI
12/04/06   sp      Added refresh_fcn_data to gsdi_data.gstk_data
10/25/06   tml     Added initial support for Universal PIN replacement support
10/02/06   sp      Moved me support for fdn to mmgsdi global data.
10/05/06   sun     Fixed Signals. 0x10000000 is reserved for any module that
                   uses SEC. Backing out previous change.
09/05/06   pv      Change return type for gsdi_util_decode_umts_file_header
                   to return gsdi_returns_T
08/25/06   jar     Support for FEATURE_CARD_ERROR_INFO by adding a uim error
                   condition for slot 2.
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/09/06   sun     Added perso event to gsdi_data
08/16/06   jar     Increased GSDI_NUM_FILES_IN_LOOKUP_TABLE to 56 to add handling
                   of UIM_USIM_ANRC and UIM_USIM_ANRC1 in the PB Path lookup table.
07/26/06   sun     Added prototypes for gsdi_is_perso_completed and
                   gsdi_get_card_mode_capability_usim
07/5/06    jar     Increased GSDI_NUM_FILES_IN_LOOKUP_TABLE to 54 to make room
                   for TELECOM ANRC1 and TELECOM ANRC2.
06/12/06   pv      Separate MMGSDI_SYNCH_UIM_REPORT_SIG AND
                   GSDI_UIM_REPORT_SIG.  Add booleans to track recovery status.
06/12/06   tml     fixed compilation issue
06/02/06   jar     Increased GSDI_FILE_SERVICE_TABLE_SIZE by 2 to allow the
                   additions of MMGSDI_GSM_ACMMAX and MMGSDI_GSM_PUCT
05/25/06   jar     Added function prototype gsdi_util_need_to_take_down_call_stack()
                   because it is now called in gsdi.c
05/09/06   tml     Refresh cleanup and compilation fix
05/03/06   sp      Added support to send pin_status evt after sim_insert.
03/21/06   tml     added cfis and missing service table support and lint
03/11/06   tml     compilation fix
03/10/06   jar     Modified Featurization
03/09/06   tml     Clear UIM Get Response cache when refresh
03/08/06   jar     Merged in Branched HZI Support
02/21/06   sp      Updating GSDI_FILE_SERVICE_TABLE_SIZE for EHPLMN
02/16/06   sp      Support for EF_EHPLMN
02/14/06   nk      Merge for: Bring file attribute setting to avoid synchronous
                   file attribute access in the library function that could have
                   caused potential deadlock situation and ARR Caching merge and
                   get fle attribute caching additional support
01/26/06   tml     Fixed get EF cache length not returning more than 256 byte
                   issue
12/14/05   tml     Moved mmgsdi related commands to mmgsdi.h
11/22/05   jk      Changes to support multiprocessor RPC
11/14/05    pv     change signature for functions that needs lookup
                   to return gsdi_returns_T and accept an output parameter
10/28/05   sun     Featurized support for cache file attributes
10/20/05   sun     Added support to cache file attributes
10/14/05   tml     Added utility function for checking if data to be
                   written same as cached data
10/18/05   tml     Added get protocol function
10/13/05   sun     Increased Command Queue Size
09/23/05   sun     Removed unused #define
09/14/05   wli     Fixed compilation error for feature
                   FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN
08/26/05   sun     Added support for BT SAP
08/02/05   wli     Added HOT SWAP feature.
07/29/05   sun     Fixed TST Length for USIM
07/26/05   sst     Added MMGSDI_RETURN_FALSE_IF_OUT_OF_RANGE to fix lint error
07/18/05   sun     Added support for USIM files in ENS
07/12/05   tml     Session Management and ISIM support
06/17/05   jar     Added Activate ONCHIP SIM Support
                   Merged in #define GSDI_APDU_DATA_MIN_LEN 4 from branch.
02/25/05   sk      Added 256+ support
05/16/05   tml     fixed USIM file service check
05/08/05   tml     Remove GSM default channel support
05/11/05   sst     Lint fixes
04/30/05   jar     Fix to avoid Multiple Terminal Profile Downloads
04/28/05   jar     Added New GSDI SIM Lock Architecutre to support Plug In
                   Proprietary Security Libraries driven by a Perso Engine.
04/04/05   tml     Added GSM open channel supports
03/16/05   wli     Added 820B EFs
03/08/05   jar     Added rec_num to param list for function:
                   gsdi_umts_populate_fid_lookup_table
02/28/05   sun     Added EF Mapping for Cingular
02/15/05    pv     added additional parameter to gsdi_uim_server_sim_reset
                   to indicate the protocol for uim to start with
02/08/05   sun     Added support for Cingular
02/04/05   jar     Added task_stopped boolean to gsdi_data to indicate
                   when not to send commands to the UIM.
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/21/05   sun     Added support for PNN and OPL
12/27/04   jar     Added support for GSDI_GET_PBR_TABLE_REQ
                   Merged in Branched Channel fixes
11/16/04   jar     Removed Feature GSTK Off Dependencies
11/01/04   pv      Added sanity_timer field to gsdi_data structure
10/18/04   sk      Added macro GSDI_CACHE_ECC_LEN
                   Added definition for cuntions gsdi_util_map_to_ecc()
                   and gsdi_util_get_ecc().
10/18/04   sk      Added prototypes for functions gsdi_uim_server_cdma_seek
                   and gsdi_cdma_proc_ruim_search.
10/07/04   tml     added CBMID cache
09/30/04   sk      Added support for ELP Caching
09/29/04   sk      Added prototypes for function gsdi_uim_notify_reset() and
                   gsdi_add_to_app_pin_info_table()
09/28/04   tml     Change definition of the parsing usim pb function
09/23/04   jar     Added CPHS Global Data Structure to GSDI_DATA
09/03/04   jar     Orange ALS Support
09/02/04   jar     Added file enum support
09/01/04   tml     Ensure event is being sent out to client once only
06/24/04  jar/tml  Addition of UIM - GSDI Interface Sanity Timer
                   Addition of Personalization Cmds
06/16/04   tml     Added Language Preference caching length
06/08/04   jar     Added FEATURE_MMGSDI_PERSONALIZATION_SUPPORT
06/02/04   tml     Added unit test trigger for GSTK
05/18/04   jar     Added Channel Management Support
05/05/04   jar     Increased GSDI_NUM_FILES_IN_USIM_CACHE by 1 to include
                   UIM_USIM_AD.
04/23/04   jar     Added support for GSDI_GET_ATR_REQ and buffering of the
                   ATR.
04/13/04   jar     Created function prototype to set cache ready
                   gsdi_util_set_cache_ready().  Lint Fixes
04/12/04   tml     Added Unit test for Increase, Get SIM Capability, Search
                   and Select
03/31/04   tml     Added Read/Write/Get File Attribute and Select Unit API Test
03/28/04   tml     Merged from branch
03/27/04   tml     Added cache_ready_2
03/18/04   jar     Added additional utility function prototypes.
03/12/04   jar     Added support for Independent SIM Intialization for Dual Slot
                   targets.
03/01/04   tml     Added rec num accessed support for write
02/04/04 jar/tml   Added just_invalidated_2 and just_rehabilitated_2 for dual slots.
01/25/04   tml     Refresh dual slot enhancement
01/24/04   tml     Refresh dual slot support
01/15/04   tml     Allowed GSDI to sent the last SIM event to client if
                   client event registration is too slow
12/20/03   tml     Added FDN support for RUIM
12/18/03  jar/tml  Allowed Get Sim capability to go to global data immediately
                   if there is no need to synch up the global data.
11/05/03   jar     Added Support for Dual Slot, Pref Slot Change
10/30/03   tml     Compilation fix for CDMA only target
10/20/03   jar     Added support for SEND APDU under FEATURE_MMGSDI_TEST_
                   AUTOMATION
10/07/03   jar     Increased number of GSDI Clients allowed to register for
                   clients (GSDI_MAX_REGISTERED_TASK_CBS 6 -> 10 )
08/29/03   jar     Added gsdi_uim_slot_error_condition under FEATURE_MMGSDI_
                   DUAL_SLOT
08/07/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT Support
06/25/03   jar     Added #defines to Support ARR Selection.
06/22/03   jar     Externalized gsdi_crit_sect as part of proper Critical Section
                   handling fix.
05/19/03   jar     Fixed Lint Errors
05/14/03   jar     Added additional support for FEATURE_UIM_RUIM_W_GSM_ACCESS
05/01/03   jar     Added support for REFRESH FCN Modes
04/22/03   tml     Added get image supports
04/13/03   tml     Added UIM_USIM_KCGPRS cache length and increase the number
                   of files in the usim cache table
04/10/03   jar     Added function prototypes for:
                   gsdi_get_available_card_applications,
                   gsdi_gsm_is_icc_gsm_df_present,
                   gsdi_cdma_is_icc_dcs1800_df_present,
                   gsdi_umts_is_uicc_usim_adf_present,
                   gsdi_cdma_is_icc_cdma_df_present
03/21/03   tml     Changed GSDI_GSTK_WAIT_TP_DL_SIG to GSDI_GSTK_WAIT_TP_DL_SIG
                   _SUCCESS and added GSDI_GSTK_WAIT_TP_DL_SIG_FAIL
02/28/03   jar     Added Feature GSTK around Toolkit specific function prototypes.
02/18/03   jar     Added messages to handle different SIM REFRESH Modes.
                   Added #defines for GSDI_WAIT_FOR_FCN_ACKS.
                   Added data type gsdi_gstk_data_type to save refresh information.
                   Added gstk_data to global data structure gsdi_data.
                   Added function prototypes for gsdi_util_notify_modules,
                   gsdi_util_convert_paths, gsdi_util_delete_usim_cache, gsdi_util_
                   preinit_gsdi, gsdi_util_delete_ruim_cache.
02/11/03   jar     Modified Timer SIGs to support fix made in gsdi_wait_for_tasks
                   to_register to ensure we don't hang waiting on a task to register.
                   Removed function prototype for gsdi_wait_for_tasks_to_register.
02/04/03   jar     Added GSTK Support.  Moved virtuim function prototype for
                   virtuim_get_ef_from_path to virtuim.h
01/24/03   jar     Added support to Cache RUIM Lang Pref and Emergency Call Codes.
                   Added gsdi_util_get_ruim_ef_cache_length prototype.
01/16/03   jar     Externed SIM/USIM/RUIM Cache.  Increased cache by 1.
01/07/03   jar     Fixed Featurization problem for LTK RUIM Support.
11/25/02   jar     Removed RTRE Code.  Renamed gsdi_cdma_proc_get_sim_capabilities
                   to gsdi_cdma_proc_get_ruim_capabilities.  Added #defines for
                   RUIM support.
11/14/02   jar     Added enum BY_PATH to gsdi_item_category_T
11/07/02   jar     Added Boolean cache_ready.
10/14/02   jar     Added a #include nv.h to add first instruction class item
                   to gsdi_data global data.
                   Chaged GSDI_NV_SIG from 0x2000 to 0x10000 to avoid
                   conflicts with TASK_STOP_SIG.
10/14/02   tml     1x, gsm merge
10/07/02   jar     Added function gsdi_cache_pin_status() function to be
                   executed before the GSDI_SIM_INSERTED Event is sent to
                   registered clients.  Add pin1_status, pin2_status and
                   pin_status_cached to the pin_status_info_T structure
09/24/02   tml     Added disable_chv1_allowed in gsdi_data struct
09/17/02   tml     Added gsdi_util_get_sim_ef_cache_length and
                   gsdi_util_get_usim_ef_cache_length
09/14/02   jar     Changed GSDI_RTRE to MMGSDI_RTRE per RTRE Integration changes.
09/12/02   tml     Added rec_len parameter to gsdi_populate_fid_lookup_table()
08/30/02   tml     Replace gs_status_T with gsdi_returns_T
08/21/02   jar     Added Service Table Offset and Masks for CDMA UTK.  Define the
                   GSDI_DCS1800_DF, define a GSDI_NV_SIG.  Added functions:
                   gsdi_uim_server_store_esn, gsdi_uim_server_terminal_profile,
                   gsdi_cdma_test_functions, gsdi_nvruim_set_df_present.
                   Added additional featurization.
08/16/02   jar     Added UIM_USIM_FPLMN to USIMCache
08/12/02   tml     Added resp enum type
08/11/02   jar     Added functions gsdi_util_get_usim_ef_cache_length and
                   gsdi_util_get_sim_ef_cache_length to be called
                   by gsdi_get_ef_length library function to return cached EF
                   length. (Optimization)
07/28/02   jar     Added functions prototypes for:  gsdi_cm_call_cmd_err_cb
                   and gsdi_cm_call_event_cb to handle CM Events which then
                   notify the UIM to POLL during a Call.
07/24/02   tml     Added Select api
07/16/02   jar     Added UIM_GSM_AD, UIM_GSM_PLMN, UIM_GSM_PLMNWACT,
                   UIM_GSM_OPLMNWACT, UIM_GSM_HPLMNACT, UIM_USIM_KC,
                   UIM_USIM_PLMNWACT, UIM_USIM_HPLMNWACT, UIM_USIM_OPLMNWACT,
                   UIM_USIM_RPLMNWACT to cache.
                   Modified GSDI_NUM_FILES_IN_USIM_CACHE to 22 and GSDI_NUM_FILES
                   _IN_SIM_CACHE to 22.
                   Added addtional SST Offsets and Masks for SST Check.
07/15/02   jar     Added UIM_USIM_START_HFN, UIM_USIM_PSLOCI, UIM_USIM_THRESHOLD
                   UIM_USIM_START_HFN and UIM_USIM_KC to usim cache.
                   Modified GSDI_NUM_FILES_IN_USIM_CACHE to 17.
07/13/02   jar     Added #defines for the rest of the Services in the UST
                   (Services 42 - 55).
05/21/02   jar     Added the gsdi_uim_server_usim_status function prototype.
                   Added the gsdi_session_control_T type and added it as a
                   member to the gsdi_data structure.
05/19/02   jar     Modified signal values for GSDI_CALLBACK_FUNC_REG_SIGNAL -
                   GSDI_CALLBACK_FUNC_REG_SIGNAL_5.
                   Now only support callbacks from 6 Tasks instead of 10.
04/04/02   jar     Added gsdi_data.init_completed boolean.
04/02/02   jar     Added gsdi_wait_for_tasks_to_register prototype.  Added
                   new defines for callback signals.
03/23/02   jar     Added #defines to support parsing the ATR.  Added the
                   function prototype gsdi_util_parse_atr.  Added a new
                   datatype gsdi_atr_results_T.
03/02/02   jar     Added #define GSDI_MAX_REGISTERED_TASK_CBS   10
                   Made gsdi_event_callback_funct_type *gsdi_callback_ptr an
                   array of callbacks when FEATURE_GSDI_MULTICLIENT_SUPPORT
                   is defined. Added function prototypes for:
                   gsdi_proc_handle_callback_request,gsdi_util_send_sim_events
                   _to_reg_tasks,gsdi_insert_cb_into_cb_array.  All changes need
                   feature define FEATUER_GSDI_MULTICLIENT_SUPPORT.
02/10/02   jar     Moved gsdi_cmd to gsdi_exp.h.
02/03/02   jar     Added Phonebook Support Functionality.  It's featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.  Added Virtual UIM Server
                   functions for searching and resolving requests with PATHS.
11/20/01   jar     Added a function prototype for the UIM to use as a method
                   of determining what SIM/USIM is used in the way of a protocol
10/23/01   jc      Add param to gsdi_util_send_gsdi_cnf signature.
10/11/01   jc      Code cleanup
09/13/01   jc      Add length param to usim authenticate proc.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim.h"
#include "memheap.h"
#include "nv.h"
#include "mmgsdi.h"

#include "gsdi_exp.h"

#ifdef FEATURE_OEMOMH
#define GSDI_SST_MSG_3GPD_OFFSET 3
#define GSDI_SST_MSG_3GPD_MASK 0x30 // 0011 0000
#define GSDI_SST_BREW_OFFSET 5
#define GSDI_SST_BREW_MASK 0x30 // 0011 0000
// Add more services if needed
typedef struct
{
uint16 omh_enabled:1; // 3GPD extension
uint16 rc_enabled:1; // Root Certificates
uint16 browser_enabled:1; // Browser
uint16 brew_enabled:1; // BREW
} gsdi_uim_omh_cap_type;
#endif

#ifdef FEATURE_OEMOMH
extern gsdi_uim_omh_cap_type gsdi_uim_omh_cap;
#endif


#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)*/

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#include "gsdi_perso.h"
#else
#include "gsdi_perso_engine.h"
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PEROSNALIZATION */

#if defined (FEATURE_UIM_EUIMID) || defined (FEATURE_RUIM_CDMA_REFRESH)
#include "nvruimi.h"
#endif /*defined (FEATURE_UIM_EUIMID) || defined (FEATURE_RUIM_CDMA_REFRESH)*/
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define GSDI_CACHE_LP_LEN           GSDI_LP_LEN
#define GSDI_CACHE_ICC_ID_LEN       GSDI_ICC_IC_LEN
#define GSDI_CACHE_IMSI_LEN         GSDI_IMSI_LEN
#define GSDI_CACHE_KC_LEN           (GSDI_KC_LEN + 1) /* allow space for seqno*/
#define GSDI_CACHE_HPLMN_SP_LEN     GSDI_HPLMN_SP_LEN
#define GSDI_CACHE_ACM_MAX_LEN      GSDI_ACM_MAX_LEN
#define GSDI_CACHE_SST_LEN          15
#define GSDI_CACHE_ACM_LEN          GSDI_ACM_LEN
#define GSDI_CACHE_SPN_LEN          GSDI_SPN_LEN
#define GSDI_CACHE_PUCT_LEN         GSDI_PUCT_LEN
#define GSDI_CACHE_BCCH_LEN         GSDI_BCCH_LEN
#define GSDI_CACHE_ACC_LEN          GSDI_ACC_LEN
#define GSDI_CACHE_FPLMN_LEN        GSDI_FPLMN_LEN
#define GSDI_CACHE_LOCI_LEN         GSDI_LOCI_LEN
#define GSDI_CACHE_PHASE_LEN        GSDI_PHASE_LEN
#define GSDI_CACHE_EMLPP_LEN        GSDI_EMLPP_LEN
#define GSDI_CACHE_AAEM_LEN         GSDI_AAEM_LEN
#define GSDI_CACHE_KCGPRS_LEN       GSDI_KCGPRS_LEN
#define GSDI_CACHE_LOCIGPRS_LEN     GSDI_LOCIGPRS_LEN
#define GSDI_CACHE_AD_LEN           GSDI_AD_LEN
#define GSDI_CACHE_PLMN_LEN         GSDI_PLMN_LEN
#define GSDI_CACHE_PLMNWACT_LEN     GSDI_PLMNWACT_LEN
#define GSDI_CACHE_OPLMNWACT_LEN    GSDI_OPLMNWACT_LEN
#define GSDI_CACHE_HPLMNWACT_LEN    GSDI_HPLMNWACT_LEN
#define GSDI_CACHE_GSM_CBMID_LEN    255
#define GSDI_CACHE_ECC_LEN          11
#ifdef FEATURE_MMGSDI_DUAL_SLOT
#define GSDI_NUM_FILES_IN_SIM_CACHE   26
#else
#define GSDI_NUM_FILES_IN_SIM_CACHE   25
#endif /* FEATURE_MMGSDI_DUAL_SLOT*/


#if defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
#define GSDI_CACHE_CDMA_CST_LEN         GSDI_CST_LEN
#define GSDI_CACHE_CDMA_IMSI_M_LEN      GSDI_IMSI_M_LEN
#define GSDI_CACHE_CDMA_IMSI_T_LEN      GSDI_IMSI_T_LEN
#define GSDI_CACHE_CDMA_PREF_LANG_LEN   GSDI_LANG_PREF_LEN
#define GSDI_CACHE_CDMA_ECC_LEN         GSDI_CDMA_ECC_LEN
#ifdef FEATURE_MMGSDI_DUAL_SLOT
#define GSDI_NUM_FILES_IN_RUIM_CACHE    7
#else
#define GSDI_NUM_FILES_IN_RUIM_CACHE    5
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
#ifndef FEATURE_MMGSDI_GSM /* CDMA is defined but not GSM */
#ifndef NULL_PTR
#define NULL_PTR 0x0000
#endif
#endif /* !FEATURE_MMGSDI_GSM */
#endif /* FEEATURE_GSDI_CDMA */

#define GSDI_CACHE_MAX_FILE_LEN       255


#define GSDI_NUM_SUBDIRECTORY_FILES   10
#define GSDI_CACHE_DEFAULT_FILE_LEN 0
#define GSDI_DEFAULT_FILE_HEADER_LEN 80

#define GSDI_NUM_FILES_IN_COMMON_CACHE    1

#define GSDI_CMD_QUEUE_MAX_SIZE 40

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_CDMA)
#define GSDI_SST_CHV1_DISABLE_OFFSET    0
#define GSDI_SST_ADN_OFFSET             0
#define GSDI_SST_FDN_OFFSET             0
#define GSDI_SST_SMS_OFFSET             0
#define GSDI_SST_ACM_OFFSET             1
#define GSDI_SST_CCP_OFFSET             1
#define GSDI_SST_PLMN_SEL_OFFSET        1
#define GSDI_SST_MSISDN_OFFSET          2
#define GSDI_SST_EXT1_OFFSET            2
#define GSDI_SST_EXT2_OFFSET            2
#define GSDI_SST_SMSP_OFFSET            2
#define GSDI_SST_LND_OFFSET             3
#define GSDI_SST_CBMI_OFFSET            3
#define GSDI_SST_GID1_OFFSET            3
#define GSDI_SST_GID2_OFFSET            3
#define GSDI_SST_SPN_OFFSET             4
#define GSDI_SST_SDN_OFFSET             4
#define GSDI_SST_EXT3_OFFSET            4
#define GSDI_SST_VGCS_GID_LIST_OFFSET   5
#define GSDI_SST_VBS_GID_LIST_OFFSET    5
#define GSDI_SST_eMLPP_OFFSET           5
#define GSDI_SST_AAeMLPP_OFFSET         5
#define GSDI_SST_DATA_DL_SMSCP_OFFSET   6
#define GSDI_SST_DATA_DL_SMSPP_OFFSET   6
#define GSDI_SST_MENU_SELECTION_OFFSET  6
#define GSDI_SST_CC_OFFSET              6
#define GSDI_SST_PROACTIVE_SIM_OFFSET   7
#define GSDI_SST_CBMID_RANGES_OFFSET    7
#define GSDI_SST_BDN_OFFSET             7
#define GSDI_SST_EXT4_OFFSET            7
#define GSDI_SST_DEPERSONALIZE_KEYS_OFFSET     8
#define GSDI_SST_CNL_OFFSET             8
#define GSDI_SST_SMSR_OFFSET            8
#define GSDI_SST_NTWRK_ALRT_ST_OFFSET   8
#define GSDI_SST_MOSMS_OFFSET           9
#define GSDI_SST_GPRS_OFFSET            9
#define GSDI_SST_IMAGE_OFFSET           9
#define GSDI_SST_SOLSA_OFFSET           9
#define GSDI_SST_USSD_OFFSET            10
#define GSDI_SST_RUNAT_OFFSET           10
#define GSDI_SST_UPLMNWACT_OFFSET       10
#define GSDI_SST_OPLMNWACT_OFFSET       10
#define GSDI_SST_HPLMNWACT_OFFSET       11
#define GSDI_SST_CPBCCH_OFFSET          11
#define GSDI_SST_INV_SCAN_OFFSET        11
#define GSDI_SST_EXT_CCP_OFFSET         11
#define GSDI_SST_MEXE_OFFSET            12
#define GSDI_SST_RPLMNWACT_OFFSET       12
#define GSDI_SST_PNN_OFFSET             12
#define GSDI_SST_OPL_OFFSET             12
#define GSDI_CDMA_SST_LBS_OFFSET        6


#define GSDI_SST_CHV1_DISABLE_MASK  0x03   /* 0000 0011 */
#define GSDI_SST_ADN_MASK           0x0C   /* 0000 1100 */
#define GSDI_SST_FDN_MASK           0x30   /* 0011 0000 */
#define GSDI_SST_SMS_MASK           0xC0   /* 1100 0000 */
#define GSDI_SST_ACM_MASK           0x03   /* 0000 0011 */
#define GSDI_SST_CCP_MASK           0x0C   /* 0000 1100 */
#define GSDI_SST_PLMN_SEL_MASK      0x30   /* 0011 0000 */
#define GSDI_SST_MSISDN_MASK        0x03   /* 0000 0011 */
#define GSDI_SST_EXT1_MASK          0x0C   /* 0000 1100 */
#define GSDI_SST_EXT2_MASK          0x30   /* 0011 0000 */
#define GSDI_SST_SMSP_MASK          0xC0   /* 1100 0000 */
#define GSDI_SST_LND_MASK           0x03   /* 0000 0011 */
#define GSDI_SST_CBMI_MASK          0x0C   /* 0000 1100 */
#define GSDI_SST_GID1_MASK          0x30   /* 0011 0000 */
#define GSDI_SST_GID2_MASK          0xC0   /* 1100 0000 */
#define GSDI_SST_SPN_MASK           0x03   /* 0000 0011 */
#define GSDI_SST_SDN_MASK           0x0C   /* 1100 0000 */
#define GSDI_SST_EXT3_MASK          0x30   /* 0011 0000 */
#define GSDI_SST_VGCS_GID_MASK      0x03
#define GSDI_SST_VBS_GID_MASK       0x0C
#define GSDI_SST_eMLPP_MASK         0x30   /* 0011 0000 */
#define GSDI_SST_AAeMLPP_MASK       0xC0   /* 1100 0000 */
#define GSDI_SST_DATA_DL_SMSCP_MASK 0x03
#define GSDI_SST_DATA_DL_SMSPP_MASK 0x0C
#define GSDI_SST_MENU_SELECTION_MASK 0x30
#define GSDI_SST_CC_MASK            0xC0   /* 1100 0000 */
#define GSDI_SST_PROACTIVE_MASK     0x03   /* 0000 0011 */
#define GSDI_SST_CBMID_RANGES_MASK  0x0C
#define GSDI_SST_BDN_MASK           0x30
#define GSDI_SST_EXT4_MASK          0xC0   /* 1100 0000 */
#define GSDI_SST_DEPERKEYS_MASK     0x03
#define GSDI_SST_CNL_MASK           0x0C
#define GSDI_SST_SMSR_MASK          0x30   /* 0011 0000 */
#define GSDI_SST_NTWRK_ALRT_ST_MASK 0xC0
#define GSDI_SST_MOSMS_MASK         0x03
#define GSDI_SST_GPRS_MASK          0x0C   /* 0000 1100 */
#define GSDI_SST_IMAGE_MASK         0x30
#define GSDI_SST_SOLSA_MASK         0xC0
#define GSDI_SST_USSD_MASK          0x03
#define GSDI_SST_RUNAT_MASK         0x0C
#define GSDI_SST_UPLMNWACT_MASK     0x30
#define GSDI_SST_OPLMNWACT_MASK     0xC0   /* 1100 0000 */
#define GSDI_SST_HPLMNWACT_MASK     0x03   /* 0000 0011 */
#define GSDI_SST_CPBCCH_MASK        0x0C
#define GSDI_SST_INV_SCAN_MASK      0x30
#define GSDI_SST_EXT_CCP_MASK       0xC0
#define GSDI_SST_MEXE_MASK          0x03
#define GSDI_SST_RPLMNWACT_MASK     0x0C   /* 0000 1100 */
#define GSDI_SST_PNN_MASK           0x30
#define GSDI_SST_OPL_MASK           0xC0
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_CDMA */

#if defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#if defined (FEATURE_MMGSDI_CDMA)
#define GSDI_CDMA_SST_TOOLKIT_OFFSET 0x07
#define GSDI_CDMA_SST_TOOLKIT_MASK   0x03
#endif

#define GSDI_PIN1_STATUS_OFFSET     18
#define GSDI_PUK1_STATUS_OFFSET     19
#define GSDI_PIN2_STATUS_OFFSET     20
#define GSDI_PUK2_STATUS_OFFSET     21
#define GSDI_FILE_CHARAC_OFFSET     13     /* 14th byte in dir-info */
#define GSDI_CODE_INIT_MASK         0x80   /* 1000 0000 */
#define GSDI_CODE_GUESSES_LEFT_MASK 0x0F   /* 0000 1111 */
#define GSDI_CHV1_DISABLED_MASK     0x80   /* 1000 0000 */

#define GSDI_VALID_FLAG_OFFSET      11
#define GSDI_VALID_FLAG_MASK        0x01   /* 0000 0001 */
#define GSDI_REC_LEN_OFFSET         14
#define GSDI_FILE_STRUCT_OFFSET     13
#define GSDI_CLK_INFO_OFFSET        GSDI_FILE_CHARAC_OFFSET
#define GSDI_CLK_FREQ_MASK          0x02   /* 0000 0010 */
#define GSDI_IDLE_CLK_STATE_MASK    0x0D   /* 0000 1101 */
#define GSDI_VOLTAGE_OFFSET         GSDI_FILE_CHARAC_OFFSET
#define GSDI_VOLTAGE_MASK           0x10   /* 0001 0000 */

#define GSDI_MAX_SEC_ATTR_BUF       128
#define GSDI_MF_DIR                 0x3F00
#define GSDI_GSM_DIR                0x7f20
#define GSDI_TELECOM_DIR            0x7f10
#define GSDI_ADF_DIR                0x7fff
#define GSDI_RUIM_DIR               0x7F25
#define GSDI_DCS1800_DIR            0x7F21

#define GSDI_RUIM_ESN_USAGE_MASK        0x01
#define GSDI_RUIM_STORE_ESN_LENGTH      0x04
#define GSDI_RUIM_STORE_ESN_USE_UIM_ID  0x10

#define GSDI_HISTORICAL_CHAR_T1     0x80
#define GSDI_T2_HIST_CHAR_OFFSET    1
#define GSDI_T3_HIST_CHAR_OFFSET    2
#define GSDI_T4_HIST_CHAR_OFFSET    3
#define GSDI_T5_HIST_CHAR_OFFSET    4
#define GSDI_T6_HIST_CHAR_OFFSET    5
#define GSDI_T7_HIST_CHAR_OFFSET    6
/* ISO/IEC 7816-4 */
#define GSDI_CARD_INDICATOR_TLVS    0x80
#define GSDI_COUNTRY_NAT_DATA       0x01
#define GSDI_ISSUER_ID_NUM          0x02
#define GSDI_CARD_SERVICES_DATA     0x31
#define GSDI_AID_BY_FULL_DF_MASK    0x80
#define GSDI_AID_BY_PART_DF_MASK    0x40
#define GSDI_AID_IN_EF_DIR_MASK     0x20
#define GSDI_AID_IN_EF_ATR_MASK     0x10

#define GSDI_AID_EFDIR_PRESENT_MASK 0xA0


#define GSDI_SELECT_FCP_FILE_DESCRIPTOR_LEN_INDEX 0x03
#define GSDI_SELECT_FCP_FILE_DESCRIPTOR_LEN_LOC   0x04
#define GSDI_FCP_TEMPLATE_TAG       0x62
#define GSDI_FILE_DESCRIPTOR_TAG    0x82
#define GSDI_FILE_IDENTIFIER_TAG    0x83
#define GSDI_DF_NAME_AID_TAG        0x84
#define GSDI_PROPRIETARY_INFO_TAG   0xA5
#define GSDI_LIFE_CYCLE_INTEG_TAG   0x8A
#define GSDI_SEC_ATTR_PROP_TAG      0x86
#define GSDI_SEC_ATTR_CMPACT_TAG    0x8C
#define GSDI_SEC_ATTR_EXPAND_TAG    0xAB
#define GSDI_SEC_ATTR_REF_EXP_TAG   0x8B
#define GSDI_FILE_SIZE_TAG          0x80
#define GSDI_TOTAL_FILE_SIZE_TAG    0x81
#define GSDI_SHORT_FILE_ID_TAG      0x88

#define GSDI_PIN_STATUS_DO_TAG      0xC6
#define GSDI_PS_DO_TAG              0x90
#define GSDI_KEY_REF_TAG            0x83
#define GSDI_PIN_PS_BYTE_MASK       0x80
#define GSDI_PIN2_PS_BYTE_MASK      0x60

#define GSDI_UICC_CHAR_TAG          0x80
#define GSDI_APPL_PWR_CONSMPT_TAG   0x81

#define GSDI_AM_DO_BYTE_TAG         0x80
#define GSDI_AM_DO_INST_TAG         0x84
#define GSDI_SC_DO_OR_TAG           0xA0
#define GSDI_SC_DO_AND_TAG          0xAF
#define GSDI_SC_DO_ALWAYS_TAG       0x90
#define GSDI_SC_DO_NEVER_TAG        0x97
#define GSDI_SC_DO_KEY_REF_TAG      0xA4
#define GSDI_SC_DO_USAGE_QUAL_TAG   0x95

#define GSDI_PS_DO_USAGE_QUAL_LEN   0x01
#define GSDI_USAGE_QUAL_USER_VERIFICATON   0x08

#define SEC_INC_MSK                 0x32
#define SEC_INC_OPERATION           0x32
#define SEC_READ_MSK                0x01
#define SEC_READ_OPERATION          0x01
#define SEC_UPDATE_MSK              0x02
#define SEC_UPDATE_OPERATION        0x02
#define SEC_DEACT_MSK               0x08
#define SEC_DEACT_OPERATION         0x08
#define SEC_ACT_MSK                 0x10
#define SEC_ACT_OPERATION           0x10

#define MS_UIM_SERVER               0x30
#ifdef FEATURE_GSTK
#define MS_GSDI_FCN_COMPLETE          0x31
#define MS_GSDI_FCN                   0x32
#define MS_GSDI_RESET_COMPLETE        0x33
#endif /* FEATURE_GSTK */
#define MS_GSDI_NOTIFY_CLIENT_EVT   0x40
#define GSDI_LINK_ESTABLISHED_MSG   0x10
#define GSDI_ERROR_CONDITION        0x20
#define GSDI_WATCHDOG_MSG           0x21
#define GSDI_ERROR_CONDITION_2      0x22

#define GSDI_PATH_LOOKUP_TABLE_MAX_FILE_LEN         0x04
#define GSDI_LKUP_TABLE_USIM_PB_LEN                 3
#define GSDI_LKUP_TABLE_TELECOM_LEN                 4
#define GSDI_LKUP_TABLE_LEN                         4
#define GSDI_NOT_IN_LKUP_TABLE                      255

#ifdef FEATURE_USIM_1000_PBM
#define GSDI_NUM_FILES_IN_LOOKUP_TABLE              0x66
#define GSDI_MAX_NUMBER_OF_ADN_SUPPORTED            0x04
#define GSDI_NUM_FILES_IN_PB_FILE_ENUM_LOOKUP_TABLE 0x16
#else
#define GSDI_NUM_FILES_IN_LOOKUP_TABLE 57
#endif /*FEATURE_USIM_1000_PBM*/

#define GSDI_MF_PATH_USED                    0x3F00
#define GSDI_USIM_PATH_USED                  0xFFFF
#define GSDI_TELECOM_PATH_USED               0x7F10


#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
#define GSDI_MAX_REGISTERED_TASK_CBS  10
#endif


#define GSDI_NO_DIR_PRESENT          0x00
#define GSDI_MF_PRESENT              0x80
#define GSDI_CDMA_DF_PRESENT         0x01
#define GSDI_GSM_DF_PRESENT          0x02
#define GSDI_DCS1800_DF_PRESENT      0x04

#define GSDI_SLOT1                   0
#define GSDI_SLOT2                   1

/* SEND APDU REQ */
#define GSDI_INST_CLASS_OFFSET       0
#define GSDI_INST_TYPE_OFFSET        1
#define GSDI_P1_PARAM_OFFSET         2
#define GSDI_P2_PARAM_OFFSET         3
#define GSDI_P3_PARAM_OFFSET         4
#define GSDI_APDU_DATA_OFFSET        5
#define GSDI_APDU_DATA_MIN_LEN       4
#define GSDI_SW1_OFFSET              0
#define GSDI_SW2_OFFSET              1

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
#define GSDI_START_READ_API_TESTS              0x3000 /* Read                - 3 Second Timer   */
#define GSDI_START_WRITE_API_TESTS             0x3000 /* Write               - 4 Second Timer   */
#define GSDI_START_GFA_API_TESTS               0x2500 /* Get File Attributes - 5 Second Timer   */
#define GSDI_START_GSC_API_TESTS               0x2000 /* Get Sim Capabilities- 6 Second Timer   */
#define GSDI_START_GPS_API_TESTS               0x1000 /* Get PIN Status      - 5.5 Second Timer */
#define GSDI_START_SELECT_API_TESTS            0x3500 /* Select      - 3.5 Second Timer */
#define GSDI_START_INCREASE_API_TESTS          0x1500
#define GSDI_START_SEARCH_API_TESTS            0x2500
#define GSDI_START_OPEN_CHANNEL_API_TESTS      0x1600
#define GSDI_START_NEXT_API_TEST_CASE          0x0500     /* Get PIN Status      - 0.5 Second Timer */
#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
#define GSDI_EVENT_TEST_KICK_OFF_TIME          0x8000
#define GSDI_EVENT_TESTER_START_NEXT_TEST_TIME 0x100

#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#define GSDI_SPT_DATA_LEN 8

#ifdef FEATURE_HOME_ZONE_SERVICE
#define GSDI_HZI_IMSI_FIRST_BYTE 0x2F
#define GSDI_HZI_IMSI_SECOND_BYTE 0x26
#define GSDI_HZI_IMSI_THIRD_BYTE 0x70
#define GSDI_HZI_IMSI_THIRD_BYTE_ALT 0x80
#define GSDI_HZI_ACTIVATED 0x01
#define GSDI_HZI_SINGLE_MODE_SETTING 0x02
#define GSDI_HZI_DUAL_MODE_SETTING 0x01
#endif

#define GSDI_RSP_DATA_SIZE 1000
#ifdef FEATURE_MMGSDI_MCC_VERIFICATION
#define GSDI_MCC_LIST_SIZE          0x0C
#define GSDI_IMSI_HIGH_BYTE_MASK    0xF0
#define GSDI_IMSI_LOW_BYTE_MASK     0x0F
#endif /* FEATURE_MMGSDI_MCC_VERIFICATION */
/* <EJECT> */
/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/

#define GSDI_HIGH_PRI_CMD_Q_SIG             0x00000001
#define GSDI_TASK_CMD_Q_SIG                 0x00000002
#define GSDI_TASK_RSP_Q_SIG                 0x00000004
#define GSDI_UIM_REPORT_SIG                 0x00000008
#define GSDI_RPT_TIMER_SIG                  0x00000010
#define GSDI_UIM_ERROR_SIG                  0x00000020
#define GSDI_CALLBACK_FUNC_REG_SIG          0x00000040

#define GSDI_CALLBACK_FUNC_REG_SIG_1        0x00000080
#define GSDI_CALLBACK_FUNC_REG_SIG_2        0x00000100
#define GSDI_CALLBACK_FUNC_REG_SIG_3        0x00000200
#define GSDI_CALLBACK_FUNC_REG_SIG_4        0x00000400


#ifdef FEATURE_L4
#error code not present
#endif /* FEATURE_L4 */

#define GSDI_RESERVED_FOR_LIB_CMD_Q_SIG     0x00001000

#define GSDI_CLASH_WITH_TASK_OFFLINE_SIG    0x00002000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_TASK_STOP_SIG       0x00004000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_TASK_START_SIG      0x00008000 /* RESERVED SIGNAL */

#define GSDI_NV_SIG                         0x00010000

#define GSDI_REX_TIMER_EXPIRE_SIG           0x00020000
#define GSDI_WAIT_GSTK_TP_DL_COMPLETED_SIG  0x00040000
#define GSDI_PERSO_SECRND_READY_SIG         0x00080000
#define GSDI_UIM_RESET_COMPLETE_SIG         0x00100000
#define GSDI_UIM_SANITY_TIMER_EXPIRE_SIG    0x00200000

#define MMGSDI_TASK_CMD_Q_SIG               0x00400000
#define GSDI_CLASH_WITH_ONCRPC_REPLY_SIG    0x00800000 /* RESERVED SIGNAL */
#define MMGSDI_TASK_RSP_Q_SIG               0x01000000
#define MMGSDI_SYNCH_UIM_REPORT_SIG         0x02000000

/*******ADDITIONAL RESERVED SIGNALS**************/
#define GSDI_CLASH_WITH_CAMERA_SIG          0x04000000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_GRP_ACK_SIG         0x08000000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_SEC_OP_COMPLETE_SIG 0x10000000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_VS_OP_COMPLETE_SIG  0x20000000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_FS_OP_COMPLETE_SIG  0x40000000 /* RESERVED SIGNAL */
#define GSDI_CLASH_WITH_REX_SLEEP_SIG       0x80000000 /* RESERVED SIGNAL */
/************************************************/

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
#define GSDI_API_TESTER_GSC_TIMER_SIG    0x4000000  /* Get Sim Cap Timer Sig    */
#define GSDI_API_TESTER_GPS_TIMER_SIG    0x8000000  /* Get PIN STatus Timer Sig */
#define GSDI_API_TESTER_SELECT_TIMER_SIG 0x100000000
#define GSDI_API_TESTER_INCREASE_TIMER_SIG 0x20000000
#define GSDI_API_TESTER_SEARCH_TIMER_SIG 0x40000000
#define GSDI_API_TESTER_NETWORK_RESEL_ACCESS_ENV_TIMER_SIG 0x80000000 /* Test multiple Location Env and
                                                                         network file access timer sig */
#define GSDI_API_TESTER_OPEN_CHANNEL_TIMER_SIG 0x10000000
#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
#define GSDI_EVENT_TESTER_KICK_OFF_TIMER_SIG  0x200000000
#endif /* FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#ifdef FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP
#define GSDI_GSTK_CMD_UNIT_TESTER_APP_SIG    0x800000
#endif /*FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP */


/* <EJECT> */
/*--------------------------------------------------------------------------
                             GSDI Task Timer Values
--------------------------------------------------------------------------*/
#define GSDI_WAIT_FOR_TASKS_TO_REGISTER    3000 /*  3 Seconds    */

#if defined(FEATURE_APPS_IMAGE_WINMOB) || defined(FEATURE_ESTK)
/*  901 Seconds - Matches the one in GSTK with 1 second extra */
#define GSDI_WAIT_FOR_GSTK_TO_TP_DL        121000
#else
#if defined(FEATURE_GSTK_STATE_TESTER) || defined(FEATURE_GSTK_TEST_AUTOMATION)
#define GSDI_WAIT_FOR_GSTK_TO_TP_DL        12000
#else
#define GSDI_WAIT_FOR_GSTK_TO_TP_DL        6000 /*  6 Seconds   */
#endif /* #if defined(FEATURE_GSTK_STATE_TESTER) || defined(FEATURE_GSTK_TEST_AUTOMATION) */
#endif /* FEATURE_APPS_IMAGE_WINMOB || FEATURE_ESTK */

#define GSDI_UIM_SANITY_TIMER_VALUE      500000 /* 5 Minutes    */

#define GSDI_MAX_BYTES_PER_CMD_FROM_UIM     512
#define GSDI_MAX_UIM_CMDS_PENDING            10

#ifdef FEATURE_SEC
#define GSDI_PERSO_SEC_RND_TIMER 10000
#endif /*FEATURE_SEC*/

/* <EJECT> */
/*--------------------------------------------------------------------------
                             DF file ID (not defined in uim.h)
--------------------------------------------------------------------------*/
#define GSDI_IS_41_DF                0x7F22
#define GSDI_FP_CTS_DF               0x7F23

#define GSDI_DIR_DF                  0x2F00
#define GSDI_PL_DF                   0x2F05
#define GSDI_ARR_DF                  0x2F06
#define GSDI_ICCID_DF                0x2FE2
#define GSDI_ELP_DF                  0x2F05
#define GSDI_FIRST_LVL_DF            0

#define GSDI_MASK_FOR_HIGH_BYTE      0xF0
#define GSDI_MASK_FOR_LOW_BYTE       0x0F

#define GSDI_MAX_APP_COUNT           5

#if defined (FEATURE_UIM_EUIMID)
#define GSDI_CDMA_SST_SF_EUIMID_OFFSET    0x01
#define GSDI_CDMA_SST_SF_EUIMID_MASK      0xC0
#define GSDI_RUIM_SF_EUIMID_USAGE_MASK    0x03
#define GSDI_RUIM_STORE_MEID_LENGTH       0x07
#define GSDI_RUIM_STORE_MEID_USE_EUIM_ID  0x10
#define GSDI_CDMA_SST_MEID_OFFSET         0x02
#define GSDI_CDMA_SST_MEID_MASK           0x03
#define GSDI_RUIM_MEID_UIMID_USAGE_IND    0x01
#define GSDI_RUIM_SF_EUIMID_ESN_USAGE_IND 0x02
#define GSDI_RUIM_SF_EUIMID_USAGE_IND     0x03
#endif /* FEATURE_UIM_EUIMID */


/* --------------------------------------------------------------------------
                                   MMGSDI MACROS
   MACRO Definitions used through GSDI
   --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
   MACRO:       GSDI_GET_MSG_LEN
   DESCRIPTION: Used to get the Message Length from the GSDI Command
                Header.  Will only return the MSB*100 + LSB
   INPUT:       gsdiCmd:  Pointer to the cmd
   --------------------------------------------------------------------------*/
#define GSDI_GET_MSG_LEN(len, gsdiCmd) \
  len = ((gsdi_cmdhdr_T *)gsdiCmd)->message_len_lsb + \
        ((gsdi_cmdhdr_T *)gsdiCmd)->message_len_msb * 0x100;
/* --------------------------------------------------------------------------
   MACRO:       GSDI_GET_TOTAL_MSG_LEN
   DESCRIPTION: Used to get the total Message Length from the GSDI Command
                Header.  Will return GSDI_GET_MSG_LEN + the size of the
                gsdi_cmdhdr_T.
   INPUT:       gsdiCmd:  Pointer to the cmd
   --------------------------------------------------------------------------*/
#define GSDI_GET_TOTAL_MSG_LEN(len) \
  len = len + sizeof(gsdi_cmdhdr_T);
/* --------------------------------------------------------------------------
   MACRO:       GSDI_PUT_MSG_LEN
   DESCRIPTION: Used to to populate the message header contents
   INPUT:       gsdiCmd:  Pointer to the cmd
   --------------------------------------------------------------------------*/
#define GSDI_PUT_MSG_LEN(len,gsdiCmd)                       \
        ((gsdi_cmdhdr_T*)gsdiCmd)->message_len_msb =        \
          (uint8)((len - sizeof(gsdi_cmdhdr_T)) / 0x100);   \
        ((gsdi_cmdhdr_T*)gsdiCmd)->message_len_lsb =        \
          (uint8)((len - sizeof(gsdi_cmdhdr_T)) % 0x100);

/* ----------------------------------------------------------------------------
   MACRO:       GSDI_PUT_MSG_LEN_HDR_TEMPLATE
   DESCRIPTION: Used to to populate the message header contents
   INPUT:       c   ->  Command Type
                s   ->  Slot
                cb  ->  Asynch Callback
                c_p -> command Pointer to populate
   --------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
#define GSDI_PUT_MSG_HDR_TEMPLATE(c,s,cb,c_p)                        \
  ((gsdi_cmdhdr_T*)c_p)->message_set         = MS_GSDI;              \
  ((gsdi_cmdhdr_T*)c_p)->message_id          = c;                    \
  ((gsdi_cmdhdr_T*)c_p)->wait_sig            = GSDI_LIB_CMD_Q_SIG;   \
  ((gsdi_cmdhdr_T*)c_p)->slot                = s;                    \
  ((gsdi_cmdhdr_T*)c_p)->gsdi_async_callback = cb;
#else
#define GSDI_PUT_MSG_HDR_TEMPLATE(c,s,cb,c_p)                        \
  ((gsdi_cmdhdr_T*)c_p)->message_set         = MS_GSDI;              \
  ((gsdi_cmdhdr_T*)c_p)->message_id          = c;                    \
  ((gsdi_cmdhdr_T*)c_p)->tcb_ptr             = rex_self();           \
  ((gsdi_cmdhdr_T*)c_p)->wait_sig            = GSDI_LIB_CMD_Q_SIG;   \
  ((gsdi_cmdhdr_T*)c_p)->slot                = s;                    \
  ((gsdi_cmdhdr_T*)c_p)->gsdi_async_callback = cb;
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
/* --------------------------------------------------------------------------
   MACRO:       MMGSDI_DEBUG_ERR
   DESCRIPTION: When FEATURE_MMGSDI_DEBUG_ERRORS is define, an ERROR_FATA
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are logged to NV for Further Analysis.
   -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_DEBUG_ERROR
  #define MMGSDI_DEBUG_ERR(str,a,b,c)  ERR_FATAL("MMGSDI: " str,a,b,c)
#else
  #define MMGSDI_DEBUG_ERR(str,a,b,c)  ERR("MMGSDI: " str,a,b,c)
#endif /* FEATURE_MMGSDI_DEBUG_ERRORS */
/* --------------------------------------------------------------------------
   MACRO:       MMGSDI_DEBUG_MSG_ERR
   DESCRIPTION: When FEATURE_MMGSDI_DEBUG_ERRORS is define, an ERROR_FATA
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are not logged to NV.
   -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_DEBUG_ERROR
  #define MMGSDI_DEBUG_MSG_ERROR(str,a,b,c)  ERR_FATAL("MMGSDI: " str,a,b,c)
#else
  #define MMGSDI_DEBUG_MSG_ERROR(str,a,b,c)  MSG_ERROR("MMGSDI: " str,a,b,c)
#endif /* FEATURE_MMGSDI_DEBUG_ERRORS */
/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_RETURN_IF_NULL
   DESCRIPTION: Will check the paramter provided and return
                GSDI_INCORRECT_PARAMS if the pointer is NULL
   --------------------------------------------------------------------------*/
/*lint -emacro(774,MMGSDI_RETURN_IF_NULL) */
#define MMGSDI_RETURN_IF_NULL(p_var)                            \
  if ( (p_var) == NULL  )                                         \
  {                                                             \
    MMGSDI_DEBUG_MSG_ERROR("Attempt to use NULL POINTER",0,0,0);\
    return GSDI_INCORRECT_PARAMS;                               \
  }                                                             \
/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_RETURN_IF_OUT_OF_RANGE
   DESCRIPTION: Will check the paramter provided and return
                GSDI_INCORRECT_PARAMS if the pointer is NULL
   --------------------------------------------------------------------------*/
#define MMGSDI_RETURN_IF_OUT_OF_RANGE(Var,Min,Max)                          \
  if ( (Var) < (Min)  )                                                         \
  {                                                                         \
    MMGSDI_DEBUG_MSG_ERROR("Less than MIN: 0x%x < 0x%x, var,min",(Var),(Min),0);\
    return GSDI_INCORRECT_PARAMS;                                           \
  }                                                                         \
  if ( (Var) > (Max) )                                                          \
  {                                                                         \
    MMGSDI_DEBUG_MSG_ERROR("Greater than MAX: 0x%x > 0x%x, var,max",(Var),(Max),0);\
    return GSDI_INCORRECT_PARAMS;                                           \
  }                                                                         \

/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_RETURN_FALSE_IF_OUT_OF_RANGE
   DESCRIPTION: Will check the paramter provided and return
                GSDI_INCORRECT_PARAMS if the pointer is NULL
   --------------------------------------------------------------------------*/
#define MMGSDI_RETURN_FALSE_IF_OUT_OF_RANGE(Var,Min,Max)                      \
  if ( (Var) < (Min)  )                                                       \
  {                                                                           \
    MMGSDI_DEBUG_MSG_ERROR(                                                   \
      "Less than MIN: 0x%x < 0x%x, var,min",(int)(Var),(int)(Min),0);         \
    return FALSE;                                                             \
  }                                                                           \
  if ( (Var) > (Max) )                                                        \
  {                                                                           \
    MMGSDI_DEBUG_MSG_ERROR(                                                   \
      "Greater than MAX: 0x%x > 0x%x, var,max",(int)(Var),(int)(Max),0);      \
    return FALSE;                                                             \
  }                                                                           \

/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_RETURN_IF_EXCEEDS
   DESCRIPTION: Will check the paramter provided and return
                GSDI_INCORRECT_PARAMS if the pointer is NULL
   --------------------------------------------------------------------------*/
#define MMGSDI_RETURN_IF_EXCEEDS(Var,Max)                                   \
  if ( (Var) > (Max) )                                                          \
  {                                                                         \
    MMGSDI_DEBUG_MSG_ERROR("Greater than MAX: 0x%x > 0x%x, var,max",(Var),(Max),0);\
    return GSDI_INCORRECT_PARAMS;                                           \
  }                                                                         \

/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_SET_MIN
   DESCRIPTION: Will set var to the lesser of val1 or val2
   --------------------------------------------------------------------------*/
#define MMGSDI_SET_MIN(var,val1,val2) \
  if (val1 < val2 )                   \
    var = val1;                       \
  else                                \
    var = val2;                       \

/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_GET_FUNCTION_PTR
   DESCRIPTION: Will select the function pointer gased on the
                card applications information.
   --------------------------------------------------------------------------*/
  #define MMGSDI_GET_FUNCTION_PTR(p, usim_p, gsm_p) \
  if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)) \
  {                                                                    \
    p = usim_p;                                                        \
  }                                                                    \
  else                                                                 \
  {                                                                    \
    p = gsm_p;                                                         \
  }

/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_SELECT_FILE
   DESCRIPTION: Will select the filename based on the availability
                of the card applications
   --------------------------------------------------------------------------*/
#define MMGSDI_SELECT_FILE(file, usim_file, gsm_file) \
  if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)) \
  {                                                                  \
    file = usim_file;                                                \
  }                                                                  \
  else                                                               \
  {                                                                  \
    file=gsm_file;                                                   \
  }

/* <EJECT> */
/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/
typedef enum {
  GSDI_CACHE_COMMON,    /* common cache, e.g., ELP */
  GSDI_CACHE_GSM,       /* gsm cache, e.g., GSM_IMSI */
  GSDI_CACHE_UMTS,      /* USIM cache, e.g., USIM_IMSI */
  GSDI_CACHE_CDMA,      /* CDMA cache, e.g., CDMA_CDMA_SVC_TABLE */
  GSDI_CACHE_MAX = 0x7FFFFFFF
} gsdi_cache_enum_type;

typedef enum {
  GSDI_DATA_FROM_CARD,
  GSDI_DATA_FROM_CLIENT,
  GSDI_DATA_FROM_MAX = 0x7FFFFFFF
} gsdi_data_from_enum_type;

typedef enum {
    GSDI_GSM  = 0x00,
    GSDI_CDMA = 0x01,
    GSDI_UMTS = 0x02
} gsdi_technology_type;

typedef enum {
    GSDI_NOT_INIT_S = 0x00,
    GSDI_NO_SIM_S,
    GSDI_LINK_ESTABLISHED_S,
    GSDI_SIM_INSERTED_S,
    GSDI_PIN_VERIFIED_S,
    GSDI_SIM_INIT_COMPLETED_S,
    GSDI_SIM_INIT_COMPLETED_NO_PROV_S,
    GSDI_CARD_ERROR_S
} gsdi_state_enum_type;


typedef struct {
  gsdi_refresh_vote_cb_type *  callback;
  boolean ok_to_take_down_stack;
  boolean ok_to_file_notify;
} gsdi_refresh_vote_data_type;

typedef struct {
  gsdi_task_notify_file_list_cb_type * callback;
  uint8                                num_files;
  uim_items_enum_type                  file_list[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
  boolean                              client_notified;
  uint8                                num_files_in_notify_file_list;
  uim_items_enum_type                  notify_file_list[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
  boolean                              client_refresh_attempt_failed;
  }gsdi_refresh_fcn_notify_data_type;

typedef struct _gsdi_refresh_vote_node_type {
  struct _gsdi_refresh_vote_node_type *  head_p;
  struct _gsdi_refresh_vote_node_type *  prev_p;
  struct _gsdi_refresh_vote_node_type *  next_p;
  struct _gsdi_refresh_vote_node_type *  tail_p;
  gsdi_refresh_vote_data_type           vote_data;
}gsdi_refresh_vote_node_type;

typedef struct _gsdi_refresh_fcn_notify_node_type {
  struct _gsdi_refresh_fcn_notify_node_type *  head_p;
  struct _gsdi_refresh_fcn_notify_node_type *  prev_p;
  struct _gsdi_refresh_fcn_notify_node_type *  next_p;
  struct _gsdi_refresh_fcn_notify_node_type *  tail_p;
  gsdi_refresh_fcn_notify_data_type            fcn_notify_data;
  boolean                                      additional_files_read;
}gsdi_refresh_fcn_notify_node_type;

typedef struct
{
  boolean                     perso_op_done;
  uint8                       num_files;
  uim_items_enum_type         file_list_p[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
}gstk_refresh_fcn_data_type;

typedef struct
{
  boolean                      refresh_timer_expired;
  uint8                        msg_set;
  uint32                       client_ref;
  gsdi_returns_T               gsdi_refresh_status_slot1;
  gsdi_returns_T               gsdi_refresh_status_slot2;
  void                        (*gsdi_async_callback)(gsdi_cnf_T*);
  boolean                      perso_refresh_needed;
  gstk_refresh_fcn_data_type   gsdi_refresh_fcn_data;
}gsdi_gstk_save_data_type;

#ifdef FEATURE_MMGSDI_MCC_VERIFICATION
typedef struct
{
  uint8                        mcc1;
  uint8                        mcc2;
  uint8                        mcc3;
}gsdi_mcc_data_type;
#endif /* FEATURE_MMGSDI_MCC_VERIFICATION */

typedef enum {
  GSDI_MAIN_ST_TASK_NOT_INIT,
  GSDI_MAIN_ST_SIM_NOT_INIT,
  GSDI_MAIN_ST_SIM_SESSION_ACTIVE
}gsdi_main_state_T;

typedef enum {
  ROOT    = UIM_EF_UNDER_MF_START,
  CDMA    = UIM_RUIM_EF_START,
  GSM     = UIM_GSM_EF_START,
  TELECOM = UIM_TELECOM_EF_START,
  USIM    = UIM_USIM_EF_START,
  ORANGE  = UIM_ORANGE_EF_START,
  CINGULAR= UIM_CINGULAR_EF_START,
  HZI      = UIM_HZI_EF_START,
  BY_PATH = UIM_EF_PATH_START
}gsdi_item_category_T;

typedef struct {
  boolean    PIN1_enabled;
  byte       PIN1_key_ref;
  boolean    PIN2_enabled;
  byte       PIN2_key_ref;
  boolean    UNIV_enabled;
  byte       UNIV_key_ref;
  byte       pin1_num_retries_allowed;
  byte       puk1_num_retries_allowed;
  byte       pin2_num_retries_allowed;
  byte       puk2_num_retries_allowed;
  gsdi_pin_status_T   pin1_status;
  gsdi_pin_status_T   pin2_status;
  boolean    pin_status_cached;
}pin_status_info_T;

typedef enum {
  SC_DO_ALWAYS_TAG = 0x90,
  SC_DO_NEVER_TAG = 0x97,
  SC_DO_KEY_REF_TAG = 0xA4
}sc_do_tags_T;

typedef enum {
  PROP_FORMAT,
  COMPACT_FORMAT,
  EXPAND_FORMAT,
  REF_EXPAND_FORMAT
}sec_attr_format_T;

typedef struct {
  /* File Descriptor tag '82' (Mandatory) */
  byte       file_descriptor;
  byte       data_coding;
  byte       record_length[2];
  byte       number_of_records;
  /* File Identifier tag '83' (Mandatory) */
  byte       file_identifier[2];
  /* Proprietary Information tag 'A5' (Optional */
  boolean    proprietary_info_present;
  boolean    UICC_char_field_present;
  byte       UICC_char_byte;
  /* Life Cycle Status tag '8A' (Mandatory) */
  byte       life_cycle_status;
  /* Security Attributes '86', '8B', '8C' or 'AB' (Mandatory) */
  sec_attr_format_T  sec_attr_format;
  key_ref_values_T  read_search_key_ref;
  key_ref_values_T  update_key_ref;
  key_ref_values_T  increase_key_ref;
  key_ref_values_T  activate_key_ref;
  key_ref_values_T  deactivate_key_ref;
  /* File size tag '80' (Mandatory) */
  byte       file_size[2];
  /* Total file size tag '81' (Optional */
  boolean    total_file_size_present;
  byte       total_file_size[2];
  /* Short file identifier tag '88' (Optional) */
  boolean    short_file_id_present;
  boolean    short_file_id_supported;
  byte       short_file_id;
} EF_umts_header_data_T;

typedef enum {
  R99_2000_07 = 0,
  R99_2000_09 = 1,
  R99_2000_12 = 2,
  VERSION_UNKNOWN = 99
}gsdi_usim_spec_version_T;

typedef struct {
  uim_items_enum_type last_selected_ef;
  uim_items_enum_type current_ef;
  boolean             session_open;
  boolean             global_pin_set;
} gsdi_session_control_T;

typedef struct {
  int32               atr_len;
  uint8               atr_buffer[GSDI_MAX_ATR_BYTES];
}gsdi_card_atr_type;

typedef struct {
  uint32              hist_len;
  uint8               hist_buffer[GSDI_MAX_HIST_BYTES];
}gsdi_hist_byte_type;


typedef struct {
  boolean                         event_sent;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  boolean                         event2_sent;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  gsdi_event_callback_funct_type *gsdi_callback_ptr;
} gsdi_event_callback_info_type;

#ifdef FEATURE_MMGSDI_CPHS
/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_CPHS_INFORMATION_TYPE:
   DESCRIPTION:  Used for storate of CPHS Information within the GSDI
                 Global Data Variable.
                 This strucutre reused structures defined in gsdi_exp.h
   --------------------------------------------------------------------------*/
typedef struct
{
  boolean                             cphs_capable;
  gsdi_cphs_service_table_type        services;
  gsdi_cphs_cust_service_profile_type csp;
}gsdi_cphs_information_type;

#endif /* FEATURE_MMGSDI_CPHS */

typedef struct {
  gsdi_card_atr_type    card_atr;
  gsdi_hist_byte_type   hist_bytes;
  uint32                gsdi_uim_sanity_timer_value;
  byte                  sim_phase;
  boolean               task_stopped;
  boolean               sim_present_and_ready;
  boolean               init_completed;
  boolean               illegal_sim;
  boolean               disable_chv1_allowed;
  boolean               cache_ready;
  gsdi_state_enum_type  gsdi_state;
  gsdi_card_mode_capability_T  current_card_mode_capability;
  gsdi_card_apps_T      card_applications;
  gsdi_item_category_T  current_EF_category;
  uim_rpt_type          uim_rsp_report;
  uim_status_type       uim_error_condition;
  sim_capabilities_T    sim_capability;
  boolean               registered_for_fcns;
  gsdi_protocol_enum_type protocol;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    /* DUAL SLOT RELATED GLOBAL STRUCTURES */
  boolean               cache_ready_2;
  gsdi_hist_byte_type   hist_bytes_2;
  byte                  sim_phase_2;
  boolean               init_completed_2;
  boolean               sim_present_and_ready_2;
  boolean               illegal_sim_2;
  boolean               disable_chv1_allowed_2;
  pin_status_info_T     pin_status_info_2;
  gsdi_card_mode_capability_T  current_card_mode_capability_2;
  sim_capabilities_T    sim_capability_2;
  gsdi_state_enum_type  gsdi_state_2;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  /* USIM ONLY RELATED GLOBAL STRUCTURES */
  byte                  current_aid[UIM_AID_MAX_NAME_LENGTH];
  byte                  aid_length;
  boolean               aid_available;
  gsdi_usim_spec_version_T  usim_spec_version;
  gsdi_session_control_T usim_session_control;

  /* USIM/GSM INST CLASS */
  nv_uim_first_inst_class_type first_inst_class;

  /* GSM ONLY RELATED GLOBAL STRUCTURES */
  byte                  ruim_dfs_present;

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
  gsdi_event_callback_info_type  gsdi_callback_info[GSDI_MAX_REGISTERED_TASK_CBS];
#else
  gsdi_event_callback_info_type  gsdi_callback_info;
#endif

  /* TOOLKIT RELATED STRUCTURES */
  uint32                fcns_outstanding;
  gsdi_gstk_save_data_type gstk_data;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  gsdi_card_atr_type    card_atr_2;
  gsdi_slot_pref_type   user_slot_prefs;
  boolean               just_invalidated_2;
  boolean               just_rehabilitated_2;
  boolean               gsdi_gstk_tp_done_2;
  uim_status_type       uim_error_condition_2;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  boolean               just_invalidated;
  boolean               just_rehabilitated;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_perso_eng_init_state_enum_type perso_engine_state;
  boolean                             perso_post_pin1_init;
  gsdi_perso_event_enum_type          perso_event;
#else
  gsdi_perso_state_enum_type perso_state;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  boolean               onchip_sim_activated;
#ifdef FEATURE_MMGSDI_CPHS
 gsdi_cphs_information_type cphs_info;
#endif /* FEATURE_MMGSDI_CPHS */
  boolean               gsdi_gstk_tp_done;
  boolean               recovering_after_reset;
  byte                  recovery_retry_count;
  mmgsdi_client_id_type client_id;
#ifdef FEATURE_MMGSDI_MCC_VERIFICATION
  boolean               gsdi_gprs_anite_nv_status;
#endif /* FEATURE_MMGSDI_MCC_VERIFICATION */
}
gsdi_data_T;


/*
** DEFINE A SIM-CACHE ELEMENT
*/
typedef struct {
  uim_items_enum_type   filename;
  boolean               pref_slot;
  boolean               init;
  byte                  rec_len;
  byte                  body[GSDI_CACHE_MAX_FILE_LEN];
  mmgsdi_len_type       total_len;  /* rec_len + whatever is in the add_data */
  uint8*                add_data;
  boolean               in_synch;
}sim_cache_element_T;

/* Common cache for GSM,UMTS and CDMA */
extern sim_cache_element_T gsdi_common_cache[GSDI_NUM_FILES_IN_COMMON_CACHE];

extern gsdi_ecc_info_table_type *gsdi_ecc_table_p;

extern mmgsdi_uim_report_rsp_type *mmgsdi_internal_synch_uim_rpt_rsp;

#if defined(FEATURE_SEC) && defined(FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API)
extern rex_timer_type     gsdi_perso_secrnd_timer;
#endif /* FEATURE_SEC && FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API*/

#define GSDI_FILE_ENUM_TABLE_SIZE 380

#define GSDI_FILE_SERVICE_TABLE_SIZE 100

typedef struct
{
  uim_items_enum_type uim_file;
  gsdi_file_enum_type gsdi_file;
} gsdi_file_enum_table_type;

/*Maps the gsdi file type to gsdi service*/
typedef struct
{
  gsdi_file_enum_type gsdi_file;
  gsdi_services_type gsdi_service;
} gsdi_file_service_table_type;


extern gsdi_file_enum_table_type gsdi_file_enum_table[GSDI_FILE_ENUM_TABLE_SIZE];
extern gsdi_file_service_table_type gsdi_file_service_table[GSDI_FILE_SERVICE_TABLE_SIZE];

/* Structure to store Application specific PIN information */
typedef struct {
  gsdi_app_id_type app_id;
  gsdi_pin_info_T  pin1;
  gsdi_pin_info_T  pin2;
}gsdi_app_pin_info_type;

extern gsdi_app_pin_info_type gsdi_app_pin_info_table[GSDI_MAX_APP_COUNT];

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */


#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* #ifdef FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
extern sim_cache_element_T gsdi_ruim_cache[GSDI_NUM_FILES_IN_RUIM_CACHE];
#endif /*#ifdef FEATURE_MMGSDI_CDMA */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_USIM_1000_PBM
/*
** This structure is used to assign filenames to pb file ids read from the pbr
*/
typedef struct {
  uim_items_enum_type uim_base_pb_filename;
  uim_items_enum_type uim_pb_filename[GSDI_MAX_NUMBER_OF_ADN_SUPPORTED-1];
}gsdi_pb_file_enum_lookup_table_element_T;


extern gsdi_pb_file_enum_lookup_table_element_T gsdi_pb_file_enum_lookup_table[GSDI_NUM_FILES_IN_PB_FILE_ENUM_LOOKUP_TABLE];
#endif /*FEATURE_USIM_1000_PBM*/


typedef struct {
  byte   sw1;
  byte   sw2;
}
sw1_sw2_T;


typedef union {
  gsdi_init_req_T                   gsdi_init_req_cmd;
  gsdi_get_sim_capabilities_req_T   gsdi_get_sim_capabilities_req_cmd;
  gsdi_sim_search_req_T             gsdi_sim_search_req_cmd;
  gsdi_sim_read_req_T               gsdi_sim_read_req_cmd;
  gsdi_sim_write_req_T              gsdi_sim_write_req_cmd;
  gsdi_get_image_req_T              gsdi_image_req_cmd;
  gsdi_sim_access_req_T             gsdi_sim_access_req_cmd;
  gsdi_sim_increase_req_T           gsdi_sim_increase_req_cmd;
  gsdi_run_gsm_algorithm_req_T      gsdi_run_gsm_algorithm_req_cmd;
  gsdi_get_file_attributes_req_T    gsdi_get_file_attributes_req_cmd;
  gsdi_select_req_T                 gsdi_select_req_cmd;
#if defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif
  gsdi_enable_fdn_req_T             gsdi_enable_fdn_req_cmd;
  gsdi_disable_fdn_req_T            gsdi_disable_fdn_req_cmd;
  gsdi_get_pin_status_req_T         gsdi_get_pin_status_req_cmd;
  gsdi_verify_pin_req_T             gsdi_verify_pin_req_cmd;
  gsdi_change_pin_req_T             gsdi_change_pin_req_cmd;
  gsdi_unblock_pin_req_T            gsdi_unblock_pin_req_cmd;
  gsdi_disable_pin_req_T            gsdi_disable_pin_req_cmd;
  gsdi_enable_pin_req_T             gsdi_enable_pin_req_cmd;
  gsdi_illegal_sim_req_T            gsdi_illegal_sim_req_cmd;
  gsdi_call_state_req_T             gsdi_call_state_req_cmd;
  gsdi_uim_link_established_T       gsdi_uim_link_established_cmd;
#if defined ( FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif
#ifdef FEATURE_GSTK
  gsdi_sim_refresh_req_T            gsdi_sim_refresh_req_cmd;
  gsdi_refresh_registration_req_T   gsdi_refresh_reg_req_cmd;
  gsdi_refresh_complete_req_T       gsdi_refresh_completed_req_cmd;
  gsdi_internal_req_T               gsdi_internal_req_cmd;
  gsdi_update_call_stack_req_T      gsdi_update_call_stack_req_cmd;
#endif /* FEATURE_GSTK */
  gsdi_send_apdu_req_T              gsdi_send_apdu_req_cmd;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  gsdi_switch_slot_preference_req_T gsdi_switch_slot_pref_req_cmd;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  gsdi_notify_client_req_T          gsdi_notify_client_req_cmd;
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  gsdi_get_atr_req_T                gsdi_get_atr_req_cmd;
  gsdi_open_channel_req_T           gsdi_open_channel_req_cmd;
  gsdi_close_channel_req_T          gsdi_close_channel_req_cmd;
  gsdi_send_channel_data_req_T      gsdi_send_channel_data_req_cmd;
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_reg_task_req_T                gsdi_perso_reg_task_req_cmd;
  gsdi_perso_act_feature_ind_req_T         gsdi_perso_act_feature_ind_req_cmd;
  gsdi_perso_get_feature_ind_req_T         gsdi_perso_get_feature_ind_req_cmd;
  gsdi_perso_dact_feature_ind_req_T        gsdi_perso_deact_feature_ind_req_cmd;
  gsdi_perso_ota_deperso_req_T             gsdi_perso_ota_deperso_req_cmd;
  gsdi_perso_set_feature_data_req_T        gsdi_perso_set_feature_data_req_cmd;
  gsdi_perso_unblock_feature_ind_req_T     gsdi_perso_unblock_feature_ind_key_req_cmd;
  gsdi_perso_get_feature_key_req_T         gsdi_perso_get_feature_key_req_cmd;
  gsdi_perso_perm_disabled_feature_req_T   gsdi_perso_perm_disabled_feature_req_cmd;
  gsdi_perso_get_perm_feature_ind_req_T    gsdi_perso_get_perm_feature_ind_req_cmd;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_perso_verify_phone_code_req_T       gsdi_verify_phone_code_cmd;
  gsdi_perso_change_phone_code_req_T       gsdi_change_phone_code_cmd;
  gsdi_perso_unblock_phone_code_req_T      gsdi_unblock_phone_code_cmd;
  gsdi_perso_set_phone_code_req_T          gsdi_set_phone_code_cmd;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
  gsdi_sim_secure_store_req_T              gsdi_sim_secure_store_req_cmd;
  gsdi_sim_secure_read_req_T               gsdi_sim_secure_read_req_cmd;
  gsdi_secure_device_info_req_T            gsdi_secure_device_info_req_cmd;
  gsdi_perso_root_key_write_req_T          gsdi_perso_root_key_write_cmd;
  gsdi_perso_root_key_read_req_T           gsdi_perso_root_key_read_cmd;
  gsdi_perso_get_lib_version_req_T         gsdi_get_lib_version_cmd;
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#ifdef FEATURE_UIM_HOT_SWAP
  gsdi_uim_swap_T     gsdi_uim_swap_cmd;
#endif
}gsdi_task_cmd_data_type;

/* Results of Parsing ATR Card Services Data */
typedef struct {
  boolean    card_services_data_present;
  boolean    select_by_full_df_name;
  boolean    select_by_part_df_name;
  boolean    aid_available_in_dir;
  boolean    aid_available_in_atr;
}gsdi_atr_results_T;

/* The common command header for all commands received by GSDI task */
typedef struct {
  q_link_type               link;
  gsdi_task_cmd_data_type   cmd;
}gsdi_task_cmd_type;

extern word path_array_from_lkup_table[GSDI_LKUP_TABLE_LEN];

#if defined (FEATURE_MMGSDI_CDMA)
typedef enum {
  NV_GSDI_USE_ESN = 0,                  /* Use ESN as ESN */
  NV_GSDI_USE_RUIM_ID = 1,              /* Use R-UIM ID as ESN */
  NV_GSDI_ZERO_ESN = 2                  /* Zero the ESN */
} gsdi_esn_usage_type;
#endif

#define MMGSDI_PIN_STATUS_UNKNOWN -1
#define MMGSDI_PIN_STATUS_ALWAYS  0
#define MMGSDI_PIN_STATUS_NEVER   0x40000000
#define MMGSDI_PIN_STATUS_APP1_PIN1 0x00000001
#define MMGSDI_PIN_STATUS_APP1_PIN2 0x00000002

typedef struct {
  int32       and_pin_status;
  int32       or_pin_status;
}gsdi_pin_status_info_type;

typedef struct {
 uim_items_enum_type       file_name;
 gsdi_pin_status_info_type read;
 gsdi_pin_status_info_type write;
 boolean                   cyclic_increase_allowed;
 gsdi_pin_status_info_type increase;
}gsdi_attr_cache;


/* <EJECT> */
/*--------------------------------------------------------------------------
                             Global Variables
--------------------------------------------------------------------------*/

/* GSDI GLOBAL DATA */
extern gsdi_data_T gsdi_data;

/* GSDI TASK HIGH PRIORITY COMMAND QUEUE */
extern q_type gsdi_high_pri_cmd_q;

/* GSDI TASK COMMAND QUEUE */
extern q_type gsdi_task_cmd_q;

/* MMGSDI TASK COMMAND QUEUE */
extern q_type mmgsdi_task_cmd_q;

/* MMGSDI TASK RESPONSE QUEUE */
extern q_type mmgsdi_task_rsp_q;

extern gsdi_resp_type gsdi_resp;   /* store the response type for gsdi cmd */

extern rex_crit_sect_type gsdi_crit_sect; /* GSDI Critical Section */

extern rex_crit_sect_type mmgsdi_crit_sect;

/* Voting Link List */
extern gsdi_refresh_vote_node_type refresh_vote_list_node;

/* Notify Clients Link List */
extern gsdi_refresh_fcn_notify_node_type refresh_fcn_notify_list_node;

/* GSDI - UIM Sanity Timer */
extern rex_timer_type gsdi_uim_sanity_timer;

#ifdef FEATURE_MMGSDI_ENABLE_API_TESTER
/* Test SIM Reads Timer Expire */
extern rex_timer_type gsdi_read_api_test_timer;

/* Test SIM Write Timer Expire */
extern rex_timer_type gsdi_write_api_test_timer;

/* Test Get PIN Status Timer Exipre */
extern rex_timer_type gsdi_gps_api_test_timer;

/* Test SIM Get File Attributes Timer Expire */
extern rex_timer_type gsdi_gfas_api_test_timer;

/* Test SIM Get SIM Capabilities Timer Exipire */
extern rex_timer_type gsdi_gscs_api_test_timer;

/* Test SIM Select Timer Exipire */
extern rex_timer_type gsdi_select_api_test_timer;

/* Test SIM Increase Timer Exipire */
extern rex_timer_type gsdi_increase_api_test_timer;

/* Test SIM Search Timer Exipire */
extern rex_timer_type gsdi_search_api_test_timer;

/* Test Open Channel API */
rex_timer_type gsdi_open_channel_api_test_timer;

/* Test SIM Reads subsequent timer expire */
extern rex_timer_type gsdi_read_api_next_test_timer;

/* Test SIM Writes subsequent timer expire */
extern rex_timer_type gsdi_write_api_next_test_timer;

/* Test SIM Get PIN Status subsequent timer expire */
extern rex_timer_type gsdi_pin_api_next_test_timer;

/* Test SIM Get File Attribute subsequent timer expire */
extern rex_timer_type gsdi_file_attr_api_next_test_timer;

/* Test SIM Select subsequent timer expire */
extern rex_timer_type gsdi_select_api_next_test_timer;

/* Test SIM get sim capability subsequent timer expire */
extern rex_timer_type gsdi_sim_cap_api_next_test_timer;

/* Test SIM increase subsequent timer expire */
extern rex_timer_type gsdi_increase_api_next_test_timer;

/* Test SIM search subsequent timer expire */
extern rex_timer_type gsdi_search_api_next_test_timer;

#ifdef FEATURE_GSTK
extern rex_timer_type gsdi_gstk_network_resel_access_env_test_timer;
#endif /*FEATURE_GSTK */

extern rex_timer_type gsdi_open_channel_api_next_test_timer;

extern rex_timer_type gsdi_stream_apdu_api_next_test_timer;

#endif /* FEATURE_MMGSDI_ENABLE_API_TESTER */

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
extern rex_timer_type gsdi_gstk_cmd_unit_tester_timer;
#endif /* #ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER  */

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
extern rex_timer_type gsdi_event_test_kick_off_timer;
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
#define MMGSDI_MF_NUM_FILE_ENUM  UIM_MAX_EF_UNDER_MF_ITEMS
extern gsdi_attr_cache gsdi_mf_cache_tbl[MMGSDI_MF_NUM_FILE_ENUM];
#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS ) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#define MMGSDI_GSM_DF_NUM_FILE_ENUM UIM_MAX_GSM_ITEMS
extern gsdi_attr_cache   gsdi_gsm_cache_tbl[MMGSDI_GSM_DF_NUM_FILE_ENUM];
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

#if defined (FEATURE_MMGSDI_CDMA)
#define MMGSDI_CDMA_DF_NUM_FILE_ENUM UIM_MAX_RUIM_ITEMS
extern gsdi_attr_cache   gsdi_ruim_cache_tbl[MMGSDI_CDMA_DF_NUM_FILE_ENUM];
#endif
#define MMGSDI_TELECOM_DF_NUM_FILE_ENUM UIM_MAX_TELECOM_ITEMS
#define MMGSDI_EF_BY_PATH_NUM_FILE_ENUM UIM_MAX_EF_BY_PATH_CACHE_ITEMS
extern gsdi_attr_cache   gsdi_telecom_cache_tbl[MMGSDI_TELECOM_DF_NUM_FILE_ENUM];
extern gsdi_attr_cache   gsdi_ef_cache_tbl[MMGSDI_EF_BY_PATH_NUM_FILE_ENUM];
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/

/* <EJECT> */
/*-------------------------------------------------------------------------
                             Function Prototypes
---------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------*/
/*                          G S D I     C O M M O N                        */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY
#error code not present
#endif /* FEATURE_UIM_SKIP_GSM_IF_RUIM_ONLY */

/*===========================================================================
FUNCTION gsdi_proc_init

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_proc_init(
   gsdi_init_req_T * );

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
);

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
extern gsdi_sim_events_T gsdi_util_get_sim_inserted_event(
   gsdi_slot_id_type slot);
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
extern gsdi_sim_events_T gsdi_util_get_refresh_event(
   gsdi_slot_id_type slot,
   gsdi_refresh_modes_enum_type refresh_mode);

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
extern void gsdi_util_set_sim_present_and_ready(
   gsdi_slot_id_type slot,
   boolean           present_and_ready
);

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
                         1)  &gsdi_data.pin_status_info.pin1_just_verified
                             - OR -
                         2)  &gsdi_data.pin_status_info_2.pin1_just_verified

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 * gsdi_util_get_apps_enabled(
   gsdi_slot_id_type slot);

/*===========================================================================
FUNCTION GSDI_HANDLE_NO_SIM_STATE_CMD

DESCRIPTION
    Function called by GSDI when a message is received and we are in
    GSDI_NO_SIM_S.

    This function is called by gsdimain.

    INPUT:
    1.  void *  message_p:  Pointer to void * data type.  Contains message
                            just received for processing.
    2.  byte *  data_p:     Pointer to data buffer defined in gsdimain.

DEPENDENCIES

RETURN VALUE
  gsdi_returns_T.  GSDI_SUCCESS:  Command successfully handled and processed
                   GSDI_ERROR:    Command did not process correctly
                   GSDI_INCORRECT_STATE:  Command is not valid for current
                                  state or current state is invalid

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_handle_no_sim_state_cmd(
    void *  message_p,
    byte *  data_p
);

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
);
/*===========================================================================
FUNCTION GSDI_HANDLE_LINK_ESTABLISHED_STATE_CMD

DESCRIPTION
    Function called by GSDI when a message is received and GSDI in
    the SIM_INSERTED_S.

    This function is called by gsdimain.

    INPUT:
    1.  void *  message_p:  Pointer to void * data type.  Contains message
                            just received for processing.
    2.  byte *  data_p:     Pointer to data buffer defined in gsdimain.

DEPENDENCIES

RETURN VALUE
  gsdi_returns_T.  GSDI_SUCCESS:  Command successfully handled and processed
                   GSDI_ERROR:    Command did not process correctly
                   GSDI_INCORRECT_STATE:  Command is not valid for current
                                  state or current state is invalid

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_handle_link_established_state_cmd(
    void *  message_p,
    byte *  data_p
);
/*===========================================================================
FUNCTION GSDI_HANDLE_SIM_INSERTED_STATE_CMD

DESCRIPTION
    Function called by GSDI when a message is received and GSDI in
    the SIM_INSERTED_S.

    This function is called by gsdimain.

    INPUT:
    1.  void *  message_p:  Pointer to void * data type.  Contains message
                            just received for processing.
    2.  byte *  data_p:     Pointer to data buffer defined in gsdimain.

DEPENDENCIES

RETURN VALUE
  gsdi_returns_T.  GSDI_SUCCESS:  Command successfully handled and processed
                   GSDI_ERROR:    Command did not process correctly
                   GSDI_INCORRECT_STATE:  Command is not valid for current
                                  state or current state is invalid

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_handle_sim_inserted_state_cmd(
    void *  message_p,
    byte *  data_p
);
/*===========================================================================
FUNCTION   GSDI_GET_AVAILABLE_CARD_APPLICATIONS

DESCRIPTION
  This function is used by GSDI to determine which applications are available
  on the card.  This function is dependent on the Features Defined.

  FEATURE_MMGSDI_UMTS:  Check is made for EF-DIR & USIM AID
  FEATURE_MMGSDI_GSM:   Check is made for GSM DF
  FEATURE_MMGSDI_CDMA:  Check is made for CDMA DF, GSM DF, DCS 1800 DF

  In all cases, gsdi_data.card_applications.apps_enabled is a BITMAP
  updated with:
  #define GSDI_NO_APP_MASK    0x00
  #define GSDI_TELECOM_MASK   0x01
  #define GSDI_GSM_SIM_MASK   0x02
  #define GSDI_USIM_APP_MASK  0x04
  #define GSDI_RUIM_APP_MASK  0x08
  #define GSDI_EMV_APP_MASK   0x10

  A variable dfs_present is also a BITMAP carried along but only used
  if FEATURE_MMGSDI_CDMA is defined.

  The variable dfs_present is updated with:
  #defineGSDI_NO_DIR_PRESENT           0x00
  #define GSDI_MF_PRESENT              0x80
  #define GSDI_CDMA_DF_PRESENT         0x01
  #define GSDI_GSM_DF_PRESENT          0x02
  #define GSDI_DCS1800_DF_PRESENT      0x04

DEPENDENCIES
  FEATURE_MMGSDI_GSM
  FEATURE_MMGSDI_UMTS
  FEATURE_MMGSDI_CDMA

RETURNS
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_get_available_card_applications(
  byte * data_p );

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_PREF_SLOT_CHANGE

DESCRIPTION
  This function will serve as the interface to the UIM.  It will be used
  to notify the UIM Task of a Preferred Slot Change.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_pref_slot_change(
  gsdi_slot_id_type  slot,
  gsdi_app_enum_type app
);
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_send_apdu(
  gsdi_slot_id_type slot,
  uint32            apdu_len,
  uint8           * apdu_data,
  uint8           * returned_data_len_p,
  uint8           * data_p
);

/*===========================================================================
FUNCTION gsdi_uim_server_sim_reset

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_sim_reset(
  gsdi_slot_id_type slot,
  uim_protocol_type protocol
);

/*===========================================================================
FUNCTION GSDI_GET_UIM_BUF

DESCRIPTION
    This function is used by the GSDI to get UIM Buffer space to place a
    message for the UIM Server to process.

DEPENDENCIES
    None

RETURN VALUE
    uim_cmd_type

SIDE EFFECTS
    None
===========================================================================*/
extern uim_cmd_type *gsdi_get_uim_buf(
   void );

/*===========================================================================
FUNCTION GSDI_UIM_PUT_CMD_BUF

DESCRIPTION
    This function is used by the GSDI to put the command/message onto the
    UIM Server's Command Buffer.

DEPENDENCIES
    None

RETURN VALUE
    (void)

SIDE EFFECTS
    None.
===========================================================================*/
extern void gsdi_uim_put_cmd_buf(
   gsdi_task_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION GSDI_TASK_GET_CMD_BUF

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_task_cmd_type.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_task_cmd_type *gsdi_task_get_cmd_buf(
   word len );

/*===========================================================================
FUNCTION gsdi_task_put_high_pri_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_task_put_high_pri_cmd_buf(
   gsdi_task_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION gsdi_task_put_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_task_put_cmd_buf(
   gsdi_task_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION gsdi_uim_error_condition

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_uim_error_condition(
   uim_status_type error_condition );

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
extern void gsdi_uim_slot_error_condition (
  uim_status_type   error_condition,
  uim_slot_type     slot);

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
void gsdi_util_update_nv_user_pref(gsdi_app_enum_type app);

#endif /* FEATURE_MMGSDI_DUAL_SLOT */

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
void gsdi_util_preinit_gsdi( gsdi_refresh_modes_enum_type refresh_mode,
                             gsdi_slot_id_type slot,
                             boolean reselect_app);

/*===========================================================================
FUNCTION GSDI_UTIL_POPULATE_SIM_CAPABILITY

DESCRIPTION
  populate the synchronized user sim capability pointer with gsdi_data

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
extern void gsdi_utils_populate_sim_capability(
    gsdi_slot_id_type    slot,
    sim_capabilities_T * sim_capabilities);


/*===========================================================================
FUNCTION gsdi_get_card_mode_capability

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_get_card_mode_capability(
   gsdi_uim_link_established_T *req_p,
   byte * data_p );


/*===========================================================================
FUNCTION gsdi_gsm_proc_select

DESCRIPTION
  populate parameters for gsdi_gsm_proc_select() for the select process,
  EFs are being checked against sim service table to ensure that they are
  avaliable before sending down to the uim driver

DEPENDENCIES
  gsdi_util_is_file_ok_in_sst

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_gsm_proc_select(
   gsdi_select_req_T *  req_p,
  byte*                file_len_p,
  byte*                file_data_p
);


#ifdef FEATURE_MMGSDI_CDMA
/*===========================================================================
  FUNCTION GSDI_CDMA_PROC_SELECT

  DESCRIPTION
    populate parameters for gsdi_uim_server_cdma_select() for the select
    process, EFs are being checked against cdma service table to ensure
    that they are avaliable before sending down to the uim driver

  DEPENDENCIES
    gsdi_util_is_file_ok_in_cst

  RETURN VALUE
    GSDI_SUCCESS on success
    GSDI_NOT_SUPPORTED if operation is not permitted
    GSDI_ERROR general errors.
 ===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_select (
  gsdi_select_req_T *  req_p,
  byte*                file_len_p,
  byte*                file_data_p
);
#endif /* FEATURE_MMGSDI_CDMA */


#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif

/*===========================================================================
FUNCTION gsdi_wait

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type.

SIDE EFFECTS
  None
===========================================================================*/
extern rex_sigs_type  gsdi_wait(
   rex_sigs_type wait_sigs );

/*===========================================================================
FUNCTION gsdi_util_read_message

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_util_read_message(
  void                              * message_p,
  rex_sigs_type                       sigs_rcvd,
  rex_sigs_type                       sigs);

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
extern void gsdi_main_task_init (
 void
);


/*===========================================================================
FUNCTION gsdi_util_send_message

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_util_send_message(
   gs_queue_id_T   queue_id,
   void *          message_p );

/*===========================================================================
FUNCTION gsdi_util_send_gsdi_cnf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_util_send_gsdi_cnf(
   gsdi_returns_T   gsdi_status,
   uint32             client_ref,
   gs_queue_id_T    queue_id,
   byte             rec_num_accessed,
   byte             num_records,
   int32            returned_data_len,
   byte *           data_p,
   rex_tcb_type *   returned_tcb_p,
   rex_sigs_type    return_wait_sig,
   rex_crit_sect_type*  returned_crit_sec_p,
   void *           cmd_p,
   gsdi_resp_type*  resp_type_p);


/*===========================================================================
FUNCTION gsdi_util_send_mmi_sim_ind

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_util_send_mmi_sim_ind(
   gsdi_sim_events_T);


/* ----------------------------------------------------------------------------
   FUNCTION:      GSDI_UTIL_SEND_PIN_STATUS

   DESCRIPTION:
     This function send notification of pin status event to registered gsdi clients
     only

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     gsdi_return_T

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
gsdi_returns_T gsdi_util_send_pin_status(void);


/*===========================================================================
FUNCTION GSDI_UTIL_PARSE_ATR

DESCRIPTION
   This function will parse the ATR.  Parsing will focus on the Card Service
   Data which is an optional Byte in the ATR.  The Card Services is
   introduced in the TLV Tag of 0x31.

   Once this is decoded, parsing stops.  This can be expanded later to
   fully parse the ATR.  For now, only Card Service Data is of interest.

DEPENDENCIES
   None

RETURN VALUE
   void
===========================================================================*/
void gsdi_util_parse_atr(gsdi_atr_results_T *parsed_atr,
                         gsdi_uim_link_established_T *req_p);

/*===========================================================================
FUNCTION gsdi_proc_illegal_sim

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_proc_illegal_sim(
   const gsdi_illegal_sim_req_T *
);

#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
/*===========================================================================
FUNCTION GSDI_INSERT_CB_INTO_CB_ARRAY

DESCRIPTION
   This function is a utility used to insert the callback pointers into
   the callback array maintained by the GSDI.  This will only be available
   when the GSDI is running in Multi-Client Support Mode.

DEPENDENCIES
   NONE

RETURN VALUE
   gsdi_returns_T:  GSDI_SUCCESS or ERROR(s)
===========================================================================*/
extern gsdi_returns_T gsdi_insert_cb_into_cb_array(
    gsdi_event_callback_funct_type *gsdi_callback_funct );

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
void gsdi_util_send_sim_events_to_reg_tasks (
    gsdi_sim_events_T  sim_event,
    boolean            slot1_event,
    boolean            slot2_event );

/*===========================================================================
FUNCTION GSDI_PROC_HANDLE_CALLBACK_REQUEST

DESCRIPTION
   This function will be used to handle all client requests to register
   a callback function.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/

extern gsdi_returns_T gsdi_proc_handle_callback_request(
    const gsdi_register_callback_req_T* cb_req_ptr
);


#endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT */

/*===========================================================================
FUNCTION GSDI_GET_UIM_PROTOCOL

DESCRIPTION
   This function is used by the UIM Server.  It will notify what protocol is
   currently used by the GSDI and is dependent on the SIM/USIM Type

DEPENDENCIES


RETURN VALUE
  uim_protocol_type
===========================================================================*/
extern
uim_protocol_type gsdi_get_uim_protocol( void );


/*===========================================================================
FUNCTION GSDI_UTIL_CLEAR_FILE_FROM_CACHE

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
extern void gsdi_util_clear_file_from_cache(
    uint8                 num_files,
    uim_items_enum_type * sim_filenames_p,
    gsdi_slot_id_type     slot
);


/* ------------------------------------------------------------------------*/
/*                  F E A T U R E     G S D I     G S M                    */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*#ifdef FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_CDMA)
/*===========================================================================
FUNCTION gsdi_util_extract_file_attr

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_util_extract_file_attr(
   gsdi_slot_id_type slot,
   byte *            sim_format_attr,
   byte              file_header_len,
   byte *            gsdi_format_attr
);

#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_CDMA */


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

/* ------------------------------------------------------------------------*/
/*                  F E A T U R E     G S D I     U M T S                  */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

/* ------------------------------------------------------------------------*/
/*                  F E A T U R E     G S D I     C D M A                  */
/*                                                                         */
/*                 F U N C T I O N        P R O T Y P E S                  */
/* ------------------------------------------------------------------------*/

#ifdef FEATURE_MMGSDI_CDMA

#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

/*===========================================================================
FUNCTION GSDI_CDMA_IS_ICC_DCS1800_DF_PRESENT

DESCRIPTION
  This function will test for DF DCS1800 in the Card Inserted.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_cdma_is_icc_dcs1800_df_present ( byte * data_p );

/*===========================================================================
FUNCTION GSDI_CDMA_IS_ICC_CDMA_DF_PRESENT

DESCRIPTION
  This function will test for DF CDMA in the Card Inserted.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_cdma_is_icc_cdma_df_present ( byte * data_p );

/*===========================================================================
FUNCTION GSDI_UTIL_DELETE_RUIM_CACHE

DESCRIPTION
  Function will be used to delete/invalidate the Cached CDMA RUIM Data.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  All Cache Access will fail, causing the requesting task to wait for
  a response from the SIM itself.
===========================================================================*/
void gsdi_util_delete_ruim_cache (gsdi_slot_id_type slot);

/*===========================================================================
FUNCTION gsdi_cdma_post_pin1_init

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_post_pin1_init(
   gsdi_slot_id_type slot);

/*===========================================================================
FUNCTION GSDI_UTIL_GET_RUIM_EF_CACHE_LENGTH

DESCRIPTION
    This function is called to retrieve the length of an EF stored in GSDI
    RUIM Cache.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - EF from cache length found and copied
                   GSDI_NOT_FOUND - EF not found in cache
                   GSDI_NOT_INIT  - EF item was not read during init procs.
===========================================================================*/
extern gsdi_returns_T gsdi_util_get_ruim_ef_cache_length(
    uim_items_enum_type     sim_filename,
    boolean                 pref_slot,
    int*                    length_p);


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
extern boolean gsdi_util_is_file_ok_in_cst(
   uim_items_enum_type  sim_filename,
   boolean              pref_slot);

/*===========================================================================
FUNCTION gsdi_util_read_ruim_cache

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_util_read_ruim_cache(
   uim_items_enum_type  sim_filename,
   boolean              pref_slot,
   byte                 requested_data_len,
   byte                 data_offset,
   byte *               actual_data_len_p,
   byte *               data_p );

/*===========================================================================
FUNCTION gsdi_util_write_ruim_cache

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_util_write_ruim_cache(
  uim_items_enum_type       sim_filename,
  boolean                   pref_slot,
  byte                      data_len,
  byte                      data_offset,
  byte *                    data_p,
  gsdi_data_from_enum_type  data_fr_card );



/*===========================================================================
FUNCTION gsdi_cdma_proc_change_pin

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_change_pin(
    gsdi_change_pin_req_T *  req_p,
    byte *                   data_p
);

/*===========================================================================
FUNCTION gsdi_cdma_proc_disable_pin

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_disable_pin(
  gsdi_disable_pin_req_T *  req_p,
  byte *                    data_p
);

/*===========================================================================
FUNCTION gsdi_enable_proc_enable_pin

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_enable_pin(
    gsdi_enable_pin_req_T *  req_p,
    byte *                   data_p
);


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_PIN_STATUS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/

extern gsdi_returns_T gsdi_cdma_proc_get_pin_status(
   gsdi_get_pin_status_req_T *,
   gsdi_pin_status_T *
);


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_RUIM_READ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_ruim_read(
   gsdi_sim_read_req_T* read_req,
   byte *               rec_num_accessed,
   byte *               num_records,
   byte *               len,
   byte *               data_p );



/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_IMAGE

DESCRIPTION
  get image data in the CDMA card

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_get_image (
   gsdi_get_image_req_T  *req_p,
   int32                 *returned_data_length,
   uint8                 *data_p );


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_RUIM_WRITE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_ruim_write(
   gsdi_sim_write_req_T *, byte * );

/*===========================================================================
FUNCTION gsdi_cdma_proc_unblock_pin

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_unblock_pin(
    gsdi_unblock_pin_req_T *  req_p,
    byte*                     data_p
);


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_VERIFY_PIN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_verify_pin (
  gsdi_verify_pin_req_T *  req_p,
  byte *                   data_p
);


/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_FILE_ATTRIBUTES

DESCRIPTION
  Get the file header information in RUIM

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_get_file_attributes (
  gsdi_get_file_attributes_req_T *  req_p,
  byte *                            data_p
);


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_GET_RUIM_CAPABILITIES

DESCRIPTION
  Return the RUIM capablities

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_get_ruim_capabilities (
  gsdi_slot_id_type slot
);


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_REHABILITATE

DESCRIPTION
  call to the gsdi_uim_server_cdma_rehabilitate function for uim driver
  result

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_rehabilitate (
  gsdi_rehabilitate_req_T *  req_p
);




/*===========================================================================
FUNCTION gsdi_uim_server_cdma_change_chv

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_change_chv(
   gsdi_slot_id_type slot,
   gsdi_pin_ids_T  pin_id,
   byte *          old_pin,
   byte *          new_pin,
   byte *          data_p );


/*===========================================================================
FUNCTION gsdi_uim_server_cdma_disable_chv

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_disable_chv(
    gsdi_slot_id_type slot,
   gsdi_pin_ids_T  pin_id,
   byte *          pin,
   byte *          data_p );


/*===========================================================================
FUNCTION gsdi_uim_server_cdma_enable_chv

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_enable_chv(
    gsdi_slot_id_type slot,
   gsdi_pin_ids_T  pin_id,
   byte *          pin,
   byte *          dat_p );


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_READ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_read(
   gsdi_sim_read_req_T *  req_p,
   byte *                 rec_num_accessed_p,
   byte *                 num_records_p,
   byte *                 returned_data_len_p,
   byte *                 data_p );


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_RUIM_GET_IMAGE

DESCRIPTION
  This function populates the UIM command buffer correctly with an
  ACCESS request and wait synchronously for UIM responses.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_ruim_get_image(
   gsdi_get_image_req_T   *req_p,
   uint8 *                 rec_num_accessed_p,
   uint8 *                 num_records_p,
   int32                  *returned_data_length_p,
   uint8                  *data_p );


  /*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_RUIM_ACCESS

DESCRIPTION
  This function populates the UIM command buffer correctly with an
  ACCESS request and wait synchronously for UIM responses.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_ruim_access(
   gsdi_sim_access_req_T  *req_p,
   uint8 *                 rec_num_accessed_p,
   uint8 *                 num_records_p,
   int32                   *returned_data_length_p,
   uint8                  *data_p );


/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_GET_IMAGE

DESCRIPTION
  This function populates the UIM command buffer correctly with an
  ACCESS request and wait synchronously for UIM responses

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_get_image(
   gsdi_get_image_req_T   *req_p,
   uint8 *                 rec_num_accessed_p,
   uint8 *                 num_records_p,
   int32                  *returned_data_length_p,
   uint8                  *data_p );

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_SELECT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_select (
  byte                message_id,
  gsdi_slot_id_type slot,
  uim_items_enum_type sim_filename,
  uim_file_type       type_of_file,
  uim_dir_type        directory,
  const word *        path,
  byte *              length,
  byte *              response_data_p
);


/*===========================================================================
FUNCTION gsdi_uim_server_cdma_write

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_write(
   gsdi_sim_write_req_T *  req_p,
   byte *                  rec_num_accessed_p );


/*===========================================================================
FUNCTION gsdi_uim_server_cdma_unblock_chv

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_unblock_chv(
   gsdi_slot_id_type slot,
   gsdi_pin_ids_T  pin_id,
   byte *          puk,
   byte *          new_pin,
   byte *          data_p );

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_READ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/

extern gsdi_returns_T gsdi_uim_server_cdma_verify_chv (
  gsdi_slot_id_type slot,
  gsdi_pin_ids_T  pin_id,
  byte *          pin,
  byte *          data_p
);


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_ENABLE_FDN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_enable_fdn (
  gsdi_enable_fdn_req_T *  req_p
);


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CDMA_PROC_DISABLE_FDN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_proc_disable_fdn (
  gsdi_disable_fdn_req_T *  req_p);

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_INVALIDATE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_invalidate (
  gsdi_slot_id_type slot,
  uim_items_enum_type filename
);

/*===========================================================================
FUNCTION gsdi_uim_server_cdma_rehabilitate

DESCRIPTION
  make uim function call to perform rehabilitation on the EF

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_uim_server_cdma_rehabilitate(
   gsdi_slot_id_type slot,
   uim_items_enum_type filename );

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_STORE_ESN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_store_esn (
  gsdi_store_esn_req_T *  req_p
);

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_TERMINAL_PROFILE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
gsdi_returns_T gsdi_uim_server_terminal_profile (
  gsdi_slot_id_type slot
);

/*===========================================================================
FUNCTION GSDI_CDMA_TEST_FOR_DFS

DESCRIPTION
  Function used to determine which DFs are present on RUIM

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cdma_test_for_dfs(
   byte * dfs_present_p,
   byte * data_p );

#ifndef FEATURE_VIRTUAL_SIM
/*===========================================================================
FUNCTION GSDI_NVRUIM_SET_DFS_PRESENT

DESCRIPTION
  Function used to notify NVRUIM Directories present.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_nvruim_set_df_present(
   byte ruim_dfs_presnet );

/*===========================================================================
FUNCTION GSDI_NVRUIM_SET_ESN_USAGE

DESCRIPTION
  Function used to notify CDMA Stack where to retrieve ESN From

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
void gsdi_nvruim_set_esn_usage(
   byte ruim_dfs_presnet );
#endif /* FEATURE_VIRTUAL_SIM */

/*===========================================================================
FUNCTION GSDI_INIT_CDMA_FILE

DESCRIPTION
  This function calls gsdi_cdma_proc_ruim_read based on the passed in file name,
  index mode, slot id and populates the result into the passed in data pointer
  This function also print out error message when the return status for the
  read is not GSDI_SUCCESS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
extern gsdi_returns_T gsdi_init_cdma_file( uim_items_enum_type file,
                                    byte index_mode,
                                    gsdi_slot_id_type  slot,
                                    byte * data);
#endif /* FEATURE_MMGSDI_CDMA */

/*===========================================================================
FUNCTION GSDI_UTIL_USIM_SERVICE_CHECK

DESCRIPTION
  This function check whether the service is available in the usim service
  table.  It check the ust, and / or est if est is available.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_svr_rsp_type
===========================================================================*/
gsdi_returns_T gsdi_util_usim_service_check(
    gsdi_slot_id_type               slot,
    gsdi_svr_rsp_type               *svr_rsp,
    gsdi_services_type              service
);


/*===========================================================================
FUNCTION GSDI_UTIL_GSM_SERVICE_CHECK

DESCRIPTION
  This function check whether the service is available in the gsm service
  table.  It checks if it is availabe in the sst.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_svr_rsp_type
===========================================================================*/
gsdi_returns_T gsdi_util_gsm_service_check(
    gsdi_slot_id_type               slot,
    gsdi_svr_rsp_type               *svr_rsp,
    gsdi_services_type              service
);


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
);


/*===========================================================================
FUNCTION GSDI_CACHE_PIN_STATUS

DESCRIPTION
  This function is called to cache the Status of the following PINs:
  USIM:  PIN1 (Global PIN) & USIM Application PIN2
  GSM:   CHV1 & CHV2

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS   - PINS Successfully cached
                   GSDI_ERROR     - General Failure Condition
===========================================================================*/
gsdi_returns_T gsdi_cache_pin_status( void );


/*===========================================================================
FUNCTION GSDI_UTIL_CONVERT_FILE_PATHS

DESCRIPTION
  This function is called convert the File Paths in the provided data_p
  3F,00,7F,20,FF,FF into a uim_items_enum_type

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_convert_file_paths(
    uint8                 data_len,
    uint8               * data_p,
    uim_items_enum_type * enum_p,
    uint8               * num_files_p
);

/*===========================================================================
FUNCTION GSDI_UTIL_NOTIFY_MODULES

DESCRIPTION
  This function is called notify the modules of the EFs that need to
  be Read Again.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T  gsdi_util_notify_modules(
    gsdi_refresh_ef_module_type * ef_module,
    uint32                        client_ref,
    gsdi_refresh_modes_enum_type  refresh_mode,
    void                          (*gsdi_async_callback)(gsdi_cnf_T*));

/*===========================================================================
FUNCTION GSDI_UTIL_IS_REFRESH_ALLOWED

DESCRIPTION
  This function is used to determine whether or not a REFRESH can be performed

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void  gsdi_util_is_refresh_allowed(
    gsdi_sim_refresh_req_T  * req_p,
    uint8                   * num_files_p,
    uim_items_enum_type     * sim_filenames_p,
    boolean                 * reset_allowed_p,
    boolean                 * fcn_allowed_p);
/*===========================================================================
FUNCTION GSDI_UTIL_NOTIFY_CLIENTS_FCN

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_notify_clients_fcn(
    uim_items_enum_type * sim_filenames_p,
    uint8               * num_files_p);
/*===========================================================================
FUNCTION GSDI_UTIL_CLEAR_CLIENT_NOTIFY

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_util_clear_client_notify(
    boolean               pass_fail,
    uint8                 num_files,
    uim_items_enum_type * file_list_p,
    boolean             * pass_fail_p);

/*===========================================================================
FUNCTION gsdi_util_notifications_still_outstanding

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
boolean gsdi_util_notifications_still_outstanding( void );

/*===========================================================================
FUNCTION GSDI_DISCARD_CNF

DESCRIPTION
  Function will be used to provide a callback for GSDI to complete upon
  servicing a REFRESH TIMER EXPIRE REQUEST that is generated internally
  by GSDI.

  Not much is done with this function at the time.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_discard_cnf (
    gsdi_cnf_T  * gsdi_cnf);

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
);

/*===========================================================================
FUNCTION   GSDI_REGISTER_FOR_FCNS

DESCRIPTION


DEPENDENCIES

RETURNS


SIDE EFFECTS

===========================================================================*/
gsdi_returns_T gsdi_register_for_fcns( void );

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
boolean gsdi_util_is_slot_pref (gsdi_slot_id_type slot,
                                gsdi_technology_type tech,
                                gsdi_slot_id_type *new_slot);

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
gsdi_returns_T gsdi_util_send_card_error(gsdi_slot_id_type slot);

#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
/*===========================================================================
FUNCTION   GSDIDIAG_INIT

DESCRIPTION
  Function is called to init the GSDI DIAG Interface

DEPENDENCIES
  None

RETURNS
  void

SIDE EFFECTS
  Will result allowing the GSDIDIAG Interface accept commands from Diag.

===========================================================================*/
extern void gsdidiag_init(void);
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */


/*===========================================================================
FUNCTION: gsdi_util_map_file_enum_to_gsdi_service_type

DESCRIPTION
  Maps MMGSDI File Enum to MMGSDI Service

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
  gsdi_returns_T gsdi_util_map_file_enum_to_gsdi_service_type(
      gsdi_file_enum_type gsdi_file_item ,
      gsdi_services_type *gsdi_services_item);


/*===========================================================================
FUNCTION GSDI_ADD_TO_APP_PIN_INFO_TABLE

DESCRIPTION
  Function is called by all the PIN functions () to add the application-pin info
  to the gsdi_app_pin_info_table.

INPUT
  uint8             *app_id,
  gsdi_pin_ids_T     pin_id1,
  uint8             *pin,
  gsdi_pin_status_T  status

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
  gsdi_pin_status_T       status);

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
  gsdi_slot_id_type            slot,
  gsdi_refresh_modes_enum_type refresh_mode,
  boolean                      reselect_app);

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

ETURN VALUE
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
);

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
);

/*===========================================================================

FUNCTION GSDI_PROC_GET_ECC

DESCRIPTION
  -This function will be used to retrieve the Emergency Call Codes retrieved
  by the GSDI during the GSDI & SIM/USIM/RUIM Initialization Procedures.
  -This function will populate a gsdi_ecc_data_T structure passed in
  as a parameter.

INPUT
  gsdi_slot_id_type slot

DEPENDENCIES
  Dependency on Global Variable gsdi_ef_ecc_data_T ecc_data.  This
  structure must before this function can be used.

RETURN VALUE
  gsdi_returns_T.

OUTPUT
  None

===========================================================================*/

extern gsdi_returns_T gsdi_proc_get_ecc(
    gsdi_slot_id_type slot
);
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
extern gsdi_returns_T gsdi_util_map_to_ecc_table(
  gsdi_ef_ecc_data_T *ecc_data_p
  );

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CDMA_SEEK

DESCRIPTION
  This function populates the uim cmd pointer and sends it to
  the uim queue to be processed. The funtion also parses the
  response received from the uim server.

INPUT
  gsdi_slot_id_type   slot,
  uim_items_enum_type file_to_search,
  gsdi_index_modes_T  index_mode,
  byte *              search_string,
  byte                search_string_len,
  byte *              rec_num_len_p,
  byte *              rec_num_accessed_p

OUTPUT
  Populates the uim cmd ptr and puts it in the queue.

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_uim_server_cdma_seek (
  gsdi_slot_id_type   slot,
  uim_items_enum_type file_to_search,
  gsdi_index_modes_T  index_mode,
  byte *              search_string,
  byte                search_string_len,
  byte *              rec_num_len_p,
  byte *              rec_num_accessed_p
);

/*===========================================================================
FUNCTION GSDI_CDMA_PROC_RUIM_SEARCH

DESCRIPTION
  This function is called by gsdi_proc_handle_client_req to handle a
  GSDI_SIM_SEARCH_REQ.

INPUT
  gsdi_sim_search_req_T *  req_p
  byte *  rec_num_accessed_p

OUTPUT
  Invokes the appropriate uim server fucntion to handle the request.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_cdma_proc_ruim_search (
  gsdi_sim_search_req_T *  req_p,
  byte *  rec_num_accessed_p
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
void gsdi_util_free_data_ptr_member(gsdi_task_cmd_data_type* cmd_ptr);


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
  gsdi_task_cmd_data_type * new_cmd_ptr);

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SEND_CMD_TO_UIM_SERVER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
void gsdi_send_cmd_to_uim_server (
    uim_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION GSDI_UTIL_SEND_MMI_SIM_IND_EXT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_util_send_mmi_sim_ind_ext (
  gsdi_sim_events_T  sim_event
);

/*===========================================================================
FUNCTION GSDI_WAIT_FOR_TASKS_TO_REGISTER

DESCRIPTION
  Wait for the tasks to register

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
void gsdi_wait_for_tasks_to_register( void );

/*=============================================================================
FUNCTION MMGSDI_INIT_CACHE_BINARY

DESCRIPTION
  This function will initialize data in the cache for transparent files during
  the initialization process

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
mmgsdi_return_enum_type mmgsdi_init_cache_binary(gsdi_slot_id_type     gsdi_slot,
                                                 mmgsdi_file_enum_type file,
                                                 mmgsdi_data_type     *data_ptr);

#if defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
/*=============================================================================
FUNCTION MMGSDI_INIT_CACHE_RECORD

DESCRIPTION
  This function will initialize data in the cache for record based files
  during the initialization process

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
  mmgsdi_data_type     *data_ptr);
#endif /* FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

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
mmgsdi_return_enum_type mmgsdi_map_gsdi_mmgsdi_slot(
  boolean find_old,
  gsdi_slot_id_type *slot,
  mmgsdi_slot_id_enum_type* new_slot);

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
mmgsdi_return_enum_type mmgsdi_map_gsdi_mmgsdi_event(
  boolean find_old,
  mmgsdi_event_data_type *new_evt_info,
  gsdi_sim_events_T *old_evt
);

/*===========================================================================
FUNCTION GSDI_GSM_ACTIVATE_ONCHIP_SIM

DESCRIPTION
  This function will populate the GSM SIM Cache with hardcoded information
  to allow for Simple MO/MT Calls with NO Authentication capabilities.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
gsdi_returns_T gsdi_gsm_activate_onchip_sim (
  void
);


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
);

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
  gsdi_file_attributes_T *file_attr_ptr);

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
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
  gsdi_pin_status_info_type *pin_status_output_ptr);




/*===========================================================================
FUNCTION gsdi_get_cached_attributes

INPUT
       uim_items_enum_type        item
OUTPUT
       None

DESCRIPTION

RETURN VALUE
  gsdi_attr_cache*           attr
===========================================================================*/
gsdi_attr_cache *  gsdi_get_cached_attributes( uim_items_enum_type item);

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
);
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/

/*===========================================================================
FUNCTION GSDI_UIM_SERVER_CLEAR_UIM_CACHE

DESCRIPTION
  Function that will clear the UIM Cache Selectively or All

INPUT

OUTPUT
  Populates the uim cmd ptr and outs it in the queue.

RETURN VALUE
  void
===========================================================================*/
void gsdi_uim_server_clear_uim_cache( gsdi_slot_id_type slot,
                                      int32 num_file,
                                      uim_items_enum_type* file_list);

/*===========================================================================
FUNCTION GSDI_UTIL_NEED_TO_TAKE_DOWN_CALL_STACK

DESCRIPTION
  This function will be used to check if the file list provided requires
  the GSDI Task to take down the call stack.

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  The callstack needs to be taken down.
            FALSE: The callstack does not need to be taken down.
===========================================================================*/
 boolean gsdi_util_need_to_take_down_call_stack(
    uint8 num_files,
    uim_items_enum_type * file_list_p
);
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
boolean gsdi_is_perso_completed(
  void
);



#if defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */


#if defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
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
  gsdi_technology_type tech_type
);


/*===========================================================================
FUNCTION GSDI_UTIL_DELETE_PATH_LOOKUP_TABLE

DESCRIPTION:
   This function deletes the entry in the path look up table

DEPENDENCIES
  None

RETURN VALUE
  None.
===========================================================================*/
void gsdi_util_delete_path_lookup_table (
  gsdi_slot_id_type slot
);
#endif /* FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */


#ifdef FEATURE_L4
#error code not present
#endif /* FEATURE_L4 */


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
  gsdi_slot_id_type slot);


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
  gsdi_slot_id_type slot,
  byte data,
  nvruim_esn_usage_type *usage_type);


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
  gsdi_slot_id_type slot);
#endif /* FEATURE_UIM_EUIMID && FEATURE_MMGSDI_CDMA*/

/*===========================================================================
FUNCTION GSDI_UTIL_NOTIFY_CLIENTS_TO_VOTE

DESCRIPTION
  This function is used to determine whether or not the protocol stack can be
  brought down.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_notify_clients_to_vote(
    uim_items_enum_type * sim_filenames_p,
    uint8                 num_files,
    boolean               request_to_take_down_callstack
);


/*===========================================================================
FUNCTION GSDI_UTIL_CAN_GSDI_DO_FILE_NOTIFYS

DESCRIPTION
  This function notifies clients if the refresh mode is FCN
DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_util_can_gsdi_do_file_notify(
    gsdi_refresh_vote_node_type * head_node_p,
    boolean                     * ok_to_do_file_notify_p
);


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
boolean gsdi_is_gcf_test_mode_activated(void);
#endif /* FEATURE_MMGSDI_MCC_VERIFICATION */


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
  gsdi_async_cb        async_cb
);


/*===========================================================================
FUNCTION GSDI_IS_ENS_ENABLED_IN_NV

DESCRIPTION
  This function checks if ens is enabled in NV.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
===========================================================================*/
boolean gsdi_is_ens_enabled_in_nv(void);


#endif  /* GSDI_H */

