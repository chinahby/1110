/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               N V    R - U I M    S U B S Y S T E M

GENERAL DESCRIPTION
  Provides I/O services for NVM items on the R-UIM.

EXTERNALIZED FUNCTIONS
  nvruim_read
    Read an NVM item on the R-UIM

  nvruim_write
    Write an NVM item on the R-UIM

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2010 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/nvruim.c_v   1.33   05 Sep 2002 08:19:14   tsummers  $
$Header: //source/qcom/qct/modem/uim/nvruim/rel/07H1_2/src/nvruim.c#31 $ $DateTime: 2011/03/30 20:35:04 $ $Author: sratnu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/11   ssr     Fixed legacy PPP NV items(NV_PPP_USER_ID_I and NV_PPP_PASSWORD_I) 
                   to return the NV value instead of the card profile 0 for OMH card.
03/09/11   ssr     Fixed compilation error
06/28/10   ssr     Fixed FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED
05/21/10   ssr     Add non 3gpd cdma card check
04/30/10   ssr     Fixed nvruim clear cache
04/23/10   ssr     Fixed UIM 3GPD NV support
04/14/10   yk      Fixed UIM dog fatal due to a typo in access api 
03/25/10   ssr     Added nvruim_data_3gpd_ext_support
03/22/10   nmb     Removed eight extra 0's from nvruim_an_cave_nai 
02/19/10   ssr     Fixed NV_SMS_MO_ON_TRAFFIC_CHANNEL_I reading
01/14/10   ssr     Set ESN usage to USE_ESN for RUIM mode
08/13/09   yb      Fixed compilation warning
07/27/09   yb      Fixed bad RUIM card issue with HRPD Service n5 in CDMA 
                   Service Table improperly set
07/02/09   ssr     Fixed Compilation Error
07/02/09   ssr     Operator MCC Check for 3GPD
06/22/09   ssr     Fixed Linker Error
05/20/09   ps      Fixed Diag crash during QPST read in RUIM
05/18/09   ssr     Fixed compilation warning.
04/22/09   ssr     Initialize the nv buffer to 0x00 for sippapss.
04/22/09   ssr     Fixed  PRL validity code for FEATURE_2ND_PRL
04/06/09   ssr     Fixed IPv4 and IPv6 address parsing.
03/20/09   ssr     Allow task contexts other than NV to enter nvruim_read 
                   and nvruim_write
03/21/09   ssr     Fixed sippapss parsing
03/26/09   rm      Fix for a compilation error
03/23/09   rm      Fix for dog reset due to slow sim response
03/16/09   ssr     Converting 8 Bit ASCII number to hex for PDE and MPC server
                   address
02/24/09   ssr     Fixed Compilation warning.
02/16/09   ssr     Fixed QTF Lint Error: Not Referenced
02/10/09   ssr     Changed default NAI domain name for  China Telecom 
02/03/09   ssr     Removed EF_MDN checked as mandatory file
11/26/08   nb      Klockwork Fixes
11/19/08   nb      Klockwork Fixes
10/22/08   ssr     Fixed lint errors
10/14/08   jk      Fixed Compile Warnings
09/05/08   ssr     Fixed nvsim_read fucntion parameter
09/02/08   ssr     Fixed Multiple profile support for SIPUPP and SIPAPASS
06/23/08   ssr     Fixed first prl size while reading second prl.
06/20/08   ssr     Support of OMH LBS Items
05/16/08   jk      Fixed featurization of SIP NV items
05/05/08   sun     Fixed calculation for pap password length
05/01/08   nk      Fixed and added Messages to nvruim_clear_cache
04/29/08   vs      Fixed lint errors
04/25/08   ssr     Support of OMH Data Item
04/21/08   nk/jk   Fixed Parsing of 3GPDOPM Data, Added Sanity Checks
03/26/08   jk      OMH Phase 1B Support - Multiple SIP Profiles
02/26/08   jk      Added additional handling of SMS items as per OMH POC Request
02/06/08   nk      Added CDMA Refresh Support
02/04/08   nk      Fixed Compilation Warning
12/13/07   vs      Updated function nvruim_read_smscap() based on OMH spec
11/08/07   vs/jk   Fixed 3GPD featurization related compiler errors and EUIMID
                   related lint errors
10/16/07   jk      Fixed Bug for BC-SMS User Config Write
10/10/07   jk      Code changes to support Open Market Handset
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
08/03/07   nk      Updated read on esn and meid for JCDMA and EUIMID support
07/30/07   nk      Updated nvruim_read_esn for MEID support 
07/10/07   nk      Added EUIMID Support
05/11/07   wli     Changes for JCDMA requirements
05/03/07   jk      Fixed compile warning
03/14/07   sun     Fixed Compile Error
1/24/07    jk      Fixed Fatal Error
11/07/06   sun     Reorganized Functions
05/11/06   wli     Re-arrange features for perso
11/22/05   jk      Added support for multiprocessor RPC
03/07/05    jk     Added featurization for RTRE init check (to avoid compile
                   issue when RTRE off)
12/16/04    wli    Fixed size problem when reading second prl.
12/08/04    jk     Slight correction related to CR53836 fix
12/07/04    jk     CR# 53836 - Full 15 digit conversion for CAVE AN AUTH NAI
11/22/04    jk     Finished adding support for 3GPD Simple/Mobile IP
10/22/04   wli     Reinitialized nvruim_sw1 and nvruim_sw2 when read from cache.
09/16/04   wli     Added a check to prevent from accessing card before knowing
                   RTRE configuration.
09/07/04   wli     Get the size info from PRL when FEATURE_2ND_PRL defined.
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
07/20/04   wli     Reset CDMA dir present if a mandatory CDMA file is not present
                   (Merged from 6300 brance).
07/15/04   jk      Added support to map NV_HDR_AN_AUTH_USER_ID_LONG_I and
                   NV_HDR_AN_AUTH_USER_ID_I to imsi@unicomcdma.com.cn for China
                   Unicom's AN Cave Authentication
07/14/04   wli     Support feature 2ND_PRL 
06/09/04   wli     Changed NV_MODE_DIGITAL_PREF to NV_MODE_AUTOMATIC since CM
                   no longer supports NV_MODE_DIGITAL_PREF.
05/8/04    wli     Added feature that allow to update IMSI_M, IMSI_T, and PRL
05/8/04    wli     Fixed a bug that IMSI_T related items were not correctly 
                   filled.
03/10/04   ck      Set the slot in the ACCESS command to AUTOMATIC.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
11/19/03   jqi     CR37168
08/27/03   wli     Fixed missing EF name for NAM_LOCK write.
07/29/03   ck      Added support for old ACCOLC type for targets that want to 
                   use mainline NVRUIM with featurization.
07/17/03   jqi     Updated prl_validate API.
04/03/03   wli     Added comments.
03/27/03   ts      Removed MSG_ERROR in switch statement when determining if
                   items read successfully are valid.  Also, removed list of
                   NV items from switch as there is nothing to do for these 
                   items.
02/13/03   wli     Corrected lint errors.  
01/30/03   wli     Assign UIM_FAIL to nvruim_ruim_status_report in nvruim_
                   access().
12/10/02   ts      Fail accesses to the CDMA DF if the CDMA DF is not present
                   on the card.  Do not send the command to the UIM server.
12/03/02   ck      Removed the ACCOLC feature and mainlined it.
12/03/02   wli     Added feature for ACCOLC type.
11/25/02   ts      Use the status set by the UIM power-up/wake-up procedures
                   to determine if CDMA DF is present for RTRE.
10/31/02   ck      Featurized the processing of the NV item NV_PREF_MODE_I
                   under the feature !FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM
06/19/02   ts      Added processing for RTRE configuration.
03/28/02   wli     Changed NAM_LOCK access to meet IS820 Addendum 1 changes.
03/06/02   wli     Use to convert between ascii and alphabet format for
                   phonebook
02/22/02   wli     Added phonebook tag value conversion between ascii and
                   alphabet format. The UIM phonebook is in alphabet format.
                   fixed the bug that phone number was not located right
                   position in phonebook records.
02/13/02   wli     Added new function to set various DFs existing indicator
                   and allow to access phone book when
                   FEATURE_UIM_RUIM_W_GSM_ACCESS is turned ON and card is not
                   RUIM.
12/18/01   ck      Added support to check the status words and fall back to
                   NV if the file is not found in R-UIM when the feature
                   FEATURE_UIM_RUIM_W_GSM_ACCESS is turned ON.
12/13/01   wli     Added funciton to flush SMS cache.
07/23/01   ck      Added support for zeroing ESN as an option based on the esn
                   usage indicator.
06/26/01   ts      Changed debug feature switch names to have "FEATURE_"
                   prepended.
06/11/01   ts      Added NV_SID_NID_I item processing in the CDMA Home SID/NID
                   EF.
06/01/01   ts      Use proper PRL validate procedure when SD2.0 is enabled.
05/31/01   ts      Fixed reference to service preferences for EF.
05/21/01   ck      Modified the command and report processing to use the UIM
                   server Interface.
03/26/01   ts      Added cache'ing for the most recent SMS message.
03/14/01   ts      Removed "imaginary buffer" line that got in when comparing
                   nvruim.c files.
03/13/01   wli     Casted "freq_block" to eliminate ARM25 compiler warnings.
03/07/01   wli     Replaced real numbers with constants.
03/06/01   wli     Support more NV items for registration indicators.
03/06/01   ts      Changed cache control bit to indicate not-cache'd if write
                   to EF fails for cache'd buffer.  Also, fixed addr_num field
                   read request to return proper addr_num bits.
02/22/01   ck      Fixed the bug where OTAPA enable and SPC enable was written
                   and read incorrectly.
02/08/01   wli     Deleted invalidate and rehabilitate functions. Fixed the
                   order problem when call history read is in "absolute" mode
                   and write is in "previous" mode. Unlike speed dial numbers
                   call history information are stored in ruim card only.
1/28/01    wli     Added phone book support which includes speed dial (NV_DIAL_I)
                   and call history (NV_STACK_I).
                   Added a new interface function that enable user to invalidate
                   or rehabilitate an EF.
                   Fixed a CR so that imsi_addr_num is 0xFF when the class is zero.
01/11/01   ts      Added cache'ing for IMSI_M, UIM_ID, IMSI_T, CALL_TERM_MODE_PREF
                   and OTAPA_SPC_ENABLE.
01/08/01   ts      Fixed nvruim_read so that the esn checksum is returned with
                   the proper value whether or not the RUIM fails.  Changed the
                   reformat section of nvruim_read to allow forcing items to
                   debug values and reformat the data only if the value was read
                   properly from the RUIM.
01/04/01   ts      Changed writes to ESN and ESN checksum to return UNSUPPORTED.
                   This allows the ESN to be written to the NV memory.
12/19/00   ck      Changed the size of a 683A PRL returned on a read to reflect
                   the number of bits in the PRL rather than number of bytes.
12/13/00   wli     Copied ACCOLC, MOB_TERM_HOME, MOB_TERM_FOR_SID, and
                   MOB_TERM_FOR_NID to AMPS index.
                   Removed debug feature around NV_COUNT_I write.
                   Added debug code to force MIN1 and MIN2.
                   Fixed otapa_enabled flag bug.
11/28/00   wli     Suppot more nv items and SMS feature.
11/15/00   ck      Removed the parsing of result for MSISDN from Telecom dir
                   Added debug code to Unblock CHV
                   Grouped the processing of READ_ONLY RUIM items.
                   Added the function nvruim_set_esn_usage() that sets the
                   esn usage indicator to use ESN or UIM_ID.
11/06/00   ts      Added access to the MSISDN file in CDMA directory for
                   NV_DIR_NUMBER_I and NV_DIR_NUMBER_PCS_I.
10/13/00   ck      Grouped the items that return the write status as READONLY.
                   Modified the default Dir number to 858-555-1212.
                   Removed support for MS-ISDN.
10/11/00   ts      Changed failed RUIM accesses as NOT ACTIVE from FAILED.
                   Changed accesses to RUIM status to match new report type.
                   Copied MIN1 and MIN2 to index 0 of the MIN arrays to support
                   AMPS.  Added options for ESN failure.
09/18/00   ts      Added an ifndef around temp variables in nvruim_read to
                   prevent warning messages when NVRUIM_DEBUG_FORCE_DIR_NUM
                   is defined.
08/08/00   ms      Added nvruim_free to intercept NV_FREE_F calls
08/04/00   ms      Added SMS, SMSS, and SMSP support
06/22/00   ck      Changed the ifdef around writing Dir number to ifndef.
06/13/00   ck      Changed the feature around writing call count to
                   FEATURE_RUIM_ADM_ACCESS to prevent the Count from being
                   to the card as the interface needs to be changed.
06/12/00   ck      Changed the parsing of MSISDN to derive the MDN.
06/06/00   ck      Added the debug feature for writing dir number which
                   broke the service programming.
06/06/00   ck      Increased the array size from 10 to 11 to remove a compiler
                   warning.
06/06/00   ck      Rewrite the status to NOTACTIVE while reading NV_COUNT_I
                   when the debug feature is defined.
06/01/00   ck      Rewrite status to NOTACTIVE to support upper layer error
                   recovery and added a debug feature for writing call count
                   to the RUIM.
05/24/00   jah     Add MSISDN support.  Feature-ized debug support.
05/15/00   jah     Added short-cut exit for ADM protected EF writes
04/24/00   jah     Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
#include "nv.h"
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

#include "dog.h"
#include "memory.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "uim.h"
#include "nv.h"
#include "nvi.h"
#include "nvruimi.h"
#include "task.h"
#ifdef FEATURE_SD20
#include "prl.h"
#else /* !FEATURE_SD20 */
#include "mcsys.h"
#endif /* FEATURE_SD20 */


#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "bit.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_EUIMID 
#include "qw.h"
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_RUIM_CDMA_REFRESH
#include "uimi.h"
#endif /* FEATURE_RUIM_CDMA_REFRESH */

#ifdef FEATURE_NV_RUIM

/* If FEATURE_UIM_CAVE_AN_AUTH is defined and FEATURE_UIM_SUPPORT_HRPD_AN is not,
there is a problem.  Ensure that this is not the case */

#if defined(FEATURE_UIM_CAVE_AN_AUTH) && !defined(FEATURE_UIM_SUPPORT_HRPD_AN)
#error FEATURE_UIM_CAVE_AN_AUTH requires FEATURE_UIM_SUPPORT_HRPD_AN
#endif /* FEATURE_UIM_CAVE_AN_AUTH && !FEATURE_UIM_SUPPORT_HRPD_AN */

/* ASCII character */
#define ASCII_A 0x41
#define ASCII_B 0x42
#define ASCII_C 0x43
#define ASCII_D 0x44
#define ASCII_E 0x45
#define ASCII_F 0x46

#define ASCII_a 0x61
#define ASCII_b 0x62
#define ASCII_c 0x63
#define ASCII_d 0x64
#define ASCII_e 0x65
#define ASCII_f 0x66

#define ASCII_0 0x30
#define ASCII_1 0x31
#define ASCII_2 0x32
#define ASCII_3 0x33
#define ASCII_4 0x34
#define ASCII_5 0x35
#define ASCII_6 0x36
#define ASCII_7 0x37
#define ASCII_8 0x38
#define ASCII_9 0x39


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Command buffer for each command. */
LOCAL uim_cmd_type cmd;

/* Status reported by R-UIM for each command */
LOCAL uim_rpt_status nvruim_ruim_status_report = UIM_FAIL;

/* Status words returned from R-UIM */
uim_sw1_type nvruim_sw1;
uim_sw2_type nvruim_sw2;
LOCAL rex_tcb_type   *task_ptr = NULL;    /* Pointer to requesting task TCB */

/* Set as default the ESN Usage indicator to return ESN_ME for ESN */
nvruim_esn_usage_type nvruim_esn_usage = NV_RUIM_USE_ESN;

/* Indicate existing DFs (MF, CDMA, GSM, or DCS 1800 ) */
byte nvruim_dir_present = NVRUIM_NO_DIR_PRESENT;


LOCAL  struct
  {                   /* Buffer for sid nid pair */
    word sid;
    word nid;
    byte band_class;
  }temp_sid_nid_buffer;
LOCAL  byte msisdn_buffer[ NVRUIM_MSISDN_SIZE ];
LOCAL  byte prl_size[2];          /* RUIM PRL first 2 bytes */
LOCAL  byte *prl_ptr;            /* Pointer to buffer */
LOCAL  word max_prl_size = 0;    /* RUIM max prl size */

#if defined (FEATURE_UIM_CAVE_AN_AUTH) || defined (FEATURE_PERSO_RUIM)

#ifdef FEATURE_UIM_CAVE_AN_AUTH
/* Length of String for China Unicom AM Cave NAI */
#define NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH 25
/* String constant used to build NAI string for China-Unicom, AN Cave Auth
The string is of the format IMSI@unicomcdma.com.cn.  The IMSI is of the format
46003+MIN.  This string constant is in ASCII, and is 
"460030000000000@unicomcdma.com.cn".  Digits 6-15 will be re-written with the
MIN once it is read from the R-UIM.*/
const byte nvruim_an_cave_nai[NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH] =
{
 '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
 '@', 'm', 'y', 'c', 'd', 'm', 'a', '.', 'c', 'n'
};
#endif
// Constants for MIN conversion routines
#define NVRUIM_PREFIX_MASK 0x00FFC000
#define NVRUIM_THOUSANDS_MASK 0x00003C00
#define NVRUIM_LAST_3_MASK 0x000003FF

#define NVRUIM_PREFIX_SHIFT 14
#define NVRUIM_THOUSANDS_SHIFT 10
#define NVRUIM_LAST_3_SHIFT 0

const byte nvruim_min_to_num[] =
{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

const byte nvruim_bcd_to_num[] =
{ 0xFF, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


#endif /* FEATURE_UIM_CAVE_AN_AUTH */

boolean nvruim_bcsms_svc    = FALSE;
boolean nvruim_smscap_svc   = FALSE;
boolean nvruim_bcsms_table_num_entries_known = FALSE;
byte    nvruim_bcsms_table_num_entries;

#define NVRUIM_MAX_BCSMS_TABLE_ENTRY_SIZE   255
#define NVRUIM_MAX_BCSMS_TABLE_ENTRIES      128
#define NVRUIM_BCSMS_PARM_SIZE              2
#define NVRUIM_SMSCAP_SIZE                  4
#define NVRUIM_SMSCAP_ON_ACCESS_CH_MASK     0x01
#define NVRUIM_SMSCAP_ON_TRAFFIC_CH_MASK    0x02
#ifdef FEATURE_OEMOMH
#define NVRUIM_SMSCAP_STD_EMS_MASK          0x04
#endif
LOCAL byte  nvruim_bcsms_table_ef_buf[NVRUIM_MAX_BCSMS_TABLE_ENTRY_SIZE];
LOCAL byte  nvruim_bcsms_parm_ef_buf[NVRUIM_BCSMS_PARM_SIZE];
LOCAL byte  nvruim_smscap_ef_buf[NVRUIM_SMSCAP_SIZE];

/* Control Variable which determines whether support for 3GPD should come from
the R-UIM or from NV */

uim_3gpd_support_status nvruim_3gpd_control=UIM_3GPD_MIP_NV_SIP_NV;

#if defined(FEATURE_UIM_SUPPORT_3GPD_NV) && defined (FEATURE_UIM_SUPPORT_3GPD)
LOCAL  nvruim_3gpd_nv_card_status nvruim_non_3gpd_cdma_card = NVRUIM_NO_CARD;
#endif /* FEATURE_UIM_SUPPORT_3GPD_NV && FEATURE_UIM_SUPPORT_3GPD */

/* Constants and data for LBS  */
#ifdef FEATURE_UIM_SUPPORT_LBS
/* The following item indicates whether the CARD Supports GPS functionality 
(Service Number 24 in CDMA Service Table) */
boolean nvruim_lbs_support = FALSE;
/* The following are masks, constants, and shift values used during parsing of
elementary files for LBS */

/* Constant belonging to EF-LBSV2Config EF */
#define NVRUIM_MAX_LBS_V2_CONFIG_SIZE              1
#define NVRUIM_LBS_FLAG_GPS1_SEED_POSITION_SHIFT   1
#define NVRUIM_LBS_FLAG_GPS1_DYNAMIC_MODE_SHIFT    2

/* Constant belonging to EF-LBSXTRAConfig EF */
#define NVRUIM_MAX_LBS_XTRA_CONFIG_SIZE                 4
#define MAX_VALUE_NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I      168
#define MAX_VALUE_NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I  10
#define MAX_VALUE_NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I  120

/* Constant belonging to EF-LBS XTRA Server URLs EF */
#define NVRUIM_LBS_XTRA_URL_TAG		            0x80
#define NVRUIM_LBS_XTRA_URL_LEN_TAG_82	        0x82
#define NVRUIM_LBS_XTRA_URL_LEN_TAG_81	        0x81
#define NVRUIM_MAX_LBS_URL_TAG_LEN_INFO			NVRUIM_LBS_XTRA_URL_LEN_TAG_82
#define NVRUIM_LBS_XTRA_URL_FIRST				0x01
#define NVRUIM_LBS_XTRA_URL_SECOND				0x02
#define NVRUIM_LBS_XTRA_URL_THIRD               0x03
#define NVRUIM_INVALID_LBS_URL_TAG_LEN_INFO     0x80

/* Constant Belonging to LBS PDE and MPC Address Type and its maximum Length*/ 
#define NVRUIM_MAX_LBS_DOMAIN_ADDRESS_LENGTH    128
/* IPv4 addresses are usually written in dot-decimal notation, 
   which consists of the four octets of the address expressed in decimal 
   and separated by periods. for example 121.121.121.121 */
#define NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH       15
/* IPv6 addresses are normally written as eight groups of four hexadecimal
   digits, where each group is separated by a colon (:). eg:
   2001:1234:0F12:089B:0000:0000:1234:5678 */
#define NVRUIM_MAX_LBS_IPV6_ADDRESS_LENGTH       39
#define NVRUIM_LBS_ADDRESS_TYPE_DOMAIN_NAME       1
#define NVRUIM_LBS_ADDRESS_TYPE_IPV4              2
#define NVRUIM_LBS_ADDRESS_TYPE_IPV6              3

/* Constant belonging to EF-LBS V2 PDE ADDRESS EF */
#define NVRUIM_MAX_LBS_V2_PDE_ADDRESS_SIZE      132

/* Constant belonging to EF-LBS V2 MPC ADDRESS EF */
#define NVRUIM_MAX_LBS_V2_MPC_ADDRESS_SIZE      132


/* constant belonging to EF-LBS XTRA SERVER URLS */
#define NVRUIM_MAX_LBS_XTRA_SERVER_URL_LENGTH   128

LOCAL byte nvruim_lbs_v2_mpc_address_buf[NVRUIM_MAX_LBS_V2_MPC_ADDRESS_SIZE];
LOCAL byte nvruim_lbs_v2_pde_address_buf[NVRUIM_MAX_LBS_V2_PDE_ADDRESS_SIZE];
LOCAL byte nvruim_lbs_v2_config_ef_buf[NVRUIM_MAX_LBS_V2_CONFIG_SIZE];
LOCAL byte nvruim_lbs_xtra_config_ef_buf[NVRUIM_MAX_LBS_XTRA_CONFIG_SIZE];

static uint32 nvruim_get_ipv4_digit_char_string(byte *data_p, byte data_len);
static boolean nvruim_get_ipv6_hex_char_string (byte *data_p, byte data_len, uint16 *ipv6_address_p);
#endif /* FEATURE_UIM_SUPPORT_LBS */

/* Constants and data for 3GPD */
#ifdef FEATURE_UIM_SUPPORT_3GPD

/* This contains the default sip profile.  The current DATA implementation 
only supports one SIP profile so references to the generic NV SIP profile 
always map to profile 0.  If functionality is expanded in the future to
multiple profiles, this will be dynamically assigned by a parameter assigned
in an NV Access. */

byte nvruim_preferred_sip_profile = UIM_CURRENTLY_ACTIVE_SIP_PROFILE; 

/* The following variable gets set during startup, after a read to the CDMA
service table determines if the UIM card supports 3GPD functionality */
boolean nvruim_3gpd_sip_svc=FALSE;
boolean nvruim_3gpd_mip_svc=FALSE;

/* The following item indicates whether the CARD Supports extended 3GPD 
functionality as first defined on OMH on top of IS-820-C.  Gets initialized by
nvruim_data_3gpd_init_extensions_support which is called from DATA. */

boolean nvruim_3gpd_ext_support=FALSE;


/* The following are masks, constants, and shift values used when parsing
elementary files for 3GPD */
#define NVRUIM_MAX_SIPUPP_SIZE      255
#define NVRUIM_MAX_MIPUPP_SIZE      255
#define NVRUIM_MAX_NUM_MIP_PROFILES 16
#define NVRUIM_MAX_SIPPAPSS_SIZE    255
#define NVRUIM_MAX_SIPPAPSS_SS_SIZE 31
#define NVRUIM_MAX_TCPCONFIG_SIZE   2
#define NVRUIM_MAX_DGC_SIZE         3
#define NVRUIM_MAX_NUM_SIP_PROFILES 16
#define NVRUIM_MAX_SIPUPPEXT_SIZE   255

/* Since each file can contain at most 255 bytes, the max number of bits is
255 x 8 = 2040.  The following value is reserved to indicate a non valid bit value */
#define NVRUIM_NO_PROFILE_INDEX_SET    0xFFFF

#define NVRUIM_SIPPAPSS_NUM_NAI_MASK        0xF0
#define NVRUIM_SIPUPP_NUM_NAI_MASK          0xF0
#define NVRUIM_SIPUPPEXT_NUM_NAI_MASK       0xF0
#define NVRUIM_MIPUPP_RETRY_INCL_MASK       0x80
#define NVRUIM_MIPUPP_MAX_RETRY_MASK        0x60
#define NVRUIM_MIPUPP_RETRY_INT_MASK        0x1C
#define NVRUIM_SIPPAPSS_NUM_NAI_MASK        0xF0
#define NVRUIM_SIPSP_STATUS_PARAM_MASK      0xF0
#define NVRUIM_MIPSP_STATUS_PARAM_MASK      0xF0
#define NVRUIM_3GPDOPM_MASK                 0x3

#define NVRUIM_MIPSP_SHIFT                          4
#define NVRUIM_SIPSP_SHIFT                          4
#define NVRUIM_RETRY_INFO_PRESENT_SHIFT             7
#define NVRUIM_MAX_NUM_RETRY_SHIFT                  5
#define NVRUIM_MIP_RETRY_INT_SHIFT                  2
#define NVRUIM_MIPFLAGS_DRS_OPT_SHIFT               1
#define NVRUIM_MIPFLAGS_HANDDOWN_SHIFT              2
#define NVRUIM_MIPFLAGS_RRQ_IF_TRFK_SHIFT           3
#define NVRUIM_TCP_CONFIG_GRACEFUL_DORMANT_SHIFT    8


#endif /* FEATURE_UIM_SUPPORT_3GPD */

#if defined(FEATURE_UIM_SUPPORT_HRPD_AN) || defined(FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND)

#define NVRUIM_MAX_HRPDUPP_SIZE 255

#endif /* FEATURE_UIM_SUPPORT_HRPD_AN  || FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND */

#ifdef FEATURE_RUIM_CDMA_REFRESH
void nvruim_process_refresh(void);
#endif /* FEATURE_RUIM_CDMA_REFRESH */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                                                                         */
/*                     Cache Buffers and control variables                 */
/*                                                                         */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* The following values represent bits within the nvruim_cache_control that
   indicate whether the buffer is current of needs to be read from the RUIM */
#define RUIM_NO_CACHE_VALUE                  0x00000000
#define RUIM_IMSI_M_CACHE_BIT                0x00000001
#define RUIM_RUIM_ID_CACHE_BIT               0x00000002
#define RUIM_IMSI_T_CACHE_BIT                0x00000004
#define RUIM_CALL_TERM_MODE_PREF_CACHE_BIT   0x00000008
#define RUIM_OTAPA_SPC_ENABLE_CACHE_BIT      0x00000010
#define RUIM_SVC_PREF_CACHE_BIT              0x00000020
#define RUIM_TMSI_CACHE_BIT                  0x00000040
#define RUIM_ANALOG_REG_IND_CACHE_BIT        0x00000080
#define RUIM_ZONE_REG_IND_CACHE_BIT          0x00000100
#define RUIM_SYS_REG_IND_CACHE_BIT           0x00000200
#define RUIM_DIST_REG_IND_CACHE_BIT          0x00000400


#define RUIM_SIPUPP_EF_CACHE_BIT             0x00001000
#define RUIM_MIPUPP_EF_CACHE_BIT             0x00002000
#define RUIM_MIPSP_EF_CACHE_BIT              0x00004000
#define RUIM_SIPPAPSS_EF_CACHE_BIT           0x00008000
#define RUIM_MIPFLAGS_EF_CACHE_BIT           0x00010000
#define RUIM_TCPCONFIG_EF_CACHE_BIT          0x00020000
#define RUIM_SMS_CAP_EF_CACHE_BIT            0x00040000

#define RUIM_HRPD_NAI_CACHE_BIT              0x00080000
#define RUIM_SF_EUIMID_CACHE_BIT             0x00100000
#define RUIM_SIPSP_EF_CACHE_BIT              0x00200000
#define RUIM_SIPUPPEXT_EF_CACHE_BIT          0x00400000
#define RUIM_DGC_EF_CACHE_BIT                0x00800000
#define RUIM_LBS_V2_CONFIG_EF_CACHE_BIT      0x01000000
#define RUIM_LBS_XTRA_CONFIG_EF_CACHE_BIT    0x02000000
#define RUIM_LBS_V2_PDE_ADDRESS_EF_CACHE_BIT 0x04000000
#define RUIM_LBS_V2_MPC_ADDRESS_EF_CACHE_BIT 0x08000000


uint32 nvruim_cache_control = RUIM_NO_CACHE_VALUE;

LOCAL nvruim_imsi_type            nvruim_imsi_m_cache_buf;
LOCAL nv_esn_type                 nvruim_ruim_id_cache_buf;
LOCAL nvruim_imsi_type            nvruim_imsi_t_cache_buf;
LOCAL byte                        nvruim_call_term_mode_pref_cache_buf;
LOCAL byte                        nvruim_otapa_spc_enable_cache_buf;
LOCAL byte                        nvruim_nam_lock_buf;
LOCAL byte                        nvruim_svc_pref_cache_buf;
LOCAL nvruim_tmsi_type            nvruim_tmsi_cache_buf;
LOCAL nvruim_analog_reg_ind_type  nvruim_analog_reg_ind_cache_buf;
LOCAL nvruim_zone_reg_ind_type    nvruim_zone_reg_ind_cache_buf;
LOCAL nvruim_sys_reg_ind_type     nvruim_sys_reg_ind_cache_buf;
LOCAL nvruim_dist_reg_ind_type    nvruim_dist_reg_ind_cache_buf;

#ifdef FEATURE_UIM_EUIMID
LOCAL qword                       nvruim_sf_euim_id_cache_buf;
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_UIM_SUPPORT_3GPD
/* 3GPD CACHE */
LOCAL byte              nvruim_3GPD_op_mode_ef_buf;
LOCAL byte              nvruim_3gpd_mipflags_ef_buf;
LOCAL byte              nvruim_3gpd_tcp_config_ef_buf[NVRUIM_MAX_TCPCONFIG_SIZE];
LOCAL byte              nvruim_sipupp_ef_buf[NVRUIM_MAX_SIPUPP_SIZE];
LOCAL word              nvruim_sip_profile_bit_index[NVRUIM_MAX_NUM_SIP_PROFILES];
LOCAL byte              nvruim_mipupp_ef_buf[NVRUIM_MAX_MIPUPP_SIZE];

LOCAL word              nvruim_mip_profile_bit_index[NVRUIM_MAX_NUM_MIP_PROFILES];
LOCAL byte              nvruim_mipsp_ef_buf;
LOCAL byte              nvruim_sippapss_ef_buf[NVRUIM_MAX_SIPPAPSS_SIZE];
LOCAL word              nvruim_sip_pap_ss_profile_bit_index[NVRUIM_MAX_NUM_SIP_PROFILES];

#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
LOCAL byte              nvruim_3gpd_dcg_ef_buf[NVRUIM_MAX_DGC_SIZE];
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
LOCAL byte                              nvruim_sipsp_ef_buf;
LOCAL byte                              nvruim_sipuppext_ef_buf[NVRUIM_MAX_SIPUPPEXT_SIZE];
LOCAL word                              nvruim_sip_ext_profile_bit_index[NVRUIM_MAX_NUM_SIP_PROFILES];
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#if defined(FEATURE_UIM_SUPPORT_HRPD_AN) || defined(FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND)
LOCAL byte                        nvruim_hrpdupp_ef_buf[NVRUIM_MAX_HRPDUPP_SIZE];
boolean                           nvruim_hrpd_cdma_svc=FALSE;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN || FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND */

/* This variable is used to indicate whether the current R-UIM card supports
 HRPD AN Authentication, as per the CDMA service table */

uim_an_hrpd_support_status nvruim_hrpd_control=UIM_AN_HRPD_NO_SUPPORT;

boolean  nvruim_rtre_config_initialized = FALSE;

#ifdef FEATURE_RUIM_CDMA_REFRESH
uint32 nvruim_refresh_file_cache = RUIM_NO_CACHE_VALUE;
#endif /* FEATURE_RUIM_CDMA_REFRESH */

static boolean uim_hrpd_disabled_card(void);

#ifdef FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED
static boolean uim_hrpd_disabled_card_flag = FALSE;
static boolean uim_use_cave_default = FALSE;
#endif /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Debug Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

MACRO NVRUIM_CHECK_DIR_PRESENT


DESCRIPTION
  Check to see if CDMA DF doesn't exist and and either GSM DF or DCS 1800 DF
  exists

PARAMETERS
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
#define NVRUIM_CHECK_DIR_PRESENT()                                 \
              ((!(nvruim_dir_present & NVRUIM_CDMA_DF_PRESENT)) && \
              (nvruim_dir_present & (NVRUIM_GSM_DF_PRESENT |       \
                                 NVRUIM_DCS1800_DF_PRESENT)))



/*===========================================================================

MACRO NVRUIM_SIZE


DESCRIPTION
  Size of an NV item

PARAMETERS
  eee   NV item being sized

DEPENDENCIES
  None

RETURN VALUE
  size of the NV item

SIDE EFFECTS
  None

===========================================================================*/
#define NVRUIM_SIZE(eee) \
  /*lint -e545 */ ((word)(sizeof(((nv_item_type *) 0)-> eee ))) /*lint +e545 */


/*===========================================================================

MACRO NVRUIM_ACCESS_BUFFER_INIT

DESCRIPTION
  Initialize an R-UIM Access command buffer & byte count

PARAMETERS
  cmd   command buffer
  item  NV item being accessed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define NVRUIM_ACCESS_BUFFER_INIT(cmd,item) \
      (cmd).access_uim.num_bytes = NVRUIM_SIZE(item.item); \
      (cmd).access_uim.data_ptr = \
        (byte *) &(nv_cmd_ptr->data_ptr-> item.item )


/*===========================================================================

MACRO NVRUIM_ACCESS_BUFFER_INIT_MIN

DESCRIPTION
  Initialize an R-UIM Access command buffer & byte count.
  like NVRUIM_ACCESS_BUFFER_INIT, but for use when the
  NV item is bigger than the data item.

PARAMETERS
  cmd   R-UIM command buffer
  item  NV item being accessed
  size  Size of data

DEPENDENCIES
  NV item must be bigger than R-UIM item specified in "size".

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define NVRUIM_ACCESS_BUFFER_INIT_MIN(cmd,item,size) \
  (cmd).access_uim.num_bytes = size; \
  (cmd).access_uim.data_ptr = \
    (byte *) &(nv_cmd_ptr->data_ptr-> item.item [NV_CDMA_MIN_INDEX])

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                             Functions                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

FUNCTION NVRUIM_FLUSH_CACHE                                 EXTERNAL FUNCTION

DESCRIPTION
  This function clears the cache control so that all items are read from the
  RUIM the next time they are requested.

DEPENDENCIES
  This function affects the cache control variable and therefore affects the
  operation of the read of cached NV items from the RUIM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_flush_cache
(
  void
)
{
  /* Clear the cache control variable to indicate the NV items that are
     cached must be read from the RUIM. */
  nvruim_cache_control = RUIM_NO_CACHE_VALUE;
} /* nvruim_flush_cache */


/*===========================================================================

FUNCTION NVRUIM_REVERSE                                 INTERNAL FUNCTION

DESCRIPTION
  Reverse the byte order of the data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_reverse
(
  void *data_ptr,                 /* bytes to reverse */
  uint2 count                     /* count of bytes   */
)
{
  uint2 head, tail;
  byte *byte_ptr;
  byte temp;

  byte_ptr = (byte *) data_ptr;

  /* Handle count of 0 bytes */

  if (count == 0)
    return;

  /* Swap the byte at the head & tail until the
   * head and tail meet at the middle
   */

  for (head = 0, tail = count - 1; head < tail; head++, tail--)
  {
    temp = byte_ptr[ head ];
    byte_ptr[ head ] = byte_ptr[ tail ];
    byte_ptr[ tail ] = temp;
  }

} /* nvruim_reverse */

#if defined (FEATURE_UIM_CAVE_AN_AUTH) || defined (FEATURE_PERSO_RUIM)

/*===========================================================================

FUNCTION NVRUIM_DECODE_MIN                                 INTERNAL FUNCTION

DESCRIPTION
     Decode MIN 1 & 2 to a vector of digits

PARAMETERS
   min1        MIN1 being decoded
   min2        MIN2 being decoded
   digits      where the decoded MIN is placed

RETURN VALUE
   TRUE - MIN was decoded
   FALSE - Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean nvruim_decode_min
(
  byte *digits,                   /* pointer to converted digits */
  word *mcc,                      /* mcc */
  byte *mnc,                      /* mnc */
  dword *min1,                    /* min1 */
  word *min2                      /* min2 */
)
{
  byte digit = 0;
  byte digit_pointer = 0;
  word first_three = 0;
  word second_three = 0;
  byte thousands = 0;
  word last_three = 0;
  word country_code = 0;
  byte network_code = 0;

  if(digits == NULL)
  {
    /* Digits array is NULL */
    MSG_HIGH("Error in converting IMSI", 0, 0, 0);
    return FALSE;
  }
  memset(digits, 0x0, NVRUIM_NUM_MIN_DIGITS);
  country_code = *mcc;
  if(country_code <= 999)
  {
    /* Converting Mobile Country Code */
    MSG_LOW("Converting Mobile Country Code",0,0,0);
    digit = nvruim_min_to_num[country_code/100];
    digits[digit_pointer++] = digit;
    country_code %= 100;

    digit = nvruim_min_to_num[country_code / 10];
    digits[digit_pointer++] = digit;

    digit = nvruim_min_to_num[country_code % 10];
    digits[digit_pointer++] = digit;

  }
  else
  {
    /* Error in converting Mobile Country Code of IMSI */
    MSG_HIGH("Error in converting IMSI", 0, 0, 0);
    return FALSE;
  }
  network_code = *mnc;
  if(network_code <= 99)
  {
    /* Convert Mobile Network Code */
    MSG_LOW("Converting Mobile Network Code",0,0,0);
    digit = nvruim_min_to_num[network_code / 10];
    digits[digit_pointer++] = digit;
    
    digit = nvruim_min_to_num[network_code % 10];
    digits[digit_pointer++] = digit;
  }
  else
  {
    /* Error in converting Mobile Network Code of IMSI */
    MSG_HIGH("Error in converting IMSI", 0, 0, 0);
    return FALSE;
  }
  first_three = *min2;
  if (first_three <= 999)
  {
    /* Convert First Three Digits of IMSI */
    MSG_LOW("Converting First Three Digits of IMSI", 0, 0, 0);
    digit = nvruim_min_to_num[first_three / 100];
    digits[digit_pointer++] = digit;
    first_three %= 100;

    digit = nvruim_min_to_num[first_three / 10];
    digits[digit_pointer++] = digit;

    digit = nvruim_min_to_num[first_three % 10];
    digits[digit_pointer++] = digit;
  }
  else
  {
    /* Error in converting First 3 Digits of IMSI */
    MSG_HIGH("Error in converting IMSI", 0, 0, 0);
    return FALSE;
  }

  if (*min1 > 0)
  {
    /* Convert MIN 1 Portion of IMSI */
    second_three = (*min1 & NVRUIM_PREFIX_MASK) >> NVRUIM_PREFIX_SHIFT;
    thousands = (*min1 & NVRUIM_THOUSANDS_MASK) >> NVRUIM_THOUSANDS_SHIFT;
    last_three = (*min1 & NVRUIM_LAST_3_MASK) >> NVRUIM_LAST_3_SHIFT;

    thousands = nvruim_bcd_to_num[thousands];
    if ( (thousands != 0xFF) 
    &&   (second_three <= 999)
    &&   (last_three <= 999) )
    {
      /* IMSI data is of the right format */
      MSG_LOW("Converting Last 7 Digits of IMSI", 0, 0, 0);
      digit = nvruim_min_to_num[second_three / 100];
      digits[digit_pointer++] = digit;

      second_three %= 100;

      digit = nvruim_min_to_num[second_three / 10];
      digits[digit_pointer++] = digit;

      digit = nvruim_min_to_num[second_three % 10];
      digits[digit_pointer++] = digit;

      digits[digit_pointer++] = thousands;

      digit = nvruim_min_to_num[last_three / 100];
      digits[digit_pointer++] = digit;

      last_three %= 100;

      digit = nvruim_min_to_num[last_three / 10];
      digits[digit_pointer++] = digit;

      digit = nvruim_min_to_num[last_three % 10];
      digits[digit_pointer++] = digit;
    } /* end if - last 7 digits of IMSI are correct format */
  } /* end if MIN 1 is a valid number */

  if (digit_pointer != NVRUIM_NUM_MIN_DIGITS)
  {
    MSG_HIGH("Error Converting IMSI", 0, 0, 0);   
    memset(digits, 0x0, NVRUIM_NUM_MIN_DIGITS);
    return FALSE;
  }
  return TRUE;
}
#endif /* FEATURE_UIM_CAVE_AN_AUTH */


/*===========================================================================

FUNCTION NVRUIM_REPORT                                  INTERNAL FUNCTION

DESCRIPTION
  Accept status reports from R-UIM
  Set the global status and signal NV task that status has arrived.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void nvruim_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{
  rex_sigs_type   sigs;              /* Signal to set upon cmd completion */
  if (NULL == report)
  {
    MSG_ERROR("Invalid uim report",0,0,0);
    return;    
  }
  if(NULL == task_ptr)
  {
    MSG_ERROR("Error during getting task pointer in nvruim_report",0,0,0);    
    return;
  }
  if ( 0 == report->user_data)
  {
    MSG_ERROR("No signal is retrieved in uim report",0,0,0);      	  
  }	  
  nvruim_ruim_status_report = report->rpt_status;
  nvruim_sw1 = report->sw1;
  nvruim_sw2 = report->sw2;
  /* get the task pointer signal back form uim */
  sigs = report->user_data;
  /* Set signal */
  (void) rex_set_sigs(task_ptr, sigs);
} /* nvruim_report */

/*===========================================================================

FUNCTION NVRUIM_ACCESS                                  INTERNAL FUNCTION

DESCRIPTION
  Sends the Command to UIM and Waits for the response

DEPENDENCIES
  Non-reentrant, only works for NAM=0

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status nvruim_access
(
  uim_cmd_type *cmd_ptr       /* R-UIM command                               */
)
{
  /* Variable to hold the signals received */
  rex_sigs_type sigs_received = 0;
  rex_sigs_type   sigs;               /* Signal to set upon cmd completion */

  /* Check if CDMA DF is NOT present on the card */
  if (((cmd_ptr->access_uim.item >> 8) == UIM_RUIM_EF_START) && 
      !(NVRUIM_CDMA_DF_PRESENT & nvruim_dir_present))
  {
    /* Fail the command immediately */
    nvruim_ruim_status_report = UIM_FAIL;
  } 
  else
  {
    task_ptr = rex_self();

    if(NULL == task_ptr)
    {
      MSG_ERROR("Error during getting task pointer in nvruim_access",0,0,0);
      nvruim_ruim_status_report = UIM_FAIL;	    
      return(nvruim_ruim_status_report);
    }	    

    /* Before calling the uim_cmd, we check the task pointer,
       if the task is NV TASK then, we will set the signal for NV TASK
       else it is always belong to calling TASK */
    if (&nv_tcb == task_ptr)
    {
      cmd_ptr->hdr.user_data = NV_RUIM_ACCESS_SIG;
    }
	  
    cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F; /* "Access" an EF    */
  
    /* Indicate command completion is to be signaled:
     *    Do not signal the NV Task
     *    Use no signal
     *    No "done" queue
     *    Status call-back function
     *    Always Report status
     */
  
    cmd_ptr->access_uim.hdr.cmd_hdr.task_ptr = NULL;
  
    cmd_ptr->access_uim.hdr.cmd_hdr.sigs = 0;
  
    cmd_ptr->access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
  
    cmd_ptr->hdr.rpt_function = nvruim_report;
  
    cmd_ptr->hdr.protocol = UIM_CDMA;

    cmd_ptr->hdr.slot = UIM_SLOT_AUTOMATIC;
  
    cmd_ptr->access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
  
    /* Send the command to the R-UIM:
     *    Clear the "command done signal"
     *    Send the command
     *    Wait for the command to be done
     */

    sigs = cmd_ptr->hdr.user_data;  
    if (0 == sigs)
    {
      nvruim_ruim_status_report = UIM_FAIL;	    
      return(nvruim_ruim_status_report);
    }

    (void) rex_clr_sigs(task_ptr, sigs);
    if (&nv_tcb == task_ptr)  
    {  
      /* Pet the dog before sending the command */
      dog_report(DOG_NV_RPT);

      uim_cmd( cmd_ptr );
  
      /* Wait for the UIM response */
      do
      {
        /* Wait for the access and watch-dog report signal */
        sigs_received = rex_wait( NV_RUIM_ACCESS_SIG | NV_RPT_TIMER_SIG );

        /* Wait for watchdog timer sig */
        if ((sigs_received & NV_RPT_TIMER_SIG) != 0)
        {
          /* Clear the dog report timer signal */
          (void) rex_clr_sigs(&nv_tcb, NV_RPT_TIMER_SIG);

          /* Kick watchdog and reset timer */
          dog_report(DOG_NV_RPT);
          (void) rex_set_timer(&nv_rpt_timer, DOG_NV_RPT_TIME);
        }
      } while( (sigs_received & (NV_RUIM_ACCESS_SIG)) == 0 );
    }
    else
    {
      /* For other Task */	
      uim_cmd( cmd_ptr );
      (void) rex_wait(sigs);
    }	
  /* Reset the Task_Ptr */
  task_ptr = NULL;
  }/* end if - The CDMA DF is not present on the card */
 
  return(nvruim_ruim_status_report);

} /* nvruim_access */


/*===========================================================================

FUNCTION NVRUIM_WRITE_SW

DESCRIPTION
get the SW for Write Cmd

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static nv_ruim_support_status nvruim_write_sw(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status,
  uint32            cache_bit 
)
{
  *op_status = NV_FAIL_S; /* Default status is "failed" */

  switch(nvruim_ruim_status_report)
  {
    
    case UIM_PASS:
      *op_status = NV_DONE_S;     /* Request completed okay     */
      break;

      /* Other statuses:*/
      /* NV_BUSY_SRequest is queued*/
      /* NV_BADCMD_S    Unrecognizable command field  */
      /* NV_FULL_SThe NVM is full */
      /* NV_NOTACTIVE_S Variable was not active */
      /* NV_BADPARM_S   Bad parameter in command block*/
      /* NV_READONLY_S  Parameter is read only*/
      /* NV_BADTG_S     Item not valid for Target     */
      /* NV_NOMEM_S     free memory exhausted */
      /* NV_NOTALLOC_S  address is not a valid allocation */

    case UIM_FAIL:
      {
        /* For writes that fail, clear the cache valid bit for the appropriate
        buffer. Non-cached accesses should have a benign clear cache mask. */
        nvruim_cache_control &= ~cache_bit;

#ifdef FEATURE_NVRUIM_ADM_ACCESS
        /* For NV restore before ADM verification */
        switch(nv_cmd_ptr->item)
        {
          
          case NV_MIN2_I:
          case NV_MIN1_I:
          case NV_IMSI_11_12_I:
          case NV_IMSI_ADDR_NUM_I:
          case NV_IMSI_MCC_I:

          case NV_IMSI_T_S1_I:
          case NV_IMSI_T_S2_I:
          case NV_IMSI_T_MCC_I:
          case NV_IMSI_T_11_12_I:
          case NV_IMSI_T_ADDR_NUM_I:

#ifndef FEATURE_UIM_JCDMA_RUIM_SUPPORT
      /* JCDMA: this is a NV item */  
        case NV_ROAMING_LIST_683_I:
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


          case NV_ACCOLC_I:
            MSG_HIGH("Updating Failed Check ADM Verification",0,0,0);
            *op_status = NV_READONLY_S; /* Error, read-only   */
            break;
        }

#endif /* FEATURE_NVRUIM_ADM_ACCESS */

#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS ) && \
!defined( FEATURE_UIM_RUN_TIME_ENABLE )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS && !FEATURE_UIM_RUN_TIME_ENABLE */

      }
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;
}
/*===========================================================================

FUNCTION NVRUIM_CHECK_RTRE_CONFIG

DESCRIPTION
Check to see RTRE CONFIG is Initialized

DEPENDENCIES
Non-reentrant

RETURN VALUE
TRUE - If Config is Initilaized
FALSE - If Config  is not intialized


SIDE EFFECTS
None

===========================================================================*/
boolean nvruim_check_rtre_config(
  nv_cmd_type       *nv_cmd_ptr, 
  nv_stat_enum_type *op_status
)
{
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if(!nvruim_rtre_config_initialized)

  { /* Do not read before knowing RTRE configuration */
    *op_status = NV_NOTACTIVE_S;    
    nv_cmd_ptr->status = *op_status;
    return FALSE;
  }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  return TRUE;

}

/*===========================================================================

FUNCTION nvruim_check_uim_status_and_nam

DESCRIPTION
  Checks the UIM Status. If the Status is not error, then the 
   NAM is checked if CDMA DF is found.

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_check_uim_status_and_nam(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS ) && \
!defined( FEATURE_UIM_RUN_TIME_ENABLE )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS && !FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->min1.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }

      *op_status = NV_READONLY_S; /* Error, read-only   */

      nv_cmd_ptr->status = *op_status;

      return NV_RUIM_SUPPORTS_ITEM;
    }
#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS ) && \
!defined( FEATURE_UIM_RUN_TIME_ENABLE )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS && !FEATURE_UIM_RUN_TIME_ENABLE */
}

/*===========================================================================

FUNCTION NVRUIM_WRITE_ITEM

DESCRIPTION
Write to Given item 

DEPENDENCIES


RETURN VALUE
 none

SIDE EFFECTS
  none
===========================================================================*/
static void nvruim_write_item(
  byte                *data_ptr,
  word                data_len,
  word                offset,
  uim_items_enum_type item
)
{
  cmd.access_uim.data_ptr  = data_ptr;
  cmd.access_uim.num_bytes = data_len;
  cmd.access_uim.offset    = offset;
  cmd.access_uim.item      = item;
  cmd.access_uim.access    = UIM_WRITE_F;  
  nvruim_access(&cmd);
}


/*===========================================================================

FUNCTION NVRUIM_WRITE_REC_ITEM

DESCRIPTION
Write to Given record of the given item 

DEPENDENCIES


RETURN VALUE
 none

SIDE EFFECTS
  none
===========================================================================*/
static void nvruim_write_rec_item(
  byte                *data_ptr,
  word                 data_len,
  word                 offset,
  uim_record_mode_type rec_mode,
  uim_items_enum_type  item
)
{
  cmd.access_uim.data_ptr  = data_ptr;
  cmd.access_uim.num_bytes = data_len;
  cmd.access_uim.offset    = offset;
  cmd.access_uim.item      = item;
  cmd.access_uim.rec_mode  = rec_mode;
  cmd.access_uim.access    = UIM_WRITE_F;  
  nvruim_access(&cmd);
}


/*===========================================================================

FUNCTION NVRUIM_READ_ITEM

DESCRIPTION
Read the  Given item 

DEPENDENCIES


RETURN VALUE
 none

SIDE EFFECTS
  none
===========================================================================*/
static void nvruim_read_item(
  byte                *data_ptr,
  word                data_len,
  word                offset,
  uim_items_enum_type item
)
{
  cmd.access_uim.data_ptr  = data_ptr;
  cmd.access_uim.num_bytes = data_len;
  cmd.access_uim.offset    = offset;
  cmd.access_uim.item      = item;
  cmd.access_uim.access    = UIM_READ_F;  
  nvruim_access(&cmd);
}


#ifdef FEATURE_NVRUIM_ADM_ACCESS
/*===========================================================================

FUNCTION NVRUIM_WRITE_IMSI_M

DESCRIPTION
Write IMSI_M info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status
===========================================================================*/
static nv_ruim_support_status nvruim_write_imsi_m(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{

  switch(nv_cmd_ptr->item)
  {
    case NV_MIN1_I:
      MSG_HIGH("Updating ADM protected MIN1",0,0,0);
  
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->min1.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }

      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_m_cache_buf.imsi_s1[0] = 
        (byte)  nv_cmd_ptr->data_ptr->min1.min1[NV_CDMA_MIN_INDEX];
      nvruim_imsi_m_cache_buf.imsi_s1[1] = 
        (byte) (nv_cmd_ptr->data_ptr->min1.min1[NV_CDMA_MIN_INDEX] >> 8);
      nvruim_imsi_m_cache_buf.imsi_s1[2] = 
        (byte) (nv_cmd_ptr->data_ptr->min1.min1[NV_CDMA_MIN_INDEX] >> 16);

      
      nvruim_write_item((byte *) &(nvruim_imsi_m_cache_buf.imsi_s1),
                        sizeof (nvruim_imsi_m_cache_buf.imsi_s1),
                        3,
                        UIM_CDMA_IMSI_M,
                        RUIM_IMSI_M_CACHE_BIT);
      break;

    case NV_MIN2_I:
      MSG_HIGH("Updating ADM protected MIN2",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->min2.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_m_cache_buf.imsi_s2[0] = 
        (byte)  nv_cmd_ptr->data_ptr->min2.min2[NV_CDMA_MIN_INDEX];
      nvruim_imsi_m_cache_buf.imsi_s2[1] = 
        (byte) (nv_cmd_ptr->data_ptr->min2.min2[NV_CDMA_MIN_INDEX] >> 8);
    
      nvruim_write_item((byte *) &(nvruim_imsi_m_cache_buf.imsi_s2),
                        sizeof (nvruim_imsi_m_cache_buf.imsi_s2),
                        1,
                        UIM_CDMA_IMSI_M);
      break;

    case NV_IMSI_11_12_I:
      MSG_HIGH("Updating ADM protected IMSI_11_12",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_11_12.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_m_cache_buf.imsi_11_12 = 
        (byte)  nv_cmd_ptr->data_ptr->imsi_11_12.imsi_11_12;

      nvruim_write_item((byte *) &(nvruim_imsi_m_cache_buf.imsi_11_122),
                        sizeof (nvruim_imsi_m_cache_buf.imsi_11_12),
                        6,
                        UIM_CDMA_IMSI_M);
      break;

    case NV_IMSI_ADDR_NUM_I:
      MSG_HIGH("Updating ADM protected IMSI_ADDR_NUM",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_addr_num.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_m_cache_buf.imsi_addr_num = 
      (byte)  nv_cmd_ptr->data_ptr->imsi_addr_num.num | 
      NVRUIM_IMSI_PROGRAMMED;
    
      /* Update imsi class when updating addr num */
      if((nv_cmd_ptr->data_ptr->imsi_addr_num.num == 0xff)||
      (nv_cmd_ptr->data_ptr->imsi_addr_num.num == 0))
      {
        nvruim_imsi_m_cache_buf.imsi_class = 0;
      }
      else
      {
        nvruim_imsi_m_cache_buf.imsi_class = 1;
      }
      nvruim_write_item((byte *) &(nvruim_imsi_m_cache_buf.imsi_class),
                        1,
                        0,
                        UIM_CDMA_IMSI_M);

      nvruim_write_item((byte *) &(nvruim_imsi_m_cache_buf.imsi_addr_num),
                        sizeof(nvruim_imsi_m_cache_buf.imsi_addr_num),
                        7,
                        UIM_CDMA_IMSI_M);

      break;

    case NV_IMSI_MCC_I:
      MSG_HIGH("Updating ADM protected IMSI_MCC",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_mcc.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_m_cache_buf.imsi_mcc[0] = 
        (byte)  nv_cmd_ptr->data_ptr->imsi_mcc.imsi_mcc;
      nvruim_imsi_m_cache_buf.imsi_mcc[1] = 
        (byte)  (nv_cmd_ptr->data_ptr->imsi_mcc.imsi_mcc >> 8);

      nvruim_write_item((byte *) &(nvruim_imsi_m_cache_buf.imsi_mcc),
                        sizeof(nvruim_imsi_m_cache_buf.imsi_mcc),
                        8,
                        UIM_CDMA_IMSI_M);
      break;
    default:
      MSG_ERROR("Invalid Item");
      return NV_RUIM_ITEM_NOT_SUPPORTED;

  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_IMSI_M_CACHE_BIT);
}


/*===========================================================================

FUNCTION NVRUIM_WRITE_IMSI_T

DESCRIPTION
Write IMSI_T info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_imsi_t(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  switch(nv_cmd_ptr->item)
  {
    case NV_IMSI_T_S1_I:
      MSG_HIGH("Updating ADM protected IMSI_T_S1",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_t_s1.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }

      nvruim_imsi_t_cache_buf.imsi_s1[0] = 
        (byte)  nv_cmd_ptr->data_ptr->imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
      nvruim_imsi_t_cache_buf.imsi_s1[1] = 
        (byte) (nv_cmd_ptr->data_ptr->imsi_t_s1.min1[NV_CDMA_MIN_INDEX] >> 8);
      nvruim_imsi_t_cache_buf.imsi_s1[2] = 
        (byte) (nv_cmd_ptr->data_ptr->imsi_t_s1.min1[NV_CDMA_MIN_INDEX] >> 16);

      nvruim_write_item((byte *) &(nvruim_imsi_t_cache_buf.imsi_s1),
                        sizeof(nvruim_imsi_t_cache_buf.imsi_s1),
                        3,
                        UIM_CDMA_IMSI_T);
      break;

    case NV_IMSI_T_S2_I:
      MSG_HIGH("Updating ADM protected IMSI_T_S2",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_t_s2.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_t_cache_buf.imsi_s2[0] = 
        (byte)  nv_cmd_ptr->data_ptr->imsi_t_s2.min2[NV_CDMA_MIN_INDEX];
      nvruim_imsi_t_cache_buf.imsi_s2[1] = 
        (byte) (nv_cmd_ptr->data_ptr->imsi_t_s2.min2[NV_CDMA_MIN_INDEX] >> 8);

      nvruim_write_item((byte *) &(nvruim_imsi_t_cache_buf.imsi_s21),
                        sizeof(nvruim_imsi_t_cache_buf.imsi_s2),
                        1,
                        UIM_CDMA_IMSI_T);
      break;
 
    case NV_IMSI_T_11_12_I:
      MSG_HIGH("Updating ADM protected IMSI_T_11_12",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_t_11_12.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_t_cache_buf.imsi_11_12 = 
      (byte)  nv_cmd_ptr->data_ptr->imsi_t_11_12.imsi_11_12;

      nvruim_write_item((byte *) &(nvruim_imsi_t_cache_buf.imsi_11_12),
                        sizeof(nvruim_imsi_t_cache_buf.imsi_11_12),
                        6,
                        UIM_CDMA_IMSI_T);
      break;

    case NV_IMSI_T_ADDR_NUM_I:
      MSG_HIGH("Updating ADM protected IMSI_T_ADDR_NUM",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_t_addr_num.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_t_cache_buf.imsi_addr_num = 
      (byte)  nv_cmd_ptr->data_ptr->imsi_t_addr_num.num |
      NVRUIM_IMSI_PROGRAMMED;
    
      /* Update imsi class when updating addr num */
      if((nv_cmd_ptr->data_ptr->imsi_t_addr_num.num == 0xff) ||
      (nv_cmd_ptr->data_ptr->imsi_t_addr_num.num == 0))
      {
        nvruim_imsi_t_cache_buf.imsi_class = 0;
      }
      else
      {
        nvruim_imsi_t_cache_buf.imsi_class = 1;
      }

      nvruim_write_item((byte *) &(nvruim_imsi_t_cache_buf.imsi_class),
                        1,
                        0,
                        UIM_CDMA_IMSI_T);

      nvruim_write_item((byte *) &(nvruim_imsi_t_cache_buf.imsi_addr_num),
                        sizeof (nvruim_imsi_t_cache_buf.imsi_addr_num),
                        7,
                        UIM_CDMA_IMSI_T);

      break;

    case NV_IMSI_T_MCC_I:
      MSG_HIGH("Updating ADM protected IMSI_T_MCC",0,0,0);
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_t_mcc.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_imsi_t_cache_buf.imsi_mcc[0] = 
      (byte)  nv_cmd_ptr->data_ptr->imsi_t_mcc.imsi_mcc;
      nvruim_imsi_t_cache_buf.imsi_mcc[1] = 
      (byte)  (nv_cmd_ptr->data_ptr->imsi_t_mcc.imsi_mcc >> 8);

      nvruim_write_item((byte *) &(nvruim_imsi_t_cache_buf.imsi_mcc),
                        sizeof (nvruim_imsi_t_cache_buf.imsi_mcc),
                        8,
                        UIM_CDMA_IMSI_T);
      break;

    default:
      break;

  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_IMSI_T_CACHE_BIT);
}


/*===========================================================================

FUNCTION NVRUIM_WRITE_PRL

DESCRIPTION
Write PRL info to card

DEPENDENCIES

RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_prl(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  word pr_list_size = 0;    /* RUIM prl size */
  word offset;              /* Tracking ruim card offset */
  byte num_of_accesses;     /* Devide a long EF to multiple accessers */
  byte num_of_bytes_remain; /* Remainder */
  byte i;

  MSG_HIGH("Updating ADM protected PRL",0,0,0);
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->roaming_list.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  prl_ptr   = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list;
  prl_size[0] = *prl_ptr++;
  prl_size[1] = *prl_ptr;

  pr_list_size = (prl_size[0] << 8) | prl_size[1];

  /* Read the MAX PRL SIZE */
  nvruim_read_item(prl_size,
                   2,
                   0,
                   UIM_CDMA_MAXIMUM_PRL);

  /* Check if the read succeeded. */
  if(nvruim_ruim_status_report == UIM_PASS)
  {
    max_prl_size = (prl_size[0] << 8) | prl_size[1];

    if(pr_list_size > max_prl_size)
    {

      MSG_HIGH("PRL Size too big",0,0,0);
      nv_cmd_ptr->status = *op_status = NV_READONLY_S;
      return NV_RUIM_SUPPORTS_ITEM;
    }
  }
  else
  {
    MSG_HIGH("No Max PRL Size Info",0,0,0);
    nv_cmd_ptr->status = *op_status = NV_READONLY_S;
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_write_item(prl_size,
                    MAX_BYTES_PER_ACCESS,
                    0,
                    UIM_CDMA_PRL);

  num_of_accesses          = (byte) (pr_list_size / MAX_BYTES_PER_ACCESS);
  num_of_bytes_remain      = (byte) (pr_list_size % MAX_BYTES_PER_ACCESS);

  prl_ptr = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list;
  offset = 0;

  for(i=0; i<num_of_accesses; i++)
  {
    nvruim_write_item(prl_ptr,
                      MAX_BYTES_PER_ACCESS,
                      offset,
                      UIM_CDMA_PRL);

    
    prl_ptr += MAX_BYTES_PER_ACCESS;
    offset  += MAX_BYTES_PER_ACCESS;

    if(nvruim_ruim_status_report != UIM_PASS)
      break; /* No need to proceed */

  } /* for */

  if(num_of_bytes_remain != 0)
  {
    /* The last write of the prl */
    nvruim_write_item(prl_ptr,
                      num_of_bytes_remain,
                      offset,
                      UIM_CDMA_PRL);
  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}

/*===========================================================================

FUNCTION NVRUIM_WRITE_ACCOLC

DESCRIPTION
Write ACCOLC info to card

DEPENDENCIES

RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_accolc(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  MSG_HIGH("Updating ADM protected ACCOLC",0,0,0);
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->accolc.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_write_item(
#ifndef FEATURE_NVRUIM_OLD_NV_ACCOLC_TYPE
    (byte *) &(nv_cmd_ptr->data_ptr->accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX]),
#else
    (byte *) &(nv_cmd_ptr->data_ptr->accolc.class[NV_CDMA_MIN_INDEX]),
#endif
                    1,
                    0,
                    UIM_CDMA_ACCOLC);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}
#endif /*FEATURE_NVRUIM_ADM_ACCESS*/

#ifdef FEATURE_UIM_RUIM_SUPPORT_SCI
/*===========================================================================

FUNCTION NVRUIM_WRITE_SCI

DESCRIPTION
Write SCI info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_sci(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_write_item((byte *) &(nv_cmd_ptr->data_ptr->slot_cycle_index),
                     1,
                     0,
                     UIM_CDMA_SCI);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}
#endif /*FEATURE_UIM_RUIM_SUPPORT_SCI*/


/*===========================================================================

FUNCTION NVRUIM_WRITE_SID_NID

DESCRIPTION
Write IMSI_T info to card

DEPENDENCIES

RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_sid_nid(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  byte   i;
  switch(nv_cmd_ptr->item)
  {
    case NV_SID_NID_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->sid_nid.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
    
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      temp_sid_nid_buffer.sid =
         nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].sid;
      temp_sid_nid_buffer.nid =
        nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].nid;
    break;

    case NV_HOME_SID_NID_I:
     /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->home_sid_nid.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
    
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      temp_sid_nid_buffer.sid = nv_cmd_ptr->data_ptr->home_sid_nid.pair[0].sid;
      temp_sid_nid_buffer.nid = nv_cmd_ptr->data_ptr->home_sid_nid.pair[0].nid;
      break;

    default:
      return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  nvruim_write_rec_item((byte *) &temp_sid_nid_buffer,
                        SID_NID_REC_SIZE,
                        1,
                        UIM_ABSOLUTE,
                        UIM_CDMA_HOME_SID_NID);


  if(nvruim_ruim_status_report == UIM_PASS)
  {
    switch(nv_cmd_ptr->item)
    {
      case NV_SID_NID_I:
        /* Write the rest of records. Since we don't know how many records
        total, we just write until it fails */
        for(i = 1;
        (i < NV_MAX_SID_NID) && (temp_sid_nid_buffer.sid != 0); i++)
        {
          /*lint -e661*/
          temp_sid_nid_buffer.sid =
            nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][i].sid;
          temp_sid_nid_buffer.nid =
            nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][i].nid;
          /*lint -restore */

          nvruim_write_rec_item((byte *) &temp_sid_nid_buffer,
                                SID_NID_REC_SIZE,
                                i+1,
                                UIM_ABSOLUTE,
                                UIM_CDMA_HOME_SID_NID);

          if(nvruim_ruim_status_report != UIM_PASS)
          {
            /* Report UIM_PASS since at least the first write is good */
            nvruim_ruim_status_report = UIM_PASS;
            break;
          } /* if */
        } /* for */
        break;

      case NV_HOME_SID_NID_I:
        /* Write the rest of records. Since we don't know how many records
        total, we just write until it fails */
        for(i = 1;
        (i < NV_MAX_HOME_SID_NID) && (temp_sid_nid_buffer.sid != 0); i++)
        {
    
          temp_sid_nid_buffer.sid =
            nv_cmd_ptr->data_ptr->home_sid_nid.pair[i].sid;
          temp_sid_nid_buffer.nid =
            nv_cmd_ptr->data_ptr->home_sid_nid.pair[i].nid;

          nvruim_write_rec_item((byte *) &temp_sid_nid_buffer,
                                SID_NID_REC_SIZE,
                                i+1,
                                UIM_ABSOLUTE,
                                UIM_CDMA_HOME_SID_NID);

          if(nvruim_ruim_status_report != UIM_PASS)
          {
            /* Report UIM_PASS since at least the first write is good */
            nvruim_ruim_status_report = UIM_PASS;
            break;
          } /* if */
        } /* for */
        break;
      default:
        break;
    }
  } /* if */
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);

}

/*===========================================================================

FUNCTION NVRUIM_WRITE_MSISDN

DESCRIPTION
Write MSISDN info to card

DEPENDENCIES

RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_msisdn(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  byte   digit;   /* Holds DIR NUM digit as its being processed */
  byte   i;
  uint32 cache_bit = RUIM_NO_CACHE_VALUE;

  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->dir_number.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* Initialize the MSISDN buffer to all F's */
  for(i = NVRUIM_MSISDN_MDN_INDEX; i < NVRUIM_MSISDN_SIZE; i++)
    msisdn_buffer[i] = 0xFF;

  /* Copy the DIR NUMBER to the MSISDN buffer.
  *
  * Convert the digits as they are copied.  */
  for(i=0; (i < NV_DIR_NUMB_SIZ); i++)
  {
    /* Convert the digit to the MSISDN format */
    digit = nv_cmd_ptr->data_ptr->dir_number.dir_number[i];

    /* Convert '0' to 0x0A. */
    if(digit == '0')
    {
      digit = 0xA;
    }
    /* Convert '*' to 0x0B. */
    else if(digit == '*')
    {
      digit = 0xB;
    }
    /* Convert '#' to 0x0C. */
    else if(digit == '#')
    {
      digit = 0xC;
    }
    /* Convert digits 1-9 to BCD. */
    else if((digit > '0') && (digit <= '9'))
    {
      digit = digit - '0';
    }
    /* If there are less than 10 digits, the buffer will contain an invalid
    value (e.g. zero).  This is used to determine the end of the DIR
    NUMBER string. */
    else
    {
      break;
    }

    /* Place the digit in the byte - based on whether the index is
    even or odd */
    if(i & 0x1)
    {
      /* Fill the MSNibble in the proper byte of the MSISDN */
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] &= 0x0F;
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] |=
        ((digit & 0x0F) << 4);
    }
    else
    { /* The digit index is even */
      /* Fill the LSnibble in the proper byte of the MSISDN */
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] &= 0xF0;
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] |=
        (digit & 0x0F);
    } /* end if - fill the buffer based on whether i is even or odd */
  } /* end for - copy the digits to the MSISDN bufer */

  /* This is the number of BCD digits that are programmed */
  msisdn_buffer[ NVRUIM_MSISDN_ND_INDEX ] = i;

  nvruim_write_rec_item(msisdn_buffer,
                        sizeof ( msisdn_buffer ),
                        1,
                        UIM_ABSOLUTE,
                        UIM_CDMA_MSISDN);

  
  return nvruim_write_sw(nv_cmd_ptr, op_status, cache_bit);
}

/*===========================================================================

FUNCTION NVRUIM_WRITE_DIR_NUMBER_PCS

DESCRIPTION
Write MSISDN info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_dir_number_pcs(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  byte i;
  uint32 cache_bit = RUIM_NO_CACHE_VALUE;

  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->mob_dir_number.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /* Initialize the MSISDN buffer to all F's */
  for(i = NVRUIM_MSISDN_MDN_INDEX; i < NVRUIM_MSISDN_SIZE; i++)
    msisdn_buffer[i] = 0xFF;

  /* Copy the DIR NUMBER to the MSISDN */

  /* This is the number of BCD digits to program into the MSISDN */
  msisdn_buffer[ NVRUIM_MSISDN_ND_INDEX ] =
  nv_cmd_ptr->data_ptr->mob_dir_number.n_digits & 0x0F;

  /* Copy the digits from the MSISDN to the DIR NUM buffer. */
  for(i = 0; (i < nv_cmd_ptr->data_ptr->mob_dir_number.n_digits); i++)
  {
    /* Retrieve the digit from the byte based on whether the index is
    even or odd */
    if(i & 0x1)
    {
      /* Fill the MSNibble in the proper byte of the MSISDN */
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] &= 0x0F;
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] |=
      (byte) (nv_cmd_ptr->data_ptr->mob_dir_number.digitn[i] << 4);
    }
    else
    { /* The digit index is even */
      /* Fill the LSnibble in the proper byte of the MSISDN */
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] &= 0xF0;
      msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] |=
      nv_cmd_ptr->data_ptr->mob_dir_number.digitn[i] & 0x0F;
    } /* end if - index is odd */

  } /* end for - copy all the digits to the DIR NUM buffer */
  nvruim_write_rec_item(msisdn_buffer,
                      sizeof ( msisdn_buffer ),
                      1,
                      UIM_ABSOLUTE,
                      UIM_CDMA_MSISDN);

  return nvruim_write_sw(nv_cmd_ptr, op_status, cache_bit);
}

/*===========================================================================

FUNCTION NVRUIM_WRITE_COUNT

DESCRIPTION
Write COUNT info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_count(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  /* Use the MS-ISDN buffer to send the 2 bytes. NV gets only one byte in
  the command */
  msisdn_buffer[0]        = 0;
  msisdn_buffer[1]        = nv_cmd_ptr->data_ptr->count.count;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_write_rec_item(msisdn_buffer,
                        1,
                        0,
                        UIM_PREVIOUS,
                        UIM_CDMA_CC);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}

/*===========================================================================

FUNCTION NVRUIM_WRITE_ANALOG_FIRSTCHIP

DESCRIPTION
Write Analog Chan Pref Info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status
===========================================================================*/
static nv_ruim_support_status nvruim_write_analog_firstchip(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->analog_firstchp.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_write_item((byte *) &(nv_cmd_ptr->data_ptr->analog_firstchp.channel),
                     NVRUIM_SIZE(analog_firstchp.channel),
                     (word) FPOS(nvruim_analog_ch_pref, analog_firstchp),
                     UIM_CDMA_ANALOG_CHAN_PREF);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}

/*===========================================================================
FUNCTION NVRUIM_WRITE_HOME_SID

DESCRIPTION
Write HOME_SID info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_home_sid(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->analog_home_sid.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_write_item((byte *) &(nv_cmd_ptr->data_ptr->analog_home_sid.sid),
                     NVRUIM_SIZE(analog_home_sid.sid),
                     0,
                     UIM_CDMA_ANALOG_HOME_SID);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}

/*===========================================================================
FUNCTION NVRUIM_WRITE_MOB_TERM

DESCRIPTION
Write MOB Term info to the card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_mob_term(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  boolean skip_write = FALSE; /* Indicates write to RUIM should be skipped bcos
  it is not needed due to the pre-read failing */

  /* MOB_TERM_HOME, MOB_TERM_FOR_SID, and MOB_TERM_FOR_NID are in
     the same byte. First read the byte and modify the bit */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_read_item(&nvruim_call_term_mode_pref_cache_buf,
                     1,
                     0,
                     UIM_CDMA_CALL_TERM_MODE_PREF);

  switch(nv_cmd_ptr->item)
  {
    case NV_MOB_TERM_HOME_I:
        /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->mob_term_home.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }

      /* Check if the read succeeded. */
      if(nvruim_ruim_status_report == UIM_PASS)
      {
        /* Reset MOB_TERM_HOME bit (bit 2) */
        nvruim_call_term_mode_pref_cache_buf &= 0xFB;
    
        if(nv_cmd_ptr->data_ptr->mob_term_home.enabled[NV_CDMA_MIN_INDEX])
        {
          /*  MOB_TERM_HOME bit (bit 2) */
          nvruim_call_term_mode_pref_cache_buf |= 0x04;
        }
      }
      else
      {
        /* Do not try writing since the read failed. */
        skip_write = TRUE;
      }
      break;
    
    case NV_MOB_TERM_FOR_SID_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->mob_term_for_sid.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }

      /* Check if the read succeeded. */
      if(nvruim_ruim_status_report == UIM_PASS)
      {
        /* Reset MOB_TERM_FOR_SID bit (bit 1) */
        nvruim_call_term_mode_pref_cache_buf &= 0xFD;
    
        if(nv_cmd_ptr->data_ptr->mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX])
        {
          /*  MOB_TERM_FOR_SID bit (bit 1) */
          nvruim_call_term_mode_pref_cache_buf |= 0x02;
        }
      }
      else
      {
        /* Do not try writing since the read failed. */
        skip_write = TRUE;
      }
      break;

    case NV_MOB_TERM_FOR_NID_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->mob_term_for_nid.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
    
      /* Check if the read succeeded. */
      if(nvruim_ruim_status_report == UIM_PASS)
      {
        /* Reset MOB_TERM_FOR_SID bit (bit 0) */
        nvruim_call_term_mode_pref_cache_buf &= 0xFE;
    
        if(nv_cmd_ptr->data_ptr->mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX])
        {
          /*  MOB_TERM_FOR_SID bit (bit 0) */
          nvruim_call_term_mode_pref_cache_buf |= 0x01;
        }
      }
      else
      {
        /* Do not try writing since the read failed. */
        skip_write = TRUE;
      }
      break;
    default:
      return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!skip_write)
  {
    nvruim_write_item(&nvruim_call_term_mode_pref_cache_buf,
                       1,
                       0,
                       UIM_CDMA_CALL_TERM_MODE_PREF);
  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_CALL_TERM_MODE_PREF_CACHE_BIT);

}

/*===========================================================================
FUNCTION NVRUIM_WRITE_SPC_ENABLE

DESCRIPTION
Write SPC ENABLE info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status
===========================================================================*/
static nv_ruim_support_status nvruim_write_spc_enable(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_read_item(&nvruim_otapa_spc_enable_cache_buf,
                  1,
                  0,
                  UIM_CDMA_OTAPA_SPC_ENABLE);

  /* Check if the read succeeded. */
  if(nvruim_ruim_status_report == UIM_PASS)
  {
    switch(nv_cmd_ptr->item)
    {
    #ifdef FEATURE_OTASP_OTAPA
      case NV_OTAPA_ENABLED_I:
        /* Only process the NAM parameters for the RUIM
        This check assumes the first byte of the data contains the NAM */
        if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->otapa_enabled.nam)
        {
          /* Go to NV for this item */
          return NV_RUIM_ITEM_NOT_SUPPORTED;
        }

        /* Reset the otapa enable bit which indicates that OTAPA is enabled */
        nvruim_otapa_spc_enable_cache_buf &= NVRUIM_OTAPA_ENABLE_MASK;
    
        /* A value of 0 indicates that otapa is enabled and '1' indicates
        that otapa is disabled */
        if(!nv_cmd_ptr->data_ptr->otapa_enabled.enabled)
        {
          /*  OTAPA_Enable bit (bit 0) */
          nvruim_otapa_spc_enable_cache_buf |= NVRUIM_OTAPA_DISABLE;
        }
        break;
    #endif /*FEATURE_OTASP_OTAPA*/
    
      case NV_SPC_CHANGE_ENABLED_I:
#ifdef FEATURE_VERSION_W208S
		if(nv_cmd_ptr->data_ptr->spc_change_enabled)
        {
			nvruim_otapa_spc_enable_cache_buf &= NVRUIM_SPC_DISABLE;
			nvruim_otapa_spc_enable_cache_buf |= NVRUIM_SPC_ENABLE_MASK;
        }		
#else
        /* Reset SPC_Change_Enable bit (bit 4) */
        nvruim_otapa_spc_enable_cache_buf &= NVRUIM_SPC_ENABLE_MASK;
    
        /* A value of 0 indicates that SPC is enabled and '1' indicates
        that SPC is disabled */
        if(!nv_cmd_ptr->data_ptr->spc_change_enabled)
        {
          nvruim_otapa_spc_enable_cache_buf |= NVRUIM_SPC_DISABLE;
        }
#endif    
        break;
      default:
        return NV_RUIM_ITEM_NOT_SUPPORTED;
    }

    nvruim_write_item(&nvruim_otapa_spc_enable_cache_buf,
                      1,
                      0,
                      UIM_CDMA_OTAPA_SPC_ENABLE);
  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_OTAPA_SPC_ENABLE_CACHE_BIT);

}

#ifdef FEATURE_OTASP_OTAPA
/*===========================================================================
FUNCTION NVRUIM_WRITE_NAM_LOCK

DESCRIPTION
Write NAM LOCK info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_nam_lock(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->nam_lock.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_read_item(&nvruim_nam_lock_buf,
                   1,
                   0,
                   UIM_CDMA_NAM_LOCK);

  /* Check if the read succeeded. */
  if(nvruim_ruim_status_report == UIM_PASS)
  {
    /* Reset NAM_LOCK bit (bit 2) */
    nvruim_nam_lock_buf &= NVRUIM_NAM_LOCK_MASK;

    if(nv_cmd_ptr->data_ptr->nam_lock.enabled)
    {
      nvruim_nam_lock_buf |= NVRUIM_NAM_LOCK;
    }

    nvruim_write_item(&nvruim_nam_lock_buf,
                       1,
                       0,
                       UIM_CDMA_NAM_LOCK);
  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}
#endif /*FEATURE_OTASP_OTAPA*/


/*===========================================================================
FUNCTION NVRUIM_WRITE_PREF

DESCRIPTION
Write MODE PREF, SYS PREF, CDMA PREf, ANALOG PREF  info to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_pref(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  byte   temp_service_pref_buf;
#ifndef FEATURE_SSPR_ENHANCEMENTS
  byte sys;
#endif


  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  nvruim_read_item(&nvruim_svc_pref_cache_buf,
                   sizeof(nvruim_svc_pref_cache_buf),
                   0,
                   UIM_CDMA_SERVICE_PREF);
  if(nvruim_ruim_status_report == UIM_PASS)
  {
    switch(nv_cmd_ptr->item)
    {
    #ifdef FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM
      case NV_PREF_MODE_I:
        switch(nv_cmd_ptr->data_ptr->pref_mode.mode)
        {
          /* The IS-820 doesn't match the NV structure.
          Find closest matches here*/
          case NV_MODE_ANALOG_PREF:
            temp_service_pref_buf = NVRUIM_ANALOG_PREF;
            break;
          case NV_MODE_AUTOMATIC:
            temp_service_pref_buf = NVRUIM_CDMA_PREF;
            break;
          case NV_MODE_ANALOG_ONLY:
            temp_service_pref_buf = NVRUIM_ANALOG_ONLY;
            break;
          case NV_MODE_DIGITAL_ONLY:
          case NV_MODE_PCS_CDMA_ONLY:
          case NV_MODE_CELL_CDMA_ONLY:
            temp_service_pref_buf = NVRUIM_CDMA_ONLY;
            break;
          default: /* Set to NV_MODE_AUTOMATIC */
            temp_service_pref_buf = NVRUIM_NO_PREF;
            break;
        }
        nvruim_svc_pref_cache_buf &= ~NVRUIM_PREF_MODE_MASK;
        nvruim_svc_pref_cache_buf |= temp_service_pref_buf;
        break;
      #endif /*FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM*/
      
      #ifdef FEATURE_SSPR_ENHANCEMENTS
      case NV_SYSTEM_PREF_I:
        switch(nv_cmd_ptr->data_ptr->system_pref.sys)
        {
          /* The IS-820 doesn't match the NV structure.
          Find closest matches here*/
          case NV_SP_A_ONLY:
            temp_service_pref_buf = NVRUIM_A_ONLY;
            break;
          case NV_SP_B_ONLY:
            temp_service_pref_buf = NVRUIM_B_ONLY;
            break;
          default: /* Set to no preference */
            temp_service_pref_buf = NVRUIM_NO_PREF;
            break;
        }
    
        nvruim_svc_pref_cache_buf &= ~NVRUIM_SYSTEM_PREF_MASK;
        nvruim_svc_pref_cache_buf |= temp_service_pref_buf;
        break;

      #else
      case NV_CDMA_PREF_SERV_I:
      case NV_ANALOG_PREF_SERV_I:
        if(nv_cmd_ptr->item == NV_CDMA_PREF_SERV_I)
          sys = nv_cmd_ptr->data_ptr->cdma_pref_serv.sys;
        else
          sys = nv_cmd_ptr->data_ptr->analog_pref_serv.sys;
        switch(sys)
        {
          case NV_SYS_A_PREF:
            temp_service_pref_buf = NVRUIM_A_PREF;
            break;
          case NV_SYS_B_PREF:
            temp_service_pref_buf = NVRUIM_B_PREF;
            break;
          case NV_SYS_A_ONLY:
            temp_service_pref_buf = NVRUIM_A_ONLY;
            break;
          case NV_SYS_B_ONLY:
            temp_service_pref_buf = NVRUIM_B_ONLY;
            break;
          default: /* Set to no preference */
            temp_service_pref_buf = NVRUIM_NO_PREF;
            break;
        }
    
        nvruim_svc_pref_cache_buf &= ~NVRUIM_SYSTEM_PREF_MASK;
        nvruim_svc_pref_cache_buf |= temp_service_pref_buf;
        break;
      #endif /*FEATURE_SSPR_ENHANCEMENTS*/

      default:
        return NV_RUIM_ITEM_NOT_SUPPORTED;
    }

    nvruim_write_item(&nvruim_svc_pref_cache_buf,
                      sizeof(nvruim_svc_pref_cache_buf),
                      0,
                      UIM_CDMA_SERVICE_PREF);

  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_SVC_PREF_CACHE_BIT);

}

/*===========================================================================
FUNCTION NVRUIM_WRITE_TMSI

DESCRIPTION
Write TMSI INFO to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_tmsi(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  byte   i;

  switch(nv_cmd_ptr->item)
  {
    case NV_ASSIGNING_TMSI_ZONE_LEN_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->assigning_tmsi_zone_length.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }

      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_tmsi_cache_buf.length =
        nv_cmd_ptr->data_ptr->assigning_tmsi_zone_length.length;
    
      nvruim_tmsi_cache_buf.length =
        nv_cmd_ptr->data_ptr->assigning_tmsi_zone_length.length;

      nvruim_write_item((byte *) &(nvruim_tmsi_cache_buf.length),
                       1,
                       0,
                       UIM_CDMA_TMSI);
      break;

    case NV_ASSIGNING_TMSI_ZONE_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->assigning_tmsi_zone.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
   
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      /* Copy qword */
      for(i=0; i<8; i++)
      {
        nvruim_tmsi_cache_buf.zone[i] =
        QW_BYTE(nv_cmd_ptr->data_ptr->assigning_tmsi_zone.zone ,i);
      }

      nvruim_write_item((byte *)&nvruim_tmsi_cache_buf.zone[0],
                         8,
                         1,
                         UIM_CDMA_TMSI);
      break;

    case NV_TMSI_CODE_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->tmsi_code.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
    
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_tmsi_cache_buf.code[3] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_code.code >> 24);
      nvruim_tmsi_cache_buf.code[2] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_code.code >> 16);
      nvruim_tmsi_cache_buf.code[1] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_code.code >> 8);
      nvruim_tmsi_cache_buf.code[0] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_code.code);

      nvruim_write_item((byte *)&nvruim_tmsi_cache_buf.code[0],
                        4,
                        9,
                        UIM_CDMA_TMSI);

      break;

    case NV_TMSI_EXP_I:
      /* Only process the NAM parameters for the RUIM
      This check assumes the first byte of the data contains the NAM */
      if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->tmsi_exp_timer.nam)
      {
        /* Go to NV for this item */
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
    
      if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
      {
        return NV_RUIM_SUPPORTS_ITEM;
      }
      nvruim_tmsi_cache_buf.time[2] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_exp_timer.time >> 16);
      nvruim_tmsi_cache_buf.time[1] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_exp_timer.time >> 8);
      nvruim_tmsi_cache_buf.time[0] =
        (byte) (nv_cmd_ptr->data_ptr->tmsi_exp_timer.time);

      nvruim_write_item((byte *) &nvruim_tmsi_cache_buf.time[0],
                       3,
                       13,
                       UIM_CDMA_TMSI);

      break;

    default:
      return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_TMSI_CACHE_BIT);
}

/*===========================================================================
FUNCTION NVRUIM_WRITE_LOCN_REGN_IND

DESCRIPTION
Write LOCN REGN IND to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_locn_regn_ind(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  switch(nv_cmd_ptr->item)
  {
    case NV_NXTREG_I:
      nvruim_analog_reg_ind_cache_buf.nxtreg[2] =
        (byte) (nv_cmd_ptr->data_ptr->nxtreg >> 16);
      nvruim_analog_reg_ind_cache_buf.nxtreg[1] =
        (byte) (nv_cmd_ptr->data_ptr->nxtreg >> 8);
      nvruim_analog_reg_ind_cache_buf.nxtreg[0] = 
        (byte) (nv_cmd_ptr->data_ptr->nxtreg);

      nvruim_write_item((byte *) nvruim_analog_reg_ind_cache_buf.nxtreg,
                        3,
                        13,
                        UIM_CDMA_ANALOG_LOCN_AND_REGN_IND);
      break;
  
    case NV_LSTSID_I:
      nvruim_analog_reg_ind_cache_buf.sid[1] =
        (byte) (nv_cmd_ptr->data_ptr->lstsid >> 8);
      nvruim_analog_reg_ind_cache_buf.sid[0] = 
         (byte) (nv_cmd_ptr->data_ptr->lstsid);

      nvruim_write_item((byte *) &nvruim_analog_reg_ind_cache_buf.sid[0],
                        2,
                        3,
                        UIM_CDMA_ANALOG_LOCN_AND_REGN_IND);

      break;
  
    case NV_LOCAID_I:

      nvruim_read_item((byte *)nvruim_analog_reg_ind_cache_buf.locaid_pureg,
                        2,
                        5,
                        UIM_CDMA_ANALOG_LOCN_AND_REGN_IND);
    
      if(nvruim_ruim_status_report == UIM_PASS)
      {
        nvruim_analog_reg_ind_cache_buf.locaid_pureg[1] |=
          (byte) ((nv_cmd_ptr->data_ptr->locaid >> 8) & 0x0F);
        nvruim_analog_reg_ind_cache_buf.locaid_pureg[0] =
          (byte) (nv_cmd_ptr->data_ptr->locaid);

        nvruim_write_item((byte *)nvruim_analog_reg_ind_cache_buf.locaid_pureg,
                          2,
                          5,
                          UIM_CDMA_ANALOG_LOCN_AND_REGN_IND);
      }
      break;

    case NV_PUREG_I:
      nvruim_read_item((byte *)nvruim_analog_reg_ind_cache_buf.locaid_pureg,
                        2,
                        5,
                        UIM_CDMA_ANALOG_LOCN_AND_REGN_IND);
    
      if(nvruim_ruim_status_report == UIM_PASS)
      {
        if(nv_cmd_ptr->data_ptr->pureg)
          nvruim_analog_reg_ind_cache_buf.locaid_pureg[1] |= 0x080;
        else
          nvruim_analog_reg_ind_cache_buf.locaid_pureg[1] &= 0x07F;

        nvruim_write_item((byte *)nvruim_analog_reg_ind_cache_buf.locaid_pureg,
                          2,
                          5,
                          UIM_CDMA_ANALOG_LOCN_AND_REGN_IND);
      }
      break;
    default:
      return NV_RUIM_ITEM_NOT_SUPPORTED;

  }
  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_ANALOG_REG_IND_CACHE_BIT);
}


/*===========================================================================
FUNCTION NVRUIM_WRITE_ZONE_LIST

DESCRIPTION
Write LOCN ZONE LIST to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_zone_list(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_zone_reg_ind_cache_buf.reg_zone[1] =
    (byte) (nv_cmd_ptr->data_ptr->zone_list.reg_zone >> 8);
  nvruim_zone_reg_ind_cache_buf.reg_zone[0] =
    (byte) (nv_cmd_ptr->data_ptr->zone_list.reg_zone);

  nvruim_zone_reg_ind_cache_buf.sid[1] =
    (byte) (nv_cmd_ptr->data_ptr->zone_list.sid >> 8);
  nvruim_zone_reg_ind_cache_buf.sid[0] =
    (byte) (nv_cmd_ptr->data_ptr->zone_list.sid);

  nvruim_zone_reg_ind_cache_buf.nid[1] =
    (byte) (nv_cmd_ptr->data_ptr->zone_list.nid >> 8);
  nvruim_zone_reg_ind_cache_buf.nid[0] =
    (byte) (nv_cmd_ptr->data_ptr->zone_list.nid);

  if(nv_cmd_ptr->data_ptr->zone_list.band_class != 0)
  { 
    /* PCS band or others. The Cellulaer band information can be
    retreaved from the sid.  */
    nvruim_zone_reg_ind_cache_buf.freq_block =
      (byte) (nv_cmd_ptr->data_ptr->zone_list.block_or_sys);
  }

  nvruim_zone_reg_ind_cache_buf.band_class =
    nv_cmd_ptr->data_ptr->zone_list.band_class;

  nvruim_write_rec_item((byte *) &(nvruim_zone_reg_ind_cache_buf),
                        sizeof(nvruim_zone_reg_ind_type),
                        1,
                        UIM_ABSOLUTE,
                        UIM_CDMA_ZONE_BASED_REGN_IND);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_ZONE_REG_IND_CACHE_BIT);
}

/*===========================================================================
FUNCTION NVRUIM_WRITE_SID_NID_LIST

DESCRIPTION
Write SID NID List  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_sid_nid_list(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  uint32 cache_bit = RUIM_SYS_REG_IND_CACHE_BIT; /* Cache bit representing cache buffer*/

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_read_item((byte *) &(nvruim_sys_reg_ind_cache_buf),
                   sizeof(nvruim_sys_reg_ind_type),
                   1,
                   UIM_CDMA_SYS_REGN_IND);


  if(nvruim_ruim_status_report == UIM_PASS)
  {
    nvruim_sys_reg_ind_cache_buf.sid[1] =
      (byte) ((nv_cmd_ptr->data_ptr->sid_nid_list.sid & 0x7FFF) >> 8);
    nvruim_sys_reg_ind_cache_buf.sid[0] =
      (byte) (nv_cmd_ptr->data_ptr->sid_nid_list.sid);

    nvruim_sys_reg_ind_cache_buf.nid[1] =
      (byte) (nv_cmd_ptr->data_ptr->sid_nid_list.nid >> 8);
    nvruim_sys_reg_ind_cache_buf.nid[0] =
      (byte) (nv_cmd_ptr->data_ptr->sid_nid_list.nid);

    /* Preserve the 3 LSB bits of ZONE_TIMER */
    nvruim_sys_reg_ind_cache_buf.band_class &= ~NVRUIM_BAND_CLASS_MASK;
    nvruim_sys_reg_ind_cache_buf.band_class |=
      (byte) (nv_cmd_ptr->data_ptr->sid_nid_list.band_class << 3);

    if(nv_cmd_ptr->data_ptr->sid_nid_list.band_class != 0)
    { 
      /* PCS band or others. The Cellulaer band information can be
         retreaved from the sid.  */
      nvruim_sys_reg_ind_cache_buf.freq_block =
         (byte) (nv_cmd_ptr->data_ptr->sid_nid_list.block_or_sys);
    }

    nvruim_write_item((byte *) &(nvruim_sys_reg_ind_cache_buf),
                     sizeof(nvruim_sys_reg_ind_type),
                     1,
                     UIM_CDMA_SYS_REGN_IND);

  }

  return nvruim_write_sw(nv_cmd_ptr, op_status, cache_bit);
}


/*===========================================================================
FUNCTION NVRUIM_WRITE_DIST_REG

DESCRIPTION
Write DIST REG  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_dist_reg(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_dist_reg_ind_cache_buf.base_lat_reg[2] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.base_lat_reg >> 16);
  nvruim_dist_reg_ind_cache_buf.base_lat_reg[1] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.base_lat_reg >> 8);
  nvruim_dist_reg_ind_cache_buf.base_lat_reg[0] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.base_lat_reg);

  nvruim_dist_reg_ind_cache_buf.base_long_reg[2] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.base_long_reg >> 16);
  nvruim_dist_reg_ind_cache_buf.base_long_reg[1] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.base_long_reg >> 8);
  nvruim_dist_reg_ind_cache_buf.base_long_reg[0] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.base_long_reg);

  nvruim_dist_reg_ind_cache_buf.reg_dist_reg[1] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.reg_dist_reg >> 8);
  nvruim_dist_reg_ind_cache_buf.reg_dist_reg[0] =
    (byte) (nv_cmd_ptr->data_ptr->dist_reg.reg_dist_reg);

  nvruim_write_item((byte *) &(nvruim_dist_reg_ind_cache_buf),
                    sizeof(nvruim_dist_reg_ind_type),
                    0,
                    UIM_CDMA_DIST_BASED_REGN_IND);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_DIST_REG_IND_CACHE_BIT);
}

#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================
FUNCTION NVRUIM_WRITE_DS_QSMIP

DESCRIPTION
Write DS QCMIP to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_ds_qcmip(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support())
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_mip_svc && !nvruim_3gpd_sip_svc)
  {
    *op_status = NV_DONE_S;
    return NV_RUIM_SUPPORTS_ITEM;
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_3GPD_op_mode_ef_buf = (nv_cmd_ptr->data_ptr->ds_qcmip) & NVRUIM_3GPDOPM_MASK;
  nvruim_write_item((byte *) &(nvruim_3GPD_op_mode_ef_buf),
                    1,
                    0,
                    UIM_CDMA_3GPD_3GPDOPM);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE);
}

/*===========================================================================
FUNCTION NVRUIM_WRITE_DS_MIP_ACTIVE_PROF

DESCRIPTION
Write ACTIVE PROF  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_ds_mip_active_prof(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() 
  || UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(nvruim_3gpd_mip_svc == FALSE)
  {
    *op_status = NV_DONE_S;
    return NV_RUIM_SUPPORTS_ITEM;
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_mipsp_ef_buf      = (nv_cmd_ptr->data_ptr->ds_mip_active_prof) << NVRUIM_MIPSP_SHIFT;

  nvruim_write_item((byte *) &(nvruim_mipsp_ef_buf),
                    1,
                    0,
                    UIM_CDMA_3GPD_MIPSP);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_MIPSP_EF_CACHE_BIT);
}


#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
/*===========================================================================
FUNCTION NVRUIM_WRITE_DS_MIP_ACTIVE_PROF

DESCRIPTION
Write ACTIVE PROF  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_ds_sip_active_profile(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  if(UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support() 
  || UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support())
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(nvruim_3gpd_sip_svc == FALSE)   
  {
    *op_status = NV_DONE_S;
    return NV_RUIM_SUPPORTS_ITEM;
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_sipsp_ef_buf      = (nv_cmd_ptr->data_ptr->ds_sip_active_profile_index) << NVRUIM_SIPSP_SHIFT;

  nvruim_write_item((byte *) &(nvruim_sipsp_ef_buf),
                    1,
                    0,
                    UIM_CDMA_3GPD_SIPSP);

  return nvruim_write_sw(nv_cmd_ptr, op_status, RUIM_SIPSP_EF_CACHE_BIT);
}
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */


/*===========================================================================
FUNCTION NVRUIM_WRITE_PPP_PASSWD

DESCRIPTION
Write PPP Passwd  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_ppp_passwd(
  nv_cmd_type       *nv_cmd_ptr,
  nv_stat_enum_type *op_status
)
{
  boolean skip_write = FALSE; /* Indicates write to RUIM should be skipped bcos
  it is not needed due to the pre-read failing */
  uint32 cache_bit = RUIM_SIPPAPSS_EF_CACHE_BIT; /* Cache bit representing cache buffer*/
  byte   i;
  word   src_bit_count;
  word   dest_bit_count;
  byte   num_nai;
  byte   nai_count;
  byte   old_nvruim_sippapss_ef_buf[NVRUIM_MAX_SIPPAPSS_SIZE];
  byte   profile_number;
  byte   ss_length;
  byte   stored_ss_size=0;


  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() 
  || UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;  
  }

  /* No need to write PPP NAI\Password Legacy NV items to 
     RUIM for OMH card*/
  if((nv_cmd_ptr->item == NV_PPP_PASSWORD_I) && 
     (TRUE == nvruim_data_3gpd_ext_support()))
  {
    MSG_HIGH("Not writing legacy NV_PPP_PASSWORD_I NV items to RUIM",0,0,0);
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
  
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(nvruim_3gpd_sip_svc == FALSE)
  {
    *op_status = NV_DONE_S;
    return NV_RUIM_SUPPORTS_ITEM;
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_read_item((byte *) &(old_nvruim_sippapss_ef_buf),
                  NVRUIM_MAX_SIPPAPSS_SIZE,
                  0,
                  UIM_CDMA_3GPD_SIPPAPSS);

  if(nvruim_ruim_status_report == UIM_PASS)
  {
    /* The src and dest counter are "bit pointers" used as cursors in the buffers */
    /* Skip the first two fields to get to the first profile */
    src_bit_count = FSIZ(nvruim_sippapss_ef_fields,length_of_block_field) +
      FSIZ(nvruim_sippapss_ef_fields, num_nai_field); 
    dest_bit_count = src_bit_count;
    nai_count = 0;
    /* The number of NAIs is specified in bits 9-12 of the file */
    num_nai = (old_nvruim_sippapss_ef_buf[1] & NVRUIM_SIPPAPSS_NUM_NAI_MASK) >> 4;
    /* Clear out the buffer with zeros */
    memset(nvruim_sippapss_ef_buf, 0x0, NVRUIM_MAX_SIPPAPSS_SIZE);
    /* Copy the number of NAIs from the old buffer to the new buffer */
    b_copy((void*) old_nvruim_sippapss_ef_buf, 
      FSIZ(nvruim_sippapss_ef_fields, length_of_block_field), 
      (void*) nvruim_sippapss_ef_buf, 
      FSIZ(nvruim_sippapss_ef_fields, length_of_block_field), 
      FSIZ(nvruim_sippapss_ef_fields, num_nai_field));
    /* Clear out the buffer containing ptrs to SIP Profiles */
    for(i = 0; i < NVRUIM_MAX_NUM_SIP_PROFILES; i++)
    {
      nvruim_sip_pap_ss_profile_bit_index[i] = NVRUIM_NO_PROFILE_INDEX_SET;
    }
    /* Initialize skip_write to TRUE (so the write fails), when the profile
    to be overwritten is found, set skip_write to FALSE */
    skip_write = TRUE;
    while(nai_count < num_nai)
    {
      profile_number = b_unpackb(old_nvruim_sippapss_ef_buf, src_bit_count, 
                            FSIZ(nvruim_sippapss_ef_fields, profile_num_field));
      b_copy((void*) old_nvruim_sippapss_ef_buf, src_bit_count, 
             (void*) nvruim_sippapss_ef_buf, dest_bit_count, 
             FSIZ(nvruim_sippapss_ef_fields, profile_num_field));
      nvruim_sip_pap_ss_profile_bit_index[profile_number] = dest_bit_count;
      src_bit_count += FSIZ(nvruim_sippapss_ef_fields, profile_num_field);
      dest_bit_count += FSIZ(nvruim_sippapss_ef_fields, profile_num_field);
      if((nv_cmd_ptr->item == NV_PPP_PASSWORD_I) &&
          profile_number == nvruim_preferred_sip_profile)
      {
        /* Legacy Item for Single SIP Profile Support */
        skip_write = FALSE;
        if(nv_cmd_ptr->data_ptr->ppp_password.password_len > 
           NVRUIM_MAX_SIPPAPSS_SS_SIZE)
        {
           MSG_HIGH("SIPPAPSS is too large for card.  Truncating",0,0,0);
           stored_ss_size = NVRUIM_MAX_SIPPAPSS_SS_SIZE;
        }
        else
        {
           stored_ss_size = nv_cmd_ptr->data_ptr->ppp_password.password_len;
        }
        b_packb(stored_ss_size, 
                nvruim_sippapss_ef_buf, dest_bit_count, 
                FSIZ(nvruim_sippapss_ef_fields, ss_length_field));
        dest_bit_count += FSIZ(nvruim_sippapss_ef_fields, ss_length_field);
        /* Bypass the old SS size, and SS for this profile */
        src_bit_count += (b_unpackb(old_nvruim_sippapss_ef_buf, src_bit_count, 
                         FSIZ(nvruim_sippapss_ef_fields, ss_length_field)) * 8) +
                         FSIZ(nvruim_sippapss_ef_fields, ss_length_field); 
        b_copy((void*) nv_cmd_ptr->data_ptr->ppp_password.password, 0, 
               (void*) nvruim_sippapss_ef_buf, dest_bit_count, 
               stored_ss_size * 8);
        dest_bit_count += (stored_ss_size * 8);

      } /* Legacy Item */
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      else if((nv_cmd_ptr->item == NV_DS_SIP_PPP_SS_INFO_I) &&
          profile_number == nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.index) 
      {
        skip_write = FALSE;
        if(nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss_length > 
           NVRUIM_MAX_SIPPAPSS_SS_SIZE)
        {
           MSG_HIGH("SIPPAPSS is too large for card.  Truncating",0,0,0);
           stored_ss_size = NVRUIM_MAX_SIPPAPSS_SS_SIZE;
        }
        else
        {
           stored_ss_size = nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss_length;
        }
        b_packb(stored_ss_size, 
                nvruim_sippapss_ef_buf, dest_bit_count, 
                FSIZ(nvruim_sippapss_ef_fields, ss_length_field));
        dest_bit_count += FSIZ(nvruim_sippapss_ef_fields, ss_length_field);
        /* Bypass the old SS size, and SS for this profile */
        src_bit_count += (b_unpackb(old_nvruim_sippapss_ef_buf, src_bit_count, 
                         FSIZ(nvruim_sippapss_ef_fields, ss_length_field)) * 8) +
                         FSIZ(nvruim_sippapss_ef_fields, ss_length_field); 
        b_copy((void*) nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss, 0, 
               (void*) nvruim_sippapss_ef_buf, dest_bit_count, 
               stored_ss_size * 8);
        dest_bit_count += (stored_ss_size * 8);

      } /* Multiple Profiles Support */
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      else
      {
        ss_length = b_unpackb(old_nvruim_sippapss_ef_buf, src_bit_count, 
                      FSIZ(nvruim_sippapss_ef_fields, ss_length_field)); 
        b_copy((void*) old_nvruim_sippapss_ef_buf, src_bit_count, 
               (void*) nvruim_sippapss_ef_buf, dest_bit_count,
               (FSIZ(nvruim_sippapss_ef_fields, ss_length_field) + (ss_length * 8)));
        src_bit_count += (FSIZ(nvruim_sippapss_ef_fields, ss_length_field) + (ss_length * 8));
        dest_bit_count += (FSIZ(nvruim_sippapss_ef_fields, ss_length_field) + (ss_length * 8));

      } 
      nai_count++;
    } /* end While */
    /* Make sure that the byte count includes a rounded up count of bits */
    if(dest_bit_count / 8 != 0)
    {
      nvruim_sippapss_ef_buf[0] = (dest_bit_count / 8) + 1;
    }
    else
    {
      nvruim_sippapss_ef_buf[0] = dest_bit_count / 8;
    }

  }
  else
  {
    skip_write = TRUE;
  }
  
  if(!skip_write)
  {
    nvruim_write_item((byte *) &(nvruim_sippapss_ef_buf),
                      NVRUIM_MAX_SIPPAPSS_SIZE,
                      0,
                      UIM_CDMA_3GPD_SIPPAPSS);
  }

  return nvruim_write_sw(nv_cmd_ptr, op_status, cache_bit);
}

#endif /*FEATURE_UIM_SUPPORT_3GPD*/

/*===========================================================================
FUNCTION nvruim_write_bcsms_pref

DESCRIPTION
  Write BCSMS preference  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_bcsms_pref
(
  nv_cmd_type         * nv_cmd_ptr,
  nv_stat_enum_type   * op_status
)
{
  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_write_bcsms_pref",0,0,0);
    return NV_RUIM_SUPPORTS_ITEM;
  }
  if( nvruim_bcsms_svc == FALSE )
  {
    MSG_HIGH("Card doesn't support BCSMS Extensions, falling back to NV",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED; 

  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_write_item( (byte*)(& nv_cmd_ptr->data_ptr->sms_bc_user_pref),
                     1,
                     0,
                     UIM_CDMA_BCSMS_PREF );

  return nvruim_write_sw( nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE );

} /* nvruim_write_bcsms_pref() */

/*===========================================================================
FUNCTION nvruim_write_bcsms_table

DESCRIPTION
  Write BCSMS preference  to card

DEPENDENCIES


RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_write_bcsms_table
(
  nv_cmd_type         * nv_cmd_ptr,
  nv_stat_enum_type   * op_status
)
{
  byte         rec_num = 0;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_write_bcsms_table",0,0,0);
    return NV_RUIM_SUPPORTS_ITEM;
  }
  if( nvruim_bcsms_svc == FALSE )
  {
    MSG_HIGH("Card doesn't support BCSMS Extensions, falling back to NV",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;  
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  if( nv_cmd_ptr->item == NV_SMS_BC_SERVICE_TABLE_I )
  {
    rec_num = nv_cmd_ptr->data_ptr->sms_bc_service_table.index + 1;
    nvruim_bcsms_parm_ef_buf[0] = nv_cmd_ptr->data_ptr->sms_bc_service_table.selected;
    nvruim_bcsms_parm_ef_buf[1] = nv_cmd_ptr->data_ptr->sms_bc_service_table.priority;
  }
  else
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  nvruim_write_rec_item( nvruim_bcsms_parm_ef_buf,
                         NVRUIM_BCSMS_PARM_SIZE,
                         rec_num, /* record number */
                         UIM_ABSOLUTE,
                         UIM_CDMA_BCSMS_PARAMS );

  return nvruim_write_sw( nv_cmd_ptr, op_status, RUIM_NO_CACHE_VALUE );

} /* nvruim_write_bcsms_table() */

/*===========================================================================

FUNCTION NVRUIM_WRITE

DESCRIPTION
Check to see if the item is supported by the R-UIM.
If it is supported, write it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the write.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
The command data buffer is re-formatted.

===========================================================================*/
nv_ruim_support_status nvruim_write
(
  nv_cmd_type  *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status    /* status of the I/O operation*/
)
{
  nv_ruim_support_status nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Take the task pointer signal from nv_cmd_ptr and put into the uim_cmd */
  cmd.hdr.user_data = nv_cmd_ptr->sigs;   

#ifdef FEATURE_RUIM_CDMA_REFRESH
  /* always refresh the nvruim buffer if we have clear refresh flag */
  nvruim_process_refresh();
#endif /* FEATURE_RUIM_CDMA_REFRESH */

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

#if defined( FEATURE_RUIM_PHONEBOOK )
  /* Determine if item is a phonebook item */
  if((NV_DIAL_I != nv_cmd_ptr->item) && (NV_STACK_I != nv_cmd_ptr->item))
#endif /* FEATURE_RUIM_PHONEBOOK */
  {
    /* Determine if the CDMA DF is on the card */
    if((NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
#if defined ( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
    )
    {
      /* Do not try to access the R-UIM card.  Get item from NV */
      return NV_RUIM_ITEM_NOT_SUPPORTED;
    }
  }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

  /* Find-out if the NV item is an R-UIM item */
  /* If it is an R-UIM item:  */
  /*     Initialize the buffer with item pointer & size */
  /*     Reformat the data for R-UIM (reverse bytes & set flags)*/
  /*     Select the offset into the EF  */

  switch(nv_cmd_ptr->item)
  {
/*************HANDLE READ ONLY ITEMS FIRST**************************/
/*-------------ESN READ ONLY Items --------------------------------*/
    /* Allow Fallthrough */
    case NV_ESN_CHKSUM_I:
    case NV_ESN_I:
#ifdef FEATURE_UIM_EUIMID
    case NV_MEID_I:
#endif /* FEATURE_UIM_EUIMID */
      nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;
      break;

#ifdef FEATURE_UIM_SUPPORT_LBS
    case NV_GPS1_CAPABILITIES_I:
    case NV_GPS1_SEEDPOS_OPTION_I:
    case NV_GPS1_DYNAMIC_MODE_I:
    case NV_GPS1_XTRA_ENABLED_I:
    case NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:
    case NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I:
    case NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:
    case NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:
    case NV_GPS1_PDE_ADDRESS_I:
    case NV_GPS1_PDE_PORT_I:
    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
    case NV_CGPS_1X_PDE_SERVER_PORT_I:
    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
    case NV_CGPS_1X_MPC_SERVER_PORT_I:
    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:
    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:
    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I:
    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV6_I:            
	case NV_CGPS_1X_MPC_SERVER_ADDR_IPV6_I:
    case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:             
    case NV_CGPS_1X_MPC_SERVER_ADDR_URL_I:
      /*  Checking of Boolean nvruim_lbs_support, it indicate whether n24 is 
      is supported or not and Checking the RTRE Mode whether it is NV Mode or not */
      if((!nvruim_lbs_support) ||(NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control()))
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;
        break;
      }
      /* Return READ ONLY */
      nvruim_write_sprt_status = nvruim_check_uim_status_and_nam(nv_cmd_ptr,
                                                                 op_status);
      break;
#endif /* FEATURE_UIM_SUPPORT_LBS */

#ifdef FEATURE_UIM_SUPPORT_3GPD
#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
    case  NV_HYSTERISIS_ACTIVATION_TIMER_I:
    case  NV_ACTIVE_EPZID_TYPE_I:
    case  NV_TCP_KEEPALIVE_IDLE_TIME_I:
    case  NV_DSAT707_CTA_TIMER_I:
      /*  Checking of Boolean nvruim_3gpd_ext_support,
      it indicate whether n15 is supported or not   */
      if((!nvruim_data_3gpd_ext_support())||
         (UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support()))
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;  
        break;
      }
      /* Return READ ONLY */
      nvruim_write_sprt_status = nvruim_check_uim_status_and_nam(nv_cmd_ptr,
                                                                 op_status);
      break;
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */
/*-------------SIP 3GPD READ ONLY Items --------------------------------*/
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_PROFILE_I:
      /* If OMH 3GPD Extensions are not supported, allow them to fallback to 
      NV */
      if(!nvruim_data_3gpd_ext_support())
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;  
        break;
      }
      /* Allow Fallthrough */
    case NV_DS_SIP_NUM_VALID_PROFILES_I:
    case NV_DS_SIP_NAI_INFO_I:
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
    case NV_PPP_USER_ID_I:
  /* No need to write PPP NAI\Password Legacy NV items to 
     RUIM for OMH card*/
      if( (UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) ||
          (UIM_3GPD_MIP_NV_SIP_NV   == uim_3gpd_support()) || 
          ( (TRUE == nvruim_data_3gpd_ext_support()) && 
            (nv_cmd_ptr->item == NV_PPP_USER_ID_I)))
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;  
      }
      else
      {
        /* Return READ ONLY */
        nvruim_write_sprt_status = nvruim_check_uim_status_and_nam(nv_cmd_ptr, op_status);
      }
      break;

#ifdef FEATURE_UIM_3GPD_MOBILE_IP
/*-------------MIP 3GPD READ ONLY Items --------------------------------*/
    /* Allow Fallthrough */
    /* The following items are 3GPD Extended parameters for OMH */
    case NV_DS_MIP_QC_DRS_OPT_I:
    case NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I:
    case NV_DS_MIP_RRQ_IF_TFRK_I:
    case NV_TCP_GRACEFUL_DORMANT_CLOSE_I:
    /* If OMH 3GPD Extensions are not supported, allow them to fallback to 
      NV with the exception of 2002BIS, which should never fallback to NV when 
      using RUIM.  If 3GPD extensions not supported, return READ ONLY for 2002BIS*/
      if(!nvruim_data_3gpd_ext_support())
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;  
        break;
      }
    case NV_DS_MIP_2002BIS_MN_HA_AUTH_I:
    case NV_DS_MIP_NUM_PROF_I:
    case NV_DS_MIP_GEN_USER_PROF_I:
    case NV_DS_MIP_RETRIES_I:
    case NV_DS_MIP_RETRY_INT_I:
    case NV_DS_MIP_PRE_RE_RRQ_TIME_I:
    case NV_DS_MIP_ENABLE_PROF_I:
      /* The following case is not truly read-only, it is
      actually neither readable nor writable in an R-UIM
      implementation, Nevertheless, NV_READONLY_S is being 
      returned */
    case NV_DS_MIP_SS_USER_PROF_I:

      /* The following 2 cases relate to items not used
      with an R-UIM implementation.  They will return read-only,
      if a write is attempted, and not active if a read is attempted */
    case NV_DS_MIP_DMU_PKOID_I:
    case NV_DS_MIP_DMU_MN_AUTH_I:
      if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
      UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      else
      {
        /* Return READ ONLY */
        nvruim_write_sprt_status = nvruim_check_uim_status_and_nam(nv_cmd_ptr, op_status);
      }
      break;
#endif /* FEATURE_UIM_3GPD_MOBILE_IP */
#endif  /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
/*-------------HRPD AN READ ONLY Items --------------------------------*/
      /* These four HRPD related items MUST come after the 3GPD items and before the
      other Read only items, because they also require their own runtime check!!!
      The following items store the HRPD NAI (read-only for an R-UIM implementation
      NV_HDR_AN_AUTH_USER_ID_LONG_I is the preferred NV item, although some may still 
      be using the older item */
    /* Allow Fallthrough */
    case NV_HDR_AN_AUTH_USER_ID_LONG_I:
    case NV_HDR_AN_AUTH_NAI_I:
    case NV_HDR_AN_AUTH_PASSWD_LONG_I:
    case NV_HDR_AN_AUTH_PASSWORD_I:
      if(UIM_AN_HRPD_NO_SUPPORT == uim_an_hrpd_support())
      {
        nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      else
      {
        /* Return READ ONLY */
        nvruim_write_sprt_status = nvruim_check_uim_status_and_nam(nv_cmd_ptr, op_status);
      }
      break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

/*-------------OTHER READ ONLY Items --------------------------------*/
    /* Allow Fallthrough */
#ifndef FEATURE_NVRUIM_ADM_ACCESS
    case NV_MIN1_I:
    case NV_IMSI_T_S1_I:
    case NV_IMSI_T_S2_I:
    case NV_MIN2_I:
    case NV_IMSI_ADDR_NUM_I:
    case NV_IMSI_T_ADDR_NUM_I:
    case NV_IMSI_11_12_I:
    case NV_IMSI_T_11_12_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_T_MCC_I:
    case NV_ACCOLC_I:
#endif /* !FEATURE_NVRUIM_ADM_ACCESS */
      nvruim_write_sprt_status = nvruim_check_uim_status_and_nam(nv_cmd_ptr, op_status);
      break;
/************************END READ ONLY ITEMS***************************/
/************************Writable ITEMS********************************/
    /* Allow Fallthrough */
#ifdef FEATURE_NVRUIM_ADM_ACCESS
    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_IMSI_11_12_I:
    case NV_IMSI_ADDR_NUM_I:
    case NV_IMSI_MCC_I:
      nvruim_write_sprt_status = nvruim_write_imsi_m(nv_cmd_ptr, op_status);
      break;
      

    case NV_IMSI_T_S1_I:
    case NV_IMSI_T_S2_I:
    case NV_IMSI_T_11_12_I:
    case NV_IMSI_T_ADDR_NUM_I:
    case NV_IMSI_T_MCC_I:
       nvruim_write_sprt_status = nvruim_write_imsi_t(nv_cmd_ptr, op_status);
       break;

#ifndef FEATURE_UIM_JCDMA_RUIM_SUPPORT
#ifndef FEATURE_DISABLE_UIMPRL // Gemsea Add
    case NV_ROAMING_LIST_683_I:
      nvruim_write_sprt_status = nvruim_write_prl(nv_cmd_ptr, op_status);
      break;
#endif
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

    case NV_ACCOLC_I:
      nvruim_write_sprt_status = nvruim_write_accolc(nv_cmd_ptr, op_status);
      break;
      
#endif /* FEATURE_NVRUIM_ADM_ACCESS */
#ifdef FEATURE_UIM_RUIM_SUPPORT_SCI
    case NV_SLOT_CYCLE_INDEX_I:
      nvruim_write_sprt_status = nvruim_write_sci(nv_cmd_ptr, op_status);
      break;
#endif /* FEATURE_UIM_RUIM_SUPPORT_SCI */

    case NV_SID_NID_I:
    case NV_HOME_SID_NID_I:
      nvruim_write_sprt_status = nvruim_write_sid_nid(nv_cmd_ptr, op_status);
      break;
      

    case NV_DIR_NUMBER_I:
      nvruim_write_sprt_status = nvruim_write_msisdn(nv_cmd_ptr, op_status);
      break;
      

    case NV_DIR_NUMBER_PCS_I:
      nvruim_write_sprt_status = nvruim_write_dir_number_pcs(nv_cmd_ptr, 
                                                             op_status);
      break;
      

    case NV_COUNT_I:
      nvruim_write_sprt_status = nvruim_write_count(nv_cmd_ptr, op_status);
      break;
      

    case NV_ANALOG_FIRSTCHP_I:
      nvruim_write_sprt_status = nvruim_write_analog_firstchip(nv_cmd_ptr, 
                                                               op_status);
      break;
      

    case NV_ANALOG_HOME_SID_I:
      nvruim_write_sprt_status = nvruim_write_home_sid(nv_cmd_ptr, op_status);
      break;
      

    case NV_MOB_TERM_HOME_I:
    case NV_MOB_TERM_FOR_SID_I:
    case NV_MOB_TERM_FOR_NID_I:
      nvruim_write_sprt_status = nvruim_write_mob_term(nv_cmd_ptr, op_status);
      break;
      
#ifdef FEATURE_OTASP_OTAPA
    case NV_OTAPA_ENABLED_I:
#endif /* FEATURE_OTASP_OTAPA */
    case NV_SPC_CHANGE_ENABLED_I:
      nvruim_write_sprt_status = nvruim_write_spc_enable(nv_cmd_ptr, 
                                                         op_status);
      break;
      

#ifdef FEATURE_OTASP_OTAPA
    case NV_NAM_LOCK_I:
      nvruim_write_sprt_status = nvruim_write_nam_lock(nv_cmd_ptr, op_status);
      break;
      
#endif

#ifndef FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM
      /* Service Preferences */
    case  NV_PREF_MODE_I:
#endif
#ifdef FEATURE_SSPR_ENHANCEMENTS
    case  NV_SYSTEM_PREF_I:
#else /* FEATURE_SSPR_ENHANCEMENTS */
    case  NV_CDMA_PREF_SERV_I:
    case  NV_ANALOG_PREF_SERV_I:
#endif /* FEATURE_SSPR_ENHANCEMENTS */
      nvruim_write_sprt_status = nvruim_write_pref(nv_cmd_ptr, op_status);
      break;
      

      /* TMSI */
    case  NV_ASSIGNING_TMSI_ZONE_LEN_I:
    case  NV_ASSIGNING_TMSI_ZONE_I:
    case  NV_TMSI_CODE_I:
    case  NV_TMSI_EXP_I:
      nvruim_write_sprt_status = nvruim_write_tmsi(nv_cmd_ptr, op_status);
      break;
      

      /* Analog Location and Registration Indicators */
    case  NV_NXTREG_I:
    case  NV_LSTSID_I:
    case  NV_LOCAID_I:
    case  NV_PUREG_I:
      nvruim_write_sprt_status = nvruim_write_locn_regn_ind(nv_cmd_ptr, 
                                                            op_status);
      break;
      

      /* CDMA Zone-Based Registration Indicators */
    case  NV_ZONE_LIST_I:
      nvruim_write_sprt_status = nvruim_write_zone_list(nv_cmd_ptr, op_status);
      break;
      

      /* CDMA System/Network Registration Indicators */
    case  NV_SID_NID_LIST_I:
      nvruim_write_sprt_status = nvruim_write_sid_nid_list(nv_cmd_ptr, 
                                                           op_status);
      break;
      

      /* CDMA Distance-Based Registration Indicators */
    case  NV_DIST_REG_I:
      nvruim_write_sprt_status = nvruim_write_dist_reg(nv_cmd_ptr, op_status);
      break;
      

#ifdef FEATURE_UIM_SUPPORT_3GPD
      /* 3GPD Operational Mode Parameters */
    case NV_DS_QCMIP_I:
      nvruim_write_sprt_status = nvruim_write_ds_qcmip(nv_cmd_ptr, op_status);
      break;
      

      /* Mobile IP Status Parameters */
    case NV_DS_MIP_ACTIVE_PROF_I:
      nvruim_write_sprt_status = nvruim_write_ds_mip_active_prof(nv_cmd_ptr, op_status);
      break;
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      /* Simple IP Status Parameters */
    case NV_DS_SIP_ACTIVE_PROFILE_INDEX_I:
      nvruim_write_sprt_status = nvruim_write_ds_sip_active_profile(nv_cmd_ptr, op_status);
      break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      /* Simple IP PAP SS Parameters */
      /* Please note that the following 2 NV items are used to refer to a generic
      password used for both PAP and CHAP.  In R-UIM, this item refers
      only to the PAP password.  The CHAP password is held internall by the 
      card and is not accessible to the ME.  It is used indirectly only when
      a CHAP Challenge is executed with the corresponding NAI-Entry-Index.
      The NV_PPP_PASSWORD_I is the legacy NV item used by a single SIP Profile
      scenario, and mapps to the preferred SS on the card.  The 
      NV_DS_SIP_PPP_SS_INFO_I item refers to the multiple SIP Profile solution*/

    case NV_PPP_PASSWORD_I:
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_PPP_SS_INFO_I:
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      nvruim_write_sprt_status = nvruim_write_ppp_passwd(nv_cmd_ptr, op_status);
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    case NV_SMS_BC_USER_PREF_I:
      nvruim_write_sprt_status = nvruim_write_bcsms_pref( nv_cmd_ptr, op_status );
      break;

    case NV_SMS_BC_SERVICE_TABLE_I:
      nvruim_write_sprt_status = nvruim_write_bcsms_table( nv_cmd_ptr, op_status );
      break;

    default:
      /* Item is not supported by R-UIM */
      nvruim_write_sprt_status = NV_RUIM_ITEM_NOT_SUPPORTED;
      break;
  }
  return nvruim_write_sprt_status;
} /* nvruim_write */

/*===========================================================================
FUNCTION NVRUIM_CHECK_MNANDATORY

DESCRIPTION
Check to see if the the namdatory files are present or not.

DEPENDENCIES
Non-reentrant

RETURN VALUE
None


SIDE EFFECTS
None

===========================================================================*/
static nv_ruim_support_status nvruim_check_mandatory(
  nv_cmd_type  *nv_cmd_ptr
)
{
  switch(nv_cmd_ptr->item)
  {
    case NV_ESN_CHKSUM_I:
    case NV_ESN_I:
    case NV_COUNT_I:
    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_IMSI_11_12_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_ADDR_NUM_I:
    case NV_IMSI_T_S1_I:
    case NV_IMSI_T_S2_I:
    case NV_IMSI_T_MCC_I:
    case NV_IMSI_T_11_12_I:
    case NV_IMSI_T_ADDR_NUM_I:
    case NV_SID_NID_I:
    case NV_ANALOG_FIRSTCHP_I:
    case NV_ANALOG_HOME_SID_I:
    case NV_ACCOLC_I:
    case NV_MOB_TERM_HOME_I   :
    case NV_MOB_TERM_FOR_SID_I:
    case NV_MOB_TERM_FOR_NID_I:
    case NV_HOME_SID_NID_I:
#ifndef FEATURE_UIM_JCDMA_RUIM_SUPPORT
    /* JCDMA: this is a NV item */  
    case NV_ROAMING_LIST_683_I:
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

    case NV_OTAPA_ENABLED_I:
    case NV_SPC_CHANGE_ENABLED_I:
    case NV_NAM_LOCK_I:
    case NV_PREF_MODE_I:
    case NV_SYSTEM_PREF_I:
    case NV_CDMA_PREF_SERV_I:
    case NV_ANALOG_PREF_SERV_I:
    case NV_ASSIGNING_TMSI_ZONE_LEN_I:
    case NV_ASSIGNING_TMSI_ZONE_I:
    case NV_TMSI_CODE_I:
    case NV_TMSI_EXP_I:
    case NV_NXTREG_I:
    case NV_LSTSID_I:
    case NV_LOCAID_I:
    case NV_PUREG_I:
    case NV_ZONE_LIST_I:
    case NV_SID_NID_LIST_I:
#ifdef FEATURE_UIM_EUIMID
    case NV_MEID_I:
#endif /* FEATURE_UIM_EUIMID */

      if((nvruim_sw1 == SW1_REFERENCE) && (nvruim_sw2 == SW2_NOT_FOUND))
      {
        /* File not found */
        nvruim_dir_present = (nvruim_dir_present & ~NVRUIM_CDMA_DF_PRESENT);
        return NV_RUIM_ITEM_NOT_SUPPORTED;
      }
      else
        break;

    default:  
      break;
  }
  return NV_RUIM_SUPPORTS_ITEM;
}

/*===========================================================================
FUNCTION NVRUIM_READ_SW

DESCRIPTION
Parses the SW from UIM

DEPENDENCIES
Non-reentrant

RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_sw(
  nv_stat_enum_type *op_status,   /* status of the I/O operation*/
  uint32             cache_bit
)
{
  /* Translate the R-UIM status into an NV status     */
  /* Data processing may alter this status */

  *op_status = NV_NOTACTIVE_S; /* Default status is "not active" to
  support upper layer fault handling  */
  switch(nvruim_ruim_status_report)
  {
    case UIM_PASS:
      *op_status = NV_DONE_S;     /* Request completed okay     */
      nvruim_cache_control |= cache_bit; /* If read was okay, set cache bit   */
      break;

      /* Other statuses:*/
      /* NV_BUSY_SRequest is queued*/
      /* NV_BADCMD_S    Unrecognizable command field  */
      /* NV_FULL_SThe NVM is full */
      /* NV_FAIL_S,     Command failed, reason other than NVM was full*/
      /* NV_BADPARM_S   Bad parameter in command block*/
      /* NV_READONLY_S  Parameter is read only*/
      /* NV_BADTG_S     Item not valid for Target     */
      /* NV_NOMEM_S     free memory exhausted */
      /* NV_NOTALLOC_S  address is not a valid allocation */

    case UIM_FAIL:
#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS ) && \
!defined( FEATURE_UIM_RUN_TIME_ENABLE )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS && !FEATURE_UIM_RUN_TIME_ENABLE */
      break;

  }
  return NV_RUIM_SUPPORTS_ITEM;
}


/*===========================================================================
FUNCTION NVRUIM_READ_AND_CHECK_RSP

DESCRIPTION
Read the Rsp from NVRUIM

DEPENDENCIES
Non-reentrant

RETURN VALUE
 nv_ruim_support_status

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_and_check_rsp(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  uint32             cache_bit,
  nv_stat_enum_type *op_status,   /* status of the I/O operation*/
  boolean           *ef_data_refreshed
)
{
  nv_ruim_support_status support_status;

  cmd.access_uim.access = UIM_READ_F; /* "Read" command */

  /* Determine if this EF needs to be read or if there is a valid cached
  value */
  if((nvruim_cache_control & cache_bit) == 0)
  {
    (void) nvruim_access( &cmd );  /* Access the R-UIM     */
#ifdef FEATURE_UIM_SUPPORT_3GPD
    /* The following is used to indicate that the 3GPD bit pointer arrays and
    auxiliary caches no longer contain valid data.*/ 
    *ef_data_refreshed = TRUE;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  }
  else
  { /* The value can be read from the cache buffer */
    nvruim_ruim_status_report = UIM_PASS;

    /* Read from cache, initialize status word */
    nvruim_sw1 = SW1_NORMAL_END;
    nvruim_sw2 = SW2_NORMAL_END;

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /* The following is used to indicate that the 3GPD bit pointer arrays and
    auxiliary caches still contain valid data (even if a structure contains the
    wrong profile.  This variable has been initalized to TRUE */ 
    *ef_data_refreshed = FALSE;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  } /* end if - the value is not cached and needs to be read from the RUIM */
  support_status = nvruim_check_mandatory(nv_cmd_ptr);

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
    return support_status;

  return nvruim_read_sw(op_status,cache_bit); 
}

/*===========================================================================
FUNCTION NVRUIM_READ_ESN

DESCRIPTION
Check to see if the ESN is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  Nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_esn(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  /* If the usage indicator is NV_RUIM_USE_MEID then the pESN from 
     the handset should be returned not the pUIMID value from the card */
  MSG_FATAL("nvruim_read_esn %d 0x%x", nvruim_esn_usage,tmc_get_stored_esn_me(),0);
#ifdef FEATURE_OEMOMH
  if(nvruim_esn_usage == NV_RUIM_USE_MEID || 
     nvruim_esn_usage == NV_RUIM_USE_SF_EUIMID_ESN)
#else
  if(nvruim_esn_usage == NV_RUIM_USE_ESN ||
     nvruim_esn_usage == NV_RUIM_USE_MEID || 
     nvruim_esn_usage == NV_RUIM_USE_SF_EUIMID_ESN)
#endif
  {
#ifndef FEATURE_UIM_JCDMA_RUIM_SUPPORT
    return NV_RUIM_ITEM_NOT_SUPPORTED;
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
  }
  else if(nvruim_esn_usage == NV_RUIM_ZERO_ESN)
  {
    if(nv_cmd_ptr->item == NV_ESN_CHKSUM_I)
    {
      nv_cmd_ptr->data_ptr->esn_chksum.chksum = 
       (dword) NV_VALID_ESN_CHKSUM;
    }
    else
      nv_cmd_ptr->data_ptr->esn.esn = 0;
      *op_status = NV_DONE_S;
      return NV_RUIM_SUPPORTS_ITEM;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* Setup the command structure to read the RUIM_ID EF from the RUIM */
  cmd.access_uim.num_bytes = NVRUIM_SIZE(esn.esn);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_ruim_id_cache_buf);
#ifdef FEATURE_OEMOMH
  if(nvruim_esn_usage == NV_RUIM_USE_ESN)
  {
    cmd.access_uim.item    = UIM_CDMA_ESN_ME;
  }
  else
  {
    cmd.access_uim.item    = UIM_CDMA_RUIM_ID;
  }
#else
  cmd.access_uim.item      = UIM_CDMA_RUIM_ID;  
#endif
  cmd.access_uim.offset    = 1; /* EF offset  */
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_RUIM_ID_CACHE_BIT,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  if(nv_cmd_ptr->item == NV_ESN_I)
  {
    if(*op_status == NV_NOTACTIVE_S)
    {
    #ifdef FEATURE_RUIM_ALLOW_ESN_FAIL
      /* The builder wants to know when the ESN read fails. */
      *op_status = NV_FAIL_S;
    #elif  defined(FEATURE_RUIM_ZERO_ESN_FAIL)
      /* The builder wants a zero ESN when the read fails. */
      nv_cmd_ptr->data_ptr->esn.esn = 0;
      *op_status = NV_DONE_S;
    #else
      /* The builder wants the ESN out of EEPROM. */
      return NV_RUIM_ITEM_NOT_SUPPORTED;
    #endif /* ESN feature selection on failed RUIM access */
    } /* end if - the read from the RUIM has failed. */
  
#ifdef FEATURE_NVRUIM_DEBUG_FORCE_ESN
    /* Debug code to override the ESN */
    nv_cmd_ptr->data_ptr->esn.esn = 0xD75A96ECL;
    *op_status = NV_DONE_S;     /* Request completed okay     */
#else
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the ESN from the cache buffer */
      nv_cmd_ptr->data_ptr->esn.esn = nvruim_ruim_id_cache_buf.esn;
    } /* end if - the read was successful. */
#endif
  }
  else if(nv_cmd_ptr->item == NV_ESN_CHKSUM_I)
  {
    nv_cmd_ptr->data_ptr->esn_chksum.chksum = (dword)NV_VALID_ESN_CHKSUM;
  }
  
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;
}

/*===========================================================================

FUNCTION NVRUIM_READ_COUNT

DESCRIPTION
Check to see if the CALL Count is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_count(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  /* Use the MSISDN buffer */
  cmd.access_uim.data_ptr  = msisdn_buffer;
  cmd.access_uim.num_bytes = NVRUIM_SIZE(count.count);
  cmd.access_uim.item      = UIM_CDMA_CC;  
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;   /* Record Mode*/
  cmd.access_uim.offset    = 0;    /* Not used*/
  support_status           = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  /* Reformat the data only if the read was successful. */
  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->count.count =
       msisdn_buffer[1] & 0x3F;  /* 6 LSB bits */
  } 
  nv_cmd_ptr->status = *op_status;
  return support_status;
}

/*===========================================================================

FUNCTION NVRUIM_READ_IMSI_M

DESCRIPTION
Check to see if IMSI_M is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_imsi_m(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status    /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  /* Only process the NAM parameters for the RUIM
     This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->min1.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* Setup the command structure to read the IMSI_M EF from the RUIM */
  cmd.access_uim.num_bytes = sizeof(nvruim_imsi_type);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_imsi_m_cache_buf);
  cmd.access_uim.item      = UIM_CDMA_IMSI_M;  
  cmd.access_uim.offset    = 0;    /* EF offset*/

  support_status= nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_IMSI_M_CACHE_BIT,op_status,&dummy);

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }

  /* Note whether or not the IMSI was programmed  */
  if(nv_cmd_ptr->item == NV_IMSI_ADDR_NUM_I)
  {
    if((nvruim_imsi_m_cache_buf.imsi_addr_num & NVRUIM_IMSI_PROGRAMMED) == 0)
      *op_status = NV_NOTACTIVE_S;

    #ifdef FEATURE_NVRUIM_DEBUG_FORCE_ADDR_NUM
      /* Debug code to override the ADDR_NUM */
      nv_cmd_ptr->data_ptr->imsi_addr_num.num = 6;
      *op_status = NV_DONE_S;     /* Request completed okay     */
      break;
    #else
      /* Reformat the data only if the read was successful. */
      if(NV_DONE_S == *op_status)
      {
        if(nvruim_imsi_m_cache_buf.imsi_class == 0)
          nv_cmd_ptr->data_ptr->imsi_addr_num.num = 0xFF;
        else
          nv_cmd_ptr->data_ptr->imsi_addr_num.num =
        nvruim_imsi_m_cache_buf.imsi_addr_num & NVRUIM_IMSI_ADDR_NUM;
      } /* end if - the read was successful. */
    #endif
  }
  else if(nv_cmd_ptr->item == NV_IMSI_11_12_I)
  {
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->imsi_11_12.imsi_11_12 =
        nvruim_imsi_m_cache_buf.imsi_11_12;
    } /* end if - the read was successful. */
  }
  else if(nv_cmd_ptr->item == NV_MIN1_I){
  #ifdef FEATURE_NVRUIM_DEBUG_FORCE_MIN1
    /* Debug code to override the MIN1 */
    nv_cmd_ptr->data_ptr-> min1.min1[NV_CDMA_MIN_INDEX] = 0x00792971;
  #else
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->min1.min1 [NV_CDMA_MIN_INDEX] =
              (nvruim_imsi_m_cache_buf.imsi_s1[2] << 16) |
              (nvruim_imsi_m_cache_buf.imsi_s1[1] << 8)  |
              (nvruim_imsi_m_cache_buf.imsi_s1[0]);
    } /* end if - the read was successful. */
#endif
    /* Copy the contents of CDMA MIN1 to Amps MIN1 */
    nv_cmd_ptr->data_ptr->min1.min1[0] =
    nv_cmd_ptr->data_ptr-> min1.min1[NV_CDMA_MIN_INDEX] ;
  }
  else if(nv_cmd_ptr->item == NV_MIN2_I)
  {
  #ifdef FEATURE_NVRUIM_DEBUG_FORCE_MIN2
     /* Debug code to override the MIN2 */
     nv_cmd_ptr->data_ptr->min2.min2[NV_CDMA_MIN_INDEX] = 0x02EB;
  #else 
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->min2.min2 [NV_CDMA_MIN_INDEX] =
            (nvruim_imsi_m_cache_buf.imsi_s2[1] << 8) |
            nvruim_imsi_m_cache_buf.imsi_s2[0];
    } /* end if - the read was successful. */
  #endif
  /* Copy the contents of CDMA MIN2 to Amps MIN2 */
    nv_cmd_ptr->data_ptr->min2.min2[0] =
    nv_cmd_ptr->data_ptr-> min2.min2[NV_CDMA_MIN_INDEX] ;
  }
  else if(nv_cmd_ptr->item == NV_IMSI_MCC_I)
  {
  #ifdef FEATURE_NVRUIM_DEBUG_FORCE_MCC
    /* Debug code to override the MCC */
    nv_cmd_ptr->data_ptr->imsi_mcc.imsi_mcc = 209;
    *op_status = NV_DONE_S;     /* Request completed okay     */
  #else
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->imsi_mcc.imsi_mcc =
          (nvruim_imsi_m_cache_buf.imsi_mcc[1] << 8) |
          (nvruim_imsi_m_cache_buf.imsi_mcc[0]);
    } /* end if - the read was successful. */
  #endif
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}

/*===========================================================================

FUNCTION NVRUIM_READ_IMSI_T

DESCRIPTION
Check to see if IMSI_T is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_imsi_t(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->imsi_t_s1.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* Setup the command structure to read the IMSI_M EF from the RUIM */
  cmd.access_uim.num_bytes = sizeof(nvruim_imsi_type);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_imsi_t_cache_buf);
  cmd.access_uim.item= UIM_CDMA_IMSI_T;  
  cmd.access_uim.offset    = 0; /* EF offset  */
  support_status= nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_IMSI_T_CACHE_BIT,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  /* Note whether or not the IMSI was programmed  */
  
  if(nv_cmd_ptr->item == NV_IMSI_T_ADDR_NUM_I)
  {
    if((nvruim_imsi_t_cache_buf.imsi_addr_num & NVRUIM_IMSI_PROGRAMMED) == 0)
      *op_status = NV_NOTACTIVE_S;
    
  #ifdef FEATURE_NVRUIM_DEBUG_FORCE_ADDR_NUM
    /* Debug code to override the ADDR_NUM */
    
    nv_cmd_ptr->data_ptr->imsi_t_addr_num.num = 6;
    
    *op_status = NV_DONE_S;     /* Request completed okay     */
  #else
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      if(nvruim_imsi_t_cache_buf.imsi_class == 0)
        nv_cmd_ptr->data_ptr->imsi_t_addr_num.num = 0xFF;
      else
        nv_cmd_ptr->data_ptr->imsi_t_addr_num.num =
      nvruim_imsi_t_cache_buf.imsi_addr_num & NVRUIM_IMSI_ADDR_NUM;
    } /* end if - the read was successful. */
  #endif
  }
  
  if(nv_cmd_ptr->item == NV_IMSI_T_S2_I)
  {
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->imsi_t_s2.min2 [NV_CDMA_MIN_INDEX] =
              (nvruim_imsi_t_cache_buf.imsi_s2[1] << 8) |
              (nvruim_imsi_t_cache_buf.imsi_s2[0]);
    } /* end if - the read was successful. */
  }
  
  else if(nv_cmd_ptr->item == NV_IMSI_T_S1_I)
  {
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->imsi_t_s1.min1 [NV_CDMA_MIN_INDEX] =
             (nvruim_imsi_t_cache_buf.imsi_s1[2] << 16) |
             (nvruim_imsi_t_cache_buf.imsi_s1[1] << 8) |
             (nvruim_imsi_t_cache_buf.imsi_s1[0]);
    } /* end if - the read was successful. */
  }
  
  else if(nv_cmd_ptr->item == NV_IMSI_T_11_12_I)
  {
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->imsi_t_11_12.imsi_11_12 =
      nvruim_imsi_t_cache_buf.imsi_11_12;
    } /* end if - the read was successful. */
  }
  
  else if(nv_cmd_ptr->item == NV_IMSI_T_MCC_I)
  {
  #ifdef FEATURE_NVRUIM_DEBUG_FORCE_MCC
    /* Debug code to override the MCC */
      
    nv_cmd_ptr->data_ptr->imsi_t_mcc.imsi_mcc = 209;
    *op_status = NV_DONE_S;     /* Request completed okay     */
  #else
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the item from the cache buffer. */
      nv_cmd_ptr->data_ptr->imsi_t_mcc.imsi_mcc =
             (nvruim_imsi_t_cache_buf.imsi_mcc[1] << 8) |
             (nvruim_imsi_t_cache_buf.imsi_mcc[0]);
    } /* end if - the read was successful. */
  #endif
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
  
}

/*===========================================================================

FUNCTION NVRUIM_READ_SID_NID

DESCRIPTION
Check to see if SID_NID is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_sid_nid(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  word num_of_record; /* Number of records for a liner fixed EF */
  word i =0;
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->sid_nid.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes = SID_NID_REC_SIZE;
  cmd.access_uim.data_ptr  = (byte *) &temp_sid_nid_buffer;
  cmd.access_uim.offset    = 1; /* The first record */
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.item      = UIM_CDMA_HOME_SID_NID;  
  
  support_status= nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM) 
  {
    return support_status;
  }
  
  /* First pair */
  nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].sid =
    temp_sid_nid_buffer.sid;
  nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].nid =
    temp_sid_nid_buffer.nid;
  
  /* Check number of records */
  if(cmd.access_uim.num_records_rsp > NV_MAX_SID_NID)
    num_of_record = NV_MAX_SID_NID; /* RAM can only handle this many */
  else
    num_of_record = cmd.access_uim.num_records_rsp;
  
  /* Read the rest of records */
  for(i = 1; (i < num_of_record) && (temp_sid_nid_buffer.sid != 0); i++)
  {
    cmd.access_uim.rec_mode  = UIM_ABSOLUTE;

    nvruim_read_item((byte *) &(temp_sid_nid_buffer),
                     SID_NID_REC_SIZE,
                     i+1 /* Record number */,
                     UIM_CDMA_HOME_SID_NID);
  
    if(nvruim_ruim_status_report == UIM_PASS)
    {
      /*lint -e661*/
      nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][i].sid =
        temp_sid_nid_buffer.sid;
      nv_cmd_ptr->data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][i].nid =
        temp_sid_nid_buffer.nid;
      /*lint -restore */
    }
    else
      break;   /* Break at any bad read */
  }
  nvruim_read_sw(op_status,RUIM_NO_CACHE_VALUE);

  /* Report UIM_PASS since the first read is good */
  nvruim_ruim_status_report = UIM_PASS;
  nv_cmd_ptr->status= *op_status;
  return support_status;
  
}

/*===========================================================================

FUNCTION NVRUIM_READ_MSISDN

DESCRIPTION
Check to see if MSISDN is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_msisdn(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  byte temp_msisdn_buffer[NV_DIR_NUMB_SIZ];/* buffer to parse the MSISDN */
  byte num_bcd_digits; /* length of BCD number in MSISDN */
  word i=0, j = 0;
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->dir_number.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.data_ptr  = msisdn_buffer;
  cmd.access_uim.num_bytes = sizeof( msisdn_buffer );
  cmd.access_uim.offset    = 1; /* EF offset  */
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.item      = UIM_CDMA_MSISDN;  
  
  support_status= nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)             
  {
    return support_status;
  }
  
  if(nv_cmd_ptr->item == NV_DIR_NUMBER_I)
  {
  #ifdef FEATURE_NVRUIM_DEBUG_FORCE_DIR_NUM
    {
      static byte dir_number[NV_DIR_NUMB_SIZ + 1] = "8585551212";
  
      memcpy( (byte*)(nv_cmd_ptr->data_ptr->dir_number.dir_number),
      dir_number, NV_DIR_NUMB_SIZ );
  
      *op_status = NV_DONE_S;     /* Request completed okay   */
    }
  #else
    /* Copy the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the MDN from the MSISDN
      *
      * The buffer index is based on the size of the MSISDN EF.
      * Copy no more than the lesser of the size of the DN or
      * the destination buffer.
      */
      /* Initialize the temp buffer to character '0' */
      for(i = 0; i < NV_DIR_NUMB_SIZ; i++)
        temp_msisdn_buffer[i] = 0x30;
    
      /* this is the number of BCD digits in the MDN that are returned */
      num_bcd_digits = msisdn_buffer[ NVRUIM_MSISDN_ND_INDEX ] & 0x0F;
    
      /* Determine which (the number of digits or DIR NUM buffer size) is
      larger */
      if(NV_DIR_NUMB_SIZ > num_bcd_digits)
      {
        /* Determine number of digits that are not returned from RUIM */
        j = NV_DIR_NUMB_SIZ - num_bcd_digits;
        /* The MSISDN digit index starts at the first digit. */
        i = 0;
      }
      else /* num_bcd_digits >= NV_DIR_NUMB_SIZ */
      {
        /* The DIR NUM index starts at the first byte. */
        j = 0;
        /* The MSISDN index starts passed the MSnibbles to fill out the DIR
        NUM */
        i = (num_bcd_digits - NV_DIR_NUMB_SIZ);
      } /* end if - the number of bcd digits is less than the DIR NUMBER
      size. */
    
      /* Copy the digits from the MSISDN to the temporary DIR NUM buffer.
         Do not exceed either buffer. */
      for(; (i < num_bcd_digits) && (j < NV_DIR_NUMB_SIZ); i++, j++)
      {
        /* Retrieve the digit from the byte - based on whether the index is
        even or odd */
        if(i & 0x1)
        {
          /* Retrieve the MSNibble from the proper byte of the MSISDN */
          temp_msisdn_buffer[j] |=
            ((msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] & 0xF0) >> 4);
        }
        else /* The digit index is even */
        {
          /* Retrieve the LSnibble from the proper byte of the MSISDN */
          temp_msisdn_buffer[j] |=
            (msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] & 0x0F);
        } /* end if - the index into the MSISDN buffer is odd. */
    
        /* Convert the '0' of the MDN. */
        if(temp_msisdn_buffer[j] == 0x3A)
        {
          temp_msisdn_buffer[j] = '0';
        } /* end if - digit is '0'. */
        /* Convert the '*' of the MDN. */
        if(temp_msisdn_buffer[j] == 0x3B)
        {
           temp_msisdn_buffer[j] = '*';
        } /* end if - digit is '*'. */
        /* Convert the '#' of the MDN. */
        if(temp_msisdn_buffer[j] == 0x3C)
        {
          temp_msisdn_buffer[j] = '#';
        } /* end if - digit is '#'. */
      } /* end for - process all of the digits */
    
      /* copy the contents of the temp msisdn buffer */
      memcpy( (byte *) (nv_cmd_ptr->data_ptr->dir_number.dir_number),
      temp_msisdn_buffer, NV_DIR_NUMB_SIZ);
    } /* end if - the read was successful. */
#endif
  }
  
  else if(nv_cmd_ptr->item == NV_DIR_NUMBER_PCS_I)
  {
    #ifdef FEATURE_NVRUIM_DEBUG_FORCE_DIR_NUM
    {
      static byte pcs_dir_number[NV_DIR_NUMB_PCS_SIZ] ={ 8, 5, 8, 5, 5, 5, 1, 2, 1, 2, 0xf, 0xf, 0xf, 0xf, 0xf};
      
      /* Copy the fixed array to the buffer */
      memcpy( (byte*)(nv_cmd_ptr->data_ptr->mob_dir_number.digitn),
      pcs_dir_number, NV_DIR_NUMB_PCS_SIZ );
      
      /* Fill out the number of digits. */
      nv_cmd_ptr->data_ptr->mob_dir_number.n_digits = NV_DIR_NUMB_SIZ;
      *op_status = NV_DONE_S;     /* Request completed okay   */
    }
    break;
  #else
  
    /* Copy the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* -----------------------------------------------------
      * Copy the MDN from the MSISDN
      *
      * The buffer index is based on the size of the MSISDN EF.
      * Copy no more than the lesser of the size of the DN or
      * the destination buffer.
      * Initialize the temp buffer to character pad digits 
      --------------------------------------------------------*/
      for(i = 0; i < NV_DIR_NUMB_PCS_SIZ; i++)
        nv_cmd_ptr->data_ptr->mob_dir_number.digitn[i] = 0xf;
      
      /* this is the number of BCD digits in the MDN that are returned */
      num_bcd_digits = msisdn_buffer[ NVRUIM_MSISDN_ND_INDEX ] & 0x0F;
      
      /* Copy the digits from the MSISDN to the DIR NUM buffer.
      Do not exceed either buffer. */
      for(i = 0; (i < num_bcd_digits); i++)
      {
        /* Retrieve the digit from the byte based on whether the index is
           even or odd */
        if(i & 0x1)
        {
          /* Retrieve the MSNibble from the proper byte of the MSISDN */
          nv_cmd_ptr->data_ptr->mob_dir_number.digitn[i] =
            ((msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] & 0xF0) >> 4);
        }
        else /* The digit index is even */
        {
          /* Retrieve the LSnibble from the proper byte of the MSISDN */
          nv_cmd_ptr->data_ptr->mob_dir_number.digitn[i] =
            (msisdn_buffer[ (i/2) + NVRUIM_MSISDN_MDN_INDEX ] & 0x0F);
        } /* end if - index is odd */
      } /* end for - copy all the digits to the DIR NUM buffer */
    
      /* Give the number of digits read from the card. */
      nv_cmd_ptr->data_ptr->mob_dir_number.n_digits = num_bcd_digits;
    } /* end if - the read was successful. */
  #endif
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}

/*===========================================================================
FUNCTION NVRUIM_READ_ANALOG_CHAN_PREF

DESCRIPTION
Check to see ANALOG CHAN PREF item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_analog_chan_pref(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  /* Only process the NAM parameters for the RUIM
     This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->analog_firstchp.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = NVRUIM_SIZE(analog_firstchp.channel);
  cmd.access_uim.data_ptr  = 
      (byte *) &(nv_cmd_ptr->data_ptr->analog_firstchp.channel);
  cmd.access_uim.offset    = (word) FPOS(nvruim_analog_ch_pref, analog_firstchp);
  cmd.access_uim.item      = UIM_CDMA_ANALOG_CHAN_PREF;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  nv_cmd_ptr->status = *op_status;
  return support_status;
}

/*===========================================================================

FUNCTION NVRUIM_READ_HOME_SID

DESCRIPTION
Check to see if HOME SID is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_home_sid(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->analog_home_sid.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  
  cmd.access_uim.num_bytes = NVRUIM_SIZE(analog_home_sid.sid);
  cmd.access_uim.data_ptr  =
    (byte *) &(nv_cmd_ptr->data_ptr->analog_home_sid.sid);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item= UIM_CDMA_ANALOG_HOME_SID;
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  nv_cmd_ptr->status = *op_status;
  return support_status;
}

/*===========================================================================

FUNCTION NVRUIM_READ_ACCOLC

DESCRIPTION
Check to see if ACOLC is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_accolc(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  
  /* Only process the NAM parameters for the RUIM
     This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->accolc.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  =
  #ifndef FEATURE_NVRUIM_OLD_NV_ACCOLC_TYPE
    (byte *) &(nv_cmd_ptr->data_ptr->accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX]);
  #else
    (byte *) &(nv_cmd_ptr->data_ptr->accolc.class[NV_CDMA_MIN_INDEX]);
  #endif
  
  cmd.access_uim.offset = 0;
  cmd.access_uim.item   = UIM_CDMA_ACCOLC;
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  #ifndef FEATURE_NVRUIM_OLD_NV_ACCOLC_TYPE
  /* Copy to ACCOLC */
  nv_cmd_ptr->data_ptr->accolc.ACCOLCpClass[NV_ANALOG_MIN_INDEX] =
  nv_cmd_ptr->data_ptr->accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];
  #else
  nv_cmd_ptr->data_ptr->accolc.class[NV_ANALOG_MIN_INDEX] =
  nv_cmd_ptr->data_ptr->accolc.class[NV_CDMA_MIN_INDEX];
  #endif
  
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;
}

#ifdef FEATURE_UIM_RUIM_SUPPORT_SCI 
/*===========================================================================

FUNCTION NVRUIM_READ_SCI

DESCRIPTION
Check to see if SCI is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_sci(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status    /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  =
  (byte *) &(nv_cmd_ptr->data_ptr->slot_cycle_index);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_SCI;  

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  nv_cmd_ptr->data_ptr->slot_cycle_index &= 0x07;
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}
#endif /*#ifdef FEATURE_UIM_RUIM_SUPPORT_SCI */

/*===========================================================================

FUNCTION NVRUIM_READ_CALL_TERM

DESCRIPTION
Check to see if CALL TERM is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_call_term(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)

{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  
  /* Only process the NAM parameters for the RUIM
       This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->mob_term_home.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  cmd.access_uim.num_bytes = 1;
  /* Re-format it later */
  cmd.access_uim.data_ptr  = &nvruim_call_term_mode_pref_cache_buf;
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_CALL_TERM_MODE_PREF;  
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_CALL_TERM_MODE_PREF_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  if(nv_cmd_ptr->item == NV_MOB_TERM_HOME_I)
  {
    if(NV_DONE_S == *op_status)
    {
      if(nvruim_call_term_mode_pref_cache_buf & 0x04) /* Bit 2 */
      {
        nv_cmd_ptr->data_ptr->mob_term_home.enabled[NV_CDMA_MIN_INDEX] 
          = TRUE;
      }
      else
      {
        nv_cmd_ptr->data_ptr->mob_term_home.enabled[NV_CDMA_MIN_INDEX]
          = FALSE;
      }
      nv_cmd_ptr->data_ptr->mob_term_home.enabled[NV_ANALOG_MIN_INDEX] =
        nv_cmd_ptr->data_ptr->mob_term_home.enabled[NV_CDMA_MIN_INDEX];
    } /* end if - the read was successful. */
  }
  
  else if(nv_cmd_ptr->item == NV_MOB_TERM_FOR_SID_I)
  {
    if(NV_DONE_S == *op_status)
    {
      if(nvruim_call_term_mode_pref_cache_buf & 0x02) /* Bit 1 */
      {
        nv_cmd_ptr->data_ptr->mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX]
          = TRUE;
      }
      else
      {
        nv_cmd_ptr->data_ptr->mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX]
          = FALSE;
      }
      nv_cmd_ptr->data_ptr->mob_term_for_sid.enabled[NV_ANALOG_MIN_INDEX] =
        nv_cmd_ptr->data_ptr->mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX];
    } /* end if - the read was successful. */
  }
  
  else if(nv_cmd_ptr->item == NV_MOB_TERM_FOR_NID_I)
  {
    if(NV_DONE_S == *op_status)
    {
      if(nvruim_call_term_mode_pref_cache_buf & 0x01) /* Bit 0 */
      {
        nv_cmd_ptr->data_ptr->mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX]
            = TRUE;
      }
      else
      {
        nv_cmd_ptr->data_ptr->mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX]
          = FALSE;
      }
      nv_cmd_ptr->data_ptr->mob_term_for_nid.enabled[NV_ANALOG_MIN_INDEX] =
        nv_cmd_ptr->data_ptr->mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX];
    } /* end if - the read was successful. */
  }
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;

}

/*===========================================================================

FUNCTION NVRUIM_READ_HOME_SID_NID

DESCRIPTION
Check to see if HOME_SID_NID is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_home_sid_nid(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean                dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  word                   num_of_record; /* Number of records for a liner fixed EF */
  uint32                 i; 
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->home_sid_nid.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes = SID_NID_REC_SIZE;
  cmd.access_uim.data_ptr  = (byte *) &temp_sid_nid_buffer;
  cmd.access_uim.offset    = 1; /* The first record */
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.item      = UIM_CDMA_HOME_SID_NID;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }

  /* First pair */
  nv_cmd_ptr->data_ptr->home_sid_nid.pair[0].sid = temp_sid_nid_buffer.sid;
  nv_cmd_ptr->data_ptr->home_sid_nid.pair[0].nid = temp_sid_nid_buffer.nid;

  /* Check number of records */
  if(cmd.access_uim.num_records_rsp > NV_MAX_HOME_SID_NID)
    num_of_record = NV_MAX_HOME_SID_NID; /* RAM can only handle this many */
  else
    num_of_record = cmd.access_uim.num_records_rsp;
  
  /* Read the rest of records */
  for(i = 1; (i < num_of_record) && (temp_sid_nid_buffer.sid != 0); i++)
  {
 
    cmd.access_uim.offset = i+1; /* Record number */
    cmd.access_uim.rec_mode = UIM_ABSOLUTE;  /* Mode */

    (void) nvruim_access( &cmd ); /* Access the R-UIM*/
  
    if(nvruim_ruim_status_report == UIM_PASS)
    {
      nv_cmd_ptr->data_ptr->home_sid_nid.pair[i].sid =
        temp_sid_nid_buffer.sid;
      nv_cmd_ptr->data_ptr->home_sid_nid.pair[i].nid =
        temp_sid_nid_buffer.nid;
    }
    else
    {
      nv_cmd_ptr->status = *op_status;
      return NV_RUIM_SUPPORTS_ITEM;
    }
  }
  nvruim_read_sw(op_status,RUIM_NO_CACHE_VALUE);
  /* Report UIM_PASS since the first read is good */
  nvruim_ruim_status_report = UIM_PASS;
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;
}

#ifndef FEATURE_UIM_JCDMA_RUIM_SUPPORT
/*===========================================================================

FUNCTION NVRUIM_READ_PRL

DESCRIPTION
Check to see if PRL is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_prl(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status    /* status of the I/O operation*/
)
{
  boolean                dummy;
  nv_ruim_support_status support_status      = NV_RUIM_SUPPORTS_ITEM;
  uint32                 i;
  /* RUIM prl size */
  word                   pr_list_size        = 0;    
#ifdef FEATURE_2ND_PRL
  /* First prl size */  
  word                   pr_list_size1       = 0;   
#endif /* FEATURE_2ND_PRL */
  /* Tracking ruim card offset */  
  word                   offset = 0;
  /* Devide a long EF to multiple accessers */  
  byte                   num_of_accesses     = 0; 
  /* Remainder */  
  byte                   num_of_bytes_remain = 0; 

  /* Validate all sspr p rev version with P REV 1 first.*/
  prl_sspr_p_rev_e_type  sspr_p_rev          = PRL_SSPR_P_REV_1;  
  boolean                is_full_validation  = TRUE;  
  
  /* Only process the NAM parameters for the RUIM
       This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->roaming_list.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  nvruim_read_item(prl_size,
                   2,
                   0,
                   UIM_CDMA_MAXIMUM_PRL);  

  /* Check if the read succeeded. */
  if(nvruim_ruim_status_report == UIM_PASS)
  {
    max_prl_size = (prl_size[0] << 8) | prl_size[1];
    /* Should not be bigger than buffer */
    if(max_prl_size > NV_ROAMING_LIST_MAX_SIZE)
      max_prl_size = NV_ROAMING_LIST_MAX_SIZE;
  }
  else
  {
    /* Continue to RFprl even failing to read this file */
    max_prl_size = NV_ROAMING_LIST_MAX_SIZE;
  }
  
  /* Read the PR_LIST_SIZE */
  cmd.access_uim.num_bytes = 2;
  cmd.access_uim.data_ptr  = prl_size;
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item= UIM_CDMA_PRL;  

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  /* Read the rest of PRL */

  /* Fill up list size */
  pr_list_size = (prl_size[0] << 8) | prl_size[1];
  
  /* Check to see if the list size is valid */
  if((pr_list_size == 0) || (pr_list_size > max_prl_size))
  {
    nvruim_ruim_status_report = UIM_FAIL;
    MSG_LOW("RUIM PRL Size = %d", pr_list_size,0,0);
    nv_cmd_ptr->status= *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  /* Read the prl to a buffer */
  prl_ptr   = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list;
  offset    = 0;
  
  /* 255 byte each read. Figure out how many read needed */
  /* Include tow bytes of PR_LIST_MAX_SIZE */
  num_of_accesses     = (byte) (pr_list_size / MAX_BYTES_PER_ACCESS);
  num_of_bytes_remain = (byte) (pr_list_size % MAX_BYTES_PER_ACCESS);
  
  /* Read all blocks */
  for(i=0; i<num_of_accesses; i++)
  {
    nvruim_read_item(prl_ptr,
                     MAX_BYTES_PER_ACCESS,
                     offset,
                     UIM_CDMA_PRL);
    
    prl_ptr += MAX_BYTES_PER_ACCESS;
    offset  += MAX_BYTES_PER_ACCESS;
    
    if(nvruim_ruim_status_report != UIM_PASS)
    {
      nv_cmd_ptr->status = *op_status;
      return NV_RUIM_SUPPORTS_ITEM;
    }
  } /* for */
  
  if(nvruim_ruim_status_report != UIM_PASS)
  {
    nv_cmd_ptr->status = *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  if(num_of_bytes_remain != 0)
  {
    /* The last read of the prl */
    nvruim_read_item(prl_ptr,
                     num_of_bytes_remain,
                     offset,
                     UIM_CDMA_PRL);
  }
  
  if(nvruim_ruim_status_report != UIM_PASS)
  {
    nv_cmd_ptr->status = *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  
#ifdef FEATURE_2ND_PRL
  /* We have read the first prl so we need to update the size here */
  prl_ptr   = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list;
  /* Check the validity of the first PRL */
  if(PRL_VALID == prl_validate((byte*) prl_ptr, NULL, &sspr_p_rev, 
                               is_full_validation))
  {
    /* Fill up list size
    The first 2 byte of the roaming list contains the first prl size */
    nv_cmd_ptr->data_ptr->roaming_list.size  = prl_ptr[0];
    nv_cmd_ptr->data_ptr->roaming_list.size <<= 8;
    nv_cmd_ptr->data_ptr->roaming_list.size |=  prl_ptr[1];

    /* Present the size of the 683A PRL in bits */
    nv_cmd_ptr->data_ptr->roaming_list.size *= 8;

    /* Fill up list id 
    The 3rd and 4th byte contains the version information */
    nv_cmd_ptr->data_ptr->roaming_list.prl_version  = prl_ptr[2];
    nv_cmd_ptr->data_ptr->roaming_list.prl_version <<= 0x08;
    nv_cmd_ptr->data_ptr->roaming_list.prl_version |= prl_ptr[3];
  
    /* Point to the end of first PRL */
    prl_ptr   = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list
                + pr_list_size;
    /* Remember the first prl size */
    pr_list_size1 = pr_list_size;
  } /* if First PRL is valid */   
  else
  {
    pr_list_size1 = 0;
    nv_cmd_ptr->data_ptr->roaming_list.size = 0;	  
  }	  
  /* EF Next Offset */
  offset    = pr_list_size;

  /* Read the the second PR_LIST_SIZE */
  nvruim_read_item(prl_size,
                   2,
                   pr_list_size,
                   UIM_CDMA_PRL);

  
  if(nvruim_ruim_status_report != UIM_PASS)
  {
    /* We had good first PRL so don't fail here, reset to PASS*/
    nvruim_ruim_status_report = UIM_PASS;
    nv_cmd_ptr->status = *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  /* Fill up list size */
  pr_list_size = (prl_size[0] << 8) | prl_size[1];
  
  if((pr_list_size == 0)||(pr_list_size > max_prl_size))
  {
    MSG_HIGH("Invalid second RUIM PRL Size = %d", pr_list_size,0,0);
    nv_cmd_ptr->status = *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /* Include two bytes of total CRC to prl size */
  pr_list_size += 2;
  
  /* Check to see if the list size is valid. */
  if((pr_list_size + pr_list_size1) > max_prl_size)
  {
    MSG_HIGH("Invalid second RUIM PRL Size = %d", pr_list_size,0,0);
    nv_cmd_ptr->status = *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  /* 255 byte each read. Figure out how many read needed */
  /* Include tow bytes of PR_LIST_MAX_SIZE */
  num_of_accesses     = (byte) (pr_list_size / MAX_BYTES_PER_ACCESS);
  num_of_bytes_remain = (byte) (pr_list_size % MAX_BYTES_PER_ACCESS);
  
  /* Read all blocks */
  for(i=0; i<num_of_accesses; i++)
  {
    nvruim_read_item(prl_ptr,
                     MAX_BYTES_PER_ACCESS,
                     offset,
                     UIM_CDMA_PRL);

    prl_ptr += MAX_BYTES_PER_ACCESS;
    offset  += MAX_BYTES_PER_ACCESS;
    
    if(nvruim_ruim_status_report != UIM_PASS)
    {
      /* We had good first PRL so don't fail here */
      nvruim_ruim_status_report = UIM_PASS;
      nv_cmd_ptr->status = *op_status;
      return NV_RUIM_SUPPORTS_ITEM;
    }
  } /* for */
  
  if(nvruim_ruim_status_report != UIM_PASS)
  {
    /* We had good first PRL so don't fail here */
    nvruim_ruim_status_report = UIM_PASS;
    nv_cmd_ptr->status = *op_status;
    return NV_RUIM_SUPPORTS_ITEM;
  }
  
  
  if(num_of_bytes_remain != 0)
  {
    /* The last read of the prl */
    nvruim_read_item(prl_ptr,
                     num_of_bytes_remain,
                     offset,
                     UIM_CDMA_PRL);
  }
  if(nvruim_ruim_status_report != UIM_PASS)
  {
    /* We had good first PRL so don't fail here */
    nvruim_ruim_status_report = UIM_PASS;
  }
#endif /* FEATURE_2ND_PRL */
  
  nvruim_read_sw(op_status,RUIM_NO_CACHE_VALUE);
  
#ifdef FEATURE_2ND_PRL
  /* Move to Second PRL */
  prl_ptr   = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list
              + pr_list_size1;
  /* Check the validity of the second PRL */
  if(PRL_VALID == prl_validate((byte*) prl_ptr, NULL,&sspr_p_rev, 
                               is_full_validation))
  {
    /* When First PRL is invalid */
    if( 0x00 == pr_list_size1)
    {
      /* Fill up list size
         The first 2 byte of the roaming list contains the first prl size */
      nv_cmd_ptr->data_ptr->roaming_list.size  = prl_ptr[0];
      nv_cmd_ptr->data_ptr->roaming_list.size <<= 8;
      nv_cmd_ptr->data_ptr->roaming_list.size |=  prl_ptr[1];
      /* Present the size of the 683A PRL in bits */
      nv_cmd_ptr->data_ptr->roaming_list.size *= 8;
      /* Fill up list id
         The 3rd and 4th byte contains the version information */
      nv_cmd_ptr->data_ptr->roaming_list.prl_version  = prl_ptr[2];
      nv_cmd_ptr->data_ptr->roaming_list.prl_version <<= 0x08;
      nv_cmd_ptr->data_ptr->roaming_list.prl_version |= prl_ptr[3];
    }
    else
    {
      /* Both are valid */
      pr_list_size = (prl_size[0] << 8) | prl_size[1];
      nv_cmd_ptr->data_ptr->roaming_list.size =  pr_list_size1 + pr_list_size + 2;
      /* Present the size of the 683A PRL in bits */
      nv_cmd_ptr->data_ptr->roaming_list.size *= 8;
    }
  } /* second PRL validation */
  /* When second PRL is invalid then,
     No need to update the PRL length as we already update it for first PRL */
  
#else /* FEATURE_2ND_PRL */
  
  /* Reformat the data only if the read was successful. */
  if(NV_DONE_S == *op_status)
  {
    /* Validate the PRL */
  #ifdef FEATURE_SD20
  #ifdef FEATURE_EXTENDED_PRL
    if(prl_validate(
        (byte*) nv_cmd_ptr->data_ptr->roaming_list.roaming_list, NULL,
        &sspr_p_rev, is_full_validation)  == PRL_VALID)
  #else
    if(prl_validate(
        (byte*) nv_cmd_ptr->data_ptr->roaming_list.roaming_list, NULL)
         == PRL_VALID)
  #endif
  #else /* !FEATURE_SD20 */
    if(mcsys_validate_roaming_list(
        (byte *) nv_cmd_ptr->data_ptr->roaming_list.roaming_list)
        == PRL_VALID)
  #endif /* FEATURE_SD20 */
    {
      nv_cmd_ptr->data_ptr->roaming_list.valid = TRUE;
  
      prl_ptr   = (byte *)nv_cmd_ptr->data_ptr->roaming_list.roaming_list;
  
      /* Fill up list size */
      nv_cmd_ptr->data_ptr->roaming_list.size  = *prl_ptr++ << 8;
      nv_cmd_ptr->data_ptr->roaming_list.size |=  *prl_ptr++;
  
      /* Present the size of the 683A PRL in bits */
      nv_cmd_ptr->data_ptr->roaming_list.size *= 8;
  
      /* Fill up list id */
      nv_cmd_ptr->data_ptr->roaming_list.prl_version  = *prl_ptr++ << 8;
      nv_cmd_ptr->data_ptr->roaming_list.prl_version |= *prl_ptr++;
  
    }
    else
    {
      nv_cmd_ptr->data_ptr->roaming_list.valid = FALSE;
    }
  } /* end if - the read was successful. */
  
#endif /* FEATURE_2ND_PRL */
  
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;
}
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

/*===========================================================================

FUNCTION NVRUIM_READ_SPC_ENABLED

DESCRIPTION
Check to see if SPC_ENABLED is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_spc_enabled(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;
  
  #ifdef FEATURE_OTASP_OTAPA
  if(nv_cmd_ptr->item == NV_OTAPA_ENABLED_I)
  {
    /* Only process the NAM parameters for the RUIM
    This check assumes the first byte of the data contains the NAM */
    if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->otapa_enabled.nam)
    {
      /* Go to NV for this item */
      return NV_RUIM_ITEM_NOT_SUPPORTED;
    }
  }
  #endif

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  = &nvruim_otapa_spc_enable_cache_buf;
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_OTAPA_SPC_ENABLE;
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_OTAPA_SPC_ENABLE_CACHE_BIT,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  switch(nv_cmd_ptr->item)
  {
    case NV_SPC_CHANGE_ENABLED_I:
      if(NV_DONE_S == *op_status)
      {
#ifdef FEATURE_VERSION_W208S
        nv_cmd_ptr->data_ptr->spc_change_enabled =
          !((nvruim_otapa_spc_enable_cache_buf & NVRUIM_SPC_DISABLE) == 0);
#else
		nv_cmd_ptr->data_ptr->spc_change_enabled = 
          ((nvruim_otapa_spc_enable_cache_buf & NVRUIM_SPC_DISABLE) == 0);
#endif
      } /* end if - the read was successful. */
      break;
  
  #ifdef FEATURE_OTASP_OTAPA
    case NV_OTAPA_ENABLED_I:
      /* Copy the data only if the read was successful. */
      if(NV_DONE_S == *op_status)
      {
        /* The enable field is set to TRUE when the bit in the EF is set to
          zero */
        nv_cmd_ptr->data_ptr->otapa_enabled.enabled =
            ((nvruim_otapa_spc_enable_cache_buf & NVRUIM_OTAPA_DISABLE) == 0);
      } /* end if - the read was successful. */
      break;
  #endif
    default:
      break;
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
  
}

#ifdef FEATURE_OTASP_OTAPA
/*===========================================================================

FUNCTION NVRUIM_READ_NAM_LOCK

DESCRIPTION
Check to see if NAM_LOCK is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_nam_lock(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;
  
  /* Only process the NAM parameters for the RUIM
  This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->nam_lock.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  = & nvruim_nam_lock_buf;
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_NAM_LOCK;
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->nam_lock.enabled =
      (( nvruim_nam_lock_buf & NVRUIM_NAM_LOCK) == NVRUIM_NAM_LOCK);
  } /* end if - the read was successful. */
  
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;

}
#endif/*#ifdef FEATURE_OTASP_OTAPA*/


/*===========================================================================

FUNCTION NVRUIM_READ_SVC_PREF

DESCRIPTION
Check to see if SVC_PREF is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_svc_pref(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean                dummy;
  nv_ruim_support_status support_status;
  byte                   temp_service_pref_buf;

  /* Only process the NAM parameters for the RUIM
     This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->pref_mode.nam){
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = sizeof(nvruim_svc_pref_cache_buf);
  cmd.access_uim.data_ptr  = &nvruim_svc_pref_cache_buf;
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_SERVICE_PREF;  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SVC_PREF_CACHE_BIT,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  switch(nv_cmd_ptr->item)
  {
  #ifndef FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM
    case  NV_PREF_MODE_I:
      /* Analog/Digital selection */
      if(NV_DONE_S == *op_status)
      {
        temp_service_pref_buf = nvruim_svc_pref_cache_buf;
        temp_service_pref_buf &= NVRUIM_PREF_MODE_MASK;
  
        switch(temp_service_pref_buf)
        {
          case NVRUIM_ANALOG_PREF:
            /* Analog preferred */
            nv_cmd_ptr->data_ptr->pref_mode.mode = NV_MODE_ANALOG_PREF;
            break;
          case NVRUIM_CDMA_PREF:
            /* CDMA preferred */
            nv_cmd_ptr->data_ptr->pref_mode.mode = NV_MODE_AUTOMATIC;
            break;
          case NVRUIM_ANALOG_ONLY:
            /* Analog only */
            nv_cmd_ptr->data_ptr->pref_mode.mode = NV_MODE_ANALOG_ONLY;
            break;
          case NVRUIM_CDMA_ONLY:
            /* CDMA only */
            nv_cmd_ptr->data_ptr->pref_mode.mode = NV_MODE_DIGITAL_ONLY;
            break;
          default:
            nv_cmd_ptr->data_ptr->pref_mode.mode = NV_MODE_AUTOMATIC;
            break;
        }
      }
      break;
  #endif
  
  #ifdef FEATURE_SSPR_ENHANCEMENTS
    case  NV_SYSTEM_PREF_I:
  #else /* FEATURE_SSPR_ENHANCEMENTS */
    case  NV_CDMA_PREF_SERV_I:
    case  NV_ANALOG_PREF_SERV_I:
  #endif /* FEATURE_SSPR_ENHANCEMENTS */
      /* System A/B selection */
      if(NV_DONE_S == *op_status)
      {
        temp_service_pref_buf = nvruim_svc_pref_cache_buf;
        temp_service_pref_buf &= NVRUIM_SYSTEM_PREF_MASK;
  
        switch(temp_service_pref_buf)
        {
        #ifdef FEATURE_SSPR_ENHANCEMENTS
          case NVRUIM_A_ONLY:
            /* A only */
            nv_cmd_ptr->data_ptr->system_pref.sys = NV_SP_A_ONLY;
            break;
          case NVRUIM_B_ONLY:
            /* B only */
            nv_cmd_ptr->data_ptr->system_pref.sys = NV_SP_B_ONLY;
            break;
          default:
            nv_cmd_ptr->data_ptr->system_pref.sys = NV_SP_STANDARD;
            break;
        #else /* FEATURE_SSPR_ENHANCEMENTS */
          case NVRUIM_A_PREF:
            nv_cmd_ptr->data_ptr->analog_pref_serv.sys =
              nv_cmd_ptr->data_ptr->cdma_pref_serv.sys = NV_SYS_A_PREF;
            break;
          case NVRUIM_B_PREF:
            nv_cmd_ptr->data_ptr->analog_pref_serv.sys =
            nv_cmd_ptr->data_ptr->cdma_pref_serv.sys = NV_SYS_B_PREF;
            break;
          case NVRUIM_A_ONLY:
            nv_cmd_ptr->data_ptr->analog_pref_serv.sys =
            nv_cmd_ptr->data_ptr->cdma_pref_serv.sys = NV_SYS_A_ONLY;
            break;
          case NVRUIM_B_ONLY:
            nv_cmd_ptr->data_ptr->analog_pref_serv.sys =
            nv_cmd_ptr->data_ptr->cdma_pref_serv.sys = NV_SYS_B_ONLY;
            break;
          default:
            nv_cmd_ptr->data_ptr->analog_pref_serv.sys =
            nv_cmd_ptr->data_ptr->cdma_pref_serv.sys = NV_SYS_HOME_PREF;
            break;
        #endif /* FEATURE_SSPR_ENHANCEMENTS */
      }
    }
    default:
      break;
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}

/*===========================================================================

FUNCTION NVRUIM_READ_TMSI

DESCRIPTION
Check to see if TMSI is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_tmsi(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean                dummy;
  nv_ruim_support_status support_status;
  uint32                 i;

  /* Only process the NAM parameters for the RUIM
    This check assumes the first byte of the data contains the NAM */
  if(NV_RUIM_NAM != nv_cmd_ptr->data_ptr->assigning_tmsi_zone_length.nam)
  {
    /* Go to NV for this item */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  
  cmd.access_uim.num_bytes = sizeof(nvruim_tmsi_type);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_tmsi_cache_buf);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_TMSI;  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_TMSI_CACHE_BIT,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  switch(nv_cmd_ptr->item)
  {
    case  NV_ASSIGNING_TMSI_ZONE_LEN_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->assigning_tmsi_zone_length.length =
          nvruim_tmsi_cache_buf.length;
      }
      break;
  
    case  NV_ASSIGNING_TMSI_ZONE_I:
      if(NV_DONE_S == *op_status)
      {
        /* Copy to qword */
        for(i=0; i<8; i++)
        {
          QW_BYTE(nv_cmd_ptr->data_ptr->assigning_tmsi_zone.zone, i) =
            nvruim_tmsi_cache_buf.zone[i];
        }
      }
      break;
  
    case  NV_TMSI_CODE_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->tmsi_code.code =
          (nvruim_tmsi_cache_buf.code[3] << 24) |
          (nvruim_tmsi_cache_buf.code[2] << 16) |
          (nvruim_tmsi_cache_buf.code[1] << 8)  |
          nvruim_tmsi_cache_buf.code[0];
      }
      break;
  
    case  NV_TMSI_EXP_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->tmsi_exp_timer.time =
          (nvruim_tmsi_cache_buf.time[2] << 16) |
          (nvruim_tmsi_cache_buf.time[1] << 8)  |
          nvruim_tmsi_cache_buf.time[0];
      }
      break;
    default:
      break;
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}
/*===========================================================================

FUNCTION NVRUIM_READ_LOC_REGN_IND

DESCRIPTION
Check to see if LOC REGN IND is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_loc_regn_ind(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = sizeof(nvruim_analog_reg_ind_type);
  cmd.access_uim.data_ptr = (byte *) &(nvruim_analog_reg_ind_cache_buf);
  cmd.access_uim.offset   = 0;
  cmd.access_uim.item     = UIM_CDMA_ANALOG_LOCN_AND_REGN_IND;
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_ANALOG_REG_IND_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }

  switch(nv_cmd_ptr->item)
  { 
    case  NV_NXTREG_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->nxtreg =
          (nvruim_analog_reg_ind_cache_buf.nxtreg[2] << 16) |
          (nvruim_analog_reg_ind_cache_buf.nxtreg[1] << 8) |
          nvruim_analog_reg_ind_cache_buf.nxtreg[0];
      }
      break;
    case  NV_LSTSID_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->lstsid =
          (nvruim_analog_reg_ind_cache_buf.sid[1] << 8) |
          nvruim_analog_reg_ind_cache_buf.sid[0];
      }
      break;

    case  NV_LOCAID_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->locaid =
          ((nvruim_analog_reg_ind_cache_buf.locaid_pureg[1] & 0x7F) << 8) |
            nvruim_analog_reg_ind_cache_buf.locaid_pureg[0];
      }
      break;

    case  NV_PUREG_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->pureg =
          ((nvruim_analog_reg_ind_cache_buf.locaid_pureg[1] & 0x80) != 0);
      }
      break;
    default:
      break;
  }
  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}

/*===========================================================================

FUNCTION NVRUIM_READ_ZONE_BASED_IND

DESCRIPTION
Check to see if ZONE BASED IND is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_zone_based_ind(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = sizeof(nvruim_zone_reg_ind_type);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_zone_reg_ind_cache_buf);
  cmd.access_uim.offset    = 1;
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.item      = UIM_CDMA_ZONE_BASED_REGN_IND;


  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_ZONE_REG_IND_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->zone_list.reg_zone =
     ( nvruim_zone_reg_ind_cache_buf.reg_zone[1]  << 8 ) |
       nvruim_zone_reg_ind_cache_buf.reg_zone[0];
    nv_cmd_ptr->data_ptr->zone_list.reg_zone &= 0x0FFF;

    nv_cmd_ptr->data_ptr->zone_list.sid =
      ( nvruim_zone_reg_ind_cache_buf.sid[1]  << 8  ) |
      nvruim_zone_reg_ind_cache_buf.sid[0];
    nv_cmd_ptr->data_ptr->zone_list.sid &= 0x7FFF;

    nv_cmd_ptr->data_ptr->zone_list.nid =
      ( nvruim_zone_reg_ind_cache_buf.nid[1] << 8) |
      nvruim_zone_reg_ind_cache_buf.nid[0];

    nv_cmd_ptr->data_ptr->zone_list.band_class =
      nvruim_zone_reg_ind_cache_buf.band_class;

    if(nv_cmd_ptr->data_ptr->zone_list.band_class == 0)
    {                                                /* Cellular band */
      /* Derive system A or B from SID */
      if(nv_cmd_ptr->data_ptr->zone_list.sid & 0x01)
        nv_cmd_ptr->data_ptr->zone_list.block_or_sys = NV_CELLULAR_SYS_A;
      else
        nv_cmd_ptr->data_ptr->zone_list.block_or_sys = NV_CELLULAR_SYS_B;
    }
    else
    { /* PCS band and others */
      nv_cmd_ptr->data_ptr->zone_list.block_or_sys =
        (nv_block_or_sys_enum_type) nvruim_zone_reg_ind_cache_buf.freq_block;
    }
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}

/*===========================================================================

FUNCTION NVRUIM_READ_SYS_REGN_IND

DESCRIPTION
Check to see if SYS REGN IND is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_sys_regn_ind(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status    /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = sizeof(nvruim_sys_reg_ind_type);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_sys_reg_ind_cache_buf);
  /* Ignore the size since we only read one entry */
  cmd.access_uim.offset    = 1;
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;
  cmd.access_uim.item      = UIM_CDMA_SYS_REGN_IND;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SYS_REG_IND_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->sid_nid_list.sid =
      ( nvruim_sys_reg_ind_cache_buf.sid[1] << 8 ) |
      nvruim_sys_reg_ind_cache_buf.sid[0];
    nv_cmd_ptr->data_ptr->sid_nid_list.sid &= 0x7FFF;

    nv_cmd_ptr->data_ptr->sid_nid_list.nid =
      ( nvruim_sys_reg_ind_cache_buf.nid[1] << 8 ) |
      nvruim_sys_reg_ind_cache_buf.nid[0];

    nv_cmd_ptr->data_ptr->sid_nid_list.band_class =
    nvruim_sys_reg_ind_cache_buf.band_class >> 3;

    if(nv_cmd_ptr->data_ptr->sid_nid_list.band_class == 0)
    {                                                   /* Cellular band */
      /* Derive system A or B from SID */
      if(nv_cmd_ptr->data_ptr->sid_nid_list.sid & 0x01)
        nv_cmd_ptr->data_ptr->sid_nid_list.block_or_sys = NV_CELLULAR_SYS_A;
      else
        nv_cmd_ptr->data_ptr->sid_nid_list.block_or_sys = NV_CELLULAR_SYS_B;
    }
    else
    { /* PCS band or others */
      nv_cmd_ptr->data_ptr->sid_nid_list.block_or_sys =
        (nv_block_or_sys_enum_type) nvruim_sys_reg_ind_cache_buf.freq_block;
    }
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}
/*===========================================================================

FUNCTION NVRUIM_READ_DIST_REGN_IND

DESCRIPTION
Check to see if DIST REGN IND is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_dist_regn_ind(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }


  cmd.access_uim.num_bytes = sizeof(nvruim_dist_reg_ind_type);
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_dist_reg_ind_cache_buf);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_DIST_BASED_REGN_IND;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_DIST_REG_IND_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->dist_reg.base_lat_reg =
      (nvruim_dist_reg_ind_cache_buf.base_lat_reg[2] << 16) |
      (nvruim_dist_reg_ind_cache_buf.base_lat_reg[1] << 8) |
      nvruim_dist_reg_ind_cache_buf.base_lat_reg[0];

    nv_cmd_ptr->data_ptr->dist_reg.base_long_reg =
      (nvruim_dist_reg_ind_cache_buf.base_long_reg[2] << 16) |
      (nvruim_dist_reg_ind_cache_buf.base_long_reg[1] << 8) |
      nvruim_dist_reg_ind_cache_buf.base_long_reg[0];

    nv_cmd_ptr->data_ptr->dist_reg.reg_dist_reg =
      (nvruim_dist_reg_ind_cache_buf.reg_dist_reg[1] << 8) |
      nvruim_dist_reg_ind_cache_buf.reg_dist_reg[0];
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}

#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================

FUNCTION NVRUIM_READ_3GPD

DESCRIPTION
Check to see if 3GPD is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean  dummy;
  nv_ruim_support_status support_status;

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }

#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_sip_svc && !nvruim_3gpd_mip_svc)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes  = 1;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_3GPD_op_mode_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_3GPDOPM;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_NO_CACHE_VALUE,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->ds_qcmip = nvruim_3GPD_op_mode_ef_buf & NVRUIM_3GPDOPM_MASK;
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}


#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
/*===========================================================================

FUNCTION NVRUIM_READ_3GPDSIPUPPEXT

DESCRIPTION
Check to see if the 3GPDSIPUPPEXT item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_sipuppext(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  nv_ruim_support_status support_status;
  word src_bit_count=0;
  byte num_nai=0;
  byte nai_count=0;
  boolean ef_data_refreshed = TRUE;
  byte profile_number=0;
  word i=0;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("NULL parameter in nvruim_read_3gpd_sipuppext!",0,0,0);
    return(NV_RUIM_SUPPORTS_ITEM);
  }

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support()
     || UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_sip_svc || !nvruim_data_3gpd_ext_support())
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  } /* end !nvruim_3gpd_sip_svc ||  !nvruim_data_3gpd_ext_support()*/
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_data_3gpd_ext_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes  = NVRUIM_MAX_SIPUPPEXT_SIZE;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_sipuppext_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_SIPUPPEXT;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SIPUPPEXT_EF_CACHE_BIT,op_status,&ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    if(ef_data_refreshed)
    {
      /* If EF DATA Has been refreshed, our Buffer must be reset */
      /* The src counter is a "bit pointer" used as a cursor in the buffer */
      src_bit_count = FSIZ(nvruim_sipuppext_ef_fields, num_nai_field); /* Skip to first profile */
      nai_count = 0;
      /* The number of NAIs is specified in bits 0-4 of the file */
      num_nai = (nvruim_sipuppext_ef_buf[0] & NVRUIM_SIPUPPEXT_NUM_NAI_MASK) >> 4;

      /* Clear out the buffer containing ptrs to SIP Profiles */
      for(i = 0; i < NVRUIM_MAX_NUM_SIP_PROFILES; i++)
      {
        nvruim_sip_ext_profile_bit_index[i] = NVRUIM_NO_PROFILE_INDEX_SET;
      } /* end for */

      while(nai_count < num_nai)
      {
        profile_number = b_unpackb(nvruim_sipuppext_ef_buf, src_bit_count, 
                                   FSIZ(nvruim_sipuppext_ef_fields, profile_num_field));

        nvruim_sip_ext_profile_bit_index[profile_number] = src_bit_count;
        src_bit_count += FSIZ(nvruim_sipuppext_ef_fields, profile_num_field) +
                        FSIZ(nvruim_sipuppext_ef_fields, applications_field) +
                        FSIZ(nvruim_sipuppext_ef_fields, priority_field) +
                        FSIZ(nvruim_sipuppext_ef_fields, data_rate_mode_field) +
                        FSIZ(nvruim_sipuppext_ef_fields, data_bearer_field);
        nai_count++;
      } /* end while */
    } /* end if ef_data_refreshed */

    /* Copy Profile Info into NV structure */
    if(nvruim_sip_ext_profile_bit_index[nv_cmd_ptr->data_ptr->ds_sip_profile.index]
                      != NVRUIM_NO_PROFILE_INDEX_SET)
    {
      /* set src_bit_count to point to requested profile */
      src_bit_count = nvruim_sip_ext_profile_bit_index[nv_cmd_ptr->data_ptr->ds_sip_profile.index];
      /* skip over nai_index */
      src_bit_count += FSIZ(nvruim_sipuppext_ef_fields, profile_num_field);
      /* Applications field */
      nv_cmd_ptr->data_ptr->ds_sip_profile.app_type = 
          b_unpackd(nvruim_sipuppext_ef_buf, src_bit_count,
                    FSIZ(nvruim_sipuppext_ef_fields, applications_field));
      src_bit_count += FSIZ(nvruim_sipuppext_ef_fields, applications_field);
      /* Priority Field */
      nv_cmd_ptr->data_ptr->ds_sip_profile.app_priority = 
      b_unpackb(nvruim_sipuppext_ef_buf, src_bit_count,
                FSIZ(nvruim_sipuppext_ef_fields, priority_field));
      src_bit_count += FSIZ(nvruim_sipuppext_ef_fields, priority_field);
      /* Data Rate Mode Field */
      nv_cmd_ptr->data_ptr->ds_sip_profile.data_rate_mode = 
      b_unpackb(nvruim_sipuppext_ef_buf, src_bit_count,
                FSIZ(nvruim_sipuppext_ef_fields, data_rate_mode_field));
      src_bit_count += FSIZ(nvruim_sipuppext_ef_fields, data_rate_mode_field);
      /* Data Bearer */
      nv_cmd_ptr->data_ptr->ds_sip_profile.data_bearer = 
      b_unpackb(nvruim_sipuppext_ef_buf, src_bit_count,
                FSIZ(nvruim_sipuppext_ef_fields, data_bearer_field));
      src_bit_count += FSIZ(nvruim_sipuppext_ef_fields, data_bearer_field);

    } /* Requested Profile Found */
    else
    {
        MSG_HIGH("Reqeusted SIP NAI not found on card",0,0,0);
    }
  } /* end if - NV_DONE_S == *op_status */
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
} /* nvruim_read_3gpd_sipuppext */
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */


/*===========================================================================

FUNCTION NVRUIM_READ_3GPDSIPUPP

DESCRIPTION
Check to see if the 3GPDSIPUPP item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_sipupp(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  nv_ruim_support_status support_status;
  word src_bit_count=0;
  byte num_nai=0;
  byte nai_count=0;
  byte nai_length =0;
  byte nai_length_shortened=0;
  boolean ef_data_refreshed = TRUE;
  byte profile_number=0;
  word i=0;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("NULL parameter in nvruim_read_3gpd_sipupp!",0,0,0);
    return(NV_RUIM_SUPPORTS_ITEM);
  }

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support()
     || UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }
  /* Return the NV value for NV_PPP_USER_ID_I and NV_PPP_PASSWORD_I
     for OMH card */
  if((nv_cmd_ptr->item == NV_PPP_USER_ID_I) && 
     (TRUE == nvruim_data_3gpd_ext_support())) 
  {
    MSG_HIGH("Returning the NV value for NV_PPP_USER_ID_I for OMH card",0,0,0);
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
  
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_sip_svc)
  {
    switch(nv_cmd_ptr->item)
    {
      case NV_PPP_USER_ID_I:
          nv_cmd_ptr->data_ptr->ppp_user_id.user_id_len = 0;
          break;
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      case NV_DS_SIP_NUM_VALID_PROFILES_I:
          nv_cmd_ptr->data_ptr->ds_sip_num_valid_profiles  = 0;
          break;
      case NV_DS_SIP_NAI_INFO_I:
          nv_cmd_ptr->data_ptr->ds_sip_nai_info.nai_length = 0;
          break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      default:
          MSG_HIGH("Unsupported NV ITEM Cannot be handled in nvruim_read_3gpd_sipupp",0,0,0);
          return NV_RUIM_ITEM_NOT_SUPPORTED;
    } /* end switch nv_cmd_ptr->item */
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  } /* end !nvruim_3gpd_sip_svc */
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes  = NVRUIM_MAX_SIPUPP_SIZE;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_sipupp_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_SIPUPP;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SIPUPP_EF_CACHE_BIT,op_status,&ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    if(ef_data_refreshed)
    {
      /* If EF DATA Has been refreshed, our Buffer must be reset */
      /* The src counter is a "bit pointer" used as a cursor in the buffer */
      src_bit_count = FSIZ(nvruim_sipupp_ef_fields, length_of_block_field) +
                      FSIZ(nvruim_sipupp_ef_fields, num_nai_field); /* Skip to first profile */
      nai_count = 0;
      /* The number of NAIs is specified in bits 9-12 of the file */
      num_nai = (nvruim_sipupp_ef_buf[1] & NVRUIM_SIPUPP_NUM_NAI_MASK) >> 4;

      /* Clear out the buffer containing ptrs to SIP Profiles */
      for(i = 0; i < NVRUIM_MAX_NUM_SIP_PROFILES; i++)
      {
        nvruim_sip_profile_bit_index[i] = NVRUIM_NO_PROFILE_INDEX_SET;
      } /* end for */

      while(nai_count < num_nai)
      {
        profile_number = b_unpackb(nvruim_sipupp_ef_buf, src_bit_count, 
                                   FSIZ(nvruim_sipupp_ef_fields, profile_num_field));

        nvruim_sip_profile_bit_index[profile_number] = src_bit_count;
        src_bit_count += FSIZ(nvruim_sipupp_ef_fields, profile_num_field);
        nai_length = b_unpackb(nvruim_sipupp_ef_buf, src_bit_count,
                               FSIZ(nvruim_sipupp_ef_fields, nai_length_field));
        src_bit_count += FSIZ(nvruim_sipupp_ef_fields, nai_length_field) +
                        (nai_length * 8) + 
                        FSIZ(nvruim_sipupp_ef_fields, auth_algo_field);
        nai_count++;
      } /* end while */
    } /* end if ef_data_refreshed */

    switch(nv_cmd_ptr->item)
    {
      case NV_PPP_USER_ID_I:
        /* LEGACY CASE:  When Multiple SIP Profiles is NOT supported, 
        DATA will ask for a legacy NV item, which is NV_PPP_USER_ID_I.  
        This is always mapped to the index stored in 
        nvruim_preferred_sip_profile.  For backwards compatibility, 
        we still support returning this value when that NV item is read */
        if(nvruim_sip_profile_bit_index[nvruim_preferred_sip_profile]
                      != NVRUIM_NO_PROFILE_INDEX_SET)
          {
            src_bit_count = nvruim_sip_profile_bit_index[nvruim_preferred_sip_profile];
            src_bit_count += FSIZ(nvruim_sipupp_ef_fields, profile_num_field);
            nai_length = b_unpackb(nvruim_sipupp_ef_buf, src_bit_count,
                             FSIZ(nvruim_sipupp_ef_fields, nai_length_field));
            src_bit_count += FSIZ(nvruim_sipupp_ef_fields, nai_length_field);
            if(nai_length > NV_MAX_AN_PPP_USER_ID_LENGTH)
            {
              /* Make sure that the NAI on the card is not longer than
              What the NV structure can handle.  If it is, truncate it */
              nai_length_shortened = NV_MAX_AN_PPP_USER_ID_LENGTH;
            }
            else
            {
              nai_length_shortened = nai_length;
            }
            nv_cmd_ptr->data_ptr->ppp_user_id.user_id_len = nai_length_shortened;
            b_copy((void*) nvruim_sipupp_ef_buf, src_bit_count, 
               (void*) nv_cmd_ptr->data_ptr->ppp_user_id.user_id, 0, 
               nai_length_shortened * 8);  
          }
          else
          {
             MSG_HIGH("Preferred SIP NAI not found on card",0,0,0);
             nv_cmd_ptr->data_ptr->ppp_user_id.user_id_len = 0;
          }
        break;
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      case NV_DS_SIP_NUM_VALID_PROFILES_I:
        /* Skip to NUM Profile Parameter */
        nai_count = b_unpackb(nvruim_sipupp_ef_buf, 
                           FSIZ(nvruim_sipupp_ef_fields, length_of_block_field),
                           FSIZ(nvruim_sipupp_ef_fields, num_nai_field));
        nv_cmd_ptr->data_ptr->ds_sip_num_valid_profiles = nai_count;
        break;
      case NV_DS_SIP_NAI_INFO_I:
        if(nvruim_sip_profile_bit_index[nv_cmd_ptr->data_ptr->ds_sip_nai_info.index]
                    != NVRUIM_NO_PROFILE_INDEX_SET)
        {
          src_bit_count = nvruim_sip_profile_bit_index[nv_cmd_ptr->data_ptr->ds_sip_nai_info.index];
          src_bit_count += FSIZ(nvruim_sipupp_ef_fields, profile_num_field);
          nai_length = b_unpackb(nvruim_sipupp_ef_buf, src_bit_count,
                           FSIZ(nvruim_sipupp_ef_fields, nai_length_field));
          src_bit_count += FSIZ(nvruim_sipupp_ef_fields, nai_length_field);
          if(nai_length > NV_MAX_AN_PPP_USER_ID_LENGTH)
          {
            /* Make sure that the NAI on the card is not longer than
            What the NV structure can handle.  If it is, truncate it */
            nai_length_shortened = NV_MAX_AN_PPP_USER_ID_LENGTH;
          }
          else
          {
              nai_length_shortened = nai_length;
          }
          nv_cmd_ptr->data_ptr->ds_sip_nai_info.nai_length = nai_length_shortened;
          b_copy((void*) nvruim_sipupp_ef_buf, src_bit_count, 
             (void*) nv_cmd_ptr->data_ptr->ds_sip_nai_info.nai, 0, 
             nai_length_shortened * 8);  
        }
        else
        {
           MSG_HIGH("Reqeusted SIP NAI not found on card",0,0,0);
           nv_cmd_ptr->data_ptr->ds_sip_nai_info.nai_length = 0;
        }
        break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      default:
        MSG_HIGH("Unsupported NV ITEM Cannot be handled in nvruim_read_3gpd_sipupp",0,0,0);
        return NV_RUIM_ITEM_NOT_SUPPORTED;
    }
  } /* end if - NV_DONE_S == *op_status */
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}

/*===========================================================================

FUNCTION NVRUIM_READ_3GPDMIPUPP

DESCRIPTION
Check to see if the 3gPDMIPUPP item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_mipupp(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  word src_bit_count;
  byte num_nai;
  byte nai_length=0;
  byte nai_length_shortened=0;
  byte nai_count;
  boolean ef_data_refreshed = TRUE;
  byte profile_number;
  byte i;

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_mip_svc)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes  = NVRUIM_MAX_MIPUPP_SIZE;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_mipupp_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_MIPUPP;
  support_status            = nvruim_read_and_check_rsp(nv_cmd_ptr, cache_bit,op_status,&ef_data_refreshed);

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(ef_data_refreshed && NV_DONE_S == *op_status)
  {
    /* In this case, the bit pointer/index array is no longer valid, and 
    the following structures may not contain valid data:  
    nvruim_mip_profile_bit_index
    */
    
    /* The src counter is a "bit pointer" used as a cursor in the buffer */
    
    if((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_RETRY_INCL_MASK) >> 
       NVRUIM_RETRY_INFO_PRESENT_SHIFT)
    {
      /* Retry info is included in this EF */
      /* Skip to first profile */
      src_bit_count = FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                  FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field) + 
                  FSIZ(nvruim_mipupp_ef_fields, max_num_retry_field) +
                  FSIZ(nvruim_mipupp_ef_fields, retry_timeout_field) +
                  FSIZ(nvruim_mipupp_ef_fields, rereg_threshold_field) +
                  FSIZ(nvruim_mipupp_ef_fields, num_nai_field); 
      num_nai = b_unpackb(nvruim_mipupp_ef_buf, 
                      FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                      FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field) + 
                      FSIZ(nvruim_mipupp_ef_fields, max_num_retry_field) +
                      FSIZ(nvruim_mipupp_ef_fields, retry_timeout_field) +
                      FSIZ(nvruim_mipupp_ef_fields, rereg_threshold_field),
                      FSIZ(nvruim_mipupp_ef_fields, num_nai_field));
    } /* endif - retry info is included */
    else
    {
      src_bit_count = FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                  FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field) +
                  FSIZ(nvruim_mipupp_ef_fields, num_nai_field);
    
      num_nai = b_unpackb(nvruim_mipupp_ef_buf, 
                      FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                      FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field),
                      FSIZ(nvruim_mipupp_ef_fields, num_nai_field));
    }
    
    nai_count = 0;
    
    /* Clear out the buffer containing ptrs to MIP Profiles */
    for(i = 0; i < NVRUIM_MAX_NUM_MIP_PROFILES; i++)
    {
      nvruim_mip_profile_bit_index[i] = NVRUIM_NO_PROFILE_INDEX_SET;
    } /* end for */
    
    while(nai_count < num_nai)
    {
      profile_number = b_unpackb(nvruim_mipupp_ef_buf, src_bit_count,
                             FSIZ(nvruim_mipupp_ef_fields, nai_entry_index_field));
             nvruim_mip_profile_bit_index[profile_number] = src_bit_count;
      src_bit_count += FSIZ(nvruim_mipupp_ef_fields, nai_entry_index_field);
      nai_length = b_unpackb(nvruim_mipupp_ef_buf, src_bit_count,
                         FSIZ(nvruim_mipupp_ef_fields, nai_length_field));
    
      /* Skip past this profile */
      src_bit_count += ((nai_length * 8) + 
                    FSIZ(nvruim_mipupp_ef_fields, nai_length_field) +
                    FSIZ(nvruim_mipupp_ef_fields, tunnel_bit_field) +
                    FSIZ(nvruim_mipupp_ef_fields, home_addr_field) +
                    FSIZ(nvruim_mipupp_ef_fields, primary_ha_field) +
                    FSIZ(nvruim_mipupp_ef_fields, secondary_ha_field) +
                    FSIZ(nvruim_mipupp_ef_fields, mn_aaa_auth_algo_field));
      if(b_unpackb(nvruim_mipupp_ef_buf, src_bit_count, 
               FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_indicator_field)))
      {
        /* The MN-HA Indicator is set */
        src_bit_count += (FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_indicator_field) +
                      FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_field) +
                      FSIZ(nvruim_mipupp_ef_fields, mn_ha_auth_algo_field));
      }
      else
      {
        src_bit_count+= (FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_indicator_field) +
                     FSIZ(nvruim_mipupp_ef_fields, mn_ha_auth_algo_field));
      }
      if(b_unpackb(nvruim_mipupp_ef_buf, src_bit_count,
               FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_indicator_field)))
      {
        /* The MN-AAA Indicator is set */
        src_bit_count += (FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_indicator_field) +
                      FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_field));
      }
      else
      {
         src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_indicator_field);
      }
      nai_count++;
    } /* end while */
  } /* If ef_data_refreshed */

  switch(nv_cmd_ptr->item)
  {
    case  NV_DS_MIP_NUM_PROF_I:
      if(NV_DONE_S == *op_status)
      {
        if((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_RETRY_INCL_MASK) >> 
           NVRUIM_RETRY_INFO_PRESENT_SHIFT)
        {
          /* Retry Info is included in this EF */
          /* Skip to NUM Profile Parameter */
          nai_count = b_unpackb(nvruim_mipupp_ef_buf, 
                                (FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                                 FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field) +
                                 FSIZ(nvruim_mipupp_ef_fields, max_num_retry_field) +
                                 FSIZ(nvruim_mipupp_ef_fields, retry_timeout_field) +
                                 FSIZ(nvruim_mipupp_ef_fields, rereg_threshold_field)),
                                 FSIZ(nvruim_mipupp_ef_fields, num_nai_field));
        } /* endif - retry info is included */
        else
        {
          nai_count = b_unpackb(nvruim_mipupp_ef_buf,
                                FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                                FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field),
                                FSIZ(nvruim_mipupp_ef_fields, num_nai_field));
        }
        nv_cmd_ptr->data_ptr->ds_mip_num_prof = nai_count;
      }
      break;

    case  NV_DS_MIP_GEN_USER_PROF_I:
      if(NV_DONE_S == *op_status)
      {
        profile_number = nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.index;
        if(nvruim_mip_profile_bit_index[profile_number] != NVRUIM_NO_PROFILE_INDEX_SET)
        {
          /* The request profile is actually available.  Set the profile buffer */
          src_bit_count = nvruim_mip_profile_bit_index[profile_number] + 
                          FSIZ(nvruim_mipupp_ef_fields, nai_entry_index_field);
          nai_length = b_unpackb(nvruim_mipupp_ef_buf, src_bit_count,
                                 FSIZ(nvruim_mipupp_ef_fields, nai_length_field));
          if(nai_length >= NV_MAX_NAI_LENGTH)
          {
             nai_length_shortened = NV_MAX_NAI_LENGTH - 1;
          }
          else
          {
             nai_length_shortened = nai_length;
          }
          nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.nai_length = nai_length_shortened;
          src_bit_count += FSIZ(nvruim_mipupp_ef_fields, nai_length_field);

          b_copy((void*) nvruim_mipupp_ef_buf, src_bit_count, 
                 (void*) nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.nai, 0, 
                 nai_length_shortened * 8);
          nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.nai[nai_length_shortened] = 0; /* null terminate */
          src_bit_count += nai_length * 8;
          if(b_unpackb(nvruim_mipupp_ef_buf, src_bit_count, 
                       FSIZ(nvruim_mipupp_ef_fields, tunnel_bit_field)))
          {
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.rev_tun_pref = TRUE;
          }
          else
          {
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.rev_tun_pref = FALSE;
          }
          src_bit_count+= FSIZ(nvruim_mipupp_ef_fields, tunnel_bit_field);
          nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.home_addr = 
          b_unpackd(nvruim_mipupp_ef_buf, src_bit_count,
                    FSIZ(nvruim_mipupp_ef_fields, home_addr_field));
          src_bit_count += FSIZ(nvruim_mipupp_ef_fields, home_addr_field);

          nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.primary_ha_addr = 
          b_unpackd(nvruim_mipupp_ef_buf, src_bit_count, 
                    FSIZ(nvruim_mipupp_ef_fields, primary_ha_field));
          src_bit_count += FSIZ(nvruim_mipupp_ef_fields, primary_ha_field);

          nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.secondary_ha_addr = 
          b_unpackd(nvruim_mipupp_ef_buf, src_bit_count, 
                    FSIZ(nvruim_mipupp_ef_fields, secondary_ha_field));
          src_bit_count += FSIZ(nvruim_mipupp_ef_fields, secondary_ha_field);

          src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_aaa_auth_algo_field);
          if(b_unpackb(nvruim_mipupp_ef_buf, src_bit_count,
                       FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_indicator_field)))
          {
            src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_indicator_field);
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.mn_aaa_spi_set = TRUE;
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.mn_aaa_spi =
            b_unpackd(nvruim_mipupp_ef_buf, src_bit_count,
                      FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_field));
            src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_field);
          }
          else
          {
            src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_aaa_spi_indicator_field);
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.mn_aaa_spi_set = FALSE;
          }

          src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_ha_auth_algo_field);

          if(b_unpackb(nvruim_mipupp_ef_buf, src_bit_count, 
                       FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_indicator_field)))
          {
            src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_indicator_field);
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.mn_ha_spi_set = TRUE;
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.mn_ha_spi =
            b_unpackd(nvruim_mipupp_ef_buf, src_bit_count, 
                      FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_field));
            src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_field);
          }
          else
          {
            src_bit_count += FSIZ(nvruim_mipupp_ef_fields, mn_ha_spi_indicator_field); 
            nv_cmd_ptr->data_ptr->ds_mip_gen_user_prof.mn_ha_spi_set = FALSE;
          }
        } /* end if - the index pointed to a valid profile */
        else
        {
          /* Something was wrong - the profile was not found */
          MSG_HIGH("Profile not found in MIPUPP",0,0,0);
          *op_status = NV_NOTACTIVE_S;
        }
      }
      break;

    case NV_DS_MIP_RETRIES_I:
      if(NV_DONE_S == *op_status)
      {
        if((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_RETRY_INCL_MASK) >> 
           NVRUIM_RETRY_INFO_PRESENT_SHIFT)
        {
          /* Retry info is included in this EF */
          nv_cmd_ptr->data_ptr->ds_mip_retries = 
            ((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_MAX_RETRY_MASK) >> 
              NVRUIM_MAX_NUM_RETRY_SHIFT);
        } /* endif - retry info is included */
        else
        {
          MSG_HIGH("Retry Info not found in SIPUPP",0,0,0);
          *op_status = NV_NOTACTIVE_S;
        }
      }
      break;

    case NV_DS_MIP_RETRY_INT_I:
      if(NV_DONE_S == *op_status)
      {
        if((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_RETRY_INCL_MASK) >> 
           NVRUIM_RETRY_INFO_PRESENT_SHIFT)
        {
          /* Retry info is included in this EF */
          nv_cmd_ptr->data_ptr->ds_mip_retry_int = 
              ((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_RETRY_INT_MASK) >> 
               NVRUIM_MIP_RETRY_INT_SHIFT);
        } /* endif - retry info is included */
        else
        {
          MSG_HIGH("Retry Info not found in SIPUPP",0,0,0);
          *op_status = NV_NOTACTIVE_S;
        }
      }
      break;

    case NV_DS_MIP_PRE_RE_RRQ_TIME_I:
      if(NV_DONE_S == *op_status)
      {
        if((nvruim_mipupp_ef_buf[1] & NVRUIM_MIPUPP_RETRY_INCL_MASK) >> 
           NVRUIM_RETRY_INFO_PRESENT_SHIFT)
        {
          /* Retry info is included in this EF */
          nv_cmd_ptr->data_ptr->ds_mip_pre_re_rrq_time = 
          b_unpackb(nvruim_mipupp_ef_buf,
                    FSIZ(nvruim_mipupp_ef_fields, length_of_block_field) +
                    FSIZ(nvruim_mipupp_ef_fields, retry_info_incl_field) +
                    FSIZ(nvruim_mipupp_ef_fields, max_num_retry_field) +
                    FSIZ(nvruim_mipupp_ef_fields, retry_timeout_field),
                    FSIZ(nvruim_mipupp_ef_fields, rereg_threshold_field));
        } /* endif - retry info is included */
        else
        {
          MSG_HIGH("Retry Info not found in SIPUPP",0,0,0);
          *op_status = NV_NOTACTIVE_S;
        }
      }
      break;
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}


/*===========================================================================

FUNCTION NVRUIM_READ_3GPD_MIPFLAGS

DESCRIPTION
Check to see if the 3GPD MIP FLAGS item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_mipflags(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = TRUE;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_3gpd_mipflags",0,0,0);
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* 1. First check if 3GPD MIP support should come from NV.  If so, fallback.
     2. Next, if UIM support is indicated, make sure that the service is actually
        present, and it isn't just that neither MIP nor fallback is supported.  If so, 
        just return RUIM Support and DONE (which will block a fallback).
     3. Lastly, check to see if 3GPD Extensions support is present, if not, allow
     this item to fallback */

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    MSG_HIGH("Card does not support 3GPD, falling back to NV",0,0,0);
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }

#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_mip_svc)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_data_3gpd_ext_support())
  {
    if(nv_cmd_ptr->item == NV_DS_MIP_2002BIS_MN_HA_AUTH_I)
    {
      /* If this item is not supported on the card, and the ME is using RUIM for MIP Auth, 
         then only 2002BIS is supported */
      *op_status = NV_DONE_S;
      nv_cmd_ptr->data_ptr->ds_mip_2002bis_mn_ha_auth = TRUE;
      return NV_RUIM_SUPPORTS_ITEM;
    }

    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }


  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes  = 1;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_3gpd_mipflags_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_MIPFLAGS;
  support_status            = nvruim_read_and_check_rsp(nv_cmd_ptr, cache_bit, op_status,&ef_data_refreshed);

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  switch(nv_cmd_ptr->item)
  {
    case  NV_DS_MIP_2002BIS_MN_HA_AUTH_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->ds_mip_2002bis_mn_ha_auth = nvruim_3gpd_mipflags_ef_buf & 0x01;
      }
      break;

    case  NV_DS_MIP_QC_DRS_OPT_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->ds_mip_qc_drs_opt = 
            (nvruim_3gpd_mipflags_ef_buf >>  NVRUIM_MIPFLAGS_DRS_OPT_SHIFT) & 0x01;
      }
      break;

    case NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->ds_mip_qc_handdown_to_1x_opt = 
            (nvruim_3gpd_mipflags_ef_buf >> NVRUIM_MIPFLAGS_HANDDOWN_SHIFT) & 0x01;
      }
      break;

    case NV_DS_MIP_RRQ_IF_TFRK_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->ds_mip_rrq_if_tfrk = 
            (nvruim_3gpd_mipflags_ef_buf >> NVRUIM_MIPFLAGS_RRQ_IF_TRFK_SHIFT) & 0x01;
      }
      break;
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
} /* nvruim_read_3gpd_mipflags */


/*===========================================================================

FUNCTION NVRUIM_READ_3GPD_TCP_CONFIG

DESCRIPTION
Check to see if the 3GPD TCP CONFIG items are supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_tcp_config(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = TRUE;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_3gpd_tcp_config",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  /* 1. First check if 3GPD MIP support should come from NV.  If so, fallback.
     2. Next, if UIM support is indicated, make sure that the service is actually
        present, and it isn't just that neither MIP nor fallback is supported.  If so, 
        just return RUIM Support and DONE (which will block a fallback).
     3. Lastly, check to see if 3GPD Extensions support is present, if not, allow
     this item to fallback */

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }

#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_mip_svc)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_data_3gpd_ext_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }


  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes  = NVRUIM_MAX_TCPCONFIG_SIZE;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_3gpd_tcp_config_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_TCPCONFIG;
  support_status            = nvruim_read_and_check_rsp(nv_cmd_ptr, cache_bit, op_status,&ef_data_refreshed);

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }
  /* 
	Byte Index  Description
	1           TCP Flags
	2           TCP Keep Alive Idle Timer
  */     
  switch(nv_cmd_ptr->item)
  {
    case  NV_TCP_GRACEFUL_DORMANT_CLOSE_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->tcp_graceful_dormant_close = nvruim_3gpd_tcp_config_ef_buf[0] & 0x01;
      }
      break;
#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
    case  NV_TCP_KEEPALIVE_IDLE_TIME_I:
      if(NV_DONE_S == *op_status)
      {
        nv_cmd_ptr->data_ptr->tcp_keepalive_idle_time = nvruim_3gpd_tcp_config_ef_buf[1];
      }
      break; 
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */
  }/* End of switch(nv_cmd_ptr->item)*/
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
} /* nvruim_read_3gpd_tcp_config */


#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION NVRUIM_READ_LBS_XTRA_SERVER_URLS

DESCRIPTION
Check to see if the LBS XTRA SERVER URLS File Data Items are supported by the
R-UIM. If it is supported, read it.

DEPENDENCIES
Non-reentrant
TLV should be in proper and valid format in EF. Wrongly written data will not be 
read and may be generate useless information.
This code is only support the maximum length of 3 bytes for length parameter and 
if the value of length is greater than 128 byte, 
then only 128 byte will be return and remaining value will be drop.

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED   : Item is not supported
NV_RUIM_SUPPORTS_ITEM        : Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_lbs_xtra_server_urls(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
) 
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = FALSE;
  uint32 ef_offset = 0;
/* Taking 4 byte TLV Data Buffer, Assuming that Length will never exseed 0xFFFF
   value, so We need 1 byte for Tag, 3 byte information for length for example
   0x80 0x82 0xFF 0xFF */
  byte tl_data_buffer[4]={0xFF,0x00,0x00,0x00};
  byte *v_data_buffer = NULL;
/* As the file have 3 TLV structure, to read any URL TLV we may need to jump to
   reach to the specified TLV Data.
   For the Tertiary XTRA Server url_occurance value will be 0x03. 
   To read the 3rd URL TLV, we need to jump 2 times.
   For the Seconday XTRA Server url_occurance value will be 0x02.
   To read the 2nd URL TLV we need to jump 1 times.
   For the Primary XTRA Server url_occurance value will be 0x01. 
   No jump require we read the current TLV. */
  byte url_occurance = 0;

/*   Checking for Null Value   */
  if((nv_cmd_ptr == NULL) ||(nv_cmd_ptr->data_ptr == NULL)|| (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_lbs_xtra_server_urls",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }/* End of if((nv_cmd_ptr == NULL) || (op_status == NULL))*/

  /* Checking of rtre configuration, RTRE should initialized  */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /*  Checking of Boolean nvruim_lbs_support, it indicate whether n24 is 
  is supported or not 
  Checking the RTRE Mode whether it is NV Mode or not  */
  if((!nvruim_lbs_support) ||(NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control()))
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
  /* Preparing command to read data form the file 
   we only need the 4 byte to get the first TLV Data */
  cmd.access_uim.num_bytes     = 0x04;
  cmd.access_uim.data_ptr      = (byte *) &(tl_data_buffer);
  cmd.access_uim.offset        = ef_offset;
  cmd.access_uim.num_bytes_rsp = 0;
  cmd.access_uim.item          = UIM_CDMA_LBS_XSURL;
  support_status               = nvruim_read_and_check_rsp(nv_cmd_ptr, 
														   cache_bit, 
														   op_status,
														   &ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  /* Reading of 4 bytes information from file is successful.  
     Now we are setting the url_occurance and data buffer to access the 
	 requested XTRA server Url. 
	 As we are not caching the entire data for URL so every time 
	 we need to read the file again */
  switch(nv_cmd_ptr->item)
  {
    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:
      if(NV_DONE_S == *op_status)
      {
        url_occurance = NVRUIM_LBS_XTRA_URL_FIRST;
        v_data_buffer = (byte *)&(nv_cmd_ptr->data_ptr->gps1_xtra_primary_server_url[0x00]);
      }
      break;

    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:
      if(NV_DONE_S == *op_status)
      {
        url_occurance = NVRUIM_LBS_XTRA_URL_SECOND;
        v_data_buffer = (byte *)&(nv_cmd_ptr->data_ptr->gps1_xtra_secondary_server_url[0x00]);
      }
      break;

    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I: 
      if(NV_DONE_S == *op_status)
      {
        url_occurance = NVRUIM_LBS_XTRA_URL_THIRD;
        v_data_buffer = (byte *)&(nv_cmd_ptr->data_ptr->gps1_xtra_tertiary_server_url[0x00]);
      }
      break;

    default:
      url_occurance = 0x00;
	  MSG_ERROR("NV Item not found in nvruim_read_lbs_xtra_server_urls 0x%x",
				nv_cmd_ptr->item,0,0);
	  break;
          
  }/* End of switch(nv_cmd_ptr->item) */

  /* url_occurance should be greater than zero. 
     when the value of url_occurance will become 0x01 
	 that means we reach the requested url 
	 else we need to jump to the next url.
  */
  while(url_occurance) 
  {
    uint32 length_value=0x00;
    /* Start parsing the TLV Information     */
    /* If the tag is not 0x80, that means it is invalid, 
	   break no need to continue */
    /* Folowing combination of Length Information are allowed
     Length-Byte  Range
        1         0x00-0x7F
        2         0x81 0x80 -- 0xFF
        3         0x82 0x100 -- 0xXXXX  
     */
	/* Checking for invalid Tag Value, Max Length Value and Invalid Length 
	   Information*/
    if ((NVRUIM_LBS_XTRA_URL_TAG != tl_data_buffer[0]) 
		||(NVRUIM_MAX_LBS_URL_TAG_LEN_INFO < tl_data_buffer[1]) 
		||(NVRUIM_INVALID_LBS_URL_TAG_LEN_INFO == tl_data_buffer[1]))
    {
      /* Invalid value and No need to read the data */
	  /* Break the loop and we are also not able to jump to the next Tag */
      break;
    }
    /* Tag and length is valid, now we calculate the length of Url Information)*/  
    switch (tl_data_buffer[1]) 
    {
      case NVRUIM_LBS_XTRA_URL_LEN_TAG_82:
        /* in case when url tag is 
		   0x80   (TAG) 
		   0x82 0x01 0x00  (Length Information) */
        length_value = tl_data_buffer[2];
        length_value <<= 0x08;
        length_value |= tl_data_buffer[3];
        ef_offset = ef_offset + 0x04; /* 3 bytes for length and 1 for tag */
        break;

      case NVRUIM_LBS_XTRA_URL_LEN_TAG_81:
        /* in case when url tag is 
		0x80 (TAF)
		0x81 0x80 (Length Information)*/
        length_value = tl_data_buffer[2];
        ef_offset = ef_offset + 0x03;/* 2 bytes for length and 1 for tag */
        break;

      default:
        length_value = tl_data_buffer[1];
        ef_offset = ef_offset + 0x02;/* 1 byte for length and 1 for tag */
    }/* End of  switch (tl_data_buffer[1])*/

    if (url_occurance == 0x01) 
    {
      /* No jump require we got the url tag and now we only need to read the url 
      information (Value part of the TLV) from file  */
      if (length_value > NVRUIM_MAX_LBS_XTRA_SERVER_URL_LENGTH ) 
      {
        /* MAXIMUM only 128 URL Lenght is supported so read only 128 byte data*/
        length_value = NVRUIM_MAX_LBS_XTRA_SERVER_URL_LENGTH;
      }
	  if (length_value > 0x00) 
	  {
	    nvruim_read_item(v_data_buffer,length_value,ef_offset,UIM_CDMA_LBS_XSURL);
		/* No need to check the status of the reading, because after
		  this step uim_occuranc will become 0x00 and loop will be Terminate */
	  }/*End of if (length_value > 0x00) */
    }/*End of IF (url_occurance == 0x01)*/
    else
    {
      /* Need to jump for next URL  */
	  ef_offset= ef_offset + length_value;
      /* Intialization the TL value, It is required to break the loop, 
	  if the file does not have the data */
      tl_data_buffer[0] = 0xFF;
      tl_data_buffer[1] = 0x00;
      tl_data_buffer[2] = 0x00;
      tl_data_buffer[3] = 0x00;
      nvruim_read_item(tl_data_buffer,0x04,ef_offset,UIM_CDMA_LBS_XSURL);
      /* No need to check the status of the reading, 
	  for example if the file does not have any data then the 0xFF tag value
	  will break the loop in next step. */
    }/*end of else */
    /* Decrement the url_occurance by one  */
    url_occurance--;
  }/* End of  while(url_occurance)  */
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}/* End of nvruim_read_lbs_xtra_server_urls */


/*===========================================================================

FUNCTION NVRUIM_READ_LBS_XTRA_CONFIG 

DESCRIPTION
Check to see if the LBS XTRA Configuration File Data Items are supported by the
R-UIM. If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_lbs_xtra_config(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = FALSE;
  /*   Checking for Null Value   */
  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_lbs_xtra_config",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  /* Checking of retre configuration, RTRE should initialized  */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /*  Checking of Boolean nvruim_lbs_support, it indicate whether n24 is 
  is supported or not 
  Checking the RTRE Mode whether it is NV Mode or not  */
  if((!nvruim_lbs_support) ||(NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control()))
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
 /* Preparing command to read data form the file */
  cmd.access_uim.num_bytes  = NVRUIM_MAX_LBS_XTRA_CONFIG_SIZE;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_lbs_xtra_config_ef_buf );
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_LBS_XCONFIG;
  support_status            = nvruim_read_and_check_rsp(nv_cmd_ptr, 
														cache_bit, 
														op_status,
														&ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  switch(nv_cmd_ptr->item)
  {
    /* LBS Xtra Configuration File
    Bytes	    Description	                        M/O	    Length
    1	        XTRA Flags	                        M	    1 byte
    2	        gpsOneXTRA Download Interval	    M	    1 byte
    3	        gpsOneXTRA Download Retries 	    M	    1 byte
    4	        gpsOneXTRA Download Retry Interval	M	    1 byte
    XTRA Flags  Coding (0 - disabled; 1 - enabled):
    Bit	    Parameter Indicated
    1	    Allow gpsOneXTRA    [Default Value: 1 - enabled]
    2	    Allow gpsOneXTRA Automatic Download [Default Value: 0 - disabled]
    3-8	Reserved for future use

    gpsOneXTRA Download Interval coding :8-bit integer. Range: 1 to 168 
    (168 hours is a week).[Default Value:24 hours]

    gpsOneXTRA Download Retries - Coding: 8-bit integer. Range: 0 to 10.
    [Default Values: 3 retries]
	
    gpsOneXTRA Download Retry Interval - Coding: 8-bit integer. Range: 1 to 120.
    [Default Value: 10 minutes]
    */     
    case  NV_GPS1_XTRA_ENABLED_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 1st bit of 1byte (0th offset) of EF LBSXTRA Buffer */
        nv_cmd_ptr->data_ptr->gps1_xtra_enabled =  
            (nvruim_lbs_xtra_config_ef_buf[0] & 0x01)? TRUE : FALSE;
      }/* end of if (NV_DONE_S == *op_status)*/
      break;

    case  NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 2nd bit of 1byte (0th offset) of EF LBSXTRA Buffer */
        nv_cmd_ptr->data_ptr->gps1_xtra_auto_download_enabled = 
                  (nvruim_lbs_xtra_config_ef_buf[0] & 0x02)? TRUE : FALSE;
        
      }/* end of if (NV_DONE_S == *op_status)*/
      break;

    case  NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I :
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 2byte (1st offset) of EF LBSXTRA Buffer */
        if (nvruim_lbs_xtra_config_ef_buf[1] > MAX_VALUE_NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I) 
        {
          nv_cmd_ptr->data_ptr->gps1_xtra_download_interval = MAX_VALUE_NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I;
        }
        else
        {
          nv_cmd_ptr->data_ptr->gps1_xtra_download_interval = nvruim_lbs_xtra_config_ef_buf[1];
        }
      }/* end of if (NV_DONE_S == *op_status)*/
      break;

    case  NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 3byte (2nd offset) of EF LBSXTRA Buffer */
        if(nvruim_lbs_xtra_config_ef_buf[2] > MAX_VALUE_NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I)
        {
          nv_cmd_ptr->data_ptr->gps1_xtra_num_download_attempts = MAX_VALUE_NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I;
        }
        else
        {
          nv_cmd_ptr->data_ptr->gps1_xtra_num_download_attempts = nvruim_lbs_xtra_config_ef_buf[2];
        }
      }/* end of if (NV_DONE_S == *op_status)*/
      break;

    case  NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 4byte (3rd offset) of EF LBSXTRA Buffer */
        if(nvruim_lbs_xtra_config_ef_buf[3] > MAX_VALUE_NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I)
        {
          nv_cmd_ptr->data_ptr->gps1_xtra_time_between_attempts = MAX_VALUE_NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I;
        }
        else
        {
          nv_cmd_ptr->data_ptr->gps1_xtra_time_between_attempts = nvruim_lbs_xtra_config_ef_buf[3];
        }
      }/* end of if (NV_DONE_S == *op_status)*/
      break;

    default:
      MSG_ERROR("NV Item not found in nvruim_read_lbs_xtra_config 0x%x",
			    nv_cmd_ptr->item,0,0);
	 break;
  }/* end of switch(nv_cmd_ptr->item) */
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}  /* nvruim_read_lbs_xtra_config */


/*===========================================================================

FUNCTION NVRUIM_READ_LBS_V2_CONFIG 

DESCRIPTION
Check to see if the LBS V2 Configuration File Data Items are supported by the
R-UIM. If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    : Item is not supported
NV_RUIM_SUPPORTS_ITEM         : Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_lbs_v2_config(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = FALSE;
  /*   Checking for Null Value   */
  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_lbs_v2_config ",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
 /* Checking of retre configuration, RTRE should initialized  */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /*  Checking of Boolean nvruim_lbs_support, it indicate whether n24 is 
  is supported or not 
  Checking the RTRE Mode whether it is NV Mode or not  */
  if((!nvruim_lbs_support) ||(NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control()))
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
 /* Preparing command to read data form the file */
  cmd.access_uim.num_bytes  = NVRUIM_MAX_LBS_V2_CONFIG_SIZE ;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_lbs_v2_config_ef_buf );
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_LBS_V2CONFIG;
  support_status            = nvruim_read_and_check_rsp(nv_cmd_ptr, 
														cache_bit, 
														op_status,
														&ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  switch(nv_cmd_ptr->item)
  {
    /* Data Generic Configuration File
    Bytes	Description	                    M/O	        Length
    1	    V2 LBS Flags	                M	        1 byte
    "	V2 LBS Flags  Coding (0 - disabled; 1 - enabled): 
    Bit	    Parameter Indicated
    1	    Allow Sending System Parameter Info Message [Default Value: 0 - disabled]
    2	    Allow gpsOne Seed Position Use [Default Value: 1 - enabled]
    3	    Allow gpsOne Dynamic Mode [Default Value: 1 - enabled]
    4-8	Reserved for future use
    */     
    case  NV_GPS1_CAPABILITIES_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 1st bit of 1byte (0th offset) of EF LBSV2 Buffer */
        nv_cmd_ptr->data_ptr->gps1_capabilities = nvruim_lbs_v2_config_ef_buf[0]& 0x01;
      }
      break;

    case  NV_GPS1_SEEDPOS_OPTION_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 2nd bit of 1byte (0th offset) of EF LBSV2 Buffer */
        nv_cmd_ptr->data_ptr->gps1_seedpos_option = 
        (nvruim_lbs_v2_config_ef_buf[0] >> NVRUIM_LBS_FLAG_GPS1_SEED_POSITION_SHIFT)& 0x01;
      }
      break;

    case  NV_GPS1_DYNAMIC_MODE_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 3rd bit of 1byte (0th offset) of EF LBSV2 Buffer */
        nv_cmd_ptr->data_ptr->gps1_dynamic_mode = 
        (nvruim_lbs_v2_config_ef_buf[0] >>NVRUIM_LBS_FLAG_GPS1_DYNAMIC_MODE_SHIFT )& 0x01;
      }
      break;

    default:
	  MSG_ERROR("NV Item not found in nvruim_read_lbs_v2_config 0x%x",
				nv_cmd_ptr->item,0,0);
      break;
  }/* end of switch(nv_cmd_ptr->item) */
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}  /* nvruim_read_lbs_v2_config */


/*===========================================================================

FUNCTION NVRUIM_READ_LBS_V2_PDE_ADDRESS 

DESCRIPTION
Check to see if the LBS V2 PDE AddressFile Data Items are supported by the
R-UIM. If it is supported, read it.

DEPENDENCIES
Non-reentrant
Assuming this is the Transparent File

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED   : Item is not supported
NV_RUIM_SUPPORTS_ITEM        : Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_lbs_v2_pde_adress(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = FALSE;
  uint8 lbs_pde_address_type = 0x00;
  uint8 lbs_pde_address_length = 0x00;
  uint8 port_offset;
  /*   Checking for Null Value   */
  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_lbs_v2_pde_adress",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  if (NULL == nv_cmd_ptr->data_ptr)
  {
    MSG_ERROR(" NV Item Data_ptr is null for NV Item 0x%x",nv_cmd_ptr->item,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }	
  /* Checking of retre configuration, RTRE should initialized  */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /*  Checking of Boolean nvruim_lbs_support, it indicate whether n24 is 
  is supported or not 
  Checking the RTRE Mode whether it is NV Mode or not  */
  if((!nvruim_lbs_support) ||(NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control()))
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
  /* Data Structure of the File
    Bytes            Description                    M/O        Length
    1                V2 LBS PDE Address Type        M          1 byte
                                                               1: Domain Name
                                                               2: IPV4 Address
                                                               3: IPV6 Address
    2                V2 LBS PDE Address Length      M          1 byte
    3 to X+2         V2 LBS PDE Address Information M          X bytes
    X+3 to X+4       V2 LBS PDE Port Number         M          2 bytes */

  /* Preparing command to read entire data form the file */  
  cmd.access_uim.num_bytes     = NVRUIM_MAX_LBS_V2_PDE_ADDRESS_SIZE;
  cmd.access_uim.data_ptr      = (byte *)&(nvruim_lbs_v2_pde_address_buf);
  cmd.access_uim.offset        = 0; 
  cmd.access_uim.num_bytes_rsp = 0;
  cmd.access_uim.item          = UIM_CDMA_LBS_V2PDEADDR;
  support_status               = nvruim_read_and_check_rsp(nv_cmd_ptr, 
                                                           cache_bit, 
                                                           op_status,
                                                           &ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }
  /* Reading address type infromation from 0th offset of ef buffer */
  lbs_pde_address_type = nvruim_lbs_v2_pde_address_buf[0x00];

  /* Reading address length infromation from 1st offset of ef buffer */
  lbs_pde_address_length = nvruim_lbs_v2_pde_address_buf[0x01];

  /* Update the port offset */
  port_offset =  lbs_pde_address_length + 2;

  switch(nv_cmd_ptr->item)
  {
    case NV_GPS1_PDE_ADDRESS_I:
      if(NV_DONE_S == *op_status)
      {
        /*If this is requested, the value from the card should be returned 
          if the address type is ?2? (IPV4).  
          If the address type is something else, we should fill this with 0’s.  
          The Address length should not be more than 15 bytes.
        */
        if (lbs_pde_address_length > NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH) 
        {
          /* We are only supporting 4 byte information and ignoring rest of data */
          lbs_pde_address_length = NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH;
        }
       
        if(lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV4)
        {
          /* nvruim_lbs_v2_pde_address_buf[2] is pointing to the data */
          nv_cmd_ptr->data_ptr->gps1_pde_address = 
          nvruim_get_ipv4_digit_char_string(&nvruim_lbs_v2_pde_address_buf[2],
                                            lbs_pde_address_length);
        }/* End of If (lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV4) */
      }/* end of  if(NV_DONE_S == *op_status)*/
      break;

    case NV_GPS1_PDE_PORT_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading 2nd and 3 byte of the EF LBSV2 PDE ADDRESSS Buffer */
        nv_cmd_ptr->data_ptr->gps1_pde_port = 
			                       nvruim_lbs_v2_pde_address_buf[port_offset];
        /* Left Sift to the higher byte */
        nv_cmd_ptr->data_ptr->gps1_pde_port <<= 0x08;
        /* Adding 3 byte of the the EF LBSV2 PDE ADDRESSS Buffer */
        nv_cmd_ptr->data_ptr->gps1_pde_port |= 
			                nvruim_lbs_v2_pde_address_buf[port_offset + 0x01];
      }
      break;

    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
      if(NV_DONE_S == *op_status)
      {
        /*If this is requested, the value from the card should be returned 
          if the address type is ?2? (IPV4).  
          If the address type is something else, we should fill this with 0’s.  
          The Address length should not be more than 15 bytes.      */
        if (lbs_pde_address_length > NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH) 
        {
          /* We are only supporting 15 byte information and ignoring rest of data */
          lbs_pde_address_length = NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH;
        }
        
        if(lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV4)
        {
          /* nvruim_lbs_v2_pde_address_buf[2] is pointing to the data */		
          nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_ipv4 = 		
          nvruim_get_ipv4_digit_char_string(&nvruim_lbs_v2_pde_address_buf[2],
                                            lbs_pde_address_length);
        }/* End of If (lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV4) */
      }/*end of if(NV_DONE_S == *op_status)*/
      break;

    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV6_I:
      if(NV_DONE_S == *op_status)
      {
        /* If this is requested, the value from the card should be returned 
           if the address type is ?3? (IPV6).  
           If the address type is something else, we should fill this with 0’s.
           The Address length should not be more than 39 bytes */
        if (lbs_pde_address_length > NVRUIM_MAX_LBS_IPV6_ADDRESS_LENGTH) 
        {
           /* We are only supporting 39 byte information and ignoring rest data */
           lbs_pde_address_length = NVRUIM_MAX_LBS_IPV6_ADDRESS_LENGTH;
        }
	
        if(lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV6)
        {
          /* 3 offset is pointing to the data */		
          if(FALSE == 
             nvruim_get_ipv6_hex_char_string (&nvruim_lbs_v2_pde_address_buf[2],
               lbs_pde_address_length,
               (uint16 *)(&nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_ipv6[0]))
            )
          {
            MSG_ERROR("Invalid IPv6 address",0,0,0);
            memset((uint16 *)(nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_ipv6),
                   0x0000,sizeof(nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_ipv6));
          }		  
        }/* End of if((lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV6) */
      }/* end of  if(NV_DONE_S == *op_status)*/
      break;

    case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:
      if(NV_DONE_S == *op_status)
      {
        /* If this is requested, the value from the card should be returned 
        if the address type is ?1? (Domain Name).  
        If the address type is something else, we should just insert a NULL character. 
        The address length can be anything, as long as it is less than 128 bytes.  
        It should be null terminated. */
        memset((uint8 *)(nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_url),
                0x00, sizeof(nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_url));
        if (lbs_pde_address_length > NVRUIM_MAX_LBS_DOMAIN_ADDRESS_LENGTH) 
        {
          /* We are only supporting 128 byte information and ignoring 
             rest of data */
          lbs_pde_address_length = NVRUIM_MAX_LBS_DOMAIN_ADDRESS_LENGTH;
        }
        if(lbs_pde_address_type == NVRUIM_LBS_ADDRESS_TYPE_DOMAIN_NAME)
        {
          /* copy the ef buffer data to mpc server url */  
          memcpy((uint8 *)(nv_cmd_ptr->data_ptr->cgps_1x_pde_server_addr_url),
                 (uint8 *)(&nvruim_lbs_v2_pde_address_buf[2]),
                 lbs_pde_address_length);
        }/* end of if((lbs_pde_address_type == 
                       NVRUIM_LBS_ADDRESS_TYPE_DOMAIN_NAME)*/
      }/* end of if(NV_DONE_S == *op_status) */
      break;

    case NV_CGPS_1X_PDE_SERVER_PORT_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading 2nd and 3 byte of the LBSV2 PDE ADDRESSS Buffer */
        nv_cmd_ptr->data_ptr->cgps_1x_pde_server_port = 
        nvruim_lbs_v2_pde_address_buf[port_offset];
        /* Left Sift to the higher byte */
        nv_cmd_ptr->data_ptr->cgps_1x_pde_server_port <<= 0x08;
        /* Adding 3 byte of the the EF LBSV2 PDE ADDRESSS Buffer */
        nv_cmd_ptr->data_ptr->cgps_1x_pde_server_port |= 
        nvruim_lbs_v2_pde_address_buf[port_offset + 0x01];
      }
      break;

    default:
      MSG_ERROR("NV Item not found in  nvruim_read_lbs_v2_pde_adress 0x%x",
                nv_cmd_ptr->item,0,0);
      break;
   }/* end of switch(nv_cmd_ptr->item) */

  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}/* end of nvruim_read_lbs_v2_pde_adress */


/*===========================================================================

FUNCTION NVRUIM_READ_LBS_V2_MPC_ADDRESS 

DESCRIPTION
Check to see if the LBS V2 MPC AddressFile Data Items are supported by the
R-UIM. If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    : Item is not supported
NV_RUIM_SUPPORTS_ITEM         : Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_lbs_v2_mpc_adress(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed    = FALSE;
  uint8 lbs_mpc_address_type   = 0x00;
  uint8 lbs_mpc_address_length = 0x00;
  uint8 port_offset;

  /*   Checking for Null Value   */
  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_lbs_v2_mpc_adress ",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  if (NULL == nv_cmd_ptr->data_ptr)
  {
    MSG_ERROR(" NV Item Data_ptr is null for NV Item 0x%x",nv_cmd_ptr->item,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }  
  
 /* Checking of retre configuration, RTRE should initialized  */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
  /*  Checking of Boolean nvruim_lbs_support, it indicate whether n24 is 
  is supported or not 
  Checking the RTRE Mode whether it is NV Mode or not  */
  if((!nvruim_lbs_support) ||(NV_RTRE_CONTROL_NO_RUIM == nv_rtre_control()))
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
  /* Preparing command to read data form the file */
    /* Data Structure of the File
    Bytes                Description                        M/O       Length
    1                    V2 LBS MPC Address Type            M         1 byte
                                                                      1: Domain Name
                                                                      2: IPV4 Address
                                                                      3: IPV6 Address
    2                    V2 LBS MPC Address Length          M         1 byte
    3 to X+2             V2 LBS MPC Address Information     M         X bytes
    X+3 to X+4           V2 LBS MPC Port Number             M         2 bytes */
  /* Preparing command to read byte data form the file */
  cmd.access_uim.num_bytes     =  NVRUIM_MAX_LBS_V2_MPC_ADDRESS_SIZE;
  cmd.access_uim.data_ptr      = (byte *)&(nvruim_lbs_v2_mpc_address_buf);
  cmd.access_uim.offset        = 0; 
  cmd.access_uim.num_bytes_rsp = 0;
  cmd.access_uim.item          = UIM_CDMA_LBS_V2MPCADDR;
  support_status               = nvruim_read_and_check_rsp(nv_cmd_ptr, 
                                                           cache_bit, 
                                                           op_status,
                                                           &ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }
  /* Reading address type infromation from 0th offset of ef buffer */
  lbs_mpc_address_type = nvruim_lbs_v2_mpc_address_buf[0x00];
  /* Reading address length infromation from 1st offset of ef buffer */
  lbs_mpc_address_length = nvruim_lbs_v2_mpc_address_buf[0x01];
  /* Update the port offset */
  port_offset =  lbs_mpc_address_length + 2;

  switch(nv_cmd_ptr->item)
  {
    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
      if(NV_DONE_S == *op_status)
      {
        /* If these are requested, the value from the card should be returned 
           if the address type is ?2? (IPV4).  
           If the address type is something else, we should fill this with 0’s.
           The Address length should be not more than 15 bytes              */
        nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_ipv4 =0x00000000;
        if (lbs_mpc_address_length > NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH) 
        {
          /* We are only supporting 15 byte information and ignoring rest of data */
          lbs_mpc_address_length = NVRUIM_MAX_LBS_IPV4_ADDRESS_LENGTH;
        }
        if(lbs_mpc_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV4)
        {
          nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_ipv4 =		
          nvruim_get_ipv4_digit_char_string(&nvruim_lbs_v2_mpc_address_buf[2],
                                            lbs_mpc_address_length);
        }/* End of If (lbs_mpc_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV4) */
      }/* End of if(NV_DONE_S == *op_status) */
      break;

    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV6_I:
      if(NV_DONE_S == *op_status)
      {
        /* If these are requested, the value from the card should be returned 
           if the address type is ?3? (IPV6).  
           If the address type is something else, we should fill this with 0’s.
           The Address length should be not more than 39 bytes              */
        if (lbs_mpc_address_length > NVRUIM_MAX_LBS_IPV6_ADDRESS_LENGTH) 
        {
          /* We are only supporting 39 byte information and ignoring rest data */
          lbs_mpc_address_length = NVRUIM_MAX_LBS_IPV6_ADDRESS_LENGTH;
        }
        if(lbs_mpc_address_type == NVRUIM_LBS_ADDRESS_TYPE_IPV6)
        {
          if(FALSE == 
             nvruim_get_ipv6_hex_char_string (&nvruim_lbs_v2_mpc_address_buf[2],
               lbs_mpc_address_length,
               (uint16 *)(&nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_ipv6[0]))
            )
          {
            MSG_ERROR("Invalid IPv6 address",0,0,0);
            memset((uint16 *)(nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_ipv6),
                    0x0000, sizeof(nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_ipv6));
          }		  
       }/* End of If */
      }/* end of  if(NV_DONE_S == *op_status)*/
      break;

    case NV_CGPS_1X_MPC_SERVER_ADDR_URL_I:
      if(NV_DONE_S == *op_status)
      {
        /* If these are requested, the value from the card should be returned 
           if the address type is ?1? (Domain Name).  
           If the address type is something else, we should just insert a NULL character. 
           The address length can be anything, as long as it is less than 128 bytes.  
           It should be null terminated. */
        memset((uint8 *)(nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_url),
                0x00, sizeof(nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_url));
        if(lbs_mpc_address_length > NVRUIM_MAX_LBS_DOMAIN_ADDRESS_LENGTH) 
        {
          /* We are only supporting 128 byte information and ignoring rest of data */
          lbs_mpc_address_length = NVRUIM_MAX_LBS_DOMAIN_ADDRESS_LENGTH;
        }
        if(lbs_mpc_address_type == NVRUIM_LBS_ADDRESS_TYPE_DOMAIN_NAME)
        {
          /* copy the ef buffer data to mpc server url */  
          memcpy((uint8 *)(nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_addr_url),
                 (uint8 *)(&nvruim_lbs_v2_mpc_address_buf[2]),
                 lbs_mpc_address_length);
       
        }/* end of if((lbs_mpc_address_type == 
                       NVRUIM_LBS_ADDRESS_TYPE_DOMAIN_NAME)*/
      }/* end of if(NV_DONE_S == *op_status) */
      break;

    case NV_CGPS_1X_MPC_SERVER_PORT_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading 2nd and 3 byte of the F LBSV2 PDE ADDRESSS Buffer */
        nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_port = 
                    nvruim_lbs_v2_mpc_address_buf[port_offset];
        /* Left Sift to the higher byte */
        nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_port <<= 0x08;
        /* Adding 3 byte of the the EF LBSV2 PDE ADDRESSS Buffer */
        nv_cmd_ptr->data_ptr->cgps_1x_mpc_server_port |= 
                    nvruim_lbs_v2_mpc_address_buf[port_offset + 0x01];
      }
      break;

    default:
      MSG_ERROR("NV Item not found in  nvruim_read_lbs_v2_mpc_adress 0x%x",
                 nv_cmd_ptr->item,0,0);
      break;
   }/* end of switch(nv_cmd_ptr->item) */
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}  /* nvruim_read_lbs_v2_mpc_adress */

#endif /* FEATURE_UIM_SUPPORT_LBS  */


#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
/*===========================================================================

FUNCTION NVRUIM_READ_3GPD_DATA_GENERIC_CONFIG

DESCRIPTION
Check to see if the 3GPD Data Generic CONFIG items are supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_data_generic_config(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  nv_ruim_support_status support_status;
  boolean ef_data_refreshed = FALSE;
  /*   Checking for Null Value   */
  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_3gpd_data_generic_config ",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  /* Checking for Nvruim_3gpd_control Variable, 
  It determines whether support for 3GPD should come from the R-UIM or from NV. */

  if (UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
  /*  Checking of Boolean nvruim_3gpd_ext_support,
    it indicate whether n15 is supported or not   */
  if(!nvruim_data_3gpd_ext_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }
 /* Checking of retre configuration, RTRE should initialized  */
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }
 /* Preparing command to read data form the file */
  cmd.access_uim.num_bytes  = NVRUIM_MAX_DGC_SIZE;
  cmd.access_uim.data_ptr   = (byte *) &(nvruim_3gpd_dcg_ef_buf);
  cmd.access_uim.offset     = 0;
  cmd.access_uim.item       = UIM_CDMA_3GPD_DGC;
  support_status            = nvruim_read_and_check_rsp(nv_cmd_ptr, cache_bit, op_status,&ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  switch(nv_cmd_ptr->item)
  {
    /* Data Generic Configuration File
    Byte Index  Description
    1           Data Dormant Timer
    2           EPZID Type Information
    3           Hysterisis Activation Timer
    */     
    case NV_DSAT707_CTA_TIMER_I:
	  if(NV_DONE_S == *op_status)
	  {
	    /* Reading the 3rd byte (2nd offset) of the EF DGC */
	    nv_cmd_ptr->data_ptr->dsat707_cta_timer = nvruim_3gpd_dcg_ef_buf[0];
	  }
	  break;
    case  NV_ACTIVE_EPZID_TYPE_I:
	  if(NV_DONE_S == *op_status)
	  {
	    /*  Reading the 2nd byte (1 offset) of the EF DGC  */
	    nv_cmd_ptr->data_ptr->active_epzid_type = nvruim_3gpd_dcg_ef_buf[1];
	  }
	  break;
    case  NV_HYSTERISIS_ACTIVATION_TIMER_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reading the 3rd byte (2nd offset) of the EF DGC */
        nv_cmd_ptr->data_ptr->hysterisis_activation_timer = nvruim_3gpd_dcg_ef_buf[2];
      }
      break;
  }/* end of switch(nv_cmd_ptr->item) */

  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}  /* nvruim_read_3gpd_data_generic_config */
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS  */


/*===========================================================================

FUNCTION NVRUIM_READ_3GPD_MIPSP

DESCRIPTION
Check to see if the item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_mipsp(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  nv_ruim_support_status support_status;
  boolean  dummy;

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_mip_svc)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_mipsp_ef_buf);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_3GPD_MIPSP;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_MIPSP_EF_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }
  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->ds_mip_active_prof = (nvruim_mipsp_ef_buf) >> NVRUIM_MIPSP_SHIFT;
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}


#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
/*===========================================================================

FUNCTION NVRUIM_READ_3GPD_SIPSP

DESCRIPTION
Check to see if the item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_sipsp(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  nv_ruim_support_status support_status;
  boolean  dummy;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("NULL parameter in nvruim_read_3gpd_sipsp!",0,0,0);
    return(NV_RUIM_SUPPORTS_ITEM);
  }

  if(UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }
#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(!nvruim_3gpd_sip_svc)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_sipsp_ef_buf);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item      = UIM_CDMA_3GPD_SIPSP;

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SIPSP_EF_CACHE_BIT,op_status,&dummy);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }
  if(NV_DONE_S == *op_status)
  {
    nv_cmd_ptr->data_ptr->ds_sip_active_profile_index = (nvruim_sipsp_ef_buf) >> NVRUIM_SIPSP_SHIFT;
  }
  nv_cmd_ptr->status = *op_status;
  return(NV_RUIM_SUPPORTS_ITEM);
}
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */


/*===========================================================================

FUNCTION NVRUIM_READ_3GPDSIPPAPSS

DESCRIPTION
Check to see if the 3GPDSIPPAPSS item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_3gpd_sippapss(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  nv_ruim_support_status support_status;
  word src_bit_count;
  byte num_nai;
  byte nai_count;
  boolean ef_data_refreshed = TRUE;
  byte profile_number;
  byte ss_length=0;
  byte ss_length_shortened=0;
  word i;

  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_RUIM_SIP_NV==uim_3gpd_support())
  {
    return(NV_RUIM_ITEM_NOT_SUPPORTED);  
  }

  /* Return the NV value for NV_PPP_USER_ID_I and NV_PPP_PASSWORD_I
     for OMH card */
  if((nv_cmd_ptr->item == NV_PPP_PASSWORD_I) && 
     (TRUE == nvruim_data_3gpd_ext_support()))
  {
    MSG_HIGH("Returning the NV value for NV_PPP_PASSWORD_I for OMH card",0,0,0);
    return(NV_RUIM_ITEM_NOT_SUPPORTED);
  }

  if(nv_cmd_ptr->item == NV_PPP_PASSWORD_I)
  {
    nv_cmd_ptr->data_ptr->ppp_password.password_len = 0;
  }
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
  else if(nv_cmd_ptr->item == NV_DS_SIP_PPP_SS_INFO_I)
  {
    nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss_length = 0;
  }
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */

#ifndef FEATURE_UIM_3GPD_FALLBACK
  if(nvruim_3gpd_sip_svc == FALSE)
  {
    *op_status = NV_DONE_S;
    return(NV_RUIM_SUPPORTS_ITEM);
  }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  cmd.access_uim.num_bytes = NVRUIM_MAX_SIPPAPSS_SIZE;
  cmd.access_uim.data_ptr  = (byte *) &(nvruim_sippapss_ef_buf);
  cmd.access_uim.offset    = 0;
  cmd.access_uim.item= UIM_CDMA_3GPD_SIPPAPSS;
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SIPPAPSS_EF_CACHE_BIT,op_status,&ef_data_refreshed);
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return(support_status);
  }

  if(NV_DONE_S == *op_status)
  {
    if(ef_data_refreshed)
    {

      /* The src counter is a "bit pointer" used as a cursor in the buffer */
      src_bit_count = FSIZ(nvruim_sippapss_ef_fields, length_of_block_field) +
                      FSIZ(nvruim_sippapss_ef_fields, num_nai_field); /* Skip to first profile */
      nai_count = 0;
      /* The number of NAIs is specified in bits 9-12 of the file */
      num_nai = (nvruim_sippapss_ef_buf[1] & NVRUIM_SIPPAPSS_NUM_NAI_MASK) >> 4;

      /* Clear out the buffer containing ptrs to SIP Profiles */
      for(i = 0; i < NVRUIM_MAX_NUM_SIP_PROFILES; i++)
      {
        nvruim_sip_pap_ss_profile_bit_index[i] = NVRUIM_NO_PROFILE_INDEX_SET;
      } /* end for */

      while(nai_count < num_nai)
      {
        profile_number = b_unpackb(nvruim_sippapss_ef_buf, src_bit_count, 
                                   FSIZ(nvruim_sippapss_ef_fields, profile_num_field));

        nvruim_sip_pap_ss_profile_bit_index[profile_number] = src_bit_count;
        src_bit_count += FSIZ(nvruim_sippapss_ef_fields, profile_num_field);
        ss_length = b_unpackb(nvruim_sippapss_ef_buf, src_bit_count,
                              FSIZ(nvruim_sippapss_ef_fields, ss_length_field));
        src_bit_count += FSIZ(nvruim_sippapss_ef_fields, ss_length_field)
                      + ss_length *8;
        nai_count++;
      } /* end while */
    } /* end if - data was refreshed */
    if((nv_cmd_ptr->item == NV_PPP_PASSWORD_I) && 
       nvruim_sip_pap_ss_profile_bit_index[nvruim_preferred_sip_profile] 
                                    != NVRUIM_NO_PROFILE_INDEX_SET)   
    {
      src_bit_count = nvruim_sip_pap_ss_profile_bit_index[nvruim_preferred_sip_profile];
      src_bit_count += FSIZ(nvruim_sippapss_ef_fields, profile_num_field);
      ss_length = b_unpackb(nvruim_sippapss_ef_buf, src_bit_count,
                              FSIZ(nvruim_sippapss_ef_fields, ss_length_field));
      src_bit_count += FSIZ(nvruim_sippapss_ef_fields, ss_length_field);
      if(ss_length > NVRUIM_MAX_SIPPAPSS_SS_SIZE)
      {
         ss_length_shortened = NVRUIM_MAX_SIPPAPSS_SS_SIZE;
      }
      else
      {
         ss_length_shortened = ss_length;
      }
      memset((byte *)nv_cmd_ptr->data_ptr->ppp_password.password, 
              0x00, sizeof(nv_cmd_ptr->data_ptr->ppp_password.password));
      nv_cmd_ptr->data_ptr->ppp_password.password_len = ss_length_shortened;

      b_copy((void*) nvruim_sippapss_ef_buf, src_bit_count, 
               (void*) nv_cmd_ptr->data_ptr->ppp_password.password, 0, 
               ss_length_shortened * 8);   
    } /* end if - there was valid data in the buffer */
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    else if((nv_cmd_ptr->item == NV_DS_SIP_PPP_SS_INFO_I) && 
       nvruim_sip_pap_ss_profile_bit_index
       [nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.index] != NVRUIM_NO_PROFILE_INDEX_SET)
    {
       src_bit_count = nvruim_sip_pap_ss_profile_bit_index
           [nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.index]; 
       src_bit_count += FSIZ(nvruim_sippapss_ef_fields, profile_num_field);
       ss_length = b_unpackb(nvruim_sippapss_ef_buf, src_bit_count,
                             FSIZ(nvruim_sippapss_ef_fields, ss_length_field));
       if(ss_length > NVRUIM_MAX_SIPPAPSS_SS_SIZE)
       {
          ss_length_shortened = NVRUIM_MAX_SIPPAPSS_SS_SIZE;
       }
       else
       {
          ss_length_shortened = ss_length;
       }
       src_bit_count += FSIZ(nvruim_sippapss_ef_fields, ss_length_field);
       memset((byte *)nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss, 0x00,
              sizeof(nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss));		       
       nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss_length = ss_length_shortened;
       b_copy((void*) nvruim_sippapss_ef_buf, src_bit_count, 
               (void*) nv_cmd_ptr->data_ptr->ds_sip_ppp_ss_info.ss, 0, 
               ss_length_shortened * 8);          
    }
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
    else
    {
      /* Something was wrong - the profile was not found */
      MSG_HIGH("Profile not found or invalid NV item in SIPPAPSS",0,0,0);
      *op_status = NV_NOTACTIVE_S;
    } /* end else */
  } /* end if - NV_DONE_S == *op_status */

  nv_cmd_ptr->status = *op_status;

  return(NV_RUIM_SUPPORTS_ITEM);
}

#endif /*FEATURE_UIM_SUPPORT_3GPD*/

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
/*===========================================================================

FUNCTION NVRUIM_READ_HRPD_HRPDUPP

DESCRIPTION
Check to see if the HRPDUPP item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_hrpd_hrpdupp(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  nv_ruim_support_status support_status;
  boolean dummy = TRUE;
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  dword min1;
  word min2;
  byte mnc;
  word mcc;
#endif /* FEATURE_UIM_CAVE_AN_AUTH */
  uint32 cache_bit = RUIM_NO_CACHE_VALUE;

  if (nvruim_hrpd_cdma_svc && 
       (!(nvruim_cache_control & RUIM_HRPD_NAI_CACHE_BIT)))
  {
    if (uim_hrpd_disabled_card())
    {
      MSG_HIGH("HRPD disabled card",0,0,0);
      uim_init_an_hrpd_preference(FALSE);
      nvruim_hrpd_cdma_svc=TRUE;
    }
  }

  if(UIM_AN_HRPD_SUPPORT == uim_an_hrpd_support())
  {
   #ifndef FEATURE_UIM_AN_HRPD_FALLBACK
    if(!nvruim_hrpd_cdma_svc)
    {
      if(nv_cmd_ptr->item == NV_HDR_AN_AUTH_USER_ID_LONG_I)
      {
        nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id_len = 0; 
      }
      else
      {
        nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai_len = 0;
      }
      *op_status = NV_DONE_S;
      return NV_RUIM_SUPPORTS_ITEM;
    }
    #endif /* FEATURE_UIM_AN_HRPD_FALLBACK */

    cmd.access_uim.num_bytes = NVRUIM_MAX_HRPDUPP_SIZE;
    cmd.access_uim.data_ptr = (byte *) &(nvruim_hrpdupp_ef_buf);
    cmd.access_uim.offset   = 0;
    cmd.access_uim.item     = UIM_CDMA_HRPD_HRPDUPP;
    cache_bit               = (uint32) RUIM_HRPD_NAI_CACHE_BIT; 
  }
  else if(UIM_AN_HRPD_NO_SUPPORT == uim_an_hrpd_support())
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  else
  { /* --------------------------------------------------------------
       Use Cave 
      If AN Cave Authentication is used (for China Unicom),
      the HDR NAI/User ID should be constucted as <IMSI>@unicomcdma.com.cn
      The following case for IMSI read also applies for AN CAVE NAI read
      Setup the command structure to read the IMSI_M EF from the RUIM 
      ----------------------------------------------------------------*/
    cmd.access_uim.num_bytes = sizeof(nvruim_imsi_type);
    cmd.access_uim.data_ptr  = (byte *) &(nvruim_imsi_m_cache_buf);
    cmd.access_uim.item= UIM_CDMA_IMSI_M;  
    /* EF offset  */
    cmd.access_uim.offset = 0;
    cache_bit = RUIM_IMSI_M_CACHE_BIT;
  }
#else /* AN CAVE is not defined */
  else
  {
    MSG_HIGH("Error is NVRUIM.  Unknown HRPD preference",0,0,0);
  }
#endif /* AN CAVE is not defined */

  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, cache_bit,op_status,&dummy);

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }

  switch(nv_cmd_ptr->item)
  {
    case NV_HDR_AN_AUTH_USER_ID_LONG_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reformat the data only if the read was successful. */
        if(UIM_AN_HRPD_SUPPORT == uim_an_hrpd_support())
        {
          if(nvruim_hrpdupp_ef_buf[1] > NV_MAX_AN_AUTH_USER_ID_LONG_LENGTH)
          {
            /* If the HRPD User Name is longer than the NV structure can take, truncate it */
            MSG_HIGH("AN Auth Username on card is too long!",0,0,0);
            nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id_len =
              NV_MAX_AN_AUTH_USER_ID_LONG_LENGTH;
            memcpy((byte*)nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id,
                 (byte*)&(nvruim_hrpdupp_ef_buf[2]), NV_MAX_AN_AUTH_USER_ID_LONG_LENGTH);
          }
          else
          {
            nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id_len =
            nvruim_hrpdupp_ef_buf[1];
            memcpy((byte*)nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id,
                 (byte*)&(nvruim_hrpdupp_ef_buf[2]), nvruim_hrpdupp_ef_buf[1]);
          }
        }
        else
        {
        #ifdef FEATURE_UIM_CAVE_AN_AUTH
        /* --------------------------------------------------------------------
           Use CAVE 
           Retrive MIN1, MIN2, MCC and MNC and build NAI string here 
           Copy the item from the cache buffer. 
           ------------------------------------------------------------------*/
          nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id_len = 
            NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH;
          min1 = (nvruim_imsi_m_cache_buf.imsi_s1[2] << 16) |
                 (nvruim_imsi_m_cache_buf.imsi_s1[1] << 8) |
                 (nvruim_imsi_m_cache_buf.imsi_s1[0]);

          min2 = (nvruim_imsi_m_cache_buf.imsi_s2[1] << 8) |
                  nvruim_imsi_m_cache_buf.imsi_s2[0];
          mnc = nvruim_imsi_m_cache_buf.imsi_11_12;
          mcc = (nvruim_imsi_m_cache_buf.imsi_mcc[1] << 8) |
                 nvruim_imsi_m_cache_buf.imsi_mcc[0];

         /* Copy generic China Unicom AN Cave NAI format:
            <000000000000000>@unicomcdma.com.cn */
          memcpy((byte*)nv_cmd_ptr->data_ptr->hdr_an_auth_user_id_long.user_id,
                 nvruim_an_cave_nai, NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH);

          if(!(nvruim_decode_min((byte*)(nv_cmd_ptr->data_ptr->
               hdr_an_auth_user_id_long.user_id), &mcc, &mnc, &min1, &min2)))
          {
            MSG_HIGH("NAI String Not Built Properly for AN CAVE Auth", 0, 0, 0);
            *op_status = NV_NOTACTIVE_S; 
          } /* The decode was unsuccessful */
        #else /* No Cave Support */
          MSG_HIGH("Unknown HRPD preference.",0,0,0);
        #endif /* No Cave Support */
        } /* end else */
      } /* end if - read successful */
      else
      {
        MSG_HIGH("NAI String Not Built Properly for AN CAVE Auth", 0, 0, 0);
        *op_status = NV_NOTACTIVE_S;
      }
      break;

    case NV_HDR_AN_AUTH_NAI_I:
      if(NV_DONE_S == *op_status)
      {
        /* Reformat the data only if the read was successful. */
        if(UIM_AN_HRPD_SUPPORT == uim_an_hrpd_support())
        {
          if(nvruim_hrpdupp_ef_buf[1] > NV_MAX_AN_AUTH_NAI_LENGTH)
          {
            nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai_len =
             NV_MAX_AN_AUTH_NAI_LENGTH;
            memcpy((byte*)nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai,
                 (byte*)&(nvruim_hrpdupp_ef_buf[2]), NV_MAX_AN_AUTH_NAI_LENGTH);
          }
          else
          {
            nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai_len =
               nvruim_hrpdupp_ef_buf[1];
            memcpy((byte*)nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai,
                   (byte*)&(nvruim_hrpdupp_ef_buf[2]), nvruim_hrpdupp_ef_buf[1]);
          }
        }
        else
        {
        #ifdef FEATURE_UIM_CAVE_AN_AUTH
        /*---------------------------------------------------------------------
         Retrive  MIN1, MIN2, MCC and MNC and build NAI string here   
         Copy the item from the cache buffer. 
         --------------------------------------------------------------------*/
          nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai_len = 
            NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH;
          min1 = (nvruim_imsi_m_cache_buf.imsi_s1[2] << 16) |
                 (nvruim_imsi_m_cache_buf.imsi_s1[1] << 8) |
                 (nvruim_imsi_m_cache_buf.imsi_s1[0]);

          min2 = (nvruim_imsi_m_cache_buf.imsi_s2[1] << 8) |
                  nvruim_imsi_m_cache_buf.imsi_s2[0];
          mnc = nvruim_imsi_m_cache_buf.imsi_11_12;
          mcc = (nvruim_imsi_m_cache_buf.imsi_mcc[1] << 8) |
                 nvruim_imsi_m_cache_buf.imsi_mcc[0];

         /* Copy generic China Unicom AN Cave NAI format:
            <000000000000000>@unicomcdma.com.cn */
          memcpy((byte*)nv_cmd_ptr->data_ptr->hdr_an_auth_nai.nai,
                 nvruim_an_cave_nai, NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH);

          if(!(nvruim_decode_min((byte*)(nv_cmd_ptr->data_ptr->
                 hdr_an_auth_nai.nai), &mcc, &mnc, &min1, &min2)))
          {
            MSG_HIGH("NAI String Not Built Properly for AN CAVE Auth", 0, 0, 0);
            *op_status = NV_NOTACTIVE_S;
          } /* The decode was unsuccessful */
        #else /* No Cave Support */
          MSG_HIGH("Unknown HRPD preference.",0,0,0);
        #endif /* No Cave Support */
        } /* end else */
      } /* end if - read successful */
      else
      {
        MSG_HIGH("NAI String Not Built Properly for AN CAVE Auth", 0, 0, 0);
        *op_status = NV_NOTACTIVE_S;
      }
    default:
      break;
  }

  nv_cmd_ptr->status = *op_status;
  return NV_RUIM_SUPPORTS_ITEM;
}
#endif /*FEATURE_UIM_SUPPORT_HRPD_AN*/


#ifdef FEATURE_UIM_EUIMID
/*===========================================================================
FUNCTION NVRUIM_READ_MEID

DESCRIPTION
Check to see if the MEID is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  Nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_meid(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
    
  if(nvruim_esn_usage != NV_RUIM_USE_SF_EUIMID &&
     nvruim_esn_usage != NV_RUIM_USE_SF_EUIMID_ESN 
    )
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
  
  if(!nvruim_check_rtre_config(nv_cmd_ptr,op_status))
  {
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* Setup the command structure to read the SF_RUIM_ID EF from the RUIM */
  cmd.access_uim.num_bytes = NVRUIM_SIZE(meid);
  cmd.access_uim.data_ptr  = (byte *) nvruim_sf_euim_id_cache_buf;
  cmd.access_uim.item      = UIM_CDMA_SF_EUIM_ID;  
  cmd.access_uim.offset    = 0; /* EF offset  */
  
  support_status = nvruim_read_and_check_rsp(nv_cmd_ptr, RUIM_SF_EUIMID_CACHE_BIT,op_status,&dummy);
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    return support_status;
  }
  
  if(nv_cmd_ptr->item == NV_MEID_I)
  {
    if(*op_status == NV_NOTACTIVE_S)
    {
      return NV_RUIM_ITEM_NOT_SUPPORTED;
    } /* end if - the read from the SF_EUIMD has failed. */
  
    /* Reformat the data only if the read was successful. */
    if(NV_DONE_S == *op_status)
    {
      /* Copy the MEID from the cache buffer */
      qw_equ(nv_cmd_ptr->data_ptr->meid, nvruim_sf_euim_id_cache_buf);
    } /* end if - the read was successful. */
  }
   
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;
}
#endif /*FEATURE_UIM_EUIMID*/


/*===========================================================================
FUNCTION nvruim_read_smscap

DESCRIPTION
  Check to see if SMS Capability is supported by the R-UIM.
  If it is supported, read it.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the read.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_smscap
(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status,  /* status of the I/O operation*/
  uint32            cache_bit
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_smscap",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
 
  if( nvruim_smscap_svc == FALSE )
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  /* Setup the command structure to read the EF from the RUIM */
  cmd.access_uim.num_bytes = NVRUIM_SMSCAP_SIZE;
  cmd.access_uim.data_ptr  = nvruim_smscap_ef_buf;
  cmd.access_uim.item      = UIM_CDMA_SMS_CAP;  
  cmd.access_uim.offset    = 0; /* EF offset  */
 
  support_status = nvruim_read_and_check_rsp( nv_cmd_ptr, cache_bit, op_status, & dummy );
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    nvruim_smscap_svc = FALSE;
    return support_status;
  }
 
  switch( nv_cmd_ptr->item )
  {
    case NV_SMS_MO_RETRY_PERIOD_I:
      nv_cmd_ptr->data_ptr->sms_mo_retry_period = nvruim_smscap_ef_buf[0];
      break;

    case NV_SMS_MO_RETRY_INTERVAL_I:
      nv_cmd_ptr->data_ptr->sms_mo_retry_interval = nvruim_smscap_ef_buf[1];
      break;
    case NV_SMS_MO_ON_ACCESS_CHANNEL_I:
      nv_cmd_ptr->data_ptr->sms_mo_on_access_channel = nvruim_smscap_ef_buf[2]
           & NVRUIM_SMSCAP_ON_ACCESS_CH_MASK;
      break;
    case NV_SMS_MO_ON_TRAFFIC_CHANNEL_I:
      if (nvruim_smscap_ef_buf[2] & NVRUIM_SMSCAP_ON_TRAFFIC_CH_MASK)
      {
        nv_cmd_ptr->data_ptr->sms_mo_on_traffic_channel = TRUE;
      }
      else
      {
        nv_cmd_ptr->data_ptr->sms_mo_on_traffic_channel = FALSE;
      }	
      break;
    case NV_SMS_SERVICE_OPTION_I:
      nv_cmd_ptr->data_ptr->sms_service_option = nvruim_smscap_ef_buf[3];
      break;
    default:
      MSG_ERROR("Invalid NV item for EF-SMSCAP: %d", nv_cmd_ptr->item,0,0);
      return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
   
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;

} /* nvruim_read_smscap() */


/*===========================================================================
FUNCTION nvruim_read_bcsms_config

DESCRIPTION
  Check to see if BC-SMS Config is supported by the R-UIM.
  If it is supported, read it.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the read.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_bcsms_config
(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  byte                   bcsms_config   = 0;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_bcsms_config",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
    
  if( nvruim_bcsms_svc == FALSE )
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  /* Setup the command structure to read the EF from the RUIM */
  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  = & bcsms_config;
  cmd.access_uim.item      = UIM_CDMA_BCSMS_CONFIG;  
  cmd.access_uim.offset    = 0; /* EF offset  */
 
  support_status = nvruim_read_and_check_rsp( nv_cmd_ptr, RUIM_NO_CACHE_VALUE, op_status, & dummy );
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    nvruim_bcsms_svc = FALSE;
    return support_status;
  }
  
  nv_cmd_ptr->data_ptr->sms_bc_config = bcsms_config;
   
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;

} /* nvruim_read_bcsms_config() */

/*===========================================================================
FUNCTION nvruim_read_bcsms_pref

DESCRIPTION
  Check to see if BC-SMS Pref is supported by the R-UIM.
  If it is supported, read it.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status_ptr indicates the success/failure of the read.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_bcsms_pref
(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  byte                   bcsms_pref     = 0;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_bcsms_pref",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
    
  if( nvruim_bcsms_svc == FALSE )
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  /* Setup the command structure to read the EF from the RUIM */
  cmd.access_uim.num_bytes = 1;
  cmd.access_uim.data_ptr  = & bcsms_pref;
  cmd.access_uim.item      = UIM_CDMA_BCSMS_PREF;  
  cmd.access_uim.offset    = 0; /* EF offset  */
  
  support_status = nvruim_read_and_check_rsp( nv_cmd_ptr, RUIM_NO_CACHE_VALUE, op_status, & dummy );
  
  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    nvruim_bcsms_svc = FALSE;
    return support_status;
  }
  
  nv_cmd_ptr->data_ptr->sms_bc_user_pref = bcsms_pref;
   
  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;

} /* nvruim_read_bcsms_pref() */


/*===========================================================================
FUNCTION nvruim_read_bcsms_table

DESCRIPTION
  Check to see if BC-SMS table items are supported by the R-UIM.
  If it is supported, read it.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status_ptr indicates the success/failure of the read.  For items
  which are "not supported", the value of op_statusis undefined.

SIDE EFFECTS
  nv_cmd_ptr will be modified with the op_status

===========================================================================*/
static nv_ruim_support_status nvruim_read_bcsms_table
(
  nv_cmd_type       *nv_cmd_ptr, /* command block    */
  nv_stat_enum_type *op_status   /* status of the I/O operation*/
)
{
  boolean dummy;
  nv_ruim_support_status support_status = NV_RUIM_SUPPORTS_ITEM;
  int     i = 0;

  if((nv_cmd_ptr == NULL) || (op_status == NULL))
  {
    MSG_ERROR("Null PTR in nvruim_read_bcsms_table",0,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
    
  if( nvruim_bcsms_svc == FALSE )
  {
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  /* Setup the command structure to read the EF from the RUIM */
  cmd.access_uim.access    = UIM_READ_F;  
  cmd.access_uim.num_bytes = NVRUIM_BCSMS_PARM_SIZE;
  cmd.access_uim.data_ptr  = nvruim_bcsms_parm_ef_buf;
  cmd.access_uim.item      = UIM_CDMA_BCSMS_PARAMS;  
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;

  if( nvruim_bcsms_table_num_entries_known == FALSE )
  {
    /* number of table entries unknown yet, count all records */
    cmd.access_uim.offset    = 1; /* record number */
    support_status = nvruim_read_and_check_rsp( nv_cmd_ptr, RUIM_NO_CACHE_VALUE, op_status, & dummy );
  
    if(support_status != NV_RUIM_SUPPORTS_ITEM)
    {
      nvruim_bcsms_svc = FALSE;
      return support_status;
    }

    for( i=2; i<=NVRUIM_MAX_BCSMS_TABLE_ENTRIES; i++ )
    {
      cmd.access_uim.offset = i; /* record number */
      nvruim_access(&cmd);
      if(nvruim_ruim_status_report == UIM_PASS)
      {
        continue;
      }
      else
      {
        break;
      }
    }
    nvruim_bcsms_table_num_entries_known = TRUE;
    nvruim_bcsms_table_num_entries       = i-1;
  }


  if( nv_cmd_ptr->item == NV_SMS_BC_SERVICE_TABLE_SIZE_I )
  {
    nv_cmd_ptr->data_ptr->sms_bc_service_table_size = nvruim_bcsms_table_num_entries;
    * op_status = NV_DONE_S;
    return NV_RUIM_SUPPORTS_ITEM;
  }

  /* Set up record number */
  if( nv_cmd_ptr->item == NV_SMS_BC_SERVICE_TABLE_I )
  {
    cmd.access_uim.offset = nv_cmd_ptr->data_ptr->sms_bc_service_table.index+1; /* record number */
  }
  else
  {
    cmd.access_uim.offset = nv_cmd_ptr->data_ptr->sms_bc_service_table_options.index+1; /* record number */
  }

  /* Read BCSMS_PARAMS first */
  support_status = nvruim_read_and_check_rsp( nv_cmd_ptr, RUIM_NO_CACHE_VALUE, op_status, & dummy );

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    nvruim_bcsms_svc = FALSE;
    return support_status;
  }

  /* Read BCSMS_TABLE next */
  /* Setup the command structure to read the BCSMS_TABLE from the RUIM */
  cmd.access_uim.access    = UIM_READ_F;  
  cmd.access_uim.num_bytes = NVRUIM_MAX_BCSMS_TABLE_ENTRY_SIZE;
  cmd.access_uim.data_ptr  = nvruim_bcsms_table_ef_buf;
  cmd.access_uim.item      = UIM_CDMA_BCSMS_TABLE;  
  cmd.access_uim.rec_mode  = UIM_ABSOLUTE;

  support_status = nvruim_read_and_check_rsp( nv_cmd_ptr, RUIM_NO_CACHE_VALUE, op_status, & dummy );

  if(support_status != NV_RUIM_SUPPORTS_ITEM)
  {
    nvruim_bcsms_svc = FALSE;
    return support_status;
  }

  /* Fill in NV read result from data retrieved from the card */
  if( nv_cmd_ptr->item == NV_SMS_BC_SERVICE_TABLE_I )
  {
    nv_cmd_ptr->data_ptr->sms_bc_service_table.active_service =
            nvruim_bcsms_table_ef_buf[0];
    nv_cmd_ptr->data_ptr->sms_bc_service_table.service =
            nvruim_bcsms_table_ef_buf[1] | nvruim_bcsms_table_ef_buf[2]<<8;
    nv_cmd_ptr->data_ptr->sms_bc_service_table.language =
            nvruim_bcsms_table_ef_buf[3];
    nv_cmd_ptr->data_ptr->sms_bc_service_table.selected =
            nvruim_bcsms_parm_ef_buf[0];
    nv_cmd_ptr->data_ptr->sms_bc_service_table.priority =
            nvruim_bcsms_parm_ef_buf[1];

    memset( (void*)nv_cmd_ptr->data_ptr->sms_bc_service_table.label,
            0,
            NV_SMS_BC_SRV_LABEL_SIZE);
    memcpy( (void*)nv_cmd_ptr->data_ptr->sms_bc_service_table.label,
            nvruim_bcsms_table_ef_buf+7,
            MIN(NV_SMS_BC_SRV_LABEL_SIZE,cmd.access_uim.num_bytes-7) );
  }
  else if( nv_cmd_ptr->item == NV_SMS_BC_SERVICE_TABLE_OPTIONS_I )
  {
    nv_cmd_ptr->data_ptr->sms_bc_service_table_options.max_messages =
            nvruim_bcsms_table_ef_buf[4];
    nv_cmd_ptr->data_ptr->sms_bc_service_table_options.bc_alert =
            nvruim_bcsms_table_ef_buf[5];
    nv_cmd_ptr->data_ptr->sms_bc_service_table_options.label_encoding =
            nvruim_bcsms_table_ef_buf[6];
  }
  else
  {
    MSG_ERROR("Invalid NV item for BCSMS: %d", nv_cmd_ptr->item,0,0);
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }

  nv_cmd_ptr->status = *op_status;

  return NV_RUIM_SUPPORTS_ITEM;

} /* nvruim_read_bcsms_table() */


/*===========================================================================

FUNCTION NVRUIM_READ

DESCRIPTION
Check to see if the item is supported by the R-UIM.
If it is supported, read it.

DEPENDENCIES
Non-reentrant

RETURN VALUE
NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
NV_RUIM_SUPPORTS_ITEM Item is supported, see op_status

op_status indicates the success/failure of the read.  For items
which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
None

===========================================================================*/
nv_ruim_support_status nvruim_read(
  nv_cmd_type       *nv_cmd_ptr,  /* command block    */
  nv_stat_enum_type *op_status    /* status of the I/O operation*/
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  uint32 cache_bit = RUIM_NO_CACHE_VALUE;
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
  nv_ruim_support_status nvruim_read_support_status  = NV_RUIM_ITEM_NOT_SUPPORTED;
#ifdef FEATURE_UIM_SUPPORT_3GPD
  byte i;
#endif /*FEATURE_UIM_SUPPORT_3GPD*/
  /* Take the task pointer signal from nv_cmd_ptr and put into the uim_cmd */
  cmd.hdr.user_data = nv_cmd_ptr->sigs;   

#ifdef FEATURE_RUIM_CDMA_REFRESH
  /* always refresh the nvruim buffer if we have clear refresh flag */
  nvruim_process_refresh();
#endif /* FEATURE_RUIM_CDMA_REFRESH */

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

#if defined( FEATURE_RUIM_PHONEBOOK )
  /* Determine if item is a phonebook item */
  if((NV_DIAL_I != nv_cmd_ptr->item) && (NV_STACK_I != nv_cmd_ptr->item))
#endif /* FEATURE_RUIM_PHONEBOOK */
  {
    /* Determine if the CDMA DF is on the card */
    if ((NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
#if defined ( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
    )
   {
     /* Do not try to access the R-UIM card.  Get item from NV */
     return NV_RUIM_ITEM_NOT_SUPPORTED;
   }

  }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */



  /* Find-out if the NV item is an R-UIM item */
  /* If it is an R-UIM item:  */
  /*     Initialize the buffer with item pointer & size */
  /*     Select the EF*/
  /*     Select the offset into the EF  */

  switch(nv_cmd_ptr->item)
  {
    
    case NV_ESN_CHKSUM_I:
      /* ESN checksum read returns the status of the checksum,  */
      /* which is signaled by a successful read from the R-UIM. */
      /* FALLTHROUGH */

    case NV_ESN_I:
      nvruim_read_support_status = nvruim_read_esn(nv_cmd_ptr, op_status);
      break;

    case NV_COUNT_I:
      nvruim_read_support_status = nvruim_read_count(nv_cmd_ptr, op_status);
      break;

    case NV_MIN1_I:
    case NV_MIN2_I:
    case NV_IMSI_11_12_I:
    case NV_IMSI_MCC_I:
    case NV_IMSI_ADDR_NUM_I:
      nvruim_read_support_status =  nvruim_read_imsi_m(nv_cmd_ptr, op_status);
      break;                       

    case NV_IMSI_T_S1_I:
    case NV_IMSI_T_S2_I:
    case NV_IMSI_T_MCC_I:
    case NV_IMSI_T_11_12_I:
    case NV_IMSI_T_ADDR_NUM_I:
      nvruim_read_support_status =  nvruim_read_imsi_t(nv_cmd_ptr, op_status);
      break;                       

    case NV_SID_NID_I:
      nvruim_read_support_status =  nvruim_read_sid_nid(nv_cmd_ptr, op_status);
      break;                       

    case NV_DIR_NUMBER_I:
    case NV_DIR_NUMBER_PCS_I:
      nvruim_read_support_status = nvruim_read_msisdn(nv_cmd_ptr, op_status);
      break;

    case NV_ANALOG_FIRSTCHP_I:
      nvruim_read_support_status =  nvruim_read_analog_chan_pref(nv_cmd_ptr, 
                                                                 op_status);
      break;                       

    case NV_ANALOG_HOME_SID_I:
      nvruim_read_support_status =  nvruim_read_home_sid(nv_cmd_ptr, 
                                                         op_status);
      break;                       

    case NV_ACCOLC_I:
      nvruim_read_support_status =  nvruim_read_accolc(nv_cmd_ptr, op_status);
      break;                       

#ifdef FEATURE_UIM_SUPPORT_LBS
    case NV_GPS1_CAPABILITIES_I:
    case NV_GPS1_SEEDPOS_OPTION_I:
    case NV_GPS1_DYNAMIC_MODE_I:  
      cache_bit = RUIM_LBS_V2_CONFIG_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_lbs_v2_config (nv_cmd_ptr, 
                                                              op_status, 
                                                              cache_bit);
      break;

    case NV_GPS1_XTRA_ENABLED_I:
    case NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:
    case NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I:  
    case NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:
    case NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:
      cache_bit = RUIM_LBS_XTRA_CONFIG_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_lbs_xtra_config (nv_cmd_ptr, 
                                                                op_status, 
                                                                cache_bit);
      break;

    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:
    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:
    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I:
      cache_bit = RUIM_NO_CACHE_VALUE;
      nvruim_read_support_status = nvruim_read_lbs_xtra_server_urls(nv_cmd_ptr, 
                                                                    op_status, 
                                                                    cache_bit);
      break;

    case NV_GPS1_PDE_ADDRESS_I:
    case NV_GPS1_PDE_PORT_I:
	case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
    case NV_CGPS_1X_PDE_SERVER_PORT_I:
	case NV_CGPS_1X_PDE_SERVER_ADDR_IPV6_I:
	case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:
      cache_bit = RUIM_LBS_V2_PDE_ADDRESS_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_lbs_v2_pde_adress (nv_cmd_ptr, 
                                                                  op_status, 
                                                                  cache_bit);
      break;

    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
    case NV_CGPS_1X_MPC_SERVER_PORT_I:
    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV6_I:
	case NV_CGPS_1X_MPC_SERVER_ADDR_URL_I:
      cache_bit = RUIM_LBS_V2_MPC_ADDRESS_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_lbs_v2_mpc_adress (nv_cmd_ptr, 
                                                                  op_status, 
                                                                   cache_bit);
     break;
#endif /* FEATURE_UIM_SUPPORT_LBS*/

#ifdef FEATURE_UIM_RUIM_SUPPORT_SCI 
    case NV_SLOT_CYCLE_INDEX_I:
      nvruim_read_support_status =  nvruim_read_sci(nv_cmd_ptr, op_status);
      break;                       
#endif /* FEATURE_UIM_RUIM_SUPPORT_SCI */

    case NV_MOB_TERM_HOME_I   :
    case NV_MOB_TERM_FOR_SID_I:
    case NV_MOB_TERM_FOR_NID_I:
      nvruim_read_support_status =  nvruim_read_call_term(nv_cmd_ptr, 
                                                          op_status);
      break;                       

    case NV_HOME_SID_NID_I:
      nvruim_read_support_status = nvruim_read_home_sid_nid(nv_cmd_ptr, 
                                                            op_status);
      break;

#ifndef FEATURE_UIM_JCDMA_RUIM_SUPPORT
#ifndef FEATURE_DISABLE_UIMPRL // Gemsea Add
    case NV_ROAMING_LIST_683_I:
      nvruim_read_support_status = nvruim_read_prl(nv_cmd_ptr, op_status);
      break;
#endif
#endif  /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

#ifdef FEATURE_OTASP_OTAPA
    case NV_OTAPA_ENABLED_I:
#endif
    case NV_SPC_CHANGE_ENABLED_I:
      nvruim_read_support_status = nvruim_read_spc_enabled(nv_cmd_ptr, 
                                                           op_status);
      break;

#ifdef FEATURE_OTASP_OTAPA
    case NV_NAM_LOCK_I:
      nvruim_read_support_status = nvruim_read_nam_lock(nv_cmd_ptr, op_status);
      break;
#endif

      /* Service Preferences */
#ifndef FEATURE_NVRUIM_PREF_MODE_NOT_FROM_RUIM
    case  NV_PREF_MODE_I:
#endif
#ifdef FEATURE_SSPR_ENHANCEMENTS
    case  NV_SYSTEM_PREF_I:
#else
    case  NV_CDMA_PREF_SERV_I:
    case  NV_ANALOG_PREF_SERV_I:
#endif
      nvruim_read_support_status = nvruim_read_svc_pref(nv_cmd_ptr, op_status);
      break;

      /* TMSI */
    case  NV_ASSIGNING_TMSI_ZONE_LEN_I:
    case  NV_ASSIGNING_TMSI_ZONE_I:
    case  NV_TMSI_CODE_I:
    case  NV_TMSI_EXP_I:
      nvruim_read_support_status = nvruim_read_tmsi(nv_cmd_ptr, op_status);
      break;

      /* Analog Location and Registration Indicators */
    case  NV_NXTREG_I:
    case  NV_LSTSID_I:
    case  NV_LOCAID_I:
    case  NV_PUREG_I:
      nvruim_read_support_status = nvruim_read_loc_regn_ind(nv_cmd_ptr, 
                                                            op_status);
      break;

      /* CDMA Zone-Based Registration Indicators */
    case  NV_ZONE_LIST_I:
      nvruim_read_support_status = nvruim_read_zone_based_ind(nv_cmd_ptr, 
                                                              op_status);
      break;

      /* CDMA System/Network Registration Indicators */
    case  NV_SID_NID_LIST_I:
      nvruim_read_support_status = nvruim_read_sys_regn_ind(nv_cmd_ptr, 
                                                            op_status);
      break;

      /* CDMA Distance-Based Registration Indicators */
    case  NV_DIST_REG_I:
      nvruim_read_support_status = nvruim_read_dist_regn_ind(nv_cmd_ptr, 
                                                             op_status);
      break;

#ifdef FEATURE_UIM_SUPPORT_3GPD
#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
    case  NV_HYSTERISIS_ACTIVATION_TIMER_I:
    case  NV_ACTIVE_EPZID_TYPE_I:
    case  NV_DSAT707_CTA_TIMER_I:
      cache_bit = RUIM_DGC_EF_CACHE_BIT ;
      nvruim_read_support_status = nvruim_read_3gpd_data_generic_config(nv_cmd_ptr, 
                                                                      op_status, 
                                                                      cache_bit);
      break;
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */
      /* This item is supported on the card, but it is not readable - do not allow this
      to go to NV */
    case NV_DS_MIP_SS_USER_PROF_I:
      /* The following items are not supported in an R-UIM implementation, do not
      allow these items to go to NV */
    case NV_DS_MIP_DMU_PKOID_I:
    case NV_DS_MIP_DMU_MN_AUTH_I:
      if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
      UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
      {
        nvruim_read_support_status = NV_RUIM_ITEM_NOT_SUPPORTED;  
      }
      else 
      {
        *op_status = NV_NOTACTIVE_S;
        nvruim_read_support_status = NV_RUIM_SUPPORTS_ITEM;
      }
      break;
    case NV_DS_MIP_ENABLE_PROF_I:
      if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
         UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
      {
        nvruim_read_support_status = NV_RUIM_ITEM_NOT_SUPPORTED;  
      }
#ifndef FEATURE_UIM_3GPD_FALLBACK
      else if (!nvruim_3gpd_mip_svc) 
      {
        *op_status = NV_DONE_S;
        nvruim_read_support_status = NV_RUIM_SUPPORTS_ITEM;
      }
#endif /* !FEATURE_UIM_3GPD_FALLBACK */
      else 
      {
        *op_status = NV_DONE_S;
        for(i=0; i < NV_DS_MIP_MAX_NUM_PROF; i++)
        {
          nv_cmd_ptr->data_ptr->ds_mip_enable_prof[i] = 1;
        }
        nvruim_read_support_status = NV_RUIM_SUPPORTS_ITEM;
      }
      break;
      

      /* The following items CAN be read on the card */
    case NV_DS_QCMIP_I:
      nvruim_read_support_status = nvruim_read_3gpd(nv_cmd_ptr, op_status);
      break;


#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_NUM_VALID_PROFILES_I:
    case NV_DS_SIP_NAI_INFO_I:
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
    case NV_PPP_USER_ID_I:
      nvruim_read_support_status = nvruim_read_3gpd_sipupp(nv_cmd_ptr, 
                                                           op_status);
      break;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_ACTIVE_PROFILE_INDEX_I:
      nvruim_read_support_status = nvruim_read_3gpd_sipsp(nv_cmd_ptr, op_status);
      break;
    case NV_DS_SIP_PROFILE_I:
      nvruim_read_support_status = nvruim_read_3gpd_sipuppext(nv_cmd_ptr, op_status);
      break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
    case NV_DS_MIP_GEN_USER_PROF_I:
    case NV_DS_MIP_NUM_PROF_I:
    case NV_DS_MIP_RETRIES_I:
    case NV_DS_MIP_RETRY_INT_I:
    case NV_DS_MIP_PRE_RE_RRQ_TIME_I:
      cache_bit = RUIM_MIPUPP_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_3gpd_mipupp(nv_cmd_ptr, 
                                                           op_status,
                                                           cache_bit);
      break;

    case NV_DS_MIP_ACTIVE_PROF_I:
      nvruim_read_support_status = nvruim_read_3gpd_mipsp(nv_cmd_ptr, 
                                                          op_status);
      break;

      /* The following item refers to a generic SIP password in NV, but refers only 
         to a PAP password in the R-UIM.  The CHAP password is kept separately on the card, and
         cannot be read */
    case NV_DS_SIP_PPP_SS_INFO_I:
    case NV_PPP_PASSWORD_I:
      nvruim_read_support_status = nvruim_read_3gpd_sippapss(nv_cmd_ptr, 
                                                             op_status);
      break;

    case NV_DS_MIP_2002BIS_MN_HA_AUTH_I:
    case NV_DS_MIP_QC_DRS_OPT_I:
    case NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I:
    case NV_DS_MIP_RRQ_IF_TFRK_I: 
      cache_bit = RUIM_MIPFLAGS_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_3gpd_mipflags(nv_cmd_ptr, 
                                                             op_status, 
                                                             cache_bit);
      break;

    case NV_TCP_GRACEFUL_DORMANT_CLOSE_I:
#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
    case NV_TCP_KEEPALIVE_IDLE_TIME_I:
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */
      cache_bit = RUIM_TCPCONFIG_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_3gpd_tcp_config(nv_cmd_ptr, 
                                                               op_status, 
                                                               cache_bit);
      break;

#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
    case NV_HDR_AN_AUTH_USER_ID_LONG_I:
    case NV_HDR_AN_AUTH_NAI_I:
      nvruim_read_support_status = nvruim_read_hrpd_hrpdupp(nv_cmd_ptr, 
                                                            op_status);
      break;

#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

#ifdef FEATURE_UIM_EUIMID
    case NV_MEID_I:
      nvruim_read_support_status = nvruim_read_meid(nv_cmd_ptr, op_status);
      break;
#endif /* FEATURE_UIM_EUIMID */

    case NV_SMS_BC_CONFIG_I:
      nvruim_read_support_status = nvruim_read_bcsms_config( nv_cmd_ptr, 
                                                             op_status );
      break;

    case NV_SMS_BC_USER_PREF_I:
      nvruim_read_support_status = nvruim_read_bcsms_pref( nv_cmd_ptr, 
                                                           op_status );
      break;

    case NV_SMS_BC_SERVICE_TABLE_I:
    case NV_SMS_BC_SERVICE_TABLE_SIZE_I:
    case NV_SMS_BC_SERVICE_TABLE_OPTIONS_I:
      nvruim_read_support_status = nvruim_read_bcsms_table( nv_cmd_ptr, 
                                                            op_status );
      break;

    case NV_SMS_MO_RETRY_PERIOD_I:
    case NV_SMS_MO_RETRY_INTERVAL_I:
    case NV_SMS_MO_ON_TRAFFIC_CHANNEL_I:
    case NV_SMS_MO_ON_ACCESS_CHANNEL_I:
    case NV_SMS_SERVICE_OPTION_I:
      cache_bit = RUIM_SMS_CAP_EF_CACHE_BIT;
      nvruim_read_support_status = nvruim_read_smscap( nv_cmd_ptr, op_status, 
                                                       cache_bit );
      break;

    default:
      /* Item is not supported by R-UIM */
      nvruim_read_support_status = NV_RUIM_ITEM_NOT_SUPPORTED;
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return nvruim_read_support_status;
} /* nvruim_read */


/*===========================================================================

FUNCTION NVRUIM_SET_UIM_DIR_PRESENT

DESCRIPTION
The function set the flag to indicate which of the MF, CDMA DF, GSM DF, or DCS
1800 DF exist.

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
Updates the variable nvruim_dir_present.
===========================================================================*/
void nvruim_set_uim_dir_present(
  byte dir
)
{
  nvruim_dir_present = dir;
}

#ifdef FEATURE_RUIM_PHONEBOOK
/*===========================================================================

FUNCTION NVRUIM_PHBOOK_BCD_TO_ASCII

DESCRIPTION
  This function converts phone book data from BCD format to ASCII format.
  'F' indicates the end of data string.

DEPENDENCIES
  None.

RETURN VALUE
  Actual number of digits.

SIDE EFFECTS
  None.

===========================================================================*/
byte nvruim_phbook_bcd_to_ascii( byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */
                                 byte *to_ptr   /* Convert to */
)
{
  byte i;
  byte lower_nibble, upper_nibble;

  i = 0;

  while (i < num_digi )
  {
    lower_nibble = *from_ptr & 0x0F;

    /* Check to see if this is terminator */
    if (lower_nibble == 0x0F)
    {
      break;
    }
    else
    {
      switch (lower_nibble)
      {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
          /* Digits 0 - 9 */
          *to_ptr = lower_nibble + '0';
          break;

        case 0x0A:
          *to_ptr = '*';
          break;

        case 0x0B:
          *to_ptr = '#';
          break;

        case 0x0C:
          /* */
          *to_ptr = 'T';
          break;

        case 0x0D:
          *to_ptr = 'P';
          break;

        case 0x0E:
          *to_ptr = '-';
          break;

        default:
        /* Impossible to come here */
        break;
      }
      i++;
      to_ptr++;
    }

    upper_nibble = (*from_ptr & 0xF0) >> 4;

    /* Check to see if this is terminater */
    if (upper_nibble == 0x0F)
    {
      break;
    }
    else
    {
      switch (upper_nibble)
      {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
          /* Digits 0 - 9 */
          *to_ptr = upper_nibble + '0';
          break;

        case 0x0A:
          *to_ptr = '*';
          break;

        case 0x0B:
          *to_ptr = '#';
          break;

        case 0x0C:
          /* */
          *to_ptr = 'T';
          break;

        case 0x0D:
          *to_ptr = 'P';
          break;

        case 0x0E:
          *to_ptr = '-';
          break;

        default:
        /* Impossible to come here */
        break;
      } /* switch upper_nibble */

      i++;
      to_ptr++;
      from_ptr++;

    } /* if upper_nibble */
  } /* for */
  return i;
}

/*===========================================================================

FUNCTION NVRUIM_PHBOOK_ASCII_TO_BCD

DESCRIPTION
  This function converts phone book data from ASCII format to BCD format.

DEPENDENCIES
  The buffer that "to_ptr" is pointing to need to be preset to all "FF".

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void nvruim_phbook_ascii_to_bcd( byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */
                                 byte *to_ptr   /* Convert to */
)
{
  byte i;
  byte digit;

  for (i=0; i<num_digi; i++)
  {
    digit = *from_ptr;

    switch (digit)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        /* Digits 0 - 9 */
        digit = digit - '0';
        break;

      case '*':
        digit = 0x0A;
        break;

      case '#':
        digit = 0x0B;
        break;

      case 'T':
        /* */
        digit = 0x0C;
        break;

      case 'P':
        digit = 0x0D;
        break;

      case '-':
        digit = 0x0E;
        break;

      default:
      /* Illigal digits */
      break;
    }

    if (i%2 == 0 )
    { /* Lower nibble */
      *to_ptr &= 0xF0;
      *to_ptr |= digit;
    }
    else
    { /* Upper nibble */
      *to_ptr &= 0x0F;
      *to_ptr |= (byte)(digit << 4 );
      to_ptr++;
    }
    from_ptr++;
  }
}
#endif /* FEATURE_RUIM_PHONEBOOK */
#endif /* FEATURE_NV_RUIM */
#ifdef FEATURE_UIM_RUN_TIME_ENABLE

/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIGURATION

DESCRIPTION
  This function queries the R-UIM card to determine if it supports
  R-UIM functionality (existance of CDMA DF) and if it supports
  phonebook (existance of Telcom DF).

DEPENDENCIES
  This function should only be called by the NV_CMD function to process
  an NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  NV_DONE_S  Operation is complete.

SIDE EFFECTS
  This function updates the RTRE control passed by reference based on
  the selects made to the UIM card.

===========================================================================*/
nv_stat_enum_type nvruim_process_rtre_configuration
(
  nv_cmd_type                  *cmd_ptr,    /* Pointer to command received on nv_cmd_q */
  nv_rtre_control_type         *nv_rtre_ctrl,
  nv_rtre_polling_control_type *nv_rtre_polling_ctrl
)
{
  /* Check if the command is valid */
  if(NV_RTRE_CONFIG_I != cmd_ptr->item)
  {
    /* Return a status indicating a bad command. */
    return NV_BADPARM_S;
  } /* end if - the item was incorrect for this command */

  nvruim_rtre_config_initialized = TRUE;

  /* Determine the control based on the configuration */
  switch(cmd_ptr->data_ptr->rtre_config)
  {
    /* RTRE configuration: use R-UIM only */
    case NV_RTRE_CONFIG_RUIM_ONLY:{
        /* Set the control to use the R-UIM */
        *nv_rtre_ctrl = NV_RTRE_CONTROL_USE_RUIM;
        *nv_rtre_polling_ctrl = NV_RTRE_POLLING_CONTROL_POLL;
      }
      break; /* end case - NV_RTRE_CONFIG_RUIM_ONLY */

      /* RTRE configuration: use NV only    */
    case NV_RTRE_CONFIG_NV_ONLY:{
        /* Set the control to NOT use the R-UIM */
        *nv_rtre_ctrl = NV_RTRE_CONTROL_NO_RUIM;
        *nv_rtre_polling_ctrl = NV_RTRE_POLLING_CONTROL_NO_POLL;
      }
      break; /* end case - NV_RTRE_CONFIG_NV_ONLY */

      /* RTRE: use R-UIM if available, otherwise use NV */
    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:{
        /* Check if the CDMA DF is on the card. */
        if(NVRUIM_CDMA_DF_PRESENT & nvruim_dir_present)
        {
          /* Set the control to use the R-UIM */
          *nv_rtre_ctrl = NV_RTRE_CONTROL_USE_RUIM;
          *nv_rtre_polling_ctrl = NV_RTRE_POLLING_CONTROL_POLL;
        }
        else /* There was no CDMA DF on the card */
        {
          /* Set the control to NOT use the R-UIM */
          *nv_rtre_ctrl = NV_RTRE_CONTROL_NO_RUIM;
          *nv_rtre_polling_ctrl = NV_RTRE_POLLING_CONTROL_NO_POLL;
        } /* end if -  found CDMA DF on card. */
      }
      break; /* end case - NV_RTRE_CONFIG_RUIM_OR_DROP_BACK */

      /* RTRE configuration: use GSM with 1x */
    case NV_RTRE_CONFIG_SIM_ACCESS:{
        /* Set the control to use the R-UIM */
        *nv_rtre_ctrl = NV_RTRE_CONTROL_SIM_ACCESS;
        *nv_rtre_polling_ctrl = NV_RTRE_POLLING_CONTROL_POLL;
      }
      break; /* end case - NV_RTRE_CONFIG_RUIM_ONLY */

      /* This is a bad value */
    default:{
        /* Return a status indicating a bad command. */
        return NV_BADPARM_S;
      }

  } /* end switch - cmd_ptr->data_ptr.rtre_configuration */
  
  if((*nv_rtre_ctrl    == NV_RTRE_CONTROL_USE_RUIM) &&
     (NV_RUIM_ZERO_ESN == nvruim_get_esn_usage()))
  {
    nvruim_set_esn_usage(NV_RUIM_USE_ESN);
  }
#ifdef FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED
  if(NV_RTRE_CONFIG_RUIM_ONLY == cmd_ptr->data_ptr->rtre_config)
  {
    uim_hrpd_disabled_card_flag = uim_hrpd_disabled_card();
  }
#endif /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED */

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
  uim_init_an_hrpd_preference(nvruim_hrpd_cdma_svc);
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

  /* Indicate the command has been successfully completed */
  return NV_DONE_S;

} /* nvruim_process_rtre_configuration */

#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

FUNCTION nvruim_init_wms_svc_items

DESCRIPTION
  This function is called from the WMS task during initialization for SMS.
  It passes in boolean values to indicate whether or not the RUIM provides
  for BCSMS and SMS Capabilities provisioning.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_bcsms_svc and nvruim_smscap_svc 
  global variables to indicate service support.

===========================================================================*/ 
void nvruim_init_wms_svc_items
(
  boolean service_bcsms, 
  boolean service_smscap
)
{
#ifdef FEATURE_NV_RUIM
  nvruim_bcsms_svc   = service_bcsms;
  nvruim_smscap_svc  = service_smscap;
#endif /* FEATURE_NV_RUIM */
  /* To remove Lint Error: Not Referenced' */  
  (void)service_bcsms;
  (void)service_smscap;
  return;
} /* nvruim_init_wms_svc_items() */


/*===========================================================================

FUNCTION NVRUIM_INIT_NON_3GPD_CDMA_CARD

DESCRIPTION
  This function is called from mmgsdi task during post pin initialization.
  It passes in a boolean, where
    TRUE : it is 3GPD disabled card
    FALSE: it is 3GPD enabled  card

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
none
===========================================================================*/ 
void nvruim_init_non_3gpd_cdma_card (
  boolean non_3gpd_cdma_card
)
{
#ifdef FEATURE_NV_RUIM	
#if defined(FEATURE_UIM_SUPPORT_3GPD_NV) && defined (FEATURE_UIM_SUPPORT_3GPD)
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if(NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
  {
    /* NV mode, Set to No card */
    nvruim_non_3gpd_cdma_card = NVRUIM_NO_CARD;
  }
  else
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* RUIM mode, set flag for cdma card */	  
    if ( TRUE == non_3gpd_cdma_card)
    {
      MSG_HIGH("Non 3GPD CDMA Card : TRUE",0,0,0);
      nvruim_non_3gpd_cdma_card = NVRUIM_NON_3GPD_CARD;
      /* Reset the 3GPD credential to NV */
      nvruim_3gpd_control       = UIM_3GPD_MIP_NV_SIP_NV;
    }
    else
    {
      MSG_HIGH("Non 3GPD CDMA Card : FALSE",0,0,0);
      nvruim_non_3gpd_cdma_card = NVRUIM_3GPD_CARD;
    }	    
  }
#endif /* FEATURE_UIM_SUPPORT_3GPD_NV && FEATURE_UIM_SUPPORT_3GPD  */
#endif /* FEATURE_NV_RUIM */
  (void) non_3gpd_cdma_card;
} /* nvruim_init_non_3gpd_cdma_card */


/*===========================================================================

FUNCTION NVRUIM_CHECK_NON_3GPD_CDMA_CARD

DESCRIPTION
 This function returns the non legacy cdma card presence.

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
  nvruim_3gpd_nv_card_status where
  NVRUIM_NO_CARD        : NO Card 
  NVRUIM_NON_3GPD_CARD  : 3GPD disabled card 
  NVRUIM_3GPD_CARD      : 3GPD enabled card

SIDE EFFECTS
none
===========================================================================*/ 
nvruim_3gpd_nv_card_status nvruim_check_non_3gpd_cdma_card (
  void
)
{
#ifdef FEATURE_NV_RUIM	
#if defined(FEATURE_UIM_SUPPORT_3GPD_NV) && defined (FEATURE_UIM_SUPPORT_3GPD)
  return nvruim_non_3gpd_cdma_card;
#else 
  /* No need to check non 3GPD card as 3GPD is disabled there */
  return NVRUIM_3GPD_CARD;
#endif /* FEATURE_UIM_SUPPORT_3GPD_NV && FEATURE_UIM_SUPPORT_3GPD */
#else
  /* NV mode */
  return NVRUIM_NO_CARD;
#endif /* FEATURE_NV_RUIM */
} /* nvruim_check_non_3gpd_cdma_card */


/*===========================================================================

FUNCTION UIM_INIT_3GPD_PREFERENCE

DESCRIPTION
This function is called from the PS task during initialization for 3GPD.
It passes in a boolean which indicates whether or not the RUIM provides
for 3GPD services

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
Sets the nvruim_3gpd_control global variable for RUIM support of 3GPD
Sets the nvruim_3gpd_mip_svc and nvruim_3gpd_sip_svc 
global variables to indicate service support


===========================================================================*/ 
void uim_init_3gpd_preference(
  boolean service_3gpd_sip, 
  boolean service_3gpd_mip
)
{
#ifdef FEATURE_NV_RUIM
#ifdef FEATURE_UIM_SUPPORT_3GPD

  nvruim_3gpd_sip_svc=service_3gpd_sip;
  nvruim_3gpd_mip_svc=service_3gpd_mip;
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if(NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
  {
    /* Use NV for 3GPD support */
    nvruim_3gpd_control = UIM_3GPD_MIP_NV_SIP_NV;
  }
  else
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#ifdef FEATURE_UIM_3GPD_FALLBACK
    if(service_3gpd_sip && service_3gpd_mip)
#endif /* FEATURE_UIM_3GPD_FALLBACK */
  {
    /* Use R-UIM for 3GPD Support */
    nvruim_3gpd_control = UIM_3GPD_MIP_RUIM_SIP_RUIM;
  }
#ifdef FEATURE_UIM_3GPD_FALLBACK
  else if(service_3gpd_sip)
  {
    nvruim_3gpd_control = UIM_3GPD_MIP_NV_SIP_RUIM;
  }
  else if(service_3gpd_mip)
  {
    nvruim_3gpd_control = UIM_3GPD_MIP_RUIM_SIP_NV;
  }
  else
  {
    nvruim_3gpd_control = UIM_3GPD_MIP_NV_SIP_NV;
  }
#endif /* FEATURE_UIM_3GPD_FALLBACK */

#ifdef FEATURE_UIM_SUPPORT_3GPD_NV
  if(NVRUIM_NON_3GPD_CARD == nvruim_non_3gpd_cdma_card)
  {
    /* set nv_ruim_3gpd control flag to 
       UIM_3GPD_MIP_NV_SIP_NV for non 3gpd card*/
    nvruim_3gpd_control = UIM_3GPD_MIP_NV_SIP_NV;
  }
#endif /* FEATURE_UIM_SUPPORT_3GPD_NV */
  
#else
  nvruim_3gpd_control = UIM_3GPD_MIP_NV_SIP_NV;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#endif /* FEATURE_NV_RUIM */
/* To remove Lint Error: Not Referenced' */   
  (void)service_3gpd_sip;    
  (void)service_3gpd_mip;
  return;
} /* uim_init_3gpd_preference */

/*===========================================================================

FUNCTION UIM_3GPD_SUPPORT

DESCRIPTION
This function returns the preferred source for 3GPD operations.


DEPENDENCIES
None.

RETURN VALUE
uim_3gpd_support_status - either UIM_3GPD_SUPPORT or UIM_3GPD_MIP_NV_SIP_NV

SIDE EFFECTS
None.

===========================================================================*/
uim_3gpd_support_status uim_3gpd_support()
{
#ifdef FEATURE_NV_RUIM
  return nvruim_3gpd_control;
#else
  return UIM_3GPD_MIP_NV_SIP_NV;
#endif /* FEATURE_NV_RUIM */
} /* uim_3gpd_support */


/*===========================================================================

FUNCTION NVRUIM_LBS_INIT_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for LBS  
  Support. It passes in a boolean value which indicates whether or not the 
  RUIM card have support for service 24 (LBS)in its CDMA Service Table.
  The requirement on the RUIM card for this serviec is defined in Open Market 
  Handset RUIM Specification(CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_lbs_support global variable for LBS support
===========================================================================*/ 
void nvruim_lbs_init_support(
  boolean service_lbs
)
{
#if defined(FEATURE_NV_RUIM) && defined(FEATURE_UIM_SUPPORT_LBS)
  nvruim_lbs_support = service_lbs;
#endif /* FEATURE_UIM_SUPPORT_LBS || FEATURE_NV_RUIM */
  /* To remove Lint Error: Not Referenced' */   
  (void)service_lbs;
  return;
} /* nvruim_lbs_init_support */


/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_INIT_EXTENSIONS_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for 3GPD 
  Extentions. It passes in a boolean which indicates whether or not the RUIM 
  provides support for service 15 (Messagind and 3GPD Extentions).
  The requirement on the RUIM card for this serviec is defined in Open Market 
  Handset RUIM Specification(CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_3gpd_ext_support global variable for extended 3GPD RUIM support
  Sets the nvruim_3gpd_ext_use_card variable for current status of 3GPD ext usage
===========================================================================*/ 
void nvruim_data_3gpd_init_extensions_support(
  boolean service_3gpd_extensions
)
{
#if defined(FEATURE_NV_RUIM) && defined(FEATURE_UIM_SUPPORT_3GPD)
  nvruim_3gpd_ext_support = service_3gpd_extensions;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_NV_RUIM */
  /* To remove Lint Error: Not Referenced' */   
  (void)service_3gpd_extensions;
  return;
} /* nvruim_data_3gpd_init_extensions_support */


/*===========================================================================

FUNCTION UIM_INIT_AN_HRPD_PREFERENCE

DESCRIPTION
This function is called from the PS task during initialization for HRPD.
It passes in a boolean which indicates whether or not the RUIM provides
for HRPD services, and a boolean which indicates whether cave is to be used

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
Sets the nvruim_hrpd_control global variable for RUIM support of HRPD
Sets the nvruim_hrpd_cdma_svc global variable to indicate CDMA service support
===========================================================================*/ 
void uim_init_an_hrpd_preference(
  boolean service_hrpd
)
{
#ifdef FEATURE_NV_RUIM
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
  nvruim_hrpd_cdma_svc=service_hrpd;
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if(NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
  {
    /* Use NV for HRPD support */
    nvruim_hrpd_control = UIM_AN_HRPD_NO_SUPPORT;
    return;
  }
  else
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  if(service_hrpd
#ifdef FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED
 && (!(uim_hrpd_disabled_card_flag || uim_use_cave_default))
#endif /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED */
    )
  {
    /* Use R-UIM for HRPD Support */
    nvruim_hrpd_control = UIM_AN_HRPD_SUPPORT;
    return;
  }
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  else
  {
    nvruim_hrpd_control=UIM_AN_HRPD_USE_CAVE;  
    return;
  }
#else
#ifndef FEATURE_UIM_AN_HRPD_FALLBACK
  nvruim_hrpd_control = UIM_AN_HRPD_SUPPORT;
  return;
#else /* !FEATURE_UIM_AN_HRPD_FALLBACK */
  nvruim_hrpd_control = UIM_AN_HRPD_NO_SUPPORT;
  return;
#endif /* !FEATURE_UIM_AN_HRPD_FALLBACK */
#endif /* FEATURE_UIM_CAVE_AN_AUTH */  
#else
  nvruim_hrpd_control = UIM_AN_HRPD_NO_SUPPORT;
  /* To remove Lint Error: Not Referenced' */   
  (void)service_hrpd;
  return;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */
#else
  /* To remove Lint Error: Not Referenced' */   
  (void)service_hrpd;
#endif /* FEATURE_NV_RUIM */
} /* nvruim_init_an_hrpd_preference */

/*===========================================================================

FUNCTION UIM_AN_HRPD_SUPPORT

DESCRIPTION
This function returns the preferred source for HRPD operations.


DEPENDENCIES
None.

RETURN VALUE
uim_an_hrpd_support_status - either UIM_AN_HRPD_SUPPORT, UIM_AN_HRPD_NO_SUPPORT
or UIM_AN_HRPD_USE_CAVE

SIDE EFFECTS
None.

===========================================================================*/
uim_an_hrpd_support_status uim_an_hrpd_support()
{
#ifdef FEATURE_NV_RUIM
  return nvruim_hrpd_control;
#else
  return UIM_AN_HRPD_NO_SUPPORT;
#endif /* FEATURE_NV_RUIM */
} /* uim_an_hrpd_support */


/*===========================================================================

FUNCTION NVRUIM_SET_ESN_USAGE

DESCRIPTION
This function sets the usage indicator that is used by the NV-RUIM module
to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Updates the usage indicator variable nvruim_esn_usage.

===========================================================================*/
void nvruim_set_esn_usage(
  nvruim_esn_usage_type esn_usage
)
{
#ifdef FEATURE_NV_RUIM	
  nvruim_esn_usage = esn_usage;
#endif /* FEATURE_NV_RUIM */
  (void) esn_usage;
} /* nvruim_set_esn_usage */


/*===========================================================================

FUNCTION NVRUIM_GET_ESN_USAGE

DESCRIPTION
This function gets the usage indicator that is used by the NV-RUIM module
to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
Non-reentrant

RETURN VALUE
None.

SIDE EFFECTS

===========================================================================*/
nvruim_esn_usage_type nvruim_get_esn_usage()
{
#ifdef FEATURE_NV_RUIM	
  return nvruim_esn_usage;
#else
  /* Returning default value */  
  return NV_RUIM_USE_ESN;
#endif /* FEATURE_NV_RUIM */
}/* nvruim_get_esn_usage */


#ifdef FEATURE_NV_RUIM
/*===========================================================================

FUNCTION NVRUIM_FREE

DESCRIPTION
  Check to see if the item is supported by the R-UIM.
  If it is supported, free it.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the free.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  None

===========================================================================*/
nv_ruim_support_status nvruim_free
(
  nv_cmd_type  *nv_cmd_ptr,       /* command block                            */
  nv_stat_enum_type *op_status    /* status of the I/O operation              */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */

  /* Determine if the CDMA DF is on the card */
  if ((NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
#if defined ( FEATURE_UIM_RUIM_W_GSM_ACCESS )
#error code not present
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
     )
  {
    /* Do not try to access the R-UIM card.  Get item from NV */
    return NV_RUIM_ITEM_NOT_SUPPORTED;
  }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

  /* Item is not supported by R-UIM                                       */
  return NV_RUIM_ITEM_NOT_SUPPORTED;
  
} /* nvruim_free */


#ifdef FEATURE_RUIM_CDMA_REFRESH
/*===========================================================================

FUNCTION nvruim_clear_cache

DESCRIPTION
  This function is called from the mmgsdi task as part of the refresh process.
  This function will clear the nvruim cache if a refresh on one of the files
  is requested.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  nvruim cache will be cleared

===========================================================================*/ 
boolean nvruim_clear_cache(
  uint8 num_files,
  const uim_items_enum_type *file_list_p
)
{
  int                  i                      = 0;
  uim_items_enum_type *refresh_files_p        = NULL;
  uint32               nvruim_temp_cache_list = RUIM_NO_CACHE_VALUE;  

  if (num_files != NVRUIM_CLEAR_ALL_CACHE && file_list_p == NULL) 
  {
    MSG_ERROR("Null file list pointer passed into nvruim_clear_cache", 0,0,0);
    return FALSE;
  }

  if ( ( num_files > NVRUIM_MAX_REFRESH_FILES && 
        (num_files != NVRUIM_CLEAR_ALL_CACHE)) || num_files == 0) 
  {
    MSG_ERROR("Invalid number of files passed into nvruim_clear_cache: %d", 
              num_files,0,0);
    return FALSE;
  }

  if (num_files == NVRUIM_CLEAR_ALL_CACHE && file_list_p == NULL) 
  {
    MSG_HIGH("nvruim_clear_cache: Clearing All nvruim cache items",0,0,0);
    nvruim_refresh_file_cache |=  nvruim_cache_control;    
    return TRUE;
  }

  refresh_files_p = uim_malloc(sizeof(uim_items_enum_type)* num_files);
  if ( refresh_files_p == NULL )
  {
    MSG_ERROR("Could not allocate memory for the file list",0,0,0);
    return FALSE;
  }
  memcpy(refresh_files_p, file_list_p, sizeof(uim_items_enum_type) * 
                                       num_files);
  MSG_HIGH("nvruim_clear_cache: Clearing %d files",num_files,0,0);
  
  /* Go through each file in file list and determine if cache exists if so
     clear cache and cache bit*/
  /* Set the refresh cache bit for given file list */
  for (i = 0; i < num_files; i++)
  {
    MSG_HIGH("nvruim_clear_cache: clearing cache for file 0x%x",
             refresh_files_p[i], 0, 0);
    switch (refresh_files_p[i])
    {
      case UIM_CDMA_IMSI_M:
        nvruim_temp_cache_list |= RUIM_IMSI_M_CACHE_BIT;
        break;

      case UIM_CDMA_IMSI_T:
        nvruim_temp_cache_list |=RUIM_IMSI_T_CACHE_BIT;
        break;

      case UIM_CDMA_RUIM_ID:
        nvruim_temp_cache_list |= RUIM_RUIM_ID_CACHE_BIT;
        break;

      case UIM_CDMA_CALL_TERM_MODE_PREF:
        nvruim_temp_cache_list |= RUIM_CALL_TERM_MODE_PREF_CACHE_BIT;
        break;

      case UIM_CDMA_OTAPA_SPC_ENABLE:
        nvruim_temp_cache_list |= RUIM_OTAPA_SPC_ENABLE_CACHE_BIT;
        break;

      case UIM_CDMA_SERVICE_PREF:
        nvruim_temp_cache_list |= RUIM_SVC_PREF_CACHE_BIT;
        break;

      case UIM_CDMA_TMSI:
        nvruim_temp_cache_list |= RUIM_TMSI_CACHE_BIT;
        break;

      case UIM_CDMA_ANALOG_LOCN_AND_REGN_IND:
        nvruim_temp_cache_list |= RUIM_ANALOG_REG_IND_CACHE_BIT;
        break;

      case UIM_CDMA_ZONE_BASED_REGN_IND:
        nvruim_temp_cache_list |= RUIM_ZONE_REG_IND_CACHE_BIT;
        break;

      case UIM_CDMA_SYS_REGN_IND:
        nvruim_temp_cache_list |= RUIM_SYS_REG_IND_CACHE_BIT;
        break;

      case UIM_CDMA_DIST_BASED_REGN_IND:
        nvruim_temp_cache_list |= RUIM_DIST_REG_IND_CACHE_BIT;
        break;

#ifdef FEATURE_UIM_EUIMID
      case UIM_CDMA_SF_EUIM_ID:
        nvruim_temp_cache_list |= RUIM_SF_EUIMID_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_UIM_SUPPORT_LBS
      case UIM_CDMA_LBS_V2CONFIG:
        nvruim_temp_cache_list |= RUIM_LBS_V2_CONFIG_EF_CACHE_BIT;
        break;
      case UIM_CDMA_LBS_XCONFIG:
        nvruim_temp_cache_list |= RUIM_LBS_XTRA_CONFIG_EF_CACHE_BIT;
        break;
      case UIM_CDMA_LBS_V2PDEADDR:
        nvruim_temp_cache_list |= RUIM_LBS_V2_PDE_ADDRESS_EF_CACHE_BIT;
        break;
      case UIM_CDMA_LBS_V2MPCADDR:
        nvruim_temp_cache_list |= RUIM_LBS_V2_MPC_ADDRESS_EF_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_SUPPORT_LBS */

#ifdef FEATURE_UIM_SUPPORT_3GPD
      case UIM_CDMA_3GPD_MIPFLAGS:
        nvruim_temp_cache_list |= RUIM_MIPFLAGS_EF_CACHE_BIT;
        break;

      case UIM_CDMA_3GPD_TCPCONFIG:
        nvruim_temp_cache_list |= RUIM_TCPCONFIG_EF_CACHE_BIT;
        break;
#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
      case UIM_CDMA_3GPD_DGC:
        nvruim_temp_cache_list |= RUIM_DGC_EF_CACHE_BIT ;
        break;
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */
      case UIM_CDMA_3GPD_SIPUPP:
        nvruim_temp_cache_list |= RUIM_SIPUPP_EF_CACHE_BIT;
        break;

      case UIM_CDMA_3GPD_MIPUPP:
        nvruim_temp_cache_list |= RUIM_MIPUPP_EF_CACHE_BIT;
        break;

      case UIM_CDMA_3GPD_MIPSP:
        nvruim_temp_cache_list |= RUIM_MIPSP_EF_CACHE_BIT;
        break;

      case UIM_CDMA_3GPD_SIPPAPSS:
        nvruim_temp_cache_list |= RUIM_SIPPAPSS_EF_CACHE_BIT;
        break;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      case UIM_CDMA_3GPD_SIPUPPEXT:
        nvruim_temp_cache_list |= RUIM_SIPUPPEXT_EF_CACHE_BIT;
        break;

      case UIM_CDMA_3GPD_SIPSP:
        nvruim_temp_cache_list |= RUIM_SIPSP_EF_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
      case UIM_CDMA_HRPD_HRPDUPP:
        nvruim_temp_cache_list |= RUIM_HRPD_NAI_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

      default:
        MSG_HIGH("No Cache for File 0x%x", refresh_files_p[i], 0, 0);
        break;
    }
  } /* end switch for uim file*/
  uim_free(refresh_files_p);

  /* Update the refresh cache list */
  nvruim_refresh_file_cache |= nvruim_temp_cache_list;
  return TRUE;
} /* nvruim_clear_cache()*/


/*===========================================================================

FUNCTION nvruim_nv_item_to_uim_file

DESCRIPTION
  This function will return the corresponding uim_items_enum_type for a
  nv item.  This function can be used by clients that are registering for
  refresh notifications but use NV apis to read items.

DEPENDENCIES
  None.

RETURN VALUE
  uim_items_enum_type

SIDE EFFECTS
  None

===========================================================================*/ 
uim_items_enum_type nvruim_nv_item_to_uim_file(
  nv_items_enum_type nvitem
)
{
  switch (nvitem) 
  {
#ifdef FEATURE_UIM_SUPPORT_3GPD
    case NV_DS_QCMIP_I:
      return UIM_CDMA_3GPD_3GPDOPM;
      
    case NV_PPP_USER_ID_I:
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_NUM_VALID_PROFILES_I:
    case NV_DS_SIP_NAI_INFO_I:
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      return UIM_CDMA_3GPD_SIPUPP;

    case NV_DS_MIP_RETRIES_I:
    case NV_DS_MIP_NUM_PROF_I:
    case NV_DS_MIP_RETRY_INT_I:
    case NV_DS_MIP_PRE_RE_RRQ_TIME_I:
      return UIM_CDMA_3GPD_MIPUPP;

    case NV_DS_MIP_ACTIVE_PROF_I:
      return UIM_CDMA_3GPD_MIPSP;
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_ACTIVE_PROFILE_INDEX_I:
      return UIM_CDMA_3GPD_SIPSP;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
    case NV_PPP_PASSWORD_I:
      return UIM_CDMA_3GPD_SIPPAPSS;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case NV_DS_SIP_PROFILE_I:
      return UIM_CDMA_3GPD_SIPUPPEXT;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
#endif /* FEATURE_UIM_SUPPORT_3GPD*/

    default:
      MSG_HIGH("NV Item %d could not be converted to UIM file", nvitem, 0, 0);
      return UIM_NO_SUCH_ITEM;
  }
}


/*===========================================================================

FUNCTION nvruim_uim_file_to_nv_item

DESCRIPTION
  This function will populate the pointer of type nvruim_nv_conversion passed 
  in with the NV Items corresponding with the file that is passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
void nvruim_uim_file_to_nv_item(
  uim_items_enum_type  uim_file,
  nvruim_nv_conversion *nv_items_ptr
)
{
  if (nv_items_ptr == NULL) 
  {
    MSG_ERROR("NULL Pointer for nv_items_ptr passed into nvruim_uim_file_to_nv_item",
              0,0,0);
    return;
  }
  switch (uim_file) 
  {
#ifdef FEATURE_UIM_SUPPORT_3GPD
    case UIM_CDMA_3GPD_3GPDOPM:
      nv_items_ptr->num_nv_items = 1;
      nv_items_ptr->nv_items[0] = NV_DS_QCMIP_I;
      break;
      
    case UIM_CDMA_3GPD_SIPUPP:
      nv_items_ptr->nv_items[0] = NV_PPP_USER_ID_I;
#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      nv_items_ptr->nv_items[1] = NV_DS_SIP_NUM_VALID_PROFILES_I;
      nv_items_ptr->nv_items[2] = NV_DS_SIP_NAI_INFO_I;
      nv_items_ptr->num_nv_items = 3;
#else
      nv_items_ptr->num_nv_items = 1;      
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
      break;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case UIM_CDMA_3GPD_SIPUPPEXT:    
      nv_items_ptr->num_nv_items = 1;
      nv_items_ptr->nv_items[0] = NV_DS_SIP_PROFILE_I;
      break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */

    case UIM_CDMA_3GPD_MIPUPP:
      nv_items_ptr->num_nv_items = 4;
      nv_items_ptr->nv_items[0] = NV_DS_MIP_RETRIES_I;
      nv_items_ptr->nv_items[1] = NV_DS_MIP_NUM_PROF_I;
      nv_items_ptr->nv_items[2] = NV_DS_MIP_RETRY_INT_I;
      nv_items_ptr->nv_items[3] = NV_DS_MIP_PRE_RE_RRQ_TIME_I;
      break;
    
    case UIM_CDMA_3GPD_MIPSP:
      nv_items_ptr->num_nv_items = 1;
      nv_items_ptr->nv_items[0] = NV_DS_MIP_ACTIVE_PROF_I;
      break;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
    case UIM_CDMA_3GPD_SIPSP:
      nv_items_ptr->num_nv_items = 1;
      nv_items_ptr->nv_items[0] = NV_DS_SIP_ACTIVE_PROFILE_INDEX_I;
      break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */

    case UIM_CDMA_3GPD_SIPPAPSS:
      nv_items_ptr->num_nv_items = 1;
      nv_items_ptr->nv_items[0] = NV_PPP_PASSWORD_I;
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
    default:
      nv_items_ptr->num_nv_items = 0;
  }  
} /* nvruim_uim_file_to_nv_item */


/*===========================================================================

FUNCTION nvruim_process_refresh

DESCRIPTION
  This function is called from nv request context as part of nv read\ write 
  operation. This function will clear the nvruim cache, before doing nv operation
  there if a refresh on one of the files is requested.

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  nvruim cache will be cleared

===========================================================================*/
void nvruim_process_refresh( void
)
{
  int     i                         =  0;
  uint32  nvruim_temp_cache_control =  RUIM_NO_CACHE_VALUE;
  uint32  nvruim_cache_mask         =  RUIM_IMSI_M_CACHE_BIT;
  
  /* get nvruim refresh cache file list */
  nvruim_temp_cache_control = nvruim_refresh_file_cache;

  if (nvruim_temp_cache_control == RUIM_NO_CACHE_VALUE)
  {
      /* No refresh is require */
      return;
  }

  /* Go through each file in nvruim_temp_cache_control and determine if cache exists
     if so clear cache and cache bit*/
  for (i = 0; i < NVRUIM_CLEAR_ALL_CACHE; i++)
  {
    switch (nvruim_temp_cache_control & nvruim_cache_mask)
    {
      case RUIM_IMSI_M_CACHE_BIT:
        memset((byte *)&nvruim_imsi_m_cache_buf, 0x00, sizeof(nvruim_imsi_type));
        nvruim_cache_control &= ~RUIM_IMSI_M_CACHE_BIT;
        break;

      case RUIM_IMSI_T_CACHE_BIT:
        memset((byte *)&nvruim_imsi_t_cache_buf, 0x00, sizeof(nvruim_imsi_type));
        nvruim_cache_control &= ~RUIM_IMSI_T_CACHE_BIT;
        break;

      case RUIM_RUIM_ID_CACHE_BIT:
        memset((byte *)&nvruim_ruim_id_cache_buf, 0x00,
               sizeof(nvruim_ruim_id_cache_buf));
        nvruim_cache_control &= RUIM_RUIM_ID_CACHE_BIT;
        break;

      case RUIM_CALL_TERM_MODE_PREF_CACHE_BIT:
        nvruim_call_term_mode_pref_cache_buf = 0x00;
        nvruim_cache_control &= ~RUIM_CALL_TERM_MODE_PREF_CACHE_BIT;
        break;

      case RUIM_OTAPA_SPC_ENABLE_CACHE_BIT:
        nvruim_otapa_spc_enable_cache_buf = 0x00;
        nvruim_cache_control &= ~RUIM_OTAPA_SPC_ENABLE_CACHE_BIT;
        break;

      case RUIM_SVC_PREF_CACHE_BIT:
        nvruim_svc_pref_cache_buf = 0x00;
        nvruim_cache_control &= ~RUIM_SVC_PREF_CACHE_BIT;
        break;

      case RUIM_TMSI_CACHE_BIT:
        memset((byte *)&nvruim_tmsi_cache_buf, 0x00, sizeof(nvruim_tmsi_type));
        nvruim_cache_control &= ~RUIM_TMSI_CACHE_BIT;
        break;

      case RUIM_ANALOG_REG_IND_CACHE_BIT:
        memset((byte *)&nvruim_analog_reg_ind_cache_buf, 0x00,
               sizeof(nvruim_analog_reg_ind_type));
        nvruim_cache_control &= ~RUIM_ANALOG_REG_IND_CACHE_BIT;
        break;

      case RUIM_ZONE_REG_IND_CACHE_BIT:
        memset((byte *)&nvruim_zone_reg_ind_cache_buf, 0x00,
               sizeof(nvruim_zone_reg_ind_type));
        nvruim_cache_control &= ~RUIM_ZONE_REG_IND_CACHE_BIT;
        break;

      case RUIM_SYS_REG_IND_CACHE_BIT:
        memset((byte *)&nvruim_sys_reg_ind_cache_buf, 0x00,
               sizeof(nvruim_sys_reg_ind_type));
        nvruim_cache_control &= ~RUIM_SYS_REG_IND_CACHE_BIT;
        break;

      case RUIM_DIST_REG_IND_CACHE_BIT:
        memset((byte *)&nvruim_dist_reg_ind_cache_buf, 0x00,
               sizeof(nvruim_dist_reg_ind_type));
        nvruim_cache_control &= ~RUIM_DIST_REG_IND_CACHE_BIT;
        break;

#ifdef FEATURE_UIM_EUIMID
      case RUIM_SF_EUIMID_CACHE_BIT:
        qw_set(nvruim_sf_euim_id_cache_buf, 0x00, 0x00);
        nvruim_cache_control &= ~RUIM_SF_EUIMID_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_UIM_SUPPORT_LBS
      case RUIM_LBS_V2_CONFIG_EF_CACHE_BIT:
        memset(nvruim_lbs_v2_config_ef_buf , 0x00,
             sizeof(byte) * NVRUIM_MAX_LBS_V2_CONFIG_SIZE);
        nvruim_cache_control &= ~RUIM_LBS_V2_CONFIG_EF_CACHE_BIT;
        break;
      case RUIM_LBS_XTRA_CONFIG_EF_CACHE_BIT:
        memset(nvruim_lbs_xtra_config_ef_buf, 0x00,
             sizeof(byte) * NVRUIM_MAX_LBS_XTRA_CONFIG_SIZE);
        nvruim_cache_control &= ~RUIM_LBS_XTRA_CONFIG_EF_CACHE_BIT;
        break;
      case RUIM_LBS_V2_PDE_ADDRESS_EF_CACHE_BIT:
        memset(nvruim_lbs_v2_pde_address_buf , 0x00,
             sizeof(byte) * NVRUIM_MAX_LBS_V2_PDE_ADDRESS_SIZE);
        nvruim_cache_control &= ~RUIM_LBS_V2_PDE_ADDRESS_EF_CACHE_BIT;
        break;
      case RUIM_LBS_V2_MPC_ADDRESS_EF_CACHE_BIT:
        memset(nvruim_lbs_v2_mpc_address_buf , 0x00,
           sizeof(byte) * NVRUIM_MAX_LBS_V2_MPC_ADDRESS_SIZE);
        nvruim_cache_control &= ~RUIM_LBS_V2_MPC_ADDRESS_EF_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_SUPPORT_LBS */

#ifdef FEATURE_UIM_SUPPORT_3GPD
      case RUIM_MIPFLAGS_EF_CACHE_BIT:
        nvruim_3gpd_mipflags_ef_buf = 0x00;
        nvruim_cache_control &= ~RUIM_MIPFLAGS_EF_CACHE_BIT;
        break;

      case RUIM_TCPCONFIG_EF_CACHE_BIT:
        memset(nvruim_3gpd_tcp_config_ef_buf, 0x00,
               sizeof(byte) * NVRUIM_MAX_TCPCONFIG_SIZE);
        nvruim_cache_control &= ~RUIM_TCPCONFIG_EF_CACHE_BIT;
        break;
#ifdef FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS
      case RUIM_DGC_EF_CACHE_BIT:
        memset(nvruim_3gpd_dcg_ef_buf, 0x00,
             sizeof(byte) * NVRUIM_MAX_DGC_SIZE);
        nvruim_cache_control &= ~RUIM_DGC_EF_CACHE_BIT ;
        break;
#endif /* FEATURE_NVRUIM_HANDLE_ENHANCED_3GPD_PARAMS */
      case RUIM_SIPUPP_EF_CACHE_BIT:
        memset(nvruim_sipupp_ef_buf, 0x00,
               sizeof(byte) * NVRUIM_MAX_SIPUPP_SIZE);
        memset(nvruim_sip_profile_bit_index, 0x00,
               sizeof(word) * NVRUIM_MAX_NUM_SIP_PROFILES);
        nvruim_cache_control &= ~RUIM_SIPUPP_EF_CACHE_BIT;
        break;

      case RUIM_MIPUPP_EF_CACHE_BIT:
        memset(nvruim_mipupp_ef_buf, 0x00,
               sizeof(byte) * NVRUIM_MAX_MIPUPP_SIZE);
        memset(nvruim_mip_profile_bit_index, 0x00,
               sizeof(word) * NVRUIM_MAX_NUM_MIP_PROFILES);
        nvruim_cache_control &= ~RUIM_MIPUPP_EF_CACHE_BIT;
        break;

      case RUIM_MIPSP_EF_CACHE_BIT:
        nvruim_mipsp_ef_buf = 0x00;
        nvruim_cache_control &= ~RUIM_MIPSP_EF_CACHE_BIT;
        break;

      case RUIM_SIPPAPSS_EF_CACHE_BIT:
        memset(nvruim_sippapss_ef_buf, 0x00,
             sizeof(byte) * NVRUIM_MAX_SIPPAPSS_SIZE);
        memset(nvruim_sip_pap_ss_profile_bit_index, 0x00,
             sizeof(word) * NVRUIM_MAX_NUM_SIP_PROFILES);
        nvruim_cache_control &= ~RUIM_SIPPAPSS_EF_CACHE_BIT;
        break;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
      case RUIM_SIPUPPEXT_EF_CACHE_BIT:
        memset(nvruim_sipuppext_ef_buf, 0x00,
             sizeof(byte) * NVRUIM_MAX_SIPUPPEXT_SIZE);
        nvruim_cache_control &= ~RUIM_SIPUPPEXT_EF_CACHE_BIT;
        break;

      case RUIM_SIPSP_EF_CACHE_BIT:
        nvruim_sipsp_ef_buf = 0;
        nvruim_cache_control &= ~RUIM_SIPSP_EF_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
      case RUIM_HRPD_NAI_CACHE_BIT:

        memset(nvruim_hrpdupp_ef_buf, 0x00,
               sizeof(byte) * NVRUIM_MAX_HRPDUPP_SIZE);
        nvruim_cache_control &= ~RUIM_HRPD_NAI_CACHE_BIT;
        break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */

      default:
        break;
    }/* end switch for uim file*/
    /* read the next file */
    nvruim_cache_mask = nvruim_cache_mask << 1;
  } 

  nvruim_nam_lock_buf = 0x00;
#ifdef FEATURE_UIM_SUPPORT_3GPD
  nvruim_3GPD_op_mode_ef_buf = 0x00;
#endif /* FEATURE_UIM_SUPPORT_3GPD */  
  /* update the refresh flag for cache */
  nvruim_refresh_file_cache &= ~nvruim_temp_cache_control;
} /* nvruim_process_refresh()*/
#endif /* FEATURE_RUIM_CDMA_REFRESH */


#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION nvruim_get_ipv4_digit_char_string

DESCRIPTION
  IPv4 addresses are usually written in dot-decimal notation, which consists
  of the four octets of the address expressed in decimal and separated by 
  periods. For example: 123.0.0.121.
  Data are stored in card as 8-bit ASCII string, where each character
   is store in one byte information. For example the given string is store as
   31 32 33 25 (123.)  First  Sub Address  
   30 25       (0.)    Second Sub Address
   30 25       (0.)    Third  Sub Address
   31 32 31    (0.)    Fourth Sub Address
  This function will get IPv4 address byte from EF data buffer and return 
  4 hex byte information. so for given example, The result will be 0x7B000079 
  where First Sub Adress is 123  -> 7B,
        Second Sub Address is 0  -> 0,
        Third Sub Address is  0  -> 0,
        Forth Sub Address is 121 -> 79,
  Note: Maximum length of a subaddress is 3.
DEPENDENCIES
  None.

RETURN VALUE
  0:  When data buffer does not have valid ipv4 address data.
  IPv4 Hex data: When data buffer have valid ipv4 address. 

SIDE EFFECTS
  None

===========================================================================*/ 
static uint32 nvruim_get_ipv4_digit_char_string (byte *data_ptr, byte data_len)
{
  /* we are only interested in 4 sub address */	
  byte   sub_address[4]        = {0x00, 0x00, 0x00, 0x00};
  byte   data_index            = 0;
  uint16 address               = 0;
  byte   index                 = 0;
  byte   sub_address_length    = 0;
  uint32 ipv4_address          = 0;

  if((NULL == data_ptr) || (0 == data_len))
  {
    MSG_ERROR(" There is no data to parse ipv4 address",0,0,0);
    return ipv4_address;
  }

  /* Read all '.' index and get the length of sub address
     0x2E: is ascii of '.' character,
     (index < 4): check for first 4 sub address
     */
  for (data_index = 0; (data_index < data_len) && (index < 4); data_index++)
  {
    if (data_ptr[data_index] == 0x2E)
    {
      /* Read Next offset */
      index++;
      sub_address_length=0;
      address =0;
    }
    else
    {
      /* Maximum 3 digit is allowed to express a sub address */
      if (sub_address_length > 3)
      {
        MSG_ERROR(" Sub Address length [0x%x] is greater than 3 byte",sub_address_length,0,0);
        return ipv4_address;
      }
      address = (address * 10) + (data_ptr[data_index] & 0x0F);
      /* Value of sub address is vary from 0x00 to 0xFF */       
      if (address > 0xFF)
      {
        MSG_ERROR("Sub Address[0x%x] 0x%x is greater than 0xFF",index,address,0);
        return ipv4_address;
      }
      sub_address[index]= address & 0xFF;
      sub_address_length++;
    }
  }/* for (data_index = 0; data_index < data_len; data_index++) */

  /* Reading sub address */
  for (index = 0; index < 4; index++)
  {
    ipv4_address = ipv4_address << 0x08;
    ipv4_address |= sub_address[index];
  } /* end of  for (index = 0; index < 4; index++)*/
  return ipv4_address;
} /* nvruim_get_ipv4_digit_char_string */	


/*===========================================================================

FUNCTION nvruim_get_ipv6_hex_char_string

DESCRIPTION
   IPv6 have eight groups of 4 hex digits separated by colons.
   it also can have 2 colon ?::? together but only once is allowed in entire string.
   For example, all are pointing to same addresss:
   2001:0db8:0000:0000:0000:0000:1428:57ab
   2001:0db8:0000:0000:0000::1428:57ab
   2001:0db8:0:0:0:0:1428:57ab
   2001:0db8:0:0::1428:57ab
   2001:0db8::1428:57ab
   2001:db8::1428:57ab

   Data are stored in card as 8-bit ASCII string, where each character is
   store in one byte information. for example given string 2001:db8::1428:57ab
   is stored as
   32 30 30 31 3A -> 2001:
   64 62 38 3A 3A -> db8::
   31 34 32 38 3A -> 1428:
   35 37 61 62    -> 57ab

   This function will get IPv6 address byte from EF data buffer and stored in
   array of 8 word index. So for given example, output will be
   Ipv6[0] = 0x2001
   Ipv6[1] = 0x0db8
   Ipv6[2] = 0x0000
   Ipv6[3] = 0x0000
   Ipv6[4] = 0x0000
   Ipv6[5] = 0x0000
   Ipv6[6] = 0x1428
   Ipv6[7] = 0x57ab
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  When data buffer have valid ipv6 address data.
  FALSE: When data buffer will not have valid ipv6 address. 

SIDE EFFECTS
  None

===========================================================================*/ 
static boolean nvruim_get_ipv6_hex_char_string (byte *data_ptr, byte data_len, uint16 *ipv6_address_ptr)
{
  /* max sub address length is 4 as 4 hex digit is allowed */
  byte sub_address_length    = 0;
  /* Store ipv6 address */  
  uint16 ipv6add[8];
  byte data_index            = 0;
  /* Maximum 7 colons can be defined in ipv6 address string */
  byte remaining_colon       = 7;

  /* check for :: presence in ef data */
  byte double_colon_index    = 0;
  byte index                 = 0;

  if((NULL == data_ptr) || (0 == data_len))
  {
    MSG_ERROR(" There is no data to parse ipv6 address",0,0,0);
    return FALSE;
  }

  if (NULL == ipv6_address_ptr)
  {
    MSG_ERROR(" ipv6_address_ptr is null",0,0,0);
    return FALSE;
  }

  memset((uint16 *)(ipv6add),0x0000, sizeof(ipv6add));

  /* Find out all ':'index, 0x3A is ascii of ':'
     (index < 8) : check for first 8 sub address */
  for (data_index = 0; (data_index < data_len) && (index < 8); data_index++)
  {
    if (0x3A == data_ptr[data_index])
    {
      /* ':' is found */	    
      index = index +1;
      /* set sub address length to 0 as new sub address will be start after : */      
      sub_address_length = 0;
      /* decrement remaining colon as colon is found */
      if(remaining_colon > 0)
      {
        remaining_colon = remaining_colon -1;
      }

      /* double colon is allowed in ef data, so if we did not get all
         7 colon then, Check next index */
      if ((0x3A == data_ptr[data_index +1]) && (remaining_colon > 0))
      {
        /* if next index have colon */	      
        if(0x00 == double_colon_index)
        {
          /* set double colon index */		
          double_colon_index = index + 1;
        }
        else
        {
          /* if we already found double colon then return error as
             Only one double colons is allowed */
          MSG_ERROR("Only one double colon allowed ",0,0,0);
          return FALSE;
        }
      } /*  if (data_ptr[data_index +1] == 0x3A)*/
    }
    else
    {
      /* Read data_ptr if it is not colon */
      /* max sub address length is 4 as 4 hex digit is allowed for a sub address*/	    
      if (sub_address_length > 0x04)
      {
        MSG_ERROR("Sub Address [0x%x] is more than 4 byte",0,0,0);
        return FALSE;
      }
      /* Increment sub address length */
      sub_address_length++;
      
      ipv6add[index] = ipv6add[index] << 0x04;
      switch(data_ptr[data_index])
      {
        case ASCII_A:
        case ASCII_a:
          ipv6add[index] = ipv6add[index] | 0x0A;
          break;

        case ASCII_B:
        case ASCII_b:
          ipv6add[index] = ipv6add[index] | 0x0B;
          break;

        case ASCII_C:
        case ASCII_c:
          ipv6add[index] = ipv6add[index] | 0x0C;
          break;

        case ASCII_D:
        case ASCII_d:
          ipv6add[index] = ipv6add[index] | 0x0D;
          break;

        case ASCII_e:
        case ASCII_E:
          ipv6add[index] = ipv6add[index] | 0x0E;
          break;

        case ASCII_F:
        case ASCII_f:
          ipv6add[index] = ipv6add[index] | 0x0F;
          break;

        case ASCII_0:
        case ASCII_1:
        case ASCII_2:
        case ASCII_3:
        case ASCII_4:
        case ASCII_5:
        case ASCII_6:
        case ASCII_7:
        case ASCII_8:
        case ASCII_9:
          /* 0x31 -- 0x39 */
          ipv6add[index] = ipv6add[index] | (data_ptr[data_index] & 0x0F);
          break;

        default:
          MSG_ERROR("Char 0x%x is not allowed in IPv6 address",data_ptr[data_index],0,0);
          return FALSE;
      }/* switch(data_ptr[data_index]) */
    }/* end of else */
  } /* end of for */

  /* For given example  2001:0db8::1428:57ab the ip address contain following data
     ipv6add[0] = 0x2001, ipv6add[1]= 0x0db8,  ipv6add[2]= 0x0000,  ipv6add[3]=1428 
     and  ipv6add[4] = 0x57ab */

  /* Check for remaining colon */  
  if (remaining_colon)
  {
    /* we are not getting 7 colons in data string */
    /* check for double colon index */	  
    if (0x00 == double_colon_index)
    {
      MSG_ERROR("Invalid format of IPv6  missing colons 0x%x",remaining_colon,0,0);
      return FALSE;
    }
    /* case when double_colon_index is found, then we need to add 0 for remainin 
       colon, such that we get the 7 colon in string. 
       for example : 2001:0db8::1428:57ab this string will treat as 
       2001:0db8:0:0:0:0:1428:57ab */
    
    /* index is always pointing to the last sub part address here and double colon
       index is never 0. Now we are moving data to right from double colon index 
       to IPV6 higher byte index (7). 
       for given string: index = 4 (found 4 colon), double colon index is 2.
       so in this loop we are moving IPV6[4] to IPV6[7] at last and IPV6[3] to IPV6[6]
       and IPV6[2] to IPV6[5].
       */
    for (data_index = 0x07; (index >= double_colon_index) && (index < 8); data_index --)
    {
      ipv6add[data_index] = ipv6add[index];
      ipv6add[index] = 0x0000;
      index = index - 1;
    }

    /* We have shifted the higher data to higher IPV6 address index, which come
       after double colon index. Now data_index is pointing to current index
       Here we are setting 0 for remaining index which is in between data_index 
       and double colon index. 
       For example data_index = 4 and double colon = 2 */ 
    while ((remaining_colon > 0) && (data_index > double_colon_index))
    {
      ipv6add[data_index] = 0x0000;
      data_index --;
      remaining_colon --;
    }
  }/* if (remaining_colon) */

  /* Parse entire data string */
  memcpy((uint16 *)ipv6_address_ptr,(uint16 *)ipv6add, sizeof(ipv6add));
  return TRUE;
}/* nvruim_get_ipv6_hex_char_string */	
#endif /* FEATURE_UIM_SUPPORT_LBS */


/*===================================================================
FUNCTION UIM_HRPD_DISABLED_CARD

DESCRIPTION
  Check if this is a hrpd disabled card.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  TRUE  if misconfigured card
  FALSE if a correct card

SIDE EFFECTS
  None

===================================================================*/
static boolean uim_hrpd_disabled_card()
{
#ifdef FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND	
  word    mcc              = 0;

  MSG_HIGH("uim_hrpd_disabled_card()",0,0,0);

  if (!(nvruim_cache_control & RUIM_IMSI_M_CACHE_BIT))
  {
    /* Read IMSI */
    cmd.access_uim.access    = UIM_READ_F;
    cmd.access_uim.num_bytes = sizeof(nvruim_imsi_type);
    cmd.access_uim.data_ptr  = (byte *) &(nvruim_imsi_m_cache_buf);
    cmd.access_uim.item      = UIM_CDMA_IMSI_M;
    cmd.access_uim.offset    = 0;
    if (nvruim_access(&cmd) != UIM_PASS)
    {
      MSG_HIGH("Not able to read IMSI",0,0,0);
      return (FALSE);
    }
    else
    {
      nvruim_cache_control |= RUIM_IMSI_M_CACHE_BIT;
    }
  }
  mcc = (nvruim_imsi_m_cache_buf.imsi_mcc[1] << 8) |
          nvruim_imsi_m_cache_buf.imsi_mcc[0];
  MSG_HIGH("IMSI of R-UIM card is 0x%x", mcc, 0, 0);

  /* Check if mcc is same as that of hrpd disabled card */
  if (mcc != NVRUIM_NON_3GPD_CDMA_CARD_MCC)
  {
    return (FALSE);
  }

  if(!(nvruim_cache_control & RUIM_HRPD_NAI_CACHE_BIT))
  {
    /* Read HRPD data */
    cmd.access_uim.access    = UIM_READ_F;
    cmd.access_uim.num_bytes = NVRUIM_MAX_HRPDUPP_SIZE;
    cmd.access_uim.data_ptr  = (byte *) &(nvruim_hrpdupp_ef_buf);
    cmd.access_uim.offset    = 0;
    cmd.access_uim.item      = UIM_CDMA_HRPD_HRPDUPP;
    if (nvruim_access(&cmd) != UIM_PASS)
    {
      MSG_HIGH("SW1 is 0x%x and SW2 is 0x%x", nvruim_sw1, nvruim_sw2,0);
      if (nvruim_sw1 == SW1_REFERENCE && nvruim_sw2== SW2_NOT_FOUND)
      {
        MSG_HIGH("EF HRPDUPP is not provisioned",0,0,0);
        return (TRUE);
      }
#ifdef FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED      
      else
      {
        MSG_HIGH("EF HRPDUPP is provisioned",0,0,0);
        return (FALSE);
      }
#endif /*FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED */      
    }
  }
  if (((nvruim_hrpdupp_ef_buf[0]==0xFF)&&(nvruim_hrpdupp_ef_buf[1]==0xFF)) ||
        ((nvruim_hrpdupp_ef_buf[0]==0x00)&&(nvruim_hrpdupp_ef_buf[1]==0x00)))
  {
    MSG_HIGH("wrong NAI",0,0,0);
    return (TRUE);
  }
  return (FALSE);
#else  
  return FALSE;
#endif /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND */  
} /* uim_hrpd_disabled_card */

/*===================================================================
FUNCTION NVRUIM_PPP_CAVE_FALLBACK

DESCRIPTION
  Sets the hrpd control variable to Cave

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==================================================================*/
void nvruim_ppp_cave_fallback( void
)
{
#if defined(FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED)
  MSG_HIGH("MD5 failed, falling back to cave",0,0,0);
  nvruim_hrpd_control  = UIM_AN_HRPD_USE_CAVE;
  uim_use_cave_default = TRUE;
#else
  MSG_HIGH("Fallback to CAVE is not enabled ",0,0,0);  
#endif /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED */
} /* nvruim_ppp_cave_fallback */


/*===================================================================
FUNCTION NVRUIM_GENERATE_AN_NAI_WITH_IMSI

DESCRIPTION
  Get IMSI of R-UIM.

DEPENDENCIES
  Non-reentrant

RETURN VALUE

SIDE EFFECTS
None

==================================================================*/
boolean nvruim_generate_an_nai_with_imsi(
  uint8 *user_id_len_ptr, 
  char *user_id_info_ptr
)
{
#if defined(FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND) && defined(FEATURE_UIM_CAVE_AN_AUTH)
  dword min1;
  word min2;
  byte mnc;
  word mcc;
  if ((NULL == user_id_len_ptr) || (NULL == user_id_info_ptr))
  {
    MSG_ERROR("NULL Pointer for generating AN NAI with IMSI",0,0,0);
    return FALSE;
  }
  *user_id_len_ptr = NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH;
  min1 = (nvruim_imsi_m_cache_buf.imsi_s1[2] << 16) |
          (nvruim_imsi_m_cache_buf.imsi_s1[1] << 8) |
          (nvruim_imsi_m_cache_buf.imsi_s1[0]);

  min2 = (nvruim_imsi_m_cache_buf.imsi_s2[1] << 8) |
          nvruim_imsi_m_cache_buf.imsi_s2[0];

  mnc = nvruim_imsi_m_cache_buf.imsi_11_12;
  mcc = (nvruim_imsi_m_cache_buf.imsi_mcc[1] << 8) |
        nvruim_imsi_m_cache_buf.imsi_mcc[0];
  /* mcc = 359, which converts to country code of 460 */
  if(mcc != NVRUIM_NON_3GPD_CDMA_CARD_MCC)
    return(FALSE);
  memcpy((byte*)user_id_info_ptr,
          nvruim_an_cave_nai,
          NVRUIM_CHINA_UNICOM_AN_CAVE_NAI_LENGTH);

  if (!nvruim_decode_min((byte*)user_id_info_ptr, &mcc, &mnc, &min1, &min2))
  {
    MSG_HIGH("decode min failed!, mcc=0x%x, mnc=0x%x, min1=0x%x", mcc, mnc, min1);
    return FALSE;
  }
  return TRUE;
#else /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND && FEATURE_UIM_CAVE_AN_AUTH */
  (void) user_id_len_ptr;
  (void) user_id_info_ptr;
  return FALSE;
#endif /* FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND && FEATURE_UIM_CAVE_AN_AUTH */
} /* nvruim_generate_an_nai_with_imsi */


/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_EXT_SUPPORT

DESCRIPTION
This function returns 3GPD_EXT service availability

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
 TRUE : When RUIM card has 3GPD_EXT service available
 FALSE: When RUIM card does not have 3GPD_EXT service 

SIDE EFFECTS
None.

===========================================================================*/
boolean nvruim_data_3gpd_ext_support( void 
)
{
#if defined(FEATURE_NV_RUIM) && defined(FEATURE_UIM_SUPPORT_3GPD)
  return nvruim_3gpd_ext_support;
#else
  return FALSE;
#endif /* FEATURE_NV_RUIM && FEATURE_UIM_SUPPORT_3GPD  */
} /* nvruim_data_3gpd_ext_support */

#ifdef FEATURE_OEMOMH
boolean nvruim_sms_ems_support(void)
{
    if(nvruim_smscap_svc)
    {
        if((nvruim_smscap_ef_buf[2]&NVRUIM_SMSCAP_STD_EMS_MASK) == 0)
        {
            return FALSE;
        }
    }
    return TRUE;
}
#endif

#endif /*FEATURE_NV_RUIM*/
