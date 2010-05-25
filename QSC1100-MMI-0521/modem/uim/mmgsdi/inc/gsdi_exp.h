#ifndef GSDI_EXPORT_H
#define GSDI_EXPORT_H
/*===========================================================================


            G S D I   E X T E R N A L   D E F I N I T I O N S

                      A N D   F U N C T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2009 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/inc/gsdi_exp.h#3 $$ $DateTime: 2009/03/03 06:16:30 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/03/09   mib     Changed return value of gsdi_sim_get_cphs_information()
                   when CPHS feature is disabled
01/22/09   sun     Removing GSDI Refresh timer sig
12/05/08   nb      Fixed Compilation warning related to GSDI_READ_TIMER_VALUE.
11/26/08   nb      Removed Featurization Flags for Common Modem Interface.
10/20/08   sun     Added new SW2 value
09/25/08   kk      Added support for extracting unblock dck retries available
08/28/08   sun     If command fails in the original state, then return original 
                   error mmgsdi status to client
08/14/08   js      Changed sim file path length gsdi_select_req_T to 4.
07/30/08   tml     Added meta data comment for select
07/25/08   tml     Added modem restart oncrpc support
05/01/08   sun     Fixed warning
04/30/08   kk      Added API for get_feature_indicator_key under feature
                   definition FEATURE_MMGSDI_PERSO_GET_KEY
04/30/08   kk      Added support for MMS EFs
04/21/08   kk      Added meta comments for get feature data perso definitions
04/16/08   kk      Added support of get command on sim lock data
03/20/08   nk      Added OMH File Support
01/16/08   kk      Fixed "missing header file ms.h in 1X builds" - featurized
                   the header file inclusion.
12/18/07   kk      Fixed compilation warnings, redefinition of MS_GSDI and
                   MS_TIMER - Included ms.h
11/19/07   js      Radio Access Technology Balancing support
11/29/07   sk      Added prototype for gsdi_get_protocol()
11/08/07   vs/nk   Added EUIMID and OMH feature support
09/10/07   tml     Added new SW2 values for MFLO
08/14/07   sun     Added new Error Code
08/10/07   nf      Added GPLT support
07/20/07   sun     Added new Status Word
05/23/07   sun     Added metacomments
05/16/07   tml     Added perso completed event
05/08/07   nk      Removed featurization around gsdi usim authenticate
05/07/07   tml     Increase GSDI_MAX_FILE_CHANGE_NOTIFICATIONS for 1000 PBM
05/02/07   sun     Added new error code
05/01/07   nk      Added metacomments
04/12/07   sun     Featurized Phonebook
04/09/07   nk      Updated GSDI_MIN_VALID_IMSI_LEN value
03/26/07   sun     Added new status for additional files read during refresh
02/09/07   sk      Added support for 1000 phonebook entries.
01/18/07   sun     Fixed services type for CBMI
12/15/06   sk      Increased value for GSDI_MAX_FILE_CHANGE_NOTIFICATIONs to
                   accomodate both local and global phonebook registrations by
                   PBM at the same time.
12/13/06   tml     Added invalid AID and USIM AID Not active return error codes
                   for refresh
09/11/06   jk      Changed featurization so that 7200 and 7500 generate identical
                   RPC Files
08/25/06   jar     Added Support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/20/06   tml     Added App Reset and 3G Session Reset supports
07/26/06   sun     Added new rsp for INTERNAL Rsp
06/13/06   tml     RPC fix
06/12/06   pv      Add GSDI_INTERNAL_SIM_RESET event to be sent to Gstk for
                   doing a terminal profile download while recovery.
05/10/06   tml     Add Refresh mode none enum and compilation fix
04/21/06   tml     RPC compilation fix
04/19/06   tml     Fixed RPC issue for FDN functions
04/06/06   sun     Added support for RUIM Lock
03/21/06   tml     added cfis and missing service table support and lint
03/11/06   tml     Fixed get all pin info req type
03/08/06   jar     merged in branched sim lock
02/21/06   sp      Updating gsdi_services_type for EHPLMN
02/16/06   sp      Support for EF_EHPLMN
02/14/06   nk      Merge for ARR caching
11/22/05   jk      Changes to support multiprocessor RPC
10/20/05   sun     Added support to cache file attributes
10/03/05   sun     Added additional enums for Perso Status
09/14/05   wli     Fixed compilation error for feature
                   FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN
08/29/05   tml     Fixed compilation warning
08/04/05   jar     Added Prop 2 Event to be consistent with ICARD dependencies
08/02/05   wli     Added HOT SWAP feature.
06/17/05   sun     Fixed GSDI Enums
06/14/05   jar     Fixed phonebook present logic error
06/10/05   tml     Bring in 256 MMGSDI support
06/06/05   tml     Merged in SIM Lock API changes
06/03/05   pv      Added illegal sim events
05/11/05   sst     Lint fixes
05/03/05   jar     Fixed Compile issue.
05/03/05   jar     Added the GSDI_PERSO_INIT_COMPLETED Event
04/28/05   jar     Added New GSDI SIM Lock Architecutre to support Plug In
                   Proprietary Security Libraries driven by a Perso Engine.
03/16/05   wli     Added 820B EFs
02/28/05   sun     Added file - EF MWIS
02/22/05   pv      Added GSDI_UIM_RST_INSTN_CHG_SUCCESS enum item to
                   identify a reset with instruction class change status
02/08/05   sun     Added support for Cingular
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/21/05   sun     Rearraranged gsdi_services_type based on EF-SST
01/12/05   jk      Fixed some compilation issues when FEATURE_GSTK is off.
12/27/04   jar     Added support for GSDI_GET_PBR_TABLE_REQ
                   Merged in Branched Channel Management Fixes
11/05/04   jar     Merged in FEATURE_MMGSDI_DUAL Commercial Code for next
                   mainlined commercial release.
11/01/04   pv      Added work_waiting time field to gsdi_uim_link_established
                   structure
10/18/04   sk      Added a new macro for GSDI_GET_ECC_REQ.
                   Added a new enum for GSDI_GET_ECC_RSP.
                   Added structures to support ECC table.
                   Added structures for ecc_cnf and gsdi_get_ecc_req_T.
                   Added prototype for function gsdi_get_ecc.
10/18/04   sk      Added two new structures for gsdi search mode types
                   and gsdi search direction types.
                   Added prototype for function gsdi_sim_generic_search().
09/29/04   sk      Added a boolean field is_pin_available to structure
                   gsdi_pin_info_T. Added prototype for
                   gsdi_util_get_pin_function.
09/24/04   jar     Addition of CPHS Procedures
09/23/04   jar     Increased max data buffer size
09/14/04   jar     Fixed Compilation issue with CPHS Enums.
09/07/04   jar     Modified gsdi_send_toolkit_apdu_req_T.
09/04/04   jar     Added API GSDI_GET_CPHS_INFORMATION & ATCSIM Support
09/04/04   tml     Added CPHS ALS Support and file enum
08/11/04   tml     Added support for get unblock key
08/02/04   jar     Changed MAX IMEI Len to 15
07/13/04 jar/tml   Changed number of CK code to 8
06/20/04 jar/tml   Added Personalization Internal Init state to be
                   used during self personalization on power up.  Added
                   additional personalization cmds.
06/15/04   tml     Added protocol field in link established message
06/08/04   jar     Added FEATURE_MMGSDI_PERSONALIZATION
04/28/04   jar     Added Channel Management Support
04/27/04   tml     Fixed compilation error
04/22/04   jar     Modified gsdi_get_ef_length() to support 256+ bytes.
04/18/04   jar     Updated index_mode in search_req for Lint Fix.
04/07/04   jar     Fixed Compiler Warning, added gsdi_nas_get_card_mode()
03/31/04   tml     Added number record enum for file attribute
03/27/04   tml     Merged dog petting for synchronous call from branch
03/22/04   jar     Merged in events from branch.
03/16/04   jar     Added GSDI_USIM_APP in gsdi_apps_enum_type.
01/28/04   jar     Added Function gsdi_cm_in_traffic_channel_w_mode to support
                   Polling on/off for 1X RUIM.
01/26/04   jar     Appended invalidated_deactivated_read_write_allowed to the
                   gsdi_file_attributes_T structure.
01/24/04   tml     Refresh dual slot support
01/16/04   tml     Added notify client request type
11/05/03   jar     Added Dual Slot Pref Change API and support.
10/20/03   jar     Added GSDI-DIAG Support
09/04/03   tml     Dual slot TK support
07/20/03   jar     FEATURE_MMGSDI_DUAL_SLOT Support
08/18/03   tml     Added required support for coreapp
05/29/03   tml     Linted
05/09/03   jar     Fixed compile error
05/01/03   jar     Added REFRESH FCN Modes Support
04/16/03   tml     Added image support
04/10/03   jar     Added support for GSM DCS1800 DF
03/21/03   tml     Added boolean parameter status in gsdi_util_gstk_tp_dl_
                   complete()
03/19/03   jar     Featurized gsdi_sim_get_ecc, gsdi_usim_get_ecc
                   Added gsdi_ruim_get_ecc.
                   Featurized ECC Buffers.  Added ruim_ecc_data buffer.
03/14/03   jar     Fixed Incompatible Data Type Issue.
03/13/03   jar     Added gsdi_get_apps_available Function Definition
03/06/03   jar     Fixed 6100 Feature Definition issues.
02/28/03   jar     Fixed double definition of gsdi_refresh_modes_enum_type.
02/27/03   jar     Added Masks for Tasks that receive FCNs.  Added messages
                   GSDI_SIM_INIT_REQ and GSDI_SIM_REFRESH_REQ.  Added
                   gstk_module_ef_type.  Added GSDI_SIM_REFRESH_RSP.  Added
                   Status for GSDI_REFRESH_SUCCESS, GSDI_CAN_NOT_REFRESH,
                   GSDI_PATHS_DECODE_ERROR.  Added gsdi_refresh_module_enum_type.
                   Added gsdi_refresh_modes_enum_type.  Added gsdi_refresh_cnf_T.
                   Modified gsdi_init_req_T.  Added gsdi_sim_refresh_req_T.
                   Added gsdi_sim_refresh() Library Function
02/24/03   jar     Added GSDI Refresh Modes
02/04/03   jar     Added GSTK Support for REFRESH API.
01/24/03   jar     Added GSDI_CDMA_ECC_LEN  and GSDI_LANG_PREF_LEN
01/16/03   jar     Added #defines GSDI_LP_LEN and GSDI_LI_LEN
01/02/03   jar     Lint Error Fixes. LTK RUIM Support Merge
11/26/02   jar     Added Card App Masks. Removed boolean init from
                   gsdi_card_apps_T.
11/18/02   jar     Removed phone book status unknown booleans.  They are unused.
11/13/02   djm     Pull in definition section when FEATURE_WCDMA_PLT is defined
                   as the definitions are required by some code not completely
                   wrapped by the feature defines.
11/06/02   jar     Added function gsdi_util_is_gsdi_cache_ready.
10/23/02   jar     Fixed merge error for gsdi_enable_fdn and gsdi_disable_fdn.
10/16/02   tml     Added GSDI_PIN1_PERM_BLOCKED, GSDI_PIN2_PERM_BLOCKED and
                   GSDI_CODE_PERM_BLOCKED
10/16/02   jar     Added function prototypes for gsdi_enable_fdn and
                   gsdi_disable_fdn.
10/14/02   tml     1x, gsm merge
09/31/02   jar     Added a function gsdi_cm_in_traffic_channel, for CM to call
                   when the mobile is in Traffic Channel.  This will replace
                   the current implementation of using Call backs and Events
                   from CM.
09/06/02   tml     Moved GSDI_MSISDN_NONAPLHA to outside feature defined
                   for compilation
08/30/02   tml     Removed gs_status_T and replace with gsdi_returns_T
08/26/02   cd      Changed definition of GSDI_KCGPRS_LEN from 9 to 8
08/21/02   jar     Made gsdi_cnf_T a union.  Added gsdi_rsphdr_T to all command
                   cnf types.  Added gsdi_store_esn_req_T.  Added additional
                   featurization.
08/16/02   tml     Changed byte client_ref to uint32.
08/12/02   tml     Added number of retries left for pin related api
08/11/02   jar     Added function gsdi_get_ef_length to return the length of
                   a cached EF.  Optimization to avoid a get_file_attributes
                   request for MM and REG.
07/24/02   tml     Added Select api
07/23/02   jar     Added a Function Prototype to Handle Async SIM/USIM Resp's
                   for Asych Write Access used to update the SIM/USIM to the
                   updated cached item (gsdi_handle_async_lib_rsp).
07/17/02   jar     Added a Function prototype to Handle Async SIM/USIM Resp's
                   for Asynch Write Access used to Synch the SIM/USIM to
                   cache (gsdi_handle_async_lib_rsp)
07/15/02   jar     Added Lengths for Items added to SIM and USIM Cache.
06/07/02   jar     Removed double definition of GSDI_MAX_REGISTERED_TASKS.
                   Fixed datatypes gsdi_exp_status_return_type, and gsdi_ind_
                   of_app_status_type.
05/27/02   jar     Added to the gsdi_returns_T Datatype.
05/21/02   jar     Added the gsdi_usim_status_req_T, gsdi_exp_status_return_
                   type, and gsdi_ind_of_app_status_type.  Added
                   gsdi_search_offset_type_T and gsdi_search_modes_T.  Added
                   additional gsdi_returns_T Enums.
05/19/02   jar     Removed FEATURE_GSDI_MULTICLIENT_SUPPORT dependencies from
                   GSDI Events.
05/10/02   jar     Changed GSDI_MAX_ECC_LEN from 255 to 50.  Changed
                   GSDI_MAX_ECC_RECORDS from 7 to 5.
04/30/02   jar     Removed FEATURE_GSDI_PHONEBOOK_SUPPORT needed for GSM
04/17/02   jar     Added the gsdi_sim_increase function prototype
03/23/02   jar     Added GSDI_SIM_TECHNICAL_PROBLEMS to gsdi_returns_T.
03/20/02   jar     Increased Message Size to avoid truncating data:
                   Changed GSDI_MAX_MSG_LEN from 300 to 350.  Merged from
                   branch 1.17.1.0.
03/11/02   jar     Added #defines for SIM Statuses.  Added PIN Events.
                   Added gsdi_register_callback_req_T.  Added the function
                   prototype gsdi_register_callback_function for the API.  All
                   changes need feature define FEATURE_GSDI_MULTICLIENT_SUPPORT
                   to take effect.
02/27/02   jar     Added the following #defines for IMSI Checking:
                   GSDI_MIN_VALID_IMSI_LEN, GSDI_MAX_VALID_IMSI_LEN,
                   and GSDI_IMSI_NOT_PROVISIONED
02/10/02   jar     Placed gsdi_cmd in this header from gsdi.h
02/09/02   jar     Increased ECC Length to 255 due to USIM ECC Requirements.
                   Changed GSDI_PBR_LEN to GSDI_MAX_PBR_LEN.
                   Expanded ECC Data Structure to support storage of 7
                   Uncoded ECCs.
02/03/02   jar     Add functionality to allow requests for ECC data retrieved
                   during GSDI Initialization.  Added Phonebook Support
                   Functionality which is featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.
10/19/01   jc      Add type defs for allowing client to include a callback
                   function pointer.
10/11/01   jc      Update message header w/ info on gsdi library task users.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "uim.h"
#include "dog.h"

#if (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))
#error code not present
#endif /* (defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)) */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Internal MACROs for determing the Lenght of the Message */
#define GSDI_GET_IMH_LEN(len,IMHP) \
        len = (((IMH_T *)(void*)IMHP)->message_len_lsb \
    + (0x100 * ((IMH_T *)(void*)IMHP)->message_len_msb));

#define GSDI_PUT_IMH_LEN(len,IMHP) \
        ((IMH_T *)(void*)IMHP)->message_len_msb = (unsigned char)((len) / 0x100), \
        ((IMH_T *)(void*)IMHP)->message_len_lsb = (unsigned char)((len) % 0x100);

#define GSDI_MAX_ATR_BYTES                       80

/* Max Length Allowed for IMEI */
#define GSDI_PERSO_MAX_IMEI_LEN                           0x0F

#define GSDI_PERSO_MAX_KEY_IMEI_LEN     (GSDI_PERSO_MAX_IMEI_LEN+1+GSDI_PERSO_MAX_CK)

/* --------------------------------------------------------------------------
   MAX NUMBER OF REGISTERED TASKS THAT
   CAN RECEIVE PERSONALIZATION EVENTS
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_NUM_REG_TASKS      0x000A

/* --------------------------------------------------------------------------
   MAX NUMBER OF BYTES ALLOWED FOR THE OTA DEPERSONALIZATION KEYS
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_OTA_DEPERSO_BYTES  0x64

/* --------------------------------------------------------------------------
   DATA / BUFFER LENGTHS
   Max number of bytes allowed in a the data buffer
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_DATA_LEN           0x0100

/* --------------------------------------------------------------------------
   DATA / BUFFER LENGTHS
   Max number of retries_allowed
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_CK_RETRIES           0x10

/* --------------------------------------------------------------------------
   DATA / BUFFER LENGTHS
   Max number of bytes allowed in the Control Key.  As
   currently spec'ed out, there shall be a max of 8 characters
   allowed per control key.  Since they are NULL Terminated, the
   buffer is 8 Chars + 1 Null Char.
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_CK                 0x0008

/* Number of SIM lock features */
#ifdef FEATURE_PERSO_SIM
#define NUM_OF_PERSO_SIM_FEATURES   5
#else
#define NUM_OF_PERSO_SIM_FEATURES   0
#endif
#ifdef FEATURE_PERSO_RUIM
#define NUM_OF_PERSO_RUIM_FEATURES  6
#else
#define NUM_OF_PERSO_RUIM_FEATURES  0
#endif /* FEATURE_PERSO_RUIM */

#define GSDI_REFRESH_MAX_EFS            15
#define GSDI_FCN_OUTSTANDING_GSDI       0x0001
#define GSDI_FCN_OUTSTANDING_CALL_STACK 0x0002
#define GSDI_FCN_OUTSTANDING_PB         0x0004
#define GSDI_FCN_OUTSTANDING_CORE       0x0008
#define GSDI_FCN_OUTSTANDING_SK_UI      0x0010
#define GSDI_FCN_OUTSTANDING_WMS        0x0020
#define GSDI_FCN_OUTSTANDING_CM         0x0040

#define GSDI_STANDARD_READ_LEN        0
#define GSDI_ICC_IC_LEN              10
#define GSDI_FILE_ID_LEN              2
#define GSDI_KC_LEN                   8
#define GSDI_RAND_LEN                 16
#define GSDI_LP_LEN                   10
#define GSDI_LI_LEN                   20
/*temporary */
#define GSDI_MSISDN_NONALPHA_LEN     14  /* excl. the alpha part */


#define GSDI_IMSI_LEN                 9
#define GSDI_MIN_VALID_IMSI_LEN       3
#define GSDI_MAX_VALID_IMSI_LEN       9
#define GSDI_IMSI_NOT_PROVISIONED  0xFF
#define GSDI_HPLMN_SP_LEN             1
#define GSDI_ACM_MAX_LEN              3
#define GSDI_ACM_LEN                  3
#define GSDI_SPN_LEN                 17
#define GSDI_PUCT_LEN                 5
#define GSDI_BCCH_LEN                16
#define GSDI_ACC_LEN                  2
#define GSDI_FPLMN_LEN               12
#define GSDI_EHPLMN_LEN             255
#define GSDI_LOCI_LEN                11
#define GSDI_PHASE_LEN                1
#define GSDI_ADN_NONALPHA_LEN        14  /* excl. the alpha part */
#define GSDI_FDN_NONALPHA_LEN        14  /* excl. the alpha part */
#define GSDI_SMSP_NONALPHA_LEN       28  /* excl. the alpha part */
#define GSDI_SMSP_LEN               255
#define GSDI_SMS_LEN                176
#define GSDI_SMSS_LEN                 2
#define GSDI_SMSR_LEN                30
#define GSDI_CCP_LEN                 14
//#define GSDI_MSISDN_NONALPHA_LEN     14  /* excl. the alpha part */
#define GSDI_LND_NONALPHA_LEN        14  /* excl. the alpha part */
#define GSDI_EXT1_LEN                13
#define GSDI_EXT2_LEN                13
#define GSDI_EMLPP_LEN               2
#define GSDI_AAEM_LEN                1
#define GSDI_START_HFN_LEN           6
#define GSDI_THRESHOLD_LEN           3
#define GSDI_KCGPRS_LEN              8
#define GSDI_LOCIGPRS_LEN            14
#define GSDI_PSLOCI_LEN              14
#define GSDI_KEYS_LEN                33
#define GSDI_KEYSPS_LEN              33
#define GSDI_ECC_LEN                 3
#define GSDI_ALPHA_ID_LEN            15
#define GSDI_AD_LEN                  255
#define GSDI_PLMN_LEN                255
#define GSDI_PLMNWACT_LEN            255
#define GSDI_OPLMNWACT_LEN           255
#define GSDI_HPLMNWACT_LEN           255
#define GSDI_RPLMNWACT_LEN           255
#define GSDI_RAT_LEN                 1

#define GSDI_AUTN_LEN                 18
#define GSDI_USIM_AUTH_MAX_LEN        60
#define GSDI_MAX_HIST_BYTES           15

#define GSDI_SRES_LEN                 4
#define GSDI_SRES_KC_LEN              (GSDI_SRES_LEN + GSDI_KC_LEN)
#define GSDI_SIM_CAPABILITIES_LEN     1
#define GSDI_PIN_STATUS_LEN           1

#define GSDI_CST_LEN                 16
#define GSDI_IMSI_M_LEN              25
#define GSDI_IMSI_T_LEN              25
#define GSDI_LANG_PREF_LEN           10
#define GSDI_CDMA_ECC_LEN            4

#define GSDI_PHONE_NUM_POS           0  /* offset after alpha, MSISDN,ADN,etc */
#define GSDI_PHONE_NUM_LEN          12  /* len: len byte + ton/npi + bcd = 12 */
#define GSDI_PHONE_NUM_CCP_POS      12  /* offset after alpha, bcie in ccp    */
#define GSDI_PHONE_NUM_EXT1_POS     13  /* offset after alpha, ext1           */
#define GSDI_SIM_PADDING          0xFF  /* padding byte used by the sim       */
#define GSDI_EXT1_TYPE_POS           0  /* offset of type in ext1 record      */
#define GSDI_EXT1_SUBADDRESS_MASK 0x01  /* mask where ext1 type = subaddress  */
#define GSDI_EXT1_BODY_LEN          11  /* ext1 len=13, but ext1 body len=11  */
#define GSDI_EXT1_BODY_POS           1  /* offset in ext1 of body             */
#define GSDI_EXT1_EXT1_POS          12  /* offset in ext1 of next ext1 index  */
#define GSDI_EXT1_OVERFLOW_MASK   0x02  /* mask where ext1 = overflow data    */
#define GSDI_MAX_ADDR_INFO_LEN      40  /* max len of: msisdn, ccp or subaddr */

#define GSDI_SST_FDN_BYTE_OFFSET     0    /* byte offset in SST of FDN flags  */
#define GSDI_SST_FDN_ALLOCATED_MASK 0x10  /* tests FDN allocated bit in SST   */
#define GSDI_SST_FDN_ACTIVATED_MASK 0x20  /* tests FDN activated bit in SST   */

#define GSDI_SST_SMS_BYTE_OFFSET     0    /* byte offset in SST of SMS flags  */
#define GSDI_SST_SMS_ALLOCATED_MASK  0x40 /* tests SMS allocated bit in SST   */
#define GSDI_SST_SMS_ACTIVATED_MASK  0x80 /* tests SMS activated bit in SST   */

#define GSDI_SST_AOC_BYTE_OFFSET     1    /* byte offset in SST of AOC flags  */
#define GSDI_SST_AOC_ALLOCATED_MASK  0x01 /* tests AOC allocated bit in SST   */
#define GSDI_SST_AOC_ACTIVATED_MASK  0x02 /* tests AOC activated bit in SST   */

#define SST_PIN_DISABLING_OFFSET     0x00 /* 1st byte in SST */
#define SST_PIN_DISABLING_MASK       0x03 /* 0000 0011 */

#define CST_PIN_DISABLING_OFFSET     0x00
#define CST_PIN_DISABLING_MASK       0x03

#define GSDI_MAX_PIN_LEN             8
#define GSDI_MAX_MASTER_KEY_LEN      (GSDI_MAX_PIN_LEN * 3)  /* SIM LOCK Proprietary */
#define GSDI_MAX_PHONECODE_LEN       GSDI_MAX_MASTER_KEY_LEN  /* SIM LOCK Proprietary */
#define GSDI_PIN_LEN                 GSDI_MAX_PIN_LEN
#define GSDI_MAX_PIN_LENGTH          GSDI_MAX_PIN_LEN
#define GSDI_MAX_PUK_LEN             8
#define GSDI_PUK_LEN                 GSDI_MAX_PUK_LEN
#define GSDI_MAX_DATA_BLOCK_LEN      256
#define GSDI_MAX_CHANNEL_DATA_CMD_LEN 261 /* 6 Bytes of APDU Header + 255 Data */
#define GSDI_MAX_CHANNEL_DATA_RSP_LEN 300 /* Variable Data + Overhead          */
#define GSDI_MAX_SEARCH_STR_LEN      16
#define GSDI_INCREASE_VALUE_LEN      4
#define GSDI_MAX_MSG_LEN             575
#define GSDI_LUS_OFFSET              10
#define GSDI_UPDATED                 0
#define GSDI_LAI_OFFSET              4
#define GSDI_MAX_PIN_RECORDS         2

#define SIM_SMS_STATE_FREE_SPACE     0
#define SIM_SMS_STATE_IN_READ        1
#define SIM_SMS_STATE_IN_UNREAD      3
#define SIM_SMS_STATE_OUT_SENT       5
#define SIM_SMS_STATE_OUT_UNSENT     7

/* OFFSETS IN THE FILE_ATTRIBUTES */
#define GSDI_ATTR_FILE_SIZE_MSB_OFFSET    0
#define GSDI_ATTR_FILE_SIZE_LSB_OFFSET    3
#define GSDI_ATTR_FILE_ID_MSB_OFFSET      4
#define GSDI_ATTR_FILE_ID_LSB_OFFSET      5
#define GSDI_ATTR_FILE_TYPE_OFFSET        6
#define GSDI_ATTR_INCREASE_OFFSET         7
#define GSDI_ATTR_PROT_READ_OFFSET        8
#define GSDI_ATTR_PROT_WRITE_OFFSET       9
#define GSDI_ATTR_PROT_INCREASE_OFFSET    10
#define GSDI_ATTR_PROT_INVAL_OFFSET       11
#define GSDI_ATTR_PROT_REHAB_OFFSET       12
#define GSDI_ATTR_VALID_FLAG_OFFSET       13
#define GSDI_ATTR_FILE_STRUCT_OFFSET      14
#define GSDI_ATTR_REC_LEN_OFFSET          15
#define GSDI_ATTR_REC_NUM_OFFSET          16
#define GSDI_ATTR_RW_WHEN_INV_DACT_OFFSET 17

#define GSDI_MAX_REGISTERED_TASKS       10

#define GSDI_MAX_ECC_LEN                50
#define GSDI_MAX_ECC_RECORDS            5
#define GSDI_MAX_PBR_LEN                0xFF

/* GSM11.11 Section 9.4.1                         */
/* 3GPP TS31.101 Section 10.2.1.1                 */
/* Response Commands which are correctly Executed */
#define GSDI_SIM_SW1_NORM_END                   0x90
#define GSDI_SIM_SW2_NORM_END                   0x00
#define GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO  0x91
#define GSDI_SIM_SW1_NORM_END_W_SW2_LENGTH      0x9F
#define GSDI_SIM_SW1_USIM_END                   0x61

/* GSM11.11 Section 9.4.2                         */
/* 3GPP TS31.101 Section 101.2.1.2                */
/* Responses to commands which are postponed      */
#define GSDI_SW1_SIM_APP_TOOLKIT_BUSY           0x93

/* GSM11.11 Section 9.4.3                         */
/* Memory Management                              */
#define GSDI_SW1_MEMORY_MANAGEMENT              0x92
#define GSDI_SW2_MEMORY_PROBLEM                 0x40

/* GSM11.11 Section 9.4.4                         */
/* Referencing Management                         */
#define GSDI_SW1_REFERENCE_MANAGEMENT           0x94
#define GSDI_SW2_NO_EF_SELECTED                 0x00
#define GSDI_SW2_OUT_OF_RANGE                   0x02
#define GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND    0x04
#define GSDI_SW2_FILE_INCONSISTENT_WITH_CMD     0x08

/* GSM11.11 Section 9.4.5                         */
/* 3GPP TS31.101 Section 10.2.1.6                 */
/* Security Management                            */
#define GSDI_SW1_SECURITY_MANAGEMENT            0x98
#define GSDI_SW2_NO_CHV_INITIALISED             0x02
#define GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1    0x04
#define GSDI_SW2_CNTRCTN_W_CHV_STATUS           0x08
#define GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS  0x10
#define GSDI_SW2_CHV_BLOCK_UNSUCCESSFUL_UNBLOCK 0x40
#define GSDI_SW2_CHV_UNBLOCKED                  0x40
#define GSDI_SW2_INCREASED_FAIL_MAX_REACHED     0x50
#define GSDI_SW2_AUTH_ERROR_APP_SPECIFIC        0x62
#define GSDI_SW2_AUTH_ERROR_GSM_CNTXT_NOT_SUP   0x64
#define GSDI_SW2_AUTH_ERROR_KEYFRESHNESS_FAIL   0x65
#define GSDI_SW2_AUTH_ERROR_NO_MEM              0x66

/* GSM11.11 Section 9.4.6                         */
/* 3GPP TS31.101 Section 10.2.1.5                 */
/* Appliation Independent Errors                  */
#define GSDI_SW1_INCORRECT_P3                   0x67
#define GSDI_SW1_WRONG_LENGTH                   0x67   /* USIM */
#define GSDI_SW2_WRONG_LENGTH_OR_NO_DIAGS       0xFF   /* USIM */
#define GSDI_SW1_INCORRECT_P1_OR_P2             0x6B
#define GSDI_SW2_ADDRESSED_RECORD_OUT_OF_RANGE  0x02
#define GSDI_SW2_INCORRECT_P1P2_NO_EXTRA_DATA   0x00
#define GSDI_SW1_UNKNOWN_INSTRUCTION_CODE       0x6D
#define GSDI_SW1_WRONG_INSTRUCTION_CLASS        0x6E
#define GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN        0x6F

/* 3GPP TS31.101 Section 10.2.1.3                 */
/* Warnings                                       */
#define GSDI_SW1_WARNINGS_PART_1                0x62
#define GSDI_SW2_NO_INFO_VOLATILE_MEM_UNCHANGED 0x00
#define GSDI_SW2_PART_OF_DATA_MAY_BE_CORRUPTED  0x81
#define GSDI_SW2_END_REACHED_BEFORE_LE_BYTES    0x82
#define GSDI_SW2_SELECTED_FILE_INVALIDATED      0x83
#define GSDI_SW2_FCI_NOT_ACCORDING_TO_SPECS     0x84
#define GSDI_SW1_WARNINGS_PART_2                0x63
#define GSDI_SW2_FILE_FILLED_UP_BY_LAST_WRITE   0x81
#define GSDI_SW2_CMD_SUCCESS_AFTER_X_TRYS       0xCF

/* 3GPP TS31.101 Section 10.2.1.4                 */
/* Execution Errors                               */
#define GSDI_SW1_EXECUTION_ERROR_1              0x64
#define GSDI_SW2_NO_INFO_VOL_MEM_NOT_CHANGED    0x00
#define GSDI_SW1_EXECUTION_ERROR_2              0x65
#define GSDI_SW2_NO_INFO_VOL_MEM_CHANGED        0x00
#define GSDI_SW2_EXECUTION_MEMORY_PROBLEM       0x81

/* 3GPP TS31.101 Section 10.2.1.5.1               */
/* Function in CLA Not Supported                  */
#define GSDI_SW1_FUNCTIONS_IN_CLA_ERROR         0x68
#define GSDI_SW2_NO_INFORMATION_GIVEN           0x00
#define GSDI_SW2_LOGICAL_CHANNEL_NOT_SUPPORTED  0x81
#define GSDI_SW2_SECURE_MSG_NOT_SUPPORTED       0x82

/* 3GPP TS31.101 Section 10.2.1.5.2               */
/* Command Not Allowed                            */
#define GSDI_SW1_COMMAND_NOT_ALLOWED            0x69
#define GSDI_SW2_NO_INFO_GIVEN                  0x00
#define GSDI_SW2_CMD_INCOMPATIBLE_W_FILE_STRUCT 0x81
#define GSDI_SW2_SECURITY_STATUS_NOT_SATISFIED  0x82
#define GSDI_SW2_AUTH_PIN_METHOD_BLOCKED        0x83
#define GSDI_SW2_REFERENCE_DATA_INVALIDATED     0x84
#define GSDI_SW2_CNDTNS_OF_USE_NOT_SATISFIED    0x85
#define GSDI_SW2_CMD_NOT_ALLOWD_NO_EF_SELECTED  0x86

/* 3GPP TS31.101 Section 10.2.1.5.3               */
/* Wrong Parameters                               */
#define GSDI_SW1_INCORRECT_PARAMETERS           0x6A
#define GSDI_SW2_IN_THE_DATA_FIELD              0x80
#define GSDI_SW2_FUNCTION_NOT_SUPPORTED         0x81
#define GSDI_SW2_FILE_NOT_FOUND                 0x82
#define GSDI_SW2_RECORD_NOT_FOUND               0x83
#define GSDI_SW2_LC_INCONSISTENT_WITH_TLV       0x85
#define GSDI_SW2_INCORRECT_PARAMS_P1_OR_P2      0x86
#define GSDI_SW2_LC_INCONSISTENT_WITH_P1_OR_P2  0x87
#define GSDI_SW2_REFERENCE_DATA_NOT_FOUND       0x88
#define GSDI_SW2_UNDEFINED_99                   0x99

/* MFLO Specification */
#define GSDI_SW2_UCAST_AUTH_KEY_NOT_INIT         0x01
#define GSDI_SW2_UCAST_DATA_BLOCK_NOT_INIT       0x02
#define GSDI_SW2_VERIFY_UCAST_FAILED             0x04
#define GSDI_SW2_LOCKED_INIT_PARAM               0x05
#define GSDI_SW2_APPLET_NOT_INIT                 0x06
#define GSDI_SW2_PUBLIC_KEY_CERT_NOT_INIT        0x07

/* uim specific */
#define GSDI_SW1_UIM_TIMEOUT                    0x00
#define GSDI_SW2_UIM_TIMEOUT                    0x00

/*Internal to GSDI*/
#define GSDI_SW1_INT_ERROR                      0xFF
#define GSDI_SW2_INT_ERROR                      0xFF



/* ----------------------------------------------------------------------------
** MESSAGES - Command / Request
** --------------------------------------------------------------------------*/
#define MID_GSDI_MMI_SIM_IND                0x01
#define MMI_SIM_IND                         MID_GSDI_MMI_SIM_IND
#define GSDI_CNF                            0x11
#define GSDI_SIM_INIT_REQ                   0x80
#define GSDI_SIM_READ_REQ                   0x81
#define GSDI_SIM_WRITE_REQ                  0x82
#define GSDI_SIM_INCREASE_REQ               0x83
#define GSDI_RUN_GSM_ALGORITHM_REQ          0x84
#define GSDI_GET_FILE_ATTRIBUTES_REQ        0x85
#define GSDI_GET_PIN_STATUS_REQ             0x86
#define GSDI_VERIFY_PIN_REQ                 0x87
#define GSDI_CHANGE_PIN_REQ                 0x88
#define GSDI_UNBLOCK_PIN_REQ                0x89
#define GSDI_DISABLE_PIN_REQ                0x8A
#define GSDI_ENABLE_PIN_REQ                 0x8B
#define GSDI_INIT_REQ                       0x8C
#define GSDI_GET_SIM_CAPABILITIES_REQ       0x8D
#define GSDI_ENABLE_FDN_REQ                 0x8E
#define GSDI_DISABLE_FDN_REQ                0x8F
#define GSDI_ILLEGAL_SIM_REQ                0x90
#define GSDI_CALL_STATE_REQ                 0x91
#define GSDI_SIM_SEARCH_REQ                 0x92
#define GSDI_USIM_AUTHENTICATION_REQ        0x93
#define GSDI_REGISTER_CALLBACK_REQ          0x94
#define GSDI_SELECT_REQ                     0x95
#define GSDI_STORE_ESN_REQ                  0x96
#define GSDI_GET_IMAGE_REQ                  0x97
#define GSDI_SIM_REFRESH_REQ                0x98
#define GSDI_REG_FOR_REFRESH_VOTE_REQ       0x9A
#define GSDI_REG_FOR_FILE_NOTIFY_REQ        0x9B
#define GSDI_SIM_REFRESH_COMPLETE_REQ       0x9C
#define GSDI_INTERNAL_REQ                   0x9D
#define GSDI_REFRESH_UPDATE_CALL_STACK_REQ  0x9F
#define GSDI_NOTIFIED_ERROR_REQ             0xA0
#define GSDI_NOTIFY_CLIENT_REQ              0xA1
#define GSDI_SEND_APDU_REQ                  0xA2
#define GSDI_SWITCH_SLOT_PREF_REQ           0xA3
#define GSDI_GET_ATR_REQ                    0xA4
#define GSDI_OPEN_CHANNEL_REQ               0xA5
#define GSDI_CLOSE_CHANNEL_REQ              0xA6

#define GSDI_SEND_CHANNEL_DATA_REQ          0xA7
#define GSDI_GET_ECC_REQ                    0xA8
#define GSDI_GET_PBR_TABLE_REQ              0xA9
#define GSDI_SECURE_DEVICE_INFO_REQ         0xAA
#define GSDI_SIM_ACCESS_REQ                 0xAB
#define GSDI_GET_ALL_PIN_INFO_REQ           0xAC
/* ----------------------------------------------------------------------------
   PERSONALIZATION REQ (MESSAGE) IDS
   --------------------------------------------------------------------------*/
#define GSDI_PERSO_REGISTER_TASK_REQ               0xB0
#define GSDI_PERSO_ACT_FEATURE_IND_REQ             (GSDI_PERSO_REGISTER_TASK_REQ   + 1)
#define GSDI_PERSO_DEACT_FEATURE_IND_REQ           (GSDI_PERSO_ACT_FEATURE_IND_REQ + 1)
#define GSDI_PERSO_GET_FEATURE_IND_REQ             (GSDI_PERSO_DEACT_FEATURE_IND_REQ+1)
#define GSDI_PERSO_SET_FEATURE_DATA_REQ            (GSDI_PERSO_GET_FEATURE_IND_REQ + 1)
#define GSDI_PERSO_OTA_DERPERSO_REQ                (GSDI_PERSO_SET_FEATURE_DATA_REQ+ 1)
#define GSDI_PERSO_INT_PROC_REQ                    (GSDI_PERSO_OTA_DERPERSO_REQ + 1)
#define GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ         (GSDI_PERSO_INT_PROC_REQ + 1)
#define GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ    (GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ + 1)
#define GSDI_PERSO_GET_PERM_FEATURE_IND_REQ        (GSDI_PERSO_PERM_DISABLE_FEATURE_IND_REQ + 1)
#define GSDI_PERSO_GET_FEATURE_KEY_REQ             (GSDI_PERSO_GET_PERM_FEATURE_IND_REQ + 1)
#define GSDI_PERSO_CHANGE_DCK_REQ                  (GSDI_PERSO_GET_FEATURE_KEY_REQ +1)
#define GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ         (GSDI_PERSO_CHANGE_DCK_REQ + 1)
#define GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ (GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ + 1)
#define GSDI_SIM_SECURE_STORE_REQ                  (GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ + 1)
#define GSDI_PERSO_ROOT_KEY_WRITE_REQ              (GSDI_SIM_SECURE_STORE_REQ + 1)
#define GSDI_PERSO_ROOT_KEY_READ_REQ               (GSDI_PERSO_ROOT_KEY_WRITE_REQ + 1)
#define GSDI_PERSO_VERIFY_PHONE_CODE_REQ           (GSDI_PERSO_ROOT_KEY_READ_REQ + 1)
#define GSDI_PERSO_CHANGE_PHONE_CODE_REQ           (GSDI_PERSO_VERIFY_PHONE_CODE_REQ + 1)
#define GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ          (GSDI_PERSO_CHANGE_PHONE_CODE_REQ + 1)
#define GSDI_SIM_SECURE_READ_REQ                   (GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ + 1)
#define GSDI_PERSO_SET_PHONE_CODE_REQ              (GSDI_SIM_SECURE_READ_REQ + 1)
#define GSDI_PERSO_GET_LIB_VERSION_REQ             (GSDI_PERSO_SET_PHONE_CODE_REQ + 1)
#define GSDI_PERSO_GET_PHONE_CODE_STATUS_REQ       (GSDI_PERSO_GET_LIB_VERSION_REQ + 1)
#define GSDI_PERSO_GET_FEATURE_DATA_REQ            (GSDI_PERSO_GET_PHONE_CODE_STATUS_REQ +1)
/* ----------------------------------------------------------------------------
   Common PCN Handset Specification (CPHS) REQ (MESSAGE IDS
   --------------------------------------------------------------------------*/
#define GSDI_SIM_GET_CPHS_INFORMATION_REQ        0xD0

#define GSDI_INTERNAL_POST_PIN1_INIT_REQ         0xD1
#define GSDI_ACTIVATE_ONCHIP_SIM_REQ             (GSDI_INTERNAL_POST_PIN1_INIT_REQ + 1)

/* ----------------------------------------------------------------------------
** Required Definitions not existent in CDMA Only Builds
** --------------------------------------------------------------------------*/
#define MID_TIMER_EXPIRY                    0x03

#ifndef MS_GSDI
#define MS_GSDI                             0x13
#endif

#ifndef MS_TIMER
#define MS_TIMER                            0xF0
#endif

#define GS_INVALID_ID                       3
#define GS_QUEUE_GSDI_TASK                  21
#define GS_QUEUE_GSDI_LIB                   22
#define GSDI_TASK_TIMER_VALUE               20000  /* 20 secs */

#define MS_GSDI_UIM_SANITY_TIMER_EXPIRE     0x20

/* ----------------------------------------------------------------------------
** Byte Values for the General Personalization Events
** generated by the GSDI Personalization Engine
** --------------------------------------------------------------------------*/
#define GSDI_PERSO_ENG_REQ_PHONECODE 0x01
#define GSDI_PERSO_ENG_REQ_MASTERKEY 0x02

/* ----------------------------------------------------------------------------
** Card Application Masks
** --------------------------------------------------------------------------*/
#define GSDI_NO_APP_MASK        0x00
#define GSDI_TELECOM_MASK       0x01
#define GSDI_GSM_SIM_MASK       0x02
#define GSDI_USIM_APP_MASK      0x04
#define GSDI_RUIM_APP_MASK      0x08
#define GSDI_EMV_APP_MASK       0x10
#define GSDI_DCS1800_APP_MASK   0x20

/* ----------------------------------------------------------------------------
** MASKS used to determine SLOT Information for Dual Slot Targets
** --------------------------------------------------------------------------*/
#define GSDI_SLOT_NO_APP_MASK   0x00
#define GSDI_APP_SLOT_1_MASK    0x07
#define GSDI_APP_SLOT_2_MASK    0x70
#define GSDI_GSM_SLOT_1_MASK    0x01
#define GSDI_GSM_SLOT_2_MASK    0x10
#define GSDI_CDMA_SLOT_1_MASK   0x02
#define GSDI_CDMA_SLOT_2_MASK   0x20
#define GSDI_USIM_SLOT_1_MASK   0x04
#define GSDI_USIM_SLOT_2_MASK   0x40

/* ----------------------------------------------------------------------------
** MAX Number of Files GSDI can perform File Change Notification for
** ---------------------------------------------------------------------------*/
#ifdef FEATURE_USIM_1000_PBM
#define GSDI_MAX_FILE_CHANGE_NOTIFICATIONS 120
#else
#define GSDI_MAX_FILE_CHANGE_NOTIFICATIONS 80
#endif /* FEATURE_USIM_1000_PBM */

/* ----------------------------------------------------------------------------
** Maximum Length supported for Application Identifiers
** --------------------------------------------------------------------------*/
#define GSDI_MAX_APP_ID_LEN 0x10

/* ----------------------------------------------------------------------------
** Minimum Length supported for Application Identifiers
** --------------------------------------------------------------------------*/
#define GSDI_MIN_APP_ID_LEN 0x05

/* <EJECT> */
/*-----------------------------------------------------------------------------
                             Type Defs
-----------------------------------------------------------------------------*/
typedef enum
{
  /* 0 */
  /* EFs at the MF level */
  GSDI_ICCID,                  /* ICCID */
  GSDI_ELP,                    /* Extended Language Preference */
  GSDI_DIR,                    /* EF DIR for USIM */
  GSDI_ARR,                    /* Access rule reference */
   /* EFs in CDMA DF*/
  GSDI_CDMA_CC,                  /* Call Count */
  GSDI_CDMA_IMSI_M,              /* IMSI_M */
  GSDI_CDMA_IMSI_T,              /* IMSI_T */
  GSDI_CDMA_TMSI,                /* TMSI */
  GSDI_CDMA_ANALOG_HOME_SID,     /* Analog Home SID */
  GSDI_CDMA_ANALOG_OP_PARAMS,    /* Analog Operational Parameters */

  /* 10 */
  GSDI_CDMA_ANALOG_LOCN_AND_REGN_IND,  /* Analog Locn and Regn Indicators */
  GSDI_CDMA_HOME_SID_NID,        /* CDMA Home SID and NID */
  GSDI_CDMA_ZONE_BASED_REGN_IND, /* CDMA Zone Based Regn Indicators */
  GSDI_CDMA_SYS_REGN_IND,        /* CDMA System/Network Regn Indicators */
  GSDI_CDMA_DIST_BASED_REGN_IND, /* CDMA Distance Based Regn Indicators */
  GSDI_CDMA_ACCOLC,              /* Access Overload Class */
  GSDI_CDMA_CALL_TERM_MODE_PREF, /* Call Termination Mode Preferences */
  GSDI_CDMA_SCI,                 /* Suggested Slot Cycle Index */
  GSDI_CDMA_ANALOG_CHAN_PREF,    /* Analog Channel Preferences */
  GSDI_CDMA_PRL,                 /* Preferred Roaming List */

  /* 20 */
  GSDI_CDMA_RUIM_ID,             /* Removable UIM ID */
  GSDI_CDMA_CDMA_SVC_TABLE,      /* CDMA Service Table */
  GSDI_CDMA_SPC,                 /* Service Programming Code */
  GSDI_CDMA_OTAPA_SPC_ENABLE,    /* OTAPA/SPC-Enable */
  GSDI_CDMA_NAM_LOCK,            /* NAM-Lock */
  GSDI_CDMA_OTASP_OTAPA_FEATURES,/* OTASP-OTAPA Parameters */
  GSDI_CDMA_SERVICE_PREF,        /* Service Preferences */
  GSDI_CDMA_ESN_ME,              /* ESN_ME */
  GSDI_CDMA_RUIM_PHASE,          /* R-UIM Phase */
  GSDI_CDMA_PREF_LANG,           /* Preferred Languages */

  /* 30 */
  GSDI_CDMA_UNASSIGNED_1,        /* Not assigned yet */
  GSDI_CDMA_SMS,                 /* Short Messages */
  GSDI_CDMA_SMS_PARAMS,          /* Short Message Service Parameters */
  GSDI_CDMA_SMS_STATUS,          /* SMS Status */
  GSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, /* Sup Services Feature Code Table */
  GSDI_CDMA_UNASSIGNED_2,        /* Not assigned yet */
  GSDI_CDMA_HOME_SVC_PVDR_NAME,  /* CDMA Home Service Provider Name */
  GSDI_CDMA_UIM_ID_USAGE_IND,    /* R-UIM ID usage indicator */
  GSDI_CDMA_ADM_DATA,            /* CDMA Administrative Data */
  GSDI_CDMA_MSISDN,              /* Mobile Directory Number */

  /* 40 */
  GSDI_CDMA_MAXIMUM_PRL,         /* Max size of the PRL */
  GSDI_CDMA_SPC_STATUS,          /* SPC Status */
  GSDI_CDMA_ECC,                 /* Emergency Call Codes */
  GSDI_CDMA_3GPD_ME3GPDOPC,      /* Operational Capabilities */
  GSDI_CDMA_3GPD_3GPDOPM,        /* Operational Mode */
  GSDI_CDMA_3GPD_SIPCAP,         /* Simple IP Capabilities */
  GSDI_CDMA_3GPD_MIPCAP,         /* Mobile IP Capabilities */
  GSDI_CDMA_3GPD_SIPUPP,         /* Simple IP User Profile Parameters */
  GSDI_CDMA_3GPD_MIPUPP,         /* Mobile IP User Profile Parameters */
  GSDI_CDMA_3GPD_SIPSP,          /* Simple IP Status Parameters */

  /* 50 */
  GSDI_CDMA_3GPD_MIPSP,          /* Mobile IP Status Parameters */
  GSDI_CDMA_3GPD_SIPPAPSS,       /* Simple IP PAP SS Parameters */
  GSDI_CDMA_UNASSIGNED_3,        /* Reserved */
  GSDI_CDMA_UNASSIGNED_4,        /* Reserved */
  GSDI_CDMA_PUZL,                /* Preffered User Zone List */
  GSDI_CDMA_MAXPUZL,             /* Maximum PUZL */
  GSDI_CDMA_MECRP,               /* ME-specific Config REquest Param */
  GSDI_CDMA_HRPDCAP,             /* HRPD Access Auth Capability Param */
  GSDI_CDMA_HRPDUPP,             /* HRPD Access Auth User Profile Param */
  GSDI_CDMA_CSSPR,               /* CUR_SSPR_P_REV */

  /* 60 */
  GSDI_CDMA_ATC,                 /* Acceess Terminal Class */
  GSDI_CDMA_EPRL,                /* Extended Preffered Roaming List */
  GSDI_CDMA_BCSMS_CONFIG,        /* Broadcast SMS Configuration */
  GSDI_CDMA_BCSMS_PREF,          /* Broadcast SMS Preferences */
  GSDI_CDMA_BCSMS_TABLE,         /* Broadcast SMS Table */
  GSDI_CDMA_BCSMS_PARAMS,        /* Broadcast SMS Paramaters */
  GSDI_CDMA_MMS_NOTIF,           /* MMS Notification */
  GSDI_CDMA_MMS_EXT8,            /* MMS Extension 8 */
  GSDI_CDMA_MMS_ICP,             /* MMS Issuer Connectivity Parameters */
  GSDI_CDMA_MMS_UP,              /* MMS User Preferences */

  /* 70 */
  GSDI_CDMA_SMS_CAP,             /* SMS Capabilities */
  GSDI_CDMA_3GPD_IPV6CAP,        /* IPv6 Capabilities */
  GSDI_CDMA_3GPD_MIPFLAGS,       /* Mobile IP Flags */
  GSDI_CDMA_3GPD_TCPCONFIG,      /* TCP Configurations */
  GSDI_CDMA_3GPD_DGC,            /* Data Generic Configurations */
  GSDI_CDMA_BROWSER_CP,          /* Browser Connectivity Parameters */
  GSDI_CDMA_BROWSER_BM,          /* Browser Bookmarks */
  GSDI_CDMA_3GPD_SIPUPPEXT,      /* Simple IP User Profile Parameters Extension */
  GSDI_CDMA_MMS_CONFIG,          /* MMS Configuration */
  GSDI_CDMA_JAVA_DURL,           /* Java Download URL */

  /* 80 */
  GSDI_CDMA_3GPD_MIPUPPEXT,      /* Mobile IP User Profile Parameters Extension */
  GSDI_CDMA_BREW_DLOAD,          /* BREW Download */
  GSDI_CDMA_BREW_TSID,           /* BREW Teleservice ID */
  GSDI_CDMA_BREW_SID,            /* BREW Subscriber ID */
  GSDI_CDMA_LBS_XCONFIG,         /* LBS XTRA Configuration */
  GSDI_CDMA_LBS_XSURL,           /* LBS XTRA Server URLs */
  GSDI_CDMA_LBS_V2CONFIG,        /* LBS V2 Configuration */
  GSDI_CDMA_LBS_V2PDEADDR,       /* LBS V2 PDE Address */
  GSDI_CDMA_LBS_V2MPCADDR,       /* LBS V2 MPC Address */
  GSDI_CDMA_BREW_AEP,            /* BREW Application Execution Policy*/

  /* 90 */
  GSDI_CDMA_MODEL,               /* Model Info */
  GSDI_CDMA_RC,                  /* Root Certificate */
  GSDI_CDMA_APP_LABELS,          /* Application Labels */
  GSDI_CDMA_USER_AGENT_STRING,   /* KDDI Agent String */
  GSDI_CDMA_GID2,                /* KDDI GID2 */
  GSDI_CDMA_GID1,                /* KDDI GID1 */
  GSDI_CDMA_GID_REFERENCES,      /* KDDI GID References */
  GSDI_CDMA_IMSI_STATUS,         /* KDDI IMSI Status */
  GSDI_CDMA_ME_DOWNLOADABLE_DATA, /* KDDI ME Downloadable Data */
  GSDI_CDMA_ME_SETTING_DATA,     /* KDDI ME Setting Date */

  /* 100 */
  GSDI_CDMA_ME_USER_DATA,        /* KDDI User Data */
  GSDI_CDMA_RESERVED2,           /* KDDI Reserved */
  GSDI_CDMA_RESERVED1,           /* KDDI Reserved */
  GSDI_CDMA_UIM_SVC_TABLE,       /* KDDI UIM Service Table */
  GSDI_CDMA_UIM_FEATURES,        /* KDDI UIM Features */
  GSDI_CDMA_UIM_VERSION,         /* KDDI UIM Version */
  GSDI_CDMA_HRPD_HRPDUPP,        /* HRPD User Profile Parameters */
  GSDI_CDMA_SF_EUIM_ID,          /* Short Form EUIMID */
  /* EFs in DF GSM */
  GSDI_GSM_LP,              /* Language Preference */
  GSDI_GSM_IMSI,            /* IMSI */

  /* 110 */
  GSDI_GSM_KC,              /* Ciphering Key Kc */
  GSDI_GSM_PLMN,            /* PLMN selector */
  GSDI_GSM_HPLMN,           /* HPLMN search period */
  GSDI_GSM_ACM_MAX,         /* ACM Maximum value */
  GSDI_GSM_SST,             /* SIM Service table */
  GSDI_GSM_ACM,             /* Accumulated call meter */
  GSDI_GSM_GID1,            /* Group Identifier Level 1 */
  GSDI_GSM_GID2,            /* Group Identifier Level 2 */
  GSDI_GSM_SPN,             /* Service Provider Name */
  GSDI_GSM_PUCT,            /* Price Per Unit and currency table */

  /* 120 */
  GSDI_GSM_CBMI,            /* Cell broadcast message identifier selection */
  GSDI_GSM_BCCH,            /* Broadcast control channels */
  GSDI_GSM_ACC,             /* Access control class */
  GSDI_GSM_FPLMN,           /* Forbidden PLMNs */
  GSDI_GSM_LOCI,            /* Location information */
  GSDI_GSM_AD,              /* Administrative data */
  GSDI_GSM_PHASE,           /* Phase identification */
  GSDI_GSM_VGCS,            /* Voice Group Call service */
  GSDI_GSM_VGCSS,           /* Voice Group Call service status */
  GSDI_GSM_VBS,             /* Voice Broadcast service  */

  /* 130 */
  GSDI_GSM_VBSS,            /* Voice Broadcast service status */
  GSDI_GSM_EMLPP,           /* Enhanced multi level pre-emption and priority */
  GSDI_GSM_AAEM,            /* Automatic Answer for eMLPP service */
  GSDI_GSM_CBMID,           /* Cell Broadcast Message id for data dload */
  GSDI_GSM_ECC,             /* Emergency Call Codes */
  GSDI_GSM_CBMIR,           /* Cell Broadcast Message id range selection */
  GSDI_GSM_DCK,             /* De-personalization control keys */
  GSDI_GSM_CNL,             /* Co-operative network list */
  GSDI_GSM_NIA,             /* Network's indication of alerting */
  GSDI_GSM_KCGPRS,          /* GPRS ciphering key */

  /* 140 */
  GSDI_GSM_LOCIGPRS,        /* GPRS location information */
  GSDI_GSM_SUME,            /* Setup Menu elements */
  GSDI_GSM_PLMNWACT,        /* PLMN Selector with Access technology */
  GSDI_GSM_OPLMNWACT,       /* Operator controlled PLMNWACT */
  GSDI_GSM_HPLMNACT,        /* HPLNMN Access technology */
  GSDI_GSM_CPBCCH,          /* CPBCCH information */
  GSDI_GSM_INVSCAN,         /* Investigation PLMN Scan */
  GSDI_GSM_RPLMNAT,         /* RPLMN  Last used Access Technology */
  GSDI_GSM_VMWI,            /* CPHS: Voice Mail Waiting Indicator */
  GSDI_GSM_SVC_STR_TBL,     /* CPHS: Service String Table */

  /* 150 */
  GSDI_GSM_CFF,             /* CPHS: Call Forwarding Flag */
  GSDI_GSM_ONS,             /* CPHS: Operator Name String */
  GSDI_GSM_CSP,             /* CPHS: Customer Service Profile */
  GSDI_GSM_CPHSI,           /* CPHS: CPHS Information */
  GSDI_GSM_MN,              /* CPHS: Mailbox Number */
  GSDI_GSM_PNN,             /* PLMN Network Name */
  GSDI_GSM_OPL,             /* Operator PLMN List  */
  GSDI_GSM_CFIS,            /* Call Forwarding Indicator Status */
  GSDI_GSM_ORANGE_DFS,      /* Dynamic Flags Status */
  GSDI_GSM_ORANGE_D2FS,     /* Dynamic2 Flag Setting */

  /* 160 */
  GSDI_GSM_ORANGE_CSP2,     /* Customer Service Profile Line2*/
  /* EFs at SoLSA */
  GSDI_GSM_SAI,             /* SoLSA access indicator */
  GSDI_GSM_SLL,             /* SoLSA LSA list */
  /* EFs at MExE Level */
  GSDI_GSM_MEXE_ST,         /* MExE Service table */
  GSDI_GSM_ORPK,            /* Operator Root Public Key */
  GSDI_GSM_ARPK,            /* Administrator Root Public Key */
  GSDI_GSM_TPRPK,           /* Third party Root public key */
  GSDI_GSM_MBDN,            /* Mail Box Dialing Number*/
  GSDI_GSM_EXT6,            /* Extension 6*/
  GSDI_GSM_MBI,             /* Mail Box Identifier*/

  /* 170 */
  GSDI_GSM_MWIS,            /* Message Wating Indication Status*/
  GSDI_GSM_EXT1,            /* Extension 1*/
  GSDI_GSM_SPDI,            /* Service Provider Display Information*/
  /* image file */           /* not sure where they are used */
  GSDI_GSM_IMG,
  GSDI_GSM_IMG1INST1,
  GSDI_GSM_IMG1INST2,
  GSDI_GSM_IMG1INST3,
  GSDI_GSM_IMG2INST1,
  GSDI_GSM_IMG2INST2,
  GSDI_GSM_IMG2INST3,

  /* 180 */
  GSDI_GSM_IMG3INST1,
  GSDI_GSM_IMG3INST2,
  GSDI_GSM_IMG3INST3,
  /* EFs at the telecom DF in GSM and CDMA */
  GSDI_TELECOM_ADN,         /* Abbrev Dialing Numbers */
  GSDI_TELECOM_FDN,         /* Fixed dialling numbers */
  GSDI_TELECOM_SMS,         /* Short messages */
  GSDI_TELECOM_CCP,         /* Capability Configuration Parameters */
  GSDI_TELECOM_ECCP,        /* Extended CCP */
  GSDI_TELECOM_MSISDN,      /* MSISDN */
  GSDI_TELECOM_SMSP,        /* SMS parameters */

  /* 190 */
  GSDI_TELECOM_SMSS,        /* SMS Status */
  GSDI_TELECOM_LND,         /* Last number dialled */
  GSDI_TELECOM_SDN,         /* Service Dialling numbers */
  GSDI_TELECOM_EXT1,        /* Extension 1 */
  GSDI_TELECOM_EXT2,        /* Extension 2 */
  GSDI_TELECOM_EXT3,        /* Extension 3 */
  GSDI_TELECOM_BDN,         /* Barred Dialing Numbers */
  GSDI_TELECOM_EXT4,        /* Extension 4 */
  GSDI_TELECOM_SMSR,        /* SMS reports */
  GSDI_TELECOM_CMI,         /* Comparison Method Information */

  /* 200 */
  GSDI_TELECOM_SUME,        /* Setup Menu elements */
  GSDI_TELECOM_ARR,         /* Access Rule reference */
  /* EFs at DF GRAPHICS under DF Telecom */
  GSDI_IMAGE_FILE,          /* Image instance data files */
  /* EFs at the DF PHONEBOOK under  DF Telecom */
  GSDI_TELECOM_PBR,         /* Phone book reference file */
  GSDI_TELECOM_CCP1,        /* Capability Configuration parameters 1 */
  GSDI_TELECOM_UID,         /* Unique Identifier */
  GSDI_TELECOM_PSC,         /* Phone book synchronization center */
  GSDI_TELECOM_CC,          /* Change counter */
  GSDI_TELECOM_PUID,        /* Previous Unique Identifier */
  /* EFs of USIM ADF */
  GSDI_USIM_LI,             /* Language Indication */

  /* 210 */
  GSDI_USIM_IMSI,           /* IMSI */
  GSDI_USIM_KEYS,           /* Ciphering and Integrity keys */
  GSDI_USIM_KEYSPS,         /* C and I keys for packet switched domain */
  GSDI_USIM_PLMNWACT,       /* User controlled PLMN selector with access tech */
  GSDI_USIM_UPLMNSEL,       /* UPLMN selector */
  GSDI_USIM_HPLMN,          /* HPLMN search period */
  GSDI_USIM_ACM_MAX,        /* ACM maximum value */
  GSDI_USIM_UST,            /* USIM Service table */
  GSDI_USIM_ACM,            /* Accumulated Call meter */
  GSDI_USIM_GID1,           /* Group Identifier Level  */

  /* 220 */
  GSDI_USIM_GID2,           /* Group Identifier Level 2 */
  GSDI_USIM_SPN,            /* Service Provider Name */
  GSDI_USIM_PUCT,           /* Price Per Unit and Currency table */
  GSDI_USIM_CBMI,           /* Cell Broadcast Message identifier selection */
  GSDI_USIM_ACC,            /* Access control class */
  GSDI_USIM_FPLMN,          /* Forbidden PLMNs */
  GSDI_USIM_LOCI,           /* Location information */
  GSDI_USIM_AD,             /* Administrative data */
  GSDI_USIM_CBMID,          /* Cell Broadcast msg identifier for data download */
  GSDI_USIM_ECC,            /* Emergency call codes */

  /* 230 */
  GSDI_USIM_CBMIR,          /* Cell broadcast msg identifier range selection */
  GSDI_USIM_PSLOCI,         /* Packet switched location information */
  GSDI_USIM_FDN,            /* Fixed dialling numbers */
  GSDI_USIM_SMS,            /* Short messages */
  GSDI_USIM_MSISDN,         /* MSISDN */
  GSDI_USIM_SMSP,           /* SMS parameters */
  GSDI_USIM_SMSS,           /* SMS Status */
  GSDI_USIM_SDN,            /* Service dialling numbers */
  GSDI_USIM_EXT2,           /* Extension 2 */
  GSDI_USIM_EXT3,           /* Extension 3 */

  /* 240 */
  GSDI_USIM_SMSR,           /* SMS reports */
  GSDI_USIM_ICI,            /* Incoming call information */
  GSDI_USIM_OCI,            /* Outgoing call information */
  GSDI_USIM_ICT,            /* Incoming call timer */
  GSDI_USIM_OCT,            /* Outgoing call timer */
  GSDI_USIM_EXT5,           /* Extension 5 */
  GSDI_USIM_CCP2,           /* Capability Configuration Parameters 2 */
  GSDI_USIM_EMLPP,          /* Enhanced Multi Level Precedence and Priority */
  GSDI_USIM_AAEM,           /* Automatic answer for eMLPP service */
  GSDI_USIM_GMSI,           /* Group identity */

  /* 250 */
  GSDI_USIM_HIDDENKEY,      /* key for hidden phonebook entries */
  GSDI_USIM_BDN,            /* Barred dialling numbers */
  GSDI_USIM_EXT4,           /* Extension 4 */
  GSDI_USIM_CMI,            /* Comparison Method information */
  GSDI_USIM_EST,            /* Enabled services table */
  GSDI_USIM_ACL,            /* Access Point Name Control List */
  GSDI_USIM_DCK,            /* De-personalization Control Keys */
  GSDI_USIM_CNL,            /* Co-operative network list */
  GSDI_USIM_START_HFN,      /* Init values for Hyper-frame number */
  GSDI_USIM_THRESHOLD,      /* Max value of START */

  /* 260 */
  GSDI_USIM_OPLMNWACT,      /* Operator controlled PLMN sel with access tech */
  GSDI_USIM_OPLMNSEL,       /* OPLMN selector */
  GSDI_USIM_HPLMNWACT,      /* HPLMN selector with access tech */
  GSDI_USIM_ARR,            /* Access Rule reference */
  GSDI_USIM_RPLMNACT,       /* RPLMN last used access tech */
  GSDI_USIM_NETPAR,         /* Network parameters */
  GSDI_USIM_VMWI,           /* CPHS: Voice Mail Waiting Indicator */
  GSDI_USIM_SVC_STR_TBL,    /* CPHS: Service String Table */
  GSDI_USIM_CFF,            /* CPHS: Call Forwarding Flag */
  GSDI_USIM_ONS,            /* CPHS: Operator Name String */

  /* 260 */
  GSDI_USIM_CSP,            /* CPHS: Customer Service Profile */
  GSDI_USIM_CPHSI,          /* CPHS: CPHS Information */
  GSDI_USIM_MN,             /* CPHS: Mailbox Number */
  GSDI_USIM_PNN,            /* PLMN Netowrk Operator Name */
  GSDI_USIM_OPL,            /* Operator PLMN List */
  /* EFs of ORANGE DF under USIM ADF */
  GSDI_USIM_ORANGE_DFS,     /* Dynamic Flags Status */
  GSDI_USIM_ORANGE_D2FS,    /* Dynamic2 Flag Setting */
  GSDI_USIM_ORANGE_CSP2,    /* Customer Service Profile Line2*/
  GSDI_USIM_ORANGE_PARAMS,  /* EF Parmams - Welcome Message */
  /*EFs at DF PHONEBOOK under USIM ADF */
  GSDI_USIM_PBR,            /* Phone book reference file */

  /* 280 */
  GSDI_USIM_PSC,            /* Phone book synchronization center */
  GSDI_USIM_CC,             /* Change counter */
  GSDI_USIM_PUID,           /* Previous Unique Identifier */
  /*EFs at DF GSM under USIM ADF */
  GSDI_USIM_KC,             /* GSM ciphering key Kc */
  GSDI_USIM_KCGPRS,         /* GPRS ciphering key */
  GSDI_USIM_CPBCCH,         /* CPBCCH information */
  GSDI_USIM_INVSCAN,        /* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
  GSDI_USIM_MEXE_ST,        /* MExE Service table */
  GSDI_USIM_ORPK,           /* Operator Root Public Key */
  GSDI_USIM_ARPK,           /* Administrator Root Public Key */

  /* 290 */
  GSDI_USIM_TPRPK,          /* Third party Root public key */
  GSDI_USIM_MBDN,           /* Mailbox Dialing Number*/
  GSDI_USIM_EXT6,           /* Extension 6 */
  GSDI_USIM_MBI,            /* Mailbox Identifier*/
  GSDI_USIM_MWIS,           /* Message Waiting Indication Status*/
  GSDI_USIM_SPDI,           /* Service Provider Display Information*/
  GSDI_USIM_SPT_TABLE,      /* Cingular TST*/
  GSDI_USIM_EHPLMN,         /* Equivalent HPLMN */
  GSDI_USIM_CFIS,               /* USIM ADF: Call Forwarding Indicator Status */
  /* EFs of DCS1800 DF */
  GSDI_DCS1800_IMSI,        /* Imsi */

  /* 300 */
  GSDI_DCS1800_KC,          /* Ciphering Key Kc */
  GSDI_DCS1800_PLMN,        /* PLMN selector */
  GSDI_DCS1800_SST,         /* SIM Service table */
  GSDI_DCS1800_BCCH,        /* Broadcast control channels */
  GSDI_DCS1800_ACC,         /* Access control class */
  GSDI_DCS1800_FPLMN,       /* Forbidden PLMNs */
  GSDI_DCS1800_LOCI,        /* Location information */
  GSDI_DCS1800_AD,          /* Administrative data */
  /* EFs given by path */
  GSDI_EF_BY_PATH,         /* File selection by path */
  /* EFs selected by path at the telecom DF in GSM and CDMA */
  GSDI_TELECOM_IAP,         /* Index Administration File */

  /* 310 */
  GSDI_TELECOM_PBC,         /* Phone Book Control */
  GSDI_TELECOM_GRP,         /* Grouping File */
  GSDI_TELECOM_AAS,         /* Additional Number Alpha String */
  GSDI_TELECOM_GAS,         /* Grouping Information Alpha String */
  GSDI_TELECOM_ANR,         /* Additional Number */
  GSDI_TELECOM_SNE,         /* Secondary Name Entry */
  GSDI_TELECOM_EMAIL,       /* Email */
  GSDI_TELECOM_ANRA,        /* Additional Number A */
  GSDI_TELECOM_ANRB,        /* Additional Number B */
  GSDI_TELECOM_ANRC,        /* Additional Number C */

  /* 320 */
  GSDI_TELECOM_ANR1,        /* Additional Number 1 */
  GSDI_TELECOM_ANRA1,       /* Additional Number A1 */
  GSDI_TELECOM_ANRB1,       /* Additional Number B1 */
#ifdef FEATURE_USIM_1000_PBM
  GSDI_TELECOM_ANRC1,       /* Additional Number C1 */
#endif /*FEATURE_USIM_1000_PBM*/
  GSDI_TELECOM_ADN1,        /* Abbreviated Dialing Numbers 1 */
  GSDI_TELECOM_PBC1,        /* Phone Book Control 1 */
  GSDI_TELECOM_GRP1,        /* Grouping File 1 */
  GSDI_TELECOM_SNE1,        /* Secondary Name Entry 1 */
  GSDI_TELECOM_UID1,        /* Unique Identifier 1 */
  GSDI_TELECOM_EMAIL1,      /* Email 1 */

  /* 330 */
  GSDI_TELECOM_IAP1,        /* Index Administration File 1 */
#ifdef FEATURE_USIM_1000_PBM
  GSDI_TELECOM_ANR2,        /* Additional Number 2 */
  GSDI_TELECOM_ANRA2,       /* Additional Number A2 */
  GSDI_TELECOM_ANRB2,       /* Additional Number B2 */
  GSDI_TELECOM_ANRC2,       /* Additional Number C2 */
  GSDI_TELECOM_ADN2,        /* Abbreviated Dialing Number 2 */
  GSDI_TELECOM_EMAIL2,      /* Email 2 */
  GSDI_TELECOM_PBC2,        /* Phone Book Control 2 */
  GSDI_TELECOM_GRP2,        /* Grouping File 2 */
  GSDI_TELECOM_SNE2,        /* Secondary Name Entry 2 */

  /* 340 */
  GSDI_TELECOM_UID2,        /* Unique Identifier 2 */
  GSDI_TELECOM_IAP2,        /* Index Administration File 2 */
  GSDI_TELECOM_ANR3,        /* Additional Number 3 */
  GSDI_TELECOM_ANRA3,       /* Additional Number A3 */
  GSDI_TELECOM_ANRB3,       /* Additional Number B3 */
  GSDI_TELECOM_ANRC3,       /* Additional Number C3 */
  GSDI_TELECOM_ADN3,        /* Abbreviated Dialing Numbers 3 */
  GSDI_TELECOM_PBC3,        /* Phone Book Control 3 */
  GSDI_TELECOM_GRP3,        /* Grouping File 3 */
  GSDI_TELECOM_SNE3,        /* Secondary Name Entry  3 */

  /* 350 */
  GSDI_TELECOM_UID3,        /* Unique Identifier 3*/
  GSDI_TELECOM_EMAIL3,      /* Email 3 */
  GSDI_TELECOM_IAP3,        /* Index Administration File 3 */
#endif /*FEATURE_USIM_1000_PBM*/
  /* EFs selected by path of USIM ADF */
  GSDI_USIM_ADN,            /* Abbreviated Dialing Numbers */
  GSDI_USIM_IAP,            /* Index Administration File */
  GSDI_USIM_PBC,            /* Phone Book Control */
  GSDI_USIM_EXT1,           /* Extension 1 */
  GSDI_USIM_GRP,            /* Grouping File */
  GSDI_USIM_AAS,            /* Additional Number Alpha String */
  GSDI_USIM_AAS1,           /* Additional Number Alpha String 1 */

  /* 360 */
  GSDI_USIM_GAS,            /* Grouping Information Alpha String */
  GSDI_USIM_GAS1,           /* Grouping Information Alpha String */
  GSDI_USIM_ANR,            /* Additional Number */
  GSDI_USIM_SNE,            /* Secondary Name Entry */
  GSDI_USIM_EMAIL,          /* Email */
  GSDI_USIM_ANRA,           /* Additional Number A */
  GSDI_USIM_ANRB,           /* Additional Number B */
  GSDI_USIM_ANRC,           /* Additional Number C */
  GSDI_USIM_ANR1,           /* Additional Number 1 */
  GSDI_USIM_ANRA1,          /* Additional Number A1 */

  /* 370 */
  GSDI_USIM_ANRB1,          /* Additional Number B1 */
  GSDI_USIM_ANRC1,          /* Additional Number C1 */
  GSDI_USIM_ADN1,           /* Abbreviated Dialing Numbers 1 */
  GSDI_USIM_PBC1,           /* Phone Book Control 1 */
  GSDI_USIM_GRP1,           /* Grouping File 1 */
  GSDI_USIM_SNE1,           /* Secondary Name Entry 1 */
  GSDI_USIM_UID1,           /* Unique Identifier 1 */
  GSDI_USIM_EMAIL1,         /* Email 1 */
  GSDI_USIM_IAP1,           /* Index Administration File 1 */
  GSDI_USIM_ACT_HPLMN,      /*Acting HPLMN*/

  /* 380 */
  GSDI_USIM_RAT,            /* Radio Access Technology Indicator */
  GSDI_USIM_UID,            /* Unique Identifier */
  GSDI_USIM_CCP1,           /* Capability Configuration parameters 1 */
#ifdef FEATURE_USIM_1000_PBM
  GSDI_USIM_ANR2,          /* Additional Number 2 */
  GSDI_USIM_ANRA2,         /* Additional Number A2 */
  GSDI_USIM_ANRB2,         /* Additional Number B2 */
  GSDI_USIM_ANRC2,         /* Additional Number C2 */
  GSDI_USIM_ADN2,          /* Additional Number 2 */
  GSDI_USIM_EMAIL2,        /* Email 2 */
  GSDI_USIM_PBC2,          /* Phone Book Control 2 */

  /* 390 */
  GSDI_USIM_GRP2,          /* Grouping File 2 */
  GSDI_USIM_SNE2,          /* Secondary Name Entry 2 */
  GSDI_USIM_UID2,          /* Unique Identifier 2 */
  GSDI_USIM_IAP2,          /* Index Administration File 2 */
  GSDI_USIM_ANR3,          /* Additional Number 3 */
  GSDI_USIM_ANRA3,         /* Additional Number A3 */
  GSDI_USIM_ANRB3,         /* Additional Number B3 */
  GSDI_USIM_ANRC3,         /* Additional Number C3 */
  GSDI_USIM_ADN3,          /* Abbreviated Dialling Number 3 */
  GSDI_USIM_EMAIL3,        /* Email 3 */

  /* 400 */
  GSDI_USIM_PBC3,          /* Phone Book Control 3 */
  GSDI_USIM_GRP3,          /* Grouping File 3 */
  GSDI_USIM_SNE3,          /* Secondary Name Entry 3 */
  GSDI_USIM_UID3,          /* Unique Identifier 3 */
  GSDI_USIM_IAP3,          /* Index Administration File 3 */
#endif /*FEATURE_USIM_1000_PBM*/
  /* EFs for MMS Notification */
  GSDI_USIM_MMSN,           /* MMS Notification */
  GSDI_USIM_MMSICP,         /* MMS Issuer connectivity parameters */
  GSDI_USIM_MMSUP,          /* MMS User Preferences */
  GSDI_USIM_MMSUCP,         /* MMS User connectivity parameters */
  GSDI_USIM_EXT8,           /* Extension 8 */

  /* 410 */
  /* EFs of WIM DF */
  GSDI_WIM_ODF,             /* WIM ODF */
  GSDI_WIM_TOKEN_INFO,      /* Generic token info and capabilities */
  GSDI_WIM_UNUSED_SPACE,    /* Unused Space */
  /* EFs of ORANGE DF */
  GSDI_ORANGE_DFS,          /* Dynamic Flags Status */
  GSDI_ORANGE_D2FS,         /* Dynamic2 Flag Setting */
  GSDI_ORANGE_CSP2,         /* Customer Service Profile Line2*/
  GSDI_ORANGE_PARAMS,       /* Welcome Message */
  GSDI_CINGULAR_ACT_HPLMN,  /* Dynamic Flags Status */ /*Acting HPLMN*/
  GSDI_CINGULAR_SPT_TABLE,   /*Support Table*/
  /* EFs of ISIM ADF */
  GSDI_ISIM_IMPI,            /* IMS private user identity */

  /* 420 */
  GSDI_ISIM_DOMAIN,          /* Home Network Domain Name */
  GSDI_ISIM_IMPU,            /* IMS public user identity */
  GSDI_ISIM_ARR,             /* Access Rule Reference */
  GSDI_ISIM_KEYS,            /* Ciphering and Integrity Keys for IMS */
  GSDI_ISIM_AD,              /* Administrative Data */
  /* Home zone */
  GSDI_HZI_HZ,
  GSDI_HZI_CACHE1,           /*UHZI-Cache1*/
  GSDI_HZI_CACHE2,           /*UHZI-Cache2*/
  GSDI_HZI_CACHE3,              /*UHZI-Cache3*/
  GSDI_HZI_CACHE4,              /*UHZI-Cache4*/

  /* 430 */
  GSDI_HZI_SUBSCRIBED_LAC_CI_1, /*UHZI-SUBSCRIBED_LAC_CI_1*/
  GSDI_HZI_SUBSCRIBED_LAC_CI_2, /*UHZI-SUBSCRIBED_LAC_CI_2*/
  GSDI_HZI_SUBSCRIBED_LAC_CI_3, /*UHZI-SUBSCRIBED_LAC_CI_3*/
  GSDI_HZI_SUBSCRIBED_LAC_CI_4, /*UHZI-SUBSCRIBED_LAC_CI_4*/
  GSDI_HZI_TAGS,                /*UHZI- Tags*/
  GSDI_HZI_UHZI_SETTINGS,       /*UHZI- Settings*/

  GSDI_NO_SUCH_ITEM         /* No such item */
} gsdi_file_enum_type;

/* GSDI SEARCH MODE TYPES */
typedef enum {
  GSDI_NO_SUCH_SRCH_MODE   = 0,   /* No such search mode */
  GSDI_SIMPLE_SRCH         = 1,   /* Simple search */
  GSDI_ENHANCED_SRCH       = 2,   /* Enhanced Search */
  GSDI_MAXIMUM_SRCH_MODE   = 3
} gsdi_srch_mode;

/* GSDI_SEARCH_DIRECTION_TYPES */
typedef enum {
  GSDI_SRCH_FORWARD   = 0x04,  /* Search forward */
  GSDI_SRCH_BACKWARD  = 0x05  /* Search backward */
} gsdi_srch_dir_type;


typedef enum {
  GSDI_NOT_IN_TC = 0,
  GSDI_IN_TC_GW,
  GSDI_IN_TC_1X
}gsdi_in_tc_mode_enum_type;

typedef enum {
  GSDI_GLOBAL_PHONEBOOK_TYPE,
  GSDI_LOCAL_PHONEBOOK_TYPE
}gsdi_phonebook_type;


/* GSDI_SLOT_1 is the default value for single slot solution */
typedef enum {
  GSDI_SLOT_NONE = 0x00,
  GSDI_SLOT_1 = 0x01,
  GSDI_SLOT_2 = 0x02,
  GSDI_SLOT_AUTOMATIC = 0x03,
  GSDI_SLOT_MAX = 0xFF
}gsdi_slot_id_type;

/* --------------------------------------------------------------------------
** GSDI_PERSO_EVENT_ENUM_TYPE
** Events Identifiers sent to registered clients.
** ------------------------------------------------------------------------*/
typedef enum
{
  GSDI_PERSO_NO_EVENT = 0x0, /* PERSO Event Init Value                      */
  GSDI_PERSO_NW_FAILURE,     /* NW Personalization Init Failure             */
  GSDI_PERSO_NS_FAILURE,     /* NS Personalization Init Failure             */
  GSDI_PERSO_SP_FAILURE,     /* PS Personalization Init Failure             */
  GSDI_PERSO_CP_FAILURE,     /* CP Personalization Init Failure             */
  GSDI_PERSO_SIM_FAILURE,    /* SIM/USIM Personalization Deactivated        */
  GSDI_PERSO_NW_DEACTIVATED, /* NW Personalization Deactivated              */
  GSDI_PERSO_NS_DEACTIVATED, /* NS Personalization Deactivated              */
  GSDI_PERSO_SP_DEACTIVATED, /* PS Personalization Deactivated              */
  GSDI_PERSO_CP_DEACTIVATED, /* CP Personalization Deactivated              */
  GSDI_PERSO_SIM_DEACTIVATED,/* SIM/USIM Personalization Deactivated        */
  GSDI_PERSO_NCK_BLOCKED,    /* Network Control Key was Blocked             */
  GSDI_PERSO_NSK_BLOCKED,    /* Network Subset Control Key was Blocked      */
  GSDI_PERSO_SPK_BLOCKED,    /* Service Provider Control Key was Blocked    */
  GSDI_PERSO_CCK_BLOCKED,    /* Corporate Control Key was Blocked           */
  GSDI_PERSO_PPK_BLOCKED,    /* SIM/USIM Control Key was Blocked            */
  GSDI_PERSO_NCK_UNBLOCKED,    /* Network Control Key was Unblocked         */
  GSDI_PERSO_NSK_UNBLOCKED,    /* Network Subset Control Key was Unblocked  */
  GSDI_PERSO_SPK_UNBLOCKED,    /* Service Provider Control Key Was Unblocked*/
  GSDI_PERSO_CCK_UNBLOCKED,    /* Corporate Control Key was Unblocked       */
  GSDI_PERSO_PPK_UNBLOCKED,    /* SIM/USIM Control Key was Unblocked        */

  GSDI_PERSO_RUIM_NW1_FAILURE,    /* NW TYPE1 Personalization Init Failure  */
  GSDI_PERSO_RUIM_NW2_FAILURE,    /* NW TYPE2 Personalization Init Failure  */
  GSDI_PERSO_RUIM_HRPD_FAILURE,   /* HRPD Personalization Init Failure      */
  GSDI_PERSO_RUIM_SP_FAILURE,     /* PS Personalization Init Failure        */
  GSDI_PERSO_RUIM_CP_FAILURE,     /* CP Personalization Init Failure        */
  GSDI_PERSO_RUIM_RUIM_FAILURE,   /* RUIM Personalization Init Failure      */

  GSDI_PERSO_RUIM_NW1_DEACTIVATED,  /* NW TYPE1Personalization Deactivated  */
  GSDI_PERSO_RUIM_NW2_DEACTIVATED,  /* NW TYPE2 Personalization Deactivated */
  GSDI_PERSO_RUIM_HRPD_DEACTIVATED, /*HRPD Personalization Deactivated      */
  GSDI_PERSO_RUIM_SP_DEACTIVATED,   /* SP Personalization Deactivated       */
  GSDI_PERSO_RUIM_CP_DEACTIVATED,   /* CP Personalization Deactivated       */
  GSDI_PERSO_RUIM_RUIM_DEACTIVATED, /* RUIM Personalization Deactivated     */

  GSDI_PERSO_RUIM_NCK1_BLOCKED,    /* Network1 Control Key was Blocked      */
  GSDI_PERSO_RUIM_NCK2_BLOCKED,    /* Network2 Control Key was Blocked      */
  GSDI_PERSO_RUIM_HNCK_BLOCKED,    /* HRPD Control Key was Blocked          */
  GSDI_PERSO_RUIM_SPCK_BLOCKED,    /* Service Provider Control Key was Blocked*/
  GSDI_PERSO_RUIM_CCK_BLOCKED,     /* Corporate Control Key was Blocked       */
  GSDI_PERSO_RUIM_PCK_BLOCKED,     /* RUIM Control Key was Blocked            */

  GSDI_PERSO_RUIM_NCK1_UNBLOCKED,  /* Network1 Control Key was Unblocked        */
  GSDI_PERSO_RUIM_NCK2_UNBLOCKED,  /* Network2 Control Key was Unblocked        */
  GSDI_PERSO_RUIM_HNCK_UNBLOCKED,  /* HRPD Control Key was Unblocked            */
  GSDI_PERSO_RUIM_SPCK_UNBLOCKED,  /* Service Provider Control Key Was Unblocked*/
  GSDI_PERSO_RUIM_CCK_UNBLOCKED,   /* Corporate Control Key was Unblocked       */
  GSDI_PERSO_RUIM_PCK_UNBLOCKED,   /* RUIM Control Key was Unblocked            */

  GSDI_PERSO_PROP1_FAILURE,  /* Customer Specific Proprietary Failure       */
  GSDI_PERSO_PROP1_DEACTIVATED,/* Customer Specific Proprietary Failure     */
  GSDI_PERSO_PROP1_BLOCKED,  /* The Personalization Key is Blocked          */
  GSDI_PERSO_PROP1_UNBLOCKED,  /* Proprietary Key is Unblocked              */
  GSDI_PERSO_SANITY_ERROR,   /* Files required when created are not avail   */
  GSDI_PERSO_GEN_PROP1,      /* Proprietary 1 Event...Map to Anything       */
  GSDI_PERSO_GEN_PROP2,       /* Proprietary 2 Event...Map to Anything       */
  GSDI_PERSO_EVT_INIT_COMPLETED
}gsdi_perso_event_enum_type;

/*----------------------------------------------------------------------------
** GSDI_PERSO_ENUM_TYPE
** Identifiers used for each personalization feature.
** -------------------------------------------------------------------------*/
typedef enum
{
  GSDI_PERSO_NW = 0x0,      /* Network Personalization Feature              */
  GSDI_PERSO_NS,            /* Network Subset Personalization Feature       */
  GSDI_PERSO_SP,            /* Service Provider Personalization Feature     */
  GSDI_PERSO_CP,            /* Corporate Personalization Feature            */
  GSDI_PERSO_SIM,           /* SIM/USIM Personalization Feature             */
  GSDI_PERSO_PROP1,         /* Proprietary Lock 1                           */

  GSDI_PERSO_RUIM_NW1,      /* Network1 Personalization Feature             */
  GSDI_PERSO_RUIM_NW2,      /* Network2 Personalization Feature             */
  GSDI_PERSO_RUIM_HRPD,     /* HRPD Personalization Feature                 */
  GSDI_PERSO_RUIM_SP,       /* Service Provider Personalization Feature     */
  GSDI_PERSO_RUIM_CP,       /* Corporate Personalization Feature            */
  GSDI_PERSO_RUIM_RUIM,     /* RUIM Personalization Feature                 */

  GSDI_PERSO_NONE           /* No personalization indication feature        */
}gsdi_perso_enum_type;

/*---------------------------------------------------------------------------
** GSDI_PERSO_ADDITIONAL_INFORMATION
** Structure Used to Hold Additional Information for
** the generated events.
** -------------------------------------------------------------------------*/
typedef struct
{
  int32             num_bytes;          /* Num bytes in additional info     */
  uint8 *           buffer_p;           /* Pointer to additional info       */
}gsdi_perso_additional_info_type;
/*~ FIELD gsdi_perso_additional_info_type.buffer_p
    VARRAY LENGTH gsdi_perso_additional_info_type.num_bytes */

/*---------------------------------------------------------------------------
** GSDI_PERSO_CONTROL_KEY_DATA_TYPE
** Structure used to hold a control key
** -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_perso_enum_type  feature;        /* Personalization Feature ID       */
  gsdi_slot_id_type     slot;           /* Slot for control keys            */
  int32                 num_bytes;      /* Number of bytes in the Key       */
  uint8 *               control_key_p;  /* Pointer to buffer with NULL
                                           Terminated control key string    */
}gsdi_perso_control_key_data_type;
/*~ FIELD gsdi_perso_control_key_data_type.control_key_p
    VARRAY GSDI_PERSO_MAX_CK LENGTH gsdi_perso_control_key_data_type.num_bytes */

/*---------------------------------------------------------------------------
** GSDI_PERSO_CHANGE_CONTROL_KEY_DATA_TYPE
** Structure used to hold a control key
** -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_perso_enum_type feature;         /* Personalization Feature ID       */
  gsdi_slot_id_type    slot;            /* Slot for control keys            */
  int32                num_bytes;       /* Number of bytes in the Key       */
  uint8 *              control_key_p;   /* Pointer to buffer with NULL
                                          Terminated control key string    */
  uint8 *              new_control_key_p;/* Pointer to buffer with NULL    */
                                        /* Terminal Control Key to change to*/
}gsdi_perso_change_control_key_data_type;
/*~ FIELD gsdi_perso_change_control_key_data_type.control_key_p
    VARRAY GSDI_PERSO_MAX_CK LENGTH gsdi_perso_change_control_key_data_type.num_bytes */
/*~ FIELD gsdi_perso_change_control_key_data_type.new_control_key_p
    VARRAY LENGTH 0 */

/*---------------------------------------------------------------------------
** GSDI_PERSO_OTA_DEPERSO_TYPE
** Structure used to reference Control Keys passed
** in the SMS PP OTA Depersonalization Data (User Data)
** -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_slot_id_type slot;                /* Slot for which user data is for */
  int32             num_bytes;           /* Num bytes in SMS PP OTA         */
  uint8 *           deperso_user_data_p; /* Pointer to IMEI and Cntl Keys   */
}gsdi_perso_ota_deperso_type;
/*~ FIELD gsdi_perso_ota_deperso_type.deperso_user_data_p
    VARRAY LENGTH gsdi_perso_ota_deperso_type.num_bytes */

/*---------------------------------------------------------------------------
** GSDI_PERSO_SET_FEATURE_DATA_TYPE
** Structure used to contain the personalization "codes"
** to be set in the Mobiles NV or FS.  Requires data be packed
** in the respective format as identified in 31.102 or 11.11.
** -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_perso_enum_type  feature;        /* Personalization Feature ID       */
  gsdi_slot_id_type     slot;           /* Slot for which to direct perso to*/
  int32                 num_bytes;      /* Number of Bytes in Buffer Ptr    */
  uint8 *               perso_data_p;   /* Pointer to Data in Buffer        */
}gsdi_perso_set_feature_data_type;
/*~ FIELD gsdi_perso_set_feature_data_type.perso_data_p
    VARRAY GSDI_MAX_DATA_BLOCK_LEN LENGTH gsdi_perso_set_feature_data_type.num_bytes */

/*---------------------------------------------------------------------------
** GSDI_PERSO_EVENTS_CB
** Structure used to contain the callback for
** Personalization Events.
** -------------------------------------------------------------------------*/
typedef void (*gsdi_perso_events_cb)
  (
    gsdi_perso_event_enum_type event,             /* Personalization Event  */
    gsdi_slot_id_type          slot,              /* Slot id for perso event*/
    boolean additional_info_avail,                /* Indicator to check the
                                                     additional info sturct */
    gsdi_perso_additional_info_type * info        /* Will contain data for
                                                     the event generated    */
  );
/*~ PARAM info POINTER */

/* --------------------------------------------------------------------------
   GSDI_PERSO_SELF_PERSO_STATE
   These are substates of the GSDI_PERSO_NONE ENUM in the GSDI_PERSO_INIT_STATE.
   These substates are used to self personalize each feature
   ------------------------------------------------------------------------*/
typedef enum
{
  GSDI_PERSO_SELF_INIT_STATE          = 0x00,
  GSDI_PERSO_DEACTIVATE_STATE         = 0x01,
  GSDI_PERSO_WAIT_FOR_DEACT_STATE     = 0x02,
  GSDI_PERSO_SAVE_SIM_TO_FS_STATE     = 0x03,
  GSDI_PERSO_ACTIVATE_STATE           = 0x04,
  GSDI_PERSO_WAIT_FOR_ACT_STATE       = 0x05,
  GSDI_PERSO_DONE_STATE               = 0x06,
  GSDI_PERSO_ERROR_STATE              = 0x07
}gsdi_perso_self_perso_state_enum_type;

typedef enum {
  GSDI_GSM_APP = 0x01,
  GSDI_CDMA_APP,
  GSDI_USIM_APP,
  GSDI_MAX_APP = 0xFF
}gsdi_app_enum_type;

typedef struct {
  uint8   aid_len;
  uint8   aid_data[GSDI_MAX_APP_ID_LEN];
} gsdi_app_id_type;

typedef enum
{
    GSDI_REFRESH_MODULE_NONE = 0,
    GSDI_REFRESH_GSDI_CACHE,
    GSDI_REFRESH_CALL_STACK,
    GSDI_REFRESH_PHONEBOOK,
    GSDI_REFRESH_COREAPP,
    GSDI_REFRESH_SK_UI,
    GSDI_REFRESH_WMS,
    GSDI_REFRESH_CM,
    GSDI_REFRESH_MAX = 8
} gsdi_refresh_module_enum_type;

typedef struct {
    uint8   num_of_efs;
    uim_items_enum_type ef[GSDI_REFRESH_MAX_EFS];
    gsdi_refresh_module_enum_type module[GSDI_REFRESH_MAX_EFS];
}gsdi_refresh_ef_module_type;

#if (defined(FEATURE_MMGSDI_CDMA) || defined(PLATFORM_LTK)) && \
  !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS))

typedef struct {
    byte message_set;
    byte message_id;
    byte message_len_lsb;
    byte message_len_msb;
} IMH_T;

typedef struct {
    word     data;
    word     timer_id;
}timer_data_T;


typedef struct{
    IMH_T            IMH;
    byte             timer_id;
    timer_data_T     data;
} timer_expiry_T;
typedef byte gs_queue_id_T;

#endif /*(#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)) */



/* GSDI ASYNCH RESPONSE TYPES */
typedef enum {
    GSDI_NO_RSP = 0x00,
    GSDI_SIM_READ_RSP,
    GSDI_SIM_WRITE_RSP,
    GSDI_SIM_INCREASE_RSP,
    GSDI_RUN_GSM_ALGORITHM_RSP,
    GSDI_SELECT_RSP,
    GSDI_GET_FILE_ATTRIBUTES_RSP,
    GSDI_GET_PIN_STATUS_RSP,
    GSDI_VERIFY_PIN_RSP,
    GSDI_CHANGE_PIN_RSP,

    GSDI_UNBLOCK_PIN_RSP,
    GSDI_DISABLE_PIN_RSP,
    GSDI_ENABLE_PIN_RSP,
    GSDI_INIT_RSP,
    GSDI_GET_SIM_CAPABILITIES_RSP,
    GSDI_ENABLE_FDN_RSP,
    GSDI_DISABLE_FDN_RSP,
    GSDI_ILLEGAL_SIM_RSP,
    GSDI_CALL_STATE_RSP,
    GSDI_SIM_SEARCH_RSP,

    GSDI_USIM_AUTHENTICATION_RSP,
    GSDI_STORE_ESN_RSP,
    GSDI_GET_IMAGE_RSP,
    GSDI_SIM_REFRESH_RSP,
    GSDI_REFRESH_REGISTRATION_VOTE_RSP,
    GSDI_REFRESH_REGISTRATION_FCN_RSP,
    GSDI_REFRESH_COMPLETED_RSP,
    GSDI_SEND_APDU_RSP,
    GSDI_SWITCH_SLOT_PREF_RSP,
    GSDI_OPEN_CHANNEL_RSP,

    GSDI_CLOSE_CHANNEL_RSP,
    GSDI_SEND_CHANNEL_DATA_RSP,
    GSDI_GET_ATR_RSP,
    GSDI_PERSO_REG_TASK_RSP,
    GSDI_PERSO_ACT_IND_RSP,
    GSDI_PERSO_DEACT_IND_RSP,
    GSDI_PERSO_GET_IND_RSP,
    GSDI_PERSO_SET_IND_RSP,
    GSDI_PERSO_SET_DATA_RSP,
    GSDI_PERSO_OTA_DEPERSO_RSP,

    GSDI_PERSO_INTERNAL_PROC_RSP,
    GSDI_PERSO_UNBLOCK_IND_RSP,
    GSDI_PERSO_PERM_DISABLE_IND_RSP,
    GSDI_PERSO_GET_PERM_IND_RSP,
    GSDI_PERSO_GET_FEATURE_KEY_RSP,
    GSDI_PERSO_CHANGE_DCK_RSP,
    GSDI_PERSO_GET_DCK_NUM_RETRIES_RSP,
    GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_RSP,
    GSDI_GET_CPHS_RSP,
    GSDI_GET_ECC_RSP,

    GSDI_GET_PRB_TABLE_RSP,
    GSDI_SIM_ACCESS_RSP,
    GSDI_SECURE_DEVICE_INFO_RSP,
    GSDI_SIM_SECURE_STORE_RSP,
    GSDI_PERSO_ROOT_KEY_WRITE_RSP,
    GSDI_PERSO_ROOT_KEY_READ_RSP,
    GSDI_PERSO_VERIFY_PHONE_CODE_RSP,
    GSDI_PERSO_CHANGE_PHONE_CODE_RSP,
    GSDI_PERSO_UNBLOCK_PHONE_CODE_RSP,
    GSDI_PERSO_GET_PHONE_CODE_STATUS_RSP,

    GSDI_SIM_SECURE_READ_RSP,
    GSDI_PERSO_SET_PHONE_CODE_RSP,
    GSDI_PERSO_GET_LIB_VERSION_RSP,
    GSDI_ACTIVATE_ONCHIP_SIM_RSP,
    GSDI_GET_ALL_PIN_INFO_RSP,
    GSDI_INTERNAL_RSP,
    GSDI_PERSO_GET_DATA_RSP,
    GSDI_MAX_RSP = 0xFF
}gsdi_resp_type;

typedef enum {
    GSDI_SUCCESS = 0,
    GSDI_ERROR,
    GSDI_ACCESS_DENIED,
    GSDI_NOT_FOUND,
    GSDI_INCOMPAT_PIN_STATUS,
    GSDI_INCORRECT_CODE,
    GSDI_CODE_BLOCKED,
    GSDI_INCREASE_IMPOSSIBLE,
    GSDI_INCORRECT_PARAMS,
    GSDI_NOT_SUPPORTED,

    GSDI_NOT_INIT,
    GSDI_SUCCESS_BUT_ILLEGAL_SIM,
    GSDI_AUTH_ERROR_INCORRECT_MAC,
    GSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP,
    GSDI_SIM_TECHNICAL_PROBLEMS,
    GSDI_NO_EF_SELECTED,
    GSDI_EF_INCONSISTENT,
    GSDI_ERROR_NO_EVENT_NEEDED,
    GSDI_PIN_NOT_INITIALIZED,
    GSDI_UNKNOWN_INST_CLASS,

    GSDI_WARNING_NO_INFO_GIVEN,
    GSDI_WARNING_POSSIBLE_CORRUPTION,
    GSDI_INCORRECT_LENGTH,
    GSDI_UIM_CMD_TIMEOUT,
    GSDI_CODE_PERM_BLOCKED,
    GSDI_REFRESH_SUCCESS,
    GSDI_REFRESH_IN_PROGRESS,
    GSDI_CAN_NOT_REFRESH,
    GSDI_PATHS_DECODE_ERROR,
    GSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE,

    GSDI_SIM_BUSY,
    GSDI_INVALIDATION_CONTRADICTION_STATUS,
    GSDI_INCREASE_MAX_REACHED,
    GSDI_AUTH_FAIL,
    GSDI_PERSO_CHECK_FAILED,    /* Personalization Check Failed               */
    GSDI_PERSO_INVALID_CK,      /* Control Key Verification Failed            */
    GSDI_PERSO_DUPLICATE_CK,    /* Attempted to set Control Key already used  */
    GSDI_PERSO_CK_BLOCKED,      /* Wrong Control Key Verification Exceeded Max*/
    GSDI_PERSO_CURRENTLY_ACTIVE,/* Perso already Active, or not deactivated   */
    GSDI_PERSO_CURRENTLY_DEACT, /* Perso is not active                        */

    GSDI_PERSO_INVALID_DATA,    /* Perso Data Not To spec                     */
    GSDI_PERSO_PERM_DISABLED,    /* Perso Feature is Perm Disabled             */
    GSDI_PERSO_UNBLOCK_CK_BLOCKED,
    GSDI_PERSO_PROP1_STATUS,    /* Status to allow customized Prorietary Returns */
    GSDI_PERSO_PROP2_STATUS,    /* Status to allow customized Propritary Returns */
    GSDI_PERSO_PROP3_STATUS,    /* Status to allow customized Propritary Returns */
    GSDI_PERSO_INCONSISTENT_W_IND,
    GSDI_PERSO_SUCCESS_DEL_DCK, /* Status which indicates success & delete DCK   */
    GSDI_CMD_QUEUE_FULL,        /* Command Queue is full */
    GSDI_UIM_RST_INSTN_CHG_SUCCESS, /* A command to reset uim with new
                                       instruction class is successful and
                                       is in progress
                                     */

    GSDI_REFRESH_SUCCESS_BUT_NO_AID_ACTIVE,
    GSDI_REFRESH_FAIL_INVALID_AID,
    GSDI_REFRESH_FCN_ADDITIONAL_FCN_READ,
    GSDI_RPC_ERROR,            /* Error code for ONRPC error */
    GSDI_MAX_RETURN_STATUS = 0x7FFFFFFF
}gsdi_returns_T;


/*---------------------------------------------------------------------------
 ** GSDI_SERVICES_TYPE
 ** This table is in sync with EF SST in TS 51.011.
 **  The last element in EF SST is GSDI_MMS_USR_P.
 ** All new enums for UST and RUIM Services should be appended to the list.

 ** For every new entry in here, a new entry should also be made in
 ** gsdi_usim_service_table. If an equivalent USIM service does not exist, then a
 ** GSDI_NONE should be added to gsdi_usim_service_table.
------------------------------------------------------------------------------*/
typedef enum {
   GSDI_NONE = 0x00,
   GSDI_CHV1_DISABLE,   /* CHV1 disabled function           */
   GSDI_ADN,            /* GSM Abbreviated Dialling Numbers */
   GSDI_FDN,            /* Forbidden Dialling Numbers       */
   GSDI_SMS,            /* Short Message Services           */
   GSDI_AOC,            /* Advice of Charge                 */
   GSDI_CCP,            /* Configuration Capability Params  */
   GSDI_PLMN_SELECTOR,  /* PLMN Selector                    */
   GSDI_RFU1,
   GSDI_MSISDN,         /* MSISDN                           */
   GSDI_EXT1,           /* Extension 1                      */

   GSDI_EXT2,           /* Extension 2                      */
   GSDI_SMSP,           /* SMS Parameters                   */
   GSDI_LND,            /* Last Number Dialled              */
   GSDI_CBMI,           /* Cell Broadcast Message Identifier */
   GSDI_GID1,           /* Group Identifier 1               */
   GSDI_GID2,           /* Group Identifier 2               */
   GSDI_SPN,            /* Service Provider Name            */
   GSDI_SDN,            /* Service Dialling Numbers         */
   GSDI_EXT3,           /* Extension 3                      */
   GSDI_RFU2,

   GSDI_VGCS_GID_LIST,  /* VGCS Group Identifier List       */
   GSDI_VBS_GID_LIST,   /* VBS Group Identifier List        */
   GSDI_ENH_ML_SVC,     /* Enhanced Multi Level precedence and Premption Service */
   GSDI_AA_EMLPP,       /* Automatic Answer for eMLPP       */
   GSDI_DATA_DL_SMSCB,  /* Data download via SMS-CB         */
   GSDI_DATA_DL_SMSPP,  /* Data download via SMS-PP         */
   GSDI_MENU_SEL,       /* Menu Selection                   */
   GSDI_CALL_CONTROL,   /* Call Control                     */
   GSDI_PROACTIVE_SIM,  /* Proactive SIM                    */
   GSDI_CBMID_RANGES,   /* Cell Broadcast Message Identifiers */

   GSDI_BDN,            /* Barred Dialing Numbers           */
   GSDI_EXT4,           /* Extension 4                      */
   GSDI_DEPERSON_KEYS,  /* Depersonalization Keys           */
   GSDI_COOP_NETWK_LIST,/* Co-operative Network List        */
   GSDI_SMSR,           /* Short Message Status Reports     */
   GSDI_NETWK_ALTERING_MS, /* Network's Indication of alertin in the MS */
   GSDI_MO_SMS_BY_SIM,  /* Mobile Originated Short Message control by SIM */
   GSDI_GPRS,           /* GPRS                             */
   GSDI_IMAGE,          /* Image (IMG)                      */
   GSDI_SOLSA,          /* Support of Local Service Area    */

   GSDI_USSD,           /* USSD String Data object supported in Call Control */
   GSDI_RUN_AT_CMD,     /* RUN AT COMMAND command           */
   GSDI_UPLMN_SEL_WACT,  /* User controlled PLMN Selector with Access Technology */
   GSDI_OPLMN_SEL_WACT,  /* Operator controlled PLMN Selector with Access Technology */
   GSDI_HPLMN_WACT,      /* HPLMN Selector with Access Technology */
   GSDI_CPBCCH,          /* CP BCCH */
   GSDI_INV_SCAN,        /* Investigation Scan */
   GSDI_EXT_CCP,         /* extended capability configuration param - in GSM */
   GSDI_MEXE,            /* MExE info */
   GSDI_RPLMN_LACT,      /* RPLMN Last used Access Technology */

   GSDI_PLMN_NTWRK_NAME, /* PLMN Network Name */
   GSDI_OPLMN_LIST,      /* Operator PLMN list */
   GSDI_MAILBOX_DIAL,    /* Mailbox dialling numbers */
   GSDI_MSG_WAIT,        /* Message Wait indication*/
   GSDI_CALL_FWD_IND,    /* Call Forward indication status */
   GSDI_SP_DISP_INFO,    /* Service Provider Display Information */
   GSDI_MMS,             /* Multimedia Messaging Service */
   GSDI_EXT8,            /* Extension 8 */
   GSDI_MMS_USR_P,       /* MMS User Connectivity Parameters */

   GSDI_MOSMS_CONTROL,   /* MO SMS control                   */
   GSDI_LOCAL_PHONEBOOK, /* Local Phonebook, UADF/Phonebook/ADN */
   GSDI_GLOBAL_PHONEBOOK,/* Global Phonebook on USIM (always present) */
   GSDI_OCI_OCT,         /* Outgoing Call information/timer */
   GSDI_ICI_ICT,         /* Incoming call information/timer */
   GSDI_GSM_ACCESS_IN_USIM, /* USIM ADF Access to USIM files for GSM Roaming */
   GSDI_EST,             /* Enable Service Table in USIM */
   GSDI_ACL,             /* APN Control list */
   GSDI_GSM_SECURITY_CONTEXT, /* GSM Security Context */
   GSDI_EXT5,            /* Ext 5 */

   GSDI_GPRS_USIM,       /* Call control on GPRS by USIM */
   GSDI_EHPLMN,          /* EHPLMN */
   GSDI_NIA,             /* Network's indication of alerting in the MS */
   GSDI_PSEUDONYM,       /* Pseudonmy */
   GSDI_UPLMN_WLAN_ACC,  /* User Controlled PLMNselector for WLAN access */
   GSDI_OPLMN_WLAN_ACC,  /* Opeator Controlled PLMN selector for WLAN access */
   GSDI_USER_CTRL_WSID,  /* User controlled WSID list */
   GSDI_OPER_CTRL_WSID,  /* Operator controlled WSID list */
   GSDI_VGCS_SECURITY,   /* VGCS Security */
   GSDI_VBS_SECURITY,    /* VBS Security */

   GSDI_WLAN_REAUTH_ID,  /* WLAN Reauthentication Identity */
   GSDI_MM_STORAGE,      /* Multimedia Messaging Storage */
   GSDI_GBA,             /* Generic Bootstrapping Architecture */
   GSDI_MBMS_SECURITY,   /* MBMS security */
   GSDI_DATA_DL_USSD,    /* Data Downlod via USSD and USSD application mode */
   GSDI_TP_AFTER_UICC_ACT, /* additional TERMINAL PROFILE after UICC activation */

   GSDI_MAX_SVR_RSP,
   GSDI_MAX_SERVICE = 0x7FFFFFFF

   /* Non Standard Service Checks */


}gsdi_services_type;


typedef struct {
     /* bit    7 - 5         4          3           2          1          0       */
     /*         rfu   USIM svr avail   rfu   RUIM svr avail   rfu   GSM svr avail */
  byte            svr_bitmap;
  gsdi_returns_T  gsdi_status;  /* GSDI_SUCCESS or GSDI_INCORRECT_PARAM
                                   or error status returned from get_card_mode()*/
}
gsdi_svr_rsp_type;


typedef enum {
    KEYREF_ALWAYS = 0x00,
    PIN_APPL_1 = 0x01,
    PIN_APPL_2 = 0x02,
    PIN_APPL_3 = 0x03,
    PIN_APPL_4 = 0x04,
    PIN_APPL_5 = 0x05,
    PIN_APPL_6 = 0x06,
    PIN_APPL_7 = 0x07,
    PIN_APPL_8 = 0x08,
    ADM1 = 0x0A,
    ADM2 = 0x0B,
    ADM3 = 0x0C,
    ADM4 = 0x0D,
    ADM5 = 0x0E,
    UNIVERSAL_PIN = 0x11,
    SEC_PIN_APPL_1 = 0x81,
    SEC_PIN_APPL_2 = 0x82,
    SEC_PIN_APPL_3 = 0x83,
    SEC_PIN_APPL_4 = 0x84,
    SEC_PIN_APPL_5 = 0x85,
    SEC_PIN_APPL_6 = 0x86,
    SEC_PIN_APPL_7 = 0x87,
    SEC_PIN_APPL_8 = 0x88,
    ADM6 = 0x8A,
    ADM7 = 0x8B,
    ADM8 = 0x8C,
    ADM9 = 0x8D,
    ADM10 = 0x8E,
    NEVER = 0xFF
}key_ref_values_T;

typedef enum {
    CHV_NONE = 0x00,
    CHV1 = 0x01,
    CHV2 = 0x02,
    GSM_ADM1 = 0x04,
    GSM_ADM2 = 0x05,
    GSM_ADM3 = 0x06,
    GSM_ADM4 = 0x07,
    GSM_ADM5 = 0x08,
    GSM_ADM6 = 0x09,
    GSM_ADM7 = 0x0A,
    GSM_ADM8 = 0x0B,
    GSM_ADM9 = 0x0C,
    GSM_ADM10 = 0x0D,
    GSM_ADM11 = 0x0E,
    GSM_NEVER = 0x0F,
    CHV_MAX = 0xFF
}chv_ref_values_T;


typedef struct {
  boolean  present;
  boolean  multiple_pbr_entries;
  boolean  status_unknown;
}
phone_book_T;

typedef struct {
  boolean  fdn_enabled;
  boolean  bdn_enabled;
  boolean  acl_enabled;
  boolean  imsi_invalidated;
  phone_book_T sim_phone_book;
  phone_book_T usim_global_phone_book;
  phone_book_T usim_local_phone_book;
  boolean  unknown_capabilities;
}
sim_capabilities_T;

/* Strucuture represents fields of ECC information */
typedef struct
{
  byte emer_call_code[GSDI_ECC_LEN];
  byte alpha_id[GSDI_ALPHA_ID_LEN];
  byte emer_srvc_category;
}gsdi_ecc_info_T;

/* ECC table containing rows of ECC information */
typedef struct
{
  int32              num_of_rec;
  gsdi_ecc_info_T    *ecc_p[GSDI_MAX_ECC_RECORDS];
  /*~ FIELD gsdi_ecc_info_table_type.ecc_p
      VARRAY GSDI_MAX_ECC_RECORDS LENGTH gsdi_ecc_info_table_type.num_of_rec
      gsdi_ecc_info_table_type.ecc_p[] POINTER */
}gsdi_ecc_info_table_type;

/*
** CONTEXT Parameter for 3G Authentication Request
*/
typedef enum {
    GSDI_GSM_CONTEXT  = 0,
    GSDI_3G_CONTEXT   = 1
} authentication_context_T;



typedef enum {
    GSDI_CARD_MODE_UNKNOWN = 0,
    GSDI_CARD_USIM         = 1,
    GSDI_CARD_GSM          = 2,
    GSDI_CARD_GSM_USIM     = 3,
    GSDI_CARD_RUIM         = 4,
    GSDI_CARD_GSM_RUIM     = 5,
    GSDI_CARD_EMV          = 6,
    GSDI_CARD_MAX          = 7
}gsdi_card_mode_capability_T;


typedef struct {
    boolean slot1_present;          /* TRUE Slot has Card Inserted */
    uint8   apps_enabled;           /* BITMAP Of Apps Available    */
    boolean slot2_present;          /* TRUE Slot has Card Inserted */
    uint8   apps2_enabled;          /* BITMAP Of Apps Available    */
    uint8   operational_slot;       /* Bits [0-1] -> Slot Number for GSM App */
                                    /* Bits [2-3] -> Slot Number for CDMA App */
    boolean onchip_sim_active;      /* Whether the FTM sim is active or not */
}
gsdi_card_apps_T;

typedef struct {
  gsdi_slot_id_type user_switch_gsm_app_pref;
  gsdi_slot_id_type user_switch_cdma_app_pref;
  gsdi_slot_id_type gsm_app_pref;
  gsdi_slot_id_type cdma_app_pref;
}gsdi_slot_pref_type;


/*
** SIM LANGUAGES - see GSM 03.38 section 5.
*/
typedef byte gsdi_language_T;
enum {
    GSDI_GERMAN            = 0,
    GSDI_ENGLISH           = 1,
    GSDI_ITALIAN           = 2,
    GSDI_FRENCH            = 3,
    GSDI_SPANISH           = 4,
    GSDI_DUTCH             = 5,
    GSDI_SWEDISH           = 6,
    GSDI_DANISH            = 7,
    GSDI_PORTUGUESE        = 8,
    GSDI_FINNISH           = 9,
    GSDI_NORWEGIAN         = 10,
    GSDI_GREEK             = 11,
    GSDI_TURKISH           = 12,
    GSDI_LANGUAGE_13       = 13,
    GSDI_LANGUAGE_14       = 14,
    GSDI_UNKNOWN_LANGUAGE  = 15
};


/*
** EVENTS FROM THE SIM
*/
typedef enum {
    GSDI_SIM_INSERTED = 0,
    GSDI_SIM_REMOVED,
    GSDI_DRIVER_ERROR,
    GSDI_CARD_ERROR,
    GSDI_MEMORY_WARNING,
    GSDI_NO_SIM_EVENT,
    GSDI_NO_SIM,
    GSDI_SIM_INIT_COMPLETED,
    GSDI_SIM_INIT_COMPLETED_NO_PROV,
    GSDI_PIN1_VERIFIED,

    GSDI_PIN1_BLOCKED,
    GSDI_PIN1_UNBLOCKED,
    GSDI_PIN1_ENABLED,
    GSDI_PIN1_DISABLED,
    GSDI_PIN1_CHANGED,
    GSDI_PIN1_PERM_BLOCKED,
    GSDI_PIN2_VERIFIED,
    GSDI_PIN2_BLOCKED,
    GSDI_PIN2_UNBLOCKED,
    GSDI_PIN2_ENABLED,

    GSDI_PIN2_DISABLED,
    GSDI_PIN2_CHANGED,
    GSDI_PIN2_PERM_BLOCKED,
    GSDI_NO_EVENT,
    GSDI_OK_FOR_TERMINAL_PROFILE_DL,
    GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL,
    GSDI_REFRESH_SIM_RESET,
    GSDI_REFRESH_SIM_INIT,
    GSDI_FDN_ENABLE,
    GSDI_FDN_DISABLE,

    GSDI_SIM_INSERTED_2,
    GSDI_SIM_REMOVED_2,
    GSDI_CARD_ERROR_2,
    GSDI_MEMORY_WARNING_2,
    GSDI_NO_SIM_EVENT_2,
    GSDI_SIM_INIT_COMPLETED_2,
    GSDI_SIM_INIT_COMPLETED_NO_PROV_2,
    GSDI_PIN1_VERIFIED_2,
    GSDI_PIN1_BLOCKED_2,
    GSDI_PIN1_UNBLOCKED_2,

    GSDI_PIN1_ENABLED_2,
    GSDI_PIN1_DISABLED_2,
    GSDI_PIN1_CHANGED_2,
    GSDI_PIN1_PERM_BLOCKED_2,
    GSDI_PIN2_VERIFIED_2,
    GSDI_PIN2_BLOCKED_2,
    GSDI_PIN2_UNBLOCKED_2,
    GSDI_PIN2_ENABLED_2,
    GSDI_PIN2_DISABLED_2,
    GSDI_PIN2_CHANGED_2,

    GSDI_PIN2_PERM_BLOCKED_2,
    GSDI_REFRESH_SIM_RESET_2,
    GSDI_REFRESH_SIM_INIT_2,
    GSDI_FDN_ENABLE_2,
    GSDI_FDN_DISABLE_2,
    GSDI_OK_FOR_TERMINAL_PROFILE_DL_2,
    GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2,
    GSDI_REFRESH_SIM_INIT_FCN,
    GSDI_REFRESH_SIM_INIT_FCN_2,
    GSDI_REFRESH_FCN,

    GSDI_REFRESH_FCN_2,
    GSDI_REFRESH_FAILED,
    GSDI_START_SWITCH_SLOT,
    GSDI_FINISH_SWITCH_SLOT,
    GSDI_PERSO_INIT_COMPLETED,
    GSDI_PERSO_EVENT_GEN_PROP1,
    GSDI_SIM_ILLEGAL,
    GSDI_SIM_ILLEGAL_2,
    GSDI_SIM_UHZI_V2_COMPLETE,
    GSDI_SIM_UHZI_V1_COMPLETE,

    GSDI_PERSO_EVENT_GEN_PROP2,
    GSDI_INTERNAL_SIM_RESET,
    GSDI_CARD_ERR_POLL_ERROR,
    GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE,
    GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET,
    GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES,
    GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES,
    GSDI_CARD_ERR_VOLTAGE_MISMATCH,
    GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS,
    GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS,

    GSDI_CARD_ERR_MAXED_PARITY_ERROR,
    GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR,
    GSDI_CARD_ERR_MAXED_OVERRUN_ERROR,
    GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED,
    GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION,
    GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE,
    GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE,
    GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE,
    GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE,
    GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE,

    GSDI_CARD_ERR_POLL_ERROR_2,
    GSDI_CARD_ERR_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_2,
    GSDI_CARD_ERR_NO_ATR_RECEIVED_AFTER_INT_RESET_2,
    GSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES_2,
    GSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES_2,
    GSDI_CARD_ERR_VOLTAGE_MISMATCH_2,
    GSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS_2,
    GSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_2,
    GSDI_CARD_ERR_MAXED_PARITY_ERROR_2,
    GSDI_CARD_ERR_MAXED_RX_BREAK_ERROR_2,

    GSDI_CARD_ERR_MAXED_OVERRUN_ERROR_2,
    GSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED_2,
    GSDI_CARD_ERR_POWER_DOWN_CMD_NOTIFICATION_2,
    GSDI_CARD_ERR_INT_CMD_ERR_IN_PASSIVE_MODE_2,
    GSDI_CARD_ERR_CMD_TIMED_OUT_IN_PASSIVE_MODE_2,
    GSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE_2,
    GSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE_2,
    GSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE_2,
    GSDI_REFRESH_APP_RESET,
    GSDI_REFRESH_3G_SESSION_RESET,

    GSDI_REFRESH_APP_RESET_2,
    GSDI_REFRESH_3G_SESSION_RESET_2,
    GSDI_APP_SELECTED,
    GSDI_APP_SELECTED_2,
    GSDI_MAX_SIM_EVENTS
}gsdi_sim_events_T;


/* GSDI REFRESH MODES */
typedef enum
{
    /* Note, the value of the GSDI enum is arranged
       according to specification */
    GSDI_SIM_INIT_AND_FULL_FILE_CHANGE = 0,
    GSDI_FILE_CHANGE_NOTIFICATION,
    GSDI_SIM_INIT_AND_FILE_CHANGE,
    GSDI_SIM_INIT,
    GSDI_SIM_RESET,
    GSDI_USIM_APP_RESET,
    GSDI_3G_SESSION_RESET,

    GSDI_REFRESH_MODE_NONE,
    GSDI_MAX_REFRESH_MODES
} gsdi_refresh_modes_enum_type;

/*
** MODES OF INDEXING RECORDS WITHIN FILES
*/
typedef enum {
    GSDI_NO_INDEX = 0,
    GSDI_INDEX_ABSOLUTE,
    GSDI_INDEX_CURRENT,
    GSDI_INDEX_NEXT,
    GSDI_INDEX_PREVIOUS,
    GSDI_SEARCH_FWD_FROM_START,
    GSDI_SEARCH_FWD_FROM_NEXT,
    GSDI_SEARCH_BACK_FROM_END,
    GSDI_SEARCH_BACK_FROM_PREV,

    GSDI_MAX_INDEX_MODES
}gsdi_index_modes_T;

/*
** PIN IDs
*/
typedef enum {
    /* FOR GSDI Backward compatibility */
    GSDI_PIN1 = 0,
    GSDI_PIN2 = 1,
    GSDI_GSM_CHV1 = GSDI_PIN1,
    GSDI_GSM_CHV2 = GSDI_PIN2,
    GSDI_RUIM_CHV1 = GSDI_PIN1,
    GSDI_RUIM_CHV2 = GSDI_PIN2,
    GSDI_USIM_UNIV_PIN = 2,
    GSDI_USIM_APP1_PIN1 = 3,
    GSDI_USIM_APP1_PIN2 = 4,
    GSDI_USIM_APP2_PIN1 = 5,
    GSDI_USIM_APP2_PIN2 = 6,
    GSDI_USIM_APP3_PIN1 = 7,
    GSDI_USIM_APP3_PIN2 = 8,
    GSDI_USIM_APP4_PIN1 = 9,
    GSDI_USIM_APP4_PIN2 = 10,

    GSDI_MAX_PIN_ID
}gsdi_pin_ids_T;


/*
** STATUS OF PIN
*/
typedef enum {
    GSDI_PIN_NOT_INITIALISED = 0,
    GSDI_PIN_DISABLED,
    GSDI_PIN_ENABLED,
    GSDI_PIN_BLOCKED,
    GSDI_PIN_PERM_BLOCKED,

    GSDI_MAX_PIN_STATUS
}gsdi_pin_status_T;


/*
** SIM FILE PROTECTION
*/
typedef struct {
    byte     read;
    byte     write;
    byte     increase;
    byte     invalidate;
    byte     rehabilitate;
}
gsdi_file_protection_T;


/*
** SIM FILE TYPES
*/
typedef enum {
    GSDI_MASTER_FILE     = 0x00000001,
    GSDI_DEDICATED_FILE  = 0x00000002,
    GSDI_ELEMENTARY_FILE = 0x00000004
}gsdi_file_types_T;

/*
** SIM FILE STRUCTURE
*/
typedef enum {
    GSDI_TRANSPARENT_FILE = 0x00000000,
    GSDI_LINEAR_FILE      = 0x00000001,
    GSDI_CYCLIC_FILE      = 0x00000003
}gsdi_file_structures_T;

/*
** SIM/USIM SEARCH MODES
*/
typedef enum {
    GSDI_NO_SUCH_SEARCH,
    GSDI_SIMPLE_SEARCH,
    GSDI_ENHANCED_SEARCH
} gsdi_search_modes_T;

/*
** SIM/USIM SEARCH OFFSET TYPES
*/
typedef enum {
   GSDI_SEARCH_ABSOLUTE,
   GSDI_SEARCH_CHARACTER
} gsdi_search_offset_type_T;


/*
** SIM FILE ATTRIBUTES
*/
typedef struct {
    int                       file_size;
    byte                      file_id[ GSDI_FILE_ID_LEN ];
    gsdi_file_types_T         file_type;
    boolean                   cyclic_increase_allowed;
    gsdi_file_protection_T    file_protection;
    boolean                   file_invalidated;
    gsdi_file_structures_T    file_structure;
    byte                      rec_len;
    byte                      num_of_records;
    boolean                   rw_invalidated_deactivated_allowed;
}
gsdi_file_attributes_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PBR_MAPPING_ENUM_TYPE
   DESCRIPTION: Will be used to describe the mapping available for the
                files in the Phonebook Reference File as described in
                31.102.  The mapping will be as it pertains to the Master EF
                Type1:   Record to Record Mapping
                Type2:   Requires usage of IAP
                Type3:   Linked by Record Identifier
   --------------------------------------------------------------------------*/
typedef enum {
    GSDI_MAPPING_UNKNOWN = 0x00000000,
    GSDI_MAPPING_TYPE_1,
    GSDI_MAPPING_TYPE_2,
    GSDI_MAPPING_TYPE_3  = 0x00000003
}gsdi_pbr_mapping_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PBR_TABLE_ROW_TYPE
   DESCRIPTION: Will describe the row elements used to build the PBR
                table returned to the client.
                file_name:     Filename of the Item type used
                file_mapping:  Type 1,2,or 3
                uint32:        File Shore Identifier
                byte *         path as it should exist in the UICC Telecom or
                               USIM ADF
   --------------------------------------------------------------------------*/
typedef struct {
    uint32                   file_name;      /* Client to cast to uim_items_enum_type*/
    gsdi_pbr_mapping_enum_type file_mapping; /* 1 to 1 or pointer, etc               */
    uint32                   iap_order;      /* Order after IAP File */
    uint32                   file_sfid;      /* Short file id                        */
    int32                    path_len;       /* Len of Path to follow */
    uint8 *                  path_p;         /* Pointer to the Path                  */
    /*~ FIELD gsdi_pbr_table_row_type.path_p VARRAY LENGTH gsdi_pbr_table_row_type.path_len */
}gsdi_pbr_table_row_type;



/* Used as a command type in CM */
typedef struct {
    uint8               num_files;
    uim_items_enum_type file_list[10];
}gsdi_refresh_fcn_ind_type;

/*
** Function Prototype for Callback Function
** About the parameters:
** FILE_LIST_P -> Pointer to an array of files
** NUM_FILES   -> Number of files contained in array
** REQ_TO_TAKE_DOWN_CALL_STACK --> Indication that
**                MMGSDI would like to take down the
**                call stack.
** OK_TO_TAKE_DOWN_CALL_STACK_P --> Pointer to boolean set by
**                client and used by MMGSDI to determine whether
**                or not it is ok to take down the call stack.
** OK_TO_DO_FILE_NOTIFIYS--> Pointer to boolean set by client
**               and used by MMGSDI to determine whether or not
**               it should do the file notifications.  By default,
**               you should return TRUE.
**
*/
typedef void gsdi_refresh_vote_cb_type (
  uim_items_enum_type * file_list_p,
  uint8                 num_files,
  boolean               req_to_take_down_call_stack,
  boolean             * ok_to_take_down_call_stack_p,
  boolean             * ok_to_do_file_notify
);

/*
** Function Prototype for Callback Function
** About the Parameters:
** FILE_LIST_P  -> Pointer to an array of files
** NUM_FILES    -> Number of files contained in array
*/
typedef void gsdi_task_notify_file_list_cb_type(
  uim_items_enum_type  * file_list_p,
  uint8                 num_files);
/*~ PARAM file_list_p VARRAY LENGTH num_files */

typedef enum
{
  GSDI_PERSO_PHONECODE_NOT_INIT = 0x00000000,
  GSDI_PERSO_PHONECODE_INIT,
  GSDI_PERSO_PHONECODE_MAX_ENUM = 0x7FFFFFFF
}gsdi_perso_pc_enum_status;




/* ----------------------------------------------------------------------------

                GSDI CONFIRMATION / RESPONSE MESSAGES

   ---------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------
   GSDI_RSPHDR_T
   General response message header.
   --------------------------------------------------------------------------*/
typedef struct {
    byte               message_set;
    byte               message_id;
    byte               message_len_lsb;
    byte               message_len_msb;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    rex_tcb_type       *tcb_ptr;
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    rex_sigs_type      wait_sig;
#ifndef FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
    rex_crit_sect_type *crit_sec;
#endif /* FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC */
    gsdi_returns_T     gsdi_status;   /* Status of Command              */
    uint32             client_ref;    /* User Data assigned by client   */
    gsdi_resp_type     resp_type;     /* Type of Response               */
    gsdi_slot_id_type  slot;          /* Slot for which rsp applies     */
}
gsdi_rsphdr_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_GET_PBR_REF_TABLE_CNF_T
** DESCRIPTION: Used to return the GSDI PBR TABLE to the requesting client
** --------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T             message_header;
  int32                     num_rows;
  gsdi_pbr_table_row_type * pbr_tbl_p;
  /*~ FIELD gsdi_get_pbr_table_cnf_T.pbr_tbl_p VARRAY LENGTH gsdi_get_pbr_table_cnf_T.num_rows */
}gsdi_get_pbr_table_cnf_T;

/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_ACTIVATE_ONCHIP_SIM_CNF_T

   DESCRIPTION: Confirmation / Response for the
                GSDI_ACTIVATE_ONCHIP_SIM_REQ_T.
                Command used to have the GSDI Task enable the ONCHIP SIM
                procedures to allow activation of the Handset and Call Stacks
                without a SIM/USIM Inserted in the mobile.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
}gsdi_activate_onchip_sim_cnf_T;


/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_PIN_INFO_ITEM_T

   DESCRIPTION: Confirmation / Response for the
                GSDI_GET_ALL_PIN_INFO_REQ_T
                describes each record for the response
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_pin_ids_T      pin_id;
  gsdi_pin_status_T   status;
  byte                pin_retries_allowed;
  byte                puk_retries_allowed;
}gsdi_pin_info_item_T;


/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_GET_ALL_PIN_INFO_CNF_T

   DESCRIPTION: Confirmation / Response for the
                GSDI_GET_ALL_PIN_INFO_REQ_T
                command used requesting the information
                about all the available pins.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
  uint32                      num_pins;
  gsdi_pin_info_item_T      * pin_info_items;
}gsdi_get_all_pin_info_cnf_T;
/*~ FIELD gsdi_get_all_pin_info_cnf_T.pin_info_items
    VARRAY LENGTH gsdi_get_all_pin_info_cnf_T.num_pins */

/* ----------------------------------------------------------------------------
   GSDI_PERSO_REG_TASK_CNT_T
   Confirmation/Response for the GSDI_PERSO_REG_TASK_REQ_T
   Request/Command.  Indicates whether or not the task was
   successfully registered.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
}gsdi_perso_reg_task_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_ACT_FEATURE_IND_CNF_T
   Confirmation / Response for the GSDI_PERSO_ACT_FEATURE_IND_REQ_T
   Request / Command.  Indicates whether or not the Personalization
   Feature was properly activated
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    gsdi_perso_enum_type        perso_feature;  /* Feature operated on       */
    uint32                      num_retries;      /* Num bytes for CK          */
}gsdi_perso_act_feature_ind_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_GET_FEATURE_IND_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_FEATURE_IND_REQ_T
   Request / Command.  Will return the current status of all
   Personalization Features.
   NW Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   NS Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   SP Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   CP Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   SIM Indicator -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    boolean                     nw_ind_status;  /* Network Indicator Status  */
    boolean                     ns_ind_status;  /* NW Subset Indicator Status*/
    boolean                     sp_ind_status;  /* Servic Provider Ind Status*/
    boolean                     cp_ind_status;  /* Corporate Ind Status      */
    boolean                     sim_ind_status; /* SIM Indicator Status      */

    boolean                     ruim_nw1_ind_status;  /* Network Indicator Status  */
    boolean                     ruim_nw2_ind_status;  /* NW Subset Indicator Status*/
    boolean                     ruim_hrpd_ind_status;  /* NW Subset Indicator Status*/
    boolean                     ruim_sp_ind_status;  /* Servic Provider Ind Status*/
    boolean                     ruim_cp_ind_status;  /* Corporate Ind Status      */
    boolean                     ruim_ruim_ind_status; /* SIM Indicator Status      */
} gsdi_perso_get_feature_ind_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_PERSO_DACT_FEATURE_IND_CNF_T
   Confirmation / Response for the GSDI_PERSO_DEACT_FEATURE_IND_REQ_T
   Request / Command.  Will return whether or not the depersonalization
   deactivation request was successful or not.  It will also indicate the
   number of retries remaining.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    gsdi_perso_enum_type        perso_feature;  /* Feature operated on       */
    uint32                      num_retries;    /* Number of retries left    */
}gsdi_perso_deact_feature_ind_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_SET_FEATURE_DATA_CNF_T
   Confirmation / Response for the GSDI_PERSO_SET_FEATURE_DATA_REQ_T
   Request / Command.  Will return whether or not the setting of the
   personalization data was successful or not.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T     message_header;
}gsdi_perso_set_feature_data_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_GET_FEATURE_DATA_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_FEATURE_DATA_REQ_T
   Request / Command.  Will return whether or not get of
   personalization data was successful or not.
   --------------------------------------------------------------------------*/
typedef struct
{
     gsdi_rsphdr_T     message_header;
     int32             num_bytes;     /* Number of bytes to follow */
     uint8 *           data_buffer_ptr;   /* Personalization Data*/
}gsdi_perso_get_feature_data_cnf_T;
/*~ FIELD gsdi_perso_get_feature_data_cnf_T.data_buffer_ptr
    VARRAY LENGTH gsdi_perso_get_feature_data_cnf_T.num_bytes */

typedef struct
{
     gsdi_rsphdr_T               message_header;/* Message Header            */
     int32                       num_bytes;     /* Number of bytes to follow */
                                                /* 20 Byte return expected   */
     uint8 *                     data_buffer;   /* Personalization Indicators*/

}gsdi_perso_ota_deperso_cnf_T;
/*~ FIELD gsdi_perso_ota_deperso_cnf_T.data_buffer
    VARRAY LENGTH gsdi_perso_ota_deperso_cnf_T.num_bytes */

/* ----------------------------------------------------------------------------
   STRUCT:  gsdi_perso_int_init_procedure_cnf_T
            This structure is used as a confirmation for the request
            gsdi_perso_int_init_procedure_req_T.  The feature returned is used
            to base which personalization check to perform next.
   --------------------------------------------------------------------------*/
typedef struct {
     gsdi_rsphdr_T                         message_header;/* Message Header      */
     gsdi_perso_enum_type                  feature;       /* Feature for Conf    */
     gsdi_perso_self_perso_state_enum_type int_init_s;    /* Internal Init State */
     gsdi_perso_enum_type                  int_init_feature; /* Feature to self perso */
}gsdi_perso_int_init_procedure_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_UNBLOCK_FEATURE_IND_CNF_T
   Confirmation / Response for the GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ_T
   Request / Command.  Indicates whether or not the Personalization
   Feature was properly unblock
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    gsdi_perso_enum_type        perso_feature;  /* Feature operated on       */
    uint32                      num_retries;    /* Num bytes for CK          */
}gsdi_perso_unblock_feature_ind_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DISABLE_FEATURE_IND_CNF_T
   Confirmation / Response for the GSDI_PERSO_DISABLE_FEATURE_IND_REQ_T
   Request / Command.  Indicates whether or not the Personalization
   Feature was properly disabled
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    gsdi_perso_enum_type        perso_feature;  /* Feature operated on       */
    uint32                      num_retries;    /* Num bytes for CK          */
}gsdi_perso_perm_disable_feature_ind_cnf_T;


/* ----------------------------------------------------------------------------
   GSDI_PERSO_GET_PERM_FEATURE_IND_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_PERM_FEATURE_IND_REQ_T
   Request / Command.  Will return the current status of all
   Personalization Features.
   NW Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   NS Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   SP Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   CP Indicator  -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   SIM Indicator -> TRUE / FALSE (ACTIVE / NOT ACTIVE)
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    boolean                     perm_nw_ind_status;  /* Network Indicator Status  */
    boolean                     perm_ns_ind_status;  /* NW Subset Indicator Status*/
    boolean                     perm_sp_ind_status;  /* Servic Provider Ind Status*/
    boolean                     perm_cp_ind_status;  /* Corporate Ind Status      */
    boolean                     perm_sim_ind_status; /* SIM Indicator Status      */

    boolean                     perm_ruim_nw1_ind_status;  /* Network Indicator Status  */
    boolean                     perm_ruim_nw2_ind_status;  /* NW Subset Indicator Status*/
    boolean                     perm_ruim_hrpd_ind_status;  /* NW Subset Indicator Status*/
    boolean                     perm_ruim_sp_ind_status;  /* Servic Provider Ind Status*/
    boolean                     perm_ruim_cp_ind_status;  /* Corporate Ind Status      */
    boolean                     perm_ruim_ruim_ind_status; /* SIM Indicator Status      */

} gsdi_perso_get_perm_feature_ind_cnf_T;


/* ----------------------------------------------------------------------------
   GSDI_PERSO_GET_FEATURE_KEY_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_FEATURE_KEY_REQ_T
   Request / Command.  Will return the control key for the feature
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    gsdi_perso_enum_type        feature;        /* Feature operated on       */
    int32                       key_len;
    uint8                       key[GSDI_PERSO_MAX_KEY_IMEI_LEN];
} gsdi_perso_get_feature_key_cnf_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_CHANGE_DCK_CNF_T
   Confirmation / Response for the GSDI_PERSO_CHANGE_DCK_REQ_T.  This will
   indicate what the status is for the change request, the feature operated on
   and the number of retries remaining for that feature.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    gsdi_perso_enum_type        perso_feature;  /* Feature operated on       */
    int32                       num_retries;      /* Num bytes for CK        */
}gsdi_perso_change_dck_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_GET_DCK_NUM_RETRIES_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_NUM_RETRIES_REQ_T.  This will
   get the number of incorrect DCK Verifications still remaining.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */

  int32                       nw_num_retries;
  int32                       ns_num_retries;
  int32                       sp_num_retries;
  int32                       cp_num_retries;
  int32                       sim_num_retries;

  int32                       ruim_nw1_num_retries;
  int32                       ruim_nw2_num_retries;
  int32                       ruim_hrpd_num_retries;
  int32                       ruim_sp_num_retries;
  int32                       ruim_cp_num_retries;
  int32                       ruim_ruim_num_retries;
}gsdi_perso_get_dck_num_retries_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ_T.  This will
   get the number of incorrect DCK UNBLOCK Verifications still remaining.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */

  int32                       unblock_nw_num_retries;
  int32                       unblock_ns_num_retries;
  int32                       unblock_sp_num_retries;
  int32                       unblock_cp_num_retries;
  int32                       unblock_sim_num_retries;

  int32                       ruim_unblock_nw1_num_retries;
  int32                       ruim_unblock_nw2_num_retries;
  int32                       ruim_unblock_hrpd_num_retries;
  int32                       ruim_unblock_sp_num_retries;
  int32                       ruim_unblock_cp_num_retries;
  int32                       ruim_unblock_ruim_num_retries;
}gsdi_perso_get_dck_unblock_num_retries_cnf_T;

/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_PHONECODE_STATUS_CNF_T

   DESCRIPTION: Confirmation / Response for the
                GSDI_PERSO_GET_PHONECODE_STATUS_REQ_T.
                Indicate whether the Phonecode is Init or not Init
                int32 status     :  GSDI_PERSO_PHONECODE_NOT_INIT  0x00000000
                                 :  GSDI_PERSO_PHONECODE_INIT      0x00000001
                int32 num_retries:  Will indicate the number of attempts
                                    remaining
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
  gsdi_perso_pc_enum_status   status;
  int32                       num_retries;
}gsdi_perso_get_phonecode_status_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_VERIFY_PHONE_CODE_CNF_T
   Confirmation / Response for the GSDI_PERSO_VERIFY_PHONE_CODE_REQ_T.  This will
   verify the phone code.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
}gsdi_perso_verify_phone_code_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_CHANGE_PHONE_CODE_CNF_T
   Confirmation / Response for the GSDI_PERSO_CHANGE_PHONE_CODE_REQ_T.  This will
   change the phone code.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
}gsdi_perso_change_phone_code_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_UNBLOCK_PHONE_CODE_CNF_T
   Confirmation / Response for the GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ_T.  This will
   unblock the phone code.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
}gsdi_perso_unblock_phone_code_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_SET_PHONE_CODE_CNF_T
   Confirmation / Response for the GSDI_PERSO_SET_PHONE_CODE_REQ_T.  This will
   unblock the phone code.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
}gsdi_perso_set_phone_code_cnf_T;

/* ---------------------------------------------------------------------------
   GSDI_PERSO_GET_LIB_VERSION_CNF_T
   Confirmation / Response for the GSDI_PERSO_GET_LIB_VERSION_REQ_T.  This will
   unblock the phone code.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
  int32                       lib_version_len;
  uint8                      *lib_version_ptr;
}gsdi_perso_get_lib_version_cnf_T;
/*~ FIELD gsdi_perso_get_lib_version_cnf_T.lib_version_ptr
    VARRAY LENGTH gsdi_perso_get_lib_version_cnf_T.lib_version_len */

/* ---------------------------------------------------------------------------
   GSDI_PERSO_SECURE_DEVICE_INFO_CNF_T
   Confirmation / Response for the GSDI_PERSO_SECURE_DEVICE_INFO_REQ_T.  This will
   write the IMEI, Serial number to the flash and activate the phone.
   -------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T     message_header;
}gsdi_secure_device_info_cnf_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_GET_ATR_CNF_T
** DESCRIPTION: Used to return Card ATR specific to the slot requested
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T     message_header;
    int32             data_len;
    uint8             data_buffer[GSDI_MAX_ATR_BYTES];
}gsdi_get_atr_cnf_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_OPEN_CHANNEL_CNF_T
** DESCRIPTION: Used to return the status of the GSDI_OPEN_CHANNEL_REQ_T.
**              If successful, a channel_id will be provided in member
**              channel_id.
**              If not successful, a channel_id of 0xFFFF is returned.
**              The data returned in this case will always be two bytes when
**              the command is processed successfully
**              (i.e message_header.gsdi_status == GSDI_SUCCESS)
**              The data_buffer will contain the status of open channel req
**              which are sw1 and sw2
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T     message_header;
    int32             channel_id;
    int32             data_len;
    uint8             data_buffer[GSDI_MAX_DATA_BLOCK_LEN];
}gsdi_open_channel_cnf_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_CLOSE_CHANNEL_CNF_T
** DESCRIPTION: Used to return the status of the GSDI_CLOSE_CHANNEL_REQ_T.
**              The data returned in this case will always be two bytes when
**              the command is processed successfully
**              (i.e message_header.gsdi_status == GSDI_SUCCESS)
**              The data_buffer will contain the status of open channel req
**              which are sw1 and sw2
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T     message_header;
    int32             channel_id;
    int32             data_len;
    uint8             data_buffer[GSDI_MAX_DATA_BLOCK_LEN];
}gsdi_close_channel_cnf_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_SEND_CHANNEL_DATA_CNF_T
** DESCRIPTION: Used to return the status of the GSDI_SEND_CHANNEL_REQ_T.
**              The data returned in this case will vary according to the
**              APDU sent in the GSDI_SEND_CHANNEL_REQ_T.
**              Implicit get response status words are indicators that the
**              drivers already performed the required get response for the
**              command.  When available, the two members:
**              implicit_get_rsp_sw1 and implicit_get_rsp_sw2 will be set.
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T     message_header;
    int32             channel_id;
    int32             apdu_len;
    uint16            implicit_get_rsp_sw1;
    uint16            implicit_get_rsp_sw2;
    uint8             data_buffer[GSDI_MAX_CHANNEL_DATA_RSP_LEN];
}gsdi_send_channel_data_cnf_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_CURRENT_SERVICE_ENUM_TYPE
** DESCRIPTION: Structure will be used to maintain the Service Table
**              information as it pertains to the file: EF-CPHSINFO
** --------------------------------------------------------------------------*/
typedef enum {
  GSDI_NOT_ACTIVATED_AND_NOT_ALLOCATED = 0x00,
  GSDI_NOT_ACTIVATED_AND_ALLOCATED     = 0x01,
  GSDI_ACTIVATED_AND_NOT_ALLOCATED     = 0x02,
  GSDI_ACTIVATED_AND_ALLOCATED         = 0x03
}gsdi_current_service_enum_type;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_CPHS_SERVICE_TABLE_TYPE
** DESCRIPTION: Structure will be used to maintain the Service Table
**              information as it pertains to the file: EF-CPHSINFO
** --------------------------------------------------------------------------*/
typedef struct {
  uint8                           phase;    /* CPHS Phase                    */
  gsdi_current_service_enum_type  csp;      /* Customer Service Profile      */
  gsdi_current_service_enum_type  sst;      /* Phase 1 String Service Table  */
  gsdi_current_service_enum_type  mbn;      /* Mailbox Numbers               */
  gsdi_current_service_enum_type  ops;      /* Operator Name Short Form      */
  gsdi_current_service_enum_type  ifn;      /* Information Numbers           */
}gsdi_cphs_service_table_type;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_CSP_ENUM_TYPE
** DESCRIPTION: Indication as to whether the CPSH Information is On or off.
** --------------------------------------------------------------------------*/
typedef enum {
  GSDI_CSP_IND_OFF = 0x00,
  GSDI_CSP_IND_ON  = 0x01
}gsdi_csp_enum_type;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_CPHS_SERVICE_TABLE_TYPE
** DESCRIPTION: Structure will be used to maintain the Service Table
**              information as it pertains to the file: EF-CPHSINFO
** --------------------------------------------------------------------------*/
typedef struct {
  gsdi_csp_enum_type call_offering_CFU;
  gsdi_csp_enum_type call_offering_CFB;
  gsdi_csp_enum_type call_offering_CFNRy;
  gsdi_csp_enum_type call_offering_CFNRc;
  gsdi_csp_enum_type call_offering_CT;
  gsdi_csp_enum_type call_restrict_BOAC;
  gsdi_csp_enum_type call_restrict_BOIC;
  gsdi_csp_enum_type call_restrict_BOIC_exHC;
  gsdi_csp_enum_type call_restrict_BAIC;
  gsdi_csp_enum_type call_restrict_BIC_Roam;
  gsdi_csp_enum_type other_supps_MPTY;
  gsdi_csp_enum_type other_supps_CUG;
  gsdi_csp_enum_type other_supps_AoC;
  gsdi_csp_enum_type other_supps_Pref_CUG;
  gsdi_csp_enum_type other_supps_CUG_OA;
  gsdi_csp_enum_type call_completion_HOLD;
  gsdi_csp_enum_type call_completion_CW;
  gsdi_csp_enum_type call_completion_CCBS;
  gsdi_csp_enum_type call_completion_User_User_Signalling;
  gsdi_csp_enum_type teleservices_SM_MT;
  gsdi_csp_enum_type teleservices_SM_MO;
  gsdi_csp_enum_type teleservices_SM_CB;
  gsdi_csp_enum_type teleservices_Reply_Path;
  gsdi_csp_enum_type teleservices_Del_Conf;
  gsdi_csp_enum_type teleservices_Protocol_ID;
  gsdi_csp_enum_type teleservices_Validity_Period;
  gsdi_csp_enum_type cphs_teleservices_als;
  gsdi_csp_enum_type cphs_features_SST;
  gsdi_csp_enum_type number_identify_CLIP;
  gsdi_csp_enum_type number_identify_CoLR;
  gsdi_csp_enum_type number_identify_CoLP;
  gsdi_csp_enum_type number_identify_MCI;
  gsdi_csp_enum_type number_identify_CLI_send;
  gsdi_csp_enum_type number_identify_CLI_block;
  gsdi_csp_enum_type p2plus_services_GPRS;
  gsdi_csp_enum_type p2plus_services_HSCSD;
  gsdi_csp_enum_type p2plus_services_Voice_Group_call;
  gsdi_csp_enum_type p2plus_services_Voice_Broadcast_Svc;
  gsdi_csp_enum_type p2plus_services_Muli_Sub_Profile;
  gsdi_csp_enum_type p2plus_services_Multiband;
  gsdi_csp_enum_type value_added_PLMN_MODE;
  gsdi_csp_enum_type value_added_VPS;
  gsdi_csp_enum_type value_added_SM_MO_PAGING;
  gsdi_csp_enum_type value_added_SM_MO_EMAIL;
  gsdi_csp_enum_type value_added_FAX;
  gsdi_csp_enum_type value_added_DATA;
  gsdi_csp_enum_type valued_added_Language;
  gsdi_csp_enum_type information_numbers;
}gsdi_cphs_cust_service_profile_type;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_GET_CPHS_INFORMATION_CNF_T
** DESCRIPTION: Used to return the CPHS Information as retrieved from the
**              SIM/USIM/RUIM.  The information returned should be used by
**              the caller in the following order.
**              First determine if the SIM/USIM is cphs capable.
**              Then determine if the CPHS Service Table has allowed and
**              allocated the feature you are looking for.
**              Then the caller can if they choose to, determine which
**              CPHS Files are available for use.
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_rsphdr_T                       message_header;
    boolean                             cphs_capable;
    gsdi_cphs_service_table_type        services;
    gsdi_cphs_cust_service_profile_type csp;
}gsdi_get_cphs_information_cnf_T;

typedef struct {
    byte      sw1;
    byte      sw2;
}gsdi_sw_type;

typedef struct {
    gsdi_rsphdr_T     message_header;
    gsdi_pin_ids_T    pin_id;
    gsdi_pin_status_T status;
    byte              num_of_retries;
}gsdi_get_pin_status_cnf_T;

typedef struct {
    gsdi_rsphdr_T    message_header;
    gsdi_pin_ids_T   pin_id;
    byte             num_of_retries;
    gsdi_sw_type     status_words;
}gsdi_verify_pin_cnf_T;

typedef struct {
    gsdi_rsphdr_T    message_header;
    gsdi_pin_ids_T   pin_id;
    byte             num_of_retries;
    gsdi_sw_type     status_words;
}gsdi_unblock_pin_cnf_T;

typedef struct {
    gsdi_rsphdr_T    message_header;
    gsdi_pin_ids_T   pin_id;
    byte             num_of_retries;
    gsdi_sw_type     status_words;
}gsdi_change_pin_cnf_T;

typedef struct {
    gsdi_rsphdr_T    message_header;
    gsdi_pin_ids_T   pin_id;
    byte             num_of_retries;
    gsdi_sw_type     status_words;
}gsdi_disable_pin_cnf_T;

typedef struct {
    gsdi_rsphdr_T    message_header;
    gsdi_pin_ids_T   pin_id;
    byte             num_of_retries;
    gsdi_sw_type     status_words;
}gsdi_enable_pin_cnf_T;

typedef struct {
  gsdi_rsphdr_T     message_header;
  word              sim_filename;
  byte              rec_num_accessed;    /* Record Number accessed in Read Access */
  byte              returned_data_len;   /* Number of bytes returned in data */
  byte              data[GSDI_MAX_DATA_BLOCK_LEN];   /* Data Buffer */
}gsdi_read_cnf_T;

typedef struct {
  gsdi_rsphdr_T    message_header;
  word             returned_data_len; /* Number of bytes returned in data */
  uint8            *data_p;           /* Data Buffer */
  /*~ FIELD gsdi_get_image_cnf_T.data_p VARRAY LENGTH gsdi_get_image_cnf_T.returned_data_len */
}gsdi_get_image_cnf_T;

typedef struct {
  gsdi_rsphdr_T    message_header;
  int32            returned_data_len; /* Number of bytes returned in data */
  uint8            *data_p;           /* Data Buffer */
  word             sim_filename;
  byte             rec_num_accessed;  /* Record Number accessed in Read Access */
}gsdi_sim_access_cnf_T;
/*~ FIELD gsdi_sim_access_cnf_T.data_p
    VARRAY LENGTH gsdi_sim_access_cnf_T.returned_data_len */

typedef struct {
  gsdi_rsphdr_T    message_header;
  word             sim_filename;
  byte             rec_num_accessed;  /* Record Number accessed in Read Access */
}gsdi_write_cnf_T;


typedef struct {
  gsdi_rsphdr_T    message_header;
  byte             returned_data_len; /* Number of bytes returned in data */
  byte             data[GSDI_MAX_DATA_BLOCK_LEN];   /* Data Buffer */
}gsdi_select_cnf_T;

typedef struct {
  gsdi_rsphdr_T          message_header;
  gsdi_file_attributes_T gfa_data;    /* Get file attribute data */
}gsdi_gfa_cnf_T;

typedef struct {
  gsdi_rsphdr_T    message_header;
  byte             new_value_len;                /* new increased value len */
  byte             data[GSDI_MAX_DATA_BLOCK_LEN];/* new increased value data*/
}gsdi_increase_cnf_T;

typedef struct {
  gsdi_rsphdr_T    message_header;
  byte             returned_data_len;  /* Number of bytes returned in data */
  byte             data[GSDI_MAX_DATA_BLOCK_LEN];   /* Data Buffer */
}gsdi_run_gsm_alg_cnf_T;

typedef struct {
  gsdi_rsphdr_T      message_header;
  sim_capabilities_T sim_capabilities;        /* sim capabilities */
}gsdi_get_sim_cap_cnf_T;

typedef struct {
  gsdi_rsphdr_T      message_header;
  int                dummy;
}gsdi_enable_fdn_cnf_T;

typedef struct {
  gsdi_rsphdr_T      message_header;
  int                dummy;
}gsdi_disable_fdn_cnf_T;

typedef struct {
  gsdi_rsphdr_T      message_header;
  int                dummy;
}gsdi_illegal_sim_cnf_T;

typedef struct {
  gsdi_rsphdr_T      message_header;
  byte               rec_num_accessed;
}gsdi_search_cnf_T;


typedef struct {
  gsdi_rsphdr_T      message_header;
  byte               returned_data_len;
  byte               data[GSDI_MAX_DATA_BLOCK_LEN];  /* variable length */
}gsdi_usim_auth_cnf_T;


typedef struct {
  gsdi_rsphdr_T      message_header;
  uint32             client_ref;
}gsdi_refresh_cnf_T;

typedef struct {
  gsdi_rsphdr_T      message_header;
  uint8     sw1;
  uint8     sw2;
  uint16    resp_data_len;
  uint8     data[GSDI_MAX_DATA_BLOCK_LEN];
}gsdi_send_apdu_cnf_T;

typedef struct
{
  gsdi_rsphdr_T            message_header;
  gsdi_ecc_info_table_type emergency_call_code;
}gsdi_get_ecc_cnf_T;

typedef struct {
    gsdi_rsphdr_T     message_header;
    boolean           refresh_pending;
} gsdi_switch_slot_preference_cnf_T;

/*
** GSDI_CNF message
*/
typedef union{
    gsdi_rsphdr_T                     message_header;
    gsdi_get_pin_status_cnf_T         pin_status_cnf;
    gsdi_verify_pin_cnf_T             verify_pin_cnf;
    gsdi_unblock_pin_cnf_T            unblock_pin_cnf;
    gsdi_disable_pin_cnf_T            disable_pin_cnf;
    gsdi_enable_pin_cnf_T             enable_pin_cnf;
    gsdi_change_pin_cnf_T             change_pin_cnf;
    gsdi_read_cnf_T                   read_cnf;
    gsdi_get_image_cnf_T              image_cnf;
    gsdi_sim_access_cnf_T             sim_access_cnf;
    gsdi_write_cnf_T                  write_cnf;
    gsdi_select_cnf_T                 select_cnf;
    gsdi_gfa_cnf_T                    gfa_cnf;
    gsdi_increase_cnf_T               increase_cnf;
    gsdi_run_gsm_alg_cnf_T            run_gsm_alg_cnf;
    gsdi_get_sim_cap_cnf_T            get_sim_cap_cnf;
    gsdi_enable_fdn_cnf_T             enable_fdn_cnf;
    gsdi_disable_fdn_cnf_T            disable_fdn_cnf;
    gsdi_illegal_sim_cnf_T            illegal_sim_cnf;
    gsdi_search_cnf_T                 search_cnf;
    gsdi_usim_auth_cnf_T              usim_auth_cnf;
    gsdi_refresh_cnf_T                refresh_cnf;
    gsdi_send_apdu_cnf_T              send_apdu_cnf;
    gsdi_get_ecc_cnf_T                ecc_cnf;
    gsdi_switch_slot_preference_cnf_T switch_slot_pref_cnf;
    gsdi_open_channel_cnf_T           open_channel_cnf;
    gsdi_close_channel_cnf_T          close_channel_cnf;
    gsdi_get_atr_cnf_T                get_atr_cnf;
    gsdi_send_channel_data_cnf_T      send_channel_data_cnf;
    gsdi_perso_reg_task_cnf_T                    reg_task_cnf;       /* Cnf for Register Req     */
    gsdi_perso_act_feature_ind_cnf_T             act_ind_cnf;        /* Cnf for Act Ind Req      */
    gsdi_perso_get_feature_ind_cnf_T             get_ind_cnf;        /* Cnf for Get Ind Req      */
    gsdi_perso_deact_feature_ind_cnf_T           dact_ind_cnf;       /* Cnf for D-Act Ind Req    */
    gsdi_perso_set_feature_data_cnf_T            set_data_cnf;       /* Cnf for Set Data Req     */
    gsdi_perso_get_feature_data_cnf_T            get_data_cnf;       /* Cnf for Get Data Req     */
    gsdi_perso_ota_deperso_cnf_T                 ota_cnf;            /* Cnf for OTA Deperso Req  */
    gsdi_perso_int_init_procedure_cnf_T          int_init_cnf;       /* Cnf for Inter Init Procs */
    gsdi_perso_unblock_feature_ind_cnf_T         unblock_ind_cnf;    /* Cnf for Unblock Ind Req */
    gsdi_perso_perm_disable_feature_ind_cnf_T    disable_ind_cnf;    /* Cnf for Disable Ind Req */
    gsdi_perso_get_perm_feature_ind_cnf_T        get_perm_ind_cnf;   /* Cnf for Perm Feature Ind Req */
    gsdi_perso_get_feature_key_cnf_T             get_ind_key;        /* Cnf for get key Req */
    gsdi_perso_change_dck_cnf_T                  change_dck_cnf;     /* Cnf for changing DCK */
    gsdi_perso_get_dck_num_retries_cnf_T         dck_num_retries_cnf;/* Cnf for get Num Retries */
    gsdi_perso_get_dck_unblock_num_retries_cnf_T dck_unblock_num_retries_cnf;/* Cnf for get Num Retries */
    gsdi_perso_get_phonecode_status_cnf_T     get_phone_code_cnf;
    gsdi_perso_verify_phone_code_cnf_T        verify_phone_code_cnf;
    gsdi_perso_change_phone_code_cnf_T        change_phone_code_cnf;
    gsdi_perso_unblock_phone_code_cnf_T       unblock_phone_code_cnf;
    gsdi_perso_set_phone_code_cnf_T           set_phone_code_cnf;
    gsdi_get_cphs_information_cnf_T           get_cphs_info_cnf;
    gsdi_get_pbr_table_cnf_T                  get_pbr_table_cnf;
    gsdi_activate_onchip_sim_cnf_T            act_onchip_sim_cnf;
    gsdi_get_all_pin_info_cnf_T               all_pin_info_cnf;
    /*~ CASE GSDI_GET_PIN_STATUS_RSP       gsdi_cnf_T.pin_status_cnf  */
    /*~ CASE GSDI_VERIFY_PIN_RSP           gsdi_cnf_T.verify_pin_cnf  */
    /*~ CASE GSDI_UNBLOCK_PIN_RSP          gsdi_cnf_T.unblock_pin_cnf */
    /*~ CASE GSDI_DISABLE_PIN_RSP          gsdi_cnf_T.disable_pin_cnf */
    /*~ CASE GSDI_ENABLE_PIN_RSP           gsdi_cnf_T.enable_pin_cnf  */
    /*~ CASE GSDI_CHANGE_PIN_RSP           gsdi_cnf_T.change_pin_cnf  */
    /*~ CASE GSDI_SIM_READ_RSP             gsdi_cnf_T.read_cnf        */
    /*~ CASE GSDI_GET_IMAGE_RSP            gsdi_cnf_T.image_cnf       */
    /*~ CASE GSDI_SIM_WRITE_RSP            gsdi_cnf_T.write_cnf       */
    /*~ CASE GSDI_SELECT_RSP               gsdi_cnf_T.select_cnf      */
    /*~ CASE GSDI_GET_FILE_ATTRIBUTES_RSP  gsdi_cnf_T.gfa_cnf         */
    /*~ CASE GSDI_SIM_INCREASE_RSP         gsdi_cnf_T.increase_cnf    */
    /*~ CASE GSDI_RUN_GSM_ALGORITHM_RSP    gsdi_cnf_T.run_gsm_alg_cnf */
    /*~ CASE GSDI_GET_SIM_CAPABILITIES_RSP gsdi_cnf_T.get_sim_cap_cnf */
    /*~ CASE GSDI_ENABLE_FDN_RSP           gsdi_cnf_T.enable_fdn_cnf  */
    /*~ CASE GSDI_DISABLE_FDN_RSP          gsdi_cnf_T.disable_fdn_cnf */
    /*~ CASE GSDI_ILLEGAL_SIM_RSP          gsdi_cnf_T.illegal_sim_cnf */
    /*~ CASE GSDI_SIM_SEARCH_RSP           gsdi_cnf_T.search_cnf      */
    /*~ CASE GSDI_USIM_AUTHENTICATION_RSP  gsdi_cnf_T.usim_auth_cnf   */
    /*~ CASE GSDI_SIM_REFRESH_RSP          gsdi_cnf_T.refresh_cnf     */
    /*~ CASE GSDI_SEND_APDU_RSP            gsdi_cnf_T.send_apdu_cnf   */
    /*~ CASE GSDI_GET_ECC_RSP              gsdi_cnf_T.ecc_cnf         */

    /*~ CASE GSDI_SWITCH_SLOT_PREF_RSP     gsdi_cnf_T.switch_slot_pref_cnf */

    /*~ CASE GSDI_OPEN_CHANNEL_RSP         gsdi_cnf_T.open_channel_cnf */
    /*~ CASE GSDI_CLOSE_CHANNEL_RSP        gsdi_cnf_T.close_channel_cnf */
    /*~ CASE GSDI_GET_ATR_RSP              gsdi_cnf_T.get_atr_cnf     */

    /*~ CASE GSDI_SEND_CHANNEL_DATA_RSP    gsdi_cnf_T.send_channel_data_cnf */

    /*~ CASE GSDI_PERSO_REG_TASK_RSP       gsdi_cnf_T.reg_task_cnf    */
    /*~ CASE GSDI_PERSO_ACT_IND_RSP        gsdi_cnf_T.act_ind_cnf     */
    /*~ CASE GSDI_PERSO_GET_IND_RSP        gsdi_cnf_T.get_ind_cnf     */
    /*~ CASE GSDI_PERSO_DEACT_IND_RSP      gsdi_cnf_T.dact_ind_cnf    */
    /*~ CASE GSDI_PERSO_SET_DATA_RSP       gsdi_cnf_T.set_data_cnf    */
    /*~ CASE GSDI_PERSO_OTA_DEPERSO_RSP    gsdi_cnf_T.ota_cnf     */
    /*~ CASE GSDI_PERSO_INTERNAL_PROC_RSP  gsdi_cnf_T.int_init_cnf    */
    /*~ CASE GSDI_PERSO_UNBLOCK_IND_RSP    gsdi_cnf_T.unblock_ind_cnf */
    /*~ CASE GSDI_PERSO_PERM_DISABLE_IND_RSP  gsdi_cnf_T.disable_ind_cnf */
    /*~ CASE GSDI_PERSO_GET_PERM_IND_RSP      gsdi_cnf_T.get_perm_ind_cnf */
    /*~ CASE GSDI_PERSO_GET_FEATURE_KEY_RSP   gsdi_cnf_T.get_ind_key */
    /*~ CASE GSDI_PERSO_CHANGE_DCK_RSP     gsdi_cnf_T.change_dck_cnf  */
    /*~ CASE GSDI_PERSO_GET_DCK_NUM_RETRIES_RSP  gsdi_cnf_T.dck_num_retries_cnf */
    /*~ CASE GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_RSP  gsdi_cnf_T.dck_unblock_num_retries_cnf */

    /*~ CASE GSDI_PERSO_VERIFY_PHONE_CODE_RSP  gsdi_cnf_T.verify_phone_code_cnf */
    /*~ CASE GSDI_PERSO_CHANGE_PHONE_CODE_RSP  gsdi_cnf_T.change_phone_code_cnf */
    /*~ CASE GSDI_PERSO_UNBLOCK_PHONE_CODE_RSP gsdi_cnf_T.unblock_phone_code_cnf */
    /*~ CASE GSDI_PERSO_SET_PHONE_CODE_RSP     gsdi_cnf_T.set_phone_code_cnf    */
    /*~ CASE GSDI_PERSO_GET_PHONE_CODE_STATUS_RSP  gsdi_cnf_T.get_phone_code_cnf */

    /*~ CASE GSDI_GET_CPHS_RSP             gsdi_cnf_T.get_cphs_info_cnf      */

    /*~ CASE GSDI_GET_PRB_TABLE_RSP        gsdi_cnf_T.get_pbr_table_cnf      */
    /*~ CASE GSDI_ACTIVATE_ONCHIP_SIM_RSP  gsdi_cnf_T.act_onchip_sim_cnf     */
    /*~ CASE GSDI_SIM_ACCESS_RSP           gsdi_cnf_T.sim_access_cnf*/
    /*~ CASE GSDI_GET_ALL_PIN_INFO_RSP     gsdi_cnf_T.all_pin_info_cnf*/
    /*~ CASE GSDI_PERSO_GET_DATA_RSP       gsdi_cnf_T.get_data_cnf    */

    /*~ DEFAULT gsdi_cnf_T.message_header */
}
gsdi_cnf_T;
/*~ TYPE gsdi_cnf_T DISC (_OBJ_.message_header.resp_type) */

typedef void (*gsdi_async_cb) (gsdi_cnf_T* gsdi_cnf);
/*~ PARAM gsdi_cnf POINTER */

/*
** COMMAND MESSAGE HEADER
*/
typedef struct {
    byte               message_set;
    byte               message_id;
    byte               message_len_lsb;
    byte               message_len_msb;
    rex_tcb_type       *tcb_ptr;
    rex_sigs_type      wait_sig;
    rex_crit_sect_type *crit_sec;
    gsdi_slot_id_type  slot;
    void              (*gsdi_async_callback)(gsdi_cnf_T*);
}
gsdi_cmdhdr_T;

/*
** GSDI INDICATION OF APPLICATION
** STATUS TYPE
*/
typedef enum {
    GSDI_NO_INDICATION,
    GSDI_CURRENT_APP_IS_INITIALIZED,
    GSDI_ME_INITIATES_SESSION_TERMINATION
}gsdi_ind_of_app_status_type;

/*
** GSDI EXPECTED RETURNED DATA FOR
** STATUS COMMAND
*/
typedef enum {
    GSDI_RSP_DATA_SAME_AS_SELECT,
    GSDI_DF_NAME_TLV_RETURNED,
    GSDI_NO_DATA_RETURNED = 0x0C
} gsdi_exp_status_return_type;

/* Hot swap status */
typedef enum {
  GSDI_UIM_CARD_UNCHANGED,
  GSDI_UIM_CARD_REMOVED,
  GSDI_UIM_CARD_INSERTED
}gsdi_uim_swap_enum_type;


/* ----------------------------------------------------------------------------

               GSDI   REQ   COMMAND   STRUCTURES

   --------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_ACTIVATE_ONCHIP_SIM_REQ_T
   DESCRIPTION: This command will activate the ONCHIP SIM Simulation
                to allow for Factory Test Mode procedures eventhough a
                SIM/USIM is not inserted.
   -------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Rsp Message Header        */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_activate_onchip_sim_req_T;

/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_GET_ALL_PIN_INFO_REQ_T
   DESCRIPTION: This command can be used to request the information
                about all the available pins.
   -------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Rsp Message Header        */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_get_all_pin_info_req_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_REG_TASK_REQ_T
   Command / Request structure used to register a task
   with the GSDI Personalization / Depersonalziation Engines.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_events_cb        reg_task_cb_p;  /* Pointer to task cb        */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_reg_task_req_T;
/* ----------------------------------------------------------------------------
   GSDI_PERSO_DEACT_FEATURE_IND_REQ_T
   Command / Request structure used to deactivate a
   Personalization Feature Indicator (i.e. set the
   Indicator to off so that Personalziation does occurs at
   power up)
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_enum_type        feature;        /* Perso Feature affected    */
    int32                       num_bytes;      /* Num bytes for CK          */
    uint8                       ck_buffer[GSDI_PERSO_MAX_CK]; /* Buffer to
                                                   CK String                 */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_deact_feature_ind_req_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_ACT_FEATURE_IND_REQ_T
   Command / Request structure used to activate a
   Personalization Feature Indicator (i.e. set the
   Indicator to on so that Personalziation occurs at
   power up)
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_enum_type        feature;  /* Perso Feature affected    */
    int32                       num_bytes;      /* Num bytes for CK          */
    uint8                       ck_buffer[GSDI_PERSO_MAX_CK]; /* Buffer to
                                                   CK String                 */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_act_feature_ind_req_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_GET_FEATURE_IND_REQ_T
   Command / Request structure used retrieve the
   status of all personalization indicators
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_get_feature_ind_req_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DACT_FEATURE_IND_REQ_T
   Command / Request structure used to deactivate a
   Personalization Feature Indicator (i.e. set the
   Indicator to "off" so that Personalziation does not
   occur at power up)
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Headder   */
    gsdi_perso_enum_type        perso_feature;  /* Perso Feature affected    */
    int32                       num_bytes;      /* Number of bytes for CK    */
    uint8                       ck_buffer[GSDI_PERSO_MAX_CK];
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_dact_feature_ind_req_T;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_OTA_DEPERSO_DATA_REQ_T
   Command / Request structure used to send the Depersonalization
   Control Keys from an SMS PP Message to the Depersonalization
   Engine.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    int32                       num_bytes;      /* Num Bytes of OTA Data     */
    uint8                       data_buffer[GSDI_PERSO_MAX_OTA_DEPERSO_BYTES];
                                                /* Buffer IMEI and CKs       */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_ota_deperso_req_T;
/* ----------------------------------------------------------------------------
   GSDI_PERSO_OTA_DEPERSO_DATA_REQ_T
   Command / Request structure used to send the Depersonalization
   Control Keys from an SMS PP Message to the Depersonalization
   Engine.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_enum_type        feature;        /* Internal Init for feature */
    gsdi_perso_self_perso_state_enum_type self_perso_state; /* Self Perso S  */
    gsdi_perso_enum_type        int_init_feature;/* Feature to personalize    */
    boolean                     deactivated;    /* Notify self if deactivated*/
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_int_init_procedure_req_T;

/* ----------------------------------------------------------------------------
   STRUCT:  gsdi_perso_internal_command_req_T
            This structure is used to queue up an internal Peronsalization
            command.  There is not a confirmation associated with this
            command.
   --------------------------------------------------------------------------*/
typedef struct {
     gsdi_cmdhdr_T               message_header;  /* Request Message Header  */
     gsdi_perso_enum_type        feature;         /* Feature for Conf        */
     int32                       active_lock_mask;/* Mask of Active Locks    */
     int32                       autolock_mask;   /* Mask of Auto Locks      */
     uint32                       client_ref;     /* Data returned to caller   */
     uint8                        queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_internal_command_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_VERIFY_PHONE_CODE_REQ_T
   DESCRIPTION: Command / Request structure used to verify phone code
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    int32                       phone_code_len; /* Phone code Len            */
    uint8                      *phone_code_ptr; /* Phone code                */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_verify_phone_code_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_CHANGE_PHONE_CODE_REQ_T
   DESCRIPTION: Command / Request structure used to change phone code
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header;        /* Request Message Header    */
    int32                       old_phone_code_len;    /* old phone code Len        */
    uint8                      *old_phone_code_ptr;    /* old phone code            */
    int32                       new_phone_code_len;    /* new phone code len        */
    uint8                      *new_phone_code_ptr;    /* new phone code            */
    uint32                      client_ref;            /* Data returned to caller   */
    uint8                       queue_id;              /* Legacy Queue ID Used      */
} gsdi_perso_change_phone_code_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_UNBLOCK_PHONE_CODE_REQ_T
   DESCRIPTIONP: Command / Request structure used to unblock phone code
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header;      /* Request Message Header    */
    int32                       master_key_len;      /* Master Key Len            */
    uint8                      *master_key_ptr;      /* Master Key                */
    uint32                      client_ref;          /* Data returned to caller   */
    uint8                       queue_id;            /* Legacy Queue ID Used      */
} gsdi_perso_unblock_phone_code_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_SET_PHONE_CODE_REQ_T
   DESCRIPTION: Command / Request structure used to set phone code
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header;      /* Request Message Header    */
    int32                       phone_code_len;      /* phone code len            */
    uint8                      *phone_code_ptr;      /* phone code                */
    uint32                      client_ref;          /* Data returned to caller   */
    uint8                       queue_id;            /* Legacy Queue ID Used      */
} gsdi_perso_set_phone_code_req_T;

/* ---------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_PHONECODE_STATUS_REQ_T
   DESCRIPTION: Command / Request for the GSDI_PERSO_PHONECODE_STATUS_REQ_T.
                This will get the number of current status of the Phonecode
                as well and provide the number incorrect Phonecoes
                Verifications are still remaining.
   -------------------------------------------------------------------------*/
typedef struct
{
  gsdi_rsphdr_T               message_header; /* Rsp Message Header        */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_get_phonecode_status_req_T;


/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_SET_FEATURE_DATA_REQ_T
   DESCRIPTION: Command / Request structure used to Personalize
   or store the codes for the particular personalization
   feature.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_enum_type        feature;        /* Perso Feature affected    */
    int32                       num_bytes;      /* Num Bytes of OTA Data     */
    uint8                       data_buffer[GSDI_PERSO_MAX_DATA_LEN];
                                                /* Buffer with perso codes   */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_set_feature_data_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_FEATURE_DATA_REQ_T
   DESCRIPTION: Command / Request structure used to Personalize
   or store the codes for the particular personalization
   feature.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header       */
    gsdi_perso_enum_type        feature;        /* Perso Feature to be accessed */
    uint32                      client_ref;     /* Data returned to caller      */
    uint8                       queue_id;       /* Legacy Queue ID Used         */
}gsdi_perso_get_feature_data_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_CHANGE_DCK_REQ_T
   DESCRIPTION: Command / Request structure used retrieve the
   status of all personalization indicators
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    int32                       dck_len;        /* Data returned to caller   */
    uint8 *                     dck_buf_p;      /* DCK Buffer Pointer        */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_change_dck_req_T;


/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_DCK_NUM_RETRIES_REQ_T
   DESCRIPTION: Command / Request structure used to retrieve DCK number of
   retries available
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_get_dcks_num_retries_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_DCK_UNBLOCK_NUM_RETRIES_REQ_T
   DESCRIPTION: Command / Request structure used to retrieve unblock DCK 
   number of retries available
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_get_dck_unblock_num_retries_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_PERM_DISBALE_FEATURE_IND_REQ_T
   DESCRIPTION: Command / Request structure used retrieve the
   status of all personalization indicators
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_get_perm_feature_ind_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_PERM_DISABLE_FEATURE_REQ_T
   DESCRIPTION: Command / Request structure used to permanently disable the
                personalization feature when turned off.
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_enum_type        perso_feature;  /* Perso Feature affected    */
    int32                       num_bytes;      /* Num Bytes of OTA Data     */
    uint8                       ck_buffer[GSDI_PERSO_MAX_CK];
                                                /* Buffer with perso codes   */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_perm_disabled_feature_req_T;


/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ_T
   DESCRIPTION: Command / Request structure used to unblock a blocked
   Personalization Feature Indicator
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    gsdi_perso_enum_type        feature;        /* Perso Feature affected    */
    int32                       num_bytes;      /* Num bytes for CK          */
    uint8                       unblock_ck_buffer[GSDI_PERSO_MAX_CK];
                                                /* Buffer to CK String       */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
}gsdi_perso_unblock_feature_ind_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_PERSO_GET_FEATURE_KEY_REQ_T
   DESCRIPTION: Command / Request structure used retrieve the
   key of the personalization level
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    boolean                     is_unblock_key; /* If we are retriving
                                                   unblock key or not        */
    gsdi_perso_enum_type        feature;        /* Perso Feature affected    */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_perso_get_feature_key_req_T;

/* ----------------------------------------------------------------------------
   STRUCTURE:   GSDI_SECURE_DEVICE_INFO_INFO_REQ_T
   DESCRIPTION: Command / Request structure used activate the personalization
   --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header; /* Request Message Header    */
    uint32                      client_ref;     /* Data returned to caller   */
    uint8                       queue_id;       /* Legacy Queue ID Used      */
} gsdi_secure_device_info_req_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_GET_CPHS_INFORMATION_REQ_T
** DESCRIPTION: Used to send a request to GSDI to retrieve the Common
**              PCN Handset Specifications (CPHS) Overall Information as it
**              pertains to the SIM/USIM/RUIM Inserted.
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T               message_header;
    uint8                       queue_id;       /* Legacy Queue ID Used      */
    uint32                      client_ref;
}gsdi_get_cphs_information_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    byte                        me_supports_fdn;
    uint32                      client_ref;
    gsdi_refresh_ef_module_type ef_module;
}
gsdi_init_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    uint32                      client_ref;
    gsdi_refresh_ef_module_type ef_module;
}gsdi_internal_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    uint32                      cb_table_index;
}gsdi_notify_client_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    uim_items_enum_type         sim_filename;
    gsdi_app_id_type            app_id;
}
gsdi_rehabilitate_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    uim_items_enum_type         sim_filename;
    gsdi_app_id_type            app_id;
}
gsdi_invalidate_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    byte                        queue_id;
    uint32                      client_ref;
    uint8                       num_files;
    uim_items_enum_type         file_list_p[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
}gsdi_update_call_stack_req_T;

typedef struct {
    gsdi_cmdhdr_T               message_header;
    byte                        queue_id;
    uint32                      client_ref;
    gsdi_refresh_vote_cb_type * reg_vote_cb;
    gsdi_task_notify_file_list_cb_type * reg_file_notify_cb;
    uint8                       num_files;
    uim_items_enum_type         file_list_p[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
}
gsdi_refresh_registration_req_T;

typedef struct {
    gsdi_cmdhdr_T                 message_header;
    byte                          queue_id;
    uint32                        client_ref;
    boolean                       pass_fail;
    gsdi_refresh_modes_enum_type  refresh_mode;
    gsdi_app_id_type              app_id;
    uint8                         num_files;
    uim_items_enum_type           file_list_p[GSDI_MAX_FILE_CHANGE_NOTIFICATIONS];
}
gsdi_refresh_complete_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             queue_id;
    uint32           client_ref;
    gsdi_app_id_type app_id;

}
gsdi_get_sim_capabilities_req_T;

typedef struct {
    gsdi_ind_of_app_status_type app_status;
    gsdi_exp_status_return_type data_exp;
}
gsdi_usim_status_req_T;

typedef struct {
    gsdi_cmdhdr_T      message_header;
    word               sim_filename;
    gsdi_index_modes_T index_mode;
    byte               search_string[GSDI_MAX_SEARCH_STR_LEN];
    byte               search_string_len;
    byte               search_rec_num;
    byte               search_mode;
    byte               search_direction;
    byte               search_offset_method;
    byte               offset;
    gsdi_app_id_type   app_id;
    byte               queue_id;
    uint32             client_ref;
    byte               rfu1;
    byte               rfu2;
}
gsdi_sim_search_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             queue_id;
    gsdi_refresh_modes_enum_type refresh_mode;
    uint8            num_files;
    uint8            data_len;
    gsdi_app_id_type app_id;
    uint8            data[GSDI_MAX_DATA_BLOCK_LEN];
    uint32           client_ref;
}
gsdi_sim_refresh_req_T;

typedef struct {
    gsdi_cmdhdr_T       message_header;
    word                sim_filename;
    gsdi_index_modes_T  index_mode;
    byte                rec_num;
    byte                search_string[GSDI_MAX_SEARCH_STR_LEN];
    byte                search_string_len;
    byte                required_data_len;
    byte                data[GSDI_MAX_DATA_BLOCK_LEN];
    byte                data_offset;
    byte                queue_id;
    gsdi_app_id_type    app_id;
    uint32              client_ref;
}
gsdi_sim_read_req_T;

typedef struct
{
  gsdi_cmdhdr_T       message_header;
  byte                queue_id;
  uint32              client_ref;
}
gsdi_get_ecc_req_T;

typedef struct {
    gsdi_cmdhdr_T     message_header;
    /* UIM Related Info */
    uim_status_type   error_condition;
    uim_slot_type     uim_slot;
}gsdi_notified_error_req_T;

typedef struct {
    gsdi_cmdhdr_T     message_header;
    uint32            apdu_len;
    uint8             apdu_data[GSDI_MAX_DATA_BLOCK_LEN];
    uint8             queue_id;
    uint32            client_ref;
}gsdi_send_apdu_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    word             sim_filename;
    word             *path;
    uint8            path_len;
    uint8            rec_num;
    word             file_data_offset;
    word             required_data_len;
    gsdi_app_id_type app_id;
    byte             *data;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_sim_access_req_T;


typedef struct {
    gsdi_cmdhdr_T    message_header;
    word             sim_filename;
    word             *path;
    uint8            path_len;
    uint8            rec_num;
    word             file_data_offset;
    word             required_data_len;
    gsdi_app_id_type app_id;
    byte             *data;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_get_image_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    word             sim_filename;
    byte             index_mode;
    byte             rec_num;
    byte             search_string[GSDI_MAX_SEARCH_STR_LEN];
    byte             search_string_len;
    byte             data_buffer[GSDI_MAX_DATA_BLOCK_LEN];
    byte             data_len;
    byte             data_offset;
    gsdi_app_id_type app_id;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_sim_write_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    word             sim_filename;
    byte             increase_value[GSDI_INCREASE_VALUE_LEN];
    gsdi_app_id_type app_id;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_sim_increase_req_T;

typedef struct {
    gsdi_cmdhdr_T message_header;
    byte          rand[GSDI_RAND_LEN];
    byte          queue_id;
    uint32        client_ref;
}
gsdi_run_gsm_algorithm_req_T;

typedef struct {
    gsdi_cmdhdr_T             message_header;
    authentication_context_T  authentication_context;
    byte                      rand[GSDI_RAND_LEN];
    byte                      rand_length;
    gsdi_app_id_type          app_id;
    byte                      autn[GSDI_AUTN_LEN];
    byte                      autn_length;
    byte                      queue_id;
    uint32                    client_ref;
}
gsdi_usim_authentication_req_T;

typedef struct {
    gsdi_cmdhdr_T       message_header;
    word                sim_filename;
    byte                queue_id;
    gsdi_app_id_type    app_id;
    uint32              client_ref;
}
gsdi_get_file_attributes_req_T;


typedef struct {
    gsdi_cmdhdr_T     message_header;                       /* hdr info */
    word              sim_filename[UIM_MAX_PATH_ELEMENTS];  /* file/dir to be selected */
    uim_file_type     file_type;                            /* MF/DF/EF/ADF */
    byte              path_len;
    byte              queue_id;                             /* queue id */
    gsdi_app_id_type  app_id;
    uint32            client_ref;                            /* client reference */
}
gsdi_select_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin2_present;
    byte             pin2[GSDI_PIN_LEN];
    byte             queue_id;
    gsdi_app_id_type app_id;
    uint32           client_ref;
}
gsdi_enable_fdn_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin2_present;
    byte             pin2[GSDI_PIN_LEN];
    byte             queue_id;
    gsdi_app_id_type app_id;
    uint32           client_ref;
}
gsdi_disable_fdn_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    gsdi_pin_ids_T   pin_id;
    byte             queue_id;
    gsdi_app_id_type app_id;
    uint32           client_ref;
}
gsdi_get_pin_status_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin_id;
    byte             pin[GSDI_PIN_LEN];
    byte             queue_id;
    gsdi_app_id_type app_id;
    uint32           client_ref;
}
gsdi_verify_pin_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin_id;
    byte             old_pin[GSDI_PIN_LEN];
    byte             new_pin[GSDI_PIN_LEN];
    byte             gsdi_card_mode_req;
    gsdi_app_id_type app_id;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_change_pin_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin_id;
    byte             puk[GSDI_PUK_LEN];
    byte             new_pin[GSDI_PIN_LEN];
    gsdi_app_id_type app_id;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_unblock_pin_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin_id;
    byte             pin[GSDI_PIN_LEN];
    gsdi_app_id_type app_id;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_disable_pin_req_T;

typedef struct {
    gsdi_cmdhdr_T    message_header;
    byte             pin_id;
    byte             pin[GSDI_PIN_LEN];
    gsdi_app_id_type app_id;
    byte             queue_id;
    uint32           client_ref;
}
gsdi_enable_pin_req_T;

typedef struct {
    gsdi_cmdhdr_T message_header;
    byte          illegal_sim;
}
gsdi_illegal_sim_req_T;

typedef struct {
    gsdi_cmdhdr_T message_header;
    byte          call_active;       /* TRUE or FALSE */
}
gsdi_call_state_req_T;


typedef struct {
    gsdi_cmdhdr_T message_header;
    uint32        client_ref;
    void (*gsdi_callback_to_register)(gsdi_sim_events_T);
}
gsdi_register_callback_req_T;


typedef struct {
    gsdi_cmdhdr_T message_header;
    uint32        client_ref;
    byte          esn_usage;
    byte          esn_length;
    dword         esn;
    qword         meid;
    boolean       meid_available;
}gsdi_store_esn_req_T;

/* Set Slot Preference Req Type
*/
typedef struct {
    gsdi_cmdhdr_T message_header;
    byte          queue_id;
    uint32        client_ref;
    gsdi_app_enum_type app;
}gsdi_switch_slot_preference_req_T;

typedef enum
{
  GSDI_PROTOCOL_NONE = 0,
  GSDI_PROTOCOL_GSM,
  GSDI_PROTOCOL_UICC
}gsdi_protocol_enum_type;

/* GSDI Status that indicates whether
** or Not a Slot is present on the Card
*/
/*Set by UIM */
typedef enum{
    GSDI_SLOT_ERROR = 0,
    GSDI_SLOT_PRESENT = 1
}gsdi_slot_status_type;

/*Set by UIM*/
typedef struct {
    boolean cdma_present;
    boolean gsm_present;
}gsdi_dfs_present_type;


/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_GET_ATR_REQ_T
** DESCRIPTION: Used to request the ATR of the slot provided.
** --------------------------------------------------------------------------*/
typedef struct
{
    gsdi_cmdhdr_T           message_header;
    byte                    queue_id;
    uint32                  client_ref;
}gsdi_get_atr_req_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_OPEN_CHANNEL_REQ_T
** DESCRIPTION: Used to open a channel to the card.
** --------------------------------------------------------------------------*/
typedef struct {
    gsdi_cmdhdr_T           message_header;
    gsdi_app_id_type        app_id;
    int32                   channel_id;
    int32                   requested_channel_id;
    byte                    queue_id;
    uint32                  client_ref;
}gsdi_open_channel_req_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_CLOSE_CHANNEL_REQ_T
** DESCRIPTION: Used to close a currently allocated Channel.
** --------------------------------------------------------------------------*/
typedef struct {
    gsdi_cmdhdr_T           message_header;
    int32                   channel_id;
    int32                   requested_channel_id;
    byte                    queue_id;
    uint32                  client_ref;
}gsdi_close_channel_req_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_SEND_CHANNEL_DATA_REQ_T
** DESCRIPTION: Used to send Data to a Channel.
**              The channel_id must be set to a value between 0 and 4.
**              When sent for ATCSIM Commands where a channel may not have been
**              opened prior to using this command, it is ok to set the
**              channel id to 0 to indicate the default channel.
** --------------------------------------------------------------------------*/
typedef struct {
    gsdi_cmdhdr_T           message_header;
    int32                   channel_id;
    int32                   apdu_len;
    uint8                   data_buffer[GSDI_MAX_CHANNEL_DATA_CMD_LEN];
    byte                    queue_id;
    uint32                  client_ref;
} gsdi_send_channel_data_req_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_SEND_TOOLKIT_TLV_REQ_T
** DESCRIPTION: Used to send a RAW Toolkit TLV From the GSTK Task to the
**              AT Command Processor (ATCoP) subsystem within the Data
**              Services Task
** --------------------------------------------------------------------------*/
typedef struct {
    gsdi_cmdhdr_T           message_header;
    int32                   toolkit_apdu_len;
    uint8                   data_buffer[GSDI_MAX_DATA_BLOCK_LEN];
} gsdi_send_toolkit_apdu_req_T;

/* ----------------------------------------------------------------------------
** STRUCTURE:   GSDI_GET_PBR_TABLE_REQ_T
** DESCRIPTION: Used to build a command for GSDI to process.  This req will
**              have MMGSDI copy the PBR Table and send it to the client for
**              its use
** --------------------------------------------------------------------------*/
typedef struct {
    gsdi_cmdhdr_T           message_header;
    byte                    queue_id;
    uint32                  client_ref;
}gsdi_get_pbr_table_req_T;

typedef struct {
    gsdi_cmdhdr_T           message_header;
    gsdi_protocol_enum_type protocol_slot1;
    gsdi_slot_status_type   gsdi_status_slot1;
    gsdi_dfs_present_type   gsdi_dfs_present_slot1;
    uint8                   atr_bytes[GSDI_MAX_ATR_BYTES];
    uint8                   length_atr_bytes;
    uint32                  work_waiting_time;
    gsdi_protocol_enum_type protocol_slot2;
    gsdi_slot_status_type   gsdi_status_slot2;
    gsdi_dfs_present_type   gsdi_dfs_present_slot2;
    uint8                   atr_2_bytes[GSDI_MAX_ATR_BYTES];
    uint8                   length_atr_2_bytes;
    uint8                   historical_bytes_2[GSDI_MAX_HIST_BYTES];
    uint8                   length_historical_bytes_2;
    uint8                   historical_bytes[GSDI_MAX_HIST_BYTES];
    uint8                   length_historical_bytes;
}gsdi_uim_link_established_T;

/*
** Task registration defs
*/

typedef struct
{
   rex_tcb_type   *        tasktcb;
   rex_sigs_type           wait_sig;
   dog_report_type         dog_rpt;
   rex_timer_type *        rpt_timer;
   rex_timer_cnt_type      rpt_timer_value;
}
gsdi_registered_task_info_T;

extern gsdi_registered_task_info_T gsdi_registered_task_list[GSDI_MAX_REGISTERED_TASKS];

/* Structure to store pin information
** Only to be used by the UIM Task
*/
typedef struct {
  gsdi_pin_ids_T     pin_id;
  gsdi_pin_status_T  status;
  boolean            is_pin_available;
  uint8              pin[GSDI_PIN_LEN];
}gsdi_pin_info_T;


/*
** Call Back function
*/

typedef void gsdi_event_callback_funct_type (
    gsdi_sim_events_T   gsdi_event
);

/* Watchdog report timer */
extern rex_timer_type gsdi_rpt_timer;


typedef struct {
    boolean     ef_exists;
    byte        num_of_recs;
    byte        len;
    byte        data[GSDI_MAX_ECC_RECORDS ] [GSDI_MAX_ECC_LEN];
}
gsdi_ef_ecc_data_T;

typedef struct {
  gsdi_cmdhdr_T             message_header;
  gsdi_slot_id_type         slot;
  gsdi_uim_swap_enum_type   status;
}gsdi_uim_swap_T;

extern gsdi_ef_ecc_data_T sim_ecc_data;
extern gsdi_ef_ecc_data_T usim_ecc_data;
extern gsdi_ef_ecc_data_T ruim_ecc_data;

extern gsdi_ef_ecc_data_T sim2_ecc_data;
extern gsdi_ef_ecc_data_T usim2_ecc_data;
extern gsdi_ef_ecc_data_T ruim2_ecc_data;

/* <EJECT> */
/*--------------------------------------------------------------------------
                             Function Prototypes
--------------------------------------------------------------------------*/

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
);


/*===========================================================================
FUNCTION GSDI_REG_CALLBACK

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  gsdi_return_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_reg_callback(
  gsdi_event_callback_funct_type *gsdi_callback_funct
);
/*~ FUNCTION gsdi_reg_callback    */
/*~ ONERROR return GSDI_RPC_ERROR */


/*===========================================================================
FUNCTION GSDI_GET_CARD_MODE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_return_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_get_card_mode(
  gsdi_card_mode_capability_T * card_mode
);
/*~ FUNCTION gsdi_get_card_mode   */
/*~ PARAM OUT card_mode POINTER   */


/*===========================================================================
FUNCTION GSDI_INIT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_init(
  boolean
);
/*~ FUNCTION gsdi_init */


/*===========================================================================
FUNCTION GSDI_GET_SIM_CAPABILITIES

DESCRIPTION
  This function returns certain capabilities of the SIM.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_get_sim_capabilities(
  sim_capabilities_T * sim_capabilities,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);
/*~ FUNCTION gsdi_get_sim_capabilities */
/*~ PARAM OUT sim_capabilities POINTER */
/*~ ONERROR return GSDI_RPC_ERROR      */


/*===========================================================================
FUNCTION GSDI_SIM_SEARCH

DESCRIPTION
  This function performs a search on record based files.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_sim_search(
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  char *               search_string,
  int                  search_string_len,
  int *                rec_num_accessed_ptr,
  byte                 rfu1,
  byte                 rfu2,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);


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
);


/*===========================================================================
FUNCTION GSDI_SIM_READ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_sim_read(
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  char *               search_string,
  int                  search_string_len,
  byte *               data_buffer,
  int                  required_data_len,
  int                  data_offset,
  int *                rec_num_accessed_ptr,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);
/*~ FUNCTION gsdi_sim_read                                                  */
/*~ PARAM search_string VARRAY LENGTH search_string_len                     */
/*~ PARAM OUT data_buffer VARRAY required_data_len LENGTH required_data_len */
/*~ PARAM OUT rec_num_accessed_ptr POINTER                                  */
/*~ ONERROR return GSDI_RPC_ERROR                                           */


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

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_get_image(
  uim_items_enum_type    sim_filename,
  word                 * path,
  uint8                  path_len,
  word                   image_file_offset,
  word                   requested_num_byte,
  word                 * returned_data_len,
  uint8                * data,
  uint32                 client_ref,
  gsdi_async_cb          async_cb
);


/*===========================================================================
FUNCTION GSDI_SIM_WRITE

DESCRIPTION
  This function writes data to a specified file.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_sim_write(
  uim_items_enum_type sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  char *               search_string,
  int                  search_string_len,
  byte *               data_buffer,
  int                  data_len,
  int                  data_offset,
  int *                rec_num_accessed_ptr,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);
/*~ FUNCTION gsdi_sim_write                             */
/*~ PARAM search_string VARRAY LENGTH search_string_len */
/*~ PARAM data_buffer VARRAY LENGTH data_len            */
/*~ PARAM OUT rec_num_accessed_ptr POINTER              */
/*~ ONERROR return GSDI_RPC_ERROR                       */


/*===========================================================================

FUNCTION GSDI_SIM_INCREASE

DESCRIPTION
  This function allows a cyclic type file to be increased by a specified
  amount.

DEPENDENCIES
  None.

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
extern gsdi_returns_T gsdi_sim_increase(
  uim_items_enum_type sim_filename,
  dword               increase_value,
  byte *              new_value_p,
  byte *              new_value_len_p,
  uint32              client_ref,
  gsdi_async_cb       async_cb
);


/*===========================================================================
FUNCTION GSDI_RUN_GSM_ALGORITHM

DESCRIPTION
  This function sends the proper instruction to the GSM SIM to execute
  and return authentication algorithms.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_run_gsm_algorithm(
  byte*          rand,
  byte*          sres,
  byte*          kc,
  uint32         client_ref,
  gsdi_async_cb  async_cb
);
/*~ FUNCTION gsdi_run_gsm_algorithm */
/*~ PARAM rand ARRAY GSDI_RAND_LEN  */
/*~ PARAM sres ARRAY GSDI_SRES_LEN  */
/*~ PARAM kc ARRAY GSDI_KC_LEN      */
/*~ ONERROR return GSDI_RPC_ERROR   */


/*===========================================================================
FUNCTION GSDI_USIM_AUTHENTICATION

DESCRIPTION
  This function sends the proper instruction to the UMTS USIM to execute
  and return authentication algorithms.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_usim_authentication(
  byte *                    rand,
  byte                      rand_length,
  byte *                    autn,
  byte                      autn_length,
  authentication_context_T  auth_cnt,
  byte *                    data_p,
  uint32                    client_ref,
  gsdi_async_cb             async_cb
);
/*~ FUNCTION gsdi_usim_authentication    */
/*~ PARAM rand VARRAY LENGTH rand_length */
/*~ PARAM autn VARRAY LENGTH autn_length */
/*~ PARAM OUT data_p POINTER             */
/*~ ONERROR return GSDI_RPC_ERROR        */


/*===========================================================================
FUNCTION GSDI_GET_FILE_ATTRIBUTES

DESCRIPTION
  This function returns header information of a specific file.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_get_file_attributes(
  uim_items_enum_type      sim_filename,
  gsdi_file_attributes_T * attributes,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);
/*~ FUNCTION gsdi_get_file_attributes */
/*~ PARAM OUT attributes POINTER      */
/*~ ONERROR return GSDI_RPC_ERROR     */


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
extern gsdi_returns_T gsdi_file_select(
  uim_items_enum_type     file_id,
  uim_file_type           file_type,
  word*                   path_p,
  byte                    path_len,
  byte*                   returned_data_len,
  byte*                   returned_data_p,
  uint32                  client_ref,
  gsdi_async_cb           async_cb
);
/*~ FUNCTION gsdi_file_select           */
/*~ PARAM OUT path_p POINTER            */
/*~ PARAM OUT returned_data_len POINTER */
/*~ PARAM OUT returned_data_p POINTER   */
/*~ ONERROR return GSDI_RPC_ERROR       */


/*===========================================================================
FUNCTION GSDI_GET_PIN_STATUS

DESCRIPTION
  This function returns the status of PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_get_pin_status(
  gsdi_pin_ids_T       pin_id,
  gsdi_pin_status_T *  pin_status,
  byte *               num_retry_p,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);
/*~ FUNCTION gsdi_get_pin_status  */
/*~ PARAM OUT pin_status POINTER  */
/*~ PARAM OUT num_retry_p POINTER */
/*~ ONERROR return GSDI_RPC_ERROR */


/*===========================================================================
FUNCTION GSDI_VERIFY_PIN

DESCRIPTION
  This function performs a pin verification procedure against the SIM for
  either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_verify_pin(
  gsdi_pin_ids_T  pin_id,
  char *          pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
);
/*~ FUNCTION gsdi_verify_pin         */
/*~ PARAM pin ARRAY 8                */
/*~ PARAM OUT num_retry_left POINTER */
/*~ ONERROR return GSDI_RPC_ERROR    */


/*===========================================================================
FUNCTION GSDI_CHANGE_PIN

DESCRIPTION
  This function attempts to change an old pin (PIN1 or PIN2) with a new pin.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_change_pin(
  gsdi_pin_ids_T  pin_id,
  char *          old_pin,
  char *          new_pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
);
/*~ FUNCTION gsdi_change_pin         */
/*~ PARAM old_pin ARRAY 8            */
/*~ PARAM new_pin ARRAY 8            */
/*~ PARAM OUT num_retry_left POINTER */
/*~ ONERROR return GSDI_RPC_ERROR    */


/*===========================================================================
FUNCTION GSDI_UNBLOCK_PIN

DESCRIPTION
  This function attempts to unblock either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_unblock_pin(
  gsdi_pin_ids_T  pin_id,
  char *          puk,
  char *          new_pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
);
/*~ FUNCTION gsdi_unblock_pin        */
/*~ PARAM puk ARRAY 8                */
/*~ PARAM new_pin ARRAY 8            */
/*~ PARAM OUT num_retry_left POINTER */
/*~ ONERROR return GSDI_RPC_ERROR    */


/*===========================================================================
FUNCTION GSDI_DISABLE_PIN

DESCRIPTION
  This function attempts to disable either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_disable_pin(
  gsdi_pin_ids_T  pin_id,
  char *          pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
);
/*~ FUNCTION gsdi_disable_pin        */
/*~ PARAM pin ARRAY 8                */
/*~ PARAM OUT num_retry_left POINTER */
/*~ ONERROR return GSDI_RPC_ERROR    */


/*===========================================================================
FUNCTION GSDI_ENABLE_PIN

DESCRIPTION
  This function attempts to enable either PIN1 or PIN2.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_enable_pin(
  gsdi_pin_ids_T  pin_id,
  char *          pin,
  byte *          num_retry_left,
  uint32          client_ref,
  gsdi_async_cb   async_cb
);
/*~ FUNCTION gsdi_enable_pin         */
/*~ PARAM pin ARRAY 8                */
/*~ PARAM OUT num_retry_left POINTER */
/*~ ONERROR return GSDI_RPC_ERROR    */


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_ENABLE_FDN

DESCRIPTION
  This function enables FDN.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_enable_fdn (
  boolean  pin2_present,
  char *   pin2
);


/* <EJECT> */
/*===========================================================================
FUNCTION GSDI_DISABLE_FDN

DESCRIPTION
  This function disables FDN.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
gsdi_returns_T gsdi_disable_fdn (
  boolean  pin2_present,
  char *   pin2
);


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
);
/*~ FUNCTION gsdi2_enable_fdn                     */
/*~ PARAM pin2 VARRAY LENGTH (pin2_present ? 8:0) */
/*~ PARAM aid POINTER                             */
/*~ ONERROR return GSDI_RPC_ERROR                 */


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
);
/*~ FUNCTION gsdi2_disable_fdn                    */
/*~ PARAM pin2 VARRAY LENGTH (pin2_present ? 8:0) */
/*~ PARAM aid POINTER                             */
/*~ ONERROR return GSDI_RPC_ERROR                 */


/*===========================================================================
FUNCTION GSDI_ILLEGAL_SIM

DESCRIPTION
  This function is called to mark the SIM as an illegal SIM.  Subsequent,
  request to the GSDI will be blocked.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_illegal_sim(
  boolean
);


/*===========================================================================

FUNCTION GSDI_SIM_GET_ECC

DESCRIPTION
  This function will be used to get the Emergency Call Codes retrieved
    by the GSDI during the GSDI & SIM Initialization Procedures.

    This function will populate a gsdi_ecc_data_T structure.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
extern gsdi_returns_T gsdi_sim_get_ecc(
  gsdi_ef_ecc_data_T * sim_ecc
);


/*===========================================================================

FUNCTION GSDI2_SIM_GET_ECC

DESCRIPTION
  This function will be used to get the Emergency Call Codes retrieved
  by the GSDI during the GSDI & SIM Initialization Procedures.

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
extern gsdi_returns_T gsdi2_sim_get_ecc(
  gsdi_slot_id_type slot,
  gsdi_ef_ecc_data_T * sim_ecc
);


/*===========================================================================

FUNCTION GSDI_USIM_GET_ECC

DESCRIPTION
  This function will be used to get the Emergency Call Codes retrieved
    by the GSDI during the GSDI & USIM Initialization Procedures.

    This function will populate a gsdi_ecc_data_T structure.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
extern gsdi_returns_T gsdi_usim_get_ecc(
  gsdi_ef_ecc_data_T * usim_ecc
);


/*===========================================================================

FUNCTION GSDI2_USIM_GET_ECC

DESCRIPTION
  This function will be used to get the Emergency Call Codes retrieved
  by the GSDI during the GSDI & USIM Initialization Procedures.

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
extern gsdi_returns_T gsdi2_usim_get_ecc(
  gsdi_slot_id_type slot,
  gsdi_ef_ecc_data_T * usim_ecc
);


/*===========================================================================

FUNCTION GSDI_RUIM_GET_ECC

DESCRIPTION
  This function will be used to get the Emergency Call Codes retrieved
    by the GSDI during the GSDI & RUIM Initialization Procedures.

    This function will populate a gsdi_ecc_data_T structure.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
extern gsdi_returns_T gsdi_ruim_get_ecc(
  gsdi_ef_ecc_data_T * ruim_ecc
);


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
extern gsdi_returns_T gsdi2_ruim_get_ecc(
  gsdi_slot_id_type slot,
  gsdi_ef_ecc_data_T * ruim_ecc
);


/*===========================================================================

FUNCTION GSDI_SIM_GSM_SEEK

DESCRIPTION
  This function will perform a GSM Seek.  The type of seek and method
  in which it is executed will be dependent on the parameters passed
  when this API Is Called.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
extern gsdi_returns_T gsdi_sim_gsm_seek(
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  char *               search_string,
  int                  search_string_len,
  int *                rec_num_accessed_p,
  byte                 rfu1,
  byte                 rfu2,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);


/*===========================================================================

FUNCTION GSDI_USIM_UMTS_SEARCH

DESCRIPTION
  This function will perform a GSM Seek.  The type of seek and method
  in which it is executed will be dependent on the parameters passed
  when this API Is Called.

DEPENDENCIES
  None

RETURN VALUE
  gs_returns_T.

SIDE EFFECTS
  None

===========================================================================*/
extern gsdi_returns_T gsdi_usim_umts_search(
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int          search_record_num,
  uim_srch_mode    search_mode,
  uim_srch_dir_type    search_direction,
  char *               search_string,
  int                  search_string_len,
  int *                rec_num_accessed_p,
  byte                 rfu1,
  byte                 rfu2,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);


/*===========================================================================
FUNCTION gsdi_cmd_ext

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_cmd_ext(
  void *cmd
);


/*===========================================================================
FUNCTION gsdi_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void gsdi_cmd(
  void *cmd
);


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
extern gsdi_returns_T gsdi_register_callback_function (
  gsdi_event_callback_funct_type *gsdi_callback_function_to_register,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);


/*===========================================================================
FUNCTION GSDI_HANDLE_ASYNC_LIB_REQ

DESCRIPTION
   This function will serve as the Callback Function from the GSDI to
   itself.  The purpose of this function is to discard the the Message
   generated to updating Critical Data.

DEPENDENCIES
   FEATURE_GSDI_MULTICLIENT_SUPPORT, FEATURE_UI, FEATURE_DATA

RETURN VALUE
   gsdi_returns_T
===========================================================================*/
void gsdi_handle_async_lib_rsp( gsdi_cnf_T *gsdi_cnf );


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
);
/*~ FUNCTION gsdi_lib_is_service_available */


/*===========================================================================
FUNCTION GSDIMAIN_LEGACY

DESCRIPTION
  Entry point for the MM task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdimain_legacy (
  rex_sigs_type                       sigs_rcvd,
  rex_sigs_type                       sigs
);


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
  uim_items_enum_type EF,

  /* Pointer to Length Byte to populate */
  int*               length_p
);
/*~ FUNCTION gsdi_get_ef_length   */
/*~ PARAM OUT length_p POINTER    */


/*===========================================================================
FUNCTION GSDI_CM_IN_TRAFFIC_CHANNEL

DESCRIPTION

  This function is a utility function for CM to notify GSDI when the Mobile is
  in a traffic channel.  It will also be used to indicate when the Mobile
  is no longer in a traffic channel.

  When called, this function (which executes in the context of the caller) will
  vote the TC Bit in the UIM Drivers On or Off.

  When TC is Voted on in uim_power_control, the UIM Drivers will POLL the SIM.
  When TC is Voted off in uim_power_control, the UIM will not POLL the SIM..

DEPENDENCIES
  None

RETURN VALUE

  void

===========================================================================*/
void gsdi_cm_in_traffic_channel (

  /* TRUE:  Indicates CM is in Traffic Channel (Call is up) */
  /* FALSE: Indicates CM has torn down all calls            */
  boolean in_TC
);


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

  /* TRUE:  Indicates CM is in Traffic Channel (Call is up) */
  /* FALSE: Indicates CM has torn down all calls            */
  boolean in_TC,

  /* Mode of Call will indicate which APP/Card to Poll */
  gsdi_in_tc_mode_enum_type mode
);


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
);


/*===========================================================================
FUNCTION GSDI_UTIL_IS_GSDI_CACHE_READY

DESCRIPTION

  This utility function is a called by the gsdi Library to check if the GSDI
  has cached all necessary data.  This function is a necessity for
  GSDI Optimization and will be used to make sure cached items are not accessed
  until they are all ready.

  When called, this function (which executes in the context of the caller)
  checks the gsdi_data.cache_ready Boolean.


DEPENDENCIES
  None

RETURN VALUE
  Boolean:
    if gsdi_data.cache_ready = TRUE, the function returns TRUE
    if gsdi_data.cache_ready = FALSE, the function returns FALSE

===========================================================================*/
boolean gsdi_util_is_gsdi_cache_ready (
  void
);


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
extern gsdi_returns_T gsdi_sim_refresh(
  gsdi_refresh_modes_enum_type refresh_mode,
  uint8                        num_files,
  uint8                        data_len,
  uint8 *                      data_p,
  uint32                       client_ref,
  gsdi_async_cb                async_cb
);


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
extern gsdi_returns_T gsdi_naa_refresh(
  gsdi_slot_id_type            slot,
  gsdi_refresh_modes_enum_type refresh_mode,
  gsdi_app_id_type             app_id,
  uint8                        num_files,
  uint8                        data_len,
  uint8 *                      data_ptr,
  uint32                       client_ref,
  gsdi_async_cb                async_cb
);


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
extern void gsdi_get_apps_available( gsdi_card_apps_T * card_apps );
/*~ FUNCTION gsdi_get_apps_available */
/*~ PARAM OUT card_apps POINTER      */


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
SIDE EFFECTS
  Client task will receive notifications in which it is responsible to
  analyze to decide (vote) whether to allow the REFRESH To Occur.
===========================================================================*/
extern gsdi_returns_T gsdi_register_for_refresh_voting(
  gsdi_refresh_vote_cb_type  * vote_cb_p,
  uint32                       client_ref,
  gsdi_async_cb                async_cb
);


/*===========================================================================
FUNCTION GSDI_REGISTER_FOR_REFRESH_VOTING

DESCRIPTION
  This Library Function is used by the calling task to register a callback
  for MMGSDI to call when a File Change Notification is made available.  The
  file change notification will consist of a list of files directed at the
  client that registers for the files to be read.

DEPENDENCIES:
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS - Successfully added to the file Change
                                  Notification Table.
                   GSDI_ERROR   - Failure to add client to the File Change
                                  Notification Table.
SIDE EFFECTS
  Client task will receive notifications in which it is responsible for
  properly reading the files it has registered for.
===========================================================================*/
extern gsdi_returns_T gsdi_register_for_file_change_notifications(
  gsdi_task_notify_file_list_cb_type * file_notify_cb_p,
  uint8                                num_files,
  uim_items_enum_type                * file_list_p,
  uint32                               client_ref,
  gsdi_async_cb                        async_cb
);
/*~ FUNCTION gsdi_register_for_file_change_notifications */
/*~ PARAM file_list_p VARRAY LENGTH num_files            */
/*~ ONERROR return GSDI_RPC_ERROR                        */


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
                                  notification.
SIDE EFFECTS
  Client will receive notifications for file changes and must
  re-read the files if it receives one.
===========================================================================*/
extern gsdi_returns_T gsdi_refresh_complete (
  uint8                 num_files,
  uim_items_enum_type * file_list_p,
  boolean               pass_fail,
  uint32                client_ref,
  gsdi_async_cb         async_cb
);
/*~ FUNCTION gsdi_refresh_complete            */
/*~ PARAM file_list_p VARRAY LENGTH num_files */
/*~ ONERROR return GSDI_RPC_ERROR             */


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
extern gsdi_returns_T gsdi_naa_refresh_complete (
  gsdi_slot_id_type            slot_id,
  gsdi_refresh_modes_enum_type refresh_mode,
  gsdi_app_id_type             app_id,
  uint8                        num_files,
  uim_items_enum_type        * file_list_ptr,
  boolean                      pass_fail,
  uint32                       client_ref,
  gsdi_async_cb                async_cb
);
/*~ FUNCTION gsdi_naa_refresh_complete          */
/*~ PARAM file_list_ptr VARRAY LENGTH num_files */
/*~ ONERROR return GSDI_RPC_ERROR               */

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
extern gsdi_returns_T gsdi_sim_read_ext (
  gsdi_slot_id_type    slot,
  uim_items_enum_type  sim_filename,
  gsdi_index_modes_T   index_mode,
  int                  rec_num,
  int                  required_data_len,
  int                  data_offset,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);


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
extern gsdi_returns_T gsdi2_get_file_attributes(
  gsdi_slot_id_type        slot,
  uim_items_enum_type      sim_filename,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


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
extern gsdi_returns_T gsdi2_get_pin_status(
  gsdi_slot_id_type         slot,
  gsdi_pin_ids_T            pin_id,
  gsdi_app_id_type       *  aid,
  uint32                    client_ref,
  gsdi_async_cb             async_cb
);


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
extern gsdi_returns_T gsdi2_verify_pin(
  gsdi_slot_id_type        slot,
  gsdi_pin_ids_T           pin_id,
  char                   * pin,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


/*===========================================================================
FUNCTION GSDI2_SIM_READ

DESCRIPTION
        This function will generate a Message for GSDI to read the SIM/RUIM or USIM
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
  gsdi_returns_T:
        // ADD RETURN VALUES

SIDE EFFECTS
        NONE

LIMITATIONS
        Only supported in Asynchronous Mode.
===========================================================================*/
extern gsdi_returns_T gsdi2_sim_read(
  gsdi_slot_id_type        slot,
  uim_items_enum_type      sim_filename,
  gsdi_index_modes_T       index_mode,
  int                      rec_num,
  int                      required_data_len,
  int                      data_offset,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


/*===========================================================================
FUNCTION GSDI2_SIM_WRITE

DESCRIPTION
  This function writes data to a specified file in the GSM/RUIM/USIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T

SIDE EFFECTS
  None

LIMITATIONS
        Only supported in Asynchronous Mode.
===========================================================================*/
extern gsdi_returns_T gsdi2_sim_write(
  gsdi_slot_id_type        slot,
  uim_items_enum_type      sim_filename,
  gsdi_index_modes_T       index_mode,
  int                      rec_num,
  byte                   * data_buffer,
  int                      data_len,
  int                      data_offset,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


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
extern gsdi_returns_T gsdi2_get_card_mode(
  gsdi_slot_id_type             slot,
  gsdi_card_mode_capability_T * card_mode
);


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
extern gsdi_returns_T gsdi2_enable_pin(
  gsdi_slot_id_type        slot,
  gsdi_pin_ids_T           pin_id,
  char                   * pin,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


/*===========================================================================
FUNCTION GSDI_DISABLE_PIN

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
extern gsdi_returns_T gsdi2_disable_pin(
  gsdi_slot_id_type       slot,
  gsdi_pin_ids_T          pin_id,
  char                  * pin,
  gsdi_app_id_type      * aid,
  uint32                  client_ref,
  gsdi_async_cb           async_cb
);


/*===========================================================================
FUNCTION GSDI@_UNBLOCK_PIN

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
extern gsdi_returns_T gsdi2_unblock_pin(
  gsdi_slot_id_type        slot,
  gsdi_pin_ids_T           pin_id,
  char                   * puk,
  char                   * new_pin,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


/*===========================================================================
FUNCTION GSDI2_CHANGE_PIN

DESCRIPTION
  This function attempts to cahnge either PIN1 or PIN2 for the required Slot.
  This function will only support asynchronous calls.  Thus, the caller will
  need to provide a callback function.

        Parameter Information
    1.  gsdi_slot_id_type slot [ INPUT ] - MANDATORY
                    GSDI_SLOT_NONE: select and use slot 1
                    GSDI_SLOT_1:        select and use slot 1
                    GSDI_SLOT_2:        select and use slot 2
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
gsdi_returns_T gsdi2_change_pin (
  gsdi_slot_id_type        slot,
  gsdi_pin_ids_T           pin_id,
  char                   * old_pin,
  char                   * new_pin,
  gsdi_app_id_type       * aid,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


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
gsdi_returns_T gsdi2_get_sim_capabilities (
  gsdi_slot_id_type         slot,
  gsdi_app_id_type       *  aid,
  uint32                    client_ref,
  gsdi_async_cb             async_cb
);


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
);


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
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi2_switch_slot_preference(
  gsdi_slot_id_type  slot,
  gsdi_app_enum_type app,
  uint32             client_ref,
  gsdi_async_cb      async_cb
);


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
  gsdi_slot_id_type *   slot
);


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
  gsdi_async_cb                async_cb
);


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
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi2_send_apdu(
  gsdi_slot_id_type        slot,
  gsdi_app_id_type       * aid,
  uint32                   apdu_len,
  uint8                  * apdu_data_p,
  uint32                   client_ref,
  gsdi_async_cb            async_cb
);


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
  gsdi_returns_T

SIDE EFFECTS
  None
===========================================================================*/
extern gsdi_returns_T gsdi_nas_get_card_mode(
  gsdi_card_mode_capability_T * card_mode
);


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
extern gsdi_returns_T gsdi_sim_get_atr(
  gsdi_slot_id_type   slot,
  uint32              client_ref,
  gsdi_async_cb       gsdi_asnyc_cb
);
/*~ FUNCTION gsdi_sim_get_atr     */
/*~ ONERROR return GSDI_RPC_ERROR */


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
  gsdi_aid_type *   app_id_p        Pointer to Application ID Buffer.
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
extern gsdi_returns_T gsdi_sim_open_channel(
  gsdi_slot_id_type   slot,
  gsdi_app_id_type *  app_id_p,
  int32               channel_id,
  int32               requested_channel_id,
  uint32              client_ref,
  gsdi_async_cb       async_cb
);
/*~ FUNCTION gsdi_sim_open_channel */
/*~ PARAM app_id_p POINTER         */
/*~ ONERROR return GSDI_RPC_ERROR  */


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
    uint32            requested_channel_id:  Channel to be closed
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
extern gsdi_returns_T   gsdi_sim_close_channel(
  gsdi_slot_id_type   slot,
  int32               channel_id,
  int32               requested_channel_id,
  uint32              client_ref,
  gsdi_async_cb       async_cb
);
/*~ FUNCTION gsdi_sim_close_channel */
/*~ ONERROR return GSDI_RPC_ERROR   */


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
  int32               channel_id:    Channel to transport data on
    int32             data_len:      Length of Data Provided
    byte              apdu_buffer_p: Buffer that contains APDU
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
extern gsdi_returns_T gsdi_sim_send_channel_data(
  gsdi_slot_id_type   slot,
  int32               channel_id,
  int32               apdu_len,
  uint8 *             apdu_buffer_p,
  uint32              client_ref,
  gsdi_async_cb       gsdi_asnyc_cb
);
/*~ FUNCTION gsdi_sim_send_channel_data            */
/*~ PARAM apdu_buffer_p
    VARRAY GSDI_MAX_DATA_BLOCK_LEN LENGTH apdu_len */
/*~ ONERROR return GSDI_RPC_ERROR                  */


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
extern gsdi_returns_T gsdi_perso_ota_deperso(
  gsdi_perso_ota_deperso_type * sms_ota_user_data,
  uint32                        client_ref,
  gsdi_async_cb                 async_cb
);


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
extern gsdi_returns_T gsdi_perso_register_task(
  gsdi_perso_events_cb          perso_events_cb,
  uint32                        client_ref,
  gsdi_async_cb                 async_cb
);
/*~ FUNCTION gsdi_perso_register_task */
/*~ ONERROR return GSDI_RPC_ERROR     */


/*===========================================================================
FUNCTION GSDI_PERSO_ACTIVATE_FEATURE_INDICATOR

DESCRIPTION
  This function is called to activate a personalization feature by an external
  source.  The Feature and Control Key are required.

  When called for the first time, the control key provided will overwrite the
  default (initial) control key stored in NV.  The control key defaulted is
  as follows:  Bytes[1:17] = 0xFF

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
extern gsdi_returns_T gsdi_perso_activate_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
);
/*~ FUNCTION gsdi_perso_activate_feature_indicator */
/*~ PARAM control_key_info POINTER                 */
/*~ ONERROR return GSDI_RPC_ERROR                  */


/*===========================================================================
FUNCTION GSDI_PERSO_DEACTIVATE_FEATURE_INDICATOR

DESCRIPTION
  This function is called to deactivate a personalization feature by an external
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
extern gsdi_returns_T gsdi_perso_deactivate_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
);
/*~ FUNCTION gsdi_perso_deactivate_feature_indicator */
/*~ PARAM control_key_info POINTER                   */
/*~ ONERROR return GSDI_RPC_ERROR                    */


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
extern gsdi_returns_T gsdi_perso_get_feature_indicators(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     async_cb
);
/*~ FUNCTION gsdi_perso_get_feature_indicators */
/*~ ONERROR return GSDI_RPC_ERROR              */


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
extern gsdi_returns_T gsdi_perso_set_feature_data(
  gsdi_perso_set_feature_data_type * perso_codes,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
);
/*~ FUNCTION gsdi_perso_set_feature_data */
/*~ PARAM perso_codes POINTER            */
/*~ ONERROR return GSDI_RPC_ERROR        */


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

      GSDI_SUCCESS: The command was successfully Queued onto the
                   GSDI Command Queue.
                   The parameters passed basic verification.

      GSDI_ERROR:  Indicates one of the parameters required was
                   not properly set.

      Note..the return status of the actual request will be provided
      in the gsdi_cnf_T.

===========================================================================*/
extern gsdi_returns_T gsdi_perso_get_feature_data(
  gsdi_perso_enum_type feature_ind,
  gsdi_slot_id_type    slot,
  uint32               client_ref,
  gsdi_async_cb        async_cb
);
/*~ FUNCTION gsdi_perso_get_feature_data */
/*~ ONERROR return GSDI_RPC_ERROR        */


/*===========================================================================
FUNCTION GSDI_PERSO_PERM_DISABLE_FEATURE_INDICATOR

DESCRIPTION
  This function is called to permanently disable a personalization feature by an
  external source.  The Feature and Control Key are required.

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
extern gsdi_returns_T gsdi_perso_perm_disable_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
);
/*~ FUNCTION gsdi_perso_perm_disable_feature_indicator */
/*~ PARAM control_key_info POINTER                     */
/*~ ONERROR return GSDI_RPC_ERROR                      */


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
);


/*===========================================================================
FUNCTION GSDI_PERSO_GET_DCK_NUM_RETRIES

DESCRIPTION
  This function is called to get the number of depersonalization attempts
  remaining.  Everytime a Depersonalization attempt is made and it fails,
  the number of attempts remaining will also decrement.  Therefore, this
  function can be used to check the number of retries remaining.  This
  function will return the Number of Retries for All Features regardless
  of the feature indicator state.

  INPUT:          uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_slot_id_type slot
                      Slot info on which this command is sent

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
);
/*~ FUNCTION gsdi_perso_get_dck_num_retries */
/*~ ONERROR return GSDI_RPC_ERROR           */


/*===========================================================================
FUNCTION GSDI_PERSO_GET_DCK_UNBLOCK_RETRIES

DESCRIPTION
  This function is called to get the number of unblock on DCK attempts
  remaining.  Everytime an unblock Depersonalization attempt is made and it 
  fails, the number of ublock attempts remaining will also decrement.  
  Therefore, this function can be used to check the number of unblock 
  retries remaining.  This function will return the Number of unblock 
  Retries for All Features regardless of the feature indicator state.

  INPUT:          uint32 client_ref
                      User Data the calling task can send with the request
                      that is returned in the resulting confirmation /
                      response regardless of whether the command succeeds
                      or fails.

                  gsdi_slot_id_type slot
                      Slot info on which this command is sent

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
);
/*~ FUNCTION gsdi_perso_get_dck_unblock_retries */
/*~ ONERROR return GSDI_RPC_ERROR           */


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
extern gsdi_returns_T gsdi_perso_unblock_feature_indicator(
  gsdi_perso_control_key_data_type * control_key_info,
  uint32                             client_ref,
  gsdi_async_cb                      async_cb
);
/*~ FUNCTION gsdi_perso_unblock_feature_indicator */
/*~ PARAM control_key_info POINTER                */
/*~ ONERROR return GSDI_RPC_ERROR                 */


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
);


/*===========================================================================
FUNCTION GSDI_PERSO_CHANGE_PHONECODE

DESCRIPTION
  This function is called to change the phone code

  INPUT:
  int32  old_phone_code_len     Phone code len
  uint8  *old_phone_code_ptr    Old phone code
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
gsdi_returns_T gsdi_perso_change_phonecode(
  int32            old_phone_code_len,
  uint8           *old_phone_code_ptr,
  int32            new_phone_code_len,
  uint8           *new_phone_code_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
);


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
);


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
gsdi_returns_T gsdi_perso_change_dck(
  gsdi_perso_change_control_key_data_type       * change_dck_info_p,
  uint32                                          client_ref,
  gsdi_async_cb                                   async_cb
);


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
);


/*===========================================================================
FUNCTION GSDI_UNBLOCK_PHONE_CODE

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
gsdi_returns_T gsdi_unblock_phone_code(
  int32            master_key_len,
  uint8           *master_key_ptr,
  int32            new_phone_code_len,
  uint8           *new_phone_code_ptr,
  uint32           client_ref,
  gsdi_async_cb    async_cb
);


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
);


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
);


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
);
/*~ FUNCTION gsdi_sim_get_cphs_information */
/*~ ONERROR return GSDI_RPC_ERROR          */


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
  uim_items_enum_type *uim_file_item_p
);
/*~ FUNCTION gsdi_util_map_file_enum_to_uim_enum */
/*~ PARAM OUT uim_file_item_p POINTER            */


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
extern gsdi_returns_T gsdi_util_map_uim_enum_to_file_enum(
  uim_items_enum_type uim_file_item_p ,
  gsdi_file_enum_type *gsdi_file_item
);
/*~ FUNCTION gsdi_util_map_uim_enum_to_file_enum */
/*~ PARAM OUT gsdi_file_item POINTER             */


/*===========================================================================
FUNCTION: gsdi_util_get_pin

DESCRIPTION
  Get PIN information for all applications from GSDI cache.

INPUT
  - pointer to Application Id
  - Pin Id
  - Pointer to pin_info

RETURN VALUE
  none

OUTPUT
  - Pin_info for particular a application are populated with data from the
    GSDI table.

===========================================================================*/
extern gsdi_returns_T gsdi_util_get_pin(
  gsdi_app_id_type *app_id,
  gsdi_pin_ids_T    pin_id,
  gsdi_pin_info_T  *pin_info
);


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
extern gsdi_returns_T gsdi_sim_generic_search (
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
);


/*===========================================================================
FUNCTION GSDI_GET_ECC

DESCRIPTION
  This function will perform get ECC

INPUT
  gsdi_slot_id_type slot:           Slot to send the cmd to
  uint32            client_ref:     User Data returned in response
  gsdi_async_cb     gsdi_asnyc_cb:  Callback GSDI Calls to provide gsdi_cnf_T

LIMITATIONS
  API is only supported in Asynchronous Mode.

DEPENDENCIES
  None

OUTPUT

RETURN VALUE
  gsdi_returns_T gsdi_status - GSDI_SUCCESS
                             - GSDI_ERROR
===========================================================================*/
extern gsdi_returns_T gsdi_get_ecc (
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     gsdi_asnyc_cb
);


/*===========================================================================
FUNCTION GSDI_GET_PHONEBOOK_REFERENCE_TABLE

DESCRIPTION
  This function will retrieve the Phonebook Reference Table information stored
  in MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

LIMITATIONS
    Only supported in Asynchronous Mode.
===========================================================================*/
extern gsdi_returns_T gsdi_get_phonebook_reference_table(
  gsdi_slot_id_type slot,
  uint32            client_ref,
  gsdi_async_cb     gsdi_asnyc_cb
);


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
===========================================================================*/
extern boolean gsdi2_phonebook_present(
  gsdi_slot_id_type   slot,
  gsdi_phonebook_type pbm
);


/*===========================================================================
FUNCTION GSDIMAIN

DESCRIPTION
  Entry point for the MM task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdimain (
  dword dummy
  /* Parameter required for REX.  Tell lint to ignore it. */
  /*lint -esym(715,dummy) */
);


/*===========================================================================
FUNCTION GSDI_UIM_NOTIFY_RESET

DESCRIPTION
  Function is called by UIM Server to report reset status of the card. This
  will signal GSDI to wait for a Link Established message in case of reset init
  or to proceed if reset complete.

INPUT
  reset_status - uninitialized/initialized.

RETURN VALUE
  Void

OUTPUT
  Triggers GSDI to signal itself to go into waiting state if reset_status is
  uninitialized or to signal itself to proceed in case of the reset_status being
  initialized.
===========================================================================*/
void gsdi_uim_notify_reset(
  uim_status_type reset_status
);


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
  gsdi_async_cb     gsdi_async_cb
);


/*===========================================================================
FUNCTION GSDI_GET_PROTOCOL

DESCRIPTION
  This function returns the protocol for the particular slot.

DEPENDENCIES
  None

RETURN VALUE
  GSDI_SUCCESS - if protocol can be returned successfully
  GSDI_ERROR   - in case of error
===========================================================================*/
gsdi_returns_T gsdi_get_protocol (
  gsdi_slot_id_type        slot,
  gsdi_protocol_enum_type *protocol
);


/*===========================================================================
FUNCTION GSDI_CONVERT_DETAILED_ERR_TO_GEN_ERROR

DESCRIPTION
  This function will convert the detailed card errors event resulting from
  FEATURE_MMGSDI_CARD_ERROR_INFO to a generic GSDI_CARD_ERROR event for clients
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
void gsdi_convert_detailed_err_evt_to_gen_err(
  gsdi_sim_events_T   in_err,
  gsdi_sim_events_T * out_err
);
/*~ FUNCTION gsdi_convert_detailed_err_evt_to_gen_err */
/*~ PARAM OUT out_err POINTER                         */


/*===========================================================================
FUNCTION GSDI_PERSO_GET_FEATURE_INDICATOR_KEY

DESCRIPTION
  This API is used to get the feature Indicator key.
  The feature key will be sent back in the callback function and the data
  will be available in gsdi_perso_get_feature_key_cnf_T

  Parameters
  gsdi_slot_id_type slot - slot information
  gsdi_perso_enum_type feature_ind - feature indicator
  boolean is_unblock_key - if retriving unblock key then set to TRUE
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
);
/*~ FUNCTION gsdi_perso_get_feature_indicator_key */
/*~ ONERROR return GSDI_RPC_ERROR                 */


#endif /* GSDI_EXPORT_H */

