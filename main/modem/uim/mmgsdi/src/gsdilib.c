/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S D I   L I B R A R Y


GENERAL DESCRIPTION

  This source file contains the API GSDI library interface functions.

EXTERNALIZED FUNCTIONS
  gsdi_init
    Allows certain parameters to be set for gsdi initialization.

  gsdi_file_select

  gsdi_get_file_attributes

  gsdi_get_sim_capabilities

  gsdi_sim_search

  gsdi_sim_gsm_seek

  gsdi_usim_umts_search

  gsdi_sim_read

  gsdi_get_image

  gsdi_sim_write

  gsdi_sim_increase

  gsdi_get_pin_status

  gsdi_verify_pin

  gsdi_change_pin

  gsdi_enable_pin

  gsdi_disable_pin

  gsdi_unblock_pin

  gsdi_run_gsm_algorithm

  gsdi_usim_authentication

  gsdi_illegal_sim

  gsdi_register_callback_function

  gsdi_enable_fdn

  gsdi_disable_fdn

  gsdi_set_cache

  gsdi_force_sim_read

  gsdi_sim_read_ext

  gsdi_sim_gsm_seek

  gsdi_usim_umts_search

  gsdi_register_callback_function

  gsdi_get_ef_length

  gsdi_sim_refresh

  gsdi_naa_refresh

  gsdi_register_for_refresh_voting

  gsdi_register_for_file_change_notifications

  gsdi_refresh_complete

  gsdi_naa_refresh_complete

  gsdi_get_user_preferred_slot

  gsdi_nas_get_card_mode

  gsdi_sim_get_atr

  gsdi_sim_open_channel

  gsdi_sim_close_channel

  gsdi_sim_send_channel_data

  gsdi_perso_ota_deperso

  gsdi_perso_register_task

  gsdi_perso_activate_feature_indicator

  gsdi_perso_deactivate_feature_indicator

  gsdi_perso_get_perm_feature_indicators

  gsdi_perso_perm_disable_feature_indicator

  gsdi_perso_get_feature_indicators

  gsdi_perso_set_feature_data

  gsdi_perso_get_feature_data

  gsdi_perso_get_dck_num_retries

  gsdi_perso_change_dck

  gsdi_perso_unblock_feature_indicator

  gsdi_sim_secure_store

  gsdi_sim_secure_read

  gsdi_perso_read_root_key

  gsdi_perso_write_root_key

  gsdi_get_lib_version

  gsdi_perso_verify_phonecode

  gsdi_perso_change_phonecode

  gsdi_perso_unblock_phonecode

  gsdi_perso_set_phonecode

  gsdi_sim_generic_search

  gsdi_get_ecc

  gsdi_get_phonebook_reference_table

  gsdi_perso_get_phonecode_status

  gsdi_activate_onchip_sim

  gsdi_get_all_pin_status_info

  gsdi_perso_get_feature_indicator_key

  When FEATURE_MMGSDI_DUAL_SLOT is defined

  gsdi_get_user_preferred_slot

  gsdi2_enable_fdn

  gsdi2_disable_fdn

  gsdi2_get_file_attributes

  gsdi2_get_pin_status

  gsdi2_verify_pin

  gsdi2_sim_read

  gsdi2_sim_write

  gsdi2_disable_pin

  gsdi2_enable_pin

  gsdi2_unblock_pin

  gsdi2_change_pin

  gsdi2_switch_slot_preference

  gsdi2_sim_refresh

  When FEATURE_MMGSDI_TEST_AUTOMATION is defined.

  gsdi2_send_apdu

  When FEATURE_MMGSDI_CPHS is defined.

  gsdi_sim_get_cphs_information


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/sqa/mmgsdi/src/11/gsdilib.c#2 $ $DateTime: 2010/11/04 05:20:19 $ $Author: rmandala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/10   shr     Fixed input data length check when handling an OTA Deperso
05/22/09   js      Fixed file path logic in gsdi_file_select()
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/29/09   js      Added prototype for static functions
04/15/09   rm      Fix lint error due to disablement of
                   FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC feature
03/25/09   sun     Added support for Virtuim
03/04/09   mib     Get perso keys if FEATURE_MMGSDI_PERSO_GET_KEY not defined
03/03/09   mib     Changed return value of gsdi_sim_get_cphs_information()
                   when CPHS feature is disabled
02/17/09   kk      Removing featurization for get_dck_unblock_retries API
01/20/09   nb      Corrected mispelled Feature Flag
12/05/08   nb      Fixed Compilation warning related to GSDI_READ_TIMER_VALUE.
11/26/08   nb      Removed Featurization Flags for Common Modem Interface and
                   moved gsdi_get_card_mode() function from gsdi.c
10/14/08   nb      Added support to empty the command queue having more than
                   one message in synchronous mode
10/06/08   jk      Allow GSM SIM Access in 1X Only
09/25/08   kk      Added support for extracting unblock dck retries available
08/06/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/22/08   tml     Fixed signal not being clear issue for synchronous calls
06/06/08   tml     Fixed memory leak for synchronous call on CDMA only target
05/12/08   kk      Featurized gsdi_perso_get_feature_data under
                   FEATURE_GSDI_PERSO_GET_DATA
04/30/08   kk      Added API for get_feature_indicator_key under feature
                   definition FEATURE_MMGSDI_PERSO_GET_KEY
04/16/08   kk      Added support of get command on sim lock data
02/06/08   tml     Return not support from perso APIs if feature is not
                   defined
11/15/07   sun     Added support for NAA Refresh
08/10/07   jk      Lock tasks in gsdi_lib_put_cmd_buf
05/08/07   nk      Fixed Featurization in gsdi usim authenticate
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
03/28/07   sun     Fixed Buffer overflow
03/19/07   sun     Fixed Lint Error
03/06/07   nk      Added pin validation check to pin operations.
02/21/07   sun     Redirect get_pin_status to MMGSDI
10/02/06   sp      Changed me supports fdn from gsdi to mmgsdi global data
09/12/06   jk      Lint errors
09/11/06   jk      Changed featurization so that 7200 and 7500 generate identical
                   RPC Files
08/25/06   jar     Added a function to convert a Detailed GSDI Card Error Evt
                   into a generic Error Evt.
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/16/06   wli     Fixed a bug that may cause memory leak.
08/07/06   sun     Diverted GSDI PIN APIS to MMGSDI
07/07/06   sun     Lint Fixes
06/30/06   tml     Lint fixes for L4 build
05/23/06   wli     Fixed a bug that may cause memory leak.
05/15/06   tml     Fixed compilation issue
05/03/06   pv      Set the signal to the waiting task in gsdi_lib_put_cmd_buf
                   when FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC is defined
04/05/06   tml     Lint
03/30/06   tml     Fixed compilation errors
03/29/06   sun     If the control key length is incorrect, then the key should
                   should be set to FFs and sent to the security library and
                   change gsdi_cmd to gsdi_cmd_ext for force sim read
03/10/06   tml     Lint Fixes
03/08/06   jar     Lint Fixes.  Merge from Brnach for HZI and SIM Lock
02/15/06   tml     Lint fix and fixed > 256 cache write
02/16/06   sp      Support for EF_EHPLMN
02/14/06   nk      Merge for: Bring file attribute setting to avoid synchronous
                   file attribute access in the library function that could have
                   caused potential deadlock situation and ARR Caching merge and
                   get fle attribute caching additional support
02/03/06   tml     FIxed compilation error
01/26/06   tml     Fixed get EF cache length not returning more than 256 byte
                   issue
12/09/05   sun     Initialized length to zero in gsdi_get_ef_length
11/28/05   tml     Fixed Run GSM Algorithm message ID
11/22/05   jk      Changes to support multiprocessor RPC
11/09/05   tml     Fixed Lint Errors
11/03/05   tml     Null pointer check
10/28/05   sun     Featurized support for cache file attributes
10/20/05   sun     Added support to cache file attributes
10/14/05   tml     Added utility function for checking if data to be
                   written same as cached data
10/13/05   sun     Return SUCCESS , even if the file was not written successfully.
09/13/05   sun     A different approach to serialization is required for L4.
07/26/05   sst     Fixed lint errors.
07/18/05   jk      Fixed Compile Issues for 1x Targets
07/11/05   sun     Fixed Memory Leaks
06/14/05   jar     Fixed phonebook present logic error
06/10/05   tml     Fixed potential memory null access issue
05/26/05   tml     fixed gsdi_cmd_ext input param
05/11/05   sst     Lint fixes
04/28/05   jar     Release of the GSDI SIM Lock Architecutre to support
                   Plug In Proprietary Security Libraries driven by a Perso Engine.
04/01/05   jar     Enable / Disable Autodog to handle tasks getting into
                   pending states when another task owns the critical section.
03/01/05   tml     Fixed get_image memory issue
02/14/05   sun     Added support for Cingular
02/14/05   tml     Assign asynch cb pointer to the req command
02/09/05   jk      Fixed compilation issue
01/28/05   tml     Fixed compilation issue
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/17/05   jar     Merged back in Channel Management Length check
01/12/05   jk      Fixed some compilation issues when FEATURE_GSTK is off.
01/12/05   jar     Fixed Error in Function gsdi_get_phonebook_reference_table
                   Added Support for GET PBR API
                   Added comments for comments missed when checked in as #63
12/04/04   jar     Fixed gsdi2_phonebook_present library to use the sim
                   capabilities from GSDI obtained during power up.
11/15/04   jar     Dual Slot Changes and Code Clean up
10/18/04   sk      Added defintion for function gsdi_get_ecc().
10/18/04   sk      Added defintion for function gsdi_sim_generic_search().
10/10/04   tml     Cache read/get ef length for CBMID
09/30/04   sk      Corrected UIM_ELP under function gsdi_sim_read
                   to use the gsdi_common_cache for all protocols.
09/29/04   sk      Added case UIM_ELP for features GSM,UMTS and CDMA under
                   function gsdi_sim_read().
09/24/04   jar     Added gsdi_get_cphs_info API
09/20/04   tml     Allowed only AID send APDU to be more than 5 bytes
09/14/04   jar     Return error for function not yet implimented.
                   Compiler Warning Fix.
09/07/04   jar     ATCSIM Change in gsdi_sim_send_channel_data() to allow
                   a channel ID of 0 when FEATURE_MMGSDI_ATCSIM is defined.
09/02/04   jar     Added API GSDI_SIM_GET_CPHS_INFORMATION
08/06/04   sk      Fixed code where open channel id is not set.
08/02/04   jar     Moved Slot Check for Automatic out of FEATURE_MMGSDI_DUAL_SLOT
                   in function gsdi_perso_ota_deperso().
07/07/04   jar     Added Memfree for switch slot pref
06/30/04   tml     Allowed zero bytes of feature data code to be written to
                   the efs
06/24/04   tml     Added additional Perso Functions.
06/16/04   tml     Added Language Preference caching
06/08/04   jar     Added FEATURE_MMGSDI_PERSONALIZATION Support
04/28/04   jar     Added Channel Management Support
04/22/04   jar     Chaged gsdi_get_ef_length to accept an int Pointer as we move
                   to ints instead of bytes to allow for 256+ Byte SIM Accesses.
04/07/04   jar     Added API gsdi_nas_get_card_mode().
03/27/04   tml     Added dog petting for syncrhonous call merge from branch
03/21/04   jar     Fixed compile error found as a result of backing out of fix.
03/20/04   jar     Backed out Fix for Null Pointer access in write.
03/01/04   tml     Fixed null pointer access in write
02/21/03   jar     Added protection in gsdi_get_pin_status() against NULL
                   parameters passed in.
01/26/03   jar     Added an rw_invalidated_deactivated_allowed to
                   gsdi_get_file_attributes_T in gsdi_get_file_attributes();
01/20/04   tml     Refresh dual slot support
11/18/03  jar/tml  Fixed slot info during cache write for non dual slot build
11/05/03   jar     Added Library Functions gsdi_get_user_preferred_slot() &
                   gsdi2_switch_slot_preference().
10/29/03   tml     Compilation fix for CDMA only target
10/28/03   jar     Use GSDI_SLOT_AUTOMATIC in gsdi_lib_utility_sync_sim_and_cache()
10/20/03   jar     Added gsdi2_send_apdu support under feature
                   FEATURE_MMGSDI_TEST_AUTOMATION
10/16/03   jar     Removed references to the obsolete my_cs variable as a
                   result of REX Changes.  Added MSG_MEDs to indicate when we
                   Enter/Leave GSDI Critical Section.
09/29/03   tml     Initialize search_string_len to the passed in value in
                   sim read access
09/11/03   jar     Added additional parameter checking in gsdi lib functions.
09/04/03   tml     Fixed gsdi2 sim write message req id
09/03/03  jar/tml  Fixed GSDI2 LIB IS SERVICE AVAILABLE BITMASK Problem
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT Support
08/23/03   jar     Made gsdi_get_sim_capabilities asnch for FEATURE_MMGSDI_CDMA
                   only builds.
08/18/03   tml     Added required support for coreapp
06/22/03   jar     Removed Compiler Warnings.
                   Fixed issue with Critical Inits in gsdi_lib_send_and_read
06/10/03   jar     Modified Features for rex_clr_timer (gsdi_lib_timer) in
                   gsdi_lib_send_and_read() to avoid clearing an undef timer.
05/18/03   tml     linted.  Added synchronous support for CDMA
05/01/03   jar     Added support for Additional REFRESH FCN Modes
04/28/03   tml     Added image support
04/15/03   tml     Evaluated ERR usage
04/13/03   tml     Added UIM_USIM_KCGPRS in sim read and sim write cache and
                   get ef length
04/10/03   jar     Removed declaration of unused gsdi_cnf_T
02/27/03   jar     Put gsdi_lib_timer under FEATURE_MMGSDI_CDMA.  Added library
                   function gsdi_sim_refresh().
02/24/03   jar     Changed gsdi_lib_utility_sync_ruim_and_cache to the generic
                   function gsdi_lib_utility_sync_sim_and_cache for SIM/USIM/RUIM.
01/23/03   jar     Cleaned up Library File.  Allows SIM Optimization to be used
                   across 1X/GSM/UMTS
01/17/03   jar     Added UIM_GSM_LP to GSM Optimization and UIM_GSM_LI to USIM
                   Optimization.
12/01/02   jar     Fixed Featurization problem preventing SIM Write Optimization
                   from working properly.  Added fix to not read/write cache
                   if Service Table option is disabled.
11/26/02   jar     Renamed gsdi_cdma_proc_get_sim_capabilities to
                   gsdi_cdma_proc_get_ruim_capabilities
11/08/02   jar     Added usage for gsdi_util_is_gsdi_cache_ready()
10/21/02   jar     Added fix to the handling of a Timer Expire in the GSDI
                   Library.
10/14/02   tml     1x, gsm merge
09/18/02   tml     Fixed gsdi_get_file_attributes error message issue
09/14/02  jar/tml  Fixed UTSM to UMTS ERRORS, added gsdi_get_ef_length.
09/03/02   jar     Removed 1 ERR and changed another ERR to MSG_HIGH in
                   gsdi_get_ef_length
08/28/02   jar     Fixed Null Pointer issue in gsdi_sim_read under
                   optimization code.
08/27/02   tml     Changed gs_status to gsdi_status. Set file_attributes->rec_len
                   and file_attributes->num_of_records.
08/21/02   jar     Modified libraries to support new gsdi_cnf_T union.  Required
                   changes in most library functions.
08/12/02   tml     Added num of retries left for pin related api
08/11/02   jar     Added function gsdi_get_ef_length to return the length of
                   a cached EF.  Optimization to avoid a get_file_attributes
                   request for MM and REG.
07/24/02   tml     Added Select api
07/23/02   jar     Added Cache Access for writes.  Added a function call that
                   updates the SIM/USIM with the contents written to cache using
                   the Asynchronous Interface.  This function uses the gsdi
                   function (gsdi_handle_async_lib_rsp) as the callback.
07/17/02   jar     Added Phase II optimization development of Cache Access
                   for Writes.  Added function to Synch Cach and SIM/USIM for
                   write access.
07/13/02   jar     Added additional items to the SIM/USIM reads for items added
                   to SIM/USIM Cache within Optimization Code under FEATURE
                   FEATURE_GSDI_OPTIMIZATION.
07/13/02   jar     Added SIM Optimization Code For SIM Reads under FEATURE
                   FEATURE_GSDI_OPTIMIZATION.
06/29/02   jar     Added a FIX to ensure 0 bytes are not copied over on a Read
                   request when requesting a length of GSDI_STANDARD_READ_LEN
06/10/02   jar     Modified gsdi_sim_read to not copy more data than requested
                   into the client's data buffer.
05/13/02   jar     Modified gsdi_lib_send_and_read to not get items from the
                   buffer until the correct signal is set and an item is
                   in the QUEUE.  Set file_attributes->rec_len in
                   gsdi_get_file_attributes to file_attributes->file_size
                   for a Binary/Transparent EF.
05/01/02   jar     Removed FEATURE_GSDI_PHONEBOOK_SUPPORT for GSM.
04/17/02   jar     Modified gsdi_sim_increase to always index at 0.  Modified
                   method in which data is broken down into bytes.
03/11/02   jar     Added API gsdi_register_callback_function.  This function
                   needs FEATURE_GSDI_MULTICLIENT_SUPPORT defined.
02/28/02   jar     gsdi_get_sim_capabilities now copies phonebook presence
                   data into the structure passed in by the caller.
02/20/02   jar     NULL'ed out msg.message_header.gsdi_async_callback in all
                   relevant functions to avert the async callback ptr from
                   the previous async request geting used for synchrounous
                   calls.  Modified gsdi_sim_gsm_seek function to set message
                   header elements.
02/13/02   jar     Removed FEATURE_GSDI_PHONEBOOK_SUPPORT from around
                   GSDI_SIM_GSM_SEEK and SDI_USIM_UMTS_SEARCH API Functions.
02/08/02   jar     Added Code to insure a memcpy isn't executed for a failed
                   Request.  Fix to CR20356.
02/03/02   jar     Added gsdi_sim_gsm_seek, gsdi_usim_umts_search APIs.
                   Fixed the calculation for num_of_records in
                   gsdi_get_file_attributes.
10/23/01   jc      Add task registration utilities.
10/11/01   jc      Correct queue corruption from race conditions during
                   high traffic through the library.


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Common Libraries */
#include "intconv.h"
#include "customer.h"
#include "memheap.h"
#include "rex.h"
#include "gsdi_exp.h"
#include "gsdilib.h"
#include "gsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "gsdi_convert.h"

#include "tmc.h"

/* UMTS Libraries */
#if defined (FEATURE_MMGSDI_GSM ) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif

#ifdef FEATURE_UIM
#include "uim.h"
#endif

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* ----------------------------------------------------------------------------
   MACRO:       MMGSDI_INIT_LIB
   DESCRIPTION: Will initialize the library if not already
                initialized.
   --------------------------------------------------------------------------*/
#define MMGSDI_INIT_LIB()                           \
  if ( gsdi_lib_initialised == FALSE )              \
  {                                                 \
    if ( gsdi_lib_init() == GSDI_SUCCESS )          \
    {                                               \
        gsdi_lib_initialised = TRUE;                \
    }                                               \
  } /* gsdi_lib_initialized */                      \

#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || \
    defined(FEATURE_MMGSDI_UMTS))
#define GSDI_READ_TIMER_VALUE               25000  /* 25 secs */
#endif /* defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) ||
          defined(FEATURE_MMGSDI_UMTS))*/
/* ----------------------------------------------------------------------------
                GLOBAL DECLARATIONS
   --------------------------------------------------------------------------*/

/* The tcb of the calling task upon timer expiration */
static  rex_tcb_type *timer_expiry_tcb;

/* Command queue for the gsdi library */
q_type gsdi_lib_cmd_q;

/* Has the Library Been Initialized */
static boolean gsdi_lib_initialised = FALSE;

static boolean gsdi_registration_list_initialized = FALSE;

gsdi_registered_task_info_T gsdi_registered_task_list[GSDI_MAX_REGISTERED_TASKS];

#ifdef FEATURE_L4
#error code not present
#endif

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/
static void gsdi_lib_empty_queue(
  q_type *  gsdi_lib_q_ptr
);

static gsdi_returns_T gsdi_lib_init(void);

static gsdi_returns_T gsdi_lib_write_access_cache_using_mmgsdi(
  uim_items_enum_type       file,
  boolean                   pref_slot,
  int                       data_len,
  int                       data_offset,
  byte                    * data_p,
  gsdi_data_from_enum_type  data_fr_card
);

static gsdi_returns_T gsdi_lib_read_access_cache_using_mmgsdi(
  uim_items_enum_type file,
  boolean             pref_slot,
  int                 request_data_len,
  int                 offset,
  int               * ret_data_len_p,
  byte              * data_p
);

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*  (FEATURE_MMGSDI_GSM) || (FEATURE_MMGSDI_UMTS) */

static rex_sigs_type  gsdi_lib_wait (
   rex_tcb_type  *tcb,
   rex_sigs_type wait_sigs
);

static gsdi_returns_T gsdi_lib_send_message (
  gs_queue_id_T  queue_id,
  void *         message_p
);

static gsdi_returns_T gsdi_lib_send_and_read (
  gsdi_task_cmd_data_type *  msg_p,
  gsdi_cnf_T *  gsdi_cnf_p,
  dword         timeout
);

static gsdi_returns_T gsdi_set_cache(
  uim_items_enum_type filename,
  boolean enable
);

#ifdef FEATURE_GSDI_OPTIMIZATION
static void gsdi_lib_mmgsdi_discard_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);
#endif /* FEATURE_GSDI_OPTIMIZATION */

static gsdi_returns_T gsdi_lib_update_sim (
    uim_items_enum_type    uim_filename,
    gsdi_index_modes_T     index_mode,
    int                    rec_num,
    byte *                 data_p,
    int                    data_len,
    int                    data_offset
);


/*===========================================================================
FUNCTION GSDI_LIB_EMPTY_QUEUE

DESCRIPTION
  This API is used to remove any of the stray messages from the queue.

  Parameters
  gsdi_lib_q_ptr - The pointer to the queue to be emptied

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

LIMITATIONS
  None
===========================================================================*/
static void gsdi_lib_empty_queue(
  q_type *  gsdi_lib_q_ptr
)
{
#ifdef FEATURE_Q_SINGLE_LINK
  q_head_link_type  link;
#else
  q_link_type  link;
#endif /* FEATURE_Q_SINGLE_LINK */

  if(gsdi_lib_q_ptr == NULL)
  {
    MSG_HIGH("NULL QUEUE PTR",0,0,0);
    return;
  }

  while(q_cnt(gsdi_lib_q_ptr) != 0)
  {
    link = gsdi_lib_q_ptr->link;
    q_delete(
            #ifdef FEATURE_Q_NO_SELF_QPTR
              gsdi_lib_q_ptr,
            #endif
              (q_link_type *) link.next_ptr
            );
  }
} /* gsdi_lib_empty_queue*() */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_INIT

DESCRIPTION
  This function is called to initialize supporting structures for the GSDI
  library.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static gsdi_returns_T gsdi_lib_init(void)
{
    gsdi_returns_T   gsdi_status = GSDI_SUCCESS; /* For future usage. */

    /* Initialize command queue */
    (void) q_init(&gsdi_lib_cmd_q);

    return gsdi_status;

} /* gsdi_lib_init */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_WRITE_ACCESS_CACHE_USING_MMGSDI

DESCRIPTION
  Wrapper Function generated as a result of switching to using the MMGSDI
  methods of accessing the cache.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static gsdi_returns_T gsdi_lib_write_access_cache_using_mmgsdi(
  uim_items_enum_type       file,
  boolean                   pref_slot,
  int                       data_len,
  int                       data_offset,
  byte                    * data_p,
  gsdi_data_from_enum_type  data_fr_card
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_tech_enum_type   mmgsdi_tech   = MMGSDI_MAX_TECH_ENUM;
  gsdi_item_category_T    EF_category   = ROOT;
  gsdi_returns_T          gsdi_status   = GSDI_ERROR;

  /* --------------------------------------------------------------------------
     Verify all parameters provided are valid
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(data_p);

  /* --------------------------------------------------------------------------
     Calculate the Category Value so that the proper technology can be
     determined to access the orrect cache is made
     ------------------------------------------------------------------------*/
  EF_category = (gsdi_item_category_T)(((word)file >> 8) & 0x0F);

  switch (EF_category)
  {
    case GSM:
      mmgsdi_tech = MMGSDI_TECH_GSM;
      break;

    case USIM:
      mmgsdi_tech = MMGSDI_TECH_UMTS;
      break;

    case CDMA:
      mmgsdi_tech = MMGSDI_TECH_CDMA;
      break;

    default:
      return GSDI_INCORRECT_PARAMS;

  }

  /* --------------------------------------------------------------------------
     Access the Cache
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_write_cache_item(file,
                                              mmgsdi_tech,
                                              pref_slot,
                                              (mmgsdi_len_type)data_len,
                                              (mmgsdi_len_type)data_offset,
                                              (uint8 *)data_p,
                                              data_fr_card);

  /* --------------------------------------------------------------------------
     Convert from an MMGSDI Status to a GSDI Status for the client to handle
     ------------------------------------------------------------------------*/
  (void) mmgsdi_util_convert_to_gsdi_status(mmgsdi_status, &gsdi_status);

  return gsdi_status;

} /* gsdi_lib_write_access_cache_using_mmgsdi */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_READ_ACCESS_CACHE_USING_MMGSDI

DESCRIPTION
  Wrapper Function generated as a result of switching to using the MMGSDI
  methods of accessing the cache.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static gsdi_returns_T gsdi_lib_read_access_cache_using_mmgsdi(
  uim_items_enum_type file,
  boolean             pref_slot,
  int                 request_data_len,
  int                 offset,
  int               * ret_data_len_p,
  byte              * data_p
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_tech_enum_type   mmgsdi_tech   = MMGSDI_MAX_TECH_ENUM;
  gsdi_item_category_T    EF_category   = ROOT;
  gsdi_returns_T          gsdi_status   = GSDI_ERROR;

  /* --------------------------------------------------------------------------
     Verify all parameters provided are valid
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(data_p);

  /* --------------------------------------------------------------------------
     Calculate the Category Value so that the proper technology can be
     determined to access the orrect cache is made
     ------------------------------------------------------------------------*/
  EF_category = (gsdi_item_category_T)(((word)file >> 8) & 0x0F);

  switch (EF_category)
  {
    case GSM:
      mmgsdi_tech = MMGSDI_TECH_GSM;
      break;

    case USIM:
      mmgsdi_tech = MMGSDI_TECH_UMTS;
      break;

    case CDMA:
      mmgsdi_tech = MMGSDI_TECH_CDMA;
      break;

    default:
      return GSDI_INCORRECT_PARAMS;

  }

  /* --------------------------------------------------------------------------
     Access the Cache
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_read_cache_item(file,
                                              mmgsdi_tech,
                                              pref_slot,
                                              (mmgsdi_len_type)request_data_len,
                                              (mmgsdi_len_type)offset,
                                              (mmgsdi_len_type*)ret_data_len_p,
                                              (uint8 *)data_p);

  /* --------------------------------------------------------------------------
     Convert from an MMGSDI Status to a GSDI Status for the client to handle
     ------------------------------------------------------------------------*/
  (void) mmgsdi_util_convert_to_gsdi_status(mmgsdi_status, &gsdi_status);

  return gsdi_status;

} /* gsdi_lib_read_access_cache_using_mmgsdi */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_REGISTER_TASK

DESCRIPTION
  This function is called to initialize supporting structures for the GSDI
  library.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_lib_register_task (
  gsdi_registered_task_info_T  *taskinfo
)
{
#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#else
  (void) taskinfo;
  return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GSM && FEATURE_MMGSDI_UMTS */
} /* gsdi_lib_register_task */




#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_GET_CMD_BUF

DESCRIPTION
  This function if called by the GSDI task to get a buffer from the GSDI
  library queue.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_lib_cmd_type.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gsdi_lib_cmd_type *gsdi_lib_get_cmd_buf (
  void
)
{
    /* Allocate command buffer from the heap */
    return(gsdi_lib_cmd_type*)mmgsdi_malloc(sizeof(gsdi_lib_cmd_type));

} /* gsdi_lib_get_cmd_buf */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_PUT_CMD_BUF

DESCRIPTION
  This function is called by the GSDI task to place a command buffer onto
  the queue of the GSDI library.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_lib_put_cmd_buf (
  gsdi_lib_cmd_type *cmd_ptr
)
{
    rex_tcb_type  *current_tcb;
    //rex_sigs_type wait_sig;  /* For future use */

    /* Check for Null pointer */
    if (cmd_ptr != NULL) {
        /* locking so that no other task runs and deallocates before we have a chance to
        populate the current_tcb pointer (unlikely, but possible scenario) */
        rex_task_lock();
        (void) q_link(cmd_ptr, &cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&gsdi_lib_cmd_q, &cmd_ptr->link);
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
        /* Signal the task that is waiting for this message */
        current_tcb = cmd_ptr->cmd.gsdi_cnf_cmd.message_header.tcb_ptr;
#else
        /* Signal the task that is waiting for this message using the TCB
         * pointer that has been stored for the timer expiry
         */
        current_tcb = timer_expiry_tcb;
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */

        /* Reserved for future use.  Calling task is allowed to specify
           a wait signal */
        // wait_sig = cmd_ptr->cmd.gsdi_cnf_cmd.message_header.wait_sig;

        /* Set the command queue signal for
           receive task.  */
        (void) rex_set_sigs(current_tcb, GSDI_LIB_CMD_Q_SIG);
        rex_task_free();
    } else {
        ERR("cmd_ptr NULL in put_cmd_buf",0,0,0);
    }

} /* gsdi_lib_put_cmd_buf */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_WAIT

DESCRIPTION
  This function is the main wait routine for the GSDI library.

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type.
===========================================================================*/
LOCAL rex_sigs_type  gsdi_lib_wait (
   rex_tcb_type  *tcb,
   rex_sigs_type wait_sigs
)
{
  rex_sigs_type     sigs             = 0x00;
  rex_sigs_type     task_wait_sig    = 0x00;
  dog_report_type   dog_rpt          = 0x00;
  uint8             i = 0;

  if ( tcb == NULL )
  {
    ERR_FATAL("NULL TCB PTR",0,0,0);
  }

  /* Task Context is current and belongs to the task
  ** context currently executing this code.
  */
  if (gsdi_registration_list_initialized)
  {
    /* Need to search the list of registered Tasks */
    for ( i=0; i<GSDI_MAX_REGISTERED_TASKS; i++)
    {
      if ( tcb == gsdi_registered_task_list[i].tasktcb )
      {
        /* TCB Found in Registered Task List.
        ** Retrieve the Wait Sig Registered
        ** and Dog Rpt Value
        */
        task_wait_sig  = gsdi_registered_task_list[i].wait_sig;
        dog_rpt        = gsdi_registered_task_list[i].dog_rpt;

        break;
      }
    }/* for */
  }

  /* Wait for a signal to show up on the tcb's register
  */
  do
  {
    if ( dog_rpt       != 0x00 &&
         task_wait_sig != 0x00
       )
    {
      /* ===================================================
         First report the to the dog using the REPORT
         Registered by the task.  It is important that the
         task that registers uses the correct DOG_RPT Value
         as this function can not verify the correctness of
         it
         ===================================================
      */
      #ifdef FEATURE_DOG

      dog_report(dog_rpt);

      #endif /* FEATURE_DOG */
    }

    /* =====================================================
       Go Into Rex Wait and wait for
       1)  the GSDI Task To Signal Back to the GSDI Library
       Wait Function that it has completed the request
       - or -
       2) Wait for the Signal to be set for DOG_RPT belonging
       to the task put into this wait.
       =====================================================
    */
    (void)rex_wait(wait_sigs | task_wait_sig);

    sigs = rex_get_sigs(tcb);

    /* Is the Dog Report Sig Set */
    if ( sigs & task_wait_sig )
    {

      if (i < GSDI_MAX_REGISTERED_TASKS)
      {
        /* i should be less than GSDI_MAX_REGISTERED_TASKS to avoid memory
           overflow */
        (void)rex_clr_sigs(tcb,task_wait_sig);

        /* Is the Timer Pointer Provided Valid */
        if ( gsdi_registered_task_list[i].rpt_timer != NULL )
        {
          (void)rex_set_timer(gsdi_registered_task_list[i].rpt_timer,
                        gsdi_registered_task_list[i].rpt_timer_value
                        );
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR("NULL RPT TIMER PROVIDED",0,0,0);
        }
      }
    }

  }while ((sigs & wait_sigs) == 0);

  return(sigs);

} /* gsdi_lib_wait */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_SEND_MESSAGE

DESCRIPTION
  This function sends a predefined message to the proper task.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
LOCAL gsdi_returns_T gsdi_lib_send_message (
  gs_queue_id_T  queue_id,
  void *         message_p
)
{
    gsdi_task_cmd_type *gsdi_task_cmd_ptr;
    gsdi_returns_T     gsdi_status = GSDI_SUCCESS;
    word               message_length;

    MMGSDI_RETURN_IF_NULL(message_p);
    /* Initialize return value */
    gsdi_status = GSDI_SUCCESS;

    /* Get message length */
    GSDI_GET_IMH_LEN(message_length, (IMH_T*)message_p);

    /* Add in header size */
    message_length += sizeof(gsdi_cmdhdr_T);

    /* Get command buffer from destination task */
    switch (queue_id) {
    /* Message destination is GSDI Task */
    case GS_QUEUE_GSDI_TASK  :

        /* Get a message queue buffer from the gsdi task */
        gsdi_task_cmd_ptr = gsdi_task_get_cmd_buf(message_length);

        if (gsdi_task_cmd_ptr != NULL) {

            /* Copy data to command buffer */
            memcpy(&gsdi_task_cmd_ptr->cmd,
                   message_p,
                   message_length);

            /* Put message in destination task queue */
            gsdi_task_put_cmd_buf(gsdi_task_cmd_ptr);

            /* Set return status */
            gsdi_status = GSDI_SUCCESS;

        } else {
            /* Set return status */
            gsdi_status = GSDI_ERROR;
        }

        break;

    /* All others are an error */
    default :

        /* Set return status */
        gsdi_status = GSDI_ERROR;

        break;
    } /* switch */
    return gsdi_status;

} /* gsdi_lib_send_message */

#ifdef FEATURE_L4
#error code not present
#endif

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_LIB_SEND_AND_READ

DESCRIPTION
  This function sends a request message to the gsdi task and waits for a
  response. During this time, the calling task enters critical section
  and is locked until the response is received.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
LOCAL gsdi_returns_T gsdi_lib_send_and_read (
  gsdi_task_cmd_data_type *  msg_p,
  gsdi_cnf_T *  gsdi_cnf_p,
  dword         timeout
)
{
    gsdi_returns_T       gsdi_status = GSDI_SUCCESS;
    word                 message_length;
    gsdi_lib_cmd_type    *rsp_ptr=NULL;
    rex_sigs_type        sigs = 0;
    int                  current_q_cnt = 0;
#if !defined (FEATURE_MMGSDI_GSM) && !defined (FEATURE_MMGSDI_UMTS)
    rex_timer_type       gsdi_lib_timer;
#endif /* !FEATURE_MMGSDI_GSM && !FEATURE_MMGSDI_UMTS */

    MMGSDI_RETURN_IF_NULL(msg_p);
#ifdef FEATURE_ENHANCED_REX_TASK
    /* Enable the Autodog for the Task in case it is put
    ** into a pending state by Rex as a result of another task
    ** owning the critical section
    */
    (void)rex_autodog_enable(rex_self()->dog_report_val);
#endif /* FEATURE_ENHANCED_REX_TASK */

    /* Set the calling task as entering a critical section */
    MSG_MED("ENTER GSDI CRITICAL SECTION",0,0,0);
#ifndef FEATURE_L4
    rex_enter_crit_sect(&gsdi_crit_sect);
#else
#error code not present
#endif

#ifdef FEATURE_ENHANCED_REX_TASK
    /* Disable the Autodog for the Task because it was able
    ** to exit the "Enter Rex Critical Section Function" thus
    ** getting out of the Pending State.  Current Task will
    ** own the Critical Section.
    */
    (void)rex_autodog_disable();
#endif /* FEATURE_ENHANCED_REX_TASK */
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    /* Save critical section of tcb for return */
#ifndef FEATURE_L4
    ((gsdi_cmdhdr_T*)msg_p)->crit_sec = &gsdi_crit_sect;
#else
#error code not present
#endif
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    /* Turn off a callback response */
    ((gsdi_cmdhdr_T*)msg_p)->gsdi_async_callback = NULL;

    /* Clear the sig register of the calling task */
    (void) rex_clr_sigs(rex_self(), GSDI_LIB_CMD_Q_SIG);

    /* Send message to GSDI Task */
    gsdi_status = gsdi_lib_send_message( GS_QUEUE_GSDI_TASK, (void*)msg_p );

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#else
    if ( gsdi_status == GSDI_SUCCESS )
    {
        timer_expiry_tcb = rex_self();

        /* Need to define the REX Timer */
        (void) rex_def_timer( &gsdi_lib_timer,     /* pointer to a valid timer structure */
                             timer_expiry_tcb,  /* tcb to associate with the timer    */
                             GSDI_LIB_CMD_Q_SIG  /* sigs to set upon timer expiration  */
                             );

        /* Start timer in case GSDI Task does not respond */
        (void) rex_set_timer( &gsdi_lib_timer, timeout );

    }
#endif


    if ( gsdi_status == GSDI_SUCCESS ) {

        /* Q Count should be 0...assume it is */
        /* Stay in this loop until there is   */
        /* An item in the quue.               */

        while ( current_q_cnt == 0 )
        {

            /* Check QUEUE --> F3 Message */
            current_q_cnt = gsdi_lib_cmd_q.cnt;
            MSG_LOW("LIB:  Q_CNT = %x",current_q_cnt,0,0);

            /* Wait for response from GSDI task */
            sigs = gsdi_lib_wait(rex_self(), GSDI_LIB_CMD_Q_SIG);

#if !defined (FEATURE_MMGSDI_GSM) && !defined (FEATURE_MMGSDI_UMTS)
            /* clear the gsdi_lib_timer */
            (void)rex_clr_timer( &gsdi_lib_timer);
#endif /* !FEATURE_MMGSDI_GSM && !FEATURE_MMGSDI_UMTS */

            /* Immediately check the QUEUE Count */
            /* If the current_q_cnt is still 0   */
            /* We will continue to wait for the  */
            /* proper signal set by the GSDI     */
            current_q_cnt = gsdi_lib_cmd_q.cnt;
            MSG_LOW("LIB: Q_CNT = %x",current_q_cnt,0,0);

            /* Error:  A Task Set the Signal GSDI Should have */
            if ( current_q_cnt == 0 )
            {
                ERR("LIB CAUTION: Non GSDI Task Set Sig: %x",GSDI_LIB_CMD_Q_SIG,0,0);
                (void) rex_clr_sigs(rex_self(), GSDI_LIB_CMD_Q_SIG);
             }
        }

        if (sigs & GSDI_LIB_CMD_Q_SIG) {

            /* regardless of whether the command was handled successfully
               or not, since we are leaving the synchronous call processing
               clear the LIB_CMD_Q_SIG */
            (void) rex_clr_sigs(rex_self(), GSDI_LIB_CMD_Q_SIG);

            /* Get the buffer from the library queue */
            if ((rsp_ptr = (gsdi_lib_cmd_type*)q_get(&gsdi_lib_cmd_q)) != NULL) {
#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */

                if (gsdi_lib_cmd_q.cnt != 0) {
                  MMGSDI_DEBUG_MSG_ERROR("lib cmd q not empty",0,0,0);
                  /*
                  ** Since we are operating in synchronous mode, remove any stray
                  ** messages in the queue.
                  */
                  gsdi_lib_empty_queue(&gsdi_lib_cmd_q);
                }


#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))

                message_length = ( ((IMH_T *)&rsp_ptr->cmd)->message_len_lsb + ( 0x100 * ((IMH_T *)&rsp_ptr->cmd)->message_len_msb ) );
#else
                GET_IMH_LEN(message_length, ((IMH_T *)&rsp_ptr->cmd));

#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))*/

                /* Copy data from command queue to local buffer */
                MMGSDI_RETURN_IF_NULL(gsdi_cnf_p);
                memcpy(gsdi_cnf_p, &rsp_ptr->cmd, message_length + sizeof(gsdi_rsphdr_T));

                gsdi_status = GSDI_SUCCESS;

            } else {
                ERR("gsdi_lib_send_and_read NULL q_get rsp_ptr",0,0,0);
                gsdi_status = GSDI_ERROR;
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("gsdi_lib_send_and_read invalid sig received",sigs,0,0);
            gsdi_status = GSDI_ERROR;
        }

    }

    if ( gsdi_status == GSDI_SUCCESS ) {

        MMGSDI_RETURN_IF_NULL(gsdi_cnf_p);

        /* Have we received the expected 'GSDI_CNF' message ? */
        if (((IMH_T *)gsdi_cnf_p)->message_set == MS_TIMER)  {
          ERR("timeout on response",0,0,0);
          gsdi_status = GSDI_ERROR;
        } else if ((((IMH_T *)gsdi_cnf_p)->message_set != MS_GSDI) &&
                  (((IMH_T *)gsdi_cnf_p)->message_id  != GSDI_CNF)) {
            ERR("Unexpected Message Received",0,0,0);
            gsdi_status = GSDI_ERROR;
        }

    } else {
        MMGSDI_DEBUG_MSG_ERROR("GSDI_STATUS from library wait %d",gsdi_status,0,0);

    } /* gsdi_status */

    /* Leave the Critical Section */
    MSG_MED("LEAVING GSDI CRITICAL SECTION",0,0,0);
#ifndef FEATURE_L4
    rex_leave_crit_sect(&gsdi_crit_sect);
#else
#error code not present
#endif

    /* Deallocate the cmd queue */
    MMGSDIUTIL_TMC_MEM_FREE( rsp_ptr);

    return gsdi_status;

} /* gsdi_lib_send_and_read */


/* <EJECT> */

/*lint -e715 me_supports_fdn not used when
FEATURE_MMGSDI_NV_ME_CONFIG is defined */
/*===========================================================================
FUNCTION GSDI_INIT

DESCRIPTION
  This access function sets certain parameters that is important to the
  operation of the GSDI.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdi_init (
  boolean  me_supports_fdn
)
{
#ifndef FEATURE_MMGSDI_NV_ME_CONFIG

  /* Set whether the mobile supports FDN */

  mmgsdi_generic_data.me_capabilities.me_supports_fdn = me_supports_fdn;

#endif /* FEATURE_MMGSDI_NV_ME_CONFIG */

} /* gsdi_init */

/*lint -e715 me_supports_fdn not used when
FEATURE_MMGSDI_NV_ME_CONFIG is defined */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_SIM_CAPABILITIES

DESCRIPTION
  This function returns certain capabilities of the SIM.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_get_sim_capabilities (
    sim_capabilities_T * sim_capabilities,
    uint32               client_ref,
    gsdi_async_cb        async_cb
)
{
  gsdi_get_sim_capabilities_req_T  msg;
  gsdi_returns_T                   gsdi_status;

 #if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)*/
  /* --------------------------------------------------------------------------
     Perform parameter checking
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(sim_capabilities);

  /* --------------------------------------------------------------------------
    Initialize the GSDI Library if it hasn't been initialized already
    -------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* Message Header */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_get_sim_capabilities_req_T),&msg.message_header);

  msg.message_header.message_set  = MS_GSDI;
  msg.message_header.message_id   = GSDI_GET_SIM_CAPABILITIES_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr      = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig     = GSDI_LIB_CMD_Q_SIG;
  msg.message_header.slot         = GSDI_SLOT_1;
  msg.message_header.gsdi_async_callback = async_cb;

  /* Message Contents */
  msg.queue_id                    = GS_QUEUE_GSDI_LIB;
  msg.client_ref                  = client_ref;

  /* --------------------------------------------------------------------------
     If a callback is provided, bypass gsdi_lib_send_and_read() function
     and queue to gsdi task.  Then return success.  Callback will have
     result of command
     ------------------------------------------------------------------------*/
  if (async_cb != NULL)
  {
    gsdi_status = gsdi_cmd_ext(&msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }

    return gsdi_status;
  }

  /* ------------------------------------------------------------------------
     Continue on...this is a synchronous call
     asynch_cb == NULL
     ----------------------------------------------------------------------*/

#if defined (FEATURE_MMGSDI_CDMA) && !defined (FEATURE_MMGSDI_GSM)
  /* ------------------------------------------------------------------------
     1X ONLY BUILD...Reject all Synchronous Calls
     ----------------------------------------------------------------------*/
  return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA && !FEATURE_MMGSDI_GSM */

 #if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*defined (FEATURE_MMGSDI_GSM) || FEATURE_MMGSDI_UMTS*/

} /* gsdi_get_sim_capabilities */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SIM_SEARCH

DESCRIPTION
  This function performs a search on record based files.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_sim_search (
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  char *               search_string,
  int                  search_string_len,
  int *                rec_num_accessed_p,
  byte                 rfu1,
  byte                 rfu2,
  uint32               client_ref,
  gsdi_async_cb        async_cb
)
{
  gsdi_sim_search_req_T            msg;
  gsdi_returns_T                   gsdi_status;

 #if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)*/

  /* --------------------------------------------------------------------------
     Perform Basic Parameter checking
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(search_string);

  MMGSDI_RETURN_IF_NULL(rec_num_accessed_p);

  MMGSDI_RETURN_IF_OUT_OF_RANGE(search_string_len,0,GSDI_MAX_SEARCH_STR_LEN);

  /* --------------------------------------------------------------------------
     Has the GSDI lib been initialized yet ?
     ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Populate the Message Header
     ------------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_sim_search_req_T),&msg.message_header);

  msg.message_header.message_set = MS_GSDI;
  msg.message_header.message_id  = GSDI_SIM_SEARCH_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr     = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig    = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
  msg.message_header.slot        = GSDI_SLOT_1;
#else
  msg.message_header.slot        = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  msg.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Message Contents
     ------------------------------------------------------------------------*/
  msg.sim_filename               = (word) sim_filename;
  msg.index_mode                 = index_mode;
  memcpy( msg.search_string, search_string, (uint32)search_string_len );
  msg.search_string_len          = (byte) search_string_len;
  msg.queue_id                   = GS_QUEUE_GSDI_LIB;
  msg.client_ref                 = client_ref;
  msg.rfu1                       = rfu1;
  msg.rfu2                       = rfu2;

  /* --------------------------------------------------------------------------
     If a callback is provided, bypass gsdi_lib_send_and_read() function
     and queue to gsdi task.  Then return success.  Callback will have
     result of command
     ------------------------------------------------------------------------*/
  if (async_cb != NULL)
  {
    gsdi_status = gsdi_cmd_ext(&msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }
    return gsdi_status;
  }

  /* ------------------------------------------------------------------------
     Continue on...this is a synchronous call
     asynch_cb == NULL
     ----------------------------------------------------------------------*/

#if defined (FEATURE_MMGSDI_CDMA) && !defined (FEATURE_MMGSDI_GSM)
  /* ------------------------------------------------------------------------
     1X ONLY BUILD...Reject all Synchronous Calls
     ----------------------------------------------------------------------*/
  return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_CDMA && !FEATURE_MMGSDI_GSM */

 #if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)*/

} /* gsdi_sim_search */

/*===========================================================================
FUNCTION gsdi_set_cache

DESCRIPTION
  Will turn off the init bit in the GSDI_SIM_CACHE or GSDI_USIM_CACHE for the
  appropriate file

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL gsdi_returns_T gsdi_set_cache(
  uim_items_enum_type filename,
  boolean enable
)
{
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_FORCE_SIM_READ

DESCRIPTION
  This function returns the data stored by a specified file.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_force_sim_read(
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  int                  required_data_len,
  int                  data_offset,
  uint32               client_ref,
  gsdi_async_cb        async_cb
)
{
  gsdi_sim_read_req_T  msg;


  /* --------------------------------------------------------------------------
       Basic Parameter checking
  ------------------------------------------------------------------------*/

#ifdef FEATURE_GSDI_OPTIMIZATION

      if (async_cb == NULL)
      {
          return GSDI_ERROR;
      }
#endif /* FEATURE_GSDI_OPTIMIZATION */

    /* ------------------------------------------------------------------------
       Has the GSDI lib been initialized yet ?
       ----------------------------------------------------------------------*/
    MMGSDI_INIT_LIB();


    (void)gsdi_set_cache(sim_filename,FALSE);
    /* ------------------------------------------------------------------------
       Populate Message Header contents
       -----------------------------------------------------------------------*/
    GSDI_PUT_MSG_LEN(sizeof(gsdi_sim_read_req_T),&msg.message_header);
    msg.message_header.message_set            = MS_GSDI;
    msg.message_header.message_id             = GSDI_SIM_READ_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr                = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig               = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot                   = GSDI_SLOT_1;
#else
    msg.message_header.slot                   = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    msg.message_header.gsdi_async_callback    = async_cb;

    /* ------------------------------------------------------------------------
       Message Contents
       ----------------------------------------------------------------------*/
    msg.sim_filename                          = (word) sim_filename;
    msg.index_mode                            = index_mode;
    msg.rec_num                               = (byte) rec_num;
    msg.search_string_len                     = 0;
    msg.required_data_len                     = (byte) required_data_len;
    msg.data_offset                           = (byte) data_offset;
    msg.queue_id                              = GS_QUEUE_GSDI_LIB;
    msg.client_ref                            = client_ref;

    return gsdi_cmd_ext(&msg);
}


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SIM_READ_EXT

DESCRIPTION
  This function allows client to obtain data from the image instance files.
  Client is required to provide the path for the image instance files with
  the path_len specifies as the number of "word" in the path pointer.
  The only uim_items_enum_type allowed for this function is UIM_EF_BY_PATH.
  If client doesn't know the requested number of byte, it can send down 0x00,
  which GSDI will internally translate that to maximum size access.
  The get image API supports asynchronous access at this time.  Thus, client
  is required to provide a non null asynchronous callback function pointer.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_sim_read_ext (
    gsdi_slot_id_type    slot,
    uim_items_enum_type  sim_filename,
    gsdi_index_modes_T   index_mode,
    int                  rec_num,
    int                  required_data_len,
    int                  data_offset,
    uint32               client_ref,
    gsdi_async_cb        async_cb

)
{
    gsdi_sim_access_req_T  *msg = NULL;
    gsdi_returns_T         gsdi_status = GSDI_SUCCESS;

    /* check input params */

    /* support async at the moment */
    if (async_cb == NULL)
    {
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE )
    {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* get memory for access request type */
    msg = mmgsdi_malloc(sizeof(gsdi_sim_access_req_T));
    if(msg == NULL)
    {
        return GSDI_ERROR;
    }

    /* Populate Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_SIM_ACCESS_REQ;
    ((IMH_T *)&(msg->message_header))->message_len_msb =
        (unsigned char)((sizeof( *msg ) - sizeof(gsdi_cmdhdr_T)) / 0x100);
    ((IMH_T *)&(msg->message_header))->message_len_lsb =
        (unsigned char)((sizeof( *msg ) - sizeof(gsdi_cmdhdr_T)) % 0x100);
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg->message_header.slot     = GSDI_SLOT_1;
#else
    msg->message_header.slot     = slot;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* Message Contents */
    msg->sim_filename      = (word) sim_filename;

    /* set rec number to 0 since image instance files are transparent files */
    msg->rec_num            = int32touint8(rec_num);
    msg->required_data_len  = int32touint8(required_data_len); /* get the max data */
    msg->file_data_offset   = int32touint8(data_offset);
    msg->queue_id           = GS_QUEUE_GSDI_LIB;
    msg->client_ref         = client_ref;

    msg->message_header.gsdi_async_callback = async_cb;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);
    msg = NULL;
    return gsdi_status;

} /* gsdi_sim_read_ext */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SIM_READ

DESCRIPTION
  This function returns the data stored by a specified file.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_sim_read (
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  char *               search_string,
  int                  search_string_len,
  byte *               data_p,
  int                  required_data_len,
  int                  data_offset,
  int *                rec_num_accessed_p,
  uint32               client_ref,
  gsdi_async_cb        async_cb
)
{
  gsdi_sim_read_req_T  msg;
  gsdi_cnf_T           gsdi_cnf;
  gsdi_returns_T       gsdi_status;
  int                  returned_data_len;

  /* --------------------------------------------------------------------------
     Basic Parameter checking
     ------------------------------------------------------------------------*/
  if ( search_string != NULL )
  {
    MMGSDI_RETURN_IF_OUT_OF_RANGE(search_string_len,0,GSDI_MAX_SEARCH_STR_LEN);
  }
#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
  gsdi_status = gsdi_ef_access_conditions_check(sim_filename,GSDI_SIM_READ_REQ);
  if(gsdi_status != GSDI_SUCCESS)
  {
      MMGSDI_DEBUG_MSG_ERROR("BAD ACCESS_CONDITIONS",0,0,0);
      return GSDI_ERROR;
  }
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/

#ifdef FEATURE_GSDI_OPTIMIZATION

  if (async_cb == NULL)
  {
      MMGSDI_RETURN_IF_NULL(data_p);

      /* Initialize gsdi_status */
      gsdi_status = GSDI_ERROR;

      switch ( sim_filename )
      {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
      /* Cached RUIM EFs */
      case UIM_CDMA_PREF_LANG:
          /* Special Case...allow cache read */
          /* eventhouh GSDI Initi is not     */
          /* completed.                      */
          if ( gsdi_util_is_file_ok_in_cst(sim_filename, TRUE) )
          {
              /* GSDI Completed Caching Data */
              MSG_HIGH("CACHE READ %x LEN %x OFFSET %x", sim_filename,
                                                         required_data_len,
                                                         data_offset);

              gsdi_status = gsdi_lib_read_access_cache_using_mmgsdi( sim_filename,
                                                       TRUE,
                                                       required_data_len,
                                                       data_offset,
                                                       &returned_data_len,
                                                       data_p );
              if (gsdi_status != GSDI_SUCCESS)
              {
                  MSG_HIGH("CACHE READ FAILED: %x FILE: %x",gsdi_status,sim_filename,0);
              }
              return gsdi_status;
          }
         /* Service Table Check Failed */
          MSG_HIGH("CACHE READ NOT ALLOWED: %x",sim_filename,0,0);
          /* Exit this function immediately */
          return GSDI_ERROR;

      case UIM_CDMA_CDMA_SVC_TABLE:
      case UIM_CDMA_ECC:
      case UIM_CDMA_IMSI_M:
      case UIM_CDMA_IMSI_T:
          /* Need to check GSDI Status        */
          /* to determine whether GSDI    */
          /* has completed init procs     */
          if ( gsdi_util_is_gsdi_cache_ready() &&
               gsdi_util_is_file_ok_in_cst(sim_filename, TRUE) )
          {
              /* GSDI Completed Caching Data */
              MSG_HIGH("CACHE READ %x LEN %x OFFSET %x", sim_filename,
                                                         required_data_len,
                                                         data_offset);

              gsdi_status = gsdi_lib_read_access_cache_using_mmgsdi( sim_filename,
                                                       TRUE,
                                                       required_data_len,
                                                       data_offset,
                                                       &returned_data_len,
                                                       data_p );
              if (gsdi_status != GSDI_SUCCESS)
              {
                  MSG_HIGH("CACHE READ FAILED: %x FILE: %x",gsdi_status,sim_filename,0);
              }
              return gsdi_status;
          }
          /* GSDI Is still Initializing */
          /* or                         */
          /* Service Table Check Failed */
          MSG_HIGH("CACHE READ NOT INIT OR NOT ALLOWED: %x",sim_filename,0,0);
          /* Exit this function immediately */
          return GSDI_ERROR;

#endif /* FEATURE_MMGSDI_CDMA */
      /* Files under MF that exist for different card
      ** technologies
      */
      case UIM_ELP:
          gsdi_status = gsdi_util_read_common_cache( sim_filename,
                                                    TRUE,
                                                    (uint8)required_data_len,
                                                    (uint8)data_offset,
                                                    (byte*)&returned_data_len,
                                                    data_p );

          if (gsdi_status != GSDI_SUCCESS)
          {
              MSG_HIGH("CACHE READ FAILED: 0x%x FILE: 0x%x",gsdi_status,sim_filename,0);
          }
          return gsdi_status;

      default:
          MSG_MED("CASE NOT MET FOR CACHE READ %x",sim_filename,0,0);
          break;

      }
  } /* NULL */
#endif /* FEATURE_GSDI_OPTIMIZATION */

  /* ------------------------------------------------------------------------
     Has the GSDI lib been initialized yet ?
     ----------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* ------------------------------------------------------------------------
     Verify the data_p for the buffer is not NULL
     ----------------------------------------------------------------------*/
  if ( async_cb == NULL )
  {
    MMGSDI_RETURN_IF_NULL(data_p);
    MMGSDI_RETURN_IF_NULL(rec_num_accessed_p);
  }

  /* ------------------------------------------------------------------------
     Populate Message Header contents
     -----------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_sim_read_req_T),&msg.message_header);
  msg.message_header.message_set            = MS_GSDI;
  msg.message_header.message_id             = GSDI_SIM_READ_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr                = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig           = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
  msg.message_header.slot               = GSDI_SLOT_1;
#else
  msg.message_header.slot                   = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  msg.message_header.gsdi_async_callback    = async_cb;

  /* ------------------------------------------------------------------------
     Message Contents
     ----------------------------------------------------------------------*/
  msg.sim_filename                          = (word) sim_filename;
  msg.index_mode                            = index_mode;
  msg.rec_num                               = (byte) rec_num;

  if ( search_string != NULL )
  {
      memcpy( msg.search_string,
              search_string,
              (uint32)search_string_len );
  }
  msg.search_string_len                     = int32touint8(search_string_len);
  msg.required_data_len                     = (byte) required_data_len;
  msg.data_offset                           = (byte) data_offset;
  msg.queue_id                              = GS_QUEUE_GSDI_LIB;
  msg.client_ref                            = client_ref;

  if (async_cb != NULL)
  {
    gsdi_status = gsdi_cmd_ext(&msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Send message to GSDI task and get reply
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                       &gsdi_cnf,
                                       GSDI_READ_TIMER_VALUE );

  if (gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Verify the Message Returned and populate
     structure provided.
     ------------------------------------------------------------------------*/
  if ( ( gsdi_cnf.message_header.message_set != MS_GSDI  ) ||
       ( gsdi_cnf.message_header.message_id  != GSDI_CNF ) )
  {
    MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x"
                           ,gsdi_cnf.message_header.message_set,0,0);
    MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",
                           gsdi_cnf.message_header.message_id,0,0);;
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Set the status based on what was returned in the cnf
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cnf.message_header.gsdi_status;

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("GSDI LIB: Read request Failed 0x%x",gsdi_status,0,0);
    /*lint -esym(613,rec_num_accessed_p)*/
    *rec_num_accessed_p = 0;
    /*lint +esym(613,rec_num_accessed_p)*/
    return gsdi_status;
  }

  MMGSDI_RETURN_IF_NULL(data_p);
  /* --------------------------------------------------------------------------
     Examine the length requested
     ------------------------------------------------------------------------*/
 if ( required_data_len == GSDI_STANDARD_READ_LEN )
 {
   /* Client is requesting maximum number of bytes    */
   /* Client should have buffer of 256 bytes          */
   memcpy ( data_p,
            gsdi_cnf.read_cnf.data,
            gsdi_cnf.read_cnf.returned_data_len );
 }
 else if ( gsdi_cnf.read_cnf.returned_data_len > required_data_len )
 {
   /* Returned data length is greater than client's   */
   /* request.  Only copy amount client requested     */
   memcpy ( data_p,
            gsdi_cnf.read_cnf.data,
            (uint32)required_data_len );
 }
 else
 {
   /* Data retrieved is less than data requested       */
   memcpy( data_p,
           gsdi_cnf.read_cnf.data,
           gsdi_cnf.read_cnf.returned_data_len );
 }

 /* ---------------------------------------------------------------------------
    Populate the record number accessed
    -------------------------------------------------------------------------*/
 /*lint -esym(613,rec_num_accessed_p)*/
 *rec_num_accessed_p = gsdi_cnf.read_cnf.rec_num_accessed;
 /*lint +esym(613,rec_num_accessed_p)*/

 return gsdi_status;

} /* gsdi_sim_read */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_IMAGE

DESCRIPTION
  This function allows client to obtain data from the image instance files.
  Client is required to provide the path for the image instance files with
  the path_len specifies as the number of "word" in the path pointer.
  The only uim_items_enum_type allowed for this function is UIM_EF_BY_PATH.
  If client doesn't know the requested number of byte, it can send down 0x00,
  which GSDI will internally translate that to maximum size access.
  The get image API supports asynchronous access at this time.  Thus, client
  is required to provide a non null asynchronous callback function pointer.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_get_image (
    uim_items_enum_type    sim_filename,
    word                 * path,
    uint8                  path_len,
    word                   image_file_offset,
    word                   requested_num_byte,
    word                 * returned_data_len,
    uint8                * data,
    uint32                 client_ref,
    gsdi_async_cb          async_cb
)
{
    gsdi_get_image_req_T  *msg;
    gsdi_returns_T         gsdi_status = GSDI_SUCCESS;

    /* check input params */
    /* only allow image instance by path referencing */
    if ( sim_filename != UIM_EF_BY_PATH ||
         path         == NULL           ||
         path_len     == 0)
    {
        return GSDI_ERROR;
    }

    if ( returned_data_len == NULL ||
         data              == NULL)
    {
       MSG_HIGH("null returned data len/data", 0, 0, 0);
       return GSDI_ERROR;
    }

    /* support async at the moment */
    if (async_cb == NULL)
    {
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE )
    {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* get memory for access request type */
    msg = mmgsdi_malloc(sizeof(gsdi_get_image_req_T));
    if(msg == NULL)
    {
        return GSDI_ERROR;
    }

    /* Populate Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_GET_IMAGE_REQ;
    ((IMH_T *)&(msg->message_header))->message_len_msb =
        (unsigned char)((sizeof( *msg ) - sizeof(gsdi_cmdhdr_T)) / 0x100);
    ((IMH_T *)&(msg->message_header))->message_len_lsb =
        (unsigned char)((sizeof( *msg ) - sizeof(gsdi_cmdhdr_T)) % 0x100);
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg->message_header.slot     = GSDI_SLOT_1;
#else
    msg->message_header.slot     = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* Message Contents */
    msg->sim_filename      = (word) sim_filename; /* should be EF_BY_PATH */
    msg->path              = mmgsdi_malloc(2*path_len);
    if (msg->path == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE( msg);
      return GSDI_ERROR;
    }

    /* copy the path and path len*/
    memcpy(msg->path, path, sizeof(word)*path_len);
    msg->path_len          = path_len;
    /* set rec number to 0 since image instance files are transparent files */
    msg->rec_num           = 0;
    msg->required_data_len = requested_num_byte; /* get the max data */
    msg->file_data_offset  = image_file_offset;
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    /* data pointer didn't get copy since it is not used at this moment */

    msg->message_header.gsdi_async_callback = async_cb;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    /* free access request type */
    MMGSDIUTIL_TMC_MEM_FREE( msg->path);

    MMGSDIUTIL_TMC_MEM_FREE( msg);
    return gsdi_status;

} /* gsdi_get_image */

#ifdef FEATURE_GSDI_OPTIMIZATION
/*===========================================================================
FUNCTION GSDI_LIB_MMGSDI_DISCARD_CB

DESCRIPTION
  This function discard the cb from mmgsdi function call

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static void gsdi_lib_mmgsdi_discard_cb
( mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr)
{
  if (status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("MMGSDI failed for cnf 0x%x with status 0x%x", cnf, status, 0);
  }
}

/*===========================================================================
FUNCTION GSDI_LIB_UPDATE_SIM

DESCRIPTION
  This function call the lib_utility sync sim and cache if the data length
  is less than 256, else it calls the new MMGSDI API to write to the card.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static gsdi_returns_T gsdi_lib_update_sim(
    uim_items_enum_type    uim_filename,
    gsdi_index_modes_T     index_mode,
    int                    rec_num,
    byte *                 data_p,
    int                    data_len,
    int                    data_offset )
{

  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_file_enum_type   mmgsdi_file_name;
  mmgsdi_write_data_type  write_data;
  gsdi_returns_T          gsdi_status   = GSDI_ERROR;
  mmgsdi_tech_enum_type   mmgsdi_tech   = MMGSDI_MAX_TECH_ENUM;
  gsdi_item_category_T    EF_category   = ROOT;
  mmgsdi_len_type         mmgsdi_cache_len = 0;
  uint32                  write_data_len = 0;

  write_data.data_len = 0;
  write_data.data_ptr = NULL;
  /* --------------------------------------------------------------------------
     Calculate the Category Value so that the proper technology can be
     determined to access the orrect cache is made
     ------------------------------------------------------------------------*/
  EF_category = (gsdi_item_category_T)(((word)uim_filename >> 8) & 0x0F);

  switch (EF_category)
  {
    case GSM:
      mmgsdi_tech = MMGSDI_TECH_GSM;
      break;

    case USIM:
      mmgsdi_tech = MMGSDI_TECH_UMTS;
      break;

    case CDMA:
      mmgsdi_tech = MMGSDI_TECH_CDMA;
      break;

    default:
      return GSDI_INCORRECT_PARAMS;

  }

  mmgsdi_status = mmgsdi_util_read_cache_item_len(
                    uim_filename,
                    mmgsdi_tech,
                    TRUE,
                    (mmgsdi_len_type)data_offset,
                    (mmgsdi_len_type*)&mmgsdi_cache_len);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* --------------------------------------------------------------------------
     Convert from an MMGSDI Status to a GSDI Status for the client to handle
     ------------------------------------------------------------------------*/
    (void) mmgsdi_util_convert_to_gsdi_status(mmgsdi_status, &gsdi_status);

    return gsdi_status;
  }

  if (mmgsdi_cache_len < 256)
  {
    return gsdi_lib_utility_sync_sim_and_cache(
                      uim_filename,
                      index_mode,
                      rec_num,
                      data_p,
                      data_len,
                      data_offset );
  }
  else
  {
    mmgsdi_status = mmgsdi_util_convert_from_uim_items_enum(uim_filename,
      &mmgsdi_file_name);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* --------------------------------------------------------------------------
      Convert from an MMGSDI Status to a GSDI Status for the client to handle
      ------------------------------------------------------------------------*/
      (void) mmgsdi_util_convert_to_gsdi_status(mmgsdi_status, &gsdi_status);

      return gsdi_status;
    }
    write_data.data_len = data_len;
    write_data_len      = int32touint32(write_data.data_len);

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(write_data.data_ptr,
                                           write_data_len,
                                           mmgsdi_status);
    if (write_data.data_ptr == NULL)
    {
      return GSDI_ERROR;
    }

    memcpy(write_data.data_ptr, data_p,
           int32touint32(write_data.data_len));
    mmgsdi_status = mmgsdi_write_transparent (
                      mmgsdi_generic_data.client_id,
                      MMGSDI_SLOT_1,
                      mmgsdi_file_name,
                     (mmgsdi_offset_type)data_offset,
                      write_data,
                      gsdi_lib_mmgsdi_discard_cb,
                      0 );
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(write_data.data_ptr);

    /* --------------------------------------------------------------------------
      Convert from an MMGSDI Status to a GSDI Status for the client to handle
    ------------------------------------------------------------------------*/
    (void) mmgsdi_util_convert_to_gsdi_status(mmgsdi_status, &gsdi_status);

    return gsdi_status;
  }
}
#endif /* #ifdef FEATURE_GSDI_OPTIMIZATION */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SIM_WRITE

DESCRIPTION
  This function writes data to a specified file.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_sim_write (
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  char *               search_string,
  int                  search_string_len,
  byte *               data_p,
  int                  data_len,
  int                  data_offset,
  int *                rec_num_accessed_p,
  uint32               client_ref,
  gsdi_async_cb        async_cb
)
{
    gsdi_sim_write_req_T             msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;
    boolean                          same_data = TRUE;

    if ( data_p == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("SOURCE DATA BUFFER NULL (data_p)",data_p,0,0);
        return GSDI_ERROR;
    }

    if ( ( data_len > GSDI_MAX_DATA_BLOCK_LEN )||
         ( data_len < 0 ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("LEN %x EXCEEDS MAX %x",
                  data_len,GSDI_MAX_DATA_BLOCK_LEN,0);
        return GSDI_ERROR;
    }

    if ( ( search_string != NULL ) &&
         ( ( search_string_len > GSDI_MAX_SEARCH_STR_LEN ) ||
           ( search_string_len < 0 ) ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SEARCH & WRITE PARAMETERS",0,0,0);
        return GSDI_ERROR;
    }

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
    gsdi_status = gsdi_ef_access_conditions_check(sim_filename,GSDI_SIM_WRITE_REQ);
    if(gsdi_status != GSDI_SUCCESS)
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD ACCESS_CONDITIONS",0,0,0);
        return GSDI_ERROR;
    }
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/
#ifdef FEATURE_GSDI_OPTIMIZATION
    if ( async_cb == NULL )
    {
        switch ( sim_filename )
        {
        case UIM_ELP:
          gsdi_status = gsdi_util_is_same_cache_synch_data( GSDI_CACHE_COMMON,
                                                    sim_filename,
                                                    TRUE,
                                                    data_len,  /* data_len    */
                                                    0,         /* data_offset */
                                                    data_p,
                                                    &same_data);
          if (gsdi_status != GSDI_SUCCESS)
          {
            MSG_MED("Cache Synch Data Check failed 0x%x, queue the cmd", gsdi_status, 0, 0);
            gsdi_status = GSDI_SUCCESS;
            break;
          }
          if (same_data == FALSE)
          {
            gsdi_status = gsdi_util_write_common_cache( UIM_ELP,
                                                        TRUE,
                                                        int32touint8(data_len),  /* data_len    */
                                                        0,         /* data_offset */
                                                        data_p,
                                                        GSDI_DATA_FROM_CLIENT);
            if ( gsdi_status == GSDI_SUCCESS )
            {
                MSG_MED("CACHE WRITE SUCCESS %x",sim_filename,0,0);

                /* Update the SIM so that it doesn't get out of */
                /* Sync with the Cache.   This will use an      */
                /* an Async Method with the GSDI discarding the */
                /* response.                                    */
                gsdi_status = gsdi_lib_utility_sync_sim_and_cache(  sim_filename,
                                                                    index_mode,
                                                                    rec_num,
                                                                    data_p,
                                                                    data_len,
                                                                    data_offset);

                if(gsdi_status != GSDI_SUCCESS)
                {
                  MSG_ERROR("Could not queue to the file %x Reset status success",sim_filename,0,0);
                  gsdi_status = GSDI_SUCCESS;
                }
                return gsdi_status;
            }
            else
            {
                MSG_MED("CACHE WRITE FAIL 0x%x STATUS: 0x%x",sim_filename,gsdi_status,0);
            }
          }
          else
          {
             /* Same data to be written, no need to process */
             MSG_MED("Same data to be written, no need to process", 0, 0, 0);
             gsdi_status = GSDI_SUCCESS;
             return gsdi_status;
          }
          break;

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
        /* Cached RUIM EFs */
        case UIM_CDMA_CDMA_SVC_TABLE:
        case UIM_CDMA_ECC:
        case UIM_CDMA_IMSI_M:
        case UIM_CDMA_IMSI_T:
        case UIM_CDMA_PREF_LANG:
            /* Need to check GSDI Status        */
            /* to determine whether GSDI    */
            /* has completed init procs     */
            if ( gsdi_util_is_gsdi_cache_ready() &&
                 gsdi_util_is_file_ok_in_cst(sim_filename, TRUE)  )
            {
              gsdi_status = gsdi_util_is_same_cache_synch_data( GSDI_CACHE_CDMA,
                                                      sim_filename,
                                                      TRUE,
                                                      data_len,  /* data_len    */
                                                      0,         /* data_offset */
                                                      data_p,
                                                      &same_data);
              if (gsdi_status != GSDI_SUCCESS)
              {
                MSG_MED("Cache Synch Data Check failed 0x%x, queue the cmd", gsdi_status, 0, 0);
                gsdi_status = GSDI_SUCCESS;
                break;
              }
              if (same_data == FALSE)
              {
                /* Different data to be written */
                MSG_MED("CACHE WRITE %x LEN %x OFFSET %x", sim_filename,
                                                           data_len,
                                                           data_offset);

                gsdi_status = gsdi_lib_write_access_cache_using_mmgsdi (
                                                           sim_filename,
                                                           TRUE,
                                                           data_len,
                                                           data_offset,
                                                           data_p,
                                                           GSDI_DATA_FROM_CLIENT);

                if ( gsdi_status == GSDI_SUCCESS )
                {
                    /* Update the RUIM so that it doesn't get out of*/
                    /* Sync with the Cache.   This will use an      */
                    /* an Async Method with the GSDI discarding the */
                    /* response.                                    */
                    gsdi_status = gsdi_lib_update_sim(
                      sim_filename,
                      index_mode,
                      rec_num,
                      data_p,
                      data_len,
                      data_offset );
                    if(gsdi_status != GSDI_SUCCESS)
                    {
                      MSG_ERROR("Could not queue to the file %x Reset status success",sim_filename,0,0);
                      gsdi_status = GSDI_SUCCESS;
                    }
                    return gsdi_status;
                }
                else
                {
                    MSG_MED("CAN'T WRITE IN CACHE %x, STATUS: %x",sim_filename,
                                                                  gsdi_status,
                                                                  0);
                }
              }
              else
              {
                /* Same data to be written, no need to process */
                MSG_MED("Same data to be written, no need to process", 0, 0, 0);
                gsdi_status = GSDI_SUCCESS;
                return gsdi_status;
              }
            }
            else
            {
                /* GSDI Is still Initializing */
                /* or                         */
                /* Service Table Check Failed */
                MSG_HIGH("CACHE READ NOT INIT OR NOT ALLOWED: %x",sim_filename,0,0);

                /* Exit this function immediately */
                return GSDI_ERROR;
            }
            break;
#endif /* FEATURE_MMGSI_CDMA */

        default:
            MSG_MED("CASE NOT MET FOR FAST WRITE %x",sim_filename,0,0);
            break;

        }
    } /* asynch_cb == NULL */
#endif  /* FEATURE_GSDI_OPTIMIZATION */

    memset((byte *)&msg, 0xFF, sizeof( gsdi_sim_write_req_T ) );

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_SIM_WRITE_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_write_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot     = GSDI_SLOT_1;
#else
    msg.message_header.slot     = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* Message Contents */
    msg.sim_filename      = (word) sim_filename;
    msg.index_mode        = (byte)index_mode;
    msg.rec_num           = (byte) rec_num;
    if (search_string != NULL) {
      memcpy( msg.search_string,search_string,(uint32)search_string_len );
    }
    msg.search_string_len = (byte) search_string_len;
    memcpy( msg.data_buffer, data_p, (uint32)data_len );
    msg.data_len          = (byte) data_len;
    msg.data_offset       = (byte) data_offset;
    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = client_ref;

    if (async_cb != NULL)
    {
      msg.message_header.gsdi_async_callback = async_cb;
      gsdi_status = gsdi_cmd_ext(&msg);
      if(gsdi_status != GSDI_SUCCESS)
      {
        return GSDI_ERROR;
      }
      return gsdi_status;
    }
     else { /* async_cb == NULL */
       /* NULL Out GSDI ASYNC Call Back Ptr */
       msg.message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
          MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.write_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.write_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.write_cnf.message_header.gsdi_status;

            if ( gsdi_status == GSDI_SUCCESS) {
                /* OK to Copy Contents */
                if ( rec_num_accessed_p != NULL ) {
                    rec_num_accessed_p[0] = gsdi_cnf.write_cnf.rec_num_accessed;
                }
            } else {
                /* Don't copy to data_p to avoid overwriting other stack elements */
                MSG_HIGH("GSDI_CNF status %x", gsdi_status,0,0);

            }
        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",
                      gsdi_cnf.write_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",
                      gsdi_cnf.write_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
        }

        return gsdi_status;
     } /* asynch_cb == NULL */

} /* gsdi_sim_write */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SIM_INCREASE

DESCRIPTION
  This function allows a cyclic type file to be increased by a specified
  amount.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_sim_increase (
  uim_items_enum_type   sim_filename,
  dword                 increase_value,
  byte *                new_value_p,
  byte *                new_value_len_p,
  uint32                client_ref,
  gsdi_async_cb         async_cb
)
{
    gsdi_sim_increase_req_T          msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status;

#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
    gsdi_status = gsdi_ef_access_conditions_check(sim_filename,GSDI_SIM_INCREASE_REQ);
    if(gsdi_status != GSDI_SUCCESS)
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD ACCESS_CONDITIONS",0,0,0);
        return GSDI_ERROR;
    }
#endif /*FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES*/
    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_SIM_INCREASE_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_increase_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot     = GSDI_SLOT_1;
#else
    msg.message_header.slot     = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* Message Contents */
    msg.sim_filename      = (word) sim_filename;

    msg.increase_value[2] = (byte)( increase_value & 0x00FF );
    msg.increase_value[1] = (byte)( ( increase_value >> 8  ) & 0x00FF );
    msg.increase_value[0] = (byte)( ( increase_value >> 16 ) & 0x00FF );

    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = client_ref;

    if (async_cb != NULL)
    {
      msg.message_header.gsdi_async_callback = async_cb;
      gsdi_status = gsdi_cmd_ext(&msg);
      if(gsdi_status != GSDI_SUCCESS)
      {
        return GSDI_ERROR;
      }
      return gsdi_status;
    }
     else { /* async_cb == NULL */
       if ((new_value_len_p == NULL) || (new_value_p == NULL))
       {
         MSG_ERROR("Synch but data_p is NULL", 0, 0, 0);
         return GSDI_ERROR;
       }
       /* NULL Out GSDI ASYNC Call Back Ptr */
       msg.message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
          MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status        = gsdi_cnf.message_header.gsdi_status;

            if ( gsdi_status == GSDI_SUCCESS) {
                MMGSDI_RETURN_IF_NULL(new_value_len_p);
                /* OK to Copy Contents */
                new_value_len_p[0] = gsdi_cnf.increase_cnf.new_value_len;
                memcpy( new_value_p,
                        gsdi_cnf.increase_cnf.data,
                        new_value_len_p[0] );

            } else {
                /* Don't copy to data_p to avoid overwriting other stack elements */
                MSG_HIGH("GSDI_CNF status %x", gsdi_status,0,0);

            }
        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
        }

        return gsdi_status;
     } /* asynch_cb == NULL */

} /* gsdi_sim_increase */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_RUN_GSM_ALGORITHM

DESCRIPTION
  This function sends the proper instruction to the GSM SIM to execute
  and return authentication algorithms.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_run_gsm_algorithm (
  byte *         rand,
  byte *         sres,
  byte *         kc,
  uint32         client_ref,
  gsdi_async_cb  async_cb
)
{
    gsdi_run_gsm_algorithm_req_T     msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status;

    if ( rand == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD RAND PARAM: %x",
                  rand,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_RUN_GSM_ALGORITHM_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_run_gsm_algorithm_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot     = GSDI_SLOT_1;
#else
    msg.message_header.slot     = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* Message Contents */
    memcpy( msg.rand, rand, GSDI_RAND_LEN );
    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = client_ref;

    if (async_cb != NULL)
    {
      msg.message_header.gsdi_async_callback = async_cb;
      gsdi_status = gsdi_cmd_ext(&msg);
      if(gsdi_status != GSDI_SUCCESS)
      {
        return GSDI_ERROR;
      }
      return gsdi_status;
    }
     else { /* async_cb == NULL */
       /* NULL Out GSDI ASYNC Call Back Ptr */
       msg.message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
          MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;

            if ( gsdi_status == GSDI_SUCCESS) {
                /* OK to Copy Contents */
                /* SRES */
              if ((sres == NULL) || (kc == NULL))
              {
                MMGSDI_DEBUG_MSG_ERROR("null input pointers sres_ptr: %x, kc_ptr: %x",sres,kc,0);
                gsdi_status = GSDI_ERROR;
              }
              else
              {
                memcpy( sres,
                        gsdi_cnf.run_gsm_alg_cnf.data,
                        GSDI_SRES_LEN );
                /* KC */
                memcpy( kc,
                        gsdi_cnf.run_gsm_alg_cnf.data + GSDI_SRES_LEN,
                        GSDI_KC_LEN   );
              }

            } else {
                /* Don't copy to data_p to avoid overwriting other stack elements */
                MSG_HIGH("GSDI_CNF status %x", gsdi_status,0,0);

            }
        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
        }

        return gsdi_status;
     } /* async_cb == NULL */

} /* gsdi_run_gsm_algorithm */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_USIM_AUTHENTICATION

DESCRIPTION
  This function sends the proper instruction to the UMTS USIM to execute
  and return authentication algorithms.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
/*lint -save -esym(818,rand,autn,data_p) suppress could be const */
gsdi_returns_T gsdi_usim_authentication (
  byte *                    rand,
  byte                      rand_length,
  byte *                    autn,
  byte                      autn_length,
  authentication_context_T  auth_cnt,
  byte *                    data_p,
  uint32                    client_ref,
  gsdi_async_cb             async_cb
)
{
#if defined (FEATURE_MMGSDI_UMTS)
#error code not present
#else
  MMGSDI_DEBUG_MSG_ERROR("gsdi_usim_authenticate not supported",0,0,0);
  return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */
} /* gsdi_usim_authentication */
/*lint -save +esym(818,rand,autn,data_p) suppress could be const */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_FILE_ATTRIBUTES

DESCRIPTION
  This function returns header information of a specific file.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_get_file_attributes (
  uim_items_enum_type      sim_filename,
  gsdi_file_attributes_T * file_attributes,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
)
{
  gsdi_get_file_attributes_req_T   msg;
  gsdi_cnf_T                       gsdi_cnf;
  gsdi_returns_T                   gsdi_status = GSDI_SUCCESS;

  /* Has the GSDI lib been initialized yet ? */
  if ( gsdi_lib_initialised == FALSE )
  {
    gsdi_status = gsdi_lib_init();
    if ( gsdi_status == GSDI_SUCCESS )
    {
      gsdi_lib_initialised = TRUE;
    }
  } /* gsdi_lib_initialized */

  /* Message Header */
  msg.message_header.message_set = MS_GSDI;
  msg.message_header.message_id  = GSDI_GET_FILE_ATTRIBUTES_REQ;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_get_file_attributes_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot     = GSDI_SLOT_1;
#else
    msg.message_header.slot     = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  /* Message Contents */
  msg.sim_filename      = (word) sim_filename;
  msg.queue_id          = GS_QUEUE_GSDI_LIB;
  msg.client_ref        = client_ref;

  if (async_cb != NULL)
  {
    msg.message_header.gsdi_async_callback = async_cb;
    gsdi_status = gsdi_cmd_ext(&msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }
  }
  else
  { /* async_cb == NULL */
    if (file_attributes == NULL)
    {
      MSG_ERROR("synch but null file_attributes ptr", 0, 0, 0);
      return GSDI_ERROR;
    }
    /* NULL Out GSDI ASYNC Call Back Ptr */
    msg.message_header.gsdi_async_callback = NULL;

    /* Send message to GSDI task and get reply */
    gsdi_status = gsdi_lib_send_and_read(
      ( gsdi_task_cmd_data_type*)&msg,&gsdi_cnf, GSDI_TASK_TIMER_VALUE );

    /* Check send and read status */
    if (gsdi_status != GSDI_SUCCESS)
        MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);

    /* Set parameters for return */
    if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
         ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) )
    {
      gsdi_status = gsdi_cnf.message_header.gsdi_status;

      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* OK To Copy Contents */
        file_attributes->file_size                    = gsdi_cnf.gfa_cnf.gfa_data.file_size;
        file_attributes->file_id[0]                   = gsdi_cnf.gfa_cnf.gfa_data.file_id[0];
        file_attributes->file_id[1]                   = gsdi_cnf.gfa_cnf.gfa_data.file_id[1];
        file_attributes->file_type                    = gsdi_cnf.gfa_cnf.gfa_data.file_type;
        file_attributes->cyclic_increase_allowed      = gsdi_cnf.gfa_cnf.gfa_data.cyclic_increase_allowed;
        file_attributes->file_protection.read         = gsdi_cnf.gfa_cnf.gfa_data.file_protection.read;
        file_attributes->file_protection.write        = gsdi_cnf.gfa_cnf.gfa_data.file_protection.write;
        file_attributes->file_protection.increase     = gsdi_cnf.gfa_cnf.gfa_data.file_protection.increase;
        file_attributes->file_protection.invalidate   = gsdi_cnf.gfa_cnf.gfa_data.file_protection.invalidate;
        file_attributes->file_protection.rehabilitate = gsdi_cnf.gfa_cnf.gfa_data.file_protection.rehabilitate;
        file_attributes->file_invalidated             = gsdi_cnf.gfa_cnf.gfa_data.file_invalidated;
        file_attributes->file_structure               = gsdi_cnf.gfa_cnf.gfa_data.file_structure;
        file_attributes->rec_len                      = gsdi_cnf.gfa_cnf.gfa_data.rec_len;
        file_attributes->num_of_records               = gsdi_cnf.gfa_cnf.gfa_data.num_of_records;
        file_attributes->rw_invalidated_deactivated_allowed =
          gsdi_cnf.gfa_cnf.gfa_data.rw_invalidated_deactivated_allowed;
      }
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
      MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
      gsdi_status = GSDI_ERROR;
    }
  } /* if asynch_cb == NULL */
  return gsdi_status;
} /* gsdi_get_file_attributes */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_FILE_SELECT

DESCRIPTION
  This function selects a specific file.
  to select MF:              (UIM_NO_SUCH_FILE, UIM_MF, NULL, 0, size, data_p)
  to select 1st level DF:    (UIM_NO_SUCH_FILE, UIM_DF, UIM_TELECOM_DF, 1, size, data_p)
  to select other level DF:  (UIM_NO_SUCH_FILE, UIM_DF, full_path, path_len, size, data_p)
  to select ADF              (..................UIM_ADF, ...............)
  to select EF with uim enum:(UIM_TELECOM_ADN,  UIM_EF, NULL, 0, size, data_p)
  to select EF by path:      (UIM_EF_BY_PATH,   UIM_EF, full_path, path_len, size, data_p)
                             (if 1st level EF, still need to give FFFF for DF1 and DF2)

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECT
  None
===========================================================================*/
gsdi_returns_T gsdi_file_select (
  uim_items_enum_type     file_id,   /* ignore for directory, otherwise similar to
                                        get_file_attributes*/
  uim_file_type           file_type, /* UIM_MF, UIM_DF, UIM_ADF, UIM_EF */
  word*                   path_p,      /* 3F007F10 etc...*/
  byte                    path_len,
  byte*                   returned_data_len,
  byte*                   returned_data_p,
  uint32                  client_ref,
  gsdi_async_cb           async_cb
)
{
    gsdi_select_req_T                msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status;
    boolean                          first_lvl_df = FALSE;
    int i;

    /* check for path len in case that user supplied the full path for DF or EF */
    if ((file_type == UIM_DF || file_id == UIM_EF_BY_PATH) &&
        (path_len > UIM_MAX_PATH_ELEMENTS || path_len == 0)) {
        MMGSDI_DEBUG_MSG_ERROR(" Select failed: A) UIM_DF && invalid path len ", 0, 0, 0);
        MMGSDI_DEBUG_MSG_ERROR("                B) or UIM_EF_BY_PATH && invalid path len ", 0, 0, 0);
        gsdi_status = GSDI_ERROR;
        return gsdi_status;
    }
    if ((path_len != 0) && (path_p == NULL)) { /* path len and path array don't match */
        MMGSDI_DEBUG_MSG_ERROR(" Select failed: null path pointer with invalid path len", 0, 0, 0);
        gsdi_status = GSDI_ERROR;
        return gsdi_status;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_SELECT_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_select_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
#ifndef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot     = GSDI_SLOT_1;
#else
    msg.message_header.slot     = GSDI_SLOT_AUTOMATIC;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    /* Message Contents */

    /* check for DF input */
    if (file_type == UIM_DF && path_len == 1) { /* is user trying to access 1st DF? */
        if(path_p[0] != UIM_CDMA_DF      &&
           path_p[0] != UIM_GSM_DF       &&
           path_p[0] != UIM_DCS_1800_DF  &&
           path_p[0] != UIM_TELECOM_DF   &&
           path_p[0] != GSDI_IS_41_DF     &&
           path_p[0] != GSDI_FP_CTS_DF ) { /* user didn't select 1st level DF */ /*lint !e613 */

            gsdi_status = GSDI_ERROR;
            return gsdi_status;
        }
        else {
            first_lvl_df = TRUE;
        }

    }


    /*                  file_type, sim_filename[], path_len
      MF:              (UIM_MF,     0xFFFFs,        0 )
      1st level DF:    (UIM_DF,     UIM_TELECOM_DF, 0 )
      other level DF:  (UIM_DF,     path,           path_len )
      ADF              (UIM_ADF,    0xFFFFs,        0 )
      EF with uim enum:(UIM_EF,     UIM_TELECOM_ADN,0 )
      EF by path:      (UIM_EF,     path,           path_len )
    */
    /* assign msg.file_type, msg.sim_filename[], msg.path_len */

    msg.file_type = file_type;

    /* initialized to 0 for path_len and 0xFFFF for filename[] */
    msg.path_len = 0;
    for (i = 0; i < UIM_MAX_PATH_ELEMENTS; i++) {
        msg.sim_filename[i]   = 0xFFFF;
    }

    if (path_p != NULL) {
        if(path_p[0] == 0x3F00) { /* required to shift the path_p */
            for (i = 0; i < path_len - 1; i++) {
                path_p[i] = path_p[i+1];
            }
            path_len--;
            ASSERT(path_len < UIM_MAX_PATH_ELEMENTS);
        }
    }


    /* DF and EF modification */
    switch(file_type) {
    case UIM_DF:
        if(path_p != NULL) {
            for (i = 0; (i < path_len) && (i < UIM_MAX_PATH_ELEMENTS); i++) {
                msg.sim_filename[i] = (word) path_p[i];
            }
            if (!first_lvl_df) {
                msg.path_len = path_len;
            }
        }
        else {
            MMGSDI_DEBUG_MSG_ERROR("UIM_DF null path pt", 0, 0, 0);
            return GSDI_ERROR;
        }
        break;

    case UIM_EF:
        if (file_id != UIM_EF_BY_PATH) {
            msg.sim_filename[0] = (word)file_id;
        }
        else { /* EF by path */
            if(path_p == NULL) {
                MMGSDI_DEBUG_MSG_ERROR("UIM_EF by path null path pt", 0, 0, 0);
                return GSDI_ERROR;
            }
            /* EF should always be in second to last position */
            msg.sim_filename[UIM_MAX_PATH_ELEMENTS-2] = (word) path_p[path_len-1];
            for (i = 0; i < path_len-1; i++) {
                msg.sim_filename[i] = (word) path_p[i];
            }
            msg.path_len = UIM_MAX_PATH_ELEMENTS;
        }
        break;
    default: /* MF and ADF */
        break;
    }

    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = client_ref;

    /* Send message to GSDI task and get reply */

    ASSERT(msg.path_len <= UIM_MAX_PATH_ELEMENTS); /* gsdi parsing error !!!*/

    if (async_cb != NULL)
    {
      msg.message_header.gsdi_async_callback = async_cb;
      gsdi_status = gsdi_cmd_ext(&msg);
      if(gsdi_status != GSDI_SUCCESS)
      {
        return GSDI_ERROR;
      }
      return gsdi_status;
    }

    else { /* async_cb == NULL */
        if ((returned_data_p == NULL) || (returned_data_len == NULL))
        {
          MSG_ERROR("Synch func with null returned_data_p 0x%x or len 0x%x",
            returned_data_p, returned_data_len, 0);
          return GSDI_ERROR;
        }
        /* NULL Out GSDI ASYNC Call Back Ptr */
        msg.message_header.gsdi_async_callback = NULL;

        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                           &gsdi_cnf, GSDI_TASK_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;

            if ( gsdi_status != GSDI_SUCCESS ) {
                MSG_HIGH("GSDI_CNF status %x", gsdi_status,0,0);
            }

            else {
                returned_data_len[0] = gsdi_cnf.select_cnf.returned_data_len;
                                   /* number of bytes of the file */
                memcpy( returned_data_p,
                        gsdi_cnf.select_cnf.data,
                        gsdi_cnf.select_cnf.returned_data_len );
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
        }

        return gsdi_status;
    } /* async_cb == NULL */

} /* gsdi_file_select */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_ENABLE_FDN

DESCRIPTION
  This function enables FDN on a GSM SIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_enable_fdn (
    boolean            pin2_present,
    char *             pin2
)
{
    gsdi_enable_fdn_req_T            msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status;

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_ENABLE_FDN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_enable_fdn_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = GSDI_SLOT_1;
    /* NULL Out GSDI ASYNC Call Back Ptr */
    msg.message_header.gsdi_async_callback = NULL;


    /* Message Contents */
    msg.pin2_present      = pin2_present;
    if (msg.pin2_present) {
      if (pin2 == NULL)
      {
        MSG_ERROR("pin2 null but indicated presented", 0, 0, 0);
        return GSDI_ERROR;
      }
      memcpy( msg.pin2, pin2, GSDI_PIN_LEN );
    }
    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = 0;

    /* Send message to GSDI task and get reply */
    gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                  &gsdi_cnf, GSDI_READ_TIMER_VALUE );

    /* Check send and read status */
    if (gsdi_status != GSDI_SUCCESS) {
        MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
    }

    /* Set parameters for return */
    if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
        ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

        gsdi_status = gsdi_cnf.message_header.gsdi_status;

        if (gsdi_status != GSDI_SUCCESS) {
            MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
        }

    } else {
        MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
        MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
        gsdi_status = GSDI_ERROR;
    }

    return gsdi_status;

} /* gsdi_enable_fdn */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_DISABLE_FDN

DESCRIPTION
  This function disables FDN on a GSM SIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_disable_fdn (
    boolean            pin2_present,
    char *             pin2
)
{
    gsdi_disable_fdn_req_T           msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status;

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_DISABLE_FDN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_disable_fdn_req_T ),
                      &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg.pin2_present      = pin2_present;
    if (pin2_present)
    {
      if (pin2 == NULL)
      {
        MSG_ERROR("pin2 null but indicated presented", 0, 0, 0);
        return GSDI_ERROR;
      }
      memcpy( msg.pin2, pin2, GSDI_PIN_LEN );
    }
    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = 0x00;

    /* NULL Out GSDI ASYNC Call Back Ptr */
    msg.message_header.gsdi_async_callback = NULL;

    /* Send message to GSDI task and get reply */
    gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                  &gsdi_cnf, GSDI_READ_TIMER_VALUE );

    /* Check send and read status */
    if (gsdi_status != GSDI_SUCCESS) {
      MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
    }

    /* Set parameters for return */
    if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
        ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

        gsdi_status = gsdi_cnf.message_header.gsdi_status;

        if (gsdi_status != GSDI_SUCCESS) {
            MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
        }

    } else {
        MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
        MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
        gsdi_status = GSDI_ERROR;
    }

    return gsdi_status;

} /* gsdi_disable_fdn */

/* <EJECT> */
/*===========================================================================
FUNCTION GSDI2_ENABLE_FDN

DESCRIPTION
  This function enables FDN on a GSM SIM for the required slot.
  This function will only support asynchronous calls.  Thus, the caller will
  need to provide a callback function.

    Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  boolean   pin2_present [ INPUT ] - MANDATORY
        Indicate whether pin2 is required to be verified before performing the
        enable FDN command
    3.  char * pin2 [ INPUT ]
        Pointer to char buffer of 8 bytes and only 8 bytes.

    4.  gsdi_app_id_type   aid [ INPUT ] - OPTIONAL
        CARD IS USIM:  NULL Indicates USIM App should be used
                       NOT NULL Assumes valid Application ID is
                       present.  Will use Application ID to select
                       Application and then enable PIN.
        CARD IS GSM:   IGNORED
        CARD IS RUIM:  IGNORED
    5.  client_ref [ INPUT/OUTPUT ]
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    6.  async_cb [ INPUT ]
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi2_enable_fdn (
    gsdi_slot_id_type  slot,
    boolean            pin2_present,
    char *             pin2,
    gsdi_app_id_type * aid,
    uint32             client_ref,
    gsdi_async_cb      async_cb
)
{
    gsdi_enable_fdn_req_T            *msg = 0x00;
    gsdi_returns_T                    gsdi_status = GSDI_ERROR;

    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                      */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
          MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
          return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
          MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
          return GSDI_ERROR;
    }
    msg = mmgsdi_malloc(sizeof(gsdi_disable_fdn_req_T));

    if ( msg == NULL )
    {
          /* FATAL...No Memory to execute request.  Bad Situation. */
          ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_enable_fdn_req_T));
    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            MMGSDIUTIL_TMC_MEM_FREE( msg);
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_ENABLE_FDN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_enable_fdn_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Message Contents */
    msg->pin2_present      = pin2_present;
    if (msg->pin2_present) {
      if (pin2 == NULL)
      {
        MSG_ERROR("pin2 null but indicated presented", 0, 0, 0);
        return GSDI_ERROR;
      }
      memcpy( msg->pin2, pin2, GSDI_PIN_LEN );
    }
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);
    return gsdi_status;

} /* gsdi2_enable_fdn */ /*lint !e715 */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI2_DISABLE_FDN

DESCRIPTION
  This function disables FDN on a GSM SIM for the required slot.
  This function will only support asynchronous calls.  Thus, the caller will
  need to provide a callback function.

    Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  boolean   pin2_present [ INPUT ] - MANDATORY
        Indicate whether pin2 is required to be verified before performing the
    disable FDN command
    3.  char * pin2 [ INPUT ]
        Pointer to char buffer of 8 bytes and only 8 bytes.

    4.  gsdi_app_id_type   aid [ INPUT ] - OPTIONAL
        CARD IS USIM:  NULL Indicates USIM App should be used
                       NOT NULL Assumes valid Application ID is
                       present.  Will use Application ID to select
                       Application and then enable PIN.
        CARD IS GSM:   IGNORED
        CARD IS RUIM:  IGNORED
    5.  client_ref [ INPUT/OUTPUT ]
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    6.  async_cb [ INPUT ]
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi2_disable_fdn (
    gsdi_slot_id_type  slot,
    boolean            pin2_present,
    char *             pin2,
    gsdi_app_id_type * aid,
    uint32             client_ref,
    gsdi_async_cb      async_cb
)
{
    gsdi_disable_fdn_req_T           *msg = 0x00;
    gsdi_returns_T                   gsdi_status = GSDI_ERROR;

    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                      */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
          MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
          return GSDI_ERROR;
    }

  /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
          MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
          return GSDI_ERROR;
    }
    msg = mmgsdi_malloc(sizeof(gsdi_disable_fdn_req_T));

    if ( msg == NULL )
    {
          /* FATAL...No Memory to execute request.  Bad Situation. */
          ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_disable_fdn_req_T));

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            MMGSDIUTIL_TMC_MEM_FREE( msg);
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_DISABLE_FDN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_disable_fdn_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Message Contents */
    msg->pin2_present      = pin2_present;
    if (msg->pin2_present) {
      if (pin2 == NULL)
      {
        MSG_ERROR("pin2 null but indicated presented", 0, 0, 0);
        return GSDI_ERROR;
      }
      memcpy( msg->pin2, pin2, GSDI_PIN_LEN );
    }
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);
    return gsdi_status;

} /* gsdi2_disable_fdn */ /*lint !e715 */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_GET_PIN_STATUS

DESCRIPTION
  This function returns the status of PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_get_pin_status (
  gsdi_pin_ids_T       pin_id,
  gsdi_pin_status_T *  pin_status,
  byte *               num_retry_p,
  uint32               client_ref,
  gsdi_async_cb        async_cb
)
{
  gsdi_get_pin_status_req_T        *msg_ptr      = NULL;
  gsdi_cnf_T                       gsdi_cnf;
  gsdi_returns_T                   gsdi_status   = GSDI_SUCCESS;
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_SUCCESS;

  /* Has the GSDI lib been initialized yet ? */
  if ( gsdi_lib_initialised == FALSE )
  {
    gsdi_status = gsdi_lib_init();

    if ( gsdi_status == GSDI_SUCCESS )
    {
      gsdi_lib_initialised = TRUE;
    }
  } /* gsdi_lib_initialized */

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg_ptr, sizeof(gsdi_get_pin_status_req_T), mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Null msg ptr", 0, 0, 0);
    return GSDI_ERROR;
  }

  /* Message Header */
  msg_ptr->message_header.message_set = MS_GSDI;
  msg_ptr->message_header.message_id  = GSDI_GET_PIN_STATUS_REQ;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_get_pin_status_req_T ), &msg_ptr->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg_ptr->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg_ptr->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
  msg_ptr->message_header.slot = GSDI_SLOT_1;

  /* Message Contents */
  msg_ptr->pin_id          = pin_id;
  msg_ptr->queue_id        = GS_QUEUE_GSDI_LIB;
  msg_ptr->client_ref      = client_ref;

  if (async_cb != NULL)
  {
    msg_ptr->message_header.gsdi_async_callback = async_cb;

    mmgsdi_status = mmgsdi_get_all_pin_status(mmgsdi_generic_data.client_id,
                                              MMGSDI_SLOT_1,
                                              mmgsdi_util_gsdi_cb,
                                              (mmgsdi_client_data_type) msg_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
      MMGSDIUTIL_TMC_MEM_FREE( msg_ptr);
    }
    return gsdi_status;
  }
  else
  { /* async_cb == NULL */
      /* NULL Out GSDI ASYNC Call Back Ptr */
    msg_ptr->message_header.gsdi_async_callback = NULL;

    /* Verify Parameters needed for a Synchronous Call are valid */
    if ( pin_status == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("NULL PARAMETER: pin_status provided",0,0,0);
      return GSDI_ERROR;
    }

    if ( num_retry_p == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("NULL PARAMETER: num_retry_p provided",0,0,0);
      return GSDI_ERROR;
    }
    /* Send message to GSDI task and get reply */
    gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)msg_ptr,
                                     &gsdi_cnf, GSDI_READ_TIMER_VALUE );

    /* Check send and read status */
    if (gsdi_status != GSDI_SUCCESS)
    {
      MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
    }

    /* Set parameters for return */
    if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
         ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) )
    {

      gsdi_status = gsdi_cnf.message_header.gsdi_status;


      if ( gsdi_status == GSDI_SUCCESS )
      {
        /* OK to Copy Contents */
        pin_status[0] = gsdi_cnf.pin_status_cnf.status;
        num_retry_p[0] = gsdi_cnf.pin_status_cnf.num_of_retries;
      }
      else
      {
        /* Don't copy, avoid overwriting other stack elements */
        MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF status %x", gsdi_status,0,0);
      }
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
      MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
      gsdi_status = GSDI_ERROR;
    }
    MMGSDIUTIL_TMC_MEM_FREE( msg_ptr);
    return gsdi_status;
  } /* asynch_cb == NULL */
} /* gsdi_get_pin_status */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_VERIFY_PIN

DESCRIPTION
  This function performs a pin verification procedure against the SIM for
  either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_verify_pin (
  gsdi_pin_ids_T  pin_id,
  char *          pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
)
{
    gsdi_verify_pin_req_T            *msg           = NULL;
    gsdi_cnf_T                        gsdi_cnf;
    gsdi_returns_T                    gsdi_status   = GSDI_SUCCESS;
    mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_data_type                  pin_data;

    if (pin == NULL)
    {
      MSG_ERROR("Null pin ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)pin))
    {
       return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg, sizeof(gsdi_verify_pin_req_T), mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Null msg ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_VERIFY_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_verify_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot = GSDI_SLOT_1;

    /* Message Contents */
    msg->pin_id            = (byte)pin_id;
    memcpy( msg->pin, pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    if (async_cb != NULL) {
        msg->message_header.gsdi_async_callback = async_cb;

        pin_data.data_len = GSDI_PIN_LEN;
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(pin_data.data_ptr,
                                               GSDI_PIN_LEN,
                                               mmgsdi_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          MSG_ERROR("PIN Ptr is NULL",0,0,0);
          return GSDI_ERROR;
        }

        memcpy(pin_data.data_ptr, msg->pin, GSDI_PIN_LEN);

        mmgsdi_status = mmgsdi_verify_pin(mmgsdi_generic_data.client_id,
                                          MMGSDI_SLOT_1,
                                          (mmgsdi_pin_enum_type)mmgsdi_util_convert_from_gsdi_pin_id(pin_id),
                                          pin_data,
                                          mmgsdi_util_gsdi_cb,
                                          (mmgsdi_client_data_type) msg);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          gsdi_status = GSDI_ERROR;
          MMGSDIUTIL_TMC_MEM_FREE( msg);
        }
        MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
        return gsdi_status;
    }
    else { /* async_cb == NULL */
        if (num_retry_left == NULL)
        {
          MSG_ERROR("Synch call with null num_retry_left ptr", 0, 0, 0);
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          return GSDI_ERROR;
        }
        /* NULL Out GSDI ASYNC Call Back Ptr */
        msg->message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;
            num_retry_left[0] = gsdi_cnf.verify_pin_cnf.num_of_retries;

            if (gsdi_status != GSDI_SUCCESS) {
                MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
            num_retry_left[0] = gsdi_cnf.verify_pin_cnf.num_of_retries;
        }
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        return gsdi_status;
    } /* asynch_cb == NULL */

} /* gsdi_verify_pin */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_CHANGE_PIN

DESCRIPTION
  This function attempts to change an old pin (PIN1 or PIN2) with a new pin.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_change_pin (
  gsdi_pin_ids_T  pin_id,
  char *          old_pin,
  char *          new_pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
)
{
    gsdi_change_pin_req_T            *msg           = NULL;
    gsdi_cnf_T                        gsdi_cnf;
    gsdi_returns_T                    gsdi_status   = GSDI_SUCCESS;
    mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_data_type                  pin_data;
    mmgsdi_data_type                  new_pin_data;

    if ((old_pin == NULL) || (new_pin == NULL))
    {
      MSG_ERROR("Null old pin 0x%x or new pin 0x%x ptr", old_pin, new_pin, 0);
      return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if ((!mmgsdi_util_is_pin_valid((uint8 *)old_pin)) ||
        (!mmgsdi_util_is_pin_valid((uint8 *)new_pin)) )
    {
       return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg, sizeof(gsdi_change_pin_req_T), mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Null msg ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* Message header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_CHANGE_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_change_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg->pin_id            = (byte)pin_id;
    memcpy( msg->old_pin, old_pin, GSDI_PIN_LEN );
    memcpy( msg->new_pin, new_pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    if (async_cb != NULL)
    {
      msg->message_header.gsdi_async_callback = async_cb;

      pin_data.data_len = GSDI_PIN_LEN;
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(pin_data.data_ptr,
                                             GSDI_PIN_LEN,
                                             mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        MSG_ERROR("PIN Ptr is NULL",0,0,0);
        return GSDI_ERROR;
      }
      memcpy(pin_data.data_ptr, msg->old_pin, GSDI_PIN_LEN);

      new_pin_data.data_len = GSDI_PIN_LEN;
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(new_pin_data.data_ptr,
                                             GSDI_PIN_LEN,
                                             mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
        MSG_ERROR("PIN Ptr is NULL",0,0,0);
        return GSDI_ERROR;
      }
      memcpy(new_pin_data.data_ptr, msg->new_pin, GSDI_PIN_LEN);

      mmgsdi_status = mmgsdi_change_pin(mmgsdi_generic_data.client_id,
                                        MMGSDI_SLOT_1,
                                        (mmgsdi_pin_enum_type)mmgsdi_util_convert_from_gsdi_pin_id(pin_id),
                                        pin_data,
                                        new_pin_data,
                                        mmgsdi_util_gsdi_cb,
                                       (mmgsdi_client_data_type) msg);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        gsdi_status = GSDI_ERROR;
      }
      MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE( new_pin_data.data_ptr);
      return gsdi_status;
    }
    else { /* async_cb == NULL */
       if (num_retry_left == NULL)
       {
         MSG_ERROR("Synch call with null num_retry_left ptr", 0, 0, 0);
         MMGSDIUTIL_TMC_MEM_FREE( msg);
         return GSDI_ERROR;
       }
       /* NULL Out GSDI ASYNC Call Back Ptr */
       msg->message_header.gsdi_async_callback = NULL;

       /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;
            num_retry_left[0] = gsdi_cnf.change_pin_cnf.num_of_retries;

            if (gsdi_status != GSDI_SUCCESS) {
                MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
            num_retry_left[0] = gsdi_cnf.change_pin_cnf.num_of_retries;
        }
        MMGSDIUTIL_TMC_MEM_FREE(msg);
        return gsdi_status;
    } /* async_cb == NULL */

} /* gsdi_change_pin */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_UNBLOCK_PIN

DESCRIPTION
  This function attempts to unblock either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_unblock_pin (
  gsdi_pin_ids_T  pin_id,
  char *          puk,
  char *          new_pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
)
{
    gsdi_unblock_pin_req_T           *msg           = NULL;
    gsdi_cnf_T                        gsdi_cnf;
    gsdi_returns_T                    gsdi_status   = GSDI_SUCCESS;
    mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_data_type                  pin_data;
    mmgsdi_data_type                  new_pin_data;


    if ((puk == NULL) || (new_pin == NULL))
    {
      MSG_ERROR("Null puk ptr 0x%x or new_pin ptr 0x%x", puk, new_pin, 0);
      return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)new_pin))
    {
       return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg, sizeof(gsdi_unblock_pin_req_T), mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Null msg ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_UNBLOCK_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_unblock_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg->pin_id            = (byte)pin_id;
    memcpy( msg->puk,     puk,     GSDI_PUK_LEN );
    memcpy( msg->new_pin, new_pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    if (async_cb != NULL)
    {
      msg->message_header.gsdi_async_callback = async_cb;

      pin_data.data_len = GSDI_PIN_LEN;
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(pin_data.data_ptr,
                                             GSDI_PIN_LEN,
                                             mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        MSG_ERROR("PIN Ptr is NULL",0,0,0);
        return GSDI_ERROR;
      }
      memcpy(pin_data.data_ptr, msg->puk, GSDI_PIN_LEN);

      new_pin_data.data_len = GSDI_PIN_LEN;
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(new_pin_data.data_ptr,
                                             GSDI_PIN_LEN,
                                             mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
        MSG_ERROR("PIN Ptr is NULL",0,0,0);
        return GSDI_ERROR;
      }
      memcpy(new_pin_data.data_ptr, msg->new_pin, GSDI_PIN_LEN);

      mmgsdi_status = mmgsdi_unblock_pin(mmgsdi_generic_data.client_id,
                                        MMGSDI_SLOT_1,
                                        (mmgsdi_pin_enum_type)mmgsdi_util_convert_from_gsdi_pin_id(pin_id),
                                        pin_data,
                                        new_pin_data,
                                        mmgsdi_util_gsdi_cb,
                                       (mmgsdi_client_data_type) msg);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        gsdi_status = GSDI_ERROR;
      }
      MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE( new_pin_data.data_ptr);
      return gsdi_status;
    }
    else { /* async_cb == NULL */
        if (num_retry_left == NULL)
        {
          MSG_ERROR("Synch call with null num_retry_left ptr", 0, 0, 0);
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          return GSDI_ERROR;
        }
        /* NULL Out GSDI ASYNC Call Back Ptr */
        msg->message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;
            num_retry_left[0] = gsdi_cnf.unblock_pin_cnf.num_of_retries;

            if (gsdi_status != GSDI_SUCCESS) {
                MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
            num_retry_left[0] = gsdi_cnf.unblock_pin_cnf.num_of_retries;
        }

        MMGSDIUTIL_TMC_MEM_FREE( msg);
        return gsdi_status;
    } /* async_cb == NULL */

} /* gsdi_unblock_pin */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_DISABLE_PIN

DESCRIPTION
  This function attempts to disable either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_disable_pin (
  gsdi_pin_ids_T  pin_id,
  char *          pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
)
{
    gsdi_disable_pin_req_T           *msg           = NULL;
    gsdi_cnf_T                        gsdi_cnf;
    gsdi_returns_T                    gsdi_status   = GSDI_SUCCESS;
    mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_data_type                  pin_data;

    if (pin == NULL)
    {
      MSG_ERROR("Null pin ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)pin))
    {
       return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg, sizeof(gsdi_disable_pin_req_T), mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Null msg ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_DISABLE_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_disable_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg->pin_id            = (byte)pin_id;
    memcpy( msg->pin, pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    if (async_cb != NULL) {
        msg->message_header.gsdi_async_callback = async_cb;

        pin_data.data_len = GSDI_PIN_LEN;
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(pin_data.data_ptr,
                                             GSDI_PIN_LEN,
                                             mmgsdi_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          MSG_ERROR("PIN Ptr is NULL",0,0,0);
          return GSDI_ERROR;
        }
        memcpy(pin_data.data_ptr, msg->pin, GSDI_PIN_LEN);

        mmgsdi_status = mmgsdi_disable_pin(mmgsdi_generic_data.client_id,
                                           MMGSDI_SLOT_1,
                                           (mmgsdi_pin_enum_type)mmgsdi_util_convert_from_gsdi_pin_id(pin_id),
                                           pin_data,
                                           mmgsdi_util_gsdi_cb,
                                          (mmgsdi_client_data_type) msg);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          gsdi_status = GSDI_ERROR;
        }
        MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
        return gsdi_status;
    }
    else { /* async_cb == NULL */
        if (num_retry_left == NULL)
        {
          MSG_ERROR("Synch call with null num_retry_left ptr", 0, 0, 0);
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          return GSDI_ERROR;
        }

        /* NULL Out GSDI ASYNC Call Back Ptr */
        msg->message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;
            num_retry_left[0] = gsdi_cnf.disable_pin_cnf.num_of_retries;

            if (gsdi_status != GSDI_SUCCESS) {
                MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
            num_retry_left[0] = gsdi_cnf.disable_pin_cnf.num_of_retries;
        }
        MMGSDIUTIL_TMC_MEM_FREE( msg);
        return gsdi_status;
    } /* async_cb == NULL */

} /* gsdi_disable_pin */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_ENABLE_PIN

DESCRIPTION
  This function attempts to enable either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_enable_pin (
  gsdi_pin_ids_T  pin_id,
  char *          pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
)
{
    gsdi_enable_pin_req_T            *msg           = NULL;
    gsdi_cnf_T                        gsdi_cnf;
    gsdi_returns_T                    gsdi_status   = GSDI_SUCCESS;
    mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_data_type                  pin_data;


    if (pin == NULL)
      return GSDI_ERROR;

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)pin))
    {
       return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(msg, sizeof(gsdi_enable_pin_req_T), mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("Null msg ptr", 0, 0, 0);
      return GSDI_ERROR;
    }

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_ENABLE_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_enable_pin_req_T ),  &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg->pin_id            = (byte)pin_id;
    memcpy( msg->pin, pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    if (async_cb != NULL) {
        msg->message_header.gsdi_async_callback = async_cb;

        pin_data.data_len = GSDI_PIN_LEN;
        MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(pin_data.data_ptr,
                                             GSDI_PIN_LEN,
                                             mmgsdi_status);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          MSG_ERROR("PIN Ptr is NULL",0,0,0);
          return GSDI_ERROR;
        }
        memcpy(pin_data.data_ptr, msg->pin, GSDI_PIN_LEN);

        mmgsdi_status = mmgsdi_enable_pin(mmgsdi_generic_data.client_id,
                                           MMGSDI_SLOT_1,
                                           (mmgsdi_pin_enum_type)mmgsdi_util_convert_from_gsdi_pin_id(pin_id),
                                           pin_data,
                                           mmgsdi_util_gsdi_cb,
                                          (mmgsdi_client_data_type) msg);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          gsdi_status = GSDI_ERROR;
        }
        MMGSDIUTIL_TMC_MEM_FREE( pin_data.data_ptr);
        return gsdi_status;
    }
    else { /* async_cb == NULL */
        if (num_retry_left == NULL)
        {
          MSG_ERROR("Synch call with null num_retry_left ptr", 0, 0, 0);
          MMGSDIUTIL_TMC_MEM_FREE( msg);
          return GSDI_ERROR;
        }

        /* NULL Out GSDI ASYNC Call Back Ptr */
        msg->message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;
            num_retry_left[0] = gsdi_cnf.enable_pin_cnf.num_of_retries;

            if (gsdi_status != GSDI_SUCCESS) {
                MSG_HIGH("GSDI_CNF status %x",gsdi_status,0,0);
            }

        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
            num_retry_left[0] = gsdi_cnf.enable_pin_cnf.num_of_retries;
        }

        MMGSDIUTIL_TMC_MEM_FREE( msg);
        return gsdi_status;
    } /* asynch_cb == NULL */

} /* gsdi_enable_pin */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_ILLEGAL_SIM

DESCRIPTION
  This function is called to mark the SIM as an illegal SIM.  Subsequent,
  request to the GSDI will be blocked.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
void gsdi_illegal_sim (
  boolean         illegal_sim
)
{
    gsdi_illegal_sim_req_T   msg;
    gsdi_returns_T           gsdi_status = GSDI_SUCCESS;

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id   = GSDI_ILLEGAL_SIM_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_illegal_sim_req_T ), &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg.illegal_sim = illegal_sim;

    /* NULL Out GSDI ASYNC Call Back Ptr */
    msg.message_header.gsdi_async_callback = NULL;

    /* Send message to GSDI task */
    gsdi_status = gsdi_lib_send_message( GS_QUEUE_GSDI_TASK, &msg );

    /* Check send and read status */
    if (gsdi_status != GSDI_SUCCESS) {
        MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send error %x",gsdi_status,0,0);
    }

} /* gsdi_illegal_sim */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_SIM_GSM_SEEK

DESCRIPTION
  This function will perform a GSM Seek.  The type of seek and method
  in which it is executed will be dependent on the parameters passed
  when this API Is Called

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_sim_gsm_seek (uim_items_enum_type  sim_filename,
    gsdi_index_modes_T   index_mode,
    char *               search_string,
    int                  search_string_len,
    int *                rec_num_accessed_p,
    byte                 rfu1,
    byte                 rfu2,
    uint32               client_ref,
    gsdi_async_cb        async_cb)
{
#ifdef FEATURE_MMGSDI_GSM
    gsdi_sim_search_req_T            msg;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_returns_T                   gsdi_status;

    if ( search_string == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL SEARCH STRING: %x",search_string,0,0);
        return GSDI_ERROR;
    }

    if ( search_string_len > GSDI_MAX_SEARCH_STR_LEN ||
         search_string_len < 0 )
    {
        MMGSDI_DEBUG_MSG_ERROR("SEARCH STRING LEN: %x EXCEEDS MAX: %x",
                  search_string_len,GSDI_MAX_SEARCH_STR_LEN,0);
        return GSDI_ERROR;
    }
    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_SIM_SEARCH_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_search_req_T ), &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = GSDI_SLOT_1;

    /* Message Contents */
    msg.sim_filename       = (word) sim_filename;
    msg.index_mode         = index_mode;
    memcpy( msg.search_string, search_string, (uint32)search_string_len );
    msg.search_string_len  = (byte) search_string_len;
    msg.queue_id           = GS_QUEUE_GSDI_LIB;
    msg.client_ref         = client_ref;
    msg.rfu1               = rfu1;
    msg.rfu2               = rfu2;
    msg.search_mode        = (byte)UIM_SIMPLE_SRCH;
    msg.search_rec_num       = 0;
    msg.search_direction   = (byte)UIM_SRCH_FORWARD;

    if (async_cb != NULL) {
        msg.message_header.gsdi_async_callback = async_cb;
        gsdi_status = gsdi_cmd_ext(&msg);
        if(gsdi_status != GSDI_SUCCESS)
        {
          gsdi_status = GSDI_ERROR;
        }
        return gsdi_status;
    }
    else { /* async_cb == NULL */
        /* NULL Out GSDI ASYNC Call Back Ptr */
        msg.message_header.gsdi_async_callback = NULL;

        /* Send message to GSDI task and get reply */
        gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                       &gsdi_cnf, GSDI_READ_TIMER_VALUE );

        /* Check send and read status */
        if (gsdi_status != GSDI_SUCCESS) {
            MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
        }

        /* Set parameters for return */
        if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
             ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

            gsdi_status = gsdi_cnf.message_header.gsdi_status;

            if ( gsdi_status == GSDI_SUCCESS) {
                /* OK to Copy Contents */
                if ( rec_num_accessed_p != NULL ) {
                    rec_num_accessed_p[0] = gsdi_cnf.search_cnf.rec_num_accessed;

                }
            } else {
                /* Don't copy, avoid overwriting other stack elements */
                MSG_HIGH("GSDI_CNF status %x", gsdi_status,0,0);

            }
        } else {
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message set %x",gsdi_cnf.message_header.message_set,0,0);
            MMGSDI_DEBUG_MSG_ERROR("GSDI_CNF error message id  %x",gsdi_cnf.message_header.message_id,0,0);
            gsdi_status = GSDI_ERROR;
        }

        return gsdi_status;
    } /* async_cb == NULL */
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_GSM */
} /* gsdi_sim_gsm_seek */ /* lint !e715 */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_USIM_UMTS_SEARCH
DESCRIPTION
    This function will perform a USIM UMTS Search.  The type of search and method
    in which it is executed will be dependent on the parameters passed
    when this API Is Called

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_usim_umts_search (
    uim_items_enum_type  sim_filename,
    gsdi_index_modes_T   index_mode,
    int                  search_record_num,
    uim_srch_mode        search_mode,
    uim_srch_dir_type    search_direction,
    char *               search_string,
    int                  search_string_len,
    int *                rec_num_accessed_p,
    byte                 rfu1,
    byte                 rfu2,
    uint32               client_ref,
    gsdi_async_cb        async_cb)
{
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_UMTS */ /* lint !e715 */
} /* gsdi_usim_umts_search */


/*===========================================================================
FUNCTION GSDI_REGISTER_CALLBACK_FUNCTION

DESCRIPTION
    This function will be used by all tasks that need SIM Events generated by
    the SIM or GSDI.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_register_callback_function (
    gsdi_event_callback_funct_type *gsdi_callback_function_to_register,
    uint32               client_ref,
    gsdi_async_cb        async_cb
)
{
#ifdef FEATURE_GSDI_MULTICLIENT_SUPPORT
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    gsdi_cnf_T                       gsdi_cnf;
    gsdi_register_callback_req_T     msg;

    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_REGISTER_CALLBACK_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_register_callback_req_T ), &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = GSDI_SLOT_1;

    /* NULL Out GSDI ASYNC Call Back Ptr */
    msg.message_header.gsdi_async_callback = NULL;

    /* Save the client's reference value */
   /// msg.gsdi_callback_to_register.client_ref = client_ref;

    /* Message Contents */
    if ( gsdi_callback_function_to_register == NULL )
    {
        gsdi_status = GSDI_ERROR;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
       ERR_FATAL("NULL Pointer %x", (uint32)msg.message_header.tcb_ptr,0,0);
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    }
    else
    {
        msg.gsdi_callback_to_register = gsdi_callback_function_to_register;
    }

    /* Send Message to GSDI QUEUE, Wait for Response */
    gsdi_status = gsdi_lib_send_and_read((gsdi_task_cmd_data_type*)&msg,
                                   &gsdi_cnf, GSDI_READ_TIMER_VALUE );

    /* Check send and read status */
    if (gsdi_status != GSDI_SUCCESS) {
      MMGSDI_DEBUG_MSG_ERROR("GSDI LIB send and read error %x",gsdi_status,0,0);
    }

    if ( ( gsdi_cnf.message_header.message_set == MS_GSDI ) &&
         ( gsdi_cnf.message_header.message_id  == GSDI_CNF ) ) {

         gsdi_status = gsdi_cnf.message_header.gsdi_status;

    }

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_GSDI_MULTICLIENT_SUPPORT */
} /* gsdi_register_callback_function *//*lint !e715 */


#ifdef FEATURE_GSDI_OPTIMIZATION
/*===========================================================================
FUNCTION GSDI_LIB_UTILITY_SYNC_SIM_AND_CACHE

DESCRIPTION
  This function when called will be used to access the cach for read and
    writes.  On a successful access, a corresponding Async Message is
    generated to also update the SIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_lib_utility_sync_sim_and_cache(
    uim_items_enum_type    sim_filename,
    gsdi_index_modes_T     index_mode,
    int                    rec_num,
    byte *                 data_p,
    int                    data_len,
    int                    data_offset

)
{
    gsdi_sim_write_req_T msg;
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;

    MSG_MED("Sending Async Command to GSDI",0,0,0);

    if (data_p == NULL)
      return GSDI_ERROR;

    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_SIM_WRITE_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_write_req_T ), &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;

    /* Set the Slot Information to GSDI_SLOT_AUTOMATIC
    ** to allow GSDI to determine which slot it should
    ** go to based on subscription information used
    */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    msg.message_header.slot     = GSDI_SLOT_AUTOMATIC;
#else
    msg.message_header.slot     = GSDI_SLOT_1;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    /* Provide Callback handler
    */
    msg.message_header.gsdi_async_callback = gsdi_handle_async_lib_rsp;

    /* Message Contents
    */
    msg.sim_filename      = (word) sim_filename;
    msg.index_mode        = (byte)index_mode;
    msg.rec_num           = (byte) rec_num;;
    msg.search_string_len = NULL;
    memcpy( msg.data_buffer, data_p, (uint32)data_len );
    msg.data_len          = (byte) data_len;
    msg.data_offset       = (byte) data_offset;
    msg.queue_id          = GS_QUEUE_GSDI_LIB;
    msg.client_ref        = 0x00;


    gsdi_status = gsdi_cmd_ext(&msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    return gsdi_status;

} /* gsdi_lib_utility_sync_sim_and_cache */

#endif /* FEATURE_GSDI_OPTIMIZATION */

/*===========================================================================
FUNCTION GSDI_GET_EF_LENGTH

DESCRIPTION
  This function is part of GSDI OPTIMIZATION and should not be used unless
  it is known that the item for which you seek out a length for resides in
  gsdi_sim_cache or gsdi_usim_cache.

  When called, it will search the SIM Cache or USIM Cache for the item
  whose length needs to be determined.  This gets around blocking calls for
  gsdi_get_file_attributes.

  If the return is not successful, the client should follow up with a
  get_file_attributes to retrieve a length.

DEPENDENCIES
  EF should reside in GSDI SIM/USIM Cache

RETURN VALUE

  GSDI_SUCCESS:  EF was found in Cache
                 EF was successful
                 Retrieval of length was successful and length returned is usable

  GSDI_NOT_SUPPORTED:  EF is not in SIM/USIM Cache.

  GSDI_ERROR:    EF was found in Cache
                 EF was not Initialized due to not being properly cached
===========================================================================*/
gsdi_returns_T gsdi_get_ef_length (

    /* EF whose length needs to be determed */
    uim_items_enum_type sim_filename,

    /* Pointer to Length Byte to populate */
    int*               length_p
)
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;

    if(length_p == NULL)
       return GSDI_ERROR;

    *length_p = 0;

    /* Determine if the EF is a cached item */
    switch ( sim_filename )
    {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_CDMA
        /* Cached RUIM EFs */
        case UIM_CDMA_CDMA_SVC_TABLE:
        case UIM_CDMA_ECC:
        case UIM_CDMA_IMSI_M:
        case UIM_CDMA_IMSI_T:
        case UIM_CDMA_PREF_LANG:
        MSG_HIGH("GET RUIM CACHE EF %x length ", sim_filename,
                                                 0,
                                                 0);


        gsdi_status = gsdi_util_get_ruim_ef_cache_length( sim_filename,
                                                          TRUE,
                                                          length_p);
#endif /* FEATURE_MMGSDI_CDMA */

    default:
        MSG_MED("CASE NOT MET FOR GET CACHE LEN: %x",sim_filename,0,0);
        break;

    }

    /* Now check the Status of the */
    /* cache access.  If success   */
    /* return the results to the   */
    /* client                      */
    if ( gsdi_status == GSDI_SUCCESS )
    {
        MSG_MED("CACHE GET LEN SUCESS %x",sim_filename,0,0);
        return gsdi_status;
    }
    else
    {
        MSG_HIGH("USE GET FILE ATTRIBUTES ",0,0,0);

    }
    return gsdi_status;
} /*gsdi_get_ef_length*/

/*===========================================================================
FUNCTION GSDI_LIB_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function check whether the service is available in the different technology's
  service table.  It call the corresponding utility functions based on the card moe
  returned from the gsdi_get_card_mode().  The return struct has a gsdi_returns_T and
  bitmap that are populated in correspondence to the check.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_svr_rsp_type
===========================================================================*/
gsdi_svr_rsp_type gsdi_lib_is_service_available(
    gsdi_services_type service
)
{
    gsdi_svr_rsp_type               gsdi_svr_rsp;
    gsdi_returns_T                  gsdi_status;
    gsdi_card_mode_capability_T     card_mode;

    /* initialize gsdi_svr_rsp.svr_bitmap to 0 */
    gsdi_svr_rsp.svr_bitmap = 0x00;

    /* Get the card mode */
    gsdi_status = gsdi_get_card_mode(&card_mode);
    if (gsdi_status != GSDI_SUCCESS) { /* problem obtaining card mode */
        gsdi_svr_rsp.gsdi_status = gsdi_status;
        return gsdi_svr_rsp;
    }


    /* Card mode has been obtained successfully, go through the services in each
       technology as appropriate */
    switch(card_mode) {
  #ifdef FEATURE_MMGSDI_UMTS
#error code not present
  #endif /*#ifdef FEATURE_MMGSDI_UMTS*/

  #ifdef FEATURE_MMGSDI_GSM
#error code not present
  #endif /*#ifdef FEATURE_MMGSDI_GSM*/

  #if defined(FEATURE_MMGSDI_GSM) && defined(FEATURE_MMGSDI_UMTS)
#error code not present
  #endif /*#if defined(FEATURE_MMGSDI_GSM) && defined(FEATURE_MMGSDI_UMTS)*/

  #ifdef FEATURE_MMGSDI_CDMA
    case GSDI_CARD_RUIM:
        gsdi_status = gsdi_util_ruim_service_check(GSDI_SLOT_1, &gsdi_svr_rsp, service);
        gsdi_svr_rsp.gsdi_status = gsdi_status; /* GSDI_NOT_INIT ||
                                                GSDI_INCORRECT_PARAM ||
                                                GSDI_SUCCESS */
        break;
  #endif /*#ifdef FEATURE_MMGSDI_CDMA*/

  #if (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS))&& defined(FEATURE_MMGSDI_CDMA)
    case GSDI_CARD_GSM_RUIM:
        gsdi_status = gsdi_util_ruim_service_check(GSDI_SLOT_1, &gsdi_svr_rsp, service);
        gsdi_svr_rsp.gsdi_status = gsdi_status; /* GSDI_NOT_INIT ||
                                                GSDI_INCORRECT_PARAM ||
                                                GSDI_SUCCESS */
        gsdi_status = gsdi_util_gsm_service_check(GSDI_SLOT_1, &gsdi_svr_rsp, service);
        if(gsdi_status != GSDI_SUCCESS && gsdi_svr_rsp.gsdi_status != GSDI_SUCCESS) {
            /* both service table fail */
            gsdi_svr_rsp.gsdi_status = GSDI_INCORRECT_PARAMS;
        }
        else {  /* one of them is successful */
            gsdi_svr_rsp.gsdi_status = GSDI_SUCCESS;
        }
        break;
  #endif /* (FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS) && FEATURE_MMGSDI_CDMA */
    case GSDI_CARD_EMV:
    default:
        gsdi_svr_rsp.gsdi_status = GSDI_INCORRECT_PARAMS;
        break;
    }

    return gsdi_svr_rsp;

} /* gsdi_lib_is_service_available */

/*===========================================================================
FUNCTION GSDI_SIM_REFRESH

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  GSDI_SIM_INIT_AND_FULL_FILE_CHANGE  (SIM/USIM)
  GSDI_FILE_CHANGE_NOTIFICATION       (SIM/USIM)
  GSDI_SIM_INIT_AND_FILE_CHANGE       (SIM/USIM)
  GSDI_SIM_INIT                       (SIM/USIM)
  GSDI_SIM_RESET                      (SIM/USIM)
  GSDI_USIM_APP_RESET                 (USIM)
  GSDI_3G_SESSION_RESET               (USIM)

  Limitation is that this function can only be called Asynchrounously.  There
  is no support for a synchronous REFRESH Request.

DEPENDENCIES
  UI or CM have to "OK" the refresh request.  This is done by GSDI quering
  CM/UI Interface for active calls.  If a call is active, CM/UI will
  indicate such that the GSDI will not allow the REFRESH to go through.

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
===========================================================================*/
gsdi_returns_T gsdi_sim_refresh(
    gsdi_refresh_modes_enum_type refresh_mode,
    uint8                        num_files,
    uint8                        data_len,
    uint8 *                      data_p,
    uint32                       client_ref,
    gsdi_async_cb                async_cb)
{
  gsdi_app_id_type app_id;

  /* Default application refresh */
  memset(&app_id, 0x00, sizeof(gsdi_app_id_type));

  return gsdi_naa_refresh(GSDI_SLOT_1,
                          refresh_mode,
                          app_id,
                          num_files,
                          data_len,
                          data_p,
                          client_ref,
                          async_cb);

} /* gsdi_sim_refresh */


/*===========================================================================
FUNCTION GSDI_NAA_REFRESH

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a NAA
  REFRESH be performed on the Mobile.  There are 5 GSM SIM Refresh Modes
  and 7 USIM Refresh Modes
  (as defined by the enum gsdi_refresh_modes_enum_type).

  GSDI_SIM_INIT_AND_FULL_FILE_CHANGE  (SIM/USIM)
  GSDI_FILE_CHANGE_NOTIFICATION       (SIM/USIM)
  GSDI_SIM_INIT_AND_FILE_CHANGE       (SIM/USIM)
  GSDI_SIM_INIT                       (SIM/USIM)
  GSDI_SIM_RESET                      (SIM/USIM)
  GSDI_USIM_APP_RESET                 (USIM)
  GSDI_3G_SESSION_RESET               (USIM)

DEPENDENCIES
  If call stack is required to be taken down, UI, CM or WMS have to
  "OK" the refresh request.  This is done by GSDI quering
  CM/UI Interface for active calls.  If a call is active, CM/UI/WMS will
  indicate such that the GSDI will not allow the REFRESH to go through.

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
===========================================================================*/
gsdi_returns_T gsdi_naa_refresh(
    gsdi_slot_id_type            slot,
    gsdi_refresh_modes_enum_type refresh_mode,
    gsdi_app_id_type             app_id,
    uint8                        num_files,
    uint8                        data_len,
    uint8 *                      data_ptr,
    uint32                       client_ref,
    gsdi_async_cb                async_cb)
{
  MSG_ERROR("Deprecated, Need to use mmgsdi_naa_refresh",0,0,0);
  return GSDI_NOT_SUPPORTED;
} /* gsdi_naa_refresh */


/*===========================================================================
FUNCTION GSDI2_SIM_REFRESH

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  GSDI_SIM_INIT_AND_FULL_FILE_CHANGE  (SIM/USIM)
  GSDI_FILE_CHANGE_NOTIFICATION       (SIM/USIM)
  GSDI_SIM_INIT_AND_FILE_CHANGE       (SIM/USIM)
  GSDI_SIM_INIT                       (SIM/USIM)
  GSDI_SIM_RESET                      (SIM/USIM)
  GSDI_USIM_APP_RESET                 (USIM)
  GSDI_3G_SESSION_RESET               (USIM)

  Limitation is that this function can only be called Asynchrounously.  There
  is no support for a synchronous REFRESH Request.

DEPENDENCIES
  UI or CM have to "OK" the refresh request.  This is done by GSDI quering
  CM/UI Interface for active calls.  If a call is active, CM/UI will
  indicate such that the GSDI will not allow the REFRESH to go through.

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
===========================================================================*/
gsdi_returns_T gsdi2_sim_refresh(
    gsdi_slot_id_type            slot,
    gsdi_refresh_modes_enum_type refresh_mode,
    uint8                        num_files,
    uint8                        data_len,
    uint8 *                      data_p,
    uint32                       client_ref,
    gsdi_async_cb                async_cb)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_sim_refresh_req_T  msg;
    gsdi_returns_T          gsdi_status = GSDI_SUCCESS;

    switch (slot)
    {
      case GSDI_SLOT_1:
      case GSDI_SLOT_2:
        break;
      default:
        MMGSDI_DEBUG_MSG_ERROR("Unknown Slot for Refresh, %x", slot, 0, 0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        gsdi_status = gsdi_lib_init();

        if ( gsdi_status == GSDI_SUCCESS ) {
            gsdi_lib_initialised = TRUE;
        }
    } /* gsdi_lib_initialized */

    /* Message Header */
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_SIM_REFRESH_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_refresh_req_T ), &msg.message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = slot;

    /* Fill in Refresh Parameters */
    msg.refresh_mode = refresh_mode;
    msg.num_files    = num_files;
    msg.data_len     = data_len;
    if ( data_len > 0 && data_p != NULL && num_files > 0)
    {
        memcpy(msg.data,
               data_p,
               data_len);
    }

    msg.client_ref = client_ref;

    if (async_cb != NULL)
    {
       msg.message_header.gsdi_async_callback = async_cb;
       gsdi_status = gsdi_cmd_ext(&msg);
       if(gsdi_status != GSDI_SUCCESS)
       {
         gsdi_status = GSDI_ERROR;
       }
    }
    else
    {   /* async_cb == NULL */
        /* NULL Out GSDI ASYNC Call Back Ptr */
        MMGSDI_DEBUG_MSG_ERROR("Synchronous Refresh Not Allowed ",0,0,0);
        return GSDI_ERROR;
    }
    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

} /* gsdi2_sim_refresh */ /*lint !e715 */


/*===========================================================================
FUNCTION GSDI_REGISTER_FOR_REFRESH_VOTING

DESCRIPTION
  This Library Function is used by the calling task to register a callback
  for MMGSDI to call when a REFRESH Proactive Command is made availabled by
  GSTK.

  The Voting Mechanism is used to notify the client of the following:
  1) MMGSDI would like to take the callstack down.  At which time, the client
     can indicated that it is ok to take the call stack down, or it is not
     ok to do so.
  2) It is also used to notify the calling task that there is a list of files
     for the client to read.  In which case, the client can indicate whether
     or not it wants to read the files.

DEPENDENCIES:
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Successfully added to the refresh client
                                  table.
                   GSDI_ERROR   - Failure to add client to refresh client
                                  table.
                                  Calling Task did not provide a Callback
                                  Function.
SIDE EFFECTS
  Client task will receive notifications in which it is responsible to
  analyze to decide (vote) whether to allow the REFRESH To Occur.
===========================================================================*/
gsdi_returns_T gsdi_register_for_refresh_voting(
  gsdi_refresh_vote_cb_type  * vote_cb_p,
  uint32                       client_ref,
  gsdi_async_cb                async_cb)
{
    gsdi_refresh_registration_req_T * msg;
    gsdi_returns_T                    gsdi_status;

    /* Parameter Check and Verification Procedures */
    if ( vote_cb_p == NULL )
    {
        /* Fail, no Callback */
        MMGSDI_DEBUG_MSG_ERROR("REGSTR TO VOTE CB NULL",0,0,0);
        return GSDI_ERROR;
    }

    msg = mmgsdi_malloc(sizeof(gsdi_refresh_registration_req_T));

    if ( msg == NULL )
    {
        /* Failed to get memory */
        MMGSDI_DEBUG_MSG_ERROR("NO MEMORY VOTE REGISTRATION",0,0,0);
        return GSDI_ERROR;
    }

    /* Clear out the msg */
    memset(msg,0x00,sizeof(gsdi_refresh_registration_req_T));

    /* Populate the Message Header Information */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_REG_FOR_REFRESH_VOTE_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr     = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
     /* default to slot 1 */
    msg->message_header.slot        = GSDI_SLOT_1;

    /* Populate callback to call upon completion
    ** of servicing this message.  If NULL, use
    ** GSDI Callback to discard Message
    */
    if ( async_cb != NULL )
    {
        msg->message_header.gsdi_async_callback = async_cb;
    }
    else
    {
        MMGSDI_DEBUG_MSG_ERROR("GSDI REG FOR VOTE NULL PTR",0,0,0);
    }
    /* Store User Information sent by
    ** client.  Expected to be the same
    ** in the response sent back.
    */
    msg->client_ref = client_ref;

    /* Store the Callback Information */
    msg->reg_vote_cb = vote_cb_p;

    /* Populate Message Size Information */
    GSDI_PUT_IMH_LEN( sizeof( gsdi_refresh_registration_req_T ), &msg->message_header );

    /* Send Command to GSDI */
    gsdi_status = gsdi_cmd_ext((void *)msg);

    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return gsdi_status;

} /* gsdi_register_for_refresh_voting */

/*===========================================================================
FUNCTION GSDI_REGISTER_FOR_REFRESH_VOTING

DESCRIPTION
  This Library Function is used by the calling task to register a callback
  for MMGSDI to call when a File Change Notification is made available.  The
  file change notification will consist of a list of files directed at the
  client that registers for the files to be read.

  This function will use the memheap to create the GSDI Message.

DEPENDENCIES:
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Successfully added to the file Change
                                  Notification Table.
                   GSDI_ERROR   - Failure to add client to the File Change
                                  Notification Table.
                                  file_notify_cp_p was NULL
                                  num_files is 0
                                  file_list_p was NULL
SIDE EFFECTS
  Client task will receive notifications in which it is responsible for
  properly reading the files it has registered for.
===========================================================================*/
gsdi_returns_T gsdi_register_for_file_change_notifications(
    gsdi_task_notify_file_list_cb_type * file_notify_cb_p,
    uint8                                num_files,
    uim_items_enum_type                * file_list_p,
    uint32                               client_ref,
    gsdi_async_cb                        async_cb)
{

    gsdi_refresh_registration_req_T * msg;
    gsdi_returns_T                    gsdi_status;

    /* Basic Parameter Checking */
    if ( ( file_notify_cb_p == NULL ) ||
         ( file_list_p      == NULL ) )
    {
        /* Function not called properly */
        MMGSDI_DEBUG_MSG_ERROR("NULL POINTERS %x %x",file_notify_cb_p, file_list_p,0);
        return GSDI_ERROR;
    }

    if ( ( num_files == 0 ) ||
         ( num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS ) )
    {
        /* File List is empty or has more than supported*/
        MMGSDI_DEBUG_MSG_ERROR("BAD NUM_FILES PARAM %x",num_files,0,0);
        return GSDI_ERROR;
    }

    msg = mmgsdi_malloc(sizeof(gsdi_refresh_registration_req_T));

    if ( msg == NULL )
    {
        /* Failed to get memory */
        MMGSDI_DEBUG_MSG_ERROR("NO MEMORY FILE CHNG REGISTRATION",0,0,0);
        return GSDI_ERROR;
    }

    /* Clear out the msg */
    memset(msg,0x00,sizeof(gsdi_refresh_registration_req_T));

    /* Populate the Message Header Information */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_REG_FOR_FILE_NOTIFY_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr     = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.slot        = GSDI_SLOT_1;

    /* Populate callback to call upon completion
    ** of servicing this message.  If NULL, use
    ** GSDI Callback to discard Message
    */
    if ( async_cb != NULL )
    {
        msg->message_header.gsdi_async_callback = async_cb;
    }
    else
    {
        MMGSDI_DEBUG_MSG_ERROR("GSDI REG FOR FCN NULL PTR",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(msg);
        return GSDI_ERROR;
    }

    /* Store User Information sent by
    ** client.  Expected to be the same
    ** in the response sent back.
    */
    msg->client_ref = client_ref;

    /* Need to get memory for file list */
    msg->num_files   = num_files;
    memcpy(msg->file_list_p,
           file_list_p,
           (sizeof(uim_items_enum_type) * num_files) );

    /* Store Callback to call once it is
    ** for File Change Notifications
    */
    msg->reg_file_notify_cb = file_notify_cb_p;

    /* Populate Message Size Information */
    GSDI_PUT_IMH_LEN( sizeof( gsdi_refresh_registration_req_T ), &msg->message_header );

    /* Send Command to GSDI */
    gsdi_status = gsdi_cmd_ext((void *)msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    if ( msg != NULL ) /*lint !e774 */
    {
        MMGSDIUTIL_TMC_MEM_FREE(msg);
    }

    return gsdi_status;
} /* gsdi_register_for_file_change_notifications */

/*===========================================================================
FUNCTION GSDI_REFRESH_COMPLETE

DESCRIPTION
  This Library Function is called by the client task once it has completed
  reading all files listed in the file change notification it received.

INPUT
  num_files:   Number of files in proceeding pointer to the file list
  file_list_p: Pointer to file list
  pass_fail:   Indication that all files were successfully read or not


DEPENDENCIES:
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Successfully verified that there was a
                                  pending notification.  Cleared pending
                                  notification.
                   GSDI_ERROR   - Failure to verify there was a pending
                                  notification.  Failure to clear pending
                                  notification.  NULL Pointers are passed
                                  in as parameters.
SIDE EFFECTS
===========================================================================*/
gsdi_returns_T gsdi_refresh_complete (
    uint8                 num_files,
    uim_items_enum_type * file_list_p,
    boolean               pass_fail,
    uint32                client_ref,
    gsdi_async_cb         async_cb )
{
  gsdi_app_id_type  app_id;

  /* Default provisiong app */
  memset(&app_id, 0x00, sizeof(gsdi_app_id_type));

  return gsdi_naa_refresh_complete(
                            GSDI_SLOT_1,
                            GSDI_FILE_CHANGE_NOTIFICATION,
                            app_id,
                            num_files,
                            file_list_p,
                            pass_fail,
                            client_ref,
                            async_cb);
}/* gsdi_refresh_complete */


/*===========================================================================
FUNCTION GSDI_NAA_REFRESH_COMPLETE

DESCRIPTION
  This Library Function is called by the client task once it has completed
  the NAA refresh procedure.

INPUT
  refresh_mode:   Type of refresh mode
  app_id:         applicaton where the refresh is performed on
  num_files:      Number of files in proceeding pointer to the file list
  file_list_ptr:  Pointer to file list
  pass_fail:      Indication that the refresh is completed or not

DEPENDENCIES:
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Successfully verified that there was a
                                  pending notification.  Cleared pending
                                  notification.
                   GSDI_ERROR   - Failure to verify there was a pending
                                  notification.  Failure to clear pending
                                  notification.  NULL Pointers are passed
                                  in as parameters.
SIDE EFFECTS
===========================================================================*/
gsdi_returns_T gsdi_naa_refresh_complete (
    gsdi_slot_id_type            slot_id,
    gsdi_refresh_modes_enum_type refresh_mode,
    gsdi_app_id_type             app_id,
    uint8                        num_files,
    uim_items_enum_type        * file_list_ptr,
    boolean                      pass_fail,
    uint32                       client_ref,
    gsdi_async_cb                async_cb )
{
    gsdi_returns_T               gsdi_status = GSDI_SUCCESS;
    gsdi_refresh_complete_req_T *msg_ptr     = NULL;

    if ((num_files > 0) && (file_list_ptr == NULL))
      return GSDI_ERROR;

    if (async_cb == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("GSDI REFRESH COMP: NULL CB",0,0,0);
      return GSDI_ERROR;
    }

    if (num_files > GSDI_MAX_FILE_CHANGE_NOTIFICATIONS)
    {
      MMGSDI_DEBUG_MSG_ERROR("GSDI Num_file notifier too big 0x%x",num_files,0,0);
      return GSDI_ERROR;
    }

    if (app_id.aid_len > GSDI_MAX_APP_ID_LEN)
    {
      MMGSDI_DEBUG_MSG_ERROR("AID LEN Invalid 0x%x",app_id.aid_len,0,0);
      return GSDI_ERROR;
    }

    msg_ptr = mmgsdi_malloc(sizeof(gsdi_refresh_complete_req_T));

    if ( msg_ptr == NULL )
    {
      /* Failed to get memory */
      MMGSDI_DEBUG_MSG_ERROR("NO MEMORY FILE CHNG REGISTRATION",0,0,0);
      return GSDI_ERROR;
    }

    /* Clear out the msg */
    memset(msg_ptr,0x00,sizeof(gsdi_refresh_complete_req_T));

    /* Populate the Message Header Information */
    msg_ptr->message_header.message_set = MS_GSDI;
    msg_ptr->message_header.message_id  = GSDI_SIM_REFRESH_COMPLETE_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg_ptr->message_header.tcb_ptr     = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg_ptr->message_header.slot        = slot_id;
    msg_ptr->refresh_mode               = refresh_mode;

    /* app_id is default to len = 0 => Default channel App */

    /* Populate callback to call upon completion of servicing this message. */
    msg_ptr->message_header.gsdi_async_callback = async_cb;

    /* Store User Information sent by client.  Expected to be the same
    ** in the response sent back. */
    msg_ptr->client_ref = client_ref;

    /* Need to get copy file list */
    msg_ptr->num_files   = num_files;
    if (num_files > 0)
    {
      memcpy(msg_ptr->file_list_p,
            file_list_ptr,
            (sizeof(uim_items_enum_type) * num_files) );
    }

    msg_ptr->app_id.aid_len = app_id.aid_len;
    if (app_id.aid_len > 0)
    {
      memcpy(msg_ptr->app_id.aid_data,
             app_id.aid_data,
             app_id.aid_len);
    }

    /* Did the client's attempt to refresh succeed */
    msg_ptr->pass_fail = pass_fail;

    /* Populate Message Size Information */
    GSDI_PUT_IMH_LEN( sizeof( gsdi_refresh_registration_req_T ), &msg_ptr->message_header );

    /* Send Command to GSDI */
    gsdi_status = gsdi_cmd_ext((void *)msg_ptr);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    if ( msg_ptr != NULL ) /*lint !e774 */
    {
      MMGSDIUTIL_TMC_MEM_FREE(msg_ptr);
    }
    return gsdi_status;

}/* gsdi_naa_refresh_complete */


/*===========================================================================
FUNCTION GSDI2_GET_FILE_ATTRIBUTES

DESCRIPTION
    This function returns header information of the specified EF.
    The information returned is parsed data as defined in gsdi_file_attributes_T.
    For the RAW Header information, it is suggested you use gsdi2_select().

    Paremter Description:
    1.  slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  sim_filename [ INPUT ] - MANDATORY
            CARD IS USIM:  NULL Indicates USIM App should be used
                           NOT NULL Assumes valid Application ID is
                           present.  Will use Application ID to select
                           Application and then verify PIN.
            CARD IS GSM:   IGNORED
            CARD IS RUIM:  IGNORED
    4.  client_ref [ INPUT / OUTPUT ] - OPTIONAL
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    5.  async_cb [ INPUT ] - MANDATORY
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
    None

RETURN VALUE
    gsdi_returns_T
        GSDI_SUCCESS:           Successful putting get file attribute to the cmd queue
        GSDI_ERROR:             Unsuccessfully putting get file attribute to the cmd queue

SIDE EFFECTS
    None

LIMITATIONS
    Only supported in Asynchronous Mode.
===========================================================================*/
/*lint -save -esym(818,aid) suppress could be const */
gsdi_returns_T gsdi2_get_file_attributes(
    gsdi_slot_id_type        slot,
    uim_items_enum_type      sim_filename,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_get_file_attributes_req_T * msg = 0x00;
    gsdi_returns_T                   gsdi_status = GSDI_ERROR;
    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ((slot != GSDI_SLOT_NONE) &&
       (slot != GSDI_SLOT_1)    &&
       (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_get_file_attributes_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* NULL out the Heap Space just acquired */
    memset(msg,0x00,sizeof(gsdi_get_file_attributes_req_T));

    /* Start setting the Message Contents for
        further processing */
    /* MESSAGE HEADER INFORMATION */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_GET_FILE_ATTRIBUTES_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_get_file_attributes_req_T ), &msg->message_header );

#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* MESSAGE CONTENTS */
    msg->sim_filename      = (word) sim_filename;
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;


    /* PLACE ON GSDI QUEUE */
    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }
    /* Data has been copied to a GSDI Command Queue */
    /* OK to FREE Memory                            */
    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

} /* gsdi2_get_file_attributes */ /*lint !e715 */
/*lint -save +esym(818,aid) suppress could be const */

/*===========================================================================
FUNCTION GSDI2_GET_PIN_STATUS

DESCRIPTION
        This function will retrieve the PIN Status Information for the PIN ID
        Requested.

        Paremter Description:
        1.  slot [ INPUT ] - MANDATORY
                GSDI_SLOT_NONE: select and use slot 1
                GSDI_SLOT_1:    select and use slot 1
                GSDI_SLOT_2:    select and use slot 2
        2.  pin_id [INPUT] - MANDATORY
                GSDI_PIN1:                      Verify GSM_CHV1, RUIM_CHV1, USIM_PIN1
                GSDI_PIN2:                      Verify GSM_CHV2, RUIM_CHV2, USIM_PIN2
                GSDI_USIM_UNIV_PIN:     Verify Universal USIM PIN: Key Reference ID 11
                GSDI_USIM_APP1_PIN1:  Verify USIM Application 1, PIN1
                GSDI_USIM_APP1_PIN2:  Verify USIM Application 1, PIN2
                GSDI_USIM_APP2_PIN1:  Verify USIM Application 2, PIN1
                GSDI_USIM_APP2_PIN2:  Verify USIM Application 2, PIN2
                GSDI_USIM_APP3_PIN1:  Verify USIM Application 3, PIN1
                GSDI_USIM_APP3_PIN2:  Verify USIM Application 3, PIN2
                GSDI_USIM_APP4_PIN1:  Verify USIM Application 4, PIN1
                GSDI_USIM_APP4_PIN2:  Verify USIM Application 4, PIN2
        3.  aid [ INPUT ] - OPTIONAL
                CARD IS USIM:  NULL Indicates USIM App should be used
                                           NOT NULL Assumes valid Application ID is
                                           present.  Will use Application ID to select
                                           Application and then verify PIN.
                CARD IS GSM:   IGNORED
                CARD IS RUIM:  IGNORED
        4.  client_ref [ INPUT / OUTPUT ] - OPTIONAL
                User Data that can be passed in.  Exact data is returned upon
                completion of this procedure.
        5.  async_cb [ INPUT ] - MANDATORY
                Will contain the function to the pointer to be called upon
                completion of this procedure.

DEPENDENCIES
        None

RETURN VALUE
        gsdi_returns_T
                GSDI_SUCCESS:                   PIN Status was successfully retrieved
                GSDI_ERROR:                             PIN Status retrieval failed

SIDE EFFECTS
        NONE

LIMITATIONS
        pin_status pointer is not used when async_cb is set (i.e. an Asynchronous
        call is made).  If an asynchrounous call is made, the gsdi_cnf_T union
        must be checked.
===========================================================================*/
/*lint -save -esym(818,aid) suppress could be const */
gsdi_returns_T gsdi2_get_pin_status(
    gsdi_slot_id_type        slot,
    gsdi_pin_ids_T           pin_id,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_get_pin_status_req_T * msg = NULL;
    gsdi_returns_T              gsdi_status = GSDI_ERROR;
    /* First Check all mandatory parameters */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_get_pin_status_req_T));

    if ( msg == NULL )
    {
        ERR_FATAL("NO HEAP SPACE",0,0,0);
        return GSDI_ERROR;
    }

    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_GET_PIN_STATUS_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_get_pin_status_req_T ), &msg->message_header );

#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;

    /* Default to slot 1 */
    msg->message_header.slot = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Message Contents */
    msg->pin_id            = pin_id;
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE(msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_get_pin_status */ /*lint !e715 */
/*lint -save +esym(818,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_VERIFY_PIN

DESCRIPTION
        This function performs a pin verification procedure against the SIM, USIM,
        or RUIM.  The PIN Verified can be for:

        Paremter Description:
        1.  slot [ INPUT ] - MANDATORY
                GSDI_SLOT_NONE: select and use slot 1
                GSDI_SLOT_1:    select and use slot 1
                GSDI_SLOT_2:    select and use slot 2
        2.  pin_id [INPUT] - MANDATORY
                GSDI_PIN1:                      Verify GSM_CHV1, RUIM_CHV1, USIM_PIN1
                GSDI_PIN2:                      Verify GSM_CHV2, RUIM_CHV2, USIM_PIN2
                GSDI_USIM_UNIV_PIN:     Verify Universal USIM PIN: Key Reference ID 11
                GSDI_USIM_APP1_PIN1:  Verify USIM Application 1, PIN1
                GSDI_USIM_APP1_PIN2:  Verify USIM Application 1, PIN2
                GSDI_USIM_APP2_PIN1:  Verify USIM Application 2, PIN1
                GSDI_USIM_APP2_PIN2:  Verify USIM Application 2, PIN2
                GSDI_USIM_APP3_PIN1:  Verify USIM Application 3, PIN1
                GSDI_USIM_APP3_PIN2:  Verify USIM Application 3, PIN2
                GSDI_USIM_APP4_PIN1:  Verify USIM Application 4, PIN1
                GSDI_USIM_APP4_PIN2:  Verify USIM Application 4, PIN2
        3.  pin [ INPUT ] - MANDATORY
                Pointer to ASCII Character Array which contains the PIN
                Code.  This is sent to the SIM/USIM/RUIM and used for PIN
                Verification.
        4.  aid [ INPUT ] - OPTIONAL
                        CARD IS USIM:  NULL Indicates USIM App should be used
                                                   NOT NULL Assumes valid Application ID is
                                                   present.  Will use Application ID to select
                                                   Application and then verify PIN.
                        CARD IS GSM:   IGNORED
                        CARD IS RUIM:  IGNORED
        5.  client_ref [ INPUT / OUTPUT ] - OPTIONAL
                User Data that can be passed in.  Exact data is returned upon
                completion of this procedure.
        6.  async_cb [ INPUT ] - MANDATORY
                Will contain the function to the pointer to be called upon
                completion of this procedure.

DEPENDENCIES
        None

RETURN VALUE
        gsdi_returns_T
                GSDI_SUCCESS:                   Successful PIN Verification
                GSDI_ERROR:                             PIN Verification Failed
                GSDI_PIN_BLOCKED:               Number of verifications Exceeded number of
                                                                attempts left
                GSDI_PIN_PERM_BLOCKED:  Number of PUKs exceeded number of attempts l
                                                                eft

SIDE EFFECTS
        A gsdi_events_T is generated and sent to all clients registered.  Events
        possible are:
                GSDI_PIN1_VERIFIED
                GSDI_PIN1_UNBLOCKED
                GSDI_PIN1_ENABLED
                GSDI_PIN1_DISABLED
                GSDI_PIN1_CHANGED
                GSDI_PIN1_PERM_BLOCKED
                GSDI_PIN2_VERIFIED
                GSDI_PIN2_BLOCKED
                GSDI_PIN2_UNBLOCKED
                GSDI_PIN2_ENABLED
                GSDI_PIN2_DISABLED
                GSDI_PIN2_CHANGED
                GSDI_PIN2_PERM_BLOCKED
                *** TO BE ADDED ***
                EVENTS FOR ALL OTHER USIM APPLICATIONS
                WHICH IS ON HOLD FOR NOW UNTIL DUAL SLOT
                IS COMPLETED
                *******************

LIMITATIONS
        Only supported in Asynchronous Mode.  The async_cb must not be NULL and
        must contain a valid pointer.  Failure to do so could result in
        undesireable results.
===========================================================================*/
/*lint -save -esym(818,pin,aid) suppress could be const */
gsdi_returns_T gsdi2_verify_pin(
    gsdi_slot_id_type       slot,
    gsdi_pin_ids_T          pin_id,
    char                  * pin,
    gsdi_app_id_type      * aid,
    uint32                  client_ref,
    gsdi_async_cb           async_cb
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_verify_pin_req_T  * msg = 0x00;
    gsdi_returns_T           gsdi_status = GSDI_ERROR;
    /* First Check all mandatory parameters */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)pin))
    {
       return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_verify_pin_req_T));
    if ( msg == NULL )
    {
        ERR_FATAL("NO HEAP SPACE ",0,0,0);
        return GSDI_ERROR;
    }

    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_VERIFY_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_verify_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.gsdi_async_callback = async_cb;
    msg->message_header.slot = slot;

    /* Message Contents */
    msg->pin_id            = pin_id;
    memcpy( msg->pin, pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;
    msg->message_header.gsdi_async_callback = async_cb;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE(msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

} /* gsdi2_verify_pin */ /*lint !e715 */
/*lint -save +esym(818,pin,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_SIM_READ

DESCRIPTION
    This function will generate a Message for GSDI to read the SIM, RUIM or USIM
    for the file requested.

    Parameter Information
    1.  slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  sim_filename [ INPUT ] - MANDATORY
        uim_items_enum_type such as:
            UIM_GSM_IMSI, UIM_TELECOM_ADM, UIM_USIM_IMSI,
            or UIM_CDMA_IMSI_M
    3.  index_mode
        GSDI_NO_INDEX
        GSDI_INDEX_ABSOLUTE
        GSDI_INDEX_CURRENT
        GSDI_INDEX_NEXT - Not reliable
        GSDI_INDEX_PREVIOUS - Not reliable
    4.  rec_num
        Specifies the record number to be read.  Only applicable if the file is
        a Linear Fixed File.
    5.  required_data_len
        The number of bytes to be read.  When set to 0, this assume the caller is
        requesting the
    6.  data_offset
        Only applicable to Binary Files
    7.  aid
            CARD IS USIM:  NULL Indicates USIM App should be used
                           NOT NULL Assumes valid Application ID is
                           present.  Will use Application ID to select
                           Application and then verify PIN.
            CARD IS GSM:   IGNORED
            CARD IS RUIM:  IGNORED
    8.  client_ref
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    9.  gsdi_async_cb
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
    gsdi_returns_T
        GSDI_SUCCESS:           Command successfully put on GSDI Command Queue
        GSDI_ERROR:             Command Failed to be put on GSDI Command Queue
                                Request is using Bad Parameters:
                                * Bad SLOT ID.  != GSDI_SLOT_1, GSDI_SLOT_2, or
                                  GSDI_SLOT_NONE.
                                * Async_cb == NULL
                                * No Heap Space Left to build command with.

SIDE EFFECTS
    NONE

LIMITATIONS
    1.  Only supported in Asynchronous Mode.
    2.  Search and Read Capabilities will not be supported.
===========================================================================*/
/*lint -save -esym(818,aid) suppress could be const */
gsdi_returns_T gsdi2_sim_read(
    gsdi_slot_id_type        slot,
    uim_items_enum_type      sim_filename,
    gsdi_index_modes_T       index_mode,
    int                      rec_num,
    int                      required_data_len,
    int                      data_offset,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_sim_read_req_T * msg = 0x00;
    gsdi_returns_T        gsdi_status = GSDI_ERROR;

    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_sim_read_req_T));

    /* Start setting the Message Contents for
        further processing */
    /* MESSAGE HEADER INFORMATION */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_SIM_READ_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_read_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Populate Message Contents */
    msg->sim_filename       = sim_filename;
    msg->client_ref         = client_ref;
    msg->index_mode         = index_mode;
    msg->required_data_len  = (byte)required_data_len;
    msg->data_offset        = data_offset;
    msg->rec_num            = (byte)rec_num;
    msg->queue_id           = GS_QUEUE_GSDI_LIB;

    /* Search and Read Capabilities will not be
    ** required in API. */
    msg->search_string_len  = 0;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    /* Data has been copied to a GSDI Command Queue */
    /* OK to FREE Memory                            */
    MMGSDIUTIL_TMC_MEM_FREE(msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_sim_read */  /*lint !e715 */
/*lint -save +esym(818,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_SIM_WRITE

DESCRIPTION
  This function will generate a Message for GSDI to write information to the
  SIM, RUIM, or USIM for the file requested.

    Parameter Information
    1.  slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  sim_filename [ INPUT ] - MANDATORY
        uim_items_enum_type such as:
            UIM_GSM_IMSI, UIM_TELECOM_ADM, UIM_USIM_IMSI,
            or UIM_CDMA_IMSI_M
    3.  index_mode [ INPUT ] - MANDATORY
        GSDI_NO_INDEX
        GSDI_INDEX_ABSOLUTE
        GSDI_INDEX_CURRENT
        GSDI_INDEX_NEXT - Not reliable
        GSDI_INDEX_PREVIOUS - Not reliable
    4.  rec_num [ INPUT ] - MANDATORY
        Specifies the record number to be read.  Only applicable if the file is
        a Linear Fixed File.
    5.  data_buffer [ INPUT ] - MANDATORY
        Pointer to valid buffer that contains data to be written to the SIM,RUIM, or
        USIM.  The Data Written is not validated at the GSDI Level.  It is assumed
        the data written is compliant will all applicable specifications.
    6.  data_offset [ INPUT ] - MANDATORY
        Indicates which byte in the file the write should begin at.  Only
        applicable to Binary Files.
    7.  aid
            CARD IS USIM:  NULL Indicates USIM App should be used.
                           NOT NULL Assumes valid Application ID is
                           present.  Will use Application ID to select
                           Application and then verify PIN.
            CARD IS GSM:   IGNORED
            CARD IS RUIM:  IGNORED
    8.  client_ref
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    9.  gsdi_async_cb
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
    gsdi_returns_T
        GSDI_SUCCESS:           Command successfully put on GSDI Command Queue
        GSDI_ERROR:             Command Failed to be put on GSDI Command Queue
                                Request is using Bad Parameters:
                                * Bad SLOT ID.  != GSDI_SLOT_1, GSDI_SLOT_2, or
                                  GSDI_SLOT_NONE.
                                * Async_cb == NULL
                                * No Heap Space Left to build command with.
                                * data_buffer Pointer is NULL

SIDE EFFECTS
  None

LIMITATIONS
    Only supported in Asynchronous Mode.
===========================================================================*/
/*lint -save -esym(818,data_buffer,aid) suppress could be const */
gsdi_returns_T gsdi2_sim_write(
    gsdi_slot_id_type       slot,
    uim_items_enum_type     sim_filename,
    gsdi_index_modes_T      index_mode,
    int                     rec_num,
    byte                  * data_buffer,
    int                     data_len,
    int                     data_offset,
    gsdi_app_id_type      * aid,
    uint32                  client_ref,
    gsdi_async_cb           async_cb )
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_sim_write_req_T * msg = 0x00;
    gsdi_returns_T         gsdi_status = GSDI_ERROR;

    if ( ( data_len > GSDI_MAX_DATA_BLOCK_LEN )||
         ( data_len < 0 ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("LEN %x EXCEEDS MAX %x",
                  data_len,GSDI_MAX_DATA_BLOCK_LEN,0);
        return GSDI_ERROR;
    }

    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL  || data_buffer == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB or DATA BUF %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_sim_write_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_sim_write_req_T));

    /* Start setting the Message Contents for
        further processing */
    /* MESSAGE HEADER INFORMATION */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_SIM_WRITE_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_sim_write_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Populate Message Contents */
    msg->sim_filename       = sim_filename;
    msg->client_ref         = client_ref;
    msg->index_mode         = (byte)index_mode;
    if (data_len > GSDI_MAX_DATA_BLOCK_LEN)
    {
        msg->data_len = (uint8)GSDI_MAX_DATA_BLOCK_LEN;
    }
    else
    {
        msg->data_len       = data_len;
    }
    msg->rec_num            = (byte)rec_num;
    msg->data_offset        = data_offset;
    msg->queue_id           = GS_QUEUE_GSDI_LIB;

    memcpy(msg->data_buffer, data_buffer, msg->data_len);

    /* Search and Read Capabilities will not be
    ** required in API. */
    msg->search_string_len  = 0;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }
    /* Data has been copied to a GSDI Command Queue */
    /* OK to FREE Memory                            */
    MMGSDIUTIL_TMC_MEM_FREE(msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_sim_write */  /*lint !e715 */
/*lint -save +esym(818,data_buffer,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_ENABLE_PIN

DESCRIPTION
  This function attempts to enable either PIN1 or PIN2 for the required Slot.
  This function will only support asynchronous calls.  Thus, the call will
  need to provide a callback function.

    Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  gsdi_pin_id_T   pin [ INPUT ] - MANDATORY
            GSDI_PIN1,
            GSDI_PIN2,
            GSDI_RUIM_CHV1,
            GSDI_RUIM_CHV2,
            GSDI_USIM_UNIV_PIN,
            GSDI_USIM_APP1_PIN1,
            GSDI_USIM_APP1_PIN2,
            GSDI_USIM_APP2_PIN1,
            GSDI_USIM_APP2_PIN2,
            GSDI_USIM_APP3_PIN1,
            GSDI_USIM_APP3_PIN2,
            GSDI_USIM_APP4_PIN1,
            GSDI_USIM_APP4_PIN2
    3.  char * pin [ INPUT ] - MANDATORY
        Pointer to char buffer of 8 bytes and only 8 bytes.

    4.  gsdi_app_id_tyep   aid [ INPUT ] - OPTIONAL
        CARD IS USIM:  NULL Indicates USIM App should be used
                       NOT NULL Assumes valid Application ID is
                       present.  Will use Application ID to select
                       Application and then enable PIN.
        CARD IS GSM:   IGNORED
        CARD IS RUIM:  IGNORED
    5.  client_ref
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    6.  gsdi_async_cb
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -esym(818,pin,aid) suppress could be const */
gsdi_returns_T gsdi2_enable_pin(
    gsdi_slot_id_type        slot,
    gsdi_pin_ids_T           pin_id,
    char                   * pin,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb )
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_enable_pin_req_T * msg = 0x00;
    gsdi_returns_T          gsdi_status = GSDI_ERROR;
    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)pin))
    {
       return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    if ( pin == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL PIN PTR",0,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_enable_pin_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_enable_pin_req_T));

    /* Start setting the Message Contents for
        further processing */
    /* MESSAGE HEADER INFORMATION */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_ENABLE_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_enable_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Populate Message Contents */
    msg->pin_id            = pin_id;
    memcpy( msg->pin, pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_enable_pin */  /*lint !e715 */
/*lint -save +esym(818,pin,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_DISABLE_PIN

DESCRIPTION
  This function attempts to disable either PIN1 or PIN2 for the required Slot.
  This function will only support asynchronous calls.  Thus, the caller will
  need to provide a callback function.

    Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  gsdi_pin_id_T   pin [ INPUT ] - MANDATORY
            GSDI_PIN1,
            GSDI_PIN2,
            GSDI_RUIM_CHV1,
            GSDI_RUIM_CHV2,
            GSDI_USIM_UNIV_PIN,
            GSDI_USIM_APP1_PIN1,
            GSDI_USIM_APP1_PIN2,
            GSDI_USIM_APP2_PIN1,
            GSDI_USIM_APP2_PIN2,
            GSDI_USIM_APP3_PIN1,
            GSDI_USIM_APP3_PIN2,
            GSDI_USIM_APP4_PIN1,
            GSDI_USIM_APP4_PIN2
    3.  char * pin [ INPUT ] - MANDATORY
        Pointer to char buffer of 8 bytes and only 8 bytes.

    4.  gsdi_app_id_tyep   aid [ INPUT ] - OPTIONAL
        CARD IS USIM:  NULL Indicates USIM App should be used
                       NOT NULL Assumes valid Application ID is
                       present.  Will use Application ID to select
                       Application and then enable PIN.
        CARD IS GSM:   IGNORED
        CARD IS RUIM:  IGNORED
    5.  client_ref [ INPUT/OUTPUT ]
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    6.  gsdi_async_cb [ INPUT ]
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -esym(818,pin,aid) suppress could be const */
gsdi_returns_T gsdi2_disable_pin(
    gsdi_slot_id_type        slot,
    gsdi_pin_ids_T           pin_id,
    char                   * pin,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb )
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_disable_pin_req_T * msg = 0x00;
    gsdi_returns_T           gsdi_status = GSDI_ERROR;
    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)pin))
    {
       return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    if ( pin == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL PIN PTR",0,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_disable_pin_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_enable_pin_req_T));

    /* Start setting the Message Contents for
        further processing */
    /* MESSAGE HEADER INFORMATION */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_DISABLE_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_disable_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Populate Message Contents */
    msg->pin_id            = pin_id;
    memcpy( msg->pin, pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);
    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_disable_pin */  /*lint !e715 */
/*lint -save +esym(818,pin,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_UNBLOCK_PIN

DESCRIPTION
  This function attempts to unblick either PIN1 or PIN2 for the required Slot.
  This function will only support asynchronous calls.  Thus, the caller will
  need to provide a callback function.

    Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
        GSDI_SLOT_NONE: select and use slot 1
        GSDI_SLOT_1:    select and use slot 1
        GSDI_SLOT_2:    select and use slot 2
    2.  gsdi_pin_id_T   pin [ INPUT ] - MANDATORY
            GSDI_PIN1,
            GSDI_PIN2,
            GSDI_RUIM_CHV1,
            GSDI_RUIM_CHV2,
            GSDI_USIM_UNIV_PIN,
            GSDI_USIM_APP1_PIN1,
            GSDI_USIM_APP1_PIN2,
            GSDI_USIM_APP2_PIN1,
            GSDI_USIM_APP2_PIN2,
            GSDI_USIM_APP3_PIN1,
            GSDI_USIM_APP3_PIN2,
            GSDI_USIM_APP4_PIN1,
            GSDI_USIM_APP4_PIN2
    3.  char * puk [ INPUT ] - MANDATORY
        Pointer to char buffer of 8 bytes that contains pin/chv unblock code.
    4.  char * pin [ INPUT ] - MANDATORY
        Pointer to char buffer of 8 bytes that contains new pin/chv
    5.  gsdi_app_id_tyep   aid [ INPUT ] - OPTIONAL
        CARD IS USIM:  NULL Indicates USIM App should be used
                       NOT NULL Assumes valid Application ID is
                       present.  Will use Application ID to select
                       Application and then enable PIN.
        CARD IS GSM:   IGNORED
        CARD IS RUIM:  IGNORED
    6.  client_ref [ INPUT/OUTPUT ]
        User Data that can be passed in.  Exact data is returned upon
        completion of this procedure.
    7.  gsdi_async_cb [ INPUT ]
        Will contain the function to the pointer to be called upon
        completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -esym(818,puk,new_pin,aid) suppress could be const */
gsdi_returns_T gsdi2_unblock_pin(
    gsdi_slot_id_type        slot,
    gsdi_pin_ids_T           pin_id,
    char                   * puk,
    char                   * new_pin,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb )
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_unblock_pin_req_T * msg = 0x00;
    gsdi_returns_T           gsdi_status = GSDI_ERROR;
    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ( ( slot != GSDI_SLOT_NONE ) &&
         ( slot != GSDI_SLOT_1 )    &&
         ( slot != GSDI_SLOT_2 ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if (!mmgsdi_util_is_pin_valid((uint8 *)new_pin))
    {
       return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    if ( ( puk     == NULL ) ||
         ( new_pin == NULL ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL PIN PTRS",0,0,0);
        return GSDI_ERROR;
    }
    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_unblock_pin_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_unblock_pin_req_T));

    /* Message Header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_UNBLOCK_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_unblock_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.gsdi_async_callback = async_cb;
    msg->message_header.slot     = slot;

    /* Message Contents */
    msg->pin_id            = pin_id;
    memcpy( msg->puk,     puk,     GSDI_PUK_LEN );
    memcpy( msg->new_pin, new_pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext((void*)msg);

    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_unblock_pin */  /*lint !e715 */
/*lint -save +esym(818,puk,new_pin,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_CHANGE_PIN

DESCRIPTION
  This function attempts to cahnge either PIN1 or PIN2 for the required Slot.
  This function will only support asynchronous calls.  Thus, the caller will
  need to provide a callback function.

    Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
            GSDI_SLOT_NONE: select and use slot 1
            GSDI_SLOT_1:    select and use slot 1
            GSDI_SLOT_2:    select and use slot 2
    2.  gsdi_pin_id_T   pin [ INPUT ] - MANDATORY
              GSDI_PIN1,
              GSDI_PIN2,
              GSDI_RUIM_CHV1,
              GSDI_RUIM_CHV2,
              GSDI_USIM_UNIV_PIN,
              GSDI_USIM_APP1_PIN1,
              GSDI_USIM_APP1_PIN2,
              GSDI_USIM_APP2_PIN1,
              GSDI_USIM_APP2_PIN2,
              GSDI_USIM_APP3_PIN1,
              GSDI_USIM_APP3_PIN2,
              GSDI_USIM_APP4_PIN1,
              GSDI_USIM_APP4_PIN2
    3.  char * old_pin [ INPUT ] - MANDATORY
            Pointer to char buffer of 8 bytes that contains old chv/pin code.
    4.  char * new_pin [ INPUT ] - MANDATORY
            Pointer to char buffer of 8 bytes that contains new pin/chv
    5.  gsdi_app_id_type   aid [ INPUT ] - OPTIONAL
            CARD IS USIM:  NULL Indicates USIM App should be used
                       NOT NULL Assumes valid Application ID is
                       present.  Will use Application ID to select
                       Application and then enable PIN.
            CARD IS GSM:   IGNORED
            CARD IS RUIM:  IGNORED
    6.  client_ref [ INPUT/OUTPUT ]
            User Data that can be passed in.  Exact data is returned upon
            completion of this procedure.
      7.  gsdi_async_cb [ INPUT ]
            Will contain the function to the pointer to be called upon
            completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -esym(818,old_pin,new_pin,aid) suppress could be const */
gsdi_returns_T gsdi2_change_pin (
    gsdi_slot_id_type        slot,
    gsdi_pin_ids_T           pin_id,
    char                   * old_pin,
    char                   * new_pin,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_change_pin_req_T *          msg = NULL;
    gsdi_returns_T                   gsdi_status = GSDI_ERROR;
    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ( ( slot != GSDI_SLOT_NONE ) &&
         ( slot != GSDI_SLOT_1 )    &&
         ( slot != GSDI_SLOT_2 ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* check if pin is in the correct length and data range */
    if ((!mmgsdi_util_is_pin_valid((uint8 *)old_pin)) ||
        (!mmgsdi_util_is_pin_valid((uint8 *)new_pin)) )
    {
       return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    if ( ( old_pin == NULL ) ||
         ( new_pin == NULL ) )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL PIN PTRS",0,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_change_pin_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Initialize Heap space retrieved to NULLs */
    memset (msg,0x00,sizeof(gsdi_change_pin_req_T));

    /* Message header */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_CHANGE_PIN_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_change_pin_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* Message Contents */
    msg->pin_id            = pin_id;
    memcpy( msg->old_pin, old_pin, GSDI_PIN_LEN );
    memcpy( msg->new_pin, new_pin, GSDI_PIN_LEN );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);
    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_change_pin */ /*lint !e715 */
/*lint -save +esym(818,old_pin,new_pin,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDI2_GET_SIM_CAPABILITIES

DESCRIPTION
  This function returns the capabilites of the SIM requested.  This function
  is performed asynchronously.  Thus it is required that the caller provied
  a callback.

  The capabilites of the SIM will be returned in the callback function.

        Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
                    GSDI_SLOT_1:        select and use slot 1
                    GSDI_SLOT_2:        select and use slot 2
    2.  gsdi_app_id_type   aid [ INPUT ] - OPTIONAL
                    CARD IS USIM:  NULL Indicates USIM App should be used
                                           NOT NULL Assumes valid Application ID is
                                           present.  Will use Application ID to select
                                           Application and then enable PIN.
                    CARD IS GSM:   IGNORED
                    CARD IS RUIM:  IGNORED
    3.  client_ref [ INPUT/OUTPUT ]
                    User Data that can be passed in.  Exact data is returned upon
                    completion of this procedure.
    4.  gsdi_async_cb [ INPUT ]
                    Will contain the function to the pointer to be called upon
                    completion of this procedure.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -esym(818,aid) suppress could be const */
gsdi_returns_T gsdi2_get_sim_capabilities (
    gsdi_slot_id_type        slot,
    gsdi_app_id_type       * aid,
    uint32                   client_ref,
    gsdi_async_cb            async_cb
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_get_sim_capabilities_req_T  * msg = NULL;
    gsdi_returns_T                     gsdi_status = GSDI_ERROR;
    /* First Check All Mandatory Parameters */
    /* Is Slot Valid                        */
    if ((slot != GSDI_SLOT_NONE) &&
        (slot != GSDI_SLOT_1)    &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* Verify Async Call back is valid */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    /* Has the GSDI lib been initialized yet ? */
    if ( gsdi_lib_initialised == FALSE ) {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }  /* gsdi_lib_initialised = FALSE */

    msg = mmgsdi_malloc(sizeof(gsdi_get_sim_capabilities_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation. */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
        return GSDI_ERROR;
    }

    /* Message Header */
    msg->message_header.message_set  = MS_GSDI;
    msg->message_header.message_id   = GSDI_GET_SIM_CAPABILITIES_REQ;

    GSDI_PUT_IMH_LEN( sizeof( gsdi_get_sim_capabilities_req_T ), &msg->message_header );
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;

    /* Message Contents */
    msg->queue_id           = GS_QUEUE_GSDI_LIB;
    msg->client_ref         = client_ref;
    msg->message_header.gsdi_async_callback = async_cb;
    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE( msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_get_sim_capabilities */  /*lint !e715 */
/*lint -save +esym(818,aid) suppress could be const */


/*===========================================================================
FUNCTION GSDIi2_LIB_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function check whether the service is available in the different technology's
  service table based on the input slot information.
  It calls the corresponding utility functions based on the card moe
  returned from the gsdi_get_card_mode().  The return struct has a gsdi_returns_T and
  bitmap that are populated in correspondence to the check.

  Parameter
    slot [ INPUT ] indicates which slot the service is being requested from
    service [ INPUT ] indicates which service is being check for

DEPENDENCIES
  None

RETURN VALUE
  gsdi_svr_rsp_type
===========================================================================*/
gsdi_svr_rsp_type gsdi2_lib_is_service_available(
    gsdi_slot_id_type  slot,
    gsdi_services_type service
)
{
    gsdi_svr_rsp_type               gsdi_svr_rsp;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_returns_T                  gsdi_status;

    /* initialize gsdi_svr_rsp.svr_bitmap to 0 */
    gsdi_svr_rsp.svr_bitmap = 0x00;
    gsdi_svr_rsp.gsdi_status = GSDI_SUCCESS;

    if ((slot != GSDI_SLOT_1) &&
        (slot != GSDI_SLOT_2))
    {
        MMGSDI_DEBUG_MSG_ERROR("Invalid Slot %d", slot, 0, 0);
        gsdi_svr_rsp.gsdi_status = GSDI_ERROR;
        return gsdi_svr_rsp;
    }
    switch  (slot)
    {
    case GSDI_SLOT_1:
      if (gsdi_data.card_applications.slot1_present)
      {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
#ifdef FEATURE_MMGSDI_CDMA
        if ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
            == GSDI_RUIM_APP_MASK)
        { /* CDMA present in slot 1, check the service table */
          gsdi_status = gsdi_util_ruim_service_check(slot, &gsdi_svr_rsp, service);
          if(gsdi_status != GSDI_SUCCESS && gsdi_svr_rsp.gsdi_status != GSDI_SUCCESS) {
            /* all services table fail */
            gsdi_svr_rsp.gsdi_status = GSDI_INCORRECT_PARAMS;
          }
          else {  /* one of them is successful */
            gsdi_svr_rsp.gsdi_status = GSDI_SUCCESS;
          }
        }
#endif /*#ifdef FEATURE_MMGSDI_CDMA*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* #ifdef FEATURE_MMGSDI_UMTS */
      } /* slot 1 present */
      else {
        gsdi_svr_rsp.gsdi_status = GSDI_ERROR;
      }
      break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      if (gsdi_data.card_applications.slot2_present)
      {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
#ifdef FEATURE_MMGSDI_CDMA
        if ((gsdi_data.card_applications.apps2_enabled & GSDI_RUIM_APP_MASK)
            == GSDI_RUIM_APP_MASK)
        { /* CDMA present in slot 2, check the service table */
          gsdi_status = gsdi_util_ruim_service_check(slot, &gsdi_svr_rsp, service);
          if(gsdi_status != GSDI_SUCCESS && gsdi_svr_rsp.gsdi_status != GSDI_SUCCESS) {
            /* all services table fail */
            gsdi_svr_rsp.gsdi_status = GSDI_INCORRECT_PARAMS;
          }
          else {  /* one of them is successful */
            gsdi_svr_rsp.gsdi_status = GSDI_SUCCESS;
          }
        }
#endif /*#ifdef FEATURE_MMGSDI_CDMA*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*#ifdef FEATURE_MMGSDI_UMTS*/
      } /* slot 2 present */
      else {
        gsdi_svr_rsp.gsdi_status = GSDI_ERROR;
      }
      break;
#endif
    }

    return gsdi_svr_rsp;
#else
    /* initialize gsdi_svr_rsp.svr_bitmap to 0 */
    gsdi_svr_rsp.svr_bitmap = 0x00;
    gsdi_svr_rsp.gsdi_status = GSDI_NOT_SUPPORTED;
    return gsdi_svr_rsp;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_lib_is_service_available */ /*lint !e715 */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI2_SWITCH_SLOT_PREFERENCE

DESCRIPTION
  This function will be used to switch the slot preference from which the
  subscription information is to be obtained from.  This will be the case
  when there is ambiguity as to which slot the subscription information should
  be obtained from.  For example, SLOT 1 & SLOT 2 have a GSM Subscription.  Or,
  SLOT 1 and SLOT 2 both have a 1X Subscription.  In the case the initial
  power up doesn't make the correct assumption, usage of this Library will
  allow for the user preference to be set.

  Parameter
    1.  gsdi_slot_id_type slot:  [ INPUT ]
                                 indicates which slot should be the preferred
                                 slot for the subscription information.
    2.  uint32 client_ref        [ INPUT/OUTPUT ]
                                 User Data that can be passed in.  Exact data
                                 is returned upon completion of this
                                 procedure.
    4.  gsdi_async_cb async_cb   [ INPUT ]
                                 Will contain the function to the pointer to
                                 be called upon completion of this procedure.

DEPENDENCIES
  None

SIDE EFFECTS
  Will cause the reinitialization of the SIM Related Tasks.
  Will result in the current Subscriptions (GSM/1X Stacks) to be taken down.
  Will result in the reinitialization of the Subscriptions (GSM/1X Stacks)

  Will result in multiple events.

LIMITATIONS
  Only supported in Asynchronous Mode.

RETURN VALUE
  gsdi_returns_T:   GSDI_ERROR -> Bad Slot Param = GSDI_SLOT_NONE
                               -> Bad async_cb   = NULL
===========================================================================*/
gsdi_returns_T gsdi2_switch_slot_preference(
  gsdi_slot_id_type     slot,
  gsdi_app_enum_type    app,
  uint32                client_ref,
  gsdi_async_cb         async_cb)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_switch_slot_preference_req_T * msg = NULL_PTR;
    gsdi_returns_T                      gsdi_status = GSDI_ERROR;
    /* ----------------------------------------------------------------------
        Check All Mandatory Parameters
        1.  Is the Slot passed in valid
            NOTE:  GSDI_SLOT_AUTOMATIC is not a valid slot options.
        ---------------------------------------------------------------------- */
    if ( slot != GSDI_SLOT_NONE &&
         slot != GSDI_SLOT_1     &&
         slot != GSDI_SLOT_2 )
    {
        return GSDI_ERROR;
    }
    /* ----------------------------------------------------------------------
        2.  Verify a valid APP was provided
            Only GSDI_GSM_APP or GSDI_CDMA_APP are acceptable
        ---------------------------------------------------------------------- */
    if ( app != GSDI_GSM_APP &&
         app != GSDI_CDMA_APP )
    {
        return GSDI_ERROR;
    }
    /* ----------------------------------------------------------------------
        3.  Verify Callback is not NULL
        ---------------------------------------------------------------------- */
    if ( async_cb == NULL )
    {
        return GSDI_ERROR;
    }

    /* ----------------------------------------------------------------------
        Allocate Memory for Request
        ---------------------------------------------------------------------- */
    msg = mmgsdi_malloc( sizeof(gsdi_switch_slot_preference_req_T));

    if ( msg == NULL )
    {
        ERR_FATAL("SWITCH SLOT PREF: COULD NOT ALLOC MEM",0,0,0);
    }

    /* ----------------------------------------------------------------------
        Build the Switch Slot Preference Request Header
        ---------------------------------------------------------------------- */
    /* Message Header */
    msg->message_header.message_set         = MS_GSDI;
    msg->message_header.message_id          = GSDI_SWITCH_SLOT_PREF_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr             = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig            = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot                = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* ----------------------------------------------------------------------
        Fill in Switch Slot Preference Request Header Length Information
        ---------------------------------------------------------------------- */
    GSDI_PUT_IMH_LEN( sizeof( gsdi_switch_slot_preference_req_T ), &msg->message_header );

    /* ----------------------------------------------------------------------
        Fill in Switch Slot Preference Request Message Contents
        ---------------------------------------------------------------------- */
    msg->app        = app;
    msg->client_ref = client_ref;
    msg->queue_id   = GS_QUEUE_GSDI_LIB;

    /* ----------------------------------------------------------------------
        Queue Msg for GSDI to process
        ---------------------------------------------------------------------- */
    gsdi_status = gsdi_cmd_ext((void *)msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    /* ------------------------------------------------------------------------
       Free up memory allocated for request
       ----------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(msg);

    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi2_switch_slot_preference */ /*lint !e715 */


/*===========================================================================
FUNCTION GSDI_GET_USER_PREFERRED_SLOT

DESCRIPTION
  This function will access GSDI Global Data directly to obtain the Slot
  preferences stored in NV.

  Parameter
    1.  gsdi_app_enum_type app   [ INPUT ]
                                 indicates which slot should be the preferred
                                 slot for the subscription information.
    2.  gsdi_slot_id_type * slot [ OUTPUT ]
                                 User Data that can be passed in.  Exact data
                                 is returned upon completion of this
                                 procedure.

DEPENDENCIES
  None

SIDE EFFECTS
  None

LIMITATIONS
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS.  Slot preference is either GSDI_SLOT_NONE
                                  GSDI_SLOT_1 or GSDI_SLOT_2.
                   GSDI_ERROR.    GSDI Global Data Slot preferences is set
                                  to GSDI_SLOT_AUTOMATIC.  Error indicates
                                  NV could not be set at one point, or reads
                                  failed.
===========================================================================*/
gsdi_returns_T gsdi_get_user_preferred_slot (
    gsdi_app_enum_type    app,
    gsdi_slot_id_type *   slot)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if (slot == NULL)
      return GSDI_ERROR;

    /* initialize to automatic */
    *slot = GSDI_SLOT_AUTOMATIC;

    switch (app)
    {
    case GSDI_GSM_APP:
      *slot = gsdi_data.user_slot_prefs.gsm_app_pref;
      break;
    case GSDI_CDMA_APP:
      *slot = gsdi_data.user_slot_prefs.cdma_app_pref;
      break;
    default:
      /* unknown app */
      break;
    }
    if (*slot == GSDI_SLOT_AUTOMATIC)
      return GSDI_ERROR;

  return GSDI_SUCCESS;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
} /* gsdi_get_user_preferred_slot */ /*lint !e715 */


/*===========================================================================
FUNCTION GSDI_SEND_APDU

DESCRIPTION
  Function is used to send a APDU Stream to the card.  The APDU stream must
  be compliant with ISO/IEC 7816-3, GSM11.11, TS 31.102 and IS-820A.

  This API will not support a request with GSDI_SLOT_NONE or
  GSDI_SLOT_AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_svr_rsp_type
===========================================================================*/
/*lint -save -esym(818,aid,apdu_data_p) suppress could be const */
gsdi_returns_T gsdi2_send_apdu(
    gsdi_slot_id_type        slot,
    gsdi_app_id_type       * aid,
    uint32                   apdu_len,
    uint8                  * apdu_data_p,
    uint32                   client_ref,
    gsdi_async_cb            async_cb
)
{
#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
    gsdi_send_apdu_req_T *           msg = NULL_PTR;
    gsdi_returns_T                   gsdi_status = GSDI_ERROR;

    /* ----------------------------------------------------------------------
       First Check All Mandatory Parameters
       Is slot Valid?
       ---------------------------------------------------------------------- */
    if ( slot != GSDI_SLOT_1 &&
         slot != GSDI_SLOT_2 )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD SLOT INFORMATION %x",slot,0,0);
        return GSDI_ERROR;
    }

    /* ----------------------------------------------------------------------
       Is async_cb valid
       ---------------------------------------------------------------------- */
    if ( async_cb == NULL )
    {
        MMGSDI_DEBUG_MSG_ERROR("NULL ASYNC_CB %x",async_cb,0,0);
        return GSDI_ERROR;
    }

    /* ----------------------------------------------------------------------
       Is apdu_data_p buffer valid and is apdu_len less than the max
       allwed of GSDI_MAX_DATA_BLOCK_LEN
       ---------------------------------------------------------------------- */
    if ( apdu_data_p == NULL ||
         apdu_len    > GSDI_MAX_DATA_BLOCK_LEN )
    {
        MMGSDI_DEBUG_MSG_ERROR("BAD APDU PARAMS",0,0,0);
        return GSDI_ERROR;
    }

    /* ----------------------------------------------------------------------
       Has the GSDI lib been initialized yet ?
       ---------------------------------------------------------------------- */
    if ( gsdi_lib_initialised == FALSE )
    {
        if ( gsdi_lib_init() == GSDI_SUCCESS )
        {
            gsdi_lib_initialised = TRUE;
        }
        else
        {
            return GSDI_ERROR;
        }
    }

    msg = mmgsdi_malloc(sizeof(gsdi_send_apdu_req_T));

    if ( msg == NULL )
    {
        /* FATAL...No Memory to execute request.  Bad Situation.
        */
        ERR_FATAL("NO HEAP SPACE AVAILABLE",0,0,0);
    }

    /* ----------------------------------------------------------------------
       Initialize Heap space retrieved to NULLs
       ---------------------------------------------------------------------- */
    memset (msg,0x00,sizeof(gsdi_send_apdu_req_T));

    /* ----------------------------------------------------------------------
       Build the Message header
       ---------------------------------------------------------------------- */
    msg->message_header.message_set = MS_GSDI;
    msg->message_header.message_id  = GSDI_SEND_APDU_REQ;

    GSDI_PUT_MSG_LEN(sizeof(gsdi_send_apdu_req_T),&msg->message_header);
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg->message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg->message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg->message_header.slot     = slot;
    msg->message_header.gsdi_async_callback = async_cb;

    /* ----------------------------------------------------------------------
       Add the Message Contents
       ---------------------------------------------------------------------- */
    msg->apdu_len          = apdu_len;
    memcpy( msg->apdu_data, apdu_data_p, msg->apdu_len );
    msg->queue_id          = GS_QUEUE_GSDI_LIB;
    msg->client_ref        = client_ref;

    gsdi_status = gsdi_cmd_ext(msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE(msg);
    return gsdi_status;
#else
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */
} /* gsdi2_send_apdu */ /*lint !e715 */
/*lint -save +esym(818,aid,apdu_data_p) suppress could be const */

/*===========================================================================
FUNCTION GSDI_NAS_GET_CARD_MODE

DESCRIPTION
  Specialized API to return the Card mode of the inserted card.  This API
  was developed specifically for all tasks that only need to know if a
  GSM SIM Card is inserted or a USIM Card is inserted.

  FEATURE_MMGSDI_CDMA:
  This API will basically assume that Multimode is not supported (i.e.
  CDMA RUIM).  Therefore, if the only app Available in the card inserted
  is CDMA, the card mode returned is GSDI_CARD_MODE_UNKNOWN.

  FEATURE_MMGSDI_DUAL_SLOT:
  This API will assume single slot operation.  It will check both slots
  if necessary, but will simply return the card mode of the card inserted.
  For the dual slot targets, it is either GSDI_CARD_MODE_GSM or
  GSDI_CARD_MODE_UNKNOWN.

INPUT
  gsdi_card_mode_capability_T * :  Pointer to a gsdi_card_mode_capability_T
                                   structure

OUTPUT
  None

DEPENDENCIES
  Requires that GSDI has completed the SIM Initialization procedures.

RETURN VALUE
  gsdi_returns_T:  GSDI_ERROR - card_mode is NULL

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_nas_get_card_mode(
    gsdi_card_mode_capability_T * card_mode
)
{
  uint8 gsm_mask  = 0x00;
  uint8 usim_mask = 0x00;

  if ( card_mode == NULL )
  {
    return GSDI_ERROR;
  }

  if ( gsdi_data.card_applications.slot1_present)
  {
    usim_mask = gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK;

    if ( usim_mask )
    {
      *card_mode = GSDI_CARD_USIM;
      return GSDI_SUCCESS;
    }

    gsm_mask = gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK;

    if ( gsm_mask )
    {
      *card_mode = GSDI_CARD_GSM;
      return GSDI_SUCCESS;
    }
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ( gsdi_data.card_applications.slot2_present)
  {
    gsm_mask = gsdi_data.card_applications.apps2_enabled & GSDI_GSM_SIM_MASK;

    if ( gsm_mask )
    {
      *card_mode = GSDI_CARD_GSM;
      return GSDI_SUCCESS;
    }
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  *card_mode = GSDI_CARD_MODE_UNKNOWN;
  return GSDI_SUCCESS;

}
/*===========================================================================
FUNCTION GSDI_SIM_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

INPUT:
  gsdi_slot_id_type slot:          Slot to send the cmd to
  uint32            client_ref:    User Data returned in response
  gsdi_async_cb     gsdi_asnyc_cb: Callback GSDI Calls to provide gsdi_cnf_T

OUTPUT
  This function does not provide any output other than the return status.
  The outcome of this Command/Request is provided in the gsdi_cnf_T
  provided when GSDI Calls the Requester's callback function.

DEPENDENCIES
  Requires that GSDI has completed the SIM Initialization procedures.

RETURN VALUE
  gsdi_returns_T:
    GSDI_SUCCESS:           Request accepted and Queued to GSDI.
    GSDI_ERROR:             Could not Queue Message
    GSDI_INCORRECT_PARAMS:  The parameters provided don't pass
                            the basic param checking.
SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T  gsdi_sim_get_atr(
  gsdi_slot_id_type   slot,
  uint32              client_ref,
  gsdi_async_cb       async_cb
)
{
  gsdi_returns_T            gsdi_status       = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  gsdi_get_atr_req_T *      get_atr_req_p     = NULL;

  /* --------------------------------------------------------------------------
     Validate all parameters passed into this function
     1.  Slot ID:               Valid Values:  GSDI_SLOT_1 or GSDI_SLOT_2
     2.  client_ref:            Valid Values:  No Parameter Checking
     3.  aysnc_cb:              Valid Values:  Must not be NULL
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1 &&
       slot != GSDI_SLOT_2 )
    return GSDI_INCORRECT_PARAMS;

  if ( async_cb == NULL )
    return GSDI_INCORRECT_PARAMS;

  /* --------------------------------------------------------------------------
     Allocate memory for the request and initialize it
     --------------------------------------------------------------------------*/
  get_atr_req_p = mmgsdi_malloc(sizeof(gsdi_get_atr_req_T));
  if ( get_atr_req_p == NULL )
  {
    ERR_FATAL("Could not allocate memory for get atr req",0,0,0);
  }

  memset(get_atr_req_p,0x00,sizeof(gsdi_get_atr_req_T));

  /* --------------------------------------------------------------------------
     Build the Message Header Contents
     ------------------------------------------------------------------------*/
  get_atr_req_p->message_header.message_set         = MS_GSDI;
  get_atr_req_p->message_header.message_id          = GSDI_GET_ATR_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  get_atr_req_p->message_header.tcb_ptr             = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  get_atr_req_p->message_header.wait_sig            = GSDI_LIB_CMD_Q_SIG;
  get_atr_req_p->message_header.gsdi_async_callback = async_cb;
  get_atr_req_p->client_ref                         = client_ref;
  get_atr_req_p->message_header.slot                = slot;

  GSDI_PUT_MSG_LEN(sizeof(gsdi_get_atr_req_T),&get_atr_req_p->message_header);

  /* ------------------------------------------------------------------------
      Send / Queue the Message with the GSDI Task
      ----------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)get_atr_req_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
      Free Up the Memory Allocated for this Request
      ----------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(get_atr_req_p);
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
  return gsdi_status;
} /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_SIM_OPEN_CHANNEL

DESCRIPTION:
  This is the API used to open a channel with the UICC/RUIM Card.  This
  command is not supported for a GSM SIM.  It is required that this API is called
  prior to calling the gsdi_sim_send_channel_data() API.  Upon successful
  execution of this Command, a channel will be opened and allocated for the
  requesting task to use.

INPUT:
  gsdi_slot_id_type slot:           Slot to send the cmd to
  gsdi_aid_type *   app_id          Pointer to Application ID Buffer.
                                    Optional.  Must be set to NULL if not used.
  uint32            client_ref:     User Data returned in response
  gsdi_async_cb     gsdi_asnyc_cb:  Callback GSDI Calls to provide gsdi_cnf_T

OUTPUT
  This function does not provide any output other than the return status.
  The outcome of this Command/Request is provided in the gsdi_cnf_T
  provided when GSDI Calls the Requester's callback function.

DEPENDENCIES
  Requires that GSDI has completed the SIM Initialization procedures.
  RUIM:  ISO-820A Specifications for Number of channels available.
  UICC:  TS 102.221 Specifications for Number of channesl available.

RETURN VALUE
  gsdi_returns_T:
    GSDI_SUCCESS:           Request accepted and Queued to GSDI.
    GSDI_ERROR:             Could not Queue Message
    GSDI_INCORRECT_PARAMS:  The parameters provided don't pass
                            the basic param checking.

SIDE EFFECTS
  Successful execution of this Command/Request results in the removeal
  of Channels available.  Calling this function too many time without
  releasing the channels can and will eventually exhaust the number of
  channels avaialble in the card.
===========================================================================*/
gsdi_returns_T  gsdi_sim_open_channel(
  gsdi_slot_id_type   slot,
  gsdi_app_id_type *  app_id_p,
  int32               channel_id,
  int32               requested_channel_id,
  uint32              client_ref,
  gsdi_async_cb       async_cb
)
{
  gsdi_returns_T            gsdi_status = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  gsdi_open_channel_req_T * open_channel_req_p   = NULL;

  /* --------------------------------------------------------------------------
     Validate all parameters passed into this function
     1.  Slot ID:               Valid Values:  GSDI_SLOT_1 or GSDI_SLOT_2
     2.  channel_id:            Valid Values:  Must be greater than 0
     3.  requested_channel_id:  Valid Values:  Must be greater than 0
     4.  client_ref:            Valid Values:  No Parameter Checking
     5.  aysnc_cb:              Valid Values:  Must not be NULL
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1 &&
       slot != GSDI_SLOT_2 )
    return GSDI_INCORRECT_PARAMS;

  if ( channel_id < 0 ||
       requested_channel_id < 0 )
    return GSDI_INCORRECT_PARAMS;

  if ( async_cb == NULL )
    return GSDI_INCORRECT_PARAMS;

  /* -------------------------------------------------------------------------
     Perform Parameter Checking on the app_id_p passed into this function
        1.  Verify the app_id_p != NULL
        2.  Verify if app_id_p is valid, that app_id_len <= GSDI_MAX_APP_ID_LEN
        3.  Verify if app_id_p is valid, that app_id_data is not NULL
        4.  Once all checks have passed, it is ok to copy APP ID Data
     ------------------------------------------------------------------------*/
  if ( app_id_p != NULL )
  {
    if ( app_id_p->aid_len  >  GSDI_MAX_APP_ID_LEN )
      return GSDI_INCORRECT_PARAMS;

    if ( app_id_p->aid_len < GSDI_MIN_APP_ID_LEN )
      return GSDI_INCORRECT_PARAMS;

    if ( app_id_p->aid_data == NULL ) /*lint !e774 */
      return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Allocate memory for the request and initialize it
     --------------------------------------------------------------------------*/
  open_channel_req_p = mmgsdi_malloc(sizeof(gsdi_open_channel_req_T));
  if ( open_channel_req_p == NULL )
  {
    ERR_FATAL("Could not allocate memory for open channel req",0,0,0);
  }

  memset(open_channel_req_p,0x00,sizeof(gsdi_open_channel_req_T));

  /* --------------------------------------------------------------------------
     Build the Message Header Contents
     ------------------------------------------------------------------------*/
  open_channel_req_p->message_header.message_set         = MS_GSDI;
  open_channel_req_p->message_header.message_id          = GSDI_OPEN_CHANNEL_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  open_channel_req_p->message_header.tcb_ptr             = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  open_channel_req_p->message_header.wait_sig            = GSDI_LIB_CMD_Q_SIG;
  open_channel_req_p->message_header.gsdi_async_callback = async_cb;
  open_channel_req_p->client_ref                         = client_ref;
  open_channel_req_p->message_header.slot                = slot;
  open_channel_req_p->channel_id                         = channel_id;
  open_channel_req_p->requested_channel_id               = requested_channel_id;

  GSDI_PUT_MSG_LEN(sizeof(gsdi_open_channel_req_T),&open_channel_req_p->message_header);

  /* ------------------------------------------------------------------------
      Set the Remainder of the Message / Request
     ----------------------------------------------------------------------*/
  if ( app_id_p != NULL )
  {
    /* A Valid Application ID Pointer was provided */
    open_channel_req_p->app_id.aid_len       = app_id_p->aid_len;
    memcpy(open_channel_req_p->app_id.aid_data,
            app_id_p->aid_data,
            app_id_p->aid_len);
  }

  /* ------------------------------------------------------------------------
      Send / Queue the Message with the GSDI Task
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)open_channel_req_p);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }

  /* ------------------------------------------------------------------------
      Free Up the Memory Allocated for this Request
    ----------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE( open_channel_req_p);
#endif /*   FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
  return gsdi_status;
} /* gsdi_sim_open_channel */  /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_SIM_CLOSE_CHANNEL

DESCRIPTION:
  This is the API used to close a currently opened and assigned channel which
  in turn returns it to the pool of channels available in the card.
  The requested channel to close must be a currently open channel and must
  currently exist within the range of channels supported by the card.

INPUT:
  gsdi_slot_id_type slot:           Slot to send the cmd to
  uint32            channel_id:     Channel to transport data on
  int32             requested_channel_id: Channel to close
  uint32            client_ref:     User Data returned in response
  gsdi_async_cb     gsdi_asnyc_cb:  Callback GSDI Calls to provide gsdi_cnf_T

OUTPUT
  This function does not provide any output other than the return status.
  The outcome of this Command/Request is provided in the gsdi_cnf_T
  provided when GSDI Calls the Requester's callback function.

DEPENDENCIES
  Requires that GSDI has completed the SIM Initialization procedures.
  Requires that gsdi_sim_open_channel was called for the channel to
  be closed.
  RUIM:  ISO-820A Specifications for Number of channels available.
  UICC:  TS 102.221 Specifications for Number of channesl available.

RETURN VALUE
  gsdi_returns_T:
    GSDI_SUCCESS:           Request accepted and Queued to GSDI.
    GSDI_ERROR:             Could not Queue Message
    GSDI_INCORRECT_PARAMS:  The parameters provided don't pass
                            the basic param checking.
SIDE EFFECTS
  Every successful close channel request results in that channel_id
  returning to the pool of available channel_ids.
===========================================================================*/
gsdi_returns_T  gsdi_sim_close_channel(
  gsdi_slot_id_type   slot,
  int32               channel_id,
  int32               requested_channel_id,
  uint32              client_ref,
  gsdi_async_cb       async_cb
)
{
  gsdi_returns_T             gsdi_status = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  gsdi_close_channel_req_T * close_channel_req_p = NULL;
  /* --------------------------------------------------------------------------
     Validate all parameters passed into this function
     1.  Slot ID:               Valid Values:  GSDI_SLOT_1 or GSDI_SLOT_2
     2.  client_ref:            Valid Values:  No Parameter Checking
     3.  aysnc_cb:              Valid Values:  Must not be NULL
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1 &&
       slot != GSDI_SLOT_2 )
    return GSDI_INCORRECT_PARAMS;

  if ( async_cb == NULL )
    return GSDI_INCORRECT_PARAMS;

  /* --------------------------------------------------------------------------
     Allocate memory for the request and initialize it
     --------------------------------------------------------------------------*/
  close_channel_req_p = mmgsdi_malloc(sizeof(gsdi_close_channel_req_T));
  if ( close_channel_req_p == NULL )
  {
    ERR_FATAL("Could not allocate memory for close channel req",0,0,0);
  }

  memset(close_channel_req_p,0x00,sizeof(gsdi_close_channel_req_T));

  /* --------------------------------------------------------------------------
     Build the Message Header Contents
     ------------------------------------------------------------------------*/
  close_channel_req_p->message_header.message_set    = MS_GSDI;
  close_channel_req_p->message_header.message_id     = GSDI_CLOSE_CHANNEL_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  close_channel_req_p->message_header.tcb_ptr        = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  close_channel_req_p->message_header.wait_sig       = GSDI_LIB_CMD_Q_SIG;
  close_channel_req_p->message_header.gsdi_async_callback = async_cb;
  close_channel_req_p->client_ref                    = client_ref;

  GSDI_PUT_MSG_LEN(sizeof(gsdi_close_channel_req_T),&close_channel_req_p->message_header);

  close_channel_req_p->message_header.slot = slot;
  close_channel_req_p->channel_id          = channel_id;
  close_channel_req_p->requested_channel_id = requested_channel_id;

  /* ------------------------------------------------------------------------
     Send / Queue the Message with the GSDI Task
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)close_channel_req_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Free the Memory Allocated for this request
     ----------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(close_channel_req_p);
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
  return gsdi_status;
} /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_SIM_SEND_CHANNEL_DATA

DESCRIPTION:
  This is the API used to send and APDU to the channel opened in the
  gsdi_sim_open_channel() REQ.

  The APDU Provided is expected to follow the ISO/EIC 7816-4 Specifications.
  The format is based on ISO7816-4 (sec5.3) and  following are the four
  different command APDUs

    CLA INS P1 P2
    CLA INS P1 P2 Le
    CLA INS P1 P2 Lc DATA
    CLA INS P1 P2 Lc DATA Le

INPUT:
  gsdi_slot_id_type slot:          Slot to send the cmd to
  uint32            channel_id:    Channel to transport data on
  uint32            apdu_len:      Length of Data Provided
  uint32            apdu_buffer_p: Buffer that contains APDU
  uint32            client_ref:    User Data returned in response
  gsdi_async_cb     gsdi_asnyc_cb: Callback GSDI Calls to provide gsdi_cnf_T

OUTPUT
  This function does not provide any output other than the return status.
  The outcome of this Command/Request is provided in the gsdi_cnf_T
  provided when GSDI Calls the Requester's callback function.

DEPENDENCIES
  Requires that GSDI has completed the SIM Initialization procedures.
  Requires that gsdi_sim_open_channel was called for the channel to
  be closed.
  RUIM:  ISO-820A Specifications for Number of channels available.
  UICC:  TS 102.221 Specifications for Number of channesl available.
  ISO/EIC 7816-4 Specifications on APDU Formats

RETURN VALUE
  gsdi_returns_T:
    GSDI_SUCCESS:           Request accepted and Queued to GSDI.
    GSDI_ERROR:             Could not Queue Message
    GSDI_INCORRECT_PARAMS:  The parameters provided don't pass
                            the basic param checking.
SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T  gsdi_sim_send_channel_data(
  gsdi_slot_id_type   slot,
  int32               channel_id,
  int32               apdu_len,
  byte *              apdu_buffer_p,
  uint32              client_ref,
  gsdi_async_cb       async_cb
)
{
  gsdi_returns_T                 gsdi_status = GSDI_ERROR;
#if defined(FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || defined(FEATURE_MMGSDI_ATCSIM)
  gsdi_send_channel_data_req_T * send_channel_data_req_p = NULL;

  /* --------------------------------------------------------------------------
     Validate all parameters passed into this function
     1.  Slot ID:         Valid Values:  GSDI_SLOT_1 or GSDI_SLOT_2
     2.  channel_id:      Valid Values:  > 0.  Max not checked in Library
                          Valid Values:  >= 0 when FEATURE_MMGSDI_ATCSIM is def
     3.  apdu_len:        Valid Values:  0 < apdu_len < GSDI_MAX_DATA_BLOCK_LEN
     4.  apdu_buffer:     Valid Values:  Must not be NULL
     5.  client_ref:      Valid Values:  No Parameter Checking
     6.  aysnc_cb:        Valid Values:  Must not be NULL
     7.  apdu_len, apdu_buffer:  Valid Values:  Must be more than 10 bytes if
                                                select by APDU
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1 &&
       slot != GSDI_SLOT_2 )
    return GSDI_INCORRECT_PARAMS;

#ifdef FEATURE_MMGSDI_ATCSIM
  if ( channel_id < 0 )
    return GSDI_INCORRECT_PARAMS;
#else
  if ( channel_id <= 0 )
    return GSDI_INCORRECT_PARAMS;
#endif /* FEATURE_MMGSDI_ATCSIM */

  if ( apdu_len <= 0 ||
       apdu_len >  GSDI_MAX_CHANNEL_DATA_CMD_LEN )
   return GSDI_INCORRECT_PARAMS;

  if ( apdu_buffer_p == NULL )
    return GSDI_INCORRECT_PARAMS;

  if ( async_cb == NULL )
    return GSDI_INCORRECT_PARAMS;

  if (( apdu_buffer_p[1] == 0xA4 ) &&
      ( apdu_buffer_p[2] == 0x04 ) &&
      ( apdu_buffer_p[4] < 5 ))
     return GSDI_INCORRECT_PARAMS;

  /* --------------------------------------------------------------------------
     Allocate memory for the request and initialize it
     --------------------------------------------------------------------------*/
  send_channel_data_req_p = mmgsdi_malloc
                                     (sizeof(gsdi_send_channel_data_req_T));
  if ( send_channel_data_req_p == NULL )
  {
    ERR_FATAL("Could not allocate memory for send channel data req",0,0,0);
  }

  memset(send_channel_data_req_p,0x00,sizeof(gsdi_send_channel_data_req_T));

  /* --------------------------------------------------------------------------
     Build the Message Header Contents
     ------------------------------------------------------------------------*/
  send_channel_data_req_p->message_header.message_set = MS_GSDI;
  send_channel_data_req_p->message_header.message_id  = GSDI_SEND_CHANNEL_DATA_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  send_channel_data_req_p->message_header.tcb_ptr     = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  send_channel_data_req_p->message_header.wait_sig    = GSDI_LIB_CMD_Q_SIG;
  send_channel_data_req_p->message_header.gsdi_async_callback = async_cb;
  send_channel_data_req_p->client_ref                 = client_ref;
  send_channel_data_req_p->message_header.slot        = slot;

  GSDI_PUT_MSG_LEN(sizeof(gsdi_send_channel_data_req_T),&send_channel_data_req_p->message_header);

  /* ------------------------------------------------------------------------
     Populate remainder of Message / Request
     ----------------------------------------------------------------------*/
  send_channel_data_req_p->channel_id = channel_id;
  send_channel_data_req_p->apdu_len   = apdu_len;
  memcpy(send_channel_data_req_p->data_buffer,
         apdu_buffer_p,
         int32touint32(apdu_len));

  /* ------------------------------------------------------------------------
     Send / Queue the Message with the GSDI Task
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)send_channel_data_req_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }


  /* ------------------------------------------------------------------------
     Free the Memory Allocated for this request
     ----------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(send_channel_data_req_p);
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT || FEATURE_MMGSDI_ATCSIM */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_OTA_DEPERSO

DESCRIPTION
  This function is called by the task handling the OTA SMS Messages "WMS".
  WMS will call this Library when it determines the SMS PP contains user
  data relavent to OTA Depersonalization.

  This is achived by generated an GSDI_PERSO_OTA_DERPERSO_REQ for the
  GSDI Task to process.

  Important note: This API is not intended for "SMS-PP Download" OTA
                  Depersonalization.  This requires the handling Task,
                  WMS, to use the GSTK API to send an SMS PP Download.

  INPUT:          gsdi_pero_ota_deperso_type * ota_data
                      Pointer to Struture with SMS PP OTA User Data. Data
                      will be in packed format as described in 22.022 Annex
                      A.4.
                      Data will contain the ME's IMEI
                      Data will contain the Control Keys to be verified.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  void * gsdi_perso_async_cmd_cb
                      Eventually gets casted to a gsdi_async_cb

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T * provided
    in the gsdi_async_cb pointer.

DEPENDENCIES
  None

SIDE EFFECTS
  If the Control Keys are incorrect, it will cause the MEs internal
  Control Key Number Of Retries counter to increase by one or increase
  a timer to handle the next requrest or both depnding the the Deter
  Method selected.  Once it reaches the MAX as defined by
  #define MAX_DEPERSONALIZATION_FAILURES N, the Mobile will not allow any
  subsequent depersonalizations.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_perso_return_enum_type:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status of the attempt to perform an OTA
      Depersonalization will be returned in the gsdi_cnf_T.
===========================================================================*/
gsdi_returns_T gsdi_perso_ota_deperso(
  gsdi_perso_ota_deperso_type * sms_ota_user_data,
  uint32                        client_ref,
  gsdi_async_cb                 async_cb
)
{
  gsdi_returns_T                gsdi_status = GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_ota_deperso_req_T  ota_deperso_req_p;
  MSG_HIGH("GSDI PERSO SET FEATURE DATA",0,0,0);
  /* --------------------------------------------------------------------------
     Perform required parameter checking
     1.  sms_ota_user_data can not be NULL
     2.  slot can only be slot1 or slot2 or slot Automatic
     3.  Length can not be 0 and can not be greater than the buffer provied
     4.  The data buffer provided can not be null
     5.  async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( sms_ota_user_data == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL sms_ota_user_data provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( sms_ota_user_data->slot != GSDI_SLOT_1 &&
       sms_ota_user_data->slot != GSDI_SLOT_AUTOMATIC
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && sms_ota_user_data->slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD Slot ID: %x",sms_ota_user_data->slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( sms_ota_user_data->num_bytes <= 0x00 ||
       sms_ota_user_data->num_bytes > GSDI_PERSO_MAX_OTA_DEPERSO_BYTES )
  {
    MMGSDI_DEBUG_MSG_ERROR("bad length provided: %x",sms_ota_user_data->num_bytes,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( sms_ota_user_data->deperso_user_data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null Deperso Keys Buffer provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Callback Pointer provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Build the Message Header Contents
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  ota_deperso_req_p.message_header.message_set = MS_GSDI;
  ota_deperso_req_p.message_header.message_id  = GSDI_PERSO_OTA_DERPERSO_REQ;
  ota_deperso_req_p.message_header.slot        = GSDI_SLOT_NONE;
  ota_deperso_req_p.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_ota_deperso_req_T ),
    &ota_deperso_req_p.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     It will be the responsibility of MMGSDI to parse the user data into
     the different control keys and issue out individual commands to turn off
     depersonalization for the control key provided.
     ------------------------------------------------------------------------*/
  ota_deperso_req_p.client_ref    = client_ref;
  ota_deperso_req_p.num_bytes     = sms_ota_user_data->num_bytes;
  memcpy(ota_deperso_req_p.data_buffer,
         sms_ota_user_data->deperso_user_data_p,
         int32touint32(ota_deperso_req_p.num_bytes));

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&ota_deperso_req_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
} /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_REGISTER_TASK

DESCRIPTION
  This function is called by a task that would like to register to callback
  to receive GSDI Personalization events.

  gsdi_perso_events_enum_type gsdi_perso_event - Event Generated
  boolean additional_information               - Whether additional infomration is present,
  uint32  additional_information_len           - Length of additional information when present,
  uint8 * gsdi_perso_data_p                    - Pointer to additional information


  INPUT:          gsdi_perso_events_cb gsdi_perso_cb
                      Function Pointer to be called when an event is generated.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  The first Registration of Personalization Events will kick off the
  personalization procedures.  This in effect will enable the GSDI
  PERSO Functionality to send events to the registered Task.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_perso_return_enum_type:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provied
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_register_task(
  gsdi_perso_events_cb          perso_events_cb,
  uint32                        client_ref,
  gsdi_async_cb                 async_cb
)
{
  gsdi_returns_T            gsdi_status = GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_reg_task_req_T perso_reg_task_req;
  MSG_HIGH("GSDI PERSO REGISTER TASK",0,0,0);

  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. perso_events_cb can not be null
     2. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( perso_events_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("perso_events_cb is null : %x",perso_events_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("async_cb is null : %x",async_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  perso_reg_task_req.message_header.message_set = MS_GSDI;
  perso_reg_task_req.message_header.message_id  = GSDI_PERSO_REGISTER_TASK_REQ;
  perso_reg_task_req.message_header.slot        = GSDI_SLOT_NONE;
  perso_reg_task_req.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_reg_task_req_T ),
    &perso_reg_task_req.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_reg_task_req.client_ref    = client_ref;
  perso_reg_task_req.reg_task_cb_p = perso_events_cb;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&perso_reg_task_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_ACTIVATE_FEATURE_INDICATOR

DESCRIPTION
  This function is called to activate a personalization feature by an external
  source.  The Feature and Control Key are required.

  When called for the first time, the control key provided will overwrite the
  default (initial) control key stored in NV.  The control key defaulted is
  as follows:  Bytes[1:17] = 0xFF

  So long as the control key is 0xFF, personalization procedures are not
  performed even if the indicator is on.

  INPUT:          gsdi_perso_control_key_data_type * control_key_info
                      Pointer to structure with Control Key Information.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Personalization Feature Indicators can only be activated one at a time.

RETURN VALUE
  gsdi_perso_return_enum_type:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provied
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_activate_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info_p,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
)
{
  gsdi_returns_T                   gsdi_status=GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_act_feature_ind_req_T perso_activate_req;
  MSG_HIGH("GSDI PERSO ACTIVATE FEATURE INDICATOR",0,0,0);

  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. control_key_info can not be null
     1. control_key_info->slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. control_key_info can not be null
     3. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( control_key_info_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("control_key_info_p is NULL",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && control_key_info_p->slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",control_key_info_p->slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->control_key_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null Control Key Pointer ",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("async_cb is null : %x",async_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->num_bytes != GSDI_PERSO_MAX_CK )
  {
    MMGSDI_DEBUG_MSG_ERROR("Num Bytes for CK: %x Not equal to CK Len: %x",control_key_info_p->num_bytes,
                            GSDI_PERSO_MAX_CK,0);

    perso_activate_req.num_bytes = GSDI_PERSO_MAX_CK;
    memset(perso_activate_req.ck_buffer, 0xFF, GSDI_PERSO_MAX_CK);
  }
  else
  {
    perso_activate_req.num_bytes       = control_key_info_p->num_bytes;
    memcpy(perso_activate_req.ck_buffer,control_key_info_p->control_key_p,
      int32touint32(control_key_info_p->num_bytes));
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  perso_activate_req.message_header.message_set = MS_GSDI;
  perso_activate_req.message_header.message_id  = GSDI_PERSO_ACT_FEATURE_IND_REQ;
  perso_activate_req.message_header.slot        = control_key_info_p->slot;
  perso_activate_req.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_act_feature_ind_req_T ),
    &perso_activate_req.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_activate_req.feature         = control_key_info_p->feature;
  perso_activate_req.client_ref      = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&perso_activate_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_DEACTIVATE_FEATURE_INDICATOR

DESCRIPTION
  This function is called to deactivate a personalization feature by an external
  source.  The Feature and Control Key are required.

  When called for the first time, the control key provided will overwrite the
  default (initial) control key stored in NV.  The control key defaulted is
  as follows:  Bytes[1:17] = 0xFF

  So long as the control key is 0xFF, personalization procedures are not
  performed even if the indicator is on.

  INPUT:          gsdi_perso_control_key_data_type * control_key_info
                      Pointer to structure with Control Key Information.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Personalization Feature Indicators can only be deactivated one at a time.

RETURN VALUE
  gsdi_perso_return_enum_type:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provied
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_deactivate_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info_p,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
)
{
  gsdi_returns_T                     gsdi_status = GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_deact_feature_ind_req_T perso_deactivate_req;
  MSG_HIGH("GSDI PERSO DEACTIVATE FEATURE INDICATOR",0,0,0);

  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. control_key_info can not be null
     1. control_key_info->slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. control_key_info can not be null
     3. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( control_key_info_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("control_key_info_p is NULL",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && control_key_info_p->slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",control_key_info_p->slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->control_key_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null Control Key Pointer ",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("async_cb is null : %x",async_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->num_bytes != GSDI_PERSO_MAX_CK )
  {
    MMGSDI_DEBUG_MSG_ERROR("Num Bytes for CK: %x Not equal to CK Len: %x",control_key_info_p->num_bytes,
                            GSDI_PERSO_MAX_CK,0);

    perso_deactivate_req.num_bytes = GSDI_PERSO_MAX_CK;
    memset(perso_deactivate_req.ck_buffer, 0xFF, GSDI_PERSO_MAX_CK);
  }
  else
  {
    perso_deactivate_req.num_bytes       = control_key_info_p->num_bytes;
    memcpy(perso_deactivate_req.ck_buffer,control_key_info_p->control_key_p,
      int32touint32(control_key_info_p->num_bytes));
  }

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  perso_deactivate_req.message_header.message_set = MS_GSDI;
  perso_deactivate_req.message_header.message_id  = GSDI_PERSO_DEACT_FEATURE_IND_REQ;
  perso_deactivate_req.message_header.slot        = control_key_info_p->slot;
  perso_deactivate_req.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_deact_feature_ind_req_T ),
    &perso_deactivate_req.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_deactivate_req.feature         = control_key_info_p->feature;
  perso_deactivate_req.client_ref      = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&perso_deactivate_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_PERM_FEATURE_INDICATORS

DESCRIPTION
  This function is called to retrieve the Permanently Disabled
  Personalization Indicators

  INPUT:          uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Individual requests for personalization indicators are not supported.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_get_perm_feature_indicators(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{
  gsdi_returns_T    gsdi_status = GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_get_perm_feature_ind_req_T perso_get_feature_ind_req;

  MSG_HIGH("GSDI PERSO GET PERM FEATURE INDICATORS",0,0,0);
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    && slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("async_cb is null : %x",async_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  perso_get_feature_ind_req.message_header.message_set = MS_GSDI;
  perso_get_feature_ind_req.message_header.message_id  = GSDI_PERSO_GET_PERM_FEATURE_IND_REQ;
  perso_get_feature_ind_req.message_header.slot        = slot;
  perso_get_feature_ind_req.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_get_feature_ind_req_T ),
    &perso_get_feature_ind_req.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_get_feature_ind_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&perso_get_feature_ind_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
} /* gsdi_perso_get_perm_feature_indicators */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_PERM_DISABLE_FEATURE_INDICATOR

DESCRIPTION
  This function is called to permanently disable a personalization feature by an
  external source.  The Feature and Control Key are required.

  INPUT:          gsdi_perso_control_key_data_type * control_key_info
                      Pointer to structure with Control Key Information.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Personalization Feature Indicators can only be permanently disable one at a time.

RETURN VALUE
  gsdi_perso_return_enum_type:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provied
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_perm_disable_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info_p,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
)
{
  gsdi_returns_T                      gsdi_status=GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_perm_disabled_feature_req_T perso_disable_req;
  MSG_HIGH("GSDI PERSO DISBLE FEATURE INDICATOR",0,0,0);

  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. control_key_info can not be null
     1. control_key_info->slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. control_key_info->control_key_p can not be null
     3. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( control_key_info_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("control_key_info_p is NULL",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && control_key_info_p->slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",control_key_info_p->slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->control_key_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null Control Key Pointer ",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("async_cb is null : %x",async_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( control_key_info_p->num_bytes != GSDI_PERSO_MAX_CK )
  {
    MMGSDI_DEBUG_MSG_ERROR("Num Bytes for CK: %x Not equal to CK Len: %x",control_key_info_p->num_bytes,
                            GSDI_PERSO_MAX_CK,0);

    perso_disable_req.num_bytes = GSDI_PERSO_MAX_CK;
    memset(perso_disable_req.ck_buffer, 0xFF, GSDI_PERSO_MAX_CK);
  }
  else
  {
    perso_disable_req.num_bytes       = control_key_info_p->num_bytes;
    memcpy(perso_disable_req.ck_buffer,control_key_info_p->control_key_p,
      int32touint32(control_key_info_p->num_bytes));
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  perso_disable_req.message_header.message_set = MS_GSDI;
  perso_disable_req.message_header.message_id  = GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ;
  perso_disable_req.message_header.slot        = control_key_info_p->slot;
  perso_disable_req.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_perm_disabled_feature_req_T ),
    &perso_disable_req.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_disable_req.perso_feature   = control_key_info_p->feature;
  perso_disable_req.client_ref      = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&perso_disable_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
} /* gsdi_perso_perm_disable_feature_indicator */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_FEATURE_INDICATORS

DESCRIPTION
  This function is called to retrieve the Personalization Indicators

  INPUT:          uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Individual requests for personalization indicators are not supported.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_get_feature_indicators(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{
  gsdi_returns_T    gsdi_status=GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_get_feature_ind_req_T perso_get_feature_ind_req;

  MSG_HIGH("GSDI PERSO GET FEATURE INDICATORS",0,0,0);
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    && slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( async_cb == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("async_cb is null : %x",async_cb,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  perso_get_feature_ind_req.message_header.message_set = MS_GSDI;
  perso_get_feature_ind_req.message_header.message_id  = GSDI_PERSO_GET_FEATURE_IND_REQ;
  perso_get_feature_ind_req.message_header.slot        = slot;
  perso_get_feature_ind_req.message_header.gsdi_async_callback = async_cb;

  GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_get_feature_ind_req_T ),
    &perso_get_feature_ind_req.message_header );

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_get_feature_ind_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&perso_get_feature_ind_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_SET_FEATURE_DATA

DESCRIPTION
  This function is called to set the personalization data for a particular
  personalization feature.

  INPUT:          gsdi_perso_feature_data_type *
                      Pointer to structure that contains the Personalization
                      Feature and Data to be set.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  The Persoanlization Feature Codes can only be set 1 Feature at a time.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status of the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_set_feature_data(
  gsdi_perso_set_feature_data_type * perso_codes,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
)
{
  gsdi_returns_T                    gsdi_status=GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  gsdi_perso_set_feature_data_req_T * perso_set_data_req_p = NULL;

  MSG_HIGH("GSDI PERSO SET FEATURE DATA",0,0,0);
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. perso_codes can not be null
     2. slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     3. async_cb can not be null
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(perso_codes);
  MMGSDI_RETURN_IF_OUT_OF_RANGE(perso_codes->slot,GSDI_SLOT_1, GSDI_SLOT_2);
  MMGSDI_RETURN_IF_EXCEEDS(perso_codes->num_bytes,GSDI_MAX_DATA_BLOCK_LEN);
  MMGSDI_RETURN_IF_NULL(perso_codes->perso_data_p);
  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
     Get memory for the Set Feature Data Request
     ------------------------------------------------------------------------*/
  perso_set_data_req_p = mmgsdi_malloc(
    sizeof(gsdi_perso_set_feature_data_req_T));
  if ( perso_set_data_req_p == NULL )
  {
    ERR_FATAL("No memory for Personalization Set Data Request",0,0,0);
  }

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_set_feature_data_req_T),perso_set_data_req_p);

  perso_set_data_req_p->message_header.message_set = MS_GSDI;
  perso_set_data_req_p->message_header.message_id  = GSDI_PERSO_SET_FEATURE_DATA_REQ;
  perso_set_data_req_p->message_header.slot        = perso_codes->slot;
  perso_set_data_req_p->message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_set_data_req_p->feature    = perso_codes->feature;
  perso_set_data_req_p->client_ref = client_ref;
  perso_set_data_req_p->num_bytes  = perso_codes->num_bytes;

  /* --------------------------------------------------------------------------
     Copy over data the Personalization Codes / Data.  Some additional checks
     are not necessary but made anyways.
     ------------------------------------------------------------------------*/
  if ( (perso_codes->perso_data_p != NULL) &&
       (perso_codes->num_bytes > 0)        &&
       (perso_codes->num_bytes < GSDI_MAX_DATA_BLOCK_LEN ))
  {
    memcpy(perso_set_data_req_p->data_buffer,
           perso_codes->perso_data_p,
           int32touint32(perso_codes->num_bytes));
  }
  else
  {
    /* Parameter check on perso_codes failed */
    MMGSDI_DEBUG_ERR("Mem malloc perso_set_data_req_p data buffer null",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(perso_set_data_req_p);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(perso_set_data_req_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_FREE(perso_set_data_req_p);
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_FEATURE_DATA

DESCRIPTION
  This function is called to get the personalization data for a particular
  personalization feature.

  INPUT:          gsdi_perso_enum_type
                      Personalization Feature from which the Data need to
                      be read.

                  gsdi_slot_id_type
                      slot information

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      GSDI_INCORECT_PARAMS: Indicates incorrect parameters

      Note..the return status of the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_get_feature_data(
  gsdi_perso_enum_type feature_ind,
  gsdi_slot_id_type    slot,
  uint32               client_ref,
  gsdi_async_cb        async_cb
)
{
  gsdi_returns_T                    gsdi_status=GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifdef FEATURE_GSDI_PERSO_GET_DATA
  gsdi_perso_get_feature_data_req_T * perso_get_data_req_p = NULL;

  MSG_HIGH("GSDI PERSO GET FEATURE DATA",0,0,0);
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. async_cb can not be null
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
     Get memory for the Get Feature Data Request
     ------------------------------------------------------------------------*/
  perso_get_data_req_p = mmgsdi_malloc(
    sizeof(gsdi_perso_get_feature_data_req_T));
  if ( perso_get_data_req_p == NULL )
  {
    MMGSDI_DEBUG_ERR("No memory for Personalization Get Data Request",0,0,0);
    gsdi_status = GSDI_ERROR;
    return gsdi_status;
  }/* if ( perso_get_data_req_p == NULL ) */

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_get_feature_data_req_T),perso_get_data_req_p);

  perso_get_data_req_p->message_header.message_set = MS_GSDI;
  perso_get_data_req_p->message_header.message_id  = GSDI_PERSO_GET_FEATURE_DATA_REQ;
  perso_get_data_req_p->message_header.slot        = slot;
  perso_get_data_req_p->message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_get_data_req_p->feature    = feature_ind;
  perso_get_data_req_p->client_ref = client_ref;

    /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(perso_get_data_req_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_ERR("Queing of command failed",0,0,0);
    gsdi_status = GSDI_ERROR;
  }
  else
  {
    MSG_HIGH("Queing of command successful",0,0,0);
  }
  MMGSDIUTIL_TMC_MEM_FREE(perso_get_data_req_p);
#endif /* FEATURE_GSDI_PERSO_GET_DATA  */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_DCK_NUM_RETRIES

DESCRIPTION
  This function is called to get the number of depersonalization attempts
  remaining.  Everytime a Depersonalization attempt is made and it fails,
  the number of attempts remaining will also decrement.  Therefore, this
  function can be used to check the number of retries remaining.  This
  function will return the Number of Retries for All Features regardless
  of the feature indicator state.

  INPUT:
                  gsdi_slot_id_type slot
                      Slot info on which this command is sent

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Individual requests for personalization indicators are not supported.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_get_dck_num_retries(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{
  gsdi_returns_T                      gsdi_status = GSDI_NOT_SUPPORTED;
#if defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION)
  gsdi_perso_get_dcks_num_retries_req_T perso_get_num_retries_req;

  MSG_HIGH("GSDI PERSO GET DCK NUM OF RETRIES",0,0,0);
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    && slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  MMGSDI_RETURN_IF_NULL(async_cb);
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_get_dcks_num_retries_req_T),&perso_get_num_retries_req);

  perso_get_num_retries_req.message_header.message_set = MS_GSDI;
  perso_get_num_retries_req.message_header.message_id  = GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ;
  perso_get_num_retries_req.message_header.slot        = slot;
  perso_get_num_retries_req.message_header.gsdi_async_callback = async_cb;


  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_get_num_retries_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&perso_get_num_retries_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("get_dck_num_retries cmd post to gsdi Queue failed",0,0,0);
    gsdi_status = GSDI_ERROR;
  }
#endif /*FEATURE_MMGSDI_PERSONALIZATION_ENGINE && FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/* gsdi_perso_get_dck_num_retries */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES

DESCRIPTION
  This function is called to get the number of unblock on DCK attempts
  remaining.  Everytime an unblock Depersonalization attempt is made and it
  fails, the number of ublock attempts remaining will also decrement.
  Therefore, this function can be used to check the number of unblock
  retries remaining.  This function will return the Number of unblock
  Retries for All Features regardless of the feature indicator state.

  INPUT:
                  gsdi_slot_id_type slot
                      Slot info on which this command is sent

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Individual requests for personalization indicators are not supported.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_get_dck_unblock_retries(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{
  gsdi_returns_T                      gsdi_status = GSDI_NOT_SUPPORTED;
#if defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION)
  gsdi_perso_get_dck_unblock_num_retries_req_T perso_get_unblock_num_retries_req;

  MSG_HIGH("GSDI PERSO GET DCK UNBLOCK NUM RETRIES",0,0,0);
  /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1. slot can be GSDI_SLOT_1 or GSDI_SLOT_2 only
     2. async_cb can not be null
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    && slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  MMGSDI_RETURN_IF_NULL(async_cb);
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_get_dck_unblock_num_retries_req_T),&perso_get_unblock_num_retries_req);

  perso_get_unblock_num_retries_req.message_header.message_set = MS_GSDI;
  perso_get_unblock_num_retries_req.message_header.message_id  = GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ;
  perso_get_unblock_num_retries_req.message_header.slot        = slot;
  perso_get_unblock_num_retries_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  perso_get_unblock_num_retries_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&perso_get_unblock_num_retries_req);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("get_dck_unblock_num_retries cmd post to gsdi Queue failed",0,0,0);
    gsdi_status = GSDI_ERROR;
  }
#endif /*FEATURE_MMGSDI_PERSONALIZATION_ENGINE && FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/* gsdi_perso_get_dck_unblock_retries */ /*lint !e715 !e818 */

/*===========================================================================
FUNCTION GSDI_PERSO_CHANGE_DCK

DESCRIPTION
  This function is called to change the Depersonalization control key for
  the feature specified.  It must contain a valid Depersonalization Control
  Key for the indicator provided as well as the new Depersonalization Control
  Key.

  INPUT:

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Individual requests for personalization indicators are not supported.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
/*lint -save -esym(818,change_dck_info_p) suppress could be const */
gsdi_returns_T gsdi_perso_change_dck(
  gsdi_perso_change_control_key_data_type       * change_dck_info_p,
  uint32                                          client_ref,
  gsdi_async_cb                                   async_cb
)
{
  return GSDI_SUCCESS;
} /* gsdi_perso_change_dck */ /*lint !e715 */
/*lint -save +esym(818,change_dck_info_p) suppress could be const */

/*===========================================================================
FUNCTION GSDI_PERSO_UNBLOCK_FEATURE_INDICATOR

DESCRIPTION
  This function is called to unblock a personalization feature by an external
  source.  The Feature and Control Key are required.

  So long as the control key is 0xFF, personalization procedures are not
  performed even if the indicator is on.

  INPUT:          gsdi_perso_control_key_data_type * control_key_info
                      Pointer to structure with Control Key Information.

                  uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_async_cb async_cb
                      Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Personalization Feature Indicators can only be deactivated one at a time.

RETURN VALUE
  gsdi_perso_return_enum_type:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provied
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_unblock_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info_p,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
)
{
  gsdi_returns_T                         gsdi_status = GSDI_NOT_SUPPORTED;
#if defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_MMGSDI_PERSONALIZATION)
  gsdi_perso_unblock_feature_ind_req_T   perso_unblock_req;

  MSG_HIGH("GSDI PERSO UNBLOCK FEATURE INDICATOR",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);
  MMGSDI_RETURN_IF_NULL(control_key_info_p);
  MMGSDI_RETURN_IF_NULL(control_key_info_p->control_key_p);
  if (control_key_info_p->num_bytes <= 0)
  {
    return GSDI_ERROR;
  }

  if ( control_key_info_p->num_bytes != GSDI_PERSO_MAX_CK )
  {
    MMGSDI_DEBUG_MSG_ERROR("Num Bytes for CK: %x Not equal to CK Len: %x",control_key_info_p->num_bytes,
                            GSDI_PERSO_MAX_CK,0);

    perso_unblock_req.num_bytes = GSDI_PERSO_MAX_CK;
    memset(perso_unblock_req.unblock_ck_buffer, 0xFF, GSDI_PERSO_MAX_CK);
  }
  else
  {
    perso_unblock_req.num_bytes       = control_key_info_p->num_bytes;
    memcpy(perso_unblock_req.unblock_ck_buffer,control_key_info_p->control_key_p,
      int32touint32(control_key_info_p->num_bytes));
  }

  /* --------------------------------------------------------------------------
    Fill In Message Header Contents
    ------------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_unblock_feature_ind_req_T)),&perso_unblock_req);

  perso_unblock_req.message_header.message_set = MS_GSDI;
  perso_unblock_req.message_header.message_id  = GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ;
  perso_unblock_req.message_header.slot        = control_key_info_p->slot;
  perso_unblock_req.message_header.gsdi_async_callback = async_cb;

    /* --------------------------------------------------------------------------
      Fill In Remainder of Message Contents
      ------------------------------------------------------------------------*/
  perso_unblock_req.client_ref      = client_ref;
  perso_unblock_req.feature         = control_key_info_p->feature;

  gsdi_status = gsdi_cmd_ext(&perso_unblock_req);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE && FEATURE_MMGSDI_PERSONALIZATION */
  return gsdi_status;
}/*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION GSDI_SIM_SECURE_STORE

DESCRIPTION
  This function is called to write to the secure EFS

  INPUT:
  int32  data_len        Data Len to be written
  uint8  *data_buf_ptr   Data Stream
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_sim_secure_store(
  gsdi_slot_id_type slot,
  int32            len,
  uint8           *data_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{
  gsdi_returns_T                      gsdi_status = GSDI_ERROR;
  gsdi_sim_secure_store_req_T         sim_secure_store_req;

  MSG_HIGH("GSDI SIM SECURE STORE",0,0,0);

  /* --------------------------------------------------------------------------
     Validate the data provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(async_cb);
  MMGSDI_RETURN_IF_NULL(data_ptr);
  if (len <= 0)
  {
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Initialize Data Pointer
     ------------------------------------------------------------------------*/
  memset(&sim_secure_store_req, 0x00, sizeof(gsdi_sim_secure_store_req_T));

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_sim_secure_store_req_T) + int32touint32(len)),&sim_secure_store_req);

  sim_secure_store_req.message_header.message_set = MS_GSDI;
  sim_secure_store_req.message_header.message_id  = GSDI_SIM_SECURE_STORE_REQ;
  sim_secure_store_req.message_header.slot        = slot;
  sim_secure_store_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  sim_secure_store_req.client_ref    = client_ref;
  sim_secure_store_req.data_len      = len;

  sim_secure_store_req.data_buf_ptr = mmgsdi_malloc(sim_secure_store_req.data_len);
  if (sim_secure_store_req.data_buf_ptr == NULL)
  {
    return GSDI_ERROR;
  }
  memcpy(sim_secure_store_req.data_buf_ptr, data_ptr, int32touint32(len));

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&sim_secure_store_req);

  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Wipe out the contents of this memory location and then free it.
     ------------------------------------------------------------------------*/
  memset(sim_secure_store_req.data_buf_ptr,0x00,len);
  MMGSDIUTIL_TMC_MEM_FREE(sim_secure_store_req.data_buf_ptr);

  return gsdi_status;
} /* gsdi_sim_secure_store */

/*===========================================================================
FUNCTION GSDI_SIM_SECURE_READ

DESCRIPTION
  This function is called to read the secure EFS

  INPUT:
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_sim_secure_read(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{
  gsdi_returns_T                      gsdi_status = GSDI_ERROR;

  #ifdef FEATURE_MMGSDI_DEBUG_SIM_PARTITION

  gsdi_sim_secure_read_req_T          sim_secure_read_req;

  MSG_HIGH("GSDI SIM SECURE READ",0,0,0);
  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_sim_secure_read_req_T),&sim_secure_read_req);

  sim_secure_read_req.message_header.message_set = MS_GSDI;
  sim_secure_read_req.message_header.message_id  = GSDI_SIM_SECURE_READ_REQ;
  sim_secure_read_req.message_header.slot        = slot;
  sim_secure_read_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  sim_secure_read_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&sim_secure_read_req);

  #else

  gsdi_status = GSDI_NOT_SUPPORTED;

  #endif /* FEATURE_MMGSDI_DEBUG_SIM_PARTITION */

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_READ_ROOT_KEY

DESCRIPTION
  This function is called to read the root key from OTA

  INPUT:
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_read_root_key(
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{
  gsdi_returns_T                      gsdi_status = GSDI_ERROR;
  gsdi_perso_root_key_read_req_T      root_key_read_req;

  MSG_HIGH("GSDI ROOT KEY READ ",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_root_key_read_req_T)),&root_key_read_req);

  root_key_read_req.message_header.message_set = MS_GSDI;
  root_key_read_req.message_header.message_id  = GSDI_PERSO_ROOT_KEY_READ_REQ;
  root_key_read_req.message_header.slot        = GSDI_SLOT_1;
  root_key_read_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  root_key_read_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&root_key_read_req);

  return gsdi_status;

} /* gsdi_perso_read_root_key */ /*lint !e715 */


/*===========================================================================
FUNCTION GSDI_PERSO_WRITE_ROOT_KEY

DESCRIPTION
  This function is called to write to the root key to OTA

  INPUT:
  int32  data_len        Data Len to be written
  uint8  *data_buf_ptr   Data Stream
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_write_root_key(
  int32            len,
  uint8           *data_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{

  gsdi_returns_T                      gsdi_status = GSDI_ERROR;
  gsdi_perso_root_key_write_req_T     root_key_write_req;

  MSG_HIGH("GSDI ROOT KEY WRITE",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);

  if (len <= 0)
  {
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_root_key_write_req_T) + len),&root_key_write_req);

  root_key_write_req.message_header.message_set = MS_GSDI;
  root_key_write_req.message_header.message_id  = GSDI_PERSO_ROOT_KEY_WRITE_REQ;
  root_key_write_req.message_header.slot        = GSDI_SLOT_1;
  root_key_write_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  root_key_write_req.client_ref    = client_ref;

  root_key_write_req.data_len      = len;
  if (data_ptr == NULL)
  {
    return GSDI_ERROR;
  }
  root_key_write_req.data_buf_ptr = mmgsdi_malloc(root_key_write_req.data_len);
  if (root_key_write_req.data_buf_ptr == NULL)
  {
    return GSDI_ERROR;
  }
  memcpy(root_key_write_req.data_buf_ptr, data_ptr, len);

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&root_key_write_req);

  MMGSDIUTIL_TMC_MEM_FREE(root_key_write_req.data_buf_ptr);

  return gsdi_status;

} /* gsdi_perso_write_root_key */ /*lint !e715 */


/*===========================================================================
FUNCTION GSDI_GET_LIB_VERSION

DESCRIPTION
  This function is called to unblock the phone code

  INPUT:
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_get_lib_version(
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{

  gsdi_returns_T                          gsdi_status = GSDI_ERROR;
  gsdi_perso_get_lib_version_req_T        lib_version_req;

  MSG_HIGH("GSDI GET LIB VERSION ",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);

  /*--------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_get_lib_version_req_T)),&lib_version_req);

  lib_version_req.message_header.message_set = MS_GSDI;
  lib_version_req.message_header.message_id  = GSDI_PERSO_GET_LIB_VERSION_REQ;
  lib_version_req.message_header.slot        = GSDI_SLOT_1;
  lib_version_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  lib_version_req.client_ref    = client_ref;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&lib_version_req);

  return gsdi_status;


} /* gsdi_get_lib_version */ /*lint !e715 */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/*===========================================================================
FUNCTION GSDI_PERSO_VERIFY_PHONECODE

DESCRIPTION
  This function is called to verify the phone code

  INPUT:
  int32  phone_code_len  Phone code Len
  uint8  *phone_code_ptr Phone Code
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnf_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_verify_phonecode(
  int32            phone_code_len,
  uint8           *phone_code_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{
  gsdi_returns_T                          gsdi_status = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_perso_verify_phone_code_req_T      phone_code_req;

  MSG_HIGH("GSDI VERIFY PHONE CODE ",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);
  MMGSDI_RETURN_IF_NULL(phone_code_ptr);
  if (phone_code_len <= 0)
  {
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_verify_phone_code_req_T)),&phone_code_req);

  phone_code_req.message_header.message_set = MS_GSDI;
  phone_code_req.message_header.message_id  = GSDI_PERSO_VERIFY_PHONE_CODE_REQ;
  phone_code_req.message_header.slot        = GSDI_SLOT_1;
  phone_code_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  phone_code_req.client_ref    = client_ref;

  phone_code_req.phone_code_len = phone_code_len;
  phone_code_req.phone_code_ptr = mmgsdi_malloc(int32touint32(phone_code_len));
  MMGSDI_RETURN_IF_NULL(phone_code_req.phone_code_ptr);
  memcpy(phone_code_req.phone_code_ptr, phone_code_ptr, int32touint32(phone_code_len));

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&phone_code_req);

  MMGSDIUTIL_TMC_MEM_FREE(phone_code_req.phone_code_ptr);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
  return gsdi_status;

} /* gsdi_perso_verify_phone_code */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_CHANGE_PHONECODE

DESCRIPTION
  This function is called to change the phone code

  INPUT:
  int32  old_phone_code_len     Phone code len
  uint8  *old_phone_code_ptr    Old phone code
  int32  new_phone_code_len     New Phone code len : Not used for RFU Only
  uint8  *new_phone_code_ptr    New phone code     : Not used for RFU Only
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_change_phonecode(
  int32            old_phone_code_len,
  uint8           *old_phone_code_ptr,
  int32            new_phone_code_len,
  uint8           *new_phone_code_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{

  gsdi_returns_T                          gsdi_status = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_perso_change_phone_code_req_T      phone_code_req;

  MSG_HIGH("GSDI CHANGE PHONE CODE ",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);
  MMGSDI_RETURN_IF_NULL(old_phone_code_ptr);
  if ((old_phone_code_len <= 0) || (new_phone_code_len <= 0))
  {
    return GSDI_ERROR;
  }
  MMGSDI_RETURN_IF_NULL(new_phone_code_ptr);

  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_change_phone_code_req_T)),&phone_code_req);

  phone_code_req.message_header.message_set = MS_GSDI;
  phone_code_req.message_header.message_id  = GSDI_PERSO_CHANGE_PHONE_CODE_REQ;
  phone_code_req.message_header.slot        = GSDI_SLOT_1;
  phone_code_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  phone_code_req.client_ref    = client_ref;

  phone_code_req.old_phone_code_len = old_phone_code_len;
  phone_code_req.old_phone_code_ptr = mmgsdi_malloc(int32touint32(old_phone_code_len));
  MMGSDI_RETURN_IF_NULL(phone_code_req.old_phone_code_ptr);
  memcpy(phone_code_req.old_phone_code_ptr, old_phone_code_ptr, int32touint32(old_phone_code_len));

  phone_code_req.new_phone_code_len = new_phone_code_len;
  phone_code_req.new_phone_code_ptr = mmgsdi_malloc(int32touint32(new_phone_code_len));
  if(phone_code_req.new_phone_code_ptr == NULL)
  {
    MMGSDI_DEBUG_ERR("Mem malloc new phone code data buffer null",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(phone_code_req.old_phone_code_ptr);
    return GSDI_ERROR;
  }
  memcpy(phone_code_req.new_phone_code_ptr, new_phone_code_ptr, int32touint32(new_phone_code_len));

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&phone_code_req);

  MMGSDIUTIL_TMC_MEM_FREE(phone_code_req.old_phone_code_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(phone_code_req.new_phone_code_ptr);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
  return gsdi_status;

} /* gsdi_change_phone_code */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_PERSO_UNBLOCK_PHONECODE

DESCRIPTION
  This function is called to unblock the phone code

  INPUT:
  int32  master_key_len         Master Key len
  uint8  *master_key_ptr        Master Key code
  int32  new_phone_code_len     New Phone code len
  uint8  *new_phone_code_ptr    New phone code
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_unblock_phonecode(
  int32            master_key_len,
  uint8           *master_key_ptr,
  int32            new_phone_code_len,
  uint8           *new_phone_code_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{
  gsdi_returns_T                          gsdi_status = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_perso_unblock_phone_code_req_T     phone_code_req;

  MSG_HIGH("GSDI UNBLOCK PHONE CODE ",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);
  MMGSDI_RETURN_IF_NULL(master_key_ptr);
  MMGSDI_RETURN_IF_NULL(new_phone_code_ptr);
  if ((master_key_len <= 0) || (new_phone_code_len <= 0))
  {
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_unblock_phone_code_req_T)),&phone_code_req);

  phone_code_req.message_header.message_set = MS_GSDI;
  phone_code_req.message_header.message_id  = GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ;
  phone_code_req.message_header.slot        = GSDI_SLOT_1;
  phone_code_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  phone_code_req.client_ref    = client_ref;

  phone_code_req.master_key_len = master_key_len;
  phone_code_req.master_key_ptr = mmgsdi_malloc(int32touint32(master_key_len));
  MMGSDI_RETURN_IF_NULL(phone_code_req.master_key_ptr);
  memcpy(phone_code_req.master_key_ptr, master_key_ptr, int32touint32(master_key_len));

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&phone_code_req);

  MMGSDIUTIL_TMC_MEM_FREE(phone_code_req.master_key_ptr);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
  return gsdi_status;

} /* gsdi_unblock_phone_code */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */


/*===========================================================================
FUNCTION GSDI_PERSO_SET_PHONECODE

DESCRIPTION
  This function is called to unblock the phone code

  INPUT:
  int32  phone_code_len     Phone code len
  uint8  *phone_code_ptr    phone code
  uint32 client_ref      User Data the calling task can send with the request
                         that is returned in the resulting confirmation /
                         response regardless of whether the command succeeds
                         or fails.
  gsdi_async_cb async_cb Callback called by GSDI upon completion of the request

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_perso_set_phonecode(
  int32            phone_code_len,
  uint8           *phone_code_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
)
{
  gsdi_returns_T                          gsdi_status = GSDI_ERROR;
#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
  gsdi_perso_set_phone_code_req_T         phone_code_req;

  MSG_HIGH("GSDI SET PHONE CODE ",0,0,0);

  MMGSDI_RETURN_IF_NULL(async_cb);
  MMGSDI_RETURN_IF_NULL(phone_code_ptr);
  if (phone_code_len <= 0)
  {
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Fill In Message Header Contents
     ------------------------------------------------------------------------*/
  /* Message Header */
  GSDI_PUT_MSG_LEN((sizeof(gsdi_perso_set_phone_code_req_T)),&phone_code_req);

  phone_code_req.message_header.message_set = MS_GSDI;
  phone_code_req.message_header.message_id  = GSDI_PERSO_SET_PHONE_CODE_REQ;
  phone_code_req.message_header.slot        = GSDI_SLOT_1;
  phone_code_req.message_header.gsdi_async_callback = async_cb;

  /* --------------------------------------------------------------------------
     Fill In Remainder of Message Contents
     ------------------------------------------------------------------------*/
  phone_code_req.client_ref    = client_ref;

  phone_code_req.phone_code_len = phone_code_len;
  phone_code_req.phone_code_ptr = mmgsdi_malloc(int32touint32(phone_code_len));
  if(phone_code_req.phone_code_ptr == NULL)
  {
    MMGSDI_DEBUG_ERR("Mem malloc new phone code data buffer null",0,0,0);
    return GSDI_ERROR;
  }
  memcpy(phone_code_req.phone_code_ptr, phone_code_ptr, int32touint32(phone_code_len));

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)&phone_code_req);

  MMGSDIUTIL_TMC_MEM_FREE(phone_code_req.phone_code_ptr);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
  return gsdi_status;


} /* gsdi_set_phone_code */ /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

/*===========================================================================
FUNCTION GSDI_SIM_GET_CPHS_INFORMATION

DESCRIPTION
  This function is used to determine the Common PCN Handset Specifications
  (CPHS) Information stored on the SIM/USIM.

  This API should be called by any Application or Module prior to request
  CPHS File Information.

INPUT:
  gsdi_slot_id_type slot:  Is the CPHS Information requested for Slot 1 or
                           Slot 2.
  uint32:                  User Information to be returned in the callback
  gsdi_async_cb:           Async Callback function to call when we have
                           completed the request.

  OUTPUT:
    None, the output provided back to the caller will exist in the gsdi_cnt_T *
    provided in the gsdi_async_cb.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

LIMITATIONS
  Only supported as an Asynchronous Command.
  Individual requests for personalization indicators are not supported.

RETURN VALUE
  gsdi_returns_T:

      GSDI_SUCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      GSDI_NOT_SUPPORTED: Indicates the CPHS feature is not
                          supported

      Note..the return status the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
gsdi_returns_T gsdi_sim_get_cphs_information(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{
  gsdi_returns_T                             gsdi_status = GSDI_NOT_SUPPORTED;
#ifdef FEATURE_MMGSDI_CPHS
  gsdi_get_cphs_information_req_T            msg;

  /* --------------------------------------------------------------------------
     Basic Parameter Checks
     async_cb:  Can not be NULL
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
     Has the Library Been Initialized?
     ----------------------------------------------------------------------- */
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Populate the Message Header Accordingly for a GET CPHS INFORMATION
     REQUEST.
     ----------------------------------------------------------------------- */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_get_cphs_information_req_T),&msg);

  msg.message_header.message_set         = MS_GSDI;
  msg.message_header.message_id          = GSDI_SIM_GET_CPHS_INFORMATION_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr             = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig            = GSDI_LIB_CMD_Q_SIG;
  msg.message_header.slot                = GSDI_SLOT_1;
  msg.message_header.gsdi_async_callback = async_cb;
  msg.client_ref                         = client_ref;
  msg.queue_id                           = 0;

  /* --------------------------------------------------------------------------
     Queue Command with GSDI Task.
     Reminder:  Memory for Message Queue is allocated in the callers task
                context but then freed in gsdi when the message is removed
                from the Queue for further processing.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&msg);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_CPHS */
  return gsdi_status;
} /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */


/*===========================================================================
FUNCTION GSDI_SIM_GENERIC_SEARCH

DESCRIPTION
  This function will perform a GSM/WCDMA search.

INPUT
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  search_record_num,
  uim_srch_mode        search_mode,
  uim_srch_dir_type    search_direction,
  char                *search_string,
  int                  search_string_len,
  int                 *rec_num_accessed_p,
  uint32               client_ref,
  gsdi_async_cb        async_cb)

LIMITATIONS
  API is only supported in Asynchronous Mode.

DEPENDENCIES
  None

OUTPUT

RETURN VALUE
  gsdi_returns_T gsdi_status - GSDI_SUCCESS
                             - GSDI_ERROR
===========================================================================*/
/*lint -save -esym(818,search_chars) suppress could be const */
gsdi_returns_T gsdi_sim_generic_search (
  gsdi_file_enum_type         sim_filename,
  gsdi_slot_id_type           slot,
  gsdi_index_modes_T          index_mode,
  int32                       search_record_num,
  gsdi_srch_mode              search_mode,
  gsdi_srch_dir_type          search_direction,
  char                      * search_chars,
  int32                       search_chars_len,
  uint32                      client_ref,
  gsdi_async_cb               async_cb
)
{
  gsdi_sim_search_req_T            msg;
  gsdi_returns_T                   gsdi_status = GSDI_ERROR;
  /* --------------------------------------------------------------------------
   PERFORM Parameter Checking on parameters passed in
   1. search_string cannot be null
   2. search_string_len cannot be greater than GSDI_MAX_SEARCH_STR_LEN or
      less than zero.
   3. async_cb cannot be null
   ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(search_chars);
  MMGSDI_RETURN_IF_OUT_OF_RANGE(search_chars_len,0,GSDI_MAX_SEARCH_STR_LEN);
  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
    PERFORM checking to see if the gsdi_lib_intialized is set to true.
    1. If not set to true, invoke gsdi_lib_init() and set the value to true.
  ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Frame gsdi_sim_search_req_T pkt to queue in gsdi queue
     -------------------------------------------------------------------------*/

  /* Message Header */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_sim_search_req_T),&msg);

  msg.message_header.message_set = MS_GSDI;
  msg.message_header.message_id  = GSDI_SIM_SEARCH_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
  msg.message_header.slot     = slot;

  /* Message Contents */
  msg.sim_filename       = (word) sim_filename;
  msg.index_mode         = index_mode;
  memcpy( msg.search_string, search_chars, (uint32)search_chars_len );
  msg.search_string_len  = (byte) search_chars_len;
  msg.queue_id           = GS_QUEUE_GSDI_LIB;
  msg.client_ref         = client_ref;
  msg.search_rec_num     = (uint8)search_record_num;
  msg.search_mode        = (uint8)search_mode;
  msg.search_direction   = (uint8)search_direction;
  msg.message_header.gsdi_async_callback = async_cb;

  /*-----------------------------------------------------------------------------
    Enqueue msg in the gsdi_queue
    ---------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&msg);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
  return gsdi_status;
} /* gsdi_sim_generic_search */
/*lint -save +esym(818,search_chars) suppress could be const */

/*===========================================================================
FUNCTION GSDI_GET_ECC

DESCRIPTION
  This function will populate the msg pointer with request for ECC and
  put it in the gsdi queue. .

INPUT
  gsdi_slot_id_type slot:           Slot to send the cmd to
                                    Must be GSDI_SLOT_1 or GSDI_SLOT_2
  uint32            client_ref:     User Data returned in response
  gsdi_async_cb     gsdi_asnyc_cb:  Callback GSDI Calls to provide gsdi_cnf_T

LIMITATIONS
  API is only supported in Asynchronous Mode.

  API only support slot ids of GSDI_SLOT_1 or GSDI_SLOT_2.  All other slot ids
  will be rejected with gsdi_returns_T or GSDI_INCORRECT_PARAMS

DEPENDENCIES
  None

OUTPUT

RETURN VALUE
  gsdi_returns_T gsdi_status - GSDI_SUCCESS
                             - GSDI_ERROR
===========================================================================*/
gsdi_returns_T gsdi_get_ecc (
    gsdi_slot_id_type slot,
    uint32            client_ref,
    gsdi_async_cb     gsdi_asnyc_cb
)
{
    gsdi_returns_T    gsdi_status = GSDI_ERROR;
    gsdi_get_ecc_req_T msg;

    /* --------------------------------------------------------------------------
     PERFORM Parameter Checking on parameters passed in
     1.  Slot ID:               Valid Values:  GSDI_SLOT_1 or GSDI_SLOT_2
     2.  client_ref:            Valid Values:  No Parameter Checking
     3.  aysnc_cb:              Valid Values:  Must not be NULL
     ------------------------------------------------------------------------*/
    MMGSDI_RETURN_IF_OUT_OF_RANGE(slot,GSDI_SLOT_1,GSDI_SLOT_2);
    MMGSDI_RETURN_IF_NULL(gsdi_asnyc_cb);

   /* --------------------------------------------------------------------------
     PERFORM checking to see if the gsdi_lib_intialized is set to true.
     1, If not set to true, invoke gsdi_lib_init() and set the value to true.
     ------------------------------------------------------------------------*/
    MMGSDI_INIT_LIB();

    /* --------------------------------------------------------------------------
       Frame gsdi_get_ecc_req_T pkt to queue in gsdi queue
       -------------------------------------------------------------------------*/

    /* Message Header */
    GSDI_PUT_MSG_LEN(sizeof(gsdi_get_ecc_req_T),&msg);
    msg.message_header.message_set = MS_GSDI;
    msg.message_header.message_id  = GSDI_GET_ECC_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    msg.message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
    msg.message_header.slot     = slot;

   /* Message Contents */
    msg.queue_id           = GS_QUEUE_GSDI_LIB;
    msg.client_ref         = client_ref;
    msg.message_header.gsdi_async_callback = gsdi_asnyc_cb;

    /*-----------------------------------------------------------------------------
      Enqueue msg in the gsdi_queue
      ---------------------------------------------------------------------------*/
    gsdi_status = gsdi_cmd_ext(&msg);
    if(gsdi_status != GSDI_SUCCESS)
    {
      gsdi_status = GSDI_ERROR;
    }
    return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_GET_PHONEBOOK_REFERENCE_TABLE

DESCRIPTION
  This function will generate a command to retrieve the Phonebook
  Reference Table from the GSDI Task.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
    Only supported in Asynchronous Mode.
===========================================================================*/
gsdi_returns_T gsdi_get_phonebook_reference_table(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     gsdi_asnyc_cb
)
{
  gsdi_get_pbr_table_req_T * msg_p = NULL;
  gsdi_returns_T             gsdi_status = GSDI_ERROR;

  /* --------------------------------------------------------------------------
   PERFORM Parameter Checking on parameters passed in
   1.  Slot ID:               Valid Values:  GSDI_SLOT_1 or GSDI_SLOT_2
   2.  client_ref:            Valid Values:  No Parameter Checking
   3.  aysnc_cb:              Valid Values:  Must not be NULL
   ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_OUT_OF_RANGE(slot,GSDI_SLOT_1,GSDI_SLOT_2);
  MMGSDI_RETURN_IF_NULL(gsdi_asnyc_cb);

 /* --------------------------------------------------------------------------
   PERFORM checking to see if the gsdi_lib_intialized is set to true.
   1, If not set to true, invoke gsdi_lib_init() and set the value to true.
   ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Allocate memory for this request.  The Macro will return out of this
     function if memory can not be allocated.
     ------------------------------------------------------------------------*/
  msg_p = mmgsdi_malloc(sizeof(gsdi_get_pbr_table_req_T));
  MMGSDI_RETURN_IF_NULL(msg_p);

  /* --------------------------------------------------------------------------
     Frame gsdi_get_pbr_ref_table_req_T Message Header
     -------------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_get_pbr_table_req_T),msg_p);
  msg_p->message_header.message_set         = MS_GSDI;
  msg_p->message_header.message_id          = GSDI_GET_PBR_TABLE_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg_p->message_header.tcb_ptr             = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg_p->message_header.wait_sig            = GSDI_LIB_CMD_Q_SIG;
  msg_p->message_header.slot                = slot;
  msg_p->message_header.gsdi_async_callback = gsdi_asnyc_cb;
  /* --------------------------------------------------------------------------
     Frame gsdi_get_pbr_ref_table_req_T pkt Message Contentse
     -------------------------------------------------------------------------*/
  msg_p->queue_id                           = GS_QUEUE_GSDI_LIB;
  msg_p->client_ref                         = client_ref;

  /*-----------------------------------------------------------------------------
    Enqueue msg in the gsdi_queue
    ---------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)msg_p);
  if(gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR("Could not Queue the Message",0,0,0);
  }

  /* ----------------------------------------------------------------------------
     Free Up all memory allocated in this request attempt
     --------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(msg_p);

  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI2_PHONEBOOK_PRESENT

DESCRIPTION
  This function will check whether or not the Phonebook in question is present
  or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
    Only supported in Asynchronous Mode.
    Function only accepts a slot of GSDI_SLOT_1 or GSDI_SLOT_2.
      All other slot types are invalid as the current subscription can not
      be used to determine which Phonebook on which Slot this information
      pertains to.
===========================================================================*/
boolean gsdi2_phonebook_present(
    gsdi_slot_id_type   slot,
    gsdi_phonebook_type pb
)
{
  boolean pb_present = FALSE;

  /* -----------------------------------------------------------------------
     Validate the parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_FALSE_IF_OUT_OF_RANGE(slot,GSDI_SLOT_1,GSDI_SLOT_2);

  switch ( pb )
  {
    case  GSDI_GLOBAL_PHONEBOOK_TYPE:
      /*
      ** This is the Global Phonebook found in the
      ** MF/TELECOM/PHONEBOOK Structure within the
      ** UICC/USIM inserted
      */
      if ( gsdi_data.sim_capability.usim_global_phone_book.present )
      {
        MSG_HIGH("GLOBAL PHONEBOOK PRESENT",0,0,0);
        pb_present = TRUE;
      }
      else
      {
        MSG_HIGH("GLOBAL PHONEBOOK NOT PRESENT",0,0,0);
      }
      break;

    case GSDI_LOCAL_PHONEBOOK_TYPE:
      /*
      ** This is the Local Phonebook found in the
      ** MF / USIM ADF/PHONEBOOK structure within the
      ** UICC/USIM Inserted
      */
      if ( gsdi_data.sim_capability.usim_local_phone_book.present )
      {
        MSG_HIGH("LOCAL PHONEBOOK PRESENT",0,0,0);
        pb_present = TRUE;
      }
      else
      {
        MSG_HIGH("LOCAL PHONEBOOK NOT PRESENT",0,0,0);
      }
      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR("PB TYPE NOT VALID",pb,0,0);
      break;
  }
  return pb_present;
} /*lint !e715 !e818 */
/**** ADDED LINT SUPRESSION DUING RPC CONVERGENCE FOR 7xxx COMMON RPC API */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

/*===========================================================================
FUNCTION GSDI_SECURE_DEVICE_INFO

DESCRIPTION
  This function will write IMEI, Serial Number to the secured file
  system

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
    Only supported in Asynchronous Mode.
    Function only accepts a slot of GSDI_SLOT_1 or GSDI_SLOT_2.
      All other slot types are invalid as the current subscription can not
      be used to determine which Phonebook on which Slot this information
      pertains to.
===========================================================================*/
gsdi_returns_T gsdi_secure_device_info(
  uint32            client_ref,
  gsdi_async_cb     gsdi_asnyc_cb
)
{
  gsdi_returns_T                gsdi_status = GSDI_ERROR;
  gsdi_secure_device_info_req_T msg;

  /* --------------------------------------------------------------------------
    PERFORM Parameter Checking on parameters passed in
    1.  client_ref:            Valid Values:  No Parameter Checking
    2.  aysnc_cb:              Valid Values:  Must not be NULL
    ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(gsdi_asnyc_cb);

  /* --------------------------------------------------------------------------
    PERFORM checking to see if the gsdi_lib_intialized is set to true.
    1, If not set to true, invoke gsdi_lib_init() and set the value to true.
    ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
    Frame gsdi_get_ecc_req_T pkt to queue in gsdi queue
    -------------------------------------------------------------------------*/

  /* Message Header */
  GSDI_PUT_MSG_LEN(sizeof(gsdi_secure_device_info_req_T),&msg);
  msg.message_header.message_set = MS_GSDI;
  msg.message_header.message_id  = GSDI_SECURE_DEVICE_INFO_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  msg.message_header.tcb_ptr  = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  msg.message_header.wait_sig = GSDI_LIB_CMD_Q_SIG;
  msg.message_header.slot     = GSDI_SLOT_NONE;

  /* Message Contents */
  msg.queue_id           = GS_QUEUE_GSDI_LIB;
  msg.client_ref         = client_ref;
  msg.message_header.gsdi_async_callback = gsdi_asnyc_cb;

  /*-----------------------------------------------------------------------------
    Enqueue msg in the gsdi_queue
    ---------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext(&msg);
  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }
  return gsdi_status;

}


/*===========================================================================
FUNCTION GSDI_PERSO_GET_PHONECODE_STATUS

DESCRIPTION
  This function will unblock the Phonecode.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
  Only supported in Asynchronous Mode.
===========================================================================*/
gsdi_returns_T gsdi_perso_get_phonecode_status(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     gsdi_asnyc_cb
)
{
  gsdi_perso_get_phonecode_status_req_T * req_p       = NULL;
  gsdi_returns_T                         gsdi_status = GSDI_ERROR;
  /* --------------------------------------------------------------------------
     Validate the parameters provided.
     Return GSDI_INCORRECT_PARAMS if not valid
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(gsdi_asnyc_cb);

  /* --------------------------------------------------------------------------
    PERFORM checking to see if the gsdi_lib_intialized is set to true.
    1, If not set to true, invoke gsdi_lib_init() and set the value to true.
    ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Allocate memory for the following:
     1.  Request
     ------------------------------------------------------------------------*/
  req_p = mmgsdi_malloc(sizeof(gsdi_perso_get_phonecode_status_req_T));
  MMGSDI_RETURN_IF_NULL(req_p);

  /* --------------------------------------------------------------------------
     Start populating the Command Header Contents
     ------------------------------------------------------------------------*/
  GSDI_PUT_MSG_HDR_TEMPLATE(GSDI_PERSO_GET_PHONE_CODE_STATUS_REQ,slot,gsdi_asnyc_cb,req_p);
  GSDI_PUT_MSG_LEN(sizeof(gsdi_perso_get_phonecode_status_req_T),req_p);

  /* --------------------------------------------------------------------------
     Populate the remainder of the Message
     ------------------------------------------------------------------------*/
  req_p->client_ref     = client_ref;
  req_p->queue_id       = GS_QUEUE_GSDI_LIB;

  /* --------------------------------------------------------------------------
     Queue the command to the GSDI Command Queue
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)req_p);

  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Free up the memory allocated
     1.  Request
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(req_p);

  return gsdi_status;

} /* gsdi_perso_get_phonecode_status */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/*===========================================================================
FUNCTION GSDI_ACTIVATE_ONCHIP_SIM

DESCRIPTION
  This function will allow the MMGSDI Task to operate even without a SIM/USIM
  card inserted.  To be used only for Factory Test Mode Purposes.  Not be
  be allowed / enabled when by a user.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
  Only supported in Asynchronous Mode.
===========================================================================*/
gsdi_returns_T gsdi_activate_onchip_sim(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     gsdi_asnyc_cb
)
{
  gsdi_activate_onchip_sim_req_T * req_p       = NULL;
  gsdi_returns_T                   gsdi_status = GSDI_ERROR;
  /* --------------------------------------------------------------------------
     Validate the parameters provided.
     Return GSDI_INCORRECT_PARAMS if not valid
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(gsdi_asnyc_cb);

  /* --------------------------------------------------------------------------
    PERFORM checking to see if the gsdi_lib_intialized is set to true.
    1, If not set to true, invoke gsdi_lib_init() and set the value to true.
    ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Allocate memory for the following:
     1.  Request
     ------------------------------------------------------------------------*/
  req_p = mmgsdi_malloc(sizeof(gsdi_activate_onchip_sim_req_T));
  MMGSDI_RETURN_IF_NULL(req_p);

  /* --------------------------------------------------------------------------
     Frame gsdi_activate_onchip_sim_req_T Message Header
     -------------------------------------------------------------------------*/
  GSDI_PUT_MSG_LEN(sizeof(gsdi_activate_onchip_sim_req_T),req_p);
  req_p->message_header.message_set         = MS_GSDI;
  req_p->message_header.message_id          = GSDI_ACTIVATE_ONCHIP_SIM_REQ;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
  req_p->message_header.tcb_ptr             = rex_self();
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
  req_p->message_header.wait_sig            = GSDI_LIB_CMD_Q_SIG;
  req_p->message_header.slot                = slot;
  req_p->message_header.gsdi_async_callback = gsdi_asnyc_cb;

  /* --------------------------------------------------------------------------
     Populate the remainder of the Message
     ------------------------------------------------------------------------*/
  req_p->client_ref     = client_ref;
  req_p->queue_id       = GS_QUEUE_GSDI_LIB;

  /* --------------------------------------------------------------------------
     Queue the command to the GSDI Command Queue
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)req_p);

  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Free up the memory allocated
     1.  Request
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(req_p);

  return gsdi_status;
} /* gsdi_activate_onchip_sim */

/*===========================================================================
FUNCTION GSDI_GET_ALL_PIN_STATUS_INFO

DESCRIPTION
  This function will return all PIN Status Information for all PINs currently
  in the handset.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
  Only supported in Asynchronous Mode.
===========================================================================*/
gsdi_returns_T gsdi_get_all_pin_status_info(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
)
{

  gsdi_get_all_pin_info_req_T * req_p       = NULL;
  gsdi_returns_T                gsdi_status = GSDI_ERROR;
  /* --------------------------------------------------------------------------
     Validate the parameters provided.
     Return GSDI_INCORRECT_PARAMS if not valid
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(async_cb);

  /* --------------------------------------------------------------------------
    PERFORM checking to see if the gsdi_lib_intialized is set to true.
    1, If not set to true, invoke gsdi_lib_init() and set the value to true.
    ------------------------------------------------------------------------*/
  MMGSDI_INIT_LIB();

  /* --------------------------------------------------------------------------
     Allocate memory for the following:
     1.  Request
     ------------------------------------------------------------------------*/
  req_p = mmgsdi_malloc(sizeof(gsdi_get_all_pin_info_req_T));
  MMGSDI_RETURN_IF_NULL(req_p);

  /* --------------------------------------------------------------------------
     Start populating the Command Header Contents
     ------------------------------------------------------------------------*/
  GSDI_PUT_MSG_HDR_TEMPLATE(GSDI_GET_ALL_PIN_INFO_REQ,slot,async_cb,req_p);
  GSDI_PUT_MSG_LEN(sizeof(gsdi_get_all_pin_info_req_T),req_p);

  /* --------------------------------------------------------------------------
     Populate the remainder of the Message
     ------------------------------------------------------------------------*/
  req_p->client_ref     = client_ref;
  req_p->queue_id       = GS_QUEUE_GSDI_LIB;

#ifndef FEATURE_MMGSDI_DUAL_SLOT
  req_p->message_header.slot    = GSDI_SLOT_1;
#else
  req_p->message_header.slot    = slot;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  /* --------------------------------------------------------------------------
     Queue the command to the GSDI Command Queue
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_cmd_ext((void *)req_p);

  if(gsdi_status != GSDI_SUCCESS)
  {
    gsdi_status = GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Free up the memory allocated
     1.  Request
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(req_p);

  return gsdi_status;

}


/*===========================================================================
FUNCTION GSDI_CONVERT_DETAILED_ERR_TO_GEN_ERROR

DESCRIPTION
  This function will convert the detailed card errors resulting from
  FEATURE_MMGSDI_CARD_ERROR_INFO to a generic GSDI_CARD_ERROR for clients
  that don't need to handle the detailed errors.

DEPENDENCIES
  FEATURE_MMGSDI_CARD_ERROR_INFO:

    When the feature is not defined, this function returns the
    gsdi_returns_T  enum provided in the input as the output.

RETURN VALUE
  void

SIDE EFFECTS
  None

LIMITATIONS
  A bad input results as the same bad output.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gsdi_convert_detailed_err_evt_to_gen_err(
  gsdi_sim_events_T   in_err,
  gsdi_sim_events_T * out_err
)
{
  if ( out_err == NULL )
  {
    return;
  }

  switch ( in_err )
  {
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
      *out_err = GSDI_CARD_ERROR;
      break;

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
      *out_err = GSDI_CARD_ERROR_2;
      break;

    default:
      *out_err = in_err;
      break;
  }

}/* gsdi_convert_detailed_err_evt_to_gen_err */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION GSDI_PERSO_GET_FEATURE_INDICATOR_KEY

DESCRIPTION
  This API is used to get the feature Indicator key.
  The feature key will be sent back in the callback function and the data
  will be available in gsdi_perso_get_feature_key_cnf_T

  Parameters
  gsdi_slot_id_type slot - slot information
  gsdi_perso_enum_type feature_ind - feature indicator
  boolean is_unblock_key - if retriving unblock key then set to TURUE
  uint32 client_ref - client data,
  gsdi_async_cb async_cb - client call back function

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None

LIMITATIONS
  None
===========================================================================*/
gsdi_returns_T gsdi_perso_get_feature_indicator_key (
    gsdi_slot_id_type    slot,
    gsdi_perso_enum_type feature_ind,
    boolean              is_unblock_key,
    uint32               client_ref,
    gsdi_async_cb        async_cb
)
{
  gsdi_returns_T  gsdi_status = GSDI_NOT_SUPPORTED;

#ifdef FEATURE_MMGSDI_PERSO_GET_KEY
  /* Has the GSDI lib been initialized yet ? */
  MMGSDI_INIT_LIB();

  gsdi_status = gsdi_perso_get_key(slot,
                                   feature_ind,
                                   is_unblock_key,
                                   client_ref,
                                   async_cb);
#else
  (void)slot;
  (void)feature_ind;
  (void)is_unblock_key;
  (void)client_ref;
  (void)async_cb;
#endif /* FEATURE_MMGSDI_PERSO_GET_KEY */

  return gsdi_status;
} /* gsdi_perso_get_feature_indicator_key() */


/*===========================================================================
FUNCTION GSDI2_GET_CARD_MODE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_return_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi2_get_card_mode(
    gsdi_slot_id_type             slot,
    gsdi_card_mode_capability_T * card_mode
)
{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    /*lint -esym(613, card_mode)*/
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    switch ( slot )
    {
        case GSDI_SLOT_1:
            if ( ( gsdi_data.card_applications.operational_slot &
                    GSDI_GSM_SLOT_1_MASK ) ==
                    GSDI_GSM_SLOT_1_MASK )
            {
                *card_mode = GSDI_CARD_GSM;
            }
            else if ( ( gsdi_data.card_applications.operational_slot &
                        GSDI_CDMA_SLOT_1_MASK ) ==
                        GSDI_CDMA_SLOT_1_MASK )
            {
                *card_mode = GSDI_CARD_RUIM;
            }
            else
            {
                *card_mode = GSDI_CARD_MODE_UNKNOWN;
                gsdi_status = GSDI_ERROR;
            }
            break;

        case GSDI_SLOT_2:
            if ( ( gsdi_data.card_applications.operational_slot &
                    GSDI_GSM_SLOT_2_MASK ) ==
                    GSDI_GSM_SLOT_2_MASK )
            {
                *card_mode = GSDI_CARD_GSM;
            }
            else if ( ( gsdi_data.card_applications.operational_slot &
                        GSDI_CDMA_SLOT_2_MASK ) ==
                        GSDI_CDMA_SLOT_2_MASK )
            {
                *card_mode = GSDI_CARD_RUIM;
            }
            else
            {
                *card_mode = GSDI_CARD_MODE_UNKNOWN;
                gsdi_status = GSDI_ERROR;
            }
            break;

        default:
            MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID %x",slot,0,0);
            break;
    }
    return gsdi_status;
    /*lint +esym(613, card_mode)*/
#else
    if(card_mode == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("NULL PTR FOR SLOT ID %x",slot,0,0);
      return GSDI_ERROR;
    }
    return GSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

} /* gsdi2_get_card_mode() */


/*===========================================================================
FUNCTION GSDI_GET_CARD_MODE

DESCRIPTION
  This function returns the type of SIM card that is currently in use.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_get_card_mode (
  gsdi_card_mode_capability_T * card_mode
)
{
    gsdi_returns_T                  gsdi_status = GSDI_SUCCESS;

#ifndef FEATURE_MMGSDI_DUAL_SLOT

    if (gsdi_data.current_card_mode_capability == GSDI_CARD_MODE_UNKNOWN)
    {
        gsdi_status = GSDI_NOT_INIT;
        ERR("Card Mode Unknown",0,0,0);
    }
    else
    {
        /*lint -esym(613,card_mode)*/
        *card_mode = gsdi_data.current_card_mode_capability;
        gsdi_status = GSDI_SUCCESS;
    }

    /* Temporary solution                      */
    /* Check for GSM Operational Slot          */
    /* if not found...return CDMA as Card Type */
    /* if operational slot is available        */
    /* Priority is given to GSM                */
    /* Modifications will be made to keep this */
    /* tailored to SLOT 1 only once Stack      */
#else
    if ( ( gsdi_data.card_applications.operational_slot &
            GSDI_GSM_SLOT_1_MASK ) ==
            GSDI_GSM_SLOT_1_MASK )
    {
        /* Need to verify that GSM APP is available before */
        /* just blindly setting it                         */
        if ( ( gsdi_data.card_applications.apps_enabled &
               GSDI_GSM_SIM_MASK ) ==
               GSDI_GSM_SIM_MASK )
        {
          gsdi_data.current_card_mode_capability = GSDI_CARD_GSM;
          *card_mode = GSDI_CARD_GSM;
          gsdi_status = GSDI_SUCCESS;
        }
    }
    /* Remove once stack is updated for permanent solution */
    else if ( ( gsdi_data.card_applications.operational_slot &
                GSDI_GSM_SLOT_2_MASK ) ==
                GSDI_GSM_SLOT_2_MASK )
    {
        /* Need to verify that GSM APP is available before */
        /* just blindly setting it                         */
        if ( ( gsdi_data.card_applications.apps2_enabled &
               GSDI_GSM_SIM_MASK ) ==
               GSDI_GSM_SIM_MASK )
        {
          gsdi_data.current_card_mode_capability_2 = GSDI_CARD_GSM;
          *card_mode = GSDI_CARD_GSM;
          gsdi_status = GSDI_SUCCESS;
        }
    }
    else if ( ( gsdi_data.card_applications.operational_slot &
                GSDI_CDMA_SLOT_1_MASK ) ==
                GSDI_CDMA_SLOT_1_MASK )
    {
        gsdi_data.current_card_mode_capability = GSDI_CARD_RUIM;
        *card_mode = GSDI_CARD_RUIM;
        gsdi_status = GSDI_SUCCESS;
    }
    /* Remove once stack is updated for permanent solution */
    else if ( ( gsdi_data.card_applications.operational_slot &
                GSDI_CDMA_SLOT_2_MASK ) ==
                GSDI_CDMA_SLOT_2_MASK )
    {
        gsdi_data.current_card_mode_capability_2 = GSDI_CARD_RUIM;
        *card_mode = GSDI_CARD_RUIM;
        gsdi_status = GSDI_SUCCESS;
    }
    else
    {
        *card_mode = GSDI_CARD_MODE_UNKNOWN;
        gsdi_status = GSDI_ERROR;
    }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    return gsdi_status;
} /* gsdi_get_card_mode() */
