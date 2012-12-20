/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U T I L I T Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI
  before the command is being sent to UIM Module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2010 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/sqa/mmgsdi/src/2/mmgsdiutil.c#5 $ $DateTime: 2010/12/28 00:49:03 $ $Author: msaurabh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/28/10   shr/ms  Fixed input path/AID length check when copying file
                   access information
11/18/10   yt      Fixed data length check utility
10/12/10   shr     File path of depth greater than 5 file IDs needs
                   to be rejected when handling REFRESH file registration
10/19/10   shr     Send sync cmd to UIM only when in GSDI context
01/15/10   shr     Fixed incorrect access of freed memory locations
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
05/05/09   js      Fixed warnings
04/29/09   js      Added prototype for static functions, fixed compiler warning
04/17/09   mib     client id not passed in events to applications
04/17/09   vs      Fix to allow clients to receive all streaming APDU responses
03/25/09   sun     Added support for Virtuim
03/19/09   js      Support for Enhanced NAA Refresh by file path
01/27/09   kk      Fixed featurization, which is causing compilation warning
01/21/09   kk      Added support for converting file path to mmgsdi file enum
                   and featurize ef access condition check.
11/19/08   js      Moved definition of MMGSDIUTIL_WORD_SIZE to mmgsdiutil.h
11/13/08   nk      Added MSG in mmgsdi_util_pkcs15_free_client_table_and_build_cnf
                   to fix compiler Warning
11/05/08   tml     Fixed get session id utility function issue
10/22/08   kk      Extra check to handle invalid file path str tag in PKCS15
                   and fixed some lint errors
10/06/08   jk      Allow GSM SIM access in 1X only
10/07/08   kk      Fixed handling of invalid tags and missing tags in create
                   pkcs15 table
09/30/08   kk      Fixed incorrect boundry check on EF-DIR data during
                   PKCS15 table creation
09/24/08   js      Klockwork fix for potential null ptr dereferencing
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Support for Generate Key API and parsing of RUN CAVE
08/06/08   tml     Fixed Klocwork issues
08/08/08   vs      Fixed compile errors related to featurization
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/10/08   sp      Add file type to extra param in read
07/10/08   sp      Add file type to extra param in read
07/17/08   sun     Fixed Parsing of response for Run Cave
07/17/08   kk      Added support for PKCS 15
07/16/08   jk      Fixed Compile Warnings
07/07/08   sp      Remove message
06/12/08   srr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/23/08   sp      Don't clean up the global that was populated as a result
                   of EF-DIR aid read during the reselection of an app.
06/21/08   kk      Added PKCS15 Support
06/04/08   tml     Added sw to cnf header and added additional debug message
04/30/08   kk      Added support for MMS EFs
04/07/08   jar     Fixed Compile Error introduced by Changelist 634222
03/28/08   sun     Added support for OMA BCAST
03/20/08   nk      Added OMH EF Support
03/03/08   nk      Updated call to mmgsdi_uim_icc_select
02/28/08   tml     Fixed MKEY parsing response parsing issue
02/26/08   jk      Allow POWER DOWN command to go onto UIM even when repeated reset
                   is happening
01/10/08   nk      Fixed Lint Error
01/02/08   jk      Insert checks and mechanism to withdraw command from uim_cmd_q
                   before deallocating command buffer
12/10/07   sun     Verify PIN Internally when SAP is Disconnected
12/06/07   sun     Removed app type comparison while comparing aids
11/19/07   js      Radio Access Technology Balancing support
11/15/07   sun     Added support for NAA refresh
11/08/07   vs      Added OMH feature support
10/02/07   tml     Added proprietary app session support
09/27/07   tml     Lint fixes
09/10/07   tml     Add MFLO supports
08/15/07   jk      Changes to support MMGSDI DATA Authentication
08/14/07   sun     Fixed Buff Offset Calculation
07/16/07   sp      Fixed the size of memcpy in
                   mmgsdi_util_get_file_path_from_enum
07/02/07   sun     Added Support for ODD Ins Class
06/28/07   sun     Removed checks for uim_cmd_ptr
06/26/07   sp      Fix compiler error
06/25/07   sp      Convert static variables to pointers to avoid stack overflow
06/18/07   sun     UIM CMD Ptr should not be null - added error messages
06/05/07   nk      Fixed jcdma queueing of mmgsdi uim report rsp
05/31/07   sp      Fixed possible data abort.
05/23/07   nk      Added default value for esn_me_change_flag
05/21/07   jk      Check for sanity timer expiry
05/22/07   sun     Free the mgs ptr allocated in gsdilib.c
05/21/07   wli     support JCDMA RUIM feature
05/18/07   sp      Do not check the no of apps present in icc protocol
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/26/07   sp      Added functioon to get path from enum
04/20/07   sun     Added additional cases for MMGSDI-GSDI Status conversion
04/17/07   sp      Featurize for LTK
04/12/07   sun     Featurized Phonebook
04/11/09   sp      Fixed potential buffer overflow
04/06/07   sun     Send information about all the apps in the CARD_INIT_
                   COMPLETED event
04/04/07   sun     Added Flag to check if there are actually any files being
                   refreshed
03/28/07   sun     Check if additional files are being read during refresh
                   fcn
03/28/07   tml     Fixed compilation error
03/26/07   tml     Add Onchip memory alignment
03/19/07   sun     Fixed Lint Error
03/02/07   sun     Update App PIN with correct PS_DO information relevant to
                   that app only
03/05/07   nk      Add PIN validation function
03/02/07   sun     Added function to check for file attributes
02/21/07   sun     Removed Global PIN from GSDI
02/27/07   jk      Added featurization for Memory Reduction
02/09/07   sk      Added support for 1000 phonebook entries.
02/01/07   tml     Fixed mmgsdi state for SIM_INIT refresh and null pointer
                   check
02/01/07   jk      Added check for null pointer
01/24/07   sun     Do not set mmgsdi cache based on gsdi cache. Let MMGSDI set
                   the cache after all the MMGSDI PINs have been cached.
01/15/07   sp      Fix error with ThinUI builds.
01/11/07   sk      Return MMGSDI_NOT_FOUND if file is not init in path lookup
                   table.
01/15/07   sun     Return the number of unblock retries if the command is
                   to unblock the pin
01/11/07   sun     Fixed mapping of pin status
01/11/07   sun     Fixed Compilation
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/19/06   tml     Add support for path transparent access, send pin events
                   at select_aid state if possible
12/27/06   sp      Back out pin event changes.
12/13/06   tml     Do not check against empty aid info from the global
                   app_info_ptr
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
12/11/06   tml     Ensure proper event notification for late registered clients
11/14/06   sp      Fixed lint error
10/26/06   tml     Get Key ref from client ID
11/07/06   sun     Fixed Lint Error
11/06/06   sun     Removed Featurization
10/25/06   tml     Added initial support for Universal PIN replacement support
10/25/06   tml     Added function to check if the client is trying to access
                   selected application
10/17/06   sun     Do not wait for Card Selection if NV item is set
09/06/06   tml     Added support for refresh AID notification
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/20/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/18/06   sun     Send the original PIN Operation request when building a
                   new pin request
08/16/06   jar     Modifed mmgsdi_uim_file_enum_table to include the mapping
                   of MMGSDI_TELECOM_ANRC and MMGSDI_TELECOM_ANRC1.
08/07/06   tml     Added ACL support
08/07/06   sun     Divert GSDI Pin calls to MMGSDI
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/29/06   sun     Lint Fixes
06/12/06   sun     Set mmgsdi_sap_state  whenever mmgsdi_state is updated
06/21/06   pv      Do not send commands to UIM when UIM has already notified
                   an error.
06/15/06   tml     Fixed compilation warning
06/14/06   tml     Fixed compilation issue
06/12/06   pv      Add code to handle MMGSDI_SESSION_CLOSE_EVT and
                   call mmgsdi_recover_after_reset while not in recovery,
                   call uim_recovery_cmd to send commands to uim while in
                   recovery mode.  Use MMGSDI_SYNCH_UIM_REPORT_SIG instead of
                   GSDI_UIM_REPORT_SIG.
06/12/06   tml     lint
05/23/06   tml     lint and BDN, rehablitate and invalidate support
05/16/06   tml     Added utility to see if client reg table is free or not
05/09/06   tml     Fix memory overwrite and access potential  null pointer
05/01/06   tml     Ensure client ID will not be zero
04/14/06   tml     Added utility function to initialize cache data
                   and set init flag
04/13/06   sun     FOR SAP, Allow UIM FAIL responses also to go BT Client
04/13/06   jar     Added ONCHIP SIM Support
04/05/06   tml     Add NO FILE ENUM and Card Removed support
03/21/06   tml     added cfis and missing service table support and lint
03/03/06   tml     Added support for CACHE_SEL
02/09/06   tml     Added utility functions for conversion and lint fix
02/16/06   sp      Support for EF_EHPLMN
02/14/06   nk      Merged for ARR caching and by path changes
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/28/05   tml     PNN and OPL support
11/14/05    pv     change signature for functions that needs lookup
                   to return mmgsdi_return_enum_type and accept an
                   output parameter.
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header and cache read/write
11/03/05   sun     Added support for Refresh
10/28/05   sun     Added SAP Report Rsp case for memory alignment
10/25/05   tml     Added ISIM File supports
10/18/05   tml     Added in_synch info to the cache
09/29/05   tml     Client dereg supports
08/30/05   jar     Added utility functions and support for sending Card Status
                   Information.
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Support API to power down and power up SIM/USIM Card
08/17/05   pv      added mmgsdi_util_print_data_to_diag function to print
                   data to the diag interface
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/18/05   jk      Fixed Compile Issues for 1x Targets
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version
02/26/05   sst     Added MMGSDIBT support

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "intconv.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "mmgsdilib.h"
#include "gsdi.h"
#include "uim.h"
#include "mmgsdi_session.h"
#include "mmgsdi_recover.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_cnf.h"
#include "gsdi_nv.h"
#include "uimi.h"

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif

#ifdef FEATURE_MMGSDI_CDMA
#include "bit.h"
#endif /*FEATURE_MMGSDI_CDMA*/
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

const mmgsdi_uim_file_enum_table_type mmgsdi_uim_file_enum_table[MMGSDI_NO_FILE_ENUM] =
{
  /* 0 */
  {MMGSDI_ICCID,               UIM_ICCID},
  {MMGSDI_ELP,                 UIM_ELP},
  {MMGSDI_DIR,                 UIM_DIR},
  {MMGSDI_ARR,                 UIM_ARR},
  /* EFs in DF CDMA */
  {MMGSDI_CDMA_CC,             UIM_CDMA_CC},
  {MMGSDI_CDMA_IMSI_M,         UIM_CDMA_IMSI_M},
  {MMGSDI_CDMA_IMSI_T,         UIM_CDMA_IMSI_T},
  {MMGSDI_CDMA_TMSI,           UIM_CDMA_TMSI},
  {MMGSDI_CDMA_ANALOG_HOME_SID,UIM_CDMA_ANALOG_HOME_SID},
  {MMGSDI_CDMA_ANALOG_OP_PARAMS, UIM_CDMA_ANALOG_OP_PARAMS},

  /* 10 */
  {MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND, UIM_CDMA_ANALOG_LOCN_AND_REGN_IND},
  {MMGSDI_CDMA_HOME_SID_NID,   UIM_CDMA_HOME_SID_NID},
  {MMGSDI_CDMA_ZONE_BASED_REGN_IND, UIM_CDMA_ZONE_BASED_REGN_IND},
  {MMGSDI_CDMA_SYS_REGN_IND,   UIM_CDMA_SYS_REGN_IND},
  {MMGSDI_CDMA_DIST_BASED_REGN_IND, UIM_CDMA_DIST_BASED_REGN_IND},
  {MMGSDI_CDMA_ACCOLC,         UIM_CDMA_ACCOLC},
  {MMGSDI_CDMA_CALL_TERM_MODE_PREF, UIM_CDMA_CALL_TERM_MODE_PREF},
  {MMGSDI_CDMA_SCI,            UIM_CDMA_SCI},
  {MMGSDI_CDMA_ANALOG_CHAN_PREF, UIM_CDMA_ANALOG_CHAN_PREF},
  {MMGSDI_CDMA_PRL,            UIM_CDMA_PRL},

  /* 20 */
  {MMGSDI_CDMA_RUIM_ID,        UIM_CDMA_RUIM_ID},
  {MMGSDI_CDMA_SVC_TABLE,      UIM_CDMA_CDMA_SVC_TABLE},
  {MMGSDI_CDMA_SPC,            UIM_CDMA_SPC},
  {MMGSDI_CDMA_OTAPA_SPC_ENABLE, UIM_CDMA_OTAPA_SPC_ENABLE},
  {MMGSDI_CDMA_NAM_LOCK,       UIM_CDMA_NAM_LOCK},
  {MMGSDI_CDMA_OTASP_OTAPA_FEATURES, UIM_CDMA_OTASP_OTAPA_FEATURES},
  {MMGSDI_CDMA_SERVICE_PREF,   UIM_CDMA_SERVICE_PREF},
  {MMGSDI_CDMA_ESN_ME,         UIM_CDMA_ESN_ME},
  {MMGSDI_CDMA_RUIM_PHASE,     UIM_CDMA_RUIM_PHASE},
  {MMGSDI_CDMA_PREF_LANG,      UIM_CDMA_PREF_LANG},

  /* 30 */
  {MMGSDI_CDMA_UNASSIGNED_1,   UIM_CDMA_UNASSIGNED_1},
  {MMGSDI_CDMA_SMS,            UIM_CDMA_SMS},
  {MMGSDI_CDMA_SMS_PARAMS,     UIM_CDMA_SMS_PARAMS},
  {MMGSDI_CDMA_SMS_STATUS,     UIM_CDMA_SMS_STATUS},
  {MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE, UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE},
  {MMGSDI_CDMA_UNASSIGNED_2,   UIM_CDMA_UNASSIGNED_2},
  {MMGSDI_CDMA_HOME_SVC_PVDR_NAME, UIM_CDMA_HOME_SVC_PVDR_NAME},
  {MMGSDI_CDMA_UIM_ID_USAGE_IND, UIM_CDMA_UIM_ID_USAGE_IND},
  {MMGSDI_CDMA_ADM_DATA,       UIM_CDMA_ADM_DATA},
  {MMGSDI_CDMA_MSISDN,         UIM_CDMA_MSISDN},

  /* 40 */
  {MMGSDI_CDMA_MAXIMUM_PRL,    UIM_CDMA_MAXIMUM_PRL},
  {MMGSDI_CDMA_SPC_STATUS,     UIM_CDMA_SPC_STATUS},
  {MMGSDI_CDMA_ECC,            UIM_CDMA_ECC},
  {MMGSDI_CDMA_3GPD_ME3GPDOPC, UIM_CDMA_3GPD_ME3GPDOPC},
  {MMGSDI_CDMA_3GPD_3GPDOPM,   UIM_CDMA_3GPD_3GPDOPM},
  {MMGSDI_CDMA_3GPD_SIPCAP,    UIM_CDMA_3GPD_SIPCAP},
  {MMGSDI_CDMA_3GPD_MIPCAP,    UIM_CDMA_3GPD_MIPCAP},
  {MMGSDI_CDMA_3GPD_SIPUPP,    UIM_CDMA_3GPD_SIPUPP},
  {MMGSDI_CDMA_3GPD_MIPUPP,    UIM_CDMA_3GPD_MIPUPP},
  {MMGSDI_CDMA_3GPD_SIPSP,     UIM_CDMA_3GPD_SIPSP},

  /* 50 */
  {MMGSDI_CDMA_3GPD_MIPSP,     UIM_CDMA_3GPD_MIPSP},
  {MMGSDI_CDMA_3GPD_SIPPAPSS,  UIM_CDMA_3GPD_SIPPAPSS},
  {MMGSDI_CDMA_UNASSIGNED_3,   UIM_CDMA_UNASSIGNED_3},
  {MMGSDI_CDMA_UNASSIGNED_4,   UIM_CDMA_UNASSIGNED_4},
  {MMGSDI_CDMA_PUZL,           UIM_CDMA_PUZL},
  {MMGSDI_CDMA_MAXPUZL,        UIM_CDMA_MAXPUZL},
  {MMGSDI_CDMA_MECRP,          UIM_CDMA_MECRP},
  {MMGSDI_CDMA_HRPDCAP,        UIM_CDMA_HRPDCAP},
  {MMGSDI_CDMA_HRPDUPP,        UIM_CDMA_HRPDUPP},
  {MMGSDI_CDMA_CSSPR,          UIM_CDMA_CSSPR},

  /* 60 */
  {MMGSDI_CDMA_ATC,            UIM_CDMA_ATC},
  {MMGSDI_CDMA_EPRL,           UIM_CDMA_EPRL},
  {MMGSDI_CDMA_BCSMS_CONFIG,   UIM_CDMA_BCSMS_CONFIG},
  {MMGSDI_CDMA_BCSMS_PREF,     UIM_CDMA_BCSMS_PREF},
  {MMGSDI_CDMA_BCSMS_TABLE,    UIM_CDMA_BCSMS_TABLE},
  {MMGSDI_CDMA_BCSMS_PARAMS,   UIM_CDMA_BCSMS_PARAMS},
  {MMGSDI_CDMA_MMS_NOTIF,      UIM_CDMA_MMS_NOTIF},
  {MMGSDI_CDMA_MMS_EXT8,       UIM_CDMA_MMS_EXT8},
  {MMGSDI_CDMA_MMS_ICP,        UIM_CDMA_MMS_ICP},
  {MMGSDI_CDMA_MMS_UP,         UIM_CDMA_MMS_UP},

  /* 70 */
  {MMGSDI_CDMA_SMS_CAP,        UIM_CDMA_SMS_CAP},
  {MMGSDI_CDMA_3GPD_IPV6CAP,   UIM_CDMA_3GPD_IPV6CAP},
  {MMGSDI_CDMA_3GPD_MIPFLAGS,  UIM_CDMA_3GPD_MIPFLAGS},
  {MMGSDI_CDMA_3GPD_TCPCONFIG, UIM_CDMA_3GPD_TCPCONFIG},
  {MMGSDI_CDMA_3GPD_DGC,       UIM_CDMA_3GPD_DGC},
  {MMGSDI_CDMA_BROWSER_CP,     UIM_CDMA_BROWSER_CP},
  {MMGSDI_CDMA_BROWSER_BM,     UIM_CDMA_BROWSER_BM},
  {MMGSDI_CDMA_3GPD_SIPUPPEXT, UIM_CDMA_3GPD_SIPUPPEXT},
  {MMGSDI_CDMA_MMS_CONFIG,     UIM_CDMA_MMS_CONFIG},
  {MMGSDI_CDMA_JAVA_DURL,      UIM_CDMA_JAVA_DURL},

  /* 80 */
  {MMGSDI_CDMA_3GPD_MIPUPPEXT, UIM_CDMA_3GPD_MIPUPPEXT},
  {MMGSDI_CDMA_BREW_DLOAD,     UIM_CDMA_BREW_DLOAD},
  {MMGSDI_CDMA_BREW_TSID,      UIM_CDMA_BREW_TSID},
  {MMGSDI_CDMA_BREW_SID,       UIM_CDMA_BREW_SID},
  {MMGSDI_CDMA_LBS_XCONFIG,    UIM_CDMA_LBS_XCONFIG},
  {MMGSDI_CDMA_LBS_XSURL,      UIM_CDMA_LBS_XSURL},
  {MMGSDI_CDMA_LBS_V2CONFIG,   UIM_CDMA_LBS_V2CONFIG},
  {MMGSDI_CDMA_LBS_V2PDEADDR,  UIM_CDMA_LBS_V2PDEADDR},
  {MMGSDI_CDMA_LBS_V2MPCADDR,  UIM_CDMA_LBS_V2MPCADDR},
  {MMGSDI_CDMA_BREW_AEP,       UIM_CDMA_BREW_AEP},

  /* 90 */
  {MMGSDI_CDMA_MODEL,          UIM_CDMA_MODEL},
  {MMGSDI_CDMA_RC,             UIM_CDMA_RC},
  {MMGSDI_CDMA_APP_LABELS,     UIM_CDMA_APP_LABELS},
  {MMGSDI_CDMA_USER_AGENT_STRING, UIM_CDMA_USER_AGENT_STRING},
  {MMGSDI_CDMA_GID_REFERENCES, UIM_CDMA_GID_REFERENCES},
  {MMGSDI_CDMA_ME_DOWNLOADABLE_DATA, UIM_CDMA_ME_DOWNLOADABLE_DATA},
  {MMGSDI_CDMA_ME_SETTING_DATA, UIM_CDMA_ME_SETTING_DATA},
  {MMGSDI_CDMA_ME_USER_DATA,   UIM_CDMA_ME_USER_DATA},
  {MMGSDI_CDMA_RESERVED2,      UIM_CDMA_RESERVED2},
  {MMGSDI_CDMA_RESERVED1,      UIM_CDMA_RESERVED1},

  /* 100 */
  {MMGSDI_CDMA_UIM_SVC_TABLE,  UIM_CDMA_UIM_SVC_TABLE},
  {MMGSDI_CDMA_UIM_FEATURES,   UIM_CDMA_UIM_FEATURES},
  {MMGSDI_CDMA_UIM_VERSION,    UIM_CDMA_UIM_VERSION},
  {MMGSDI_CDMA_HRPD_HRPDUPP,   UIM_CDMA_HRPD_HRPDUPP},
  {MMGSDI_CDMA_SF_EUIM_ID,     UIM_CDMA_SF_EUIM_ID},
  {MMGSDI_CDMA_IMSI_STATUS,    UIM_CDMA_IMSI_STATUS},
  {MMGSDI_CDMA_GID1,           UIM_CDMA_GID1},
  {MMGSDI_CDMA_GID2,           UIM_CDMA_GID2},
  {MMGSDI_CDMA_3GPD_OP_CAP,    UIM_CDMA_3GPD_ME3GPDOPC},
  /* EFs in DF GSM */
  {MMGSDI_GSM_LP,              UIM_GSM_LP},

  /* 110 */
  {MMGSDI_GSM_IMSI,            UIM_GSM_IMSI},
  {MMGSDI_GSM_KC,              UIM_GSM_KC},
  {MMGSDI_GSM_PLMN,            UIM_GSM_PLMN},
  {MMGSDI_GSM_HPLMN,           UIM_GSM_HPLMN},
  {MMGSDI_GSM_ACM_MAX,         UIM_GSM_ACM_MAX},
  {MMGSDI_GSM_SST,             UIM_GSM_SST},
  {MMGSDI_GSM_ACM,             UIM_GSM_ACM},
  {MMGSDI_GSM_GID1,            UIM_GSM_GID1},
  {MMGSDI_GSM_GID2,            UIM_GSM_GID2},
  {MMGSDI_GSM_SPN,             UIM_GSM_SPN},

  /* 120 */
  {MMGSDI_GSM_PUCT,            UIM_GSM_PUCT},
  {MMGSDI_GSM_CBMI,            UIM_GSM_CBMI},
  {MMGSDI_GSM_BCCH,            UIM_GSM_BCCH},
  {MMGSDI_GSM_ACC,             UIM_GSM_ACC},
  {MMGSDI_GSM_FPLMN,           UIM_GSM_FPLMN},
  {MMGSDI_GSM_LOCI,            UIM_GSM_LOCI},
  {MMGSDI_GSM_AD,              UIM_GSM_AD},
  {MMGSDI_GSM_PHASE,           UIM_GSM_PHASE},
  {MMGSDI_GSM_VGCS,            UIM_GSM_VGCS},
  {MMGSDI_GSM_VGCSS,           UIM_GSM_VGCSS},

  /* 130 */
  {MMGSDI_GSM_VBS,             UIM_GSM_VBS},
  {MMGSDI_GSM_VBSS,            UIM_GSM_VBSS},
  {MMGSDI_GSM_EMLPP,           UIM_GSM_EMLPP},
  {MMGSDI_GSM_AAEM,            UIM_GSM_AAEM},
  {MMGSDI_GSM_CBMID,           UIM_GSM_CBMID},
  {MMGSDI_GSM_ECC,             UIM_GSM_ECC},
  {MMGSDI_GSM_CBMIR,           UIM_GSM_CBMIR},
  {MMGSDI_GSM_DCK,             UIM_GSM_DCK},
  {MMGSDI_GSM_CNL,             UIM_GSM_CNL},
  {MMGSDI_GSM_NIA,             UIM_GSM_NIA},

  /* 140 */
  {MMGSDI_GSM_KCGPRS,          UIM_GSM_KCGPRS},
  {MMGSDI_GSM_LOCIGPRS,        UIM_GSM_LOCIGPRS},
  {MMGSDI_GSM_SUME,            UIM_GSM_SUME},
  {MMGSDI_GSM_PLMNWACT,        UIM_GSM_PLMNWACT},
  {MMGSDI_GSM_OPLMNWACT,       UIM_GSM_OPLMNWACT},
  {MMGSDI_GSM_HPLMNACT,        UIM_GSM_HPLMNACT},
  {MMGSDI_GSM_CPBCCH,          UIM_GSM_CPBCCH},
  {MMGSDI_GSM_INVSCAN,         UIM_GSM_INVSCAN},
  {MMGSDI_GSM_RPLMNAT,         UIM_GSM_RPLMNAT},
  {MMGSDI_GSM_PNN,             UIM_GSM_PNN},

  /* 150 */
  {MMGSDI_GSM_OPL,             UIM_GSM_OPL},
  {MMGSDI_GSM_MBDN,            UIM_GSM_MBDN},
  {MMGSDI_GSM_EXT6,            UIM_GSM_EXT6},
  {MMGSDI_GSM_MBI,             UIM_GSM_MBI},
  {MMGSDI_GSM_MWIS,            UIM_GSM_MWIS},
  {MMGSDI_GSM_EXT1,            UIM_GSM_EXT1},
  {MMGSDI_GSM_SPDI,            UIM_GSM_SPDI},
  {MMGSDI_GSM_CFIS,            UIM_GSM_CFIS},
  {MMGSDI_GSM_VMWI,            UIM_GSM_VMWI},
  {MMGSDI_GSM_SVC_STR_TBL,     UIM_GSM_SVC_STR_TBL},

  /* 160 */
  {MMGSDI_GSM_CFF,             UIM_GSM_CFF},
  {MMGSDI_GSM_ONS,             UIM_GSM_ONS},
  {MMGSDI_GSM_CSP,             UIM_GSM_CSP},
  {MMGSDI_GSM_CPHSI,           UIM_GSM_CPHSI},
  {MMGSDI_GSM_MN,              UIM_GSM_MN},
  /* EFs at SoLSA */
  {MMGSDI_GSM_SAI,             UIM_GSM_SAI},
  {MMGSDI_GSM_SLL,             UIM_GSM_SLL},
  /* EFs at MExE Level */
  {MMGSDI_GSM_MEXE_ST,         UIM_GSM_MEXE_ST},
  {MMGSDI_GSM_ORPK,            UIM_GSM_ORPK},
  {MMGSDI_GSM_ARPK,            UIM_GSM_ARPK},

  /* 170 */
  {MMGSDI_GSM_TPRPK,           UIM_GSM_TPRPK},
  /* image file */
  {MMGSDI_GSM_IMG,             UIM_GSM_IMG},
  {MMGSDI_GSM_IMG1INST1,       UIM_GSM_IMG1INST1},
  {MMGSDI_GSM_IMG1INST2,       UIM_GSM_IMG1INST2},
  {MMGSDI_GSM_IMG1INST3,       UIM_GSM_IMG1INST3},
  {MMGSDI_GSM_IMG2INST1,       UIM_GSM_IMG2INST1},
  {MMGSDI_GSM_IMG2INST2,       UIM_GSM_IMG2INST2},
  {MMGSDI_GSM_IMG2INST3,       UIM_GSM_IMG2INST3},
  {MMGSDI_GSM_IMG3INST1,       UIM_GSM_IMG3INST1},
  {MMGSDI_GSM_IMG3INST2,       UIM_GSM_IMG3INST2},

  /* 180 */
  {MMGSDI_GSM_IMG3INST3,       UIM_GSM_IMG3INST3},
  {MMGSDI_IMAGE,               UIM_IMAGE},
  /* EFs at the telecom DF in GSM and CDMA */
  {MMGSDI_TELECOM_ADN,         UIM_TELECOM_ADN},
  {MMGSDI_TELECOM_FDN,         UIM_TELECOM_FDN},
  {MMGSDI_TELECOM_SMS,         UIM_TELECOM_SMS},
  {MMGSDI_TELECOM_CCP,         UIM_TELECOM_CCP},
  {MMGSDI_TELECOM_ECCP,        UIM_TELECOM_ECCP},
  {MMGSDI_TELECOM_MSISDN,      UIM_TELECOM_MSISDN},
  {MMGSDI_TELECOM_SMSP,        UIM_TELECOM_SMSP},
  {MMGSDI_TELECOM_SMSS,        UIM_TELECOM_SMSS},

  /* 190 */
  {MMGSDI_TELECOM_LND,         UIM_TELECOM_LND},
  {MMGSDI_TELECOM_SDN,         UIM_TELECOM_SDN},
  {MMGSDI_TELECOM_EXT1,        UIM_TELECOM_EXT1},
  {MMGSDI_TELECOM_EXT2,        UIM_TELECOM_EXT2},
  {MMGSDI_TELECOM_EXT3,        UIM_TELECOM_EXT3},
  {MMGSDI_TELECOM_BDN,         UIM_TELECOM_BDN},
  {MMGSDI_TELECOM_EXT4,        UIM_TELECOM_EXT4},
  {MMGSDI_TELECOM_SMSR,        UIM_TELECOM_SMSR},
  {MMGSDI_TELECOM_CMI,         UIM_TELECOM_CMI},
  {MMGSDI_TELECOM_SUME,        UIM_TELECOM_SUME},

  /* 200 */
  {MMGSDI_TELECOM_ARR,         UIM_TELECOM_ARR},
  /* EFs at DF GRAPHICS under DF Telecom */
  {MMGSDI_IMAGE_FILE,          UIM_IMAGE},
  /* EFs at the DF PHONEBOOK under  DF Telecom */
  {MMGSDI_TELECOM_PBR,         UIM_TELECOM_PBR},
  {MMGSDI_TELECOM_CCP1,        UIM_TELECOM_CCP1},
  {MMGSDI_TELECOM_UID,         UIM_TELECOM_UID},
  {MMGSDI_TELECOM_PSC,         UIM_TELECOM_PSC},
  {MMGSDI_TELECOM_CC,          UIM_TELECOM_CC},
  {MMGSDI_TELECOM_PUID,        UIM_TELECOM_PUID},
  /* EFs selected by path at the telecom DF in GSM and CDMA */
  {MMGSDI_TELECOM_IAP,         UIM_TELECOM_BY_PATH_IAP},
  {MMGSDI_TELECOM_PBC,         UIM_TELECOM_BY_PATH_PBC},

  /* 210 */
  {MMGSDI_TELECOM_GRP,         UIM_TELECOM_BY_PATH_GRP},
  {MMGSDI_TELECOM_AAS,         UIM_TELECOM_BY_PATH_AAS},
  {MMGSDI_TELECOM_GAS,         UIM_TELECOM_BY_PATH_GAS},
  {MMGSDI_TELECOM_ANR,         UIM_TELECOM_BY_PATH_ANR},
  {MMGSDI_TELECOM_SNE,         UIM_TELECOM_BY_PATH_SNE},
  {MMGSDI_TELECOM_EMAIL,       UIM_TELECOM_BY_PATH_EMAIL},
  {MMGSDI_TELECOM_ANRA,        UIM_TELECOM_BY_PATH_ANRA},
  {MMGSDI_TELECOM_ANRB,        UIM_TELECOM_BY_PATH_ANRB},
  {MMGSDI_TELECOM_ANR1,        UIM_TELECOM_BY_PATH_ANR1},
  {MMGSDI_TELECOM_ANRA1,       UIM_TELECOM_BY_PATH_ANRA1},

  /* 220 */
  {MMGSDI_TELECOM_ANRB1,       UIM_TELECOM_BY_PATH_ANRB1},
  {MMGSDI_TELECOM_ADN1,        UIM_TELECOM_BY_PATH_ADN1},
  {MMGSDI_TELECOM_PBC1,        UIM_TELECOM_BY_PATH_PBC1},
  {MMGSDI_TELECOM_GRP1,        UIM_TELECOM_BY_PATH_GRP1},
  {MMGSDI_TELECOM_SNE1,        UIM_TELECOM_BY_PATH_SNE1},
  {MMGSDI_TELECOM_UID1,        UIM_TELECOM_BY_PATH_UID1},
  {MMGSDI_TELECOM_EMAIL1,      UIM_TELECOM_BY_PATH_EMAIL1},
  {MMGSDI_TELECOM_IAP1,        UIM_TELECOM_BY_PATH_IAP1},
  {MMGSDI_TELECOM_ANRC,        UIM_TELECOM_BY_PATH_ANRC},
  {MMGSDI_TELECOM_ANRC1,       UIM_TELECOM_BY_PATH_ANRC1},

  /* 230 */
  {MMGSDI_TELECOM_ANR2,        UIM_TELECOM_BY_PATH_ANR2},
  {MMGSDI_TELECOM_ANRA2,       UIM_TELECOM_BY_PATH_ANRA2},
  {MMGSDI_TELECOM_ANRB2,       UIM_TELECOM_BY_PATH_ANRB2},
  {MMGSDI_TELECOM_ANRC2,       UIM_TELECOM_BY_PATH_ANRC2},
  {MMGSDI_TELECOM_ADN2,        UIM_TELECOM_BY_PATH_ADN2},
  {MMGSDI_TELECOM_PBC2,        UIM_TELECOM_BY_PATH_PBC2},
  {MMGSDI_TELECOM_GRP2,        UIM_TELECOM_BY_PATH_GRP2},
  {MMGSDI_TELECOM_SNE2,        UIM_TELECOM_BY_PATH_SNE2},
  {MMGSDI_TELECOM_UID2,        UIM_TELECOM_BY_PATH_UID2},
  {MMGSDI_TELECOM_EMAIL2,      UIM_TELECOM_BY_PATH_EMAIL2},

  /* 240 */
  {MMGSDI_TELECOM_IAP2,        UIM_TELECOM_BY_PATH_IAP2},
  {MMGSDI_TELECOM_ANR3,        UIM_TELECOM_BY_PATH_ANR3},
  {MMGSDI_TELECOM_ANRA3,       UIM_TELECOM_BY_PATH_ANRA3},
  {MMGSDI_TELECOM_ANRB3,       UIM_TELECOM_BY_PATH_ANRB3},
  {MMGSDI_TELECOM_ANRC3,       UIM_TELECOM_BY_PATH_ANRC3},
  {MMGSDI_TELECOM_ADN3,        UIM_TELECOM_BY_PATH_ADN3},
  {MMGSDI_TELECOM_PBC3,        UIM_TELECOM_BY_PATH_PBC3},
  {MMGSDI_TELECOM_GRP3,        UIM_TELECOM_BY_PATH_GRP3},
  {MMGSDI_TELECOM_SNE3,        UIM_TELECOM_BY_PATH_SNE3},
  {MMGSDI_TELECOM_UID3,        UIM_TELECOM_BY_PATH_UID3},

  /* 250 */
  {MMGSDI_TELECOM_EMAIL3,      UIM_TELECOM_BY_PATH_EMAIL3},
  {MMGSDI_TELECOM_IAP3,        UIM_TELECOM_BY_PATH_IAP3},
  /* EFs of USIM ADF */
  {MMGSDI_USIM_LI,             UIM_USIM_LI},
  {MMGSDI_USIM_IMSI,           UIM_USIM_IMSI},
  {MMGSDI_USIM_KEYS,           UIM_USIM_KEYS},
  {MMGSDI_USIM_KEYSPS,         UIM_USIM_KEYSPS},
  {MMGSDI_USIM_PLMNWACT,       UIM_USIM_PLMNWACT},
  {MMGSDI_USIM_UPLMNSEL,       UIM_USIM_UPLMNSEL},
  {MMGSDI_USIM_HPLMN,          UIM_USIM_HPLMN},
  {MMGSDI_USIM_ACM_MAX,        UIM_USIM_ACM_MAX},

  /* 260 */
  {MMGSDI_USIM_UST,            UIM_USIM_UST},
  {MMGSDI_USIM_ACM,            UIM_USIM_ACM},
  {MMGSDI_USIM_GID1,           UIM_USIM_GID1},
  {MMGSDI_USIM_GID2,           UIM_USIM_GID2},
  {MMGSDI_USIM_SPN,            UIM_USIM_SPN},
  {MMGSDI_USIM_PUCT,           UIM_USIM_PUCT},
  {MMGSDI_USIM_CBMI,           UIM_USIM_CBMI},
  {MMGSDI_USIM_ACC,            UIM_USIM_ACC},
  {MMGSDI_USIM_FPLMN,          UIM_USIM_FPLMN},
  {MMGSDI_USIM_LOCI,           UIM_USIM_LOCI},

  /* 270 */
  {MMGSDI_USIM_AD,             UIM_USIM_AD},
  {MMGSDI_USIM_CBMID,          UIM_USIM_CBMID},
  {MMGSDI_USIM_ECC,            UIM_USIM_ECC},
  {MMGSDI_USIM_CBMIR,          UIM_USIM_CBMIR},
  {MMGSDI_USIM_PSLOCI,         UIM_USIM_PSLOCI},
  {MMGSDI_USIM_FDN,            UIM_USIM_FDN},
  {MMGSDI_USIM_SMS,            UIM_USIM_SMS},
  {MMGSDI_USIM_MSISDN,         UIM_USIM_MSISDN},
  {MMGSDI_USIM_SMSP,           UIM_USIM_SMSP},
  {MMGSDI_USIM_SMSS,           UIM_USIM_SMSS},

  /* 280 */
  {MMGSDI_USIM_SDN,            UIM_USIM_SDN},
  {MMGSDI_USIM_EXT2,           UIM_USIM_EXT2},
  {MMGSDI_USIM_EXT3,           UIM_USIM_EXT3},
  {MMGSDI_USIM_SMSR,           UIM_USIM_SMSR},
  {MMGSDI_USIM_ICI,            UIM_USIM_ICI},
  {MMGSDI_USIM_OCI,            UIM_USIM_OCI},
  {MMGSDI_USIM_ICT,            UIM_USIM_ICT},
  {MMGSDI_USIM_OCT,            UIM_USIM_OCT},
  {MMGSDI_USIM_EXT5,           UIM_USIM_EXT5},
  {MMGSDI_USIM_CCP2,           UIM_USIM_CCP2},

  /* 290 */
  {MMGSDI_USIM_EMLPP,          UIM_USIM_EMLPP},
  {MMGSDI_USIM_AAEM,           UIM_USIM_AAEM},
  {MMGSDI_USIM_GMSI,           UIM_USIM_GMSI},
  {MMGSDI_USIM_HIDDENKEY,      UIM_USIM_HIDDENKEY},
  {MMGSDI_USIM_BDN,            UIM_USIM_BDN},
  {MMGSDI_USIM_EXT4,           UIM_USIM_EXT4},
  {MMGSDI_USIM_CMI,            UIM_USIM_CMI},
  {MMGSDI_USIM_EST,            UIM_USIM_EST},
  {MMGSDI_USIM_ACL,            UIM_USIM_ACL},
  {MMGSDI_USIM_DCK,            UIM_USIM_DCK},

  /* 300 */
  {MMGSDI_USIM_CNL,            UIM_USIM_CNL},
  {MMGSDI_USIM_START_HFN,      UIM_USIM_START_HFN},
  {MMGSDI_USIM_THRESHOLD,      UIM_USIM_THRESHOLD},
  {MMGSDI_USIM_OPLMNWACT,      UIM_USIM_OPLMNWACT},
  {MMGSDI_USIM_OPLMNSEL,       UIM_USIM_OPLMNSEL},
  {MMGSDI_USIM_HPLMNWACT,      UIM_USIM_HPLMNWACT},
  {MMGSDI_USIM_ARR,            UIM_USIM_ARR},
  {MMGSDI_USIM_RPLMNACT,       UIM_USIM_RPLMNACT},
  {MMGSDI_USIM_NETPAR,         UIM_USIM_NETPAR},
  {MMGSDI_USIM_PNN,            UIM_USIM_PNN},

  /* 310 */
  {MMGSDI_USIM_OPL,            UIM_USIM_OPL},
  {MMGSDI_USIM_MBDN,           UIM_USIM_MBDN},
  {MMGSDI_USIM_EXT6,           UIM_USIM_EXT6},
  {MMGSDI_USIM_MBI,            UIM_USIM_MBI},
  {MMGSDI_USIM_MWIS,           UIM_USIM_MWIS},
  {MMGSDI_USIM_SPDI,           UIM_USIM_SPDI},
  {MMGSDI_USIM_EHPLMN,         UIM_USIM_EHPLMN},
  {MMGSDI_USIM_CFIS,           UIM_USIM_CFIS},
  /* MBMS */
  {MMGSDI_USIM_GBABP,          UIM_USIM_GBABP},
  {MMGSDI_USIM_GBANL,          UIM_USIM_GBANL},

  /* 320 */
  {MMGSDI_USIM_MSK,            UIM_USIM_MSK},
  {MMGSDI_USIM_MUK,            UIM_USIM_MUK},
  /* CPHS */
  {MMGSDI_USIM_VMWI,           UIM_USIM_VMWI},
  {MMGSDI_USIM_SVC_STR_TBL,    UIM_USIM_SVC_STR_TBL},
  {MMGSDI_USIM_CFF,            UIM_USIM_CFF},
  {MMGSDI_USIM_ONS,            UIM_USIM_ONS},
  {MMGSDI_USIM_CSP,            UIM_USIM_CSP},
  {MMGSDI_USIM_CPHSI,          UIM_USIM_CPHSI},
  {MMGSDI_USIM_MN,             UIM_USIM_MN},
  /* EFs selected by path of USIM ADF */
  {MMGSDI_USIM_ADN,            UIM_USIM_ADN},

  /* 330 */
  {MMGSDI_USIM_IAP,            UIM_USIM_IAP},
  {MMGSDI_USIM_PBC,            UIM_USIM_PBC},
  {MMGSDI_USIM_EXT1,           UIM_USIM_EXT1},
  {MMGSDI_USIM_GRP,            UIM_USIM_GRP},
  {MMGSDI_USIM_AAS,            UIM_USIM_AAS},
  {MMGSDI_USIM_AAS1,           UIM_USIM_AAS1},
  {MMGSDI_USIM_GAS,            UIM_USIM_GAS},
  {MMGSDI_USIM_GAS1,           UIM_USIM_GAS1},
  {MMGSDI_USIM_ANR,            UIM_USIM_ANR},
  {MMGSDI_USIM_SNE,            UIM_USIM_SNE},

  /* 340 */
  {MMGSDI_USIM_EMAIL,          UIM_USIM_EMAIL},
  {MMGSDI_USIM_ANRA,           UIM_USIM_ANRA},
  {MMGSDI_USIM_ANRB,           UIM_USIM_ANRB},
  {MMGSDI_USIM_ANRC,           UIM_USIM_ANRC},
  {MMGSDI_USIM_ANR1,           UIM_USIM_ANR1},
  {MMGSDI_USIM_ANRA1,          UIM_USIM_ANRA1},
  {MMGSDI_USIM_ANRB1,          UIM_USIM_ANRB1},
  {MMGSDI_USIM_ANRC1,          UIM_USIM_ANRC1},
  {MMGSDI_USIM_ADN1,           UIM_USIM_ADN1},
  {MMGSDI_USIM_PBC1,           UIM_USIM_PBC1},

  /* 350 */
  {MMGSDI_USIM_GRP1,           UIM_USIM_GRP1},
  {MMGSDI_USIM_SNE1,           UIM_USIM_SNE1},
  {MMGSDI_USIM_UID1,           UIM_USIM_UID1},
  {MMGSDI_USIM_EMAIL1,         UIM_USIM_EMAIL1},
  {MMGSDI_USIM_IAP1,           UIM_USIM_IAP1},
  {MMGSDI_USIM_CCP1,           UIM_USIM_CCP1},
  {MMGSDI_USIM_UID,            UIM_USIM_UID},
  {MMGSDI_USIM_ANR2,           UIM_USIM_ANR2},
  {MMGSDI_USIM_ANRA2,          UIM_USIM_ANRA2},
  {MMGSDI_USIM_ANRB2,          UIM_USIM_ANRB2},

  /* 360 */
  {MMGSDI_USIM_ANRC2,          UIM_USIM_ANRC2},
  {MMGSDI_USIM_ADN2,           UIM_USIM_ADN2},
  {MMGSDI_USIM_PBC2,           UIM_USIM_PBC2},
  {MMGSDI_USIM_GRP2,           UIM_USIM_GRP2},
  {MMGSDI_USIM_SNE2,           UIM_USIM_SNE2},
  {MMGSDI_USIM_UID2,           UIM_USIM_UID2},
  {MMGSDI_USIM_EMAIL2,         UIM_USIM_EMAIL2},
  {MMGSDI_USIM_IAP2,           UIM_USIM_IAP2},
  {MMGSDI_USIM_ANR3,           UIM_USIM_ANR3},
  {MMGSDI_USIM_ANRA3,          UIM_USIM_ANRA3},

  /* 370 */
  {MMGSDI_USIM_ANRB3,          UIM_USIM_ANRB3},
  {MMGSDI_USIM_ANRC3,          UIM_USIM_ANRC3},
  {MMGSDI_USIM_ADN3,           UIM_USIM_ADN3},
  {MMGSDI_USIM_PBC3,           UIM_USIM_PBC3},
  {MMGSDI_USIM_GRP3,           UIM_USIM_GRP3},
  {MMGSDI_USIM_SNE3,           UIM_USIM_SNE3},
  {MMGSDI_USIM_UID3,           UIM_USIM_UID3},
  {MMGSDI_USIM_EMAIL3,         UIM_USIM_EMAIL3},
  {MMGSDI_USIM_IAP3,           UIM_USIM_IAP3},
  /*EFs at DF PHONEBOOK under USIM ADF */
  {MMGSDI_USIM_PBR,            UIM_USIM_PBR},

  /* 380 */
  {MMGSDI_USIM_PSC,            UIM_USIM_PSC},
  {MMGSDI_USIM_CC,             UIM_USIM_CC},
  {MMGSDI_USIM_PUID,           UIM_USIM_PUID},
  /*EFs at DF GSM under USIM ADF */
  {MMGSDI_USIM_KC,             UIM_USIM_KC},
  {MMGSDI_USIM_KCGPRS,         UIM_USIM_KCGPRS},
  {MMGSDI_USIM_CPBCCH,         UIM_USIM_CPBCCH},
  {MMGSDI_USIM_INVSCAN,        UIM_USIM_INVSCAN},
  /*EFs at DF MEXe under USIM ADF */
  {MMGSDI_USIM_MEXE_ST,        UIM_USIM_MEXE_ST},
  {MMGSDI_USIM_ORPK,           UIM_USIM_ORPK},
  {MMGSDI_USIM_ARPK,           UIM_USIM_ARPK},

  /* 390 */
  {MMGSDI_USIM_TPRPK,          UIM_USIM_TPRPK},
  /* EFs for MMS Notification */
  {MMGSDI_USIM_MMSN,           UIM_USIM_MMSN},
  {MMGSDI_USIM_MMSICP,         UIM_USIM_MMSICP},
  {MMGSDI_USIM_MMSUP,          UIM_USIM_MMSUP},
  {MMGSDI_USIM_MMSUCP,         UIM_USIM_MMSUCP},
  {MMGSDI_USIM_EXT8,           UIM_USIM_EXT8},
  /* Efs for PKCS #15 support */
  {MMGSDI_PKCS15_ODF,          UIM_PKCS15_ODF},
  {MMGSDI_PKCS15_PRKDF,        UIM_PKCS15_PRKDF},
  {MMGSDI_PKCS15_PUKDF,        UIM_PKCS15_PUKDF},
  {MMGSDI_PKCS15_SKDF,         UIM_PKCS15_SKDF},

  /* 400 */
  {MMGSDI_PKCS15_CDF,          UIM_PKCS15_CDF},
  {MMGSDI_PKCS15_DODF,         UIM_PKCS15_DODF},
  {MMGSDI_PKCS15_AODF,         UIM_PKCS15_AODF},
  {MMGSDI_PKCS15_TI,           UIM_PKCS15_TI},
  {MMGSDI_PKCS15_US,           UIM_PKCS15_US},
  /* EFs of DCS1800 DF */
  {MMGSDI_DCS1800_IMSI,        UIM_DCS1800_IMSI},
  {MMGSDI_DCS1800_KC,          UIM_DCS1800_KC},
  {MMGSDI_DCS1800_PLMN,        UIM_DCS1800_PLMN},
  {MMGSDI_DCS1800_SST,         UIM_DCS1800_SST},
  {MMGSDI_DCS1800_BCCH,        UIM_DCS1800_BCCH},

  /* 410 */
  {MMGSDI_DCS1800_ACC,         UIM_DCS1800_ACC},
  {MMGSDI_DCS1800_FPLMN,       UIM_DCS1800_FPLMN},
  {MMGSDI_DCS1800_LOCI,        UIM_DCS1800_LOCI},
  {MMGSDI_DCS1800_AD,          UIM_DCS1800_AD},
  /* EFs of WIM DF */
  {MMGSDI_WIM_ODF,             UIM_WIM_ODF},
  {MMGSDI_WIM_TOKEN_INFO,      UIM_WIM_TOKEN_INFO},
  {MMGSDI_WIM_UNUSED_SPACE,    UIM_WIM_UNUSED_SPACE},
  /* EFs of ISIM Application */
  {MMGSDI_ISIM_IMPI,           UIM_ISIM_IMPI},
  {MMGSDI_ISIM_DOMAIN,         UIM_ISIM_DOMAIN},
  {MMGSDI_ISIM_IMPU,           UIM_ISIM_IMPU},

  /* 420 */
  {MMGSDI_ISIM_ARR,            UIM_ISIM_ARR},
  {MMGSDI_ISIM_KEYS,           UIM_ISIM_KEYS},
  {MMGSDI_ISIM_AD,             UIM_ISIM_AD},
  /* Orange */
  {MMGSDI_SIM_7F40_PROP1_DFS,  UIM_ORANGE_DFS},
  {MMGSDI_SIM_7F40_PROP1_D2FS, UIM_ORANGE_D2FS},
  {MMGSDI_SIM_7F40_PROP1_CSP2, UIM_ORANGE_CSP2},
  {MMGSDI_SIM_7F40_PROP1_PARAMS, UIM_ORANGE_PARAMS},
  /* EFs of ORANGE DF under USIM ADF */
  {MMGSDI_USIM_7F40_PROP1_DFS,     UIM_USIM_ORANGE_DFS},
  {MMGSDI_USIM_7F40_PROP1_D2FS,    UIM_USIM_ORANGE_D2FS},
  {MMGSDI_USIM_7F40_PROP1_CSP2,    UIM_USIM_ORANGE_CSP2},

  /* 430 */
  {MMGSDI_USIM_7F40_PROP1_PARAMS,  UIM_USIM_ORANGE_PARAMS},
  /* EFs under Cingular */
  {MMGSDI_SIM_7F66_PROP1_ACT_HPLMN,   UIM_CINGULAR_ACT_HPLMN},
  {MMGSDI_SIM_7F66_PROP1_SPT_TABLE,   UIM_CINGULAR_SPT_TABLE},
  /* EFs of Cingular */
  {MMGSDI_USIM_7F66_PROP1_SPT_TABLE,  UIM_USIM_SPT_TABLE},
  {MMGSDI_USIM_7F66_PROP1_RAT,        UIM_USIM_RAT},
  {MMGSDI_USIM_7F66_PROP1_ACT_HPLMN,  UIM_USIM_ACT_HPLMN},
  /* EFs of Home Zone */
  {MMGSDI_SIM_7F43_PROP1_HZ,                    UIM_HZI_HZ},
  {MMGSDI_SIM_7F43_PROP1_CACHE1,                UIM_HZI_CACHE1},
  {MMGSDI_SIM_7F43_PROP1_CACHE2,                UIM_HZI_CACHE2},
  {MMGSDI_SIM_7F43_PROP1_CACHE3,                UIM_HZI_CACHE3},

  /* 440 */
  {MMGSDI_SIM_7F43_PROP1_CACHE4,                UIM_HZI_CACHE4},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1,   UIM_HZI_SUBSCRIBED_LAC_CI_1},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2,   UIM_HZI_SUBSCRIBED_LAC_CI_2},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3,   UIM_HZI_SUBSCRIBED_LAC_CI_3},
  {MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4,   UIM_HZI_SUBSCRIBED_LAC_CI_4},
  {MMGSDI_SIM_7F43_PROP1_TAGS,                  UIM_HZI_TAGS},
  {MMGSDI_SIM_7F43_PROP1_SETTINGS,              UIM_HZI_UHZI_SETTINGS},
  /* EFs under MediaFLO/RAN DF */
  {MMGSDI_MFLO_RAN_RFR,                         UIM_MFLO_RAN_RFR},
  {MMGSDI_MFLO_RAN_RF,                          UIM_MFLO_RAN_RF},
  /* EFs under MediaFLO/MFAPP DF */
  {MMGSDI_MFLO_MFAPP_MFR,                       UIM_MFLO_MFAPP_MFR},

  /* 450 */
  {MMGSDI_MFLO_MFAPP_BCSID,                     UIM_MFLO_MFAPP_BCSID},
  {MMGSDI_MFLO_MFAPP_MFT,                       UIM_MFLO_MFAPP_MFT},
  {MMGSDI_MFLO_MFAPP_SUBTYPE,                   UIM_MFLO_MFAPP_SUBTYPE},
  {MMGSDI_MFLO_MFAPP_SUBPROF,                   UIM_MFLO_MFAPP_SUBPROF},
  {MMGSDI_MFLO_MFAPP_UTSD,                      UIM_MFLO_MFAPP_UTSD},
  {MMGSDI_MFLO_MFAPP_UIP,                       UIM_MFLO_MFAPP_UIP},
  {MMGSDI_MFLO_MFAPP_PCSET,                     UIM_MFLO_MFAPP_PCSET},
  {MMGSDI_MFLO_MFAPP_MCHSET,                    UIM_MFLO_MFAPP_MCHSET},
  {MMGSDI_MFLO_MFAPP_RFUD,                      UIM_MFLO_MFAPP_RFUD}
};

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/

#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_TO_UIM_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding uim_items_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_FOUND       : uim item not found in the conversion table


   SIDE EFFECTS:
     None
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_convert_to_uim_items_enum (
  mmgsdi_file_enum_type mmgsdi_file,
  uim_items_enum_type  *uim_file_ptr
)
{
  uint32 i = 0;
  MMGSDIUTIL_RETURN_IF_NULL(uim_file_ptr);
  MMGSDIUTIL_RETURN_IF_EXCEEDS(mmgsdi_file, MMGSDI_NO_FILE_ENUM);

  for (i = 0; i< (int)MMGSDI_NO_FILE_ENUM; i++)
  {
    if (mmgsdi_uim_file_enum_table[i].mmgsdi_file == mmgsdi_file)
    {
      *uim_file_ptr = mmgsdi_uim_file_enum_table[i].uim_file;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_NOT_FOUND;
} /* mmgsdi_util_convert_to_uim_items_enum */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_FROM_UIM_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the uim_items_enum_type
     and returns the corresponding mmgsdi_file_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Unable to conver the items enum

   SIDE EFFECTS:
     None
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_convert_from_uim_items_enum (
  uim_items_enum_type    uim_file,
  mmgsdi_file_enum_type *mmgsdi_file_ptr
)
{
  uint32 i = 0;
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_file_ptr);

  for (i = 0; i< (int)MMGSDI_NO_FILE_ENUM; i++)
  {
    if (mmgsdi_uim_file_enum_table[i].uim_file == uim_file)
    {
      *mmgsdi_file_ptr = mmgsdi_uim_file_enum_table[i].mmgsdi_file;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
} /* mmgsdi_util_convert_from_uim_items_enum */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_UIM_ITEMS_ENUM

   DESCRIPTION:
     This function returns the uim_items_enum_type based on the access method
     and the service_table_use boolean
     MMGSDI_EF_ENUM_ACCESS:
       - return the corresponding uim_items_enum_type if there is a match
       - return UIM_NO_SUCH_FILE if there is no match
     MMGSDI_BY_PATH_ACCESS:
       1) When this function is called because it wants to find out the
          existence of corresponding path in the
          if ( service_table_use == TRUE )
            if ( path == {MF} {DF} or {ADF} )
              return UIM_NO_SUCH_FILE
            if ( path == EF )
              return corresponding uim_items_enum_type if there is a match
       2) When this function is called because it is used for non service
          table check, e.g., populating the item in the uim command for
          Access, Select functions, the following uim_items_enum_type will
          be returned
          if ( service_table_use == FALSE )
            if ( path == {MF} {DF} or {ADF} )
              return UIM_NO_SUCH_FILE
            if ( path == EF )
              return UIM_EF_BY_PATH

   DEPENDENCIES:
     None

   LIMITATIONS:
     DIRECTORY_ENUM_ACCESS is currently not being supported

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_determine_uim_items_enum (
  const mmgsdi_access_type  *access_ptr,
  uim_items_enum_type *uim_file_ptr,
  boolean              service_table_use
)
{
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(access_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(uim_file_ptr);

  /* Initialize to UIM_NO_SUCH_FILE */
  *uim_file_ptr = UIM_NO_SUCH_ITEM;

  switch (access_ptr->access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
    /* Convert to the corresponding UIM items enum type */
    mmgsdi_status = mmgsdi_util_convert_to_uim_items_enum(
      access_ptr->file.file_enum,
      uim_file_ptr);
    break;

  case MMGSDI_BY_APP_ID_ACCESS:
    *uim_file_ptr = UIM_EF_BY_PATH;
    break;

  case MMGSDI_BY_PATH_ACCESS:
    *uim_file_ptr = UIM_EF_BY_PATH;
    break;

  case MMGSDI_DF_ENUM_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERR("Bad Access Method 0x%x",
                            access_ptr->access_method, 0, 0);
    mmgsdi_status = MMGSDI_ERROR;
    break;
  }
  return mmgsdi_status;
} /* mmgsdi_util_determine_uim_items_enum */ /*lint !e715 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_EF_ENUM_DIR_LEVELS

   DESCRIPTION:
     This function determines the directory level for the EF enum type
     Result will be populated into the passed in pointer

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_determine_ef_enum_df_levels(
  mmgsdi_file_enum_type       file_enum,
  uim_dir_type               *uim_dir_level_ptr
  )
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uim_items_enum_type     uim_items_file = UIM_NO_SUCH_ITEM;
  int32                   uim_ef_start_value = 0;
  MMGSDIUTIL_RETURN_IF_NULL(uim_dir_level_ptr);
  MMGSDIUTIL_RETURN_IF_EXCEEDS(file_enum, MMGSDI_NO_FILE_ENUM);

  /* Convert to the corresponding UIM items enum type */
  mmgsdi_status = mmgsdi_util_convert_to_uim_items_enum(
      file_enum,
      &uim_items_file);

  if (mmgsdi_status != MMGSDI_SUCCESS)
    return mmgsdi_status;

  uim_ef_start_value = (((int)uim_items_file >> 8)&0x0F);

  switch(uim_ef_start_value)
  {
  case UIM_EF_UNDER_MF_START: /* 0x0 */
    *uim_dir_level_ptr = 0x3F00;
    break;

  case UIM_RUIM_EF_START: /* 0x1 */
    *uim_dir_level_ptr = 0x7F25;
    break;

  case UIM_GSM_EF_START: /* 0x2 */
    *uim_dir_level_ptr = 0x7F20;
    break;

  case UIM_TELECOM_EF_START: /* 0x3 */
    *uim_dir_level_ptr = 0x7F10;
    break;

  case UIM_USIM_EF_START: /* 0x4 */
    *uim_dir_level_ptr = 0x7FFF;
    break;

  case UIM_DCS1800_EF_START: /* 0x05 */
    *uim_dir_level_ptr = 0x7F21;
    break;

  case UIM_WIM_EF_START: /* 0x6 */
  case UIM_ORANGE_EF_START: /* 0x7 */
    *uim_dir_level_ptr = 0xFFFF;
    break;

  default:
    ERR("Invalid UIM EF level category - %d",uim_ef_start_value,0,0);
    *uim_dir_level_ptr = 0xFFFF;
    break;
  } /* switch */

  return mmgsdi_status;
} /* mmgsdi_determine_ef_enum_df_levels */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_UIM_DIR_TYPE

   DESCRIPTION:
     This function determines the uim directory type for MF, DF, ADF and EFs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_determine_uim_dir_type (
  const mmgsdi_access_type   *mmgsdi_access_ptr,
  uim_dir_type               *uim_dir_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  MMGSDIUTIL_RETURN_IF_NULL(uim_dir_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_access_ptr);

  *uim_dir_ptr = 0;

  switch(mmgsdi_access_ptr->access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
    MMGSDIUTIL_RETURN_IF_EXCEEDS(mmgsdi_access_ptr->file.file_enum,
                                MMGSDI_NO_FILE_ENUM);
    mmgsdi_status = mmgsdi_determine_ef_enum_df_levels(
                                 mmgsdi_access_ptr->file.file_enum,
                                 uim_dir_ptr);
    break;
  case MMGSDI_BY_APP_ID_ACCESS:
    *uim_dir_ptr = 0x7FFF;
    break;
  case MMGSDI_DF_ENUM_ACCESS:
    *uim_dir_ptr = (uim_dir_type)mmgsdi_access_ptr->file.df_enum;
    break;
  default:
    break;
    /* Future:
         MF => UIM_MF
         DF => first level DF: DF1
            => subsequent levelts: 0xFFFF
         ADF => UIM_ADF
    */
  }

  return mmgsdi_status;
} /* mmgsdi_util_determine_uim_dir_type */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_PROTOCOL

   DESCRIPTION:
     This function determines which card technology (UICC or ICC)

   DEPENDENCIES:
     gsdimain must be finished initialization.

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_determine_protocol(
  mmgsdi_protocol_enum_type   * protocol_type_ptr      /* UICC or ICC */
)
{
  MMGSDIUTIL_RETURN_IF_NULL(protocol_type_ptr);

  /* Select the protocol to use -  for the requested item */
  if(gsdi_data.protocol == GSDI_PROTOCOL_GSM)
  {
    *protocol_type_ptr = MMGSDI_ICC;
  } /* end if protocol is GSM */

  else if (gsdi_data.protocol == GSDI_PROTOCOL_UICC)
  {
    *protocol_type_ptr = MMGSDI_UICC;
  } /* end if protocol is UICC */
  else
  {
    ERR("BAD GSDI PROTOCOL: 0x%x",gsdi_data.protocol,0,0);
    *protocol_type_ptr =   MMGSDI_NO_PROTOCOL;
    return MMGSDI_ERROR;
  } /* end else - bad protocol */

  return MMGSDI_SUCCESS;
}/* mmgsdi_util_determine_protocol */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_RESPONSE_PTR

   DESCRIPTION:
     This function frees the dynamically allocated pointer in the
     mmgsdi cnf type structure

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_free_response_ptr(mmgsdi_cnf_type * response_ptr)
{
  mmgsdi_cnf_enum_type              cnf_type = MMGSDI_MAX_CNF_ENUM;
  mmgsdi_file_security_access_type *security_access_ptr = NULL;
  if (response_ptr != NULL)
  {
    cnf_type = response_ptr->response_header.response_type;
    switch(cnf_type)
    {
    case MMGSDI_CLIENT_ID_REG_CNF:
    case MMGSDI_CLIENT_EVT_REG_CNF:
    case MMGSDI_CLIENT_ID_DEREG_CNF:
    case MMGSDI_CLIENT_EVT_DEREG_CNF:
    case MMGSDI_SESSION_OPEN_CNF:
    case MMGSDI_SESSION_CLOSE_CNF:
    case MMGSDI_SESSION_DEACTIVATE_CNF:
    case MMGSDI_SESSION_SELECT_AID_CNF:
    case MMGSDI_CLIENT_INIT_COMP_CNF:
      break;
    case MMGSDI_READ_CNF:
      switch( response_ptr->read_cnf.access.access_method )
      {
      case MMGSDI_EF_ENUM_ACCESS:
        break;
      case MMGSDI_DF_ENUM_ACCESS:
      case MMGSDI_BY_PATH_ACCESS:
      case MMGSDI_BY_APP_ID_ACCESS:
      case MMGSDI_MAX_ACCESS_METHOD_ENUM:
        // TODO
        break;
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        response_ptr->read_cnf.read_data.data_ptr);
      break;

    case MMGSDI_WRITE_CNF:
      switch( response_ptr->write_cnf.access.access_method )
      {
      case MMGSDI_EF_ENUM_ACCESS:
        break;
      case MMGSDI_DF_ENUM_ACCESS:
      case MMGSDI_BY_PATH_ACCESS:
      case MMGSDI_BY_APP_ID_ACCESS:
      case MMGSDI_MAX_ACCESS_METHOD_ENUM:
        // TODO
        break;
      }
      break;

    case MMGSDI_GET_FILE_ATTR_CNF:
      switch( response_ptr->get_file_attr_cnf.access.access_method)
      {
      case MMGSDI_EF_ENUM_ACCESS:
        break;
      case MMGSDI_DF_ENUM_ACCESS:
      case MMGSDI_BY_PATH_ACCESS:
      case MMGSDI_BY_APP_ID_ACCESS:
      case MMGSDI_MAX_ACCESS_METHOD_ENUM:
        // TODO
        break;
      }
      switch(response_ptr->get_file_attr_cnf.file_attrib.file_type)
      {
      case MMGSDI_LINEAR_FIXED_FILE:
        security_access_ptr =
          &response_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security;
        break;

      case MMGSDI_CYCLIC_FILE:
        security_access_ptr =
          &response_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security;
        break;

      case MMGSDI_TRANSPARENT_FILE:
        security_access_ptr =
          &response_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security;
        break;

      case MMGSDI_MASTER_FILE:
      case MMGSDI_DEDICATED_FILE:
      case MMGSDI_MAX_FILE_STRUCTURE_ENUM:
        // TODO
        break;
      }
      if (security_access_ptr == NULL)
        return;
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        security_access_ptr->read.protection_pin_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        security_access_ptr->write.protection_pin_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        security_access_ptr->increase.protection_pin_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        security_access_ptr->invalidate_deactivate.protection_pin_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        security_access_ptr->rehabilitate_activate.protection_pin_ptr);
      break;

    case MMGSDI_ISIM_AUTH_CNF:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        response_ptr->isim_auth_cnf.auts.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        response_ptr->isim_auth_cnf.ck.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        response_ptr->isim_auth_cnf.ik.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        response_ptr->isim_auth_cnf.res.data_ptr);
      break;

    case MMGSDI_SAP_INIT_CNF:
    case MMGSDI_SAP_DEREG_CNF:
      break;
    case MMGSDI_SAP_CONNECT_CNF:
      break;
    case MMGSDI_SAP_DISCONNECT_CNF:
      break;
    case MMGSDI_SAP_GET_ATR_CNF:
      #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
      #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
      break;
    case MMGSDI_SEND_APDU_CNF:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        response_ptr->send_apdu_cnf.apdu_data.data_ptr);
      break;
    case MMGSDI_SAP_SEND_APDU_CNF:
      #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
      #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
      break;
    case MMGSDI_SAP_VERIFY_PIN_CNF:
      break;
    case MMGSDI_CARD_STATUS_CNF:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->status_cnf.status_data.data_ptr);
      break;
    case MMGSDI_REFRESH_CNF:
    case MMGSDI_CARD_RESET_CNF:
      break;
    case MMGSDI_SAP_POWER_OFF_CNF:
      break;
    case MMGSDI_SAP_CARD_READER_STATUS_CNF:
      #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
      #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
      break;
    case MMGSDI_COMPUTE_IP_AUTH_CNF:
      #if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            response_ptr->compute_ip_cnf.cmpt_ip_response_data.data_ptr);
      #endif
      break;
    case MMGSDI_USIM_AUTH_CNF:
      #ifdef FEATURE_MMGSDI_MBMS
#error code not present
      #endif
      break;
    case MMGSDI_GENERATE_KEY_VPM_CNF:
      #if defined(FEATURE_MMGSDI_CDMA)
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            response_ptr->generate_key_vpm_cnf.octet_data.data_ptr);
      #endif /* FEATURE_MMGSDI_CDMA */
      break;
    case MMGSDI_RUN_CAVE_CNF:
    case MMGSDI_SAP_RESET_CNF:
    case MMGSDI_SAP_POWER_ON_CNF:
    case MMGSDI_CARD_PDOWN_CNF:
    case MMGSDI_CARD_PUP_CNF:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
    case MMGSDI_DISABLE_BDN_CNF:
    case MMGSDI_ENABLE_BDN_CNF:
    case MMGSDI_REHABILITATE_CNF:
    case MMGSDI_INVALIDATE_CNF:
    case MMGSDI_PIN_OPERATION_CNF:
    case MMGSDI_SELECT_AID_CNF:
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
    case MMGSDI_ACL_OPERATION_CNF:
    case MMGSDI_SRV_AVAILABLE_CNF:
    case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
    case MMGSDI_CARD_READER_STATUS_CNF:
      break;
    case MMGSDI_GET_ATR_CNF:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->get_atr_cnf.atr_data.data_ptr);
      break;
    case MMGSDI_MFLO_INIT_ACT_PARAMS_CNF:
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF:
    case MMGSDI_MFLO_VERIFY_UCAST_CNF:
      break;
    case MMGSDI_MFLO_GET_PUBLIC_KEY_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;
    case MMGSDI_MFLO_SIGN_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;
    case MMGSDI_BCAST_CNF:
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif
      break;

    case MMGSDI_SEARCH_CNF:
      switch( response_ptr->search_cnf.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          break;

        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          // TODO
          break;
      }/*end of switch*/
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(response_ptr->search_cnf.searched_record_nums.data_ptr);
      break;
    case MMGSDI_CREATE_PKCS15_TABLE_CNF:
      break;

    case MMGSDI_MAX_CNF_ENUM:
      break;
    }
  }
} /* mmgsdi_util_free_response_ptr */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_CLIENT_REQUEST_TABLE_INDEX_FREE

   DESCRIPTION:
     This function checks if the index location is free or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     The number of entry to the table is limited to the size of the
     mmgsdi task cmd buffer

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          A valid and free index is found.
     MMGSDI_ERROR:            Unable to find a free index.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_is_client_request_table_index_free(
  int32    index,
  boolean *is_free_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  if (is_free_ptr == NULL)
    return mmgsdi_status;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_INFO))
    return mmgsdi_status;

  *is_free_ptr = FALSE;
  mmgsdi_status = MMGSDI_SUCCESS;
  if (( mmgsdi_client_req_table_info[index].client_id == 0 ) &&
      ( mmgsdi_client_req_table_info[index].request_tcb_ptr == NULL ) &&
      ( mmgsdi_client_req_table_info[index].request_type == MMGSDI_MAX_CMD_ENUM ) &&
      ( mmgsdi_client_req_table_info[index].orig_request_type == MMGSDI_MAX_CMD_ENUM ) &&
      ( mmgsdi_client_req_table_info[index].extra_param_ptr == NULL ) &&
      ( mmgsdi_client_req_table_info[index].uim_cmd_ptr == NULL ))
  {
    *is_free_ptr = TRUE;
  }

  return mmgsdi_status;
} /* mmgsdi_util_is_client_request_table_index_free */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_FREE_INDEX

   DESCRIPTION:
     This function returns the index location of the client request info table
     that is currently available

   DEPENDENCIES:
     None

   LIMITATIONS:
     The number of entry to the table is limited to the size of the
     mmgsdi task cmd buffer

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          A valid and free index is found.
     MMGSDI_ERROR:            Unable to find a free index.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_free_index(
  int32 * index_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  int32                   i             = 0;
  boolean                 is_free       = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(index_ptr);

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    mmgsdi_status = mmgsdi_util_is_client_request_table_index_free(i, &is_free);
    if (( mmgsdi_status == MMGSDI_SUCCESS ) && ( is_free == TRUE ))
    {
      *index_ptr = i;
      break;
    }
  }
  return mmgsdi_status;
} /* mmgsdi_util_get_client_request_table_free_index */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CLIENT_REQUEST_TABLE_INFO

   DESCRIPTION:
     This function populate the info to the client request table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Assignment successful
     MMGSDI_ERROR:            Assignment fail.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_populate_client_request_table_info(
  int32                               index,
  const mmgsdi_request_header_type  * request_hdr_ptr,
  uim_cmd_type                      * uim_cmd_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(request_hdr_ptr);

  /* --------------------------------------------------------------------------
     Validate all parameters provided
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  if (  ( mmgsdi_client_req_table_info[index].client_id != 0 ) ||
        ( mmgsdi_client_req_table_info[index].request_tcb_ptr != NULL ) ||
        ( mmgsdi_client_req_table_info[index].request_type != MMGSDI_MAX_CMD_ENUM ) ||
        ( mmgsdi_client_req_table_info[index].orig_request_type != MMGSDI_MAX_CMD_ENUM ) ||
        ( mmgsdi_client_req_table_info[index].extra_param_ptr != NULL ) ||
        ( mmgsdi_client_req_table_info[index].uim_cmd_ptr != NULL ))
  {
    MSG_ERROR("Client Req Index 0x%x occupied",index,0,0);
    MSG_ERROR("  client_id: 0x%x, tcb_ptr: 0x%x, req_type: 0x%x",
      mmgsdi_client_req_table_info[index].client_id,
      mmgsdi_client_req_table_info[index].request_tcb_ptr,
      mmgsdi_client_req_table_info[index].request_type);
    mmgsdi_status = MMGSDI_ERROR;
  }
  else
  {
    mmgsdi_client_req_table_info[index].uim_cmd_ptr        = uim_cmd_ptr;
    mmgsdi_client_req_table_info[index].extra_param_ptr    = extra_param_ptr;
    mmgsdi_client_req_table_info[index].client_data        = request_hdr_ptr->client_data;
    mmgsdi_client_req_table_info[index].client_id          = request_hdr_ptr->client_id;
    mmgsdi_client_req_table_info[index].request_tcb_ptr    = request_hdr_ptr->request_id_ptr;
    mmgsdi_client_req_table_info[index].request_type       = request_hdr_ptr->request_type;
    mmgsdi_client_req_table_info[index].orig_request_type  = request_hdr_ptr->orig_request_type;
    mmgsdi_client_req_table_info[index].response_cb        = request_hdr_ptr->response_cb;
    mmgsdi_client_req_table_info[index].slot_id            = request_hdr_ptr->slot_id;
  }

  return mmgsdi_status;
} /* mmgsdi_util_populate_client_request_table_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_CLIENT_REQUEST_TABLE_INDEX

   DESCRIPTION:
     This function free the client request and uim rsp data pointers
     and set the len of the rsp data data to zero

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Free was successful


   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_free_client_request_table_index(
  int32 index
)
{
  mmgsdi_uim_client_req_user_data_type *client_data_ptr = NULL;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_INFO))
    return;

  client_data_ptr = &mmgsdi_client_req_table_info[index];
  /* Synch call will require to free this pointer in the calling function */


  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_data_ptr->uim_cmd_ptr);

  switch (client_data_ptr->request_type)
  {
  case MMGSDI_WRITE_REQ:
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      client_data_ptr->extra_param_ptr->write_data.data.data_ptr);
    break;
  case MMGSDI_CLIENT_ID_REG_REQ:
  case MMGSDI_CLIENT_INIT_COMP_REQ:
  case MMGSDI_CLIENT_ID_DEREG_REQ:
  case MMGSDI_CLIENT_EVT_REG_REQ:
  case MMGSDI_CLIENT_EVT_DEREG_REQ:
  case MMGSDI_READ_REQ:
  case MMGSDI_REHABILITATE_REQ:
  case MMGSDI_INVALIDATE_REQ:
  case MMGSDI_DISABLE_BDN_REQ:
  case MMGSDI_ENABLE_BDN_REQ:
  case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
  case MMGSDI_PIN_OPERATION_REQ:
  case MMGSDI_SELECT_AID_REQ:
  case MMGSDI_GET_ALL_PIN_STATUS_REQ:
  case MMGSDI_ACL_OPERATION_REQ:
    break;
  case MMGSDI_GET_FILE_ATTR_REQ:
    if(client_data_ptr->extra_param_ptr->get_file_attr_data.int_client_data != NULL)
    {
      mmgsdi_pin_extra_info_type* pin_extra_info_ptr;
      pin_extra_info_ptr = (mmgsdi_pin_extra_info_type*)client_data_ptr->extra_param_ptr->get_file_attr_data.int_client_data;
      MMGSDIUTIL_TMC_MEM_FREE(pin_extra_info_ptr);
    }
    break;
  case MMGSDI_SESSION_SELECT_AID_REQ:
  case MMGSDI_UIM_REPORT_RSP:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_SESSION_OPEN_REQ:
  case MMGSDI_SESSION_CLOSE_REQ:
  case MMGSDI_SESSION_DEACTIVATE_REQ:
  case MMGSDI_ISIM_AUTH_REQ:
  case MMGSDI_SRV_AVAILABLE_REQ:
  case MMGSDI_USIM_AUTH_REQ:
  case MMGSDI_COMPUTE_IP_AUTH_REQ:
  case MMGSDI_RUN_CAVE_REQ:
  case MMGSDI_GENERATE_KEY_VPM_REQ:
  case MMGSDI_CNF:
  case MMGSDI_BCAST_REQ:
    break;

  case MMGSDI_SAP_CONNECT_REQ:
  case MMGSDI_SAP_DISCONNECT_REQ:
  case MMGSDI_SAP_GET_ATR_REQ:
  case MMGSDI_SAP_POWER_ON_REQ:
  case MMGSDI_SAP_POWER_OFF_REQ:
    break;
  case MMGSDI_SAP_SEND_APDU_REQ:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_INIT_REQ:
  case MMGSDI_SAP_PIN_VERIFY_REQ:
  case MMGSDI_SAP_DEREG_REQ:
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
  case MMGSDI_SAP_RESET_REQ:
  case MMGSDI_SAP_REPORT_RSP:
  case MMGSDI_CARD_STATUS_REQ:
  case MMGSDI_REFRESH_REQ:
  case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
  case MMGSDI_CARD_PDOWN_REQ:
  case MMGSDI_CARD_PUP_REQ:
  case MMGSDI_CARD_RESET_REQ:
    break;
  case MMGSDI_SEND_APDU_REQ:
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      client_data_ptr->extra_param_ptr->send_apdu_data.data.data_ptr);
    break;
  case MMGSDI_CARD_READER_STATUS_REQ:
  case MMGSDI_GET_ATR_REQ:
  case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
  case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    break;
  case MMGSDI_MFLO_SIGN_UCAST_REQ:
  case MMGSDI_MFLO_VERIFY_UCAST_REQ:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO*/
    break;
  case MMGSDI_CREATE_PKCS15_TABLE_REQ:
    break;
  case MMGSDI_SEARCH_REQ:
    break;
  case MMGSDI_MAX_CMD_ENUM:
    break;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_data_ptr->extra_param_ptr);

  client_data_ptr->client_data        = 0;
  client_data_ptr->client_id          = 0;
  client_data_ptr->request_tcb_ptr    = NULL;
  client_data_ptr->request_type       = MMGSDI_MAX_CMD_ENUM;
  client_data_ptr->orig_request_type  = MMGSDI_MAX_CMD_ENUM;
  client_data_ptr->response_cb        = NULL;
  client_data_ptr->slot_id            = MMGSDI_MAX_SLOT_ID_ENUM;

} /* mmgsdi_util_free_client_request_table_index */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INFO

   DESCRIPTION:
     This function return pointers to the information in the client request
     table at the corresponding index location.  The pointers to client request
     and uim rsp data returned will be pointing to the location that the
     table information is storing
     data_len is a pointer that needs to be a valid memory location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Successful retrieval of information
     MMGSDI_ERROR:            Unable to retrieve information
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_info(
  int32                                  index,
  mmgsdi_request_header_type            *request_hdr_ptr,
  uim_cmd_type                         **uim_cmd_ptr,
  mmgsdi_client_req_extra_info_type    **extra_param_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  MMGSDIUTIL_RETURN_IF_NULL(request_hdr_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(uim_cmd_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  request_hdr_ptr->client_data = mmgsdi_client_req_table_info[index].client_data;
  request_hdr_ptr->client_id = mmgsdi_client_req_table_info[index].client_id;
  request_hdr_ptr->request_id_ptr = mmgsdi_client_req_table_info[index].request_tcb_ptr;
  request_hdr_ptr->orig_request_type = mmgsdi_client_req_table_info[index].orig_request_type;
  request_hdr_ptr->request_type = mmgsdi_client_req_table_info[index].request_type;
  request_hdr_ptr->response_cb = mmgsdi_client_req_table_info[index].response_cb;
  request_hdr_ptr->slot_id = mmgsdi_client_req_table_info[index].slot_id;

  *uim_cmd_ptr        = mmgsdi_client_req_table_info[index].uim_cmd_ptr;
  *extra_param_ptr    = mmgsdi_client_req_table_info[index].extra_param_ptr;
  return mmgsdi_status;
} /* mmgsdi_util_get_client_request_table_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INDEX

   DESCRIPTION:
     Based on the request type, the client table is searched through to get
     a valid index.
   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Successful retrieval of information
     MMGSDI_ERROR:            Unable to retrieve information
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_index
(
  int32* index_ptr,
  mmgsdi_cmd_enum_type request_type
)
{
  int i=0;
  MMGSDIUTIL_RETURN_IF_NULL(index_ptr);

  for(i=0;i<MMGSDI_MAX_CLIENT_INFO;i++)
  {
    if(mmgsdi_client_req_table_info[i].orig_request_type == request_type &&
      mmgsdi_client_req_table_info[i].request_type == request_type)
    {
      *index_ptr = i;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_COPY_ACCESS_TYPE

   DESCRIPTION:
     This function copy access type information from one buffer to another.
     This will dynamically allocation memory if required

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_copy_access_type(
  mmgsdi_access_type       *dest_access_ptr,
  const mmgsdi_access_type *src_access_ptr
)
{
  int i  = 0;

  MMGSDIUTIL_RETURN_IF_NULL(dest_access_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(src_access_ptr);

  dest_access_ptr->access_method = src_access_ptr->access_method;
  switch (src_access_ptr->access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
    dest_access_ptr->file.file_enum = src_access_ptr->file.file_enum;
    MSG_HIGH("mmgsdi_util_copy_access_type ef_enum:0x%x",src_access_ptr->file.file_enum,0,0);
    break;
  case MMGSDI_DF_ENUM_ACCESS:
    MSG_HIGH("mmgsdi_util_copy_access_type MMGSDI_DF_ENUM_ACCESS - not supported",
        0, 0, 0);
    break;
  case MMGSDI_BY_PATH_ACCESS:
    if ((src_access_ptr->file.path_type.path_len > MMGSDI_MAX_PATH_LEN) ||
        (src_access_ptr->file.path_type.path_len < 0))
    {
      MSG_ERROR("Path Len greater than MMGSDI_MAX_PATH_LEN 0x%x",
        src_access_ptr->file.path_type.path_len, 0, 0);
      return MMGSDI_ERROR;
    }
    dest_access_ptr->file.path_type.path_len = src_access_ptr->file.path_type.path_len;
    memcpy(dest_access_ptr->file.path_type.path_buf,
           src_access_ptr->file.path_type.path_buf,
           (sizeof(uint16)*int32touint32(src_access_ptr->file.path_type.path_len)));
    for (i = 0; i < dest_access_ptr->file.path_type.path_len; i++)
    {
      MSG_HIGH("mmgsdi_util_copy_access_type path[0x%x]: 0x%x",
        i, dest_access_ptr->file.path_type.path_buf[i], 0);
    }
    break;
  case MMGSDI_BY_APP_ID_ACCESS:
    if ((src_access_ptr->file.app_id.data_len > MMGSDI_MAX_AID_LEN) ||
        (src_access_ptr->file.app_id.data_len < 0))
    {
      MSG_ERROR("App ID Len greater than MMGSDI_MAX_AID_LEN 0x%x",
        src_access_ptr->file.app_id.data_len, 0, 0);
      return MMGSDI_ERROR;
    }
    dest_access_ptr->file.app_id.data_len = src_access_ptr->file.app_id.data_len;
    memcpy(dest_access_ptr->file.app_id.data_ptr,
           src_access_ptr->file.app_id.data_ptr,
           int32touint32(src_access_ptr->file.app_id.data_len));
    for (i = 0; i < dest_access_ptr->file.app_id.data_len; i++)
    {
      MSG_HIGH("mmgsdi_util_copy_access_type app_id[0x%x]: 0x%x",
        i, dest_access_ptr->file.app_id.data_ptr[i], 0);
    }
    break;
  case MMGSDI_MAX_ACCESS_METHOD_ENUM:
    MSG_ERROR("mmgsdi_util_copy_access_type MMGSDI_MAX_ACCESS_METHOD_ENUM",
        0, 0, 0);
    break;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_copy_access_type */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_WRITE_CACHE

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And write data to different caches based on the applications available
     on the card by calling the corresding util write cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, all the caches will be
     updated

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_write_cache(
  mmgsdi_slot_id_enum_type   slot_id,
  const mmgsdi_access_type   *file_access_ptr,
  mmgsdi_data_type           data,
  mmgsdi_len_type            offset,
  gsdi_data_from_enum_type   data_fr_card
)
{
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_SUCCESS;
  uim_items_enum_type     uim_file_items = UIM_NO_SUCH_ITEM;
  boolean                 pref_slot      = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(file_access_ptr);

  /* Convert from MMGSDI access path and EF/DF to UIM value */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(file_access_ptr,
                                                       &uim_file_items,
                                                       TRUE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Skip Cache Write: Unable to Match EF enum",
                             0, 0, 0);
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    MSG_HIGH("EF (uim enums): 0x%x", uim_file_items, 0, 0);
    if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
         == GSDI_GSM_SIM_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_GSM,
                                                &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_write_cache_item( uim_file_items,
                                                      MMGSDI_TECH_GSM,
                                                      pref_slot,
                                                      data.data_len,
                                                      offset,
                                                      data.data_ptr,
                                                      data_fr_card);
      }
    }
    if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)
         == GSDI_USIM_APP_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_UMTS,
                                                &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_write_cache_item(uim_file_items,
                                                   MMGSDI_TECH_UMTS,
                                                   pref_slot,
                                                   data.data_len,
                                                   offset,
                                                   data.data_ptr,
                                                   data_fr_card);
      }
    }
    if ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
         == GSDI_RUIM_APP_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_CDMA,
                                              &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_write_cache_item(uim_file_items,
                                                   MMGSDI_TECH_CDMA,
                                                   pref_slot,
                                                   data.data_len,
                                                   offset,
                                                   data.data_ptr,
                                                   data_fr_card);
      }
    }

    switch (mmgsdi_status)
    {
    case MMGSDI_NOT_SUPPORTED:
      MSG_HIGH("Cache Write item not supported", 0, 0, 0);
      break;
    case MMGSDI_INCORRECT_PARAMS:
      MSG_HIGH("Cache Write item incorrect params", 0, 0, 0);
      break;
    case MMGSDI_SUCCESS:
      MSG_MED("Cache Write Success", 0, 0, 0);
      break;
    default:
      MSG_HIGH("Write Cache Status 0x%x", mmgsdi_status, 0, 0);
      break;
    }
  }
  return mmgsdi_status;
} /* mmgsdi_util_write_cache */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And read data to different caches based on the applications available
     on the card by calling the corresding util read cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache(
  mmgsdi_slot_id_enum_type   slot_id,
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_data_type          *data_ptr,
  mmgsdi_len_type            offset
)
{
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_SUCCESS;
  uim_items_enum_type     uim_file_items = UIM_NO_SUCH_ITEM;
  boolean                 pref_slot      = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(file_access_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);

  /* Convert from MMGSDI access path and EF/DF to UIM value */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(file_access_ptr,
                                                       &uim_file_items,
                                                       TRUE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Skip Cache Read: Unable to Match EF enum",
                             0, 0, 0);
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    MSG_HIGH("EF (uim enums): 0x%x", uim_file_items, 0, 0);
    if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
         == GSDI_GSM_SIM_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_GSM,
                                                &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_read_cache_item(uim_file_items,
                                                   MMGSDI_TECH_GSM,
                                                   pref_slot,
                                                   data_ptr->data_len,
                                                   offset,
                                                   &data_ptr->data_len,
                                                   data_ptr->data_ptr);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Return so that we don't go through the rest of the cache */
          return mmgsdi_status;
        }
      }
    }
    if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)
         == GSDI_USIM_APP_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_UMTS,
                                                 &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_read_cache_item(uim_file_items,
                                                   MMGSDI_TECH_UMTS,
                                                   pref_slot,
                                                   data_ptr->data_len,
                                                   offset,
                                                   &data_ptr->data_len,
                                                   data_ptr->data_ptr);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Return so that we don't go through the rest of the cache */
          return mmgsdi_status;
        }
      }
    }
    if ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
         == GSDI_RUIM_APP_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_CDMA,
                                                &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_read_cache_item(uim_file_items,
                                                   MMGSDI_TECH_CDMA,
                                                   pref_slot,
                                                   data_ptr->data_len,
                                                   offset,
                                                   &data_ptr->data_len,
                                                   data_ptr->data_ptr);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Return so that we don't go through the rest of the cache */
          return mmgsdi_status;
        }
      }
    }

    switch (mmgsdi_status)
    {
    case MMGSDI_NOT_SUPPORTED:
      MSG_HIGH("Cache Read item not supported", 0, 0, 0);
      break;
    case MMGSDI_INCORRECT_PARAMS:
      MSG_HIGH("Cache Read item incorrect params", 0, 0, 0);
      break;
    case MMGSDI_SUCCESS:
      MSG_MED("Cache Read Success", 0, 0, 0);
      break;
    default:
      MSG_HIGH("Other Cache Read status 0x%x", mmgsdi_status, 0, 0);
      break;
    }
  }
  return mmgsdi_status;
} /* mmgsdi_util_read_cache */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_USIM_SPEC_VERSION

   DESCRIPTION:
     This function checks the usim specification

   DEPENDENCIES:
     None

   LIMITATIONS:
     Used for UICC only

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS:         If the USIM spec version check is successful
     MMGSDI_INCORRECT_PARAM: Null input parameter

   SIDE EFFECTS:
     None

==========================================================================*/
void mmgsdi_util_check_usim_spec_version (
  mmgsdi_access_type *mmgsdi_access_ptr
)
{

  if(mmgsdi_access_ptr == NULL)
    return;

  switch (mmgsdi_access_ptr->access_method)
  {
  case MMGSDI_EF_ENUM_ACCESS:
    switch (mmgsdi_access_ptr->file.file_enum)
    {
    case MMGSDI_USIM_PLMNWACT:
      if (gsdi_data.usim_spec_version == R99_2000_07)
      {
        mmgsdi_access_ptr->file.file_enum = MMGSDI_USIM_UPLMNSEL;
      }
      break;
    case MMGSDI_USIM_OPLMNWACT:
      if (gsdi_data.usim_spec_version == R99_2000_07)
      {
        mmgsdi_access_ptr->file.file_enum = MMGSDI_USIM_OPLMNSEL;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
} /* mmgsdi_util_check_usim_spec_version */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SLOT_PREF

   DESCRIPTION:
     This function looks at the technology type and determines if the slot
     passed in is the operational slot for that particular technology.
     If it is, the function returns TRUE, else, it returns FALSE.
     The function also populates the new slot pointer to the correct slot info.
     This is required for automatic slot mode since the uim request will require
     the specification of the slot info

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_NOT_SUPPORTED:    Path not found in the lookup table

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_is_slot_pref (
  const mmgsdi_slot_id_enum_type *slot_ptr,
  mmgsdi_tech_enum_type           tech,
  boolean                        *pref_ptr)
{
  uint32 slot1_mask = 0;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint32 slot2_mask = 0;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  MMGSDIUTIL_RETURN_IF_NULL(slot_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(pref_ptr);

  switch (tech)
  {
  case MMGSDI_TECH_GSM:
    slot1_mask = GSDI_GSM_SLOT_1_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    slot2_mask = GSDI_GSM_SLOT_2_MASK;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    break;
  case MMGSDI_TECH_CDMA:
    slot1_mask = GSDI_CDMA_SLOT_1_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    slot2_mask = GSDI_CDMA_SLOT_2_MASK;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    break;
  case MMGSDI_TECH_UMTS:
    slot1_mask = GSDI_USIM_SLOT_1_MASK;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    slot2_mask = GSDI_USIM_SLOT_2_MASK;
#endif /* #ifdef FEATURE_MMGSDI_DUAL_SLOT */
    break;
  default:
    return MMGSDI_ERROR;
  }

  switch (*slot_ptr)
  {
  case MMGSDI_SLOT_1:
    if ( ( gsdi_data.card_applications.operational_slot & slot1_mask ) ==
            slot1_mask )
      *pref_ptr = TRUE;
    else
      *pref_ptr = FALSE;
    break;
  case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if ( ( gsdi_data.card_applications.operational_slot & slot2_mask ) ==
            slot2_mask )
      *pref_ptr = TRUE;
    else
      *pref_ptr = FALSE;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    break;

  case MMGSDI_SLOT_AUTOMATIC:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if ( ( gsdi_data.card_applications.operational_slot & slot1_mask ) ==
            slot1_mask )
    {
      *slot_ptr = MMGSDI_SLOT_1;
    }
    else if ( ( gsdi_data.card_applications.operational_slot & slot2_mask )==
                slot2_mask)
    {
      *slot_ptr = MMGSDI_SLOT_2;
    }
    else
    {
      MSG_ERROR("No operational slot for tech %d", tech, 0, 0);
      return MMGSDI_ERROR;
    }
    *pref_ptr = TRUE;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    break;

  default:
    *pref_ptr = FALSE;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_is_slot_pref */ /*lint !e550 */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_FILE_OK_IN_SVC_TABLE

   DESCRIPTION:
     This function based on the application available on the card,
     check if the file is allowed by the service table indication or not

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     int32:
     0 FALSE (Not Initialized)
     1 TRUE  (Initialized)
     255 (GSDI_NOT_IN_LKUP_TABLE)

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
boolean mmgsdi_util_is_file_ok_in_svc_table (
  const mmgsdi_slot_id_enum_type  *mmgsdi_slot_ptr,
  const mmgsdi_access_type        *mmgsdi_access_ptr
)
{

  boolean                 file_ok_in_svr_table  = FALSE;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uim_items_enum_type     uim_file_items = UIM_NO_SUCH_ITEM;
  boolean                 pref_slot = TRUE;

  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(mmgsdi_access_ptr,
                                                       &uim_file_items,
                                                       FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return FALSE;
  }

  if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
         == GSDI_GSM_SIM_MASK)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
  }
  if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)
         == GSDI_USIM_APP_MASK)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
  }
  if ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
         == GSDI_RUIM_APP_MASK)
  {
#ifdef FEATURE_MMGSDI_CDMA
    mmgsdi_status = mmgsdi_util_is_slot_pref( mmgsdi_slot_ptr,
                                              MMGSDI_TECH_CDMA,
                                              &pref_slot );
    if (mmgsdi_status == MMGSDI_SUCCESS) {
      file_ok_in_svr_table |= gsdi_util_is_file_ok_in_cst( uim_file_items,
                                                           pref_slot );
    }
#endif /* FEATURE_MMGSDI_CDMA */
  }

  return file_ok_in_svr_table;

} /* mmgsdi_util_is_file_ok_in_svc_table*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE_ITEM_LEN

   DESCRIPTION:
     This function return the cache item len

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_item_len(
  uim_items_enum_type   sim_filename,
  mmgsdi_tech_enum_type tech,
  boolean               pref_slot,
  mmgsdi_len_type       data_offset,
  mmgsdi_len_type       *total_len_ptr
)
{
/*-----------------------------------------------------------------------------
  Initialize variables
------------------------------------------------------------------------------*/
  word                     file_num = 0;
  int32                    cache_size = 0;
  sim_cache_element_T     *cache_table_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(total_len_ptr);

  switch (tech)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
  case MMGSDI_TECH_CDMA:
    cache_size = GSDI_NUM_FILES_IN_RUIM_CACHE;
    cache_table_ptr = gsdi_ruim_cache;
    break;
#endif /* FEATURE_MMGSDI_CDMA */
  default:
    return MMGSDI_ERROR;
  }
/*-----------------------------------------------------------------------------
  Look through cache for desired file
------------------------------------------------------------------------------*/
  /* Find requested file in usim cache */
  for ( file_num = 0; file_num < cache_size; file_num++ )
  {
    if ( cache_table_ptr[file_num].filename == sim_filename )
    {
      if ( cache_table_ptr[file_num].pref_slot == pref_slot )
      {
        if ( cache_table_ptr[file_num].init)
        {
          if (cache_table_ptr[file_num].total_len <= data_offset)
          {
            MSG_ERROR("Data offset 0x%x same or greater than total len 0x%x", data_offset,
                       cache_table_ptr[file_num].total_len, 0);
            return MMGSDI_ERROR;
          }
          *total_len_ptr = cache_table_ptr[file_num].total_len - data_offset;
          return MMGSDI_SUCCESS;
        }
        MSG_ERROR("Cache not initialized 0x%x", sim_filename, 0, 0);
        return MMGSDI_NOT_INIT;
      }/* end if - pref_slot */
    }/* end if - cache_table */
  }/* end for - file_num < num files in cache */

  return MMGSDI_NOT_SUPPORTED;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE_LEN

   DESCRIPTION:
     This function return the total len - offset of the cache item

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_len(
  mmgsdi_slot_id_enum_type  slot_id,
  const mmgsdi_access_type *file_access_ptr,
  mmgsdi_len_type          *data_len_ptr,
  mmgsdi_len_type           offset
)
{
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_SUCCESS;
  uim_items_enum_type     uim_file_items = UIM_NO_SUCH_ITEM;
  boolean                 pref_slot      = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(file_access_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_len_ptr);

  /* Convert from MMGSDI access path and EF/DF to UIM value */
  mmgsdi_status = mmgsdi_util_determine_uim_items_enum(file_access_ptr,
                                                       &uim_file_items,
                                                       TRUE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Skip Cache Read: Unable to Match EF enum",
                             0, 0, 0);
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    MSG_HIGH("EF (uim enums): 0x%x", uim_file_items, 0, 0);
    if ((gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK)
         == GSDI_GSM_SIM_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_GSM,
                                                &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_read_cache_item_len(uim_file_items,
                                                        MMGSDI_TECH_GSM,
                                                        pref_slot,
                                                        offset,
                                                        data_len_ptr);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Return so that we don't go through the rest of the cache */
          return mmgsdi_status;
        }
      }
    }
    if ((gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK)
         == GSDI_USIM_APP_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_UMTS,
                                                 &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_read_cache_item_len(uim_file_items,
                                                        MMGSDI_TECH_UMTS,
                                                        pref_slot,
                                                        offset,
                                                        data_len_ptr);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Return so that we don't go through the rest of the cache */
          return mmgsdi_status;
        }
      }
    }
    if ((gsdi_data.card_applications.apps_enabled & GSDI_RUIM_APP_MASK)
         == GSDI_RUIM_APP_MASK)
    {
      mmgsdi_status = mmgsdi_util_is_slot_pref( &slot_id,
                                                 MMGSDI_TECH_CDMA,
                                                &pref_slot );
      if (mmgsdi_status == MMGSDI_SUCCESS) {
        mmgsdi_status = mmgsdi_util_read_cache_item_len(uim_file_items,
                                                        MMGSDI_TECH_CDMA,
                                                        pref_slot,
                                                        offset,
                                                        data_len_ptr);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Return so that we don't go through the rest of the cache */
          return mmgsdi_status;
        }
      }
    }
  }
  return mmgsdi_status;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CACHE_SET_DEFAULT_INIT_VALUE

   DESCRIPTION:
     This function set the init flag to true, and initialize data value to
     0 with the "existing" total len

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cache_set_default_init_value(
  uim_items_enum_type       sim_filename,
  mmgsdi_tech_enum_type     tech,
  boolean                   pref_slot
)
{
/*-----------------------------------------------------------------------------
  Initialize variables
------------------------------------------------------------------------------*/
  boolean                  file_found = FALSE;
  word                     file_num = 0;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  int32                    cache_size = 0;
  sim_cache_element_T     *cache_table_ptr = NULL;
  uint32                   data_len =0;

  switch (tech)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
  case MMGSDI_TECH_CDMA:
    cache_size = GSDI_NUM_FILES_IN_RUIM_CACHE;
    cache_table_ptr = gsdi_ruim_cache;
    break;
#endif /* FEATURE_MMGSDI_CDMA */
  default:
    return MMGSDI_ERROR;
  }
/*-----------------------------------------------------------------------------
  Look through cache for desired file
------------------------------------------------------------------------------*/
  /* Find requested file in usim cache */
  for ( file_num = 0; file_num < cache_size; file_num++ )
  {
    if ( cache_table_ptr[file_num].filename == sim_filename )
    {
      if ( cache_table_ptr[file_num].pref_slot == pref_slot )
      {
        file_found = TRUE;
        break;
      }/* end if - pref_slot */
    }/* end if - cache_table */
  }/* end for - file_num < num files in cache */

/*-----------------------------------------------------------------------------
  Handle the special case of updating the data len in cache during init when
  GSDI is caching information.  During init, GSDI caches whole records
  partial records or offsets of records are not cached during init
-----------------------------------------------------------------------------*/

  if ((!gsdi_data.init_completed
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      || !gsdi_data.init_completed_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
       ) && file_found && file_num < cache_size)
  {
    /* Need to first update the Length of the record */
    /* Is this the first time reading the file ?     */
    cache_table_ptr[file_num].init = TRUE;
    memset(cache_table_ptr[file_num].body, 0x00, GSDI_CACHE_MAX_FILE_LEN);
    if((cache_table_ptr[file_num].total_len) > GSDI_CACHE_MAX_FILE_LEN)
    {
      data_len = int32touint32(cache_table_ptr[file_num].total_len - GSDI_CACHE_MAX_FILE_LEN);

      MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(cache_table_ptr[file_num].add_data,
                                                data_len);
    } /* end if data_length > 255 */
    else
    {
      cache_table_ptr[file_num].add_data = NULL;
    } /* end else data_length  <= 255 */
    mmgsdi_status = MMGSDI_SUCCESS;
  } /* endif - data len is not 0 */
  return mmgsdi_status;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_WRITE_CACHE_ITEM

   DESCRIPTION:
     This function writes to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_write_cache_item(
  uim_items_enum_type       sim_filename,
  mmgsdi_tech_enum_type     tech,
  boolean                   pref_slot,
  mmgsdi_len_type           data_len,
  mmgsdi_len_type           data_offset,
  const uint8 *             data_ptr,
  gsdi_data_from_enum_type  data_fr_card
)
{
/*-----------------------------------------------------------------------------
  Initialize variables
------------------------------------------------------------------------------*/
  boolean                  file_found = FALSE;
  word                     file_num = 0;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  int32                    cache_size = 0;
  sim_cache_element_T     *cache_table_ptr = NULL;
  uint32                   cache_data_len =0;

  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);

  if (data_offset < 0)
  {
    MSG_ERROR("Invalid Data Offset %d < 0", data_offset, 0, 0);
    return MMGSDI_ERROR;
  }

  switch (tech)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
  case MMGSDI_TECH_CDMA:
    cache_size = GSDI_NUM_FILES_IN_RUIM_CACHE;
    cache_table_ptr = gsdi_ruim_cache;
    break;
#endif /* FEATURE_MMGSDI_CDMA */
  default:
    return MMGSDI_ERROR;
  }
/*-----------------------------------------------------------------------------
  Look through cache for desired file
------------------------------------------------------------------------------*/
  /* Find requested file in usim cache */
  for ( file_num = 0; file_num < cache_size; file_num++ )
  {
    if ( cache_table_ptr[file_num].filename == sim_filename )
    {
      if ( cache_table_ptr[file_num].pref_slot == pref_slot )
      {
        file_found = TRUE;
        break;
      }/* end if - pref_slot */
    }/* end if - cache_table */
  }/* end for - file_num < num files in cache */

/*-----------------------------------------------------------------------------
  Handle the special case of updating the data len in cache during init when
  GSDI is caching information.  During init, GSDI caches whole records
  partial records or offsets of records are not cached during init
-----------------------------------------------------------------------------*/

  if ((!gsdi_data.init_completed
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      || !gsdi_data.init_completed_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
       ) && file_found && file_num < cache_size)
  {
    /* Need to first update the Length of the record */
    /* Is this the first time reading the file ?     */
    if ( !cache_table_ptr[file_num].init)
    {
      /* OK to update to length returned */
      /* Need to avoid updating to length 0 */
      if ( data_len != 0 )
      {
        if((data_len  + data_offset) > GSDI_CACHE_MAX_FILE_LEN)
        {
          cache_table_ptr[file_num].rec_len = GSDI_CACHE_MAX_FILE_LEN;
          cache_table_ptr[file_num].total_len = data_len + data_offset;
          cache_data_len = int32touint32(data_len  + data_offset - GSDI_CACHE_MAX_FILE_LEN);

          MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                          cache_table_ptr[file_num].add_data,
                          cache_data_len);

        } /* end if data_length > 255 */
        else
        {
          cache_table_ptr[file_num].rec_len = int32touint8(data_len); // Casting to Be remove after gsdi is gone
          cache_table_ptr[file_num].total_len = data_len;
          cache_table_ptr[file_num].add_data = NULL;
        } /* end else data_length  <= 255 */
        mmgsdi_status = MMGSDI_SUCCESS;
      } /* endif - data len is not 0 */
    } /* gsdi_usim_cache file ! init */
  } /* file found, init not completed */

/*-----------------------------------------------------------------------------
  Handle case where file is not supported in the cache
-----------------------------------------------------------------------------*/

  if (!file_found || file_num >= cache_size)
  {
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    return mmgsdi_status;
  } /* file not found */

/*-----------------------------------------------------------------------------
  Handle case where requested length is longer than the length of the record
-----------------------------------------------------------------------------*/

  else if (data_offset + data_len > cache_table_ptr[file_num].total_len)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
  } /* end if - requested data is too long */

/*-----------------------------------------------------------------------------
  Proceed with cache write, Data length of 0 indicates the max number of bytes
  to be written
-----------------------------------------------------------------------------*/
  else
  {
    if (data_len == 0)
    {
      if (cache_table_ptr[file_num].total_len <= data_offset)
      {
        MSG_ERROR("Data offset 0x%x same or greater than total len 0x%x", data_offset,
                       cache_table_ptr[file_num].total_len, 0);
        return MMGSDI_ERROR;
      }
      data_len = cache_table_ptr[file_num].total_len - data_offset;
    } /* Write max data length */

    if (data_len < 0)
    {
      MSG_ERROR("Invalid Data Len requested %d < 0", data_len, 0, 0);
      return MMGSDI_ERROR;
    }

    /* Copy contents to be written into cache */
    if((data_len + data_offset) < GSDI_CACHE_MAX_FILE_LEN)
    {
      /* Data to be written will fit entirely in the regular (old) buffer */
      memcpy( (cache_table_ptr[file_num].body) + data_offset, data_ptr,
        int32touint32(data_len) );
      cache_table_ptr[file_num].init = TRUE;
    } /* end if data_len + data_offset <= GSDI_CACHE_MAX_FILE_LEN */
    else if(data_offset >= GSDI_CACHE_MAX_FILE_LEN)
    {
      /* Data to be written is entirely contained in add_data */
      memcpy((byte*)(cache_table_ptr[file_num].add_data) + (data_offset - GSDI_CACHE_MAX_FILE_LEN),
              data_ptr,
              int32touint32(data_len));
      cache_table_ptr[file_num].init = TRUE;
    } /* end if - all written data is in add data buffer */
    else
    {
      /* data is partially in body buffer, partially in add data buffer
      First, copy first portion of data to body buffer */
      memcpy( (cache_table_ptr[file_num].body) + data_offset, data_ptr,
                 int32touint32(GSDI_CACHE_MAX_FILE_LEN - data_offset));
      /* Now copy the remaining data into the add_data buffer */
      memcpy((byte*)(cache_table_ptr[file_num].add_data),
        data_ptr + (GSDI_CACHE_MAX_FILE_LEN-data_offset),
        int32touint32(data_len - (GSDI_CACHE_MAX_FILE_LEN - data_offset)));
      cache_table_ptr[file_num].init = TRUE;
    } /* end else - data is split between body buffer and add_data buffer */

   /* Update whether the cache data is in synch with card or not,
      In the New arch, util write is called only after a process to the card
       It does not have the write to cache and then queue the command */
    switch (data_fr_card)
    {
    case GSDI_DATA_FROM_CARD:
     cache_table_ptr[file_num].in_synch = TRUE;
     break;
    case GSDI_DATA_FROM_CLIENT:
    default:
     cache_table_ptr[file_num].in_synch = FALSE;
     break;
    }
    mmgsdi_status = MMGSDI_SUCCESS;
  } /* end else - Proceed with Cache write */

  return mmgsdi_status;
} /* mmgsdi_util_write_cache_item */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE_ITEM

   DESCRIPTION:
     This function reads to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_item (
  uim_items_enum_type   sim_filename,
  mmgsdi_tech_enum_type tech,
  boolean               pref_slot,
  mmgsdi_len_type       requested_data_len,
  mmgsdi_len_type       data_offset,
  mmgsdi_len_type *     actual_data_len_ptr,
  uint8  *              data_ptr
)
{
/*-----------------------------------------------------------------------------
  Initialize variables
------------------------------------------------------------------------------*/
  boolean                  file_found = FALSE;
  word                     file_num = 0;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  int32                    cache_size = 0;
  sim_cache_element_T     *cache_table_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(actual_data_len_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);

  if (data_offset < 0)
  {
    MSG_ERROR("Invalid Data Offset %d < 0", data_offset, 0, 0);
    return MMGSDI_ERROR;
  }

  switch (tech)
  {
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /*FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
  case MMGSDI_TECH_CDMA:
    cache_size = GSDI_NUM_FILES_IN_RUIM_CACHE;
    cache_table_ptr = gsdi_ruim_cache;
    break;
#endif /* FEATURE_MMGSDI_CDMA */
  default:
    return MMGSDI_ERROR;
  }
/*-----------------------------------------------------------------------------
  Look through cache for desired file
------------------------------------------------------------------------------*/
  /* Find requested file in sim cache */
  for ( file_num = 0; file_num < cache_size; file_num++ )
  {
    if ( cache_table_ptr[file_num].filename == sim_filename )
    {
      if ( cache_table_ptr[file_num].pref_slot == pref_slot )
      {
        file_found = TRUE;
        break;
      }/* end if - pref_slot */
    }/* end if - sim_filename */
  }/* end for - file_num < num files in cache */

/*-----------------------------------------------------------------------------
  Handle File Not Found
------------------------------------------------------------------------------*/
  if (!file_found || file_num >= cache_size)
  {
    mmgsdi_status = MMGSDI_NOT_SUPPORTED;
  } /* !file_found */

/*-----------------------------------------------------------------------------
  Handle Cache Not Init
------------------------------------------------------------------------------*/
  else if (cache_table_ptr[file_num].init == FALSE)
  {
    mmgsdi_status = MMGSDI_NOT_INIT;
  } /* Cache Not Init */

/*-----------------------------------------------------------------------------
  Handle Incorrect Paramters
-----------------------------------------------------------------------------*/
  else if (data_offset+requested_data_len > cache_table_ptr[file_num].total_len)
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
  }  /* Incorrect Paramters */

/*-----------------------------------------------------------------------------
  Proceed with Read
------------------------------------------------------------------------------*/
  else
  {
    if (requested_data_len == 0)
    {
      if (cache_table_ptr[file_num].total_len <= data_offset)
      {
        MSG_ERROR("Data offset 0x%x same or greater than total len 0x%x", data_offset,
                       cache_table_ptr[file_num].total_len, 0);
        return MMGSDI_ERROR;
      }
      /* When requested data_len is 0, it means that whole file should
      be returned */
      actual_data_len_ptr[0] = cache_table_ptr[file_num].total_len - data_offset;
    } /* end if - requested_data_len is 0 */
    else
    {
      actual_data_len_ptr[0] = requested_data_len;
    } /* end else - requested data len is not 0 */

    if (actual_data_len_ptr[0] < 0)
    {
      MSG_ERROR("Invalid Data Len requested %d < 0", actual_data_len_ptr[0], 0, 0);
      return MMGSDI_ERROR;
    }

    /* Copy from cache */
    if((actual_data_len_ptr[0] + data_offset) < GSDI_CACHE_MAX_FILE_LEN)
    {
      /* Data to be read is entirely in body buffer */
      (void) memcpy( data_ptr,
                    (cache_table_ptr[file_num].body) + data_offset,
                     int32touint32(actual_data_len_ptr[0]) );
    } /* end if actual_data_len_p[0] + data_offset <= GSDI_CACHE_MAX_FILE_LEN */
    else if(data_offset >= GSDI_CACHE_MAX_FILE_LEN)
    {
      /* all data read is in add data buffer */
      (void) memcpy(data_ptr,
                    (cache_table_ptr[file_num].add_data +
                     (data_offset-GSDI_CACHE_MAX_FILE_LEN)),
                    int32touint32(actual_data_len_ptr[0]));

    } /* end if - all data read is in add data buffer */
    else
    {
      /* data is split between body buffer and add_data buffer */
      (void) memcpy(data_ptr,
                   (cache_table_ptr[file_num].body) + data_offset,
                    int32touint32(GSDI_CACHE_MAX_FILE_LEN - data_offset));
      (void) memcpy(data_ptr + (GSDI_CACHE_MAX_FILE_LEN - data_offset),
                   (cache_table_ptr[file_num].add_data),
                    int32touint32(actual_data_len_ptr[0] - (GSDI_CACHE_MAX_FILE_LEN - data_offset)));
    } /* end else - data is split between body buffer and add_data buffer */
    mmgsdi_status = MMGSDI_SUCCESS;
  } /* end else - Proceed with Read */
  return mmgsdi_status;
} /* mmgsdi_util_read_cache_item */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CLEAR_FILE_FROM_CACHE

   DESCRIPTION:
     This function will clear the indicated files from the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==========================================================================*/
void mmgsdi_util_clear_file_from_cache(
  uint8                       num_files,
  const uim_items_enum_type * sim_filenames_ptr,
  mmgsdi_slot_id_enum_type    slot
)
{
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
  uint8 cache_file_num = 0;
  uint8 file_num = 0;
  gsdi_slot_id_type new_slot = GSDI_SLOT_NONE;
  gsdi_slot_id_type gsdi_slot = GSDI_SLOT_NONE;
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS || FEATURE_UIM_RUIM_W_GSM_ACCESS */

/*-----------------------------------------------------------------------------
  Init Checking
-----------------------------------------------------------------------------*/
  if(slot != MMGSDI_SLOT_1)
  {
    MSG_ERROR("Only Slot 1 is currently supported",0,0,0);
    return;
  }

  if ( num_files == 0
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS*/
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
#ifdef FEATURE_MMGSDI_CDMA
       || num_files >= GSDI_NUM_FILES_IN_RUIM_CACHE
#endif /* FEATURE_MMGSDI_CDMA */
       )
  {
    return;
  }
  if (sim_filenames_ptr == NULL)
  {
    MSG_ERROR("sim filenames ptr NULL",0,0,0);
    return;
  }
#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

} /* mmgsdi_util_clear_file_from_cache */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_QUEUE_MMGSDI_UIM_REPORT_RSP

   DESCRIPTION:
     This function is used to queue a mmgsdi_uim_report_rsp in the event that a
     command is not sent down the the UIM (ex, error before command is sent, or
     a read that comes from cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_uim_report_rsp(
  int32                   index,
  mmgsdi_cmd_enum_type    mmgsdi_cmd_enum,
  mmgsdi_return_enum_type mmgsdi_report_status
  )
{
  mmgsdi_uim_report_rsp_type      * msg_ptr       = NULL;
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

  /*---------------------------------------------------------------------------
    - No Need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.uim_report_rsp;

  /*-----------------------------------------------------------------------
   Populating mmgsdi_uim_report_rsp_type header
   1) Client ID = 0
   2) Request TYPE
   3) Slot ID: Set the slot to automatic since the info is inside the user
               data
   4) Request Task TCB
   5) Client Data Pointer: The previous request
   6) Response CallBack
   7) Payload len and request len will be populated after the content
      has been populated
   -----------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                   = MMGSDI_UIM_REPORT_RSP;
  msg_ptr->request_header.client_id            = 0;
  msg_ptr->request_header.request_type         = MMGSDI_UIM_REPORT_RSP;
  msg_ptr->request_header.orig_request_type    = MMGSDI_UIM_REPORT_RSP;
  msg_ptr->request_header.slot_id              = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.request_id_ptr       = rex_self();
  msg_ptr->request_header.client_data          = 0;
  msg_ptr->request_header.response_cb          = NULL;
  msg_ptr->mmgsdi_error_status                 = mmgsdi_report_status;

  /*-----------------------------------------------------------------------
  Populating mmgsdi_uim_report_rsp_type content
  - the command was successful
  - report code
  - status words
  - command transacted or not
  - response data: index into mmgsdi client req table
  -----------------------------------------------------------------------*/
  msg_ptr->is_uim_success =
   ((mmgsdi_report_status == MMGSDI_SUCCESS)?TRUE:FALSE);
  if (msg_ptr->is_uim_success)
  {
    msg_ptr->status_word.sw1 = GSDI_SIM_SW1_NORM_END;
    msg_ptr->status_word.sw2 = 0x00;
  }
  else
  {
    msg_ptr->status_word.sw1 = 0xFF;
    msg_ptr->status_word.sw2 = 0xFF;
  }

  msg_ptr->uim_rpt_code = (int)UIM_NO_SUCH_REPORT_R;

  switch(mmgsdi_cmd_enum)
  {
    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_ACCESS_R;
      break;
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_SELECT_R;
      break;
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MANAGE_CHANNEL_R;
      break;
    case MMGSDI_SESSION_DEACTIVATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STATUS_R;
      break;
    case MMGSDI_ISIM_AUTH_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_ISIM_AUTHENTICATE_R;
      break;
    case MMGSDI_ENABLE_BDN_REQ:
    case MMGSDI_DISABLE_BDN_REQ:
    case MMGSDI_ACL_OPERATION_REQ:
      /* These 3 enums have no direct UIM_F, they are either translate to WRITE/REHABILITATE/INVALIDATE */
      /* UIM report not set */
      break;
    case MMGSDI_SEND_APDU_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STREAM_ISO7816_APDU_R;
      break;
    case MMGSDI_SAP_CONNECT_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_SWITCH_UIM_PASSIVE_R;
      break;
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
      /* No UIM Command associate with this, should this move to SAP report response later? */
      break;
    case MMGSDI_SAP_SEND_APDU_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STREAM_ISO7816_APDU_R;
      break;
    case MMGSDI_SAP_POWER_ON_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_UP_UIM_PASSIVE_R;
      break;
    case MMGSDI_SAP_POWER_OFF_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_DOWN_R;
      break;
    case MMGSDI_SAP_RESET_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_R;
      break;
    case MMGSDI_CARD_PUP_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_UP_UIM_PASSIVE_R;
      break;
    case MMGSDI_CARD_PDOWN_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_DOWN_R;
      break;
    case MMGSDI_CARD_STATUS_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STATUS_R;
      break;
    case MMGSDI_CARD_RESET_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_R;
      break;
    case MMGSDI_REFRESH_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_R;
      break;
    case MMGSDI_REHABILITATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_REHABILITATE_R;
      break;
    case MMGSDI_INVALIDATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_INVALIDATE_R;
      break;
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_SELECT_AID_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      break;
    case MMGSDI_USIM_AUTH_REQ:
    case MMGSDI_BCAST_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_AUTHENTICATE_R;
      break;
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_COMPUTE_IP_AUTH_R;
      break;
    case MMGSDI_RUN_CAVE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RUN_CAVE_R;
      break;
    case MMGSDI_GENERATE_KEY_VPM_REQ:
      msg_ptr->uim_rpt_code = UIM_GENERATE_KEYS_R;
      break;
    case MMGSDI_CARD_READER_STATUS_REQ:
    case MMGSDI_GET_ATR_REQ:
      /* No UIM Command associate with this */
      break;
    case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MFLO_INIT_ACT_PARAMS_R;
      break;
    case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MFLO_GET_SUBSCRIBER_ID_R;
      break;
    case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MFLO_GET_PUBLIC_KEY_R;
      break;
    case MMGSDI_MFLO_SIGN_UCAST_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MFLO_SIGN_UCAST_R;
      break;
    case MMGSDI_MFLO_VERIFY_UCAST_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MFLO_VERIFY_UCAST_R;
      break;
    case MMGSDI_CREATE_PKCS15_TABLE_REQ:
      break;
    case MMGSDI_SEARCH_REQ:
      break;
    case MMGSDI_CLIENT_ID_REG_REQ:
    case MMGSDI_CLIENT_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_DEREG_REQ:
    case MMGSDI_CLIENT_EVT_DEREG_REQ:
    case MMGSDI_SAP_INIT_REQ:
    case MMGSDI_SAP_PIN_VERIFY_REQ:
    case MMGSDI_SAP_DEREG_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    case MMGSDI_CLIENT_INIT_COMP_REQ:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
      /* Error, should not be handled by this function,
         client related function => client_id_report
         sap related function => sap_report */
      MSG_ERROR("cmd 0x%x should NOT be handled by mmgsdi_util_queue_mmgsdi_uim_report_rsp",
        mmgsdi_cmd_enum, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
      MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
      return MMGSDI_ERROR;
  }/* end switch - request type */

  msg_ptr->cmd_transacted  = FALSE;  //This should be false?
  msg_ptr->rsp_data_index  = index;
  /*-------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
    -------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi response queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
// TODO Use to free the index in the table!!!!!!
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_queue_mmgsdi_uim_report_rsp */


/* ==========================================================================
     FUNCTION:      mmgsdi_util_queue_mmgsdi_sap_report_rsp

     DESCRIPTION:
       This function is used to queue a mmgsdi_uim_report_rsp in the event that a
       command is not sent down the the UIM (ex, error before command is sent, or
       a read that comes from cache

     DEPENDENCIES:
       NONE

     LIMITATIONS:
       None

     RETURN VALUE:
       mmgsdi_return_enum_type
       MMGSDI_SUCCESS          - Command dequeued, or no commands pending
       MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
       MMGSDI_ERROR            - Other unknown failure

     SIDE EFFECTS:
       None
  ==========================================================================*/
  mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_sap_report_rsp(
    int32                    index,
    mmgsdi_cmd_enum_type     mmgsdi_cmd_enum,
    mmgsdi_return_enum_type  cmd_status
 )
{
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_generic_rsp_type         * msg_ptr = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(task_cmd_ptr,
                                            task_cmd_len);

   /*---------------------------------------------------------------------------
    - No Need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_report_rsp;

  /*-----------------------------------------------------------------------
    Populating mmgsdi_sap_report_rsp_type header
    1) Client ID = 0
    2) Request TYPE
    3) Slot ID: Set the slot to automatic since the info is inside the user
                data
    4) Request Task TCB
    5) Client Data Pointer: The previous request
    6) Response CallBack
    7) Payload len and request len will be populated after the content
      has been populated
    -----------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_REPORT_RSP;
  msg_ptr->request_header.client_id         = 0;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_REPORT_RSP;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_REPORT_RSP;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.request_id_ptr    = rex_self();
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = NULL;
  /*-----------------------------------------------------------------------
  Populating mmgsdi_uim_report_rsp_type content
  - the command was successful
  - report code
  - status words
  - command transacted or not
  - response data: index into mmgsdi client req table
  -----------------------------------------------------------------------*/
  msg_ptr->status = cmd_status;  /* Software Error */

  msg_ptr->rsp_data_index  = index;
  /*-------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
    -------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi response queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    MSG_ERROR("Command Queuing Fail 0x%x", mmgsdi_status, 0, 0);
// TODO Use to free the index in the table!!!!!!
    //mmgsdi_util_free_client_request_table_index(index);
  }
  return MMGSDI_SUCCESS;
  /*lint -esym(715,mmgsdi_cmd_enum)*/
} /* mmgsdi_util_queue_mmgsdi_sap_report_rsp */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CNF_RSP_HEADER

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     int32:
     0 FALSE (Not Initialized)
     1 TRUE  (Initialized)
     255 (GSDI_NOT_IN_LKUP_TABLE)

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_populate_cnf_rsp_header(
  const void*                         cnf_ptr,
  mmgsdi_response_header_type       * rsp_hdr_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_cnf_enum_type                cnf_cmd,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
)
{
  int32                              len = 0;
  mmgsdi_file_security_access_type  *file_security_access_ptr = NULL;
  if ((rsp_hdr_ptr == NULL) || (req_hdr_ptr == NULL))
    return;

  rsp_hdr_ptr->client_data  = req_hdr_ptr->client_data;
  rsp_hdr_ptr->client_id    = req_hdr_ptr->client_id;
  rsp_hdr_ptr->response_len = 0;

  switch(cnf_cmd)
  {
  case MMGSDI_CLIENT_ID_REG_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_client_id_reg_cnf_type));
    break;

  case MMGSDI_CLIENT_ID_DEREG_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_client_id_dereg_cnf_type));
    break;

  case MMGSDI_CLIENT_EVT_REG_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_client_evt_reg_cnf_type));
    break;

  case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */
    break;

  case MMGSDI_READ_CNF:
    if (cnf_ptr == NULL)
    {
      MSG_ERROR("Can't assign rsp_hdr_ptr->response_len: Read Cnf null", 0, 0, 0);
      return;
    }
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_read_cnf_type)) +
      ((mmgsdi_read_cnf_type*)cnf_ptr)->read_data.data_len;
    break;

  case MMGSDI_WRITE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_write_cnf_type));
    break;

  case MMGSDI_GET_FILE_ATTR_CNF:
    if (cnf_ptr == NULL)
    {
      MSG_ERROR("Can't assign rsp_hdr_ptr->response_len: Get File Attr Cnf null", 0, 0, 0);
      return;
    }
    switch (((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_type)
    {
    case MMGSDI_LINEAR_FIXED_FILE:
      file_security_access_ptr =
       &((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_info.linear_fixed_file.file_security;
      break;

    case MMGSDI_CYCLIC_FILE:
      file_security_access_ptr =
       &((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_info.cyclic_file.file_security;
      break;

    case MMGSDI_TRANSPARENT_FILE:
      file_security_access_ptr =
        &((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_info.transparent_file.file_security;
      break;

    case MMGSDI_MASTER_FILE:
    case MMGSDI_DEDICATED_FILE:
    case MMGSDI_MAX_FILE_STRUCTURE_ENUM:
      // TODO
      break;
    }
    if (file_security_access_ptr == NULL)
      return;
    len = file_security_access_ptr->read.num_protection_pin *
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->write.num_protection_pin *
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->increase.num_protection_pin *
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->invalidate_deactivate.num_protection_pin *
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    len += file_security_access_ptr->rehabilitate_activate.num_protection_pin *
      uint32toint32(sizeof(mmgsdi_pin_enum_type));
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_get_file_attr_cnf_type)) + len;
    break;

  case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
#ifdef  FEATURE_UIM_JCDMA_RUIM_SUPPORT
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_jcdma_get_card_info_cnf_type));
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */
    break;

  case MMGSDI_SESSION_OPEN_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_open_cnf_type));
    break;

  case MMGSDI_SESSION_CLOSE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_close_cnf_type));
    break;

  case MMGSDI_SESSION_DEACTIVATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_deactivate_cnf_type));
    break;

  case MMGSDI_SESSION_SELECT_AID_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_select_aid_cnf_type));
    break;

  case MMGSDI_ISIM_AUTH_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_isim_auth_cnf_type));
    break;

  case MMGSDI_SAP_INIT_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_DEREG_CNF:
   #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;

  case MMGSDI_SAP_VERIFY_PIN_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;

  case MMGSDI_SAP_CONNECT_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_DISCONNECT_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_GET_ATR_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SEND_APDU_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_SEND_APDU_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_POWER_ON_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_POWER_OFF_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_CARD_READER_STATUS_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_RESET_CNF:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_CARD_PUP_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_card_pup_cnf_type));
    break;
  case MMGSDI_CARD_PDOWN_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_card_pdown_cnf_type));
    break;

  case MMGSDI_CARD_STATUS_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_status_cnf_type));
    break;

  case MMGSDI_CLIENT_EVT_DEREG_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_client_evt_dereg_cnf_type));
    break;

  case MMGSDI_DISABLE_BDN_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_disable_bdn_cnf_type));
    break;

  case MMGSDI_ENABLE_BDN_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_enable_bdn_cnf_type));
    break;

  case MMGSDI_REHABILITATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_rehabilitate_cnf_type));
    break;
  case MMGSDI_INVALIDATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_invalidate_cnf_type));
    break;

  case MMGSDI_REFRESH_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_refresh_cnf_type));
    break;

  case MMGSDI_PIN_OPERATION_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_pin_operation_cnf_type));
    break;

  case MMGSDI_SELECT_AID_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_select_aid_cnf_type));
    break;

  case MMGSDI_GET_ALL_PIN_STATUS_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_get_all_pin_status_cnf_type));
    break;

  case MMGSDI_ACL_OPERATION_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_acl_operation_cnf_type));
    break;

  case MMGSDI_SRV_AVAILABLE_CNF:
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/
    break;

  case MMGSDI_USIM_AUTH_CNF:
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/
    break;
  case MMGSDI_COMPUTE_IP_AUTH_CNF:
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_compute_ip_auth_cnf_type)) +
        ((mmgsdi_compute_ip_auth_cnf_type*)cnf_ptr)->cmpt_ip_response_data.data_len;
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
    break;
  case MMGSDI_RUN_CAVE_CNF:
#ifdef FEATURE_MMGSDI_CDMA
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_run_cave_cnf_type));
#endif /* FEATURE_MMGSDI_CDMA */
    break;
  case MMGSDI_GENERATE_KEY_VPM_CNF:
#ifdef FEATURE_MMGSDI_CDMA
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_generate_key_vpm_cnf_type)) +
        ((mmgsdi_generate_key_vpm_cnf_type*)cnf_ptr)->octet_data.data_len;
#endif /* FEATURE_MMGSDI_CDMA */
    break;
  case MMGSDI_CARD_READER_STATUS_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_card_reader_status_cnf_type));
    break;
  case MMGSDI_GET_ATR_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_get_atr_cnf_type)) +
        ((mmgsdi_get_atr_cnf_type*)cnf_ptr)->atr_data.data_len;
    break;
  case MMGSDI_MFLO_INIT_ACT_PARAMS_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_GET_PUBLIC_KEY_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_SIGN_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_MFLO_VERIFY_UCAST_CNF:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;
  case MMGSDI_CARD_RESET_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_card_reset_cnf_type));
    break;
  case MMGSDI_CLIENT_INIT_COMP_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_client_init_comp_cnf_type));
    break;
  case MMGSDI_BCAST_CNF:
#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/
    break;

  case MMGSDI_SEARCH_CNF:
    if (cnf_ptr == NULL)
    {
      MSG_ERROR("Can't assign rsp_hdr_ptr->response_len: Search Cnf null", 0, 0, 0);
      return;
    }
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_search_cnf_type)) +
      ((mmgsdi_search_cnf_type*)cnf_ptr)->searched_record_nums.data_len;
    break;

  case MMGSDI_CREATE_PKCS15_TABLE_CNF:
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /*FEATURE_MMGSDI_PKCS15*/
    break;
  case MMGSDI_MAX_CNF_ENUM:
    return;
  }

  rsp_hdr_ptr->payload_len = rsp_hdr_ptr->response_len -
    uint32toint32(sizeof(mmgsdi_response_header_type));
  rsp_hdr_ptr->response_type          = cnf_cmd;
  rsp_hdr_ptr->slot_id                = req_hdr_ptr->slot_id;
  rsp_hdr_ptr->client_data            = req_hdr_ptr->client_data;
  rsp_hdr_ptr->status_word.present    = sw_present;
  rsp_hdr_ptr->status_word.sw1        = status_word.sw1;
  rsp_hdr_ptr->status_word.sw2        = status_word.sw2;
} /* mmgsdi_util_populate_cnf_rsp_header */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_SLOT

   DESCRIPTION:
     This function converts the mmgsdi slot type into uim slot type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_convert_uim_slot(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  uim_slot_type            *uim_slot_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(uim_slot_ptr);
  switch(mmgsdi_slot)
  {
  case MMGSDI_SLOT_1:
    *uim_slot_ptr = UIM_SLOT_1;
    break;
  case MMGSDI_SLOT_2:
    *uim_slot_ptr = UIM_SLOT_2;
    break;
  case MMGSDI_SLOT_AUTOMATIC:
    *uim_slot_ptr = UIM_SLOT_AUTOMATIC;
    break;
  default:
    *uim_slot_ptr = UIM_SLOT_NONE;
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid mmgsdi slot id 0x%x",
                             mmgsdi_slot, 0, 0);
    break;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_convert_uim_slot */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */


/* ==========================================================================
   FUNCTION:      MMGSDI_BUILD_UIM_RSP

   DESCRIPTION:
     This function will copy info from the uim report type and put it into the
     MMGSDI command queue and set the signal for RESPONSE processing.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static mmgsdi_return_enum_type  mmgsdi_build_uim_rsp (
  mmgsdi_uim_report_rsp_type **msg_ptr,
  const uim_rpt_type          *report_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  uint32                            rsp_data_len  = 0;

  MMGSDIUTIL_RETURN_IF_NULL(report_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(msg_ptr);

  /* Memory allocation for msg_ptr and check for allocation */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*msg_ptr,
                                           sizeof(mmgsdi_uim_report_rsp_type));

  /*---------------------------------------------------------------------------
    Populating mmgsdi_uim_report_rsp_type header
      1) Client ID = 0
      2) Request TYPE
      3) Slot ID: Set the slot to automatic since the info is inside the user
                  data
      4) Request Task TCB
      5) Client Data Pointer: The previous request
      6) Response CallBack
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  (*msg_ptr)->request_header.client_id          = 0;
  (*msg_ptr)->request_header.request_type       = MMGSDI_UIM_REPORT_RSP;
  (*msg_ptr)->request_header.orig_request_type  = MMGSDI_UIM_REPORT_RSP;
  (*msg_ptr)->request_header.slot_id            = MMGSDI_SLOT_1;
  (*msg_ptr)->request_header.request_id_ptr     = rex_self();
  (*msg_ptr)->request_header.client_data        = 0;
  (*msg_ptr)->request_header.response_cb        = NULL;
  (*msg_ptr)->rsp_data.data_len                 = 0;
  (*msg_ptr)->rsp_data.data_ptr                 = NULL;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_uim_report_rsp_type content
    - the command was successed or failed
    - report code
    - status words
    - command transacted or not
    - response data: index into mmgsdi client req table
  ---------------------------------------------------------------------------*/
  if (report_ptr->rpt_status == UIM_PASS)
  {
    (*msg_ptr)->is_uim_success = TRUE;
    switch (report_ptr->rpt_type)
    {
    case UIM_SELECT_R:
    case UIM_CACHED_SEL_R:
      (*msg_ptr)->rsp_data.data_len              =
         report_ptr->rpt.select.num_data_bytes;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         report_ptr->rpt.select.num_data_bytes, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr) {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.select.data,
                      report_ptr->rpt.select.num_data_bytes);
      }
      break;

    case UIM_READ_BIN_CMD_R:
    case UIM_READ_REC_CMD_R:
      (*msg_ptr)->rsp_data.data_len =
         report_ptr->rpt.read.num_bytes_read;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         report_ptr->rpt.read.num_bytes_read, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr) {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.read.data,
                      report_ptr->rpt.read.num_bytes_read);
      }
      break;

    case UIM_STATUS_R:
      (*msg_ptr)->rsp_data.data_len =
         report_ptr->rpt.status.num_bytes;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         report_ptr->rpt.status.num_bytes, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr) {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.status.data,
                      report_ptr->rpt.status.num_bytes);
      }
      break;

    case UIM_MANAGE_CHANNEL_R:
#if defined(FEATURE_UIM_UTIL)
      (*msg_ptr)->rsp_data.data_len = 1;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         1, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr) {
        (*msg_ptr)->rsp_data.data_ptr[0] = report_ptr->rpt.channel.channel_num;
      }
#endif /* FEATURE_UIM_UTIL */
      break;
    case UIM_COMPUTE_IP_AUTH_R:
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
      (*msg_ptr)->rsp_data.data_len = UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr)
      {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.compute_ip_auth.data,
                      UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
      }
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */
      break;

    case UIM_RUN_CAVE_R:
#if defined(FEATURE_MMGSDI_CDMA) && defined(FEATURE_AUTH)
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         sizeof(dword), mmgsdi_status);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        (*msg_ptr)->rsp_data.data_len = 0;
      }
      else if ((*msg_ptr)->rsp_data.data_ptr)
      {
        (*msg_ptr)->rsp_data.data_len = sizeof(dword);
        /*Pack the dword into uint8*/
        b_packd(report_ptr->rpt.run_cave.auth,
               (*msg_ptr)->rsp_data.data_ptr, 0,
               sizeof(dword)*8);
      }
#endif /*FEATURE_MMGSDI_CDMA && FEATURE_AUTH*/
      break;

    case UIM_GENERATE_KEYS_R:
#if defined(FEATURE_MMGSDI_CDMA) && defined(FEATURE_AUTH)
      (*msg_ptr)->rsp_data.data_len = UIM_MAX_CHARS;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         UIM_MAX_CHARS, mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        (*msg_ptr)->rsp_data.data_len = 0;
      }
      else if ((*msg_ptr)->rsp_data.data_ptr)
      {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.generate_keys.data,
                      UIM_MAX_CHARS);
      }
#endif /*FEATURE_MMGSDI_CDMA && FEATURE_AUTH*/
      break;

    case UIM_MFLO_GET_SUBSCRIBER_ID_R:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;

    case UIM_MFLO_GET_PUBLIC_KEY_R:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;

    case UIM_MFLO_SIGN_UCAST_R:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
      break;

    case UIM_SEEK_R:
      (*msg_ptr)->rsp_data.data_len = report_ptr->rpt.seek.num_records;
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
         report_ptr->rpt.seek.num_records, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr) {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.seek.data,
                      report_ptr->rpt.seek.num_records);
      }
      break;

    default:
      MSG_HIGH("Other UIM Response 0x%x", report_ptr->rpt_type, 0, 0);
      break;
    }
  }

  /* No matter success or failure, the card might return data back */
  if (report_ptr->rpt_type == UIM_ISIM_AUTHENTICATE_R)
  {
#ifdef FEATURE_MMGSDI_ISIM
    (*msg_ptr)->rsp_data.data_len              =
      report_ptr->rpt.isim_autn.data_length;
    if (report_ptr->rpt.isim_autn.data_length > 0)
    {
      /* memory allocation */
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*msg_ptr)->rsp_data.data_ptr,
        report_ptr->rpt.isim_autn.data_length, mmgsdi_status);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_ptr)->rsp_data.data_ptr) {
        (void) memcpy((*msg_ptr)->rsp_data.data_ptr,
                      report_ptr->rpt.isim_autn.data,
                      report_ptr->rpt.isim_autn.data_length);
      }
    }
#endif /* FEATURE_MMGSDI_ISIM */
  }

#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/

  if(report_ptr->rpt_type == UIM_STREAM_ISO7816_APDU_R)
  {
    if(report_ptr->rpt_status == UIM_PASS)
    {
       /*
       * Calculate the size of the data buffer that we need.
       */
      (*msg_ptr)->rsp_data.data_len =
        report_ptr->rpt.stream_iso7816_apdu.num_data_bytes;

      if (report_ptr->cmd_transacted) {
        if ((*msg_ptr)->rsp_data.data_len > (GSDI_MAX_CHANNEL_DATA_RSP_LEN-5)) {
          (*msg_ptr)->rsp_data.data_len = GSDI_MAX_CHANNEL_DATA_RSP_LEN - 4;
        }
        (*msg_ptr)->rsp_data.data_len = (*msg_ptr)->rsp_data.data_len + 4;
      } else  {
        (*msg_ptr)->rsp_data.data_len = 1;
      }
      /*
       * Memory allocation.
       */
      rsp_data_len = int32touint32((*msg_ptr)->rsp_data.data_len);

      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        (*msg_ptr)->rsp_data.data_ptr,
        rsp_data_len,
        mmgsdi_status);

      if ((*msg_ptr)->rsp_data.data_ptr != NULL) {
        if (report_ptr->cmd_transacted)
        {
          MSG_HIGH("SEND CHANNEL CMD COMPLETED",0,0,0);
          mmgsdi_status = MMGSDI_SUCCESS;
          /*
           * With the addition of FEATURE_MMGSDI_ATCSIM Support, there is
           * now a need to set two additional members of the confirmation.
           * Because of the nature of this function, the data is appended
           * appropriately to the data_p to be stripped out before sending
           * it back in the gsdi_cnf.
           */
          if ((*msg_ptr)->rsp_data.data_ptr != NULL) {
            (void)memcpy(
              (*msg_ptr)->rsp_data.data_ptr,
              report_ptr->rpt.stream_iso7816_apdu.data,
              report_ptr->rpt.stream_iso7816_apdu.num_data_bytes);
          }
          /*
           * Now append to the data_p accordingly.
           */
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-4] =
            (*msg_ptr)->status_word.sw1 = (uint8)report_ptr->sw1;
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-3] =
            (*msg_ptr)->status_word.sw2 = (uint8)report_ptr->sw2;
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-2] =
            (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw1;
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-1] =
            (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw2;
        } else {
          /*
           * The UIM Could not process the command due to a Software Error.
           */
          MSG_ERROR("CLOSE CHNL UIM DRIVER SOFTWARE ERROR",0,0,0);
          mmgsdi_status                    = MMGSDI_ERROR;
          (*msg_ptr)->rsp_data.data_ptr[0] = int32touint8(0); //TBD
        }
      }
    }
    else
    {
     if (report_ptr->cmd_transacted)
     {
        /* As long as the transaction was successful, even if the command did
           not pass, return the response APDU status words back to the
           streaming APDU client */
        (*msg_ptr)->is_uim_success = TRUE;
        (*msg_ptr)->rsp_data.data_len = 4;

         rsp_data_len = int32touint32((*msg_ptr)->rsp_data.data_len);

         MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
          (*msg_ptr)->rsp_data.data_ptr,
          rsp_data_len,
          mmgsdi_status);

        if ((*msg_ptr)->rsp_data.data_ptr != NULL) {

          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-4] =
              (*msg_ptr)->status_word.sw1 = (uint8)report_ptr->sw1;
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-3] =
              (*msg_ptr)->status_word.sw2 = (uint8)report_ptr->sw2;
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-2] =
              (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw1;
          (*msg_ptr)->rsp_data.data_ptr[(*msg_ptr)->rsp_data.data_len-1] =
              (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw2;
        }
      }
    }

  }

  (*msg_ptr)->uim_rpt_code    = (int32)report_ptr->rpt_type;
  (*msg_ptr)->status_word.sw1 = (uint8)report_ptr->sw1;
  (*msg_ptr)->status_word.sw2 = (uint8)report_ptr->sw2;
  (*msg_ptr)->cmd_transacted  = report_ptr->cmd_transacted;
  (*msg_ptr)->rsp_data_index  = uint32toint32(report_ptr->user_data);

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  (*msg_ptr)->request_header.request_len = uint32toint32(sizeof(mmgsdi_uim_report_rsp_type));
  (*msg_ptr)->request_header.payload_len = (*msg_ptr)->request_header.request_len
                                            - uint32toint32(sizeof(mmgsdi_request_header_type));

  return mmgsdi_status;
} /* mmgsdi_build_uim_rsp */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UIM_REPORT

   DESCRIPTION:
     This function is called when UIM report back command response

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     none

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_uim_report (
  uim_rpt_type *report_ptr
)
{
  mmgsdi_uim_report_rsp_type *msg_ptr       = NULL;
  mmgsdi_task_cmd_type       *task_cmd_ptr  = NULL;
  int32                       total_mem_len = 0;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_SUCCESS;
  uint32                      task_cmd_len  = 0;

  if (report_ptr == NULL)
  {
    MSG_ERROR("UIM reports Null report ptr", 0, 0, 0);
    return;
  }

  if (report_ptr->user_data < MMGSDI_MAX_CLIENT_INFO)
  {
    mmgsdi_status = mmgsdi_build_uim_rsp(&msg_ptr, report_ptr);

    if ((mmgsdi_status != MMGSDI_SUCCESS) || (msg_ptr == NULL))
    {
      MSG_ERROR("build uim rsp fail 0x%x or msg_ptr null, No rsp sent back to client",
                 mmgsdi_status, 0, 0);
      return;
    }
    total_mem_len = uint32toint32(sizeof(mmgsdi_task_cmd_type)) + msg_ptr->rsp_data.data_len;
    task_cmd_len  = sizeof(q_link_type) + int32touint32(total_mem_len);

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                           task_cmd_len,
                                           mmgsdi_status);

    if ((mmgsdi_status != MMGSDI_SUCCESS) || (task_cmd_ptr == NULL))
    {
      /* Free data */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((msg_ptr->rsp_data.data_ptr));
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(msg_ptr);
      MSG_ERROR("Unable to Alloc task_cmd_ptr", 0, 0, 0);
      return;
    }
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.uim_report_rsp, MMGSDI_UIM_REPORT_RSP);

    task_cmd_ptr->cmd.cmd_enum                = MMGSDI_UIM_REPORT_RSP;
    memcpy(&task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header,
      &msg_ptr->request_header,
      sizeof(mmgsdi_request_header_type));
    task_cmd_ptr->cmd.cmd.uim_report_rsp.is_uim_success = msg_ptr->is_uim_success;
    task_cmd_ptr->cmd.cmd.uim_report_rsp.uim_rpt_code = msg_ptr->uim_rpt_code;
    memcpy(&task_cmd_ptr->cmd.cmd.uim_report_rsp.status_word, &msg_ptr->status_word, sizeof(mmgsdi_sw_status_type));
    task_cmd_ptr->cmd.cmd.uim_report_rsp.cmd_transacted = msg_ptr->cmd_transacted;
    task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data_index = msg_ptr->rsp_data_index;

    if ((msg_ptr->rsp_data.data_len > 0) && (msg_ptr->rsp_data.data_ptr != NULL))
    {
      memcpy(task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data.data_ptr,
        msg_ptr->rsp_data.data_ptr,
        int32touint32(msg_ptr->rsp_data.data_len));
      task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data.data_len = msg_ptr->rsp_data.data_len;
    }

    /* Free temp data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((msg_ptr->rsp_data.data_ptr));
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(msg_ptr);

    task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header.request_len = total_mem_len;
    task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

    /* Put on mmgsdi command queue */
    if ( mmgsdi_cmd(task_cmd_ptr) != MMGSDI_SUCCESS)
    {
      /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
      MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
      ERR_FATAL("Not able to put to command queue!", 0, 0, 0);
    }
  }
  else
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("Unknown user data", report_ptr->user_data, 0, 0);
  }
} /* mmgsdi_uim_report */


/* ==========================================================================
   FUNCTION:      MMGSDI_UIM_REPORT_SYNCH

   DESCRIPTION:
     This function is called when UIM report back command response

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     none

   SIDE EFFECTS:

==========================================================================*/
static void mmgsdi_uim_report_synch (
   uim_rpt_type *report_ptr
)
{

  if (report_ptr == NULL)
  {
    MSG_ERROR("UIM reports Null report ptr", 0, 0, 0);
    return;
  }

  if (report_ptr->user_data < MMGSDI_MAX_CLIENT_INFO)
  {
    if (mmgsdi_internal_synch_uim_rpt_rsp != NULL)
    {
      //TODO
      MSG_HIGH("Internal SYNCH UIM RPT PTR is not NULL",0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        mmgsdi_internal_synch_uim_rpt_rsp->rsp_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp);
    }
    (void) mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp, report_ptr);
  }
  else
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("Unknown user data", report_ptr->user_data, 0, 0);
  }
  (void) rex_set_sigs(&gsdi_task_tcb, MMGSDI_SYNCH_UIM_REPORT_SIG);
} /* mmgsdi_uim_report_synch */ /*lint !e818 */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SEND_CMD_TO_UIM_SERVER_SYNCH

   DESCRIPTION:
     This function populate header information for uim_cmd_type and sends
     the command to uim module and wait synchronously for the reply

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server_synch (
    uim_cmd_type *cmd_ptr
)
{
    uim_rpt_type               *uim_report_ptr  = NULL;
    rex_sigs_type               sigs            = 0x00;
    mmgsdi_return_enum_type     mmgsdi_status   = MMGSDI_ERROR;

    if ( mmgsdi_generic_data.task_stopped )
    {
      ERR("GSDI TASK STOPPED...CAN'T PROCESS COMMAND ",0,0,0);
      return MMGSDI_ERROR;
    }
    if ( (rex_get_sigs(&gsdi_task_tcb) & GSDI_UIM_ERROR_SIG) )
    {
      ERR("UIM already notified error", 0, 0, 0);
      return MMGSDI_ERROR;
    }

    /* Commands can be sent to UIM in sync mode only in GSDI context */
    if(&gsdi_task_tcb != rex_self())
    {
      MSG_HIGH("mmgsdi_send_cmd_to_uim_server_synch() in non GSDI context",
               0, 0, 0);
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);

    MSG_HIGH("MMGSDI SEND CMD TO UIM",0,0,0);

    cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL;
    cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
    (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

    cmd_ptr->hdr.options      = UIM_OPTION_ALWAYS_RPT;
    cmd_ptr->hdr.rpt_function = mmgsdi_uim_report_synch;

    (void)rex_clr_sigs(&gsdi_task_tcb, MMGSDI_SYNCH_UIM_REPORT_SIG);
    (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

    /* Initialize the uim_report to not success so that it can be used in
       the failure conditions */
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(uim_report_ptr, sizeof(uim_rpt_type));

    uim_report_ptr->rpt_status = UIM_FAIL;
    uim_report_ptr->rpt_type   = UIM_NO_SUCH_REPORT_R;
    uim_report_ptr->sw1        = SW1_NO_STATUS;  /* 0x00 */
    uim_report_ptr->sw2        = SW2_NORMAL_END; /* 0x00 */
    uim_report_ptr->user_data  = cmd_ptr->hdr.user_data;
    uim_report_ptr->cmd_transacted = FALSE;

    if ( gsdi_data.task_stopped )
    {
      ERR("GSDI TASK STOPPED...CAN'T PROCESS COMMAND ",0,0,0);
      mmgsdi_status =
         mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp, uim_report_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
      return mmgsdi_status;
    }

    /* Set the 2 Minute Sanity Timer for each SIM Access */
    (void)rex_set_timer(&gsdi_uim_sanity_timer,
                        gsdi_data.gsdi_uim_sanity_timer_value);

    MSG_HIGH("SEND CMD TO UIM",0,0,0);
#ifdef FEATURE_GSDI_UIM_SERVER
    virtuim_cmd(cmd_ptr);
#else
    if (gsdi_data.recovering_after_reset)
    {
      MSG_HIGH("Sending down as a recovery command",0,0,0);
      if ( uim_recovery_cmd(cmd_ptr) != UIM_SUCCESS)
      {
        MSG_ERROR("Could not queue recovery command", 0, 0, 0);
        MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
        return MMGSDI_UIM_INTERNALLY_RESET;
      }
    }
    else
    {
      uim_cmd(cmd_ptr);
    }
#endif /* FEATURE_GSDI_UIM_SERVER */

    do
    {
      sigs = gsdi_wait(MMGSDI_SYNCH_UIM_REPORT_SIG
                      |GSDI_UIM_RESET_COMPLETE_SIG
                      |GSDI_UIM_ERROR_SIG
                      |GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

      MSG_HIGH("OUT OF WAIT FOR UIM: SIGS 0x%x",sigs,0,0);

      if ( (sigs & GSDI_UIM_RESET_COMPLETE_SIG) &&
           (cmd_ptr->hdr.command != UIM_POWER_DOWN_F) )
      {
        /* Handle the RESET Complete Sig in case we need to start/restart
           recovery.  If this is a POWER DOWN command, allow it to be processed
           by the UIM command queue regardless */

        /* If phone resets again, ignore Sanity Timer */
        (void)rex_clr_timer(&gsdi_uim_sanity_timer);
        /* Do not call recovery if already in progress */
        if (!gsdi_data.recovering_after_reset)
        {
          if ( mmgsdi_recover_after_reset(MMGSDI_SLOT_1) != MMGSDI_SUCCESS )
          {
            MSG_ERROR("Recovery failed", 0, 0, 0);
          }
        }
        else
        {
          MSG_HIGH("GSDI_UIM_RESET_COMPLETE_SIG received in recovery",0,0,0);
          MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
          if(!uim_safe_to_free(cmd_ptr))
          {
            /* Since MMGSDI considers this command to be a failure, and will
               deallocate it, make sure it is off the uim_cmd_q */
              MSG_HIGH("Failed MMGSDI command still on uim_cmd_q, removing",0,0,0);
              uim_remove_command_from_uim_cmd_q(cmd_ptr);
          }
          return MMGSDI_UIM_INTERNALLY_RESET;
        }
      }

    } while ( (sigs &
               (MMGSDI_SYNCH_UIM_REPORT_SIG |
                GSDI_UIM_ERROR_SIG |
                GSDI_UIM_SANITY_TIMER_EXPIRE_SIG)) == 0 );
    /* If the Task Stop Sig is detected, stop sending
    ** the UIM Task commands.
    */
    if ( sigs & TASK_STOP_SIG )
    {
      gsdi_data.task_stopped = TRUE;
      mmgsdi_generic_data.task_stopped = TRUE;
    }

    /* Continue with normal processing of the Signals
    */
    if ( sigs & MMGSDI_SYNCH_UIM_REPORT_SIG )
    {
      (void)rex_clr_sigs(&gsdi_task_tcb, MMGSDI_SYNCH_UIM_REPORT_SIG);
      (void)rex_clr_timer(&gsdi_uim_sanity_timer);
    }
    else if ( sigs & GSDI_UIM_ERROR_SIG )
    {
      /* Do not clear the SIG let the GSDI Task Main Wait Function
      ** handle it.
      */
      ERR("UIM ERROR SIG DURING COMMAND",0,0,0);
      (void)rex_clr_timer(&gsdi_uim_sanity_timer);
      mmgsdi_status = mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp, uim_report_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);

      if(!uim_safe_to_free(cmd_ptr))
      {
        /* Since MMGSDI considers this command to be a failure, and will
           deallocate it, make sure it is off the uim_cmd_q */
        MSG_HIGH("Failed MMGSDI command still on uim_cmd_q, removing",0,0,0);
        uim_remove_command_from_uim_cmd_q(cmd_ptr);
      }
      return mmgsdi_status;
    }
    else if ( sigs & GSDI_UIM_SANITY_TIMER_EXPIRE_SIG )
    {
      /* Do no clear the SIG let the GSDI Task Main Wait Function
      ** handle it.
      */
      ERR("GSDI - UIM SANITY TIMER EXPIRED",0,0,0);
      mmgsdi_status =
         mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp, uim_report_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
      if(!uim_safe_to_free(cmd_ptr))
      {
        /* Since MMGSDI considers this command to be a failure, and will
           deallocate it, make sure it is off the uim_cmd_q */
        MSG_HIGH("Failed MMGSDI command still on uim_cmd_q, removing",0,0,0);
        uim_remove_command_from_uim_cmd_q(cmd_ptr);
      }
      return mmgsdi_status;
    }

    /* free the memory used */
    MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
    return MMGSDI_SUCCESS;

} /* mmgsdi_send_cmd_to_uim_server_synch */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SEND_CMD_TO_UIM_SERVER

   DESCRIPTION:
     This function populate header information for uim_cmd_type and sends
     the command to uim module

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server (
    uim_cmd_type *cmd_ptr
)
{
    if ( mmgsdi_generic_data.task_stopped )
    {
      ERR("GSDI TASK STOPPED...CAN'T PROCESS COMMAND ",0,0,0);
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);

    MSG_HIGH("MMGSDI SEND CMD TO UIM",0,0,0);

    cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL;
    cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
    (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

    cmd_ptr->hdr.options      = UIM_OPTION_ALWAYS_RPT;
    cmd_ptr->hdr.rpt_function = mmgsdi_uim_report;

 //   (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_REPORT_SIG);
    (void)rex_clr_sigs(&gsdi_task_tcb, GSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

    /* Set the 2 Minute Sanity Timer for each SIM Access */
    (void)rex_set_timer(&gsdi_uim_sanity_timer,
                        gsdi_data.gsdi_uim_sanity_timer_value);

    MSG_HIGH("SEND CMD TO UIM",0,0,0);
#ifdef FEATURE_GSDI_UIM_SERVER
    virtuim_cmd(cmd_ptr);
#else
    uim_cmd(cmd_ptr);
#endif /* FEATURE_GSDI_UIM_SERVER */

    return MMGSDI_SUCCESS;

} /* mmgsdi_send_cmd_to_uim_server */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_UPDATE_STATE_VARIABLE

   DESCRIPTION:
     This function updates the state variable

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_update_state_variable(
  const mmgsdi_event_data_type *event_info_ptr)
{
  mmgsdi_slot_data_type *mmgsdi_data_slot_ptr = NULL;

  if(event_info_ptr == NULL)
    return;

  if (event_info_ptr->evt != MMGSDI_SWITCH_SLOT_EVT)
  {
    switch(event_info_ptr->data.card_inserted.slot)
    {
    case MMGSDI_SLOT_1:
      mmgsdi_data_slot_ptr = &mmgsdi_data_slot1;
      break;
    case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      mmgsdi_data_slot_ptr = &mmgsdi_data_slot2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
      break;
    default:
      return;
    }

    if(mmgsdi_data_slot_ptr == NULL)
      return;

    switch(event_info_ptr->evt)
    {
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
      /* Don't service any more requests */
      mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_NO_CARD;
      break;
    case MMGSDI_CARD_INIT_COMPLETED_EVT:
      mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_CARD_INIT_COMPLETED;
      memset(&(mmgsdi_data_slot_ptr->operational_apps), 0x00, sizeof(mmgsdi_apps_type));
      break;

    case MMGSDI_CARD_INSERTED_EVT:
      mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_CARD_INSERTED;
      break;

    case MMGSDI_SELECT_AID_EVT:
      mmgsdi_data_slot_ptr->mmgsdi_state = MMGSDI_STATE_APP_DETECTED;
      break;

    case MMGSDI_REFRESH_EVT:
    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
    case MMGSDI_FDN_EVT:
    case MMGSDI_SWITCH_SLOT_EVT:
    case MMGSDI_ILLEGAL_CARD_EVT:
    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_SAP_DISCONNECT_EVT:
    case MMGSDI_SAP_PIN_EVT:
    case MMGSDI_SESSION_CLOSE_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
    case MMGSDI_MAX_EVT_ENUM:
      break;
    }
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/

    return;
  }
  return;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_MMGSDI_EVENT_DATA

   DESCRIPTION:
     This function free any memory allocation for the mmgsdi_evet_data_type
     data structure for different command

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_util_free_mmgsdi_event_data(mmgsdi_event_data_type *event_info_ptr)
{
  if(event_info_ptr == NULL)
    return;

  switch(event_info_ptr->evt)
  {
    case MMGSDI_SESSION_CLOSE_EVT:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(event_info_ptr->data.session_close.app_id.data_ptr);
      return;
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
    case MMGSDI_CARD_INIT_COMPLETED_EVT:
    case MMGSDI_CARD_INSERTED_EVT:
    case MMGSDI_SELECT_AID_EVT:
    case MMGSDI_PIN1_EVT:
    case MMGSDI_PIN2_EVT:
    case MMGSDI_REFRESH_EVT:
    case MMGSDI_TERMINAL_PROFILE_DL_EVT:
    case MMGSDI_FDN_EVT:
    case MMGSDI_SWITCH_SLOT_EVT:
    case MMGSDI_ILLEGAL_CARD_EVT:
    case MMGSDI_SAP_CONNECT_EVT:
    case MMGSDI_SAP_DISCONNECT_EVT:
    case MMGSDI_SAP_PIN_EVT:
    case MMGSDI_UNIVERSAL_PIN_EVT:
    case MMGSDI_MAX_EVT_ENUM:
      return;
  }
}/* mmgsdi_util_free_mmgsdi_event_data */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_NOTIFY_CLIENT_EVT_EXT

   DESCRIPTION:
     This function send notification event to registered clients

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_notify_client_evt_ext(
  mmgsdi_event_data_type *event_info_ptr)
{
  int  i = 0;

  MMGSDIUTIL_RETURN_IF_NULL(event_info_ptr);

  /*---------------------------------------------------------------------------
    If gsdi already received the task stop signal, no event is needed to
    be sent to clients
  ---------------------------------------------------------------------------*/
  if ( mmgsdi_generic_data.task_stopped == TRUE )
  {
    return MMGSDI_SUCCESS;
  }

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i].evt_cb_ptr == NULL)
    {
      /* NULL callback */
      continue;
    }

    /* In case of session_close event, use the session id */
    if (MMGSDI_SESSION_CLOSE_EVT == event_info_ptr->evt)
    {
      int j = 0;

      for (j = 0; j < mmgsdi_client_id_reg_table[i].num_session &&
           j < MMGSDI_MAX_CHANNEL_INFO; j++)
      {
        mmgsdi_session_info_type* session_ptr =
          &mmgsdi_client_id_reg_table[i].session_info[j];

        if (session_ptr->session_id ==
            event_info_ptr->data.session_close.session_id)
        {
          /* Use the session id */
          event_info_ptr->client_id = session_ptr->session_id;
          /* Execute callback */
          (mmgsdi_client_id_reg_table[i].evt_cb_ptr)(event_info_ptr);
        }
      }
      continue;
    }

    /* In case of MMGSDI_PIN1_EVT, MMGSDI_PIN2_EVT or MMGSDI_UNIVERSAL_PIN_EVT
       we should use the session id, if the AID matches the AID of the session.
       Support for these three events MUST be added later */

    /* Use the client id */
    event_info_ptr->client_id = mmgsdi_client_id_reg_table[i].client_id;
    /* Execute callback */
    (mmgsdi_client_id_reg_table[i].evt_cb_ptr)(event_info_ptr);
  }

  mmgsdi_util_free_mmgsdi_event_data(event_info_ptr);

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_notify_client_evt_ext */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_NOTIFY_CLIENT_EVT

   DESCRIPTION:
     This function send notification event to registered clients

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_notify_client_evt(
  mmgsdi_event_data_type *event_info_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status  = MMGSDI_SUCCESS;
  gsdi_sim_events_T       old_gsdi_event = GSDI_NO_SIM_EVENT;

  MMGSDIUTIL_RETURN_IF_NULL(event_info_ptr);

  mmgsdi_status = mmgsdi_map_gsdi_mmgsdi_event(TRUE, event_info_ptr, &old_gsdi_event);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    (void)gsdi_util_send_mmi_sim_ind_ext(old_gsdi_event);
  }

  return mmgsdi_util_notify_client_evt_ext(
           event_info_ptr);
}/* mmgsdi_util_notify_client_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_ERROR_EVT

   DESCRIPTION:
     This function populate card error event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_card_error_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
)
{
  #ifdef FEATURE_MMGSDI_CARD_ERROR_INFO
  uim_status_type mmgsdi_uim_err_condition = UIM_NO_SUCH_STATUS_S;
  #endif /* FEATURE_MMGSDI_CARD_ERROR_INFO */

  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt = MMGSDI_CARD_ERROR_EVT;
  evt_info_ptr->data.card_error.slot = slot;

  #ifdef FEATURE_MMGSDI_CARD_ERROR_INFO

  switch ( slot )
  {
    case MMGSDI_SLOT_1:
      mmgsdi_uim_err_condition = gsdi_data.uim_error_condition;
      break;

    #ifdef FEATURE_MMGSDI_DUAL_SLOT
    case MMGSDI_SLOT_2:
      mmgsdi_uim_err_condition = gsdi_data.uim_error_condition_2;
      break;
    #endif /* FEATURE_MMGSDI_DUAL_SLOT */

    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
      return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Populate the Card Error Information based on the
     information stored in gsdi_data
     ------------------------------------------------------------------------*/
  switch ( mmgsdi_uim_err_condition )
  {
    case UIM_POLL_ERR_S:
      MSG_ERROR("MMGSDI_CARD_ERR_POLL_ERROR",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_POLL_ERROR;
      break;
    case UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S:
      MSG_ERROR("MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT;
      break;
    case UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S:
      MSG_ERROR("MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET;
      break;
    case UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S:
      MSG_ERROR("MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES;
      break;
    case UIM_PPS_TIMED_OUT_MAX_TIMES_S:
      MSG_ERROR("MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES;
      break;
    case UIM_VOLTAGE_MISMATCH_S:
      MSG_ERROR("MMGSDI_CARD_ERR_VOLTAGE_MISMATCH",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_VOLT_MISMATCH;
      break;
    case UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S:
      MSG_ERROR("MMGSDI_CARD_ERR_INTERNAL_CMD_TIMED_OUT_AFTER_PPS",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS;
      break;
    case UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S:
      MSG_ERROR("MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS;
      break;
    case UIM_MAXED_PARITY_ERROR_S:
      MSG_ERROR("MMGSDI_CARD_ERR_MAXED_PARITY_ERROR",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_MAXED_PARITY_ERROR;
      break;
    case UIM_MAXED_RX_BREAK_ERROR_S:
      MSG_ERROR("MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR;
      break;
    case UIM_MAXED_OVERRUN_ERROR_S:
      MSG_ERROR("MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR;
      break;
    case UIM_TRANSACTION_TIMER_EXPIRED_S:
      MSG_ERROR("MMGSDI_CARD_ERR_TRANSACTION_TIMER_EXPIRED",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_TRANS_TIMER_EXP;
      break;
    case UIM_POWER_DOWN_CMD_NOTIFICATION_S:
      MSG_ERROR("MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY;
      break;
    case UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S:
      MSG_ERROR("MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE;
      break;
    case UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S:
      MSG_ERROR("MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE;
      break;
    case UIM_MAX_PARITY_IN_PASSIVE_S:
      MSG_ERROR("MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE;
      break;
    case UIM_MAX_RXBRK_IN_PASSIVE_S:
      MSG_ERROR("MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE;
      break;
    case UIM_MAX_OVERRUN_IN_PASSIVE_S:
      MSG_ERROR("MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE;
      break;
    case UIM_ERR_S:
    default:
      MSG_ERROR("MMGSDI_CARD_ERR_UNKNOWN_ERROR",0,0,0);
      evt_info_ptr->data.card_error.info = MMGSDI_CARD_ERR_UNKNOWN_ERROR;
      break;
  }

  #endif /* FEATURE_MMGSDI_CARD_ERROR_INFO */

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_build_card_error_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_REMOVED_EVT

   DESCRIPTION:
     This function populate card removed event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_card_removed_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt = MMGSDI_CARD_REMOVED_EVT;
  evt_info_ptr->data.card_removed.slot = slot;
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_card_removed_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_INSERTED_EVT

   DESCRIPTION:
     This function populate card inserted event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_card_inserted_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
)
{
  uint32 i               = 0;
  uint32 evt_index       = 0;

  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  evt_info_ptr->evt = MMGSDI_CARD_INSERTED_EVT;
  evt_info_ptr->data.card_inserted.slot = slot;

  /*Send the event to clients and wait to decide which AID to select*/
  for(i=0;i<MMGSDI_MAX_APP_INFO;i++)
  {
    if((mmgsdi_data_slot1.app_info_ptr[i].app_data.app_type != MMGSDI_APP_NONE) &&
       (mmgsdi_data_slot1.app_info_ptr[i].proprietary_app == FALSE))
    {
      memcpy(&evt_info_ptr->data.card_inserted.aid_info[evt_index],
             &mmgsdi_data_slot1.app_info_ptr[i].app_data,
             sizeof(mmgsdi_aid_type));
      evt_info_ptr->data.card_inserted.num_aids_avail = ++evt_index;
    }
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_card_removed_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_INIT_COMPLETED_EVT

   DESCRIPTION:
     This function populate card init completed event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_card_init_completed_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
)
{
  mmgsdi_int_app_info_type      *sel_app_ptr = NULL;
  uint32                         i=0, j      = 0;

  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);


  evt_info_ptr->evt = MMGSDI_CARD_INIT_COMPLETED_EVT;
  evt_info_ptr->data.card_init_completed.slot = slot;

  mmgsdi_util_set_pin_info(&evt_info_ptr->data.card_init_completed.app_info[0].pin1,
                           sel_app_ptr->pin1);
  mmgsdi_util_set_pin_info(&evt_info_ptr->data.card_init_completed.app_info[0].pin2,
                           &sel_app_ptr->pin2);
  mmgsdi_util_set_pin_info(&evt_info_ptr->data.card_init_completed.app_info[0].universal_pin,
                            sel_app_ptr->universal_pin);


  memcpy(&evt_info_ptr->data.card_init_completed.app_info[i].app_data,
         &sel_app_ptr->app_data,
         sizeof(mmgsdi_aid_type));

  evt_info_ptr->data.card_init_completed.app_info[i].prov_app = TRUE;
  evt_info_ptr->data.card_init_completed.num_avail_apps = ++i;


  for(j=0;j<MMGSDI_MAX_APP_INFO;j++)
  {
    /*Skip prov app because it has already been taken care of above*/
    if(mmgsdi_data_slot1.app_info_ptr[j].app_data.app_type != MMGSDI_APP_NONE &&
       mmgsdi_data_slot1.sel_aid_index                     != j
      )
    {
      memcpy(&evt_info_ptr->data.card_init_completed.app_info[i].app_data,
             &mmgsdi_data_slot1.app_info_ptr[j].app_data,
             sizeof(mmgsdi_aid_type));

      evt_info_ptr->data.card_init_completed.app_info[i].prov_app = FALSE;
      evt_info_ptr->data.card_init_completed.num_avail_apps = ++i;
    }
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_card_init_completed_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_ILLEGAL_CARD_EVT

   DESCRIPTION:
     This function populate illegal event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_illegal_card_evt(
  mmgsdi_slot_id_enum_type     slot,
  mmgsdi_event_data_type      *evt_info_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt = MMGSDI_ILLEGAL_CARD_EVT;
  evt_info_ptr->data.illegal_card.slot = slot;
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_illegal_card_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_PIN_EVT

   DESCRIPTION:
     This function populate pin event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_pin_evt(
  mmgsdi_client_id_type       client_id,
  mmgsdi_slot_id_enum_type    slot,
  mmgsdi_pin_enum_type        pin,
  mmgsdi_pin_status_enum_type status,
  mmgsdi_event_data_type     *evt_info_ptr
)
{
  mmgsdi_int_app_info_type *sel_app_ptr   = NULL;
  uint32                    num_aids      = 0;
  uint32                    i =0,j        = 0;
  mmgsdi_app_pin_info_type  *pin_info_ptr = NULL;


  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);

  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  sel_app_ptr = mmgsdi_util_get_app_info(client_id, MMGSDI_APP_NONE);
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    /*Look for CDMA is any*/
    for(j=0;j<MMGSDI_MAX_APP_INFO;j++)
    {
      if(mmgsdi_data_slot1.app_info_ptr[j].app_data.app_type == MMGSDI_APP_RUIM)
      {
        num_aids++;
        memcpy(&evt_info_ptr->data.pin.aid_type[i++],
               &mmgsdi_data_slot1.app_info_ptr[j].app_data,
               sizeof(mmgsdi_aid_type));
        break;
      }
    }
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    /*Look for GSM is any*/
    for(j=0;j<MMGSDI_MAX_APP_INFO;j++)
    {
      if(mmgsdi_data_slot1.app_info_ptr[j].app_data.app_type == MMGSDI_APP_SIM)
      {
        num_aids++;
        memcpy(&evt_info_ptr->data.pin.aid_type[i++],
               &mmgsdi_data_slot1.app_info_ptr[j].app_data,
               sizeof(mmgsdi_aid_type));
        break;
      }
    }
  }
   /*If the selected app is USIM or any other, then just copy over
     num_aids will be 1 in this case*/

  /*Copy over the selected app's aid as well*/
  num_aids++;
  memcpy(&evt_info_ptr->data.pin.aid_type[i++],
         &sel_app_ptr->app_data,
         sizeof(mmgsdi_aid_type));

  evt_info_ptr->data.pin.num_aids = num_aids;

  switch (pin)
  {
  case MMGSDI_PIN1:
    pin_info_ptr = sel_app_ptr->pin1;
    evt_info_ptr->evt = MMGSDI_PIN1_EVT;
    break;
  case MMGSDI_PIN2:
    pin_info_ptr = &sel_app_ptr->pin2;
    evt_info_ptr->evt = MMGSDI_PIN2_EVT;
    break;
  case MMGSDI_UNIVERSAL_PIN:
    pin_info_ptr = sel_app_ptr->universal_pin;
    evt_info_ptr->evt = MMGSDI_UNIVERSAL_PIN_EVT;
    break;
  default: /* Rest of the PIN to be handle in later stage */
    return MMGSDI_ERROR;
  }
  evt_info_ptr->data.pin.slot = slot;
  evt_info_ptr->data.pin.pin_info.pin_id = pin;
  evt_info_ptr->data.pin.pin_info.status = status;
  evt_info_ptr->data.pin.pin_info.num_retries = pin_info_ptr->num_retries;
  evt_info_ptr->data.pin.pin_info.num_unblock_retries = pin_info_ptr->num_unblock_retries;
  evt_info_ptr->data.pin.pin_info.pin_replacement = pin_info_ptr->pin_replacement;

  MSG_HIGH("PIN 0x%x with replacement value 0x%x",
            evt_info_ptr->data.pin.pin_info.pin_replacement, 0, 0);

  return MMGSDI_SUCCESS;

}/* mmgsdi_util_build_pin_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_TP_DL_EVT

   DESCRIPTION:
     This function populate terminal profile download event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_tp_dl_evt(
  mmgsdi_slot_id_enum_type         slot,
  boolean                          support_tp,
  mmgsdi_event_data_type          *evt_info_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt = MMGSDI_TERMINAL_PROFILE_DL_EVT;
  evt_info_ptr->data.terminal_profile.slot = slot;
  evt_info_ptr->data.terminal_profile.ok_dl = support_tp;
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_tp_dl_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_FDN_EVT

   DESCRIPTION:
     This function populate FDN event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_fdn_evt(
  mmgsdi_slot_id_enum_type         slot,
  boolean                          fdn_enable,
  mmgsdi_event_data_type          *evt_info_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt = MMGSDI_FDN_EVT;
  evt_info_ptr->data.fdn.slot = slot;
  evt_info_ptr->data.fdn.enabled = fdn_enable;
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_fdn_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_SWITCH_SLOT_EVT

   DESCRIPTION:
     This function populate switch slot event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_switch_slot_evt(
  boolean                          start,
  mmgsdi_event_data_type          *evt_info_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  evt_info_ptr->evt = MMGSDI_SWITCH_SLOT_EVT;
  evt_info_ptr->data.switch_slot.start = start;
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_switch_slot_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_SESSION_CLOSE_EVT

   DESCRIPTION:
     This function populate session close event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_session_close_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_client_id_type      session_id,
  mmgsdi_static_data_type    app_id,
  mmgsdi_event_data_type    *evt_info_ptr
)
{
  uint32   length = 0;

  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt                           = MMGSDI_SESSION_CLOSE_EVT;
  evt_info_ptr->client_id                     = session_id;
  evt_info_ptr->data.session_close.slot       = slot;
  evt_info_ptr->data.session_close.session_id = session_id;
  evt_info_ptr->data.session_close.app_id.data_len = app_id.data_len;

  if ( (app_id.data_len > 0) && (app_id.data_len <= MMGSDI_MAX_AID_LEN))
  {
    length = int32touint32(app_id.data_len);

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
      evt_info_ptr->data.session_close.app_id.data_ptr,
      length);

    memcpy(evt_info_ptr->data.session_close.app_id.data_ptr,
           app_id.data_ptr,
           length);
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_session_close_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_AND_NOTIFY_EVT_FROM_STATE

   DESCRIPTION:
     This function builds the event to client who registered late based on
     the current state of the card.
     Multiple events may be sent out based on what is the currently state
     of MMGSDI

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_and_notify_evt_from_state(
  mmgsdi_slot_id_enum_type            slot,
  int32                               client_id_table_index
)
{
  mmgsdi_event_data_type   *evt_info_ptr              = NULL;
  mmgsdi_state_enum_type   *state_ptr                 = NULL;
  mmgsdi_return_enum_type   mmgsdi_status             = MMGSDI_ERROR;
  mmgsdi_int_app_info_type *slot_info_ptr             = NULL;
  boolean                   need_to_notify_at_the_end = TRUE;

  if ((client_id_table_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (client_id_table_index < 0))
  {
    MSG_ERROR("Exceed MMGSDI_MAX_CLIENT_INFO 0x%x",
               client_id_table_index, 0, 0);
    return mmgsdi_status;
  }

  switch (slot)
  {
    case MMGSDI_SLOT_1:
      slot_info_ptr = mmgsdi_data_slot1.app_info_ptr;
      state_ptr = &mmgsdi_data_slot1.mmgsdi_state;
      break;
    case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      state_ptr = &mmgsdi_data_slot2.mmgsdi_state;
      slot_info_ptr = mmgsdi_data_slot2.app_info_ptr;
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MSG_ERROR("Unknown Slot 0x%x", slot, 0, 0);
      return MMGSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
            evt_info_ptr,
            sizeof(mmgsdi_event_data_type),
            mmgsdi_status);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Unable to allocate evt_ptr and notify client 0x%x", mmgsdi_status, 0, 0);
    return mmgsdi_status;
  }

  evt_info_ptr->client_id = mmgsdi_client_id_reg_table[client_id_table_index].client_id;

  switch (*state_ptr)
  {
    case MMGSDI_STATE_CARD_INSERTED:
      mmgsdi_status = mmgsdi_util_build_card_inserted_evt(slot, evt_info_ptr);
      break;

    case MMGSDI_STATE_CARD_INIT_COMPLETED:
      mmgsdi_status = mmgsdi_util_build_card_init_completed_evt(slot, evt_info_ptr);
      break;

    case MMGSDI_STATE_NO_CARD:
      mmgsdi_status = mmgsdi_util_build_card_error_evt(slot, evt_info_ptr);
      break;

    case MMGSDI_STATE_APP_DETECTED:
      mmgsdi_status = mmgsdi_util_build_select_aid_evt(slot, evt_info_ptr);
      break;

    case MMGSDI_STATE_PIN_EVT_SENT:
       /* build and send out Select AID event */
      mmgsdi_status = mmgsdi_util_build_select_aid_evt(slot, evt_info_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("mmgsdi_util_build_select_aid_evt Slot: 0x%x",
                    slot, 0, 0);
      }
      else
      {
        (mmgsdi_client_id_reg_table[client_id_table_index].evt_cb_ptr)(evt_info_ptr);
      }

      /* build and send out pin1 event */
      if(slot_info_ptr->pin1 != NULL)
      {
        mmgsdi_status =  mmgsdi_util_build_pin_evt(mmgsdi_client_id_reg_table[client_id_table_index].client_id,
                                                   slot,
                                                   MMGSDI_PIN1,
                                                   slot_info_ptr->pin1->status,
                                                   evt_info_ptr);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("mmgsdi_util_build_pin_evt failed for PIN1 Slot: 0x%x",
                     slot, 0, 0);
        }
        else
        {
          (mmgsdi_client_id_reg_table[client_id_table_index].evt_cb_ptr)(evt_info_ptr);
        }
      }
      /* build and send out pin2 event */
      mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_client_id_reg_table[client_id_table_index].client_id,
                                                slot,
                                                MMGSDI_PIN2,
                                                slot_info_ptr->pin2.status,
                                                evt_info_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MSG_ERROR("mmgsdi_util_build_pin_evt failed for PIN2 Slot: 0x%x",
                   slot, 0, 0);
      }
      else
      {
        (mmgsdi_client_id_reg_table[client_id_table_index].evt_cb_ptr)(evt_info_ptr);
      }

      need_to_notify_at_the_end = FALSE;

      /* build and send out universal pin event */
      if(slot_info_ptr->universal_pin != NULL)
      {
        mmgsdi_status = mmgsdi_util_build_pin_evt(mmgsdi_client_id_reg_table[client_id_table_index].client_id,
                                                  slot,
                                                  MMGSDI_UNIVERSAL_PIN,
                                                  slot_info_ptr->universal_pin->status,
                                                  evt_info_ptr);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          MSG_ERROR("mmgsdi_util_build_pin_evt failed for UNIVERSAL PIN Slot: 0x%x",
                    slot, 0, 0);
        }
        else
        {
          need_to_notify_at_the_end = TRUE;
        }
      }
      break;

    case MMGSDI_STATE_NOT_INIT:
    case MMGSDI_STATE_ENUM_MAX:
      MSG_ERROR("State = 0x%x, nothing to build and notify", *state_ptr, 0, 0);
      need_to_notify_at_the_end = FALSE;
      mmgsdi_status =  MMGSDI_SUCCESS;
      break;
  }

  if (need_to_notify_at_the_end == TRUE)
  {
    (mmgsdi_client_id_reg_table[client_id_table_index].evt_cb_ptr)(evt_info_ptr);
  }
  MMGSDIUTIL_TMC_MEM_FREE( evt_info_ptr);

  return mmgsdi_status;
} /* mmgsdi_util_build_and_notify_evt_from_state */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CURRENT_AWAIT_CMD

   DESCRIPTION:
     This function returns the index of the command that has been sent to UIM
     for the longest time

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_current_await_cmd (
  int  *index_ptr
)
{
  int i = 0;
  uint32 min_seq = 0xFFFFFFFF;

  MMGSDIUTIL_RETURN_IF_NULL(index_ptr);
  *index_ptr = MMGSDI_MAX_CLIENT_INFO;
  for (i = 0; i< MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_req_table_info[i].request_tcb_ptr != NULL) {
      if (mmgsdi_client_req_table_info[i].process_seq < min_seq) {
        min_seq = mmgsdi_client_req_table_info[i].process_seq;
        *index_ptr = i;
      }
    }
  }
  if (*index_ptr == MMGSDI_MAX_CLIENT_INFO)
  {
    *index_ptr = 0;
    return MMGSDI_ERROR;
  }
  else
  {
    return MMGSDI_SUCCESS;
  }
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_MORE_PENDING_COMMAND

   DESCRIPTION:
     This function returns the index of the command that has been sent to UIM
     for the longest time

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_more_pending_command (
  boolean *more_pending_cmd_ptr
)
{
  int i = 0;

  MMGSDIUTIL_RETURN_IF_NULL(more_pending_cmd_ptr);
  *more_pending_cmd_ptr = FALSE;

  for (i = 0; i< MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_req_table_info[i].request_tcb_ptr != NULL) {
      *more_pending_cmd_ptr = TRUE;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_SUCCESS;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_ID_INDEX

   DESCRIPTION:
     This function checks if the client ID is valid and return the index
     location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_client_id_index (
  mmgsdi_client_id_type  client_id,
  int32                 *client_index_ptr,
  int32                 *session_index_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status         = MMGSDI_ERROR;
  int                     i                     = MMGSDI_MAX_CLIENT_INFO;
  int                     j                     = MMGSDI_MAX_CHANNEL_INFO;
  uint32                  input_client_id_shift = 0;
  uint32                  table_client_id_shift = 0;

  MMGSDIUTIL_RETURN_IF_NULL(client_index_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(session_index_ptr);

  /* comment out due to timing issue between when MMGSDI process its
     first client id reg and when we receive UIM link establish */
  /*if (client_id == 0)
  {
    MSG_ERROR("Client ID Passed in is 0", 0, 0, 0);
    return mmgsdi_status;
  }*/
  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    table_client_id_shift =
      (mmgsdi_client_id_reg_table[i].client_id >> MMGSDI_CLIENT_ID_SHIFT);
    input_client_id_shift = (client_id >> MMGSDI_CLIENT_ID_SHIFT);
    if (table_client_id_shift == input_client_id_shift)
    {
      if ((client_id & (uint64)MMGSDI_SESSION_ID_MASK) == 0)
      {
        MSG_HIGH("Default Channel", 0, 0, 0);
        mmgsdi_status = MMGSDI_SUCCESS;
        *client_index_ptr = i;
        *session_index_ptr = MMGSDI_MAX_CHANNEL_INFO;
        break;
      }
      for (j = 0; j < MMGSDI_MAX_CHANNEL_INFO; j++)
      {
        if ((mmgsdi_client_id_reg_table[i].session_info[j].session_id ==
            (client_id & (uint64)MMGSDI_SESSION_ID_MASK)) &&
            ((client_id & (uint64)MMGSDI_SESSION_ID_MASK) != (uint64)MMGSDI_INVALID_SESSION_ID))
        {
          mmgsdi_status = MMGSDI_SUCCESS;
          *client_index_ptr = i;
          *session_index_ptr = j;
          break;
        }
      }
      break;
    }
  }
  return mmgsdi_status;
}/* mmgsdi_util_get_client_id_index */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_SELECT_AID_EVT

   DESCRIPTION:
     This function populate select aid event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_build_select_aid_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
)
{
  mmgsdi_int_app_info_type    *sel_app_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(evt_info_ptr);
  memset(evt_info_ptr, 0x00, sizeof(mmgsdi_event_data_type));

  MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot, FALSE);
  evt_info_ptr->evt = MMGSDI_SELECT_AID_EVT;
  evt_info_ptr->data.card_inserted.slot = slot;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  /*Send the event to UI and wait to decide which AID to select*/
  evt_info_ptr->data.select_aid.app_data.aid.data_len = sel_app_ptr->app_data.aid.data_len;

  if(sel_app_ptr->app_data.aid.data_len)
  {
    memcpy(evt_info_ptr->data.select_aid.app_data.aid.data_ptr,
           sel_app_ptr->app_data.aid.data_ptr,
           int32touint32(sel_app_ptr->app_data.aid.data_len));
  }
  evt_info_ptr->data.select_aid.app_data.label.data_len = sel_app_ptr->app_data.label.data_len;

  if(sel_app_ptr->app_data.label.data_len)
  {
    memcpy(evt_info_ptr->data.select_aid.app_data.label.data_ptr,
           sel_app_ptr->app_data.label.data_ptr,
           int32touint32(sel_app_ptr->app_data.label.data_len));
  }
  evt_info_ptr->data.select_aid.app_data.app_type = sel_app_ptr->app_data.app_type;
  evt_info_ptr->data.select_aid.slot              = MMGSDI_SLOT_1;

  evt_info_ptr->data.select_aid.channel_id = 0; //??? Where should the channel come from
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_build_select_aid_evt */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_VALID_CLIENT_ID

   DESCRIPTION:
     This function checks if the client ID provided is in the registrating
     table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_is_valid_client_id (
  mmgsdi_client_id_type client_id
)
{
  int32                   client_index  = 0;
  int32                   session_index = 0;

  return mmgsdi_util_get_client_id_index(client_id,
    &client_index, &session_index);
}/* mmgsdi_util_is_valid_client_id */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_ALIGN_SIZE

   DESCRIPTION:
     This function calculates the aligned size

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_cal_align_size(
  int32                orig_size,
  int32               *aligned_size_ptr
)
{
  int32      mod = 0;
  if (aligned_size_ptr == NULL)
    return;
  mod = orig_size % MMGSDIUTIL_WORD_SIZE;
  if (mod == 0)
    *aligned_size_ptr = orig_size;
  else
    *aligned_size_ptr = orig_size + (MMGSDIUTIL_WORD_SIZE - mod);
}/* mmgsdi_util_cal_align_size */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_AND_ALLOC

   DESCRIPTION:
     This function calculates the total memory size required for the command
     type, allocate the memory for the command pointer, and properly assigning
     the pointer of member of the command type if applicable.
     This function also takes care of the word alignment for pointer assignment

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static void mmgsdi_util_align_ptr(
  void                *base_msg_ptr,
  void                **new_msg_ptr,
  int32                start_offset,
  int32                base_struct_size,
  int32               *end_offset_ptr)
{
  int32          aligned_var_size = 0;

  if (base_msg_ptr == NULL)
    return;
  if (end_offset_ptr == NULL)
    return;
  if (new_msg_ptr == NULL)
    return;

  mmgsdi_util_cal_align_size(base_struct_size,
                             &aligned_var_size);
  *new_msg_ptr = (uint8*)base_msg_ptr + start_offset + aligned_var_size;
  *end_offset_ptr = start_offset + aligned_var_size;
}/* mmgsdi_util_align_ptr */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_ALIGN_MEM

   DESCRIPTION:
     This function calculates the total memory size required for the command
     type, allocate the memory for the command pointer, and properly assigning
     the pointer of member of the command type if applicable.
     This function also takes care of the word alignment for pointer assignment

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_align_mem(
  void                *msg_ptr,
  mmgsdi_cmd_enum_type cmd
)
{
  int32   end_offset_ptr = 0;

  switch (cmd)
  {
  case MMGSDI_CLIENT_ID_REG_REQ:
  case MMGSDI_CLIENT_ID_DEREG_REQ:
  case MMGSDI_CLIENT_EVT_REG_REQ:
  case MMGSDI_CLIENT_EVT_DEREG_REQ:
  case MMGSDI_SESSION_CLOSE_REQ:
  case MMGSDI_SESSION_DEACTIVATE_REQ:
  case MMGSDI_SAP_INIT_REQ:
  case MMGSDI_SAP_DEREG_REQ:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_SAP_REPORT_RSP:
  case MMGSDI_CARD_STATUS_REQ:
    break;
  case MMGSDI_REFRESH_REQ:
    if (msg_ptr != NULL)
    {
      if(((mmgsdi_refresh_req_type*)(msg_ptr))->refresh_req_type ==
          MMGSDI_REFRESH_ORIG_REFRESH_REQ )
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.card_req.refresh_files.data_ptr,
          0,
          sizeof(mmgsdi_refresh_req_type),
          &end_offset_ptr);
      }
      else if(((mmgsdi_refresh_req_type*)(msg_ptr))->refresh_req_type ==
          MMGSDI_REFRESH_ORIG_REG_REQ ||
          ((mmgsdi_refresh_req_type*)(msg_ptr))->refresh_req_type ==
          MMGSDI_REFRESH_ORIG_DEREG_REQ)
      {
       /* We shall align both file_list and file path ptr here, however
          depending the type of reg/dereg request (by enum/ path) we
          shall set one of the ptrs to null in calling function */

        mmgsdi_util_align_ptr(msg_ptr,
           (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.reg_req.refresh_files.file_list_ptr,
           0,
           sizeof(mmgsdi_refresh_req_type),
           &end_offset_ptr);
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.reg_req.refresh_files.file_path_ptr,
          0,
          sizeof(mmgsdi_refresh_req_type),
          &end_offset_ptr);
      }
    }
    break;
  case MMGSDI_READ_REQ:
    // TODO Path assignment;    ((mmgsdi_read_req_type*)(*msg_ptr))->access
    break;
  case MMGSDI_WRITE_REQ:
    if (msg_ptr != NULL)
    {
      mmgsdi_util_align_ptr(msg_ptr,
      (void**)&((mmgsdi_write_req_type*)(msg_ptr))->data.data_ptr,
      0,
      sizeof(mmgsdi_write_req_type),
      &end_offset_ptr);
    }
    break;

  case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* #ifdef FEATURE_MMGSDI_ONCHIP_SIM */
    break;

  case MMGSDI_SESSION_SELECT_AID_REQ:
    break;

  case MMGSDI_GET_FILE_ATTR_REQ:
    // TODO Path
    break;

  case MMGSDI_SEND_APDU_REQ:
    if (msg_ptr != NULL)
    {
      mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_send_apdu_req_type*)(msg_ptr))->data.data_ptr,
        0,
        sizeof(mmgsdi_send_apdu_req_type),
        &end_offset_ptr);
    }
    break;

  case MMGSDI_UIM_REPORT_RSP:
    if (msg_ptr != NULL)
    {
      mmgsdi_util_align_ptr(msg_ptr,
      (void**)&((mmgsdi_uim_report_rsp_type*)(msg_ptr))->rsp_data.data_ptr,
      0,
      sizeof(mmgsdi_uim_report_rsp_type),
      &end_offset_ptr);
    }
    break;

  case MMGSDI_ISIM_AUTH_REQ:
  case MMGSDI_USIM_AUTH_REQ:
    if (msg_ptr != NULL)
    {
      mmgsdi_util_align_ptr(msg_ptr,
      (void**)&((mmgsdi_auth_req_type*)(msg_ptr))->auth_req.data_ptr,
      0,
      sizeof(mmgsdi_auth_req_type),
      &end_offset_ptr);
    }
    break;
  case MMGSDI_COMPUTE_IP_AUTH_REQ:
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
    if (msg_ptr != NULL)
    {
      switch(((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.operation_type)
      {
        case MMGSDI_CMPT_IP_HRPD_CHAP:
          mmgsdi_util_align_ptr(msg_ptr,
           (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
           0,
           sizeof(mmgsdi_compute_ip_req_type),
           &end_offset_ptr);
        break;
        case MMGSDI_CMPT_IP_MIP_MN_AAA:
          mmgsdi_util_align_ptr(msg_ptr,
           (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.mn_aaa_data.challenge_ptr,
           0,
           sizeof(mmgsdi_compute_ip_req_type),
           &end_offset_ptr);
        break;
        case MMGSDI_CMPT_IP_MIP_MN_HA:
          mmgsdi_util_align_ptr(msg_ptr,
           (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.mn_ha_data.registration_data_ptr,
           0,
           sizeof(mmgsdi_compute_ip_req_type),
           &end_offset_ptr);
        break;
        case MMGSDI_CMPT_IP_MIP_RRQ:
          mmgsdi_util_align_ptr(msg_ptr,
           (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.rrq_data.rrq_data_ptr,
           0,
           sizeof(mmgsdi_compute_ip_req_type),
           &end_offset_ptr);
        break;
        case MMGSDI_CMPT_IP_SIP_CHAP:
          /* chap_challenge is an array of type uint8 and length
             MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH */
          mmgsdi_util_align_ptr(msg_ptr,
           (void**)((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.sip_chap_data.chap_challenge,
           0,
           sizeof(mmgsdi_compute_ip_req_type),
           &end_offset_ptr);
        break;
      }

    }
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN  */
    break;
  case MMGSDI_SAP_PIN_VERIFY_REQ:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;

  case MMGSDI_CARD_READER_STATUS_REQ:
  case MMGSDI_GET_ATR_REQ:
    break;

  case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;

  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
  case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    break;

  case MMGSDI_MFLO_SIGN_UCAST_REQ:
  case MMGSDI_MFLO_VERIFY_UCAST_REQ:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;

  case MMGSDI_CNF:
    MSG_ERROR("CNF Type not handle in this function", 0, 0, 0);
    return;

  case MMGSDI_SAP_SEND_APDU_REQ:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;

  case MMGSDI_SAP_CONNECT_REQ:
  case MMGSDI_SAP_DISCONNECT_REQ:
  case MMGSDI_SAP_GET_ATR_REQ:
  case MMGSDI_SAP_POWER_ON_REQ:
  case MMGSDI_SAP_POWER_OFF_REQ:
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
  case MMGSDI_SAP_RESET_REQ:
    break;

  case MMGSDI_CARD_PDOWN_REQ:
  case MMGSDI_CARD_PUP_REQ:
  case MMGSDI_CARD_RESET_REQ:
    break;

  case MMGSDI_DISABLE_BDN_REQ:
  case MMGSDI_ENABLE_BDN_REQ:
  case MMGSDI_REHABILITATE_REQ:
  case MMGSDI_INVALIDATE_REQ:
  case MMGSDI_PIN_OPERATION_REQ:
  case MMGSDI_SELECT_AID_REQ:
  case MMGSDI_GET_ALL_PIN_STATUS_REQ:
  case MMGSDI_SESSION_OPEN_REQ:
  case MMGSDI_ACL_OPERATION_REQ:
  case MMGSDI_SRV_AVAILABLE_REQ:
  case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
  case MMGSDI_RUN_CAVE_REQ:
  case MMGSDI_GENERATE_KEY_VPM_REQ:
  case MMGSDI_CLIENT_INIT_COMP_REQ:
    break;

  case MMGSDI_BCAST_REQ:
    if (msg_ptr != NULL)
    {
      mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_bcast_req_type*)(msg_ptr))->bcast_req.data_ptr,
      0,
      sizeof(mmgsdi_bcast_req_type),
      &end_offset_ptr);
    }
    break;

  case MMGSDI_SEARCH_REQ:
    if (msg_ptr != NULL)
    {
      mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_search_req_type*)(msg_ptr))->data.data_ptr,
        0,
        sizeof(mmgsdi_search_req_type),
        &end_offset_ptr);
    }
    break;

  case MMGSDI_CREATE_PKCS15_TABLE_REQ:
    break;
  case MMGSDI_MAX_CMD_ENUM:
    MSG_ERROR("Invalid Cmd Type", 0, 0, 0);
    return;
  }

  return;
}/* mmgsdi_util_align_mem */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CHANNEL_ID

   DESCRIPTION:
     This function returns the channel ID that corresponds to the passed in
     client id

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_channel_id (
  mmgsdi_client_id_type  client_id,
  mmgsdi_channel_id_type *channel_id_ptr
)
{
  int32                    client_index = MMGSDI_MAX_CLIENT_INFO;
  int32                    session_index = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  MMGSDIUTIL_RETURN_IF_NULL(channel_id_ptr);

  *channel_id_ptr = 0;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id, &client_index, &session_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
    return mmgsdi_status;

  if (session_index == MMGSDI_MAX_CHANNEL_INFO)  /* channel ID default to 0 */
    return mmgsdi_status;

  *channel_id_ptr = mmgsdi_client_id_reg_table[client_index].session_info[session_index].channel_id;

  MSG_HIGH("Channel = 0x%x", *channel_id_ptr, 0, 0);
  return mmgsdi_status;
}/* mmgsdi_util_get_channel_id */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_AID

   DESCRIPTION:
     This function returns the AID that corresponds to the passed in
     client id. Function Applicable to UICC only.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_static_data_type * mmgsdi_util_get_aid (
  mmgsdi_client_id_type  client_id
)
{
  mmgsdi_int_app_info_type *app_info_ptr;

  app_info_ptr = mmgsdi_util_get_app_info(client_id, MMGSDI_APP_USIM);

  if(app_info_ptr == NULL)
    return NULL;

  return &app_info_ptr->app_data.aid;
}/* mmgsdi_util_get_aid */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_APP_INFO

   DESCRIPTION:
     This function returns the APP Info Pointer that corresponds to the passed in
     client id

     The function is applicable to both UICC and ICC. The caller the function
     needs to specify which app_type they need. If either is fine, then the caller
     needs to pass in MMGSDI_APP_NONE.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_int_app_info_type * mmgsdi_util_get_app_info (
  mmgsdi_client_id_type  client_id,
  mmgsdi_app_enum_type   app_type
)
{
  int32                    client_index = MMGSDI_MAX_CLIENT_INFO;
  int32                    session_index = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id, &client_index, &session_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
    return NULL;

  if (session_index == MMGSDI_MAX_CHANNEL_INFO)
  {
    if(app_type == MMGSDI_APP_USIM && gsdi_data.aid_available == FALSE)
    {
      MSG_HIGH("USIM Application Not initialized", 0, 0, 0);
      return NULL;
    }

    MSG_HIGH("USIM/SIM Application Info", 0, 0, 0);
    return (mmgsdi_util_get_sel_app());
  }

  MSG_HIGH("Other Application ID", 0, 0, 0);
  return mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr;
}/* mmgsdi_util_get_app_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_ALLOC_AND_POPULATE_EXTRA_PARAM

   DESCRIPTION:
     This function malloc the mmgsdi_client_req_extra_info_type pointer and
     populate the data.  If there is any error encountered in this function,
     the function wil free the allocated extra parameter pointer before
     returning error to the calling function

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_alloc_and_populate_extra_param(
  mmgsdi_cmd_enum_type                cmd_type,
  const void                         *cmd_ptr,
  mmgsdi_client_req_extra_info_type **extra_param_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint32                  data_len      = 0;
  MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);

  switch(cmd_type)
  {
  case MMGSDI_READ_REQ:

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_read_extra_info_type));

    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_ptr)->read_data.access,
      &((mmgsdi_read_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_ptr);
      return mmgsdi_status;
    }
    (*extra_param_ptr)->read_data.rec_num =
      ((mmgsdi_read_req_type*)cmd_ptr)->rec_num;
    (*extra_param_ptr)->read_data.offset =
      ((mmgsdi_read_req_type*)cmd_ptr)->offset;
    (*extra_param_ptr)->read_data.file_type =
      ((mmgsdi_read_req_type*)cmd_ptr)->file_type;
    break;

  case MMGSDI_WRITE_REQ:

    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_write_extra_info_type));

    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_ptr)->write_data.access,
      &((mmgsdi_write_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_ptr);
      return mmgsdi_status;
    }

    data_len = int32touint32(((mmgsdi_write_req_type*)cmd_ptr)->data.data_len);

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      (*extra_param_ptr)->write_data.data.data_ptr,
      data_len,
      mmgsdi_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("cmd_type: 0x%x failed for access type copy 0x%x",
        cmd_type, mmgsdi_status, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_ptr);
      return mmgsdi_status;
    }
    memcpy((*extra_param_ptr)->write_data.data.data_ptr,
      ((mmgsdi_write_req_type*)cmd_ptr)->data.data_ptr,
      int32touint32(((mmgsdi_write_req_type*)cmd_ptr)->data.data_len));

    (*extra_param_ptr)->write_data.rec_num =
      ((mmgsdi_write_req_type*)cmd_ptr)->rec_num;
    (*extra_param_ptr)->write_data.offset =
      ((mmgsdi_write_req_type*)cmd_ptr)->offset;
    (*extra_param_ptr)->write_data.data.data_len =
      ((mmgsdi_write_req_type*)cmd_ptr)->data.data_len;

    (*extra_param_ptr)->write_data.int_client_data =
       ((mmgsdi_write_req_type*)cmd_ptr)->int_client_data;
    break;

  case MMGSDI_GET_FILE_ATTR_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_get_file_info_extra_info_type));

    (*extra_param_ptr)->get_file_attr_data.int_client_data =
       ((mmgsdi_get_file_attr_req_type*)cmd_ptr)->int_client_data;

    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_ptr)->get_file_attr_data.access,
      &((mmgsdi_get_file_attr_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("cmd_type: 0x%x failed for access type copy 0x%x",
        cmd_type, mmgsdi_status, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_ptr);
    }
    break;

  case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
  case MMGSDI_CLIENT_ID_REG_REQ:
  case MMGSDI_CLIENT_ID_DEREG_REQ:
  case MMGSDI_CLIENT_EVT_REG_REQ:
  case MMGSDI_CLIENT_EVT_DEREG_REQ:
  case MMGSDI_SESSION_OPEN_REQ:
  case MMGSDI_SESSION_CLOSE_REQ:
  case MMGSDI_SESSION_DEACTIVATE_REQ:
  case MMGSDI_SESSION_SELECT_AID_REQ:
  case MMGSDI_ISIM_AUTH_REQ:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_UIM_REPORT_RSP:
  case MMGSDI_CNF:
  case MMGSDI_SAP_PIN_VERIFY_REQ:
  case MMGSDI_SAP_REPORT_RSP:
  case MMGSDI_CARD_STATUS_REQ:
  case MMGSDI_USIM_AUTH_REQ:
  case MMGSDI_COMPUTE_IP_AUTH_REQ:
  case MMGSDI_RUN_CAVE_REQ:
  case MMGSDI_GENERATE_KEY_VPM_REQ:
  case MMGSDI_BCAST_REQ:
    break;

  case MMGSDI_SEND_APDU_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
        *extra_param_ptr, sizeof(mmgsdi_send_apdu_extra_info_type));

    data_len = int32touint32(((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_len);

    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      (*extra_param_ptr)->send_apdu_data.data.data_ptr,
      data_len,
      mmgsdi_status);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR("cmd_type: 0x%x failed for data copy 0x%x",
        cmd_type, mmgsdi_status, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_ptr);
      return mmgsdi_status;
    }
    memcpy(
      (*extra_param_ptr)->send_apdu_data.data.data_ptr,
      ((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_ptr,
      int32touint32(
        ((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_len));
    (*extra_param_ptr)->send_apdu_data.data.data_len =
      ((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_len;
    (*extra_param_ptr)->send_apdu_data.channel_id =
      ((mmgsdi_send_apdu_req_type*)cmd_ptr)->channel_id;
    break;

  case MMGSDI_SAP_CONNECT_REQ: /*xxx_sap*/
    break;

  case MMGSDI_SAP_DISCONNECT_REQ:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;

  case MMGSDI_SAP_GET_ATR_REQ:
    break;

  case MMGSDI_SAP_SEND_APDU_REQ:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;

  case MMGSDI_SAP_POWER_ON_REQ:
  case MMGSDI_SAP_POWER_OFF_REQ:
    break;

  case MMGSDI_SAP_INIT_REQ:
    #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
    #endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
    break;
  case MMGSDI_SAP_DEREG_REQ:
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
  case MMGSDI_SAP_RESET_REQ:
  case MMGSDI_CARD_RESET_REQ:
    break;
  case MMGSDI_CARD_PDOWN_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                      *extra_param_ptr,
                      sizeof(mmgsdi_card_pdown_extra_info_type));
    (*extra_param_ptr)->pdown_data.option =
                      ((mmgsdi_card_pdown_req_type*)cmd_ptr)->option;
    break;
  case MMGSDI_CARD_PUP_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                      *extra_param_ptr,
                       sizeof(mmgsdi_card_pup_extra_info_type));
    (*extra_param_ptr)->pup_data.option =
                       ((mmgsdi_card_pup_req_type*)cmd_ptr)->option;
    break;

  case MMGSDI_REFRESH_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                       *extra_param_ptr,
                        sizeof(mmgsdi_card_refresh_extra_info_type));
    break;

  case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    break;

  case MMGSDI_REHABILITATE_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_rehabilitate_extra_info_type));

    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_ptr)->rehab_data.access,
      &((mmgsdi_rehabilitate_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(*extra_param_ptr);
      return mmgsdi_status;
    }
    break;

  case MMGSDI_INVALIDATE_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_invalidate_extra_info_type));

    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_ptr)->invalidate_data.access,
      &((mmgsdi_invalidate_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(*extra_param_ptr);
      return mmgsdi_status;
    }
    break;

  case MMGSDI_DISABLE_BDN_REQ:
  case MMGSDI_ENABLE_BDN_REQ:
    break;

  case MMGSDI_PIN_OPERATION_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
      *extra_param_ptr, sizeof(mmgsdi_pin_extra_info_type));

    /*PIN_ID*/
    (*extra_param_ptr)->pin_data.pin_info.pin_id = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_id;

    /*PIN_OP*/
    /*The orig_pin_op is filled in the only if all the data associated with the req has length of 0,
      At the external API, len = 0 is rejected, so this would allow MMGSDI to "internally" use this
      as a way to determine retry num process or not*/
    if(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len != 0){
      (*extra_param_ptr)->pin_data.retry_type = MMGSDI_RETRY_NONE;
    }
    (*extra_param_ptr)->pin_data.orig_pin_op = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op;
    break;

  case MMGSDI_SELECT_AID_REQ:
  case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    break;

  case MMGSDI_ACL_OPERATION_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_acl_operation_extra_info_type));

    (*extra_param_ptr)->acl_operation_data.acl_op =
       ((mmgsdi_acl_operation_req_type*)cmd_ptr)->acl_op;
    break;

  case MMGSDI_SRV_AVAILABLE_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
      sizeof(mmgsdi_srv_available_extra_info_type));
    break;

  case MMGSDI_CARD_READER_STATUS_REQ:
  case MMGSDI_GET_ATR_REQ:
  case MMGSDI_MFLO_INIT_ACT_PARAMS_REQ:
  case MMGSDI_MFLO_GET_SUBSCRIBER_ID_REQ:
  case MMGSDI_MFLO_GET_PUBLIC_KEY_REQ:
    break;

  case MMGSDI_MFLO_SIGN_UCAST_REQ:
  case MMGSDI_MFLO_VERIFY_UCAST_REQ:
#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */
    break;

  case MMGSDI_SEARCH_REQ:
    MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*extra_param_ptr,
                                        sizeof(mmgsdi_search_extra_info_type));

    mmgsdi_status = mmgsdi_util_copy_access_type(
                                  &(*extra_param_ptr)->search_data.access,
                                  &((mmgsdi_search_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_ptr);
      return mmgsdi_status;
    }
    break;

  case MMGSDI_CREATE_PKCS15_TABLE_REQ:
#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
    break;

  case MMGSDI_CLIENT_INIT_COMP_REQ:
  case MMGSDI_MAX_CMD_ENUM:
    break;

  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_alloc_and_populate_extra_param */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_CHANNEL

   DESCRIPTION:
     This function convert mmgsdi channel type to uim channel type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_convert_uim_channel(
  mmgsdi_channel_id_type mmgsdi_channel,
  uim_channel_type      *uim_channel
)
{
  MMGSDIUTIL_RETURN_IF_NULL(uim_channel);

  switch (mmgsdi_channel)
  {
  case 0:
    *uim_channel = UIM_CHANNEL0;
    return MMGSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
  case 1:
    *uim_channel = UIM_CHANNEL1;
    return MMGSDI_SUCCESS;
  case 2:
    *uim_channel = UIM_CHANNEL2;
    return MMGSDI_SUCCESS;
  case 3:
    *uim_channel = UIM_CHANNEL3;
    return MMGSDI_SUCCESS;
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */
  default:
    *uim_channel = UIM_CHANNEL0;
    break;
  }
  return MMGSDI_ERROR;
}/* mmgsdi_util_convert_uim_channel */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_SLOT_VALID

DESCRIPTION
  This function is used to verify the slot id received in the message.
  All channel management procedures require that the correct slot
  is provided in the request.

  The only acceptable values are:
  GSDI_SLOT_1 -> 0x01
  GSDI_SLOT_2 -> 0x02

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_slot_valid(
  mmgsdi_slot_id_enum_type slot
)
{
  boolean result = FALSE;

  switch ( slot )
  {
    case MMGSDI_SLOT_1:
      result = TRUE;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case MMGSDI_SLOT_2:
      result = TRUE;
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
      break;
  }
  return result;
} /* mmgsdi_chnl_mgt_is_slot_valid */


/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_CHANNEL_ID_VALID

DESCRIPTION
  This function is used to verify that the channel ID provided is a
  valid channel.

  The only acceptable values are:
  channel_id greater than or equal to 0

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_channel_id_valid(
  int32 channel_id
)
{
  if (channel_id >= 0)
    return TRUE;
  else
    return FALSE;
}


/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_DATA_LEN_VALID

DESCRIPTION
  This function is used to verify the length of the data meets the following
  requirements:

  1. Data must be greater than 0
  2. Data must be less than GSDI_MAX_DATA_BLOCK_LEN

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_len is valid for configuration
            FALSE: data_len is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_data_len_valid(
  int32 data_len
)
{
  if ( data_len <= 0 )
    return FALSE;

  /* --------------------------------------------------------------------------
     Allow the maximum for Send and Receive.  Allow the Send portion
     to regulate the length further and trim it down to
     ------------------------------------------------------------------------*/
  if ( data_len >= GSDI_MAX_CHANNEL_DATA_RSP_LEN )
    return FALSE;

  return TRUE;
} /* mmgsdi_chnl_mgt_is_data_len_valid*/


/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_DATA_BUFFER_VALID

DESCRIPTION
  This function is used to verify the buffer used is valid.

  1. Buffer Pointer Can not be NULL

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_buffer is valid for request
            FALSE: data_buffer is not valid for request

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_data_buffer_valid(
  const byte * data_buffer
)
{
  if ( data_buffer == NULL )
    return FALSE;

  return TRUE;
} /* mmgsdi_chnl_mgt_is_data_buffer_valid */


/*===========================================================================
FUNCTION MMGSDI_UTIL_PRINT_DATA_TO_DIAG

DESCRIPTION
  Generic Function used to print any data out onto diag.  The function takes
  a length and a data pointer and prints the data in hex format.  This
  function breaks each line to approximately 70 chars, so that the data
  would not be missed out when looking at the diag messages.

PARAMETERS
message: Message to be prefixed before each set of data
req_data: uint8 pointer to the data to be printed in hex
req_data_len: Length in bytes of the data that needs to be printed

DEPENDENCIES
  None

RETURN VALUE
  void
SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_util_print_data_to_diag(
  char*        message,
  const uint8* req_data,
  uint16       req_data_len)
{
  char ascii[17] = "0123456789ABCDEF";
  /*
   * For converting decimal to ascii. */

  char buffer[31];
  /*
   * Holds the ascii string representation of the data. */

  uint32 buffer_index = 0;
  /*
   * The last valid index in the buffer, will be set to '\0'. */

  int index;

  /*For traversing the data. */
  for (index = 0; index < req_data_len; index = index + 1) {
    /*
     * If the buffer is full flush it.
     */
    if ((buffer_index+3) >= sizeof(buffer)) {
      buffer[buffer_index] = '\0';
        MSG_SPRINTF_3(
          MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "%s(%d) %s ...\n", message, req_data_len, buffer);
      buffer_index = 0;
    }
    /*
     * Append HEX representation of the byte to the buffer.
     */
    if (req_data != NULL){
      buffer[buffer_index++] = ascii[req_data[index] / 16];
      buffer[buffer_index++] = ascii[req_data[index] % 16];
      buffer[buffer_index++] = ' ';
    }
  }
  /*
   * Flush the remainder of the buffer.
   */
  buffer[buffer_index] = '\0';
  MSG_SPRINTF_3(
    MSG_SSID_DFLT, MSG_LEGACY_HIGH,
    "%s(%d) %s\n", message, req_data_len, buffer);
}


/*===========================================================================
FUNCTION MMGSDI_UTIL_ALLOC

DESCRIPTION
  Generic Function used to allocate the memory requested.  Memory is always
  allocated from tmc_heap.

  Memory when obtained will always be memset to 0.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED:  Indication that the heap has been
                                         exhausted.
    MMGSDI_INCORRECT_PARAMS:             Indication that pointer provided
                                         is not NULL.
                                         -- or --
                                         size is not greater than 0.
SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_alloc(
  void ** mem_p,
  int32  size
)
{
  /* ------------------------------------------------------------------------
     Verify the Address of the pointer is not NULL
     ----------------------------------------------------------------------*/
  if ( mem_p == NULL )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("MMGSDI_INCORRECT_PARAMS: Bad mem_p.  Null",0,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }
  /* ------------------------------------------------------------------------
     Verify the Pointer provided hasn't already been allocated for any other
     purpose.  Therefore, if not NULL return
     ----------------------------------------------------------------------*/
  if ( *mem_p != NULL )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("MMGSDI_INCORRECT_PARAMS: Bad *mem_p. Not NULL: 0x%x",
                               mem_p,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* ------------------------------------------------------------------------
     MAX Memory size has not been defined.  Therefore, allow any size
     greater than 0.
     ----------------------------------------------------------------------*/
  if ( size < 0 )
  {
    *mem_p = NULL;
    MMGSDIUTIL_DEBUG_MSG_ERROR("MMGSDI_INCORRECT_PARAMS: alloc size < 0: 0x%x",size,0,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* -------------------------------------------------------------------------
     Get the memory required
     -----------------------------------------------------------------------*/
  *mem_p = mmgsdi_malloc(int32touint32(size));

  if ( *mem_p == NULL )
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR("Heap Exhausted",0,0,0);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* --------------------------------------------------------------------------
     Initiatialize the memory allocated to 0x00 and return success
     ------------------------------------------------------------------------*/
  memset(*mem_p,0x00,int32touint32(size));

  return MMGSDI_SUCCESS;
}/* mmgsdi_util_alloc */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CACHE_RECORD_LEN

   DESCRIPTION:
     This function will determine if the file and record are cached and return
     the record len in the pointer provided

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_cache_record_len(
  mmgsdi_access_type    access,
  mmgsdi_len_type       rec_num,
  mmgsdi_len_type      *rec_len_p
)
{
  mmgsdi_record_cache_node_type * current_node_p = NULL;
  mmgsdi_file_enum_type           EF_name        = MMGSDI_MAX_FILE_ENUM;

  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    MSG_HIGH("Access other than enum_access not supported in cache",0,0,0);
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Validate the input parameters and set the return value to a default
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(rec_len_p);
  *rec_len_p = 0;

  /* --------------------------------------------------------------------------
     check to see if mmgsdi_record_cache_list_node has been set with a
     valid Record entry
     ------------------------------------------------------------------------*/
  if ( mmgsdi_record_cache_list_node.head_p == NULL )
  {
    MSG_HIGH("Records Entries have not been cahced",0,0,0);
    return MMGSDI_NOT_FOUND;
  }

  current_node_p = mmgsdi_record_cache_list_node.head_p;

  /* --------------------------------------------------------------------------
    current_node_p can not be null because mmgsdi_record_cache_list_node
    is a global variable.  Can start checking with Head.
    -------------------------------------------------------------------------*/
  do
  {
    if (( current_node_p->record_data.EF_name == EF_name ) &&
        ( current_node_p->record_data.rec_num == rec_num ))
    {
      *rec_len_p = current_node_p->record_data.record_data.data_len;
      MSG_HIGH("Cache ARR found: REC LEN = 0x%x",*rec_len_p,0,0);
      return MMGSDI_SUCCESS;
    }

    /* Match is not made...go ahead and assign the next pointer
    ** to the current pointer so that it can be checked.
    */
    current_node_p = current_node_p->next_p;

  } while (current_node_p != NULL  );

  return MMGSDI_NOT_FOUND;
}/* mmgsdi_util_get_cache_record_len */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_ADD_CACHE_RECORD

   DESCRIPTION:
     This function check if the data requested to be added already exist in
     the cache or not, if not, it will add the data to the cache

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_add_cache_record(
  mmgsdi_access_type      access,
  mmgsdi_len_type         rec_num,
  const mmgsdi_data_type *data_ptr)
{
  mmgsdi_record_cache_node_type * new_node_p     = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_data_type                temp_data_buf;
  mmgsdi_file_enum_type           EF_name        = MMGSDI_MAX_FILE_ENUM;

  /* --------------------------------------------------------------------------
     Validate input parameters:
     1)  data_ptr can not be NULL
     2)  data_ptr->data_len must not exceed 255 Bytes (apdu limiation)
     3) Initialize any variables that need initialization
     ------------------------------------------------------------------------*/
  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    MSG_HIGH("Access other than enum_access not supported in cache",0,0,0);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);

  if ( data_ptr->data_len > MMGSDI_MAX_REC_LEN )
  {
    MSG_ERROR("Length exceeds expected max: 0x%x > 0x%x",
              data_ptr->data_len,MMGSDI_MAX_REC_LEN,0);
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(&temp_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /* --------------------------------------------------------------------------
     find if the data is already in the list.
     No need to cache if it is already cached.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_cache_record(access, rec_num, &temp_data_buf);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MSG_HIGH("Already has the data, no need to add, 0x%x, 0x%x",EF_name, rec_num, 0);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for a node to be linked in to the list
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_alloc((void *)&new_node_p,
                                    sizeof(mmgsdi_record_cache_node_type));

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MSG_ERROR("Node memory allocation failure: 0x%x",mmgsdi_status,0,0);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Allocate memory to store the Record Data
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_alloc((void *)&new_node_p->record_data.record_data.data_ptr,
                                    data_ptr->data_len);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Free allocated memory */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(new_node_p);
    MSG_ERROR("Node memory allocation failure: 0x%x",mmgsdi_status,0,0);
    return mmgsdi_status;
  }
  /* --------------------------------------------------------------------------
     Set the Node Pointer information
     ------------------------------------------------------------------------*/
  new_node_p->next_p  = NULL;
  new_node_p->record_data.EF_name = EF_name;
  new_node_p->record_data.rec_num = rec_num;
  new_node_p->record_data.record_data.data_len = data_ptr->data_len;
  memcpy(new_node_p->record_data.record_data.data_ptr,
         data_ptr->data_ptr,
         int32touint32(data_ptr->data_len));

  /* --------------------------------------------------------------------------
     First element
     ----------------------------------------------------------------------- */
  if ( mmgsdi_record_cache_list_node.head_p == NULL )
  {
    mmgsdi_record_cache_list_node.head_p = new_node_p;
    mmgsdi_record_cache_list_node.tail_p = new_node_p;
    return MMGSDI_SUCCESS;
  }

  /* --------------------------------------------------------------------------
     From this point on...running because this is not the first time
     an entry is being made into this list.  start by locating the tail
     of the list.  The new node will be linked to the tail to become the new
     tail.
     ----------------------------------------------------------------------- */
  /* --------------------------------------------------------------------------
     Now link the new_node_p to the tail of the list.
     ------------------------------------------------------------------------*/
  mmgsdi_record_cache_list_node.tail_p->next_p = new_node_p;
  mmgsdi_record_cache_list_node.tail_p = new_node_p;

  return MMGSDI_SUCCESS;
}/* mmgsdi_util_add_cache_record */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CACHE_RECORD

   DESCRIPTION:
     This function get the data from the record cache if a match is found

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_cache_record(
  mmgsdi_access_type    access,
  mmgsdi_len_type       rec_num,
  mmgsdi_data_type     *data_ptr)
{
  mmgsdi_record_cache_node_type * current_node_p = mmgsdi_record_cache_list_node.head_p;
  mmgsdi_file_enum_type           EF_name        = MMGSDI_MAX_FILE_ENUM;

  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    access.file.file_enum = (mmgsdi_file_enum_type)UIM_EF_BY_PATH;
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);

  if ( current_node_p == NULL )
  {
    MSG_ERROR("MMGSDI_NOT_FOUND: Record Cache not populated",0,0,0);
    return MMGSDI_NOT_FOUND;
  }

  do
  {
    if (( current_node_p->record_data.EF_name == EF_name ) &&
        ( current_node_p->record_data.rec_num == rec_num ))
    {
      /* Get the data, if the data len passed in is greater than the
        cache data, return the cached data and update the data len to
        reflect the actual data being copied, else copy the requested data len*/
      if (data_ptr->data_len > current_node_p->record_data.record_data.data_len)
      {
        data_ptr->data_len = current_node_p->record_data.record_data.data_len;
      }
      memcpy(data_ptr->data_ptr, current_node_p->record_data.record_data.data_ptr,
          int32touint32(data_ptr->data_len));
      return MMGSDI_SUCCESS;
    }

    current_node_p = current_node_p->next_p;
  } while ( current_node_p != NULL  );

  return MMGSDI_NOT_FOUND;
}/* mmgsdi_util_get_cache_record */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DELETE_CACHE_RECORD

   DESCRIPTION:
     This function delete the record cache requested by the caller

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_delete_cache_record(
  mmgsdi_access_type    access,
  mmgsdi_len_type       rec_num)
{
  mmgsdi_record_cache_node_type * next_node_p    = NULL;
  mmgsdi_record_cache_node_type * current_node_p = mmgsdi_record_cache_list_node.head_p;
  mmgsdi_file_enum_type           EF_name        = MMGSDI_MAX_FILE_ENUM;

  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    MSG_HIGH("Access other than enum_access not supported in cache",0,0,0);
    return MMGSDI_ERROR;
  }

  if ( current_node_p == NULL )
  {
    /* this case is to cover the first node when nothing has been added yet */
    MSG_ERROR("0x%x, 0x%x not in record cache for deletion", EF_name, rec_num, 0);
    return MMGSDI_NOT_FOUND;
  }

  next_node_p = current_node_p->next_p;

  if (next_node_p == NULL)
  {
    /* only 1 element in the list */
    if (( current_node_p->record_data.EF_name == EF_name ) &&
        ( current_node_p->record_data.rec_num == rec_num ))
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(current_node_p->record_data.record_data.data_ptr);
      current_node_p->record_data.record_data.data_len = 0;
      current_node_p->next_p = NULL;
      MMGSDIUTIL_TMC_MEM_FREE(current_node_p);
      mmgsdi_record_cache_list_node.head_p = NULL;
      mmgsdi_record_cache_list_node.tail_p = NULL;
      return MMGSDI_SUCCESS;
    }
    return MMGSDI_NOT_FOUND;
  }
  while ( next_node_p != NULL )
  {
    if (( next_node_p->record_data.EF_name == EF_name ) &&
        ( next_node_p->record_data.rec_num == rec_num ))
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(next_node_p->record_data.record_data.data_ptr);
      next_node_p->record_data.record_data.data_len = 0;
      current_node_p->next_p = next_node_p->next_p;
      if (current_node_p->next_p == NULL) {
        mmgsdi_record_cache_list_node.tail_p = current_node_p;
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(next_node_p);
      return MMGSDI_SUCCESS;
    }
    current_node_p = current_node_p->next_p;
    next_node_p = next_node_p->next_p;
  }
  return MMGSDI_NOT_FOUND;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DELETE_ALL_CACHE_RECORD

   DESCRIPTION:
     This function delete the entire record cache

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_delete_all_cache_record( void )
{
  mmgsdi_record_cache_node_type * current_node_p = mmgsdi_record_cache_list_node.head_p;
  mmgsdi_record_cache_node_type * temp_node_p = NULL;

  if ( current_node_p == NULL )
  {
    /* this case is to cover the first node when nothing has been added yet */
    MSG_HIGH("nothing to be delete", 0, 0, 0);
    return;
  }

  while ( current_node_p != NULL )
  {
    temp_node_p = current_node_p->next_p;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(current_node_p->record_data.record_data.data_ptr);
    memset(current_node_p, 0x00, sizeof(mmgsdi_record_cache_node_type));
    MMGSDIUTIL_TMC_MEM_FREE(current_node_p);
    current_node_p = temp_node_p;
  }
  mmgsdi_record_cache_list_node.tail_p = NULL;
  mmgsdi_record_cache_list_node.head_p = NULL;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_BY_PATH_ENUM

   DESCRIPTION:
     This function checks if the uim enum is those that has a different mapping
     in GSM than USIM, e.g., UIM_TELECOM_ADN, if it is,
     this function converts the uim enum to the right enum, e.g.,
     UIM_TELECOM_BY_PATH_ADN

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_convert_uim_by_path_enum(
  uim_items_enum_type                  *uim_file_items)
{
  if (uim_file_items == NULL) {
    return MMGSDI_ERROR;
  }
  switch (*uim_file_items) {
  case UIM_TELECOM_IAP:
    *uim_file_items = UIM_TELECOM_BY_PATH_IAP;
    break;
  case UIM_TELECOM_PBC:
    *uim_file_items = UIM_TELECOM_BY_PATH_PBC;
    break;
  case UIM_TELECOM_GRP:
    *uim_file_items = UIM_TELECOM_BY_PATH_GRP;
    break;
  case UIM_TELECOM_AAS:
    *uim_file_items = UIM_TELECOM_BY_PATH_AAS;
    break;
  case UIM_TELECOM_GAS:
    *uim_file_items = UIM_TELECOM_BY_PATH_GAS;
    break;
  case UIM_TELECOM_ANR:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANR;
    break;
  case UIM_TELECOM_SNE:
    *uim_file_items = UIM_TELECOM_BY_PATH_SNE;
    break;
  case UIM_TELECOM_EMAIL:
    *uim_file_items = UIM_TELECOM_BY_PATH_EMAIL;
    break;
  case UIM_TELECOM_ANRA:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRA;
    break;
  case UIM_TELECOM_ANRB:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRB;
    break;
  case UIM_TELECOM_ANRC:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRC;
    break;
  case UIM_TELECOM_ANR1:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANR1;
    break;
  case UIM_TELECOM_ANRA1:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRA1;
    break;
  case UIM_TELECOM_ANRB1:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRB1;
    break;
  case UIM_TELECOM_ANRC1:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRC1;
    break;
  case UIM_TELECOM_ADN1:
    *uim_file_items = UIM_TELECOM_BY_PATH_ADN1;
    break;
  case UIM_TELECOM_PBC1:
    *uim_file_items = UIM_TELECOM_BY_PATH_PBC1;
    break;
  case UIM_TELECOM_GRP1:
    *uim_file_items = UIM_TELECOM_BY_PATH_GRP1;
    break;
  case UIM_TELECOM_SNE1:
    *uim_file_items = UIM_TELECOM_BY_PATH_SNE1;
    break;
  case UIM_TELECOM_UID1:
    *uim_file_items = UIM_TELECOM_BY_PATH_UID1;
    break;
  case UIM_TELECOM_EMAIL1:
    *uim_file_items = UIM_TELECOM_BY_PATH_EMAIL1;
    break;
  case UIM_TELECOM_IAP1:
    *uim_file_items = UIM_TELECOM_BY_PATH_IAP1;
    break;
  case UIM_TELECOM_ADN:
    *uim_file_items = UIM_TELECOM_BY_PATH_ADN;
    break;
  case UIM_TELECOM_EXT1:
    *uim_file_items = UIM_TELECOM_BY_PATH_EXT1;
    break;
  case UIM_TELECOM_EXT2:
    *uim_file_items = UIM_TELECOM_BY_PATH_EXT2;
    break;
  case UIM_TELECOM_UID:
    *uim_file_items = UIM_TELECOM_BY_PATH_UID;
    break;
  case UIM_TELECOM_CCP1:
    *uim_file_items = UIM_TELECOM_BY_PATH_CCP1;
    break;
#ifdef FEATURE_USIM_1000_PBM
  case UIM_TELECOM_ANR2:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANR2;
    break;
  case UIM_TELECOM_ANRA2:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRA2;
    break;
  case UIM_TELECOM_ANRB2:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRB2;
    break;
  case UIM_TELECOM_ANRC2:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRC2;
    break;
  case UIM_TELECOM_ADN2:
    *uim_file_items = UIM_TELECOM_BY_PATH_ADN2;
    break;
  case UIM_TELECOM_PBC2:
    *uim_file_items = UIM_TELECOM_BY_PATH_PBC2;
    break;
  case UIM_TELECOM_GRP2:
    *uim_file_items = UIM_TELECOM_BY_PATH_GRP2;
    break;
  case UIM_TELECOM_SNE2:
    *uim_file_items = UIM_TELECOM_BY_PATH_SNE2;
    break;
  case UIM_TELECOM_UID2:
    *uim_file_items = UIM_TELECOM_BY_PATH_UID2;
    break;
  case UIM_TELECOM_EMAIL2:
    *uim_file_items = UIM_TELECOM_BY_PATH_EMAIL2;
    break;
  case UIM_TELECOM_IAP2:
    *uim_file_items = UIM_TELECOM_BY_PATH_IAP2;
    break;
  case UIM_TELECOM_ANR3:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANR3;
    break;
  case UIM_TELECOM_ANRA3:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRA3;
    break;
  case UIM_TELECOM_ANRB3:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRB3;
    break;
  case UIM_TELECOM_ANRC3:
    *uim_file_items = UIM_TELECOM_BY_PATH_ANRC3;
    break;
  case UIM_TELECOM_ADN3:
    *uim_file_items = UIM_TELECOM_BY_PATH_ADN3;
    break;
  case UIM_TELECOM_PBC3:
    *uim_file_items = UIM_TELECOM_BY_PATH_PBC3;
    break;
  case UIM_TELECOM_GRP3:
    *uim_file_items = UIM_TELECOM_BY_PATH_GRP3;
    break;
  case UIM_TELECOM_SNE3:
    *uim_file_items = UIM_TELECOM_BY_PATH_SNE3;
    break;
  case UIM_TELECOM_UID3:
    *uim_file_items = UIM_TELECOM_BY_PATH_UID3;
    break;
  case UIM_TELECOM_EMAIL3:
    *uim_file_items = UIM_TELECOM_BY_PATH_EMAIL3;
    break;
  case UIM_TELECOM_IAP3:
    *uim_file_items = UIM_TELECOM_BY_PATH_IAP3;
    break;
#endif /*FEATURE_USIM_1000_PBM*/
  default:
    break;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_uim_by_path_enum */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_BY_PATH_UIM_ENUM

   DESCRIPTION:
     This function check if the uim enum is under BY_PATH_START_EF, and convert
     to a corresponding enum, if the enum can't be found within the
     BY_PATH_START_EF, it will update the file_enum to UIM_EF_BY_PATH

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_find_by_path_uim_enum(
  uim_items_enum_type                  *uim_file_items
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  if (uim_file_items == NULL) {
    return MMGSDI_ERROR;
  }
  if (*uim_file_items == UIM_EF_BY_PATH) {
    /* Nothing to be done */
    return MMGSDI_SUCCESS;
  }
  if (((int)*uim_file_items & (UIM_EF_PATH_START << 8)) != (UIM_EF_PATH_START << 8))
  {
    /* Not under EF by path, check to see if it is one of those that are
       shared between GSM and telecom */
    mmgsdi_status = mmgsdi_util_convert_uim_by_path_enum(uim_file_items);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      *uim_file_items = UIM_EF_BY_PATH;
    }
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_find_by_path_uim_enum */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_UIM_PATH

DESCRIPTION
  This function is used to convert mmgsdi_access_type to uim path for
  MMGSDI_BY_PATH_ACCESS

  Input Path is expected to be:       translate to UIM path array:
    MF/EF                             MF,     0xFFFF, EF
    MF/DF1/EF                         DF1,    0xFFFF, EF
    MF/DF1/DF2/EF                     DF1,    DF2,    EF
    MF/ADF/EF                         0xFFFF, 0xFFFF, EF
    MF/ADF/DF1/EF                     0xFFFF, DF1,    EF

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_convert_to_uim_path(
  uim_items_enum_type                      orig_uim_file_items,
  const mmgsdi_access_type                *mmgsdi_access_ptr,
  const mmgsdi_uim_additional_params_type *add_path_ptr,
  uim_items_enum_type                     *update_uim_file_items_ptr,
  word                                    *uim_path_array
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_access_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(uim_path_array);
  MMGSDIUTIL_RETURN_IF_NULL(update_uim_file_items_ptr);

  /* initialize path array */
  memset(uim_path_array, 0xFF, (sizeof(uint16)*(UIM_MAX_PATH_ELEMENTS)));

  if (mmgsdi_access_ptr->access_method == MMGSDI_BY_PATH_ACCESS)
  {
    MSG_HIGH("MMGSDI Access Type: BY_PATH", 0, 0, 0);
    switch (mmgsdi_access_ptr->file.path_type.path_len)
    {
      case 0:
        MSG_ERROR("Unable to convert to UIM path, len too small 0",
          0, 0, 0);
        return MMGSDI_ERROR;
      case 1:
        MSG_HIGH("Has indicated MF only, no need to update path",
          0, 0, 0);
        break;
      case 2:
        MSG_HIGH("Is EF under MF", 0, 0, 0);
        uim_path_array[0] = (uint16)MMGSDI_MF_LEVEL;
        uim_path_array[UIM_MAX_PATH_ELEMENTS-2] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-1];
        break;
      case 3:
        MSG_HIGH("Is EF under 1st DF", 0, 0, 0);
        uim_path_array[UIM_MAX_PATH_ELEMENTS-2] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-1];
        uim_path_array[0] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-2];
        if (uim_path_array[0] == (uint16)MMGSDI_ADF_LEVEL)
        {
          uim_path_array[0] = (uint16)MMGSDI_DEFAULT_LEVEL;
        }
        break;
      case 4:
        MSG_HIGH("Is EF under 2nd DF", 0, 0, 0);
        uim_path_array[UIM_MAX_PATH_ELEMENTS-2] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-1];
        uim_path_array[1] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-2];
        uim_path_array[0] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-3];
        if (uim_path_array[0] == (uint16)MMGSDI_ADF_LEVEL)
        {
          uim_path_array[0] = (uint16)MMGSDI_DEFAULT_LEVEL;
        }
        break;
      /* for an EF with path {0x3F00, 0x7FFF, 0x7F66, 0x5F30, 0x4F36 }
         uim_path_array[] shall be {0x7FFF, 0x7F66, 0x5F30, 0x4F36}
      */
      case 5:
        MSG_HIGH("Is EF under 3rd DF", 0, 0, 0);
        uim_path_array[UIM_MAX_PATH_ELEMENTS-1] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-1];
        uim_path_array[2] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-2];
        uim_path_array[1] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-3];
        uim_path_array[0] =
          mmgsdi_access_ptr->file.path_type.path_buf[mmgsdi_access_ptr->file.path_type.path_len-4];
        break;
      default:
        MSG_ERROR("Unable to convert to UIM path, len invalid 0x%x",
          mmgsdi_access_ptr->file.path_type.path_len, 0, 0);
        return MMGSDI_ERROR;
    }
  }

  if(add_path_ptr != NULL)
  {
    if ((add_path_ptr->path_array_ptr != NULL) && (add_path_ptr->path_len > 0))
    {
      MSG_4(MSG_SSID_DFLT,
            MSG_LEGACY_HIGH,
            "Additional valid path ptr 0x%x, 0x%x, 0x%x, overwriting previously path if existed",
            add_path_ptr->path_array_ptr[0],
            add_path_ptr->path_array_ptr[1],
            add_path_ptr->path_array_ptr[2],
            add_path_ptr->path_array_ptr[3]);
      memcpy (uim_path_array,
              add_path_ptr->path_array_ptr,
              (UIM_MAX_PATH_ELEMENTS) * sizeof(uint16));
      mmgsdi_status = mmgsdi_util_find_by_path_uim_enum(&orig_uim_file_items);
      if (mmgsdi_status != MMGSDI_SUCCESS) {
        *update_uim_file_items_ptr = UIM_EF_BY_PATH;
      }
      else
      {
        *update_uim_file_items_ptr = orig_uim_file_items;
      }
    }
  } /* add_path_ptr != NULL */
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_to_uim_path */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SESSION_CLOSE_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function calls corresponding function cleanup session information,
     and notify client based on the corresponding client_index and session_index
     into the mmgsdi_client_id_reg_table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:
     The Session information for the indices will be cleanup, which include
     freeing up any relevant information for the session

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_session_close_cleanup_and_notify(
  int client_index,
  int session_index
)
{
  mmgsdi_event_data_type   event_notify_data;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;

  memset(&event_notify_data, 0x00, sizeof(mmgsdi_event_data_type));
  event_notify_data.evt = MMGSDI_MAX_EVT_ENUM;

  /* Range check for client index and session index */
  if ((client_index < 0) ||
      (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
      (session_index < 0) ||
      (session_index >=  MMGSDI_MAX_CHANNEL_INFO))
  {
    ERR("Invalid client index 0x%x or session index 0x%x", client_index, session_index, 0);
    return;
  }

  /* build the session close event */
  /* Notify the specific client only */
  if (mmgsdi_client_id_reg_table[client_index].evt_cb_ptr != NULL)
  {
    if (mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_util_build_session_close_evt(
        mmgsdi_client_id_reg_table[client_index].session_info[session_index].slot,
        mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id,
        mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr->app_data.aid,
        &event_notify_data);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        ERR("Unable to build event data to notify client for table[0x%x][0x%x]",
          client_index, session_index, 0);
      }
      else
      {
        /* Notify the specific client only */
        (mmgsdi_client_id_reg_table[client_index].evt_cb_ptr)(&event_notify_data);
      }
    }
    mmgsdi_util_free_mmgsdi_event_data(&event_notify_data);
  }
  else
  {
    MSG_HIGH("Unable to notify client, null event cb ptr", 0, 0, 0);
  }

  /* Perform cleanup for the session */
#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
  mmgsdi_status = mmgsdi_session_release_info(client_index, session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    ERR("Unable to cleanup for table[0x%x][0x%x]",
      client_index, session_index, 0);
  }
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */

  mmgsdi_client_id_reg_table[client_index].session_info[session_index].session_id =
    MMGSDI_MAX_CHANNEL_INFO;

  return;
}/* mmgsdi_util_session_close_cleanup_and_notify */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CLOSE_ALL_SESSIONS_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function goes through all the entries in the client id reg table,
     perform cleanup and notify the corresponding client

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_close_all_sessions_cleanup_and_notify(void)
{
  int                     client_index   = 0;
  int                     session_index  = 0;
  int                     num_session_done = 0;

  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    num_session_done = 0;

    /* loop until either
       1) we reach the max channel supported or
       2) we already cleanup and notify all the open session */
    for (session_index = 0;
         ((session_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (num_session_done != mmgsdi_client_id_reg_table[client_index].num_session));
         session_index++)
    {
      if (mmgsdi_client_id_reg_table[client_index].session_info[session_index].channel_id != 0)
      {
        /* the channel is opened */
        mmgsdi_util_session_close_cleanup_and_notify(client_index, session_index);
        num_session_done++;
      }
    }
  }
}/* mmgsdi_util_close_all_sessions_cleanup_and_notify */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION MMMGSDI_UTIL_GET_USIM_APPS

DESCRIPTION
  This function is used to get the number of usim apps.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
uint32 mmgsdi_util_get_usim_apps(void)
{
  uint32 num_apps = 0,i = 0;

  for(i=0;i<MMGSDI_MAX_APP_INFO;i++)
  {
    if(mmgsdi_data_slot1.app_info_ptr[i].app_data.app_type == MMGSDI_APP_USIM &&
       mmgsdi_data_slot1.app_info_ptr[i].app_data.aid.data_len > 0)
    {
      num_apps++;
    }
  }
  return num_apps;
}


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CMP_AID

DESCRIPTION
  This function is used to compare two app ids.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_cmp_aid(
  const mmgsdi_aid_type *aid1_ptr,
  const mmgsdi_aid_type *aid2_ptr)
{
  uint32  aid_len = 0;

  MMGSDIUTIL_RETURN_IF_NULL(aid1_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(aid2_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(aid1_ptr->aid.data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(aid2_ptr->aid.data_ptr);

  if((aid1_ptr->aid.data_len > UIM_AID_MAX_NAME_LENGTH) ||
     (aid1_ptr->aid.data_len <= 0))
  {
    MSG_ERROR("AID1 len 0x%x exceed UIM_AID_MAX_NAME_LENGTH",
               aid1_ptr->aid.data_len, 0, 0);
    return MMGSDI_ERROR;
  }

  if((aid2_ptr->aid.data_len > UIM_AID_MAX_NAME_LENGTH) ||
     (aid2_ptr->aid.data_len <= 0))
  {
    MSG_ERROR("AID2 len 0x%x exceed UIM_AID_MAX_NAME_LENGTH",
               aid2_ptr->aid.data_len, 0, 0);
    return MMGSDI_ERROR;
  }

  aid_len = int32touint32(MIN(aid1_ptr->aid.data_len,
                          aid2_ptr->aid.data_len));

  if(memcmp((void*)aid1_ptr->aid.data_ptr,
            (void*)aid2_ptr->aid.data_ptr,
            aid_len) == 0) {
    return MMGSDI_SUCCESS;
  }
  return MMGSDI_ERROR;
}/* mmgsdi_util_cmp_aid */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_INTERNAL_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands  generated by MMGSDI

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
----------------------------------------------------------------------------*/
void mmgsdi_util_internal_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/

  MSG_HIGH("MMGSDI_INTERNAL_CB: Rev'd Cnf=0x%x, Status=0x%x", cnf, status, 0);
  if (cnf == MMGSDI_MAX_CNF_ENUM)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Cnf Enum", 0, 0, 0);
    return;
  }
  if (cnf_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERR("Null Confirmation Ptr", 0, 0, 0);
    return;
  }

  if(cnf == MMGSDI_GET_ALL_PIN_STATUS_CNF)
  {
    if(status != MMGSDI_SUCCESS &&
       status != MMGSDI_CODE_BLOCKED &&
       status != MMGSDI_CODE_PERM_BLOCKED)
    {
      MSG_ERROR("GET PIN STATUS FAILED",0,0,0);
      MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        evt_info_ptr,
        sizeof(mmgsdi_event_data_type),
        mmgsdi_status);

      if(mmgsdi_status != MMGSDI_SUCCESS){
        return;
      }
      if(mmgsdi_util_build_card_error_evt(MMGSDI_SLOT_1,evt_info_ptr) == MMGSDI_SUCCESS)
        (void)mmgsdi_util_notify_client_evt(evt_info_ptr);

      MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
    }
    else
    {
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP)*/
      {
        /*Send the PIN Event*/
        mmgsdi_cnf_generate_get_all_pin_status_evt();
      }
    }
  }
}


/*===========================================================================
FUNCTION MMGSDI_UTIL_BUILD_PIN_REQ

DESCRIPTION
  This function is used to compare two app ids.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_pin_req(
  const mmgsdi_request_header_type  *req_header_ptr,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_pin_operation_enum_type     pin_op,
  mmgsdi_pin_operation_enum_type     orig_pin_op,
  mmgsdi_protocol_enum_type          protocol)
{
  mmgsdi_pin_operation_req_type *pin_req_ptr   = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;

  /* No variable len to include */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                            pin_req_ptr,
                            sizeof(mmgsdi_pin_operation_req_type));

  pin_req_ptr->request_header.client_id         = req_header_ptr->client_id;
  pin_req_ptr->request_header.request_type      = MMGSDI_PIN_OPERATION_REQ;
  pin_req_ptr->request_header.orig_request_type = req_header_ptr->orig_request_type;
  pin_req_ptr->request_header.request_len       = sizeof(mmgsdi_pin_operation_req_type);
  pin_req_ptr->request_header.payload_len       = 0;
  pin_req_ptr->request_header.request_id_ptr    = rex_self();
  pin_req_ptr->request_header.slot_id           = req_header_ptr->slot_id;
  pin_req_ptr->request_header.client_data       = req_header_ptr->client_data;
  pin_req_ptr->request_header.response_cb       = req_header_ptr->response_cb;

  /*TODO - GET ALL PIN NUM TRIES*/
  pin_req_ptr->pin_id                       = pin_id;
  pin_req_ptr->new_pin.data_len             = 0;
  memset(pin_req_ptr->new_pin.data_ptr, 0x00, MMGSDI_MAX_AID_LEN);
  pin_req_ptr->pin_op                       = orig_pin_op;

  if(protocol == MMGSDI_UICC)
  {
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/
  }
  else {
    if(pin_op == MMGSDI_PIN_OP_UNBLOCK){
      mmgsdi_status = mmgsdi_uim_icc_unblock_pin(pin_req_ptr,
                                                 MMGSDI_GET_NUM_UNBLOCK_RETRIES);
    }
    else if(pin_op == MMGSDI_PIN_OP_VERIFY){
      mmgsdi_status = mmgsdi_uim_icc_verify_pin(pin_req_ptr,
                                                MMGSDI_GET_NUM_RETRIES);
    }
  }
  MMGSDIUTIL_TMC_MEM_FREE(pin_req_ptr);
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMMGSDI_UTIL_BUILD_GET_FILE_ATTR_REQ

DESCRIPTION
  This function builds a get file attribute request and sends it to mmgsdi.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_get_file_attr_req(
  const mmgsdi_request_header_type  *req_header_ptr,
  mmgsdi_access_type                 access_type,
  mmgsdi_client_data_type            client_ref)
{
  mmgsdi_get_file_attr_req_type       *req_ptr               = NULL;
  mmgsdi_return_enum_type              mmgsdi_status         = MMGSDI_SUCCESS;

  if(NULL == req_header_ptr){
    return MMGSDI_ERROR;
  }
    /* No variable len to include */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(
                            req_ptr,
                            sizeof(mmgsdi_get_file_attr_req_type));

  req_ptr->request_header.client_id         = req_header_ptr->client_id;
  req_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  req_ptr->request_header.orig_request_type = req_header_ptr->orig_request_type;
  req_ptr->request_header.request_len       = sizeof(mmgsdi_get_file_attr_req_type);
  req_ptr->request_header.payload_len       = 0;
  req_ptr->request_header.request_id_ptr    = rex_self();
  req_ptr->request_header.slot_id           = req_header_ptr->slot_id;
  req_ptr->request_header.client_data       = req_header_ptr->client_data;
  req_ptr->request_header.response_cb       = req_header_ptr->response_cb;
  req_ptr->int_client_data                  = client_ref;
  req_ptr->activate_aid                     = TRUE;
  memcpy(&req_ptr->access, &access_type, sizeof(mmgsdi_access_type));
  mmgsdi_status = mmgsdi_uim_icc_select(req_ptr, FALSE, NULL);
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CAN_PROCEED_TO_POST_PIN1

DESCRIPTION
  This function checks the pin1 and univ pin status and the replacement
  condition to determine if post pin1 procedure is allowed or not

DEPENDENCIES
  None
LIMITATION
  Supports only MMGSDI_SLOT1 now.

RETURN VALUE
  TRUE:  Can proceed with Post PIN1 process
  FALSE: Cannot proceed with Post PIN1 process

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_can_proceed_to_post_pin1(mmgsdi_slot_id_enum_type slot)
{
  mmgsdi_int_app_info_type  *sel_app_ptr   = NULL;
  mmgsdi_app_pin_info_type  *pin1_ptr      = NULL;
  mmgsdi_app_pin_info_type  *univ_pin_ptr  = NULL;

  if(slot == MMGSDI_SLOT_2)
  {
    MSG_HIGH("Received slot 2, returning default value TRUE",0,0,0);
    return TRUE;
  }

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
  {
    MSG_ERROR("sel_app_ptr is NULL, Cannnot proceed with post pin1", 0, 0, 0);
    return FALSE;
  }

  pin1_ptr = sel_app_ptr->pin1;
  univ_pin_ptr = sel_app_ptr->universal_pin;

  if (pin1_ptr == NULL)
  {
    MSG_ERROR("PIN 1 ptr NULL, Cannnot proceed with post pin1", 0, 0, 0);
    return FALSE;
  }

  switch(pin1_ptr->status)
  {
    case MMGSDI_PIN_BLOCKED:
    case MMGSDI_PIN_PERM_BLOCKED:
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      MSG_HIGH("PIN1 Status 0x%x (Enabled!Verified, Blked, Perm Blk, Not init), Cannot proceed with post pin1",
                pin1_ptr->status, 0, 0);
      return FALSE;

    case MMGSDI_PIN_ENABLED_VERIFIED:
    case MMGSDI_PIN_UNBLOCKED:
    case MMGSDI_PIN_CHANGED:
      MSG_HIGH("PIN1 Status 0x%x (Enable&Verified, Changed or Unblocked), Can proceed with post pin1",
                pin1_ptr->status, 0, 0);
      return TRUE;

    case MMGSDI_PIN_DISABLED:
      if (pin1_ptr->pin_replacement != MMGSDI_PIN_REPLACED_BY_UNIVERSAL)
      {
        MSG_HIGH("PIN1 Disabled No Replacement, Can proceed with post pin1",
                0, 0, 0);
        return TRUE;
      }
      /* If replaced by Universal PIN, check if Universal PIN is verified,
         if not, then can't proceed with post pin1 */
      if(univ_pin_ptr == NULL)
      {
        MSG_ERROR("PIN1 Disabled W Replacement, Null Univ PIN ptr. Cannot proceed with post pin1",
            0, 0, 0);
        return FALSE;
      }
      switch(univ_pin_ptr->status)
      {
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        case MMGSDI_PIN_BLOCKED:
        case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
          MSG_HIGH("PIN1 Disabled W Replacement, UnivPin 0x%x (Enabled!Verified, Blked, Not Init), Cannot proceed with post pin1",
                univ_pin_ptr->status, 0, 0);
          return FALSE;

        default:
          MSG_HIGH("PIN1 Disabled W Replacement, UnivPin Verified/Disable 0x%x, Can proceed with post pin1",
                univ_pin_ptr->status, 0, 0);
          return TRUE;
      }
  }
  MSG_ERROR("Unknown pin1 status 0x%x, Cannot proceed with post pin1",
             pin1_ptr->status, 0, 0);
  return FALSE;
} /* mmgsdi_util_can_proceed_to_post_pin1 */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_SEL_APP

DESCRIPTION
  This function returns the currently selected app

DEPENDENCIES
  None

LIMITATIONS
  Will need modifications if two applications can be made active
  such as USIM/CSIM

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_int_app_info_type * mmgsdi_util_get_sel_app(void)
{
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;
  if(mmgsdi_data_slot1.sel_aid_index > MMGSDI_MAX_APP_INFO)
  {
    MSG_ERROR("App Index Invalid",0,0,0);
    return NULL;
  }
  sel_app_ptr = &mmgsdi_data_slot1.app_info_ptr[mmgsdi_data_slot1.sel_aid_index];

  if (sel_app_ptr->app_data.app_type == MMGSDI_APP_NONE ||
      sel_app_ptr->app_selected      == FALSE)
  {
    MSG_ERROR("Invalid sel_app type 0x%x or Invalid app selected 0x%x",
              sel_app_ptr->app_data.app_type,
              sel_app_ptr->app_selected,
              0);
    return NULL;
  }

  return sel_app_ptr;
}/* mmgsdi_util_get_sel_app */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_SEL_APP_PIN_PTR

DESCRIPTION
  This function gets the PIN Ptr for the given PIN ID for the default APP

DEPENDENCIES
  mmgsdi_util_get_sel_app

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_app_pin_info_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_app_pin_info_type*  mmgsdi_util_get_sel_app_pin_ptr(
  gsdi_pin_ids_T pin_id)
{
  mmgsdi_int_app_info_type *sel_app_ptr = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();

  if(sel_app_ptr == NULL)
  {
    MSG_ERROR("No app is selected",0,0,0);
    return NULL;
  }

  switch (pin_id)
  {
    case GSDI_PIN1:
      return sel_app_ptr->pin1;
    case GSDI_PIN2:
      return &sel_app_ptr->pin2;
    default:
      MSG_ERROR("Unknown PIN ID 0x%x",pin_id,0,0);
      return NULL;
  }
}


/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_UIM_CHV

DESCRIPTION
  This function converts mmgsdi key reference values to UIM CHV type

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  uim_chv_type

SIDE EFFECTS
  NONE
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static uim_chv_type mmgsdi_util_convert_to_uim_chv(
   mmgsdi_key_ref_values_enum_type key_ref)
{
  switch(key_ref)
  {
    case MMGSDI_PIN_APPL_1:
      return UIM_PIN1_APP1;
    case MMGSDI_PIN_APPL_2:
      return UIM_PIN1_APP2;
    case MMGSDI_PIN_APPL_3:
      return UIM_PIN1_APP3;
    case MMGSDI_PIN_APPL_4:
      return UIM_PIN1_APP4;
    case MMGSDI_PIN_APPL_5:
      return UIM_PIN1_APP5;
    case MMGSDI_PIN_APPL_6:
      return UIM_PIN1_APP6;
    case MMGSDI_PIN_APPL_7:
      return UIM_PIN1_APP7;
    case MMGSDI_PIN_APPL_8:
      return UIM_PIN1_APP8;
    case MMGSDI_SEC_PIN_APPL_1:
      return UIM_PIN2_APP1;
    case MMGSDI_SEC_PIN_APPL_2:
      return UIM_PIN2_APP2;
    case MMGSDI_SEC_PIN_APPL_3:
      return UIM_PIN2_APP3;
    case MMGSDI_SEC_PIN_APPL_4:
      return UIM_PIN2_APP4;
    case MMGSDI_SEC_PIN_APPL_5:
      return UIM_PIN2_APP5;
    case MMGSDI_SEC_PIN_APPL_6:
      return UIM_PIN2_APP6;
    case MMGSDI_SEC_PIN_APPL_7:
      return UIM_PIN2_APP7;
    case MMGSDI_SEC_PIN_APPL_8:
      return UIM_PIN2_APP8;
    case MMGSDI_UNIVERSAL_PIN_KEY:
      return UIM_UNIVERSAL_PIN;
    case MMGSDI_CHV1_KEY:
      return UIM_CHV1;
    case MMGSDI_CHV2_KEY:
      return UIM_CHV2;
    default:
      return UIM_PIN1_APP1;
  }
}/* mmgsdi_util_convert_to_uim_chv */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION MMGSDI_UTIL_GET_KEY_REF

DESCRIPTION
  This function gets the key reference based on the PIN ID and protocol
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_retrun_enum_type
SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type  mmgsdi_util_get_key_ref(
  mmgsdi_client_id_type     client_id,
  mmgsdi_pin_enum_type      pin,
  uim_chv_type             *chv)
{
  mmgsdi_int_app_info_type  *sel_app_ptr   = NULL;
  int32                      client_index  = MMGSDI_MAX_CLIENT_INFO;
  int32                      session_index = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id, &client_index, &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
    return mmgsdi_status;

  if (session_index == MMGSDI_MAX_CHANNEL_INFO)
  {
    /* Default channel */
    sel_app_ptr = mmgsdi_util_get_sel_app();
  }
  else
  {
    sel_app_ptr = mmgsdi_client_id_reg_table[client_index].session_info[session_index].app_info_ptr;
  }

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  switch(pin)
  {
    case MMGSDI_PIN1:
      if(sel_app_ptr->pin1){
        *chv = mmgsdi_util_convert_to_uim_chv(sel_app_ptr->pin1->key_ref);
        break;
      }
      MSG_HIGH("ERROR - PIN1 Not set yet",0,0,0);
      return MMGSDI_ERROR;

    case MMGSDI_PIN2:
      *chv = mmgsdi_util_convert_to_uim_chv(sel_app_ptr->pin2.key_ref);
      break;

    case MMGSDI_UNIVERSAL_PIN:
      if(sel_app_ptr->universal_pin){
        *chv =mmgsdi_util_convert_to_uim_chv(sel_app_ptr->universal_pin->key_ref);
        break;
      }
      MSG_HIGH("ERROR - UNIVERSAL PIN Not set yet",0,0,0);
      return MMGSDI_ERROR;

    default:
      MSG_HIGH("ERROR - Invalid PIN %d",pin,0,0);
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_get_key_ref */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_DATA

DESCRIPTION
  This function resets all the app related data including PIN

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
==========================================================================*/
void mmgsdi_util_reset_app_data(
  boolean reselect_app
)
{
  int                               i;
  mmgsdi_key_ref_values_enum_type   pin1_key_ref = MMGSDI_PIN_APPL_1;
  mmgsdi_key_ref_values_enum_type   pin2_key_ref = MMGSDI_SEC_PIN_APPL_1;

  /* reset the app info only if it is not for reselecting the app */
  if(!reselect_app)
  {
    for(i =0;i<MMGSDI_MAX_APP_INFO;i++)
    {
      memset(&mmgsdi_data_slot1.app_info_ptr[i], 0x00, sizeof(mmgsdi_int_app_info_type));
    }
  }

  /* reset the pin info */
  for(i =0;i<MMGSDI_MAX_APP_INFO;i++)
  {
    memset(&mmgsdi_app_pin1_table[i],0,sizeof(mmgsdi_app_pin_info_type));
    mmgsdi_app_pin1_table[i].pin_id  = MMGSDI_PIN1;
    mmgsdi_app_pin1_table[i].key_ref = pin1_key_ref++;

    mmgsdi_data_slot1.app_info_ptr[i].pin2.pin_id   = MMGSDI_PIN2;
    mmgsdi_data_slot1.app_info_ptr[i].pin2.key_ref  = pin2_key_ref++;

    mmgsdi_data_slot1.esn_me_change_flag = MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  pin2_key_ref = MMGSDI_SEC_PIN_APPL_1;

  for(i =0;i<MMGSDI_MAX_APP_INFO;i++)
  {
    memset(&mmgsdi_data_slot2.aid_info[i], 0x00, sizeof(mmgsdi_int_app_info_type));
    mmgsdi_data_slot2.app_info[i].pin2.pin_id   = MMGSDI_PIN2;

    mmgsdi_data_slot2.app_info_ptr[i].pin2.key_ref  = pin2_key_ref++;
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  memset(&mmgsdi_app_univ_pin_table[0],0,sizeof(mmgsdi_app_pin_info_type));
  mmgsdi_app_univ_pin_table[0].pin_id  = MMGSDI_UNIVERSAL_PIN;
  mmgsdi_app_univ_pin_table[0].key_ref = MMGSDI_UNIVERSAL_PIN_KEY;

}


/* ==========================================================================
FUNCTION MMGSDI_UTIL_FIND_APP_PIN_INDEX

DESCRIPTION
  This function finds the index into one of the foolowing tables -
  1. mmgsdi_app_pin1_table
  2. mmgsdi_app_pin2_table
  3. mmgsdi_app_univ_pin_table

  It first finds if the key reference already exists in the table,
  If  not, then it looks for the first empty slot.

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
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_find_app_pin_index(
  const mmgsdi_app_pin_info_type  *app_pin_table,
  mmgsdi_key_ref_values_enum_type  key_ref,
  uint32                          *index)
{
  uint32         i                = 0;

  for(i=0;i<MMGSDI_MAX_APP_INFO;i++){
    if(app_pin_table[i].key_ref  == key_ref)
    {
      *index = i;
      return MMGSDI_SUCCESS;
    }
  }

  MSG_ERROR("No Slot is empty and key ref does not already exist",0,0,0);
  return MMGSDI_ERROR;

}/* mmgsdi_util_find_app_pin_index */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET

DESCRIPTION
  This function reset the global data of gsdi and mmgsdi based on the refresh mode

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_reset(
  mmgsdi_refresh_mode_enum_type refresh_mode,
  mmgsdi_slot_id_enum_type      slot_id,
  boolean                       reselect_app)
{
  MSG_HIGH("Resetting Data , refresh Mode 0x%x, slot_id 0x%x",refresh_mode, slot_id,0);
  switch(refresh_mode)
  {
    case MMGSDI_REFRESH_RESET:
      gsdi_util_preinit_gsdi(GSDI_SIM_RESET,GSDI_SLOT_1, reselect_app);
      mmgsdi_util_reset_app_data(reselect_app);
      mmgsdi_data_slot1.mmgsdi_state = MMGSDI_STATE_CARD_INSERTED;
      break;
    default:
      break;
  }
}


/* ==========================================================================
FUNCTION MMGSDI_UTIL_SET_PIN_INFO

DESCRIPTION
  This function sets the pin info from the src pin info
DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void mmgsdi_util_set_pin_info(
  mmgsdi_pin_info_type           *dest,
  const mmgsdi_app_pin_info_type *src)
{
  if(dest == NULL || src == NULL)
    return;

  dest->num_retries = src->num_retries;
  dest->num_unblock_retries = src->num_unblock_retries;
  dest->pin_replacement = src->pin_replacement;
  dest->pin_id = src->pin_id;
  dest->status = src->status;
}/* mmgsdi_util_set_pin_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_STORE_PIN

DESCRIPTION
  This function, will store the pin for the PIN ID. It will set the
  valid_pin flag to FALSE. Once the operation succeeds the flag will
  be converted to TRUE.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
void mmgsdi_util_store_pin(
  mmgsdi_pin_enum_type           pin_id,
  mmgsdi_static_data_type        pin_data)
{
  mmgsdi_app_pin_info_type    *pin_info_ptr         = NULL;
  mmgsdi_int_app_info_type    *sel_app_ptr          = NULL;

  sel_app_ptr = mmgsdi_util_get_sel_app();

  if(sel_app_ptr == NULL )
  {
    MSG_ERROR("Cannot Store PIN",0,0,0);
    return;
  }

  if (pin_id == MMGSDI_PIN1){
    pin_info_ptr   = sel_app_ptr->pin1;
  }
  else if (pin_id == MMGSDI_PIN2){
    pin_info_ptr   = &sel_app_ptr->pin2;
  }
  else if (pin_id == MMGSDI_UNIVERSAL_PIN){
    pin_info_ptr   = sel_app_ptr->universal_pin;
  }
  else{
    MSG_ERROR("Incorrect PIN ID = %d",pin_id,0,0);
    return;
  }

  if(pin_info_ptr == NULL)
  {
    MSG_ERROR("Cannot Store PIN",0,0,0);
    return;
  }

  memcpy(pin_info_ptr->pin_data.data_ptr,
         pin_data.data_ptr,
         int32touint32(pin_data.data_len));


  pin_info_ptr->pin_data.data_len = pin_data.data_len;

  pin_info_ptr->valid_pin = FALSE;
}


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_AID_INDEX

   DESCRIPTION:
     This function returns an index location to the mmgsdi_slot_data_type
     if a matching AID is found

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uint32: index location of the AID
             MMGSDI_MAX_APP_INFO if not found

   SIDE EFFECTS:
     None
============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
uint32 mmgsdi_util_find_aid_index(
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_aid_type          aid
)
{
  uint32                 index         = 0;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;
  mmgsdi_len_type        templen       = 0;

  switch (slot_id)
  {
  case MMGSDI_SLOT_1:
    slot_data_ptr = &mmgsdi_data_slot1;
    break;
  case MMGSDI_SLOT_2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    slot_data_ptr = &mmgsdi_data_slot2;
    break;
#else
    MSG_ERROR("Invalid Slot MMGSDI_SLOT_2", slot_id, 0, 0);
    return MMGSDI_MAX_APP_INFO;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  case MMGSDI_SLOT_AUTOMATIC:
  case MMGSDI_MAX_SLOT_ID_ENUM:
  default:
    MSG_ERROR("Invalid Slot 0x%x", slot_id, 0, 0);
    return MMGSDI_MAX_APP_INFO;
  }

  if (slot_data_ptr->app_info_ptr == NULL)
  {
    MSG_ERROR("App Info Ptr Null", 0, 0, 0);
    return MMGSDI_MAX_APP_INFO;
  }

  for (index = 0; index < MMGSDI_MAX_APP_INFO; index++)
  {
    if (slot_data_ptr->app_info_ptr[index].app_data.aid.data_len == 0)
    {
      MSG_HIGH("Global app_info_ptr[0x%x] with 0 AID len", index, 0, 0);
      return MMGSDI_MAX_APP_INFO;
    }
    templen = MIN(slot_data_ptr->app_info_ptr[index].app_data.aid.data_len,
                  aid.aid.data_len);
    if(!memcmp(slot_data_ptr->app_info_ptr[index].app_data.aid.data_ptr,
              aid.aid.data_ptr, int32touint32(templen)))
    {
      /* AID equal */
      return index;
    }
  }
  MSG_ERROR("Unable to find AID from the table", 0, 0, 0);
  return index;
}/* mmgsdi_util_find_aid_index */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_IS_VALID_AID_INDEX

   DESCRIPTION:
     This function returns whether the AID index is valid or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     TRUE if valid index
     FALSE if invalid index

   SIDE EFFECTS:
     None
============================================================================*/
boolean mmgsdi_util_is_valid_aid_index(
  uint32 aid_index
)
{
  if (aid_index >= MMGSDI_MAX_APP_INFO)
  {
    MSG_ERROR("Invalid AID Index location 0x%x", aid_index, 0, 0);
    return FALSE;
  }
  return TRUE;
}


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_SELECT_FIRST_USIM_APP

   DESCRIPTION:
     This function Selects the first USIM app from the list of apps
     available

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
============================================================================*/
static void mmgsdi_util_select_first_usim_app(
  mmgsdi_slot_id_enum_type slot_id)
{
  uint32              index;

  if(slot_id != MMGSDI_SLOT_1)
  {
    MSG_ERROR("Cannot select App from any other Slot ",0,0,0);
    return;
  }

  /*Select the First App*/
  for(index=0;index<MMGSDI_MAX_APP_INFO;index++)
  {
    if(mmgsdi_data_slot1.app_info_ptr[index].app_data.app_type == MMGSDI_APP_USIM)
    {
      mmgsdi_data_slot1.sel_aid_index = index;
      mmgsdi_data_slot1.app_info_ptr[index].app_selected = TRUE;

      gsdi_data.aid_length  = int32touint8(
                  mmgsdi_data_slot1.app_info_ptr[index].app_data.aid.data_len);

      memcpy(gsdi_data.current_aid,
             (byte*)mmgsdi_data_slot1.app_info_ptr[index].app_data.aid.data_ptr,
             int32touint32(mmgsdi_data_slot1.app_info_ptr[index].app_data.aid.data_len));

      gsdi_data.aid_available = TRUE;

      return;
    }
  }
  MSG_ERROR("Cannot select App, no USIM App Available",0,0,0);
}


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_WAIT_FOR_CARD_SELECT

   DESCRIPTION:
     This function finds out if we need to wait for Card Selection from
     Clients

   DEPENDENCIES:
     NV

   LIMITATIONS:
     None

   RETURN VALUE:
     TRUE if MMGSDI needs to wait
     FALSE if MMGSDI does not need to wait

   SIDE EFFECTS:
     None
============================================================================*/
boolean mmgsdi_util_wait_for_card_select(void)
{
  mmgsdi_event_data_type  *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  boolean                  ret_value     = FALSE;

  if(gsdi_data.protocol != GSDI_PROTOCOL_UICC)
  {
    MSG_HIGH("Don't wait to select an app for icc protocol",0,0,0);
    return ret_value;
  }

  if(mmgsdi_util_get_usim_apps() > 1)
  {
    {
      nv_stat_enum_type  nv_status            = NV_DONE_S;
      nv_item_type       select_app;
#ifndef TEST_FRAMEWORK
      nv_status = gsdi_get_nv(NV_UIM_SELECT_DEFAULT_USIM_APP_I,
                             &select_app);
#endif /*!TEST_FRAMEWORK*/
#if defined (PLATFORM_LTK) || defined(TEST_FRAMEWORK)
      select_app.uim_select_default_usim_app = TRUE;
#endif/*PLATFORM_LTK || TEST_FRAMEWORK*/

    if ( nv_status == NV_DONE_S &&
           select_app.uim_select_default_usim_app == TRUE)
      {
       /*Do not need to Wait*/
        mmgsdi_util_select_first_usim_app(MMGSDI_SLOT_1);
        return FALSE;
      }
    }
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(
       evt_info_ptr,
       sizeof(mmgsdi_event_data_type),
       mmgsdi_status);

    if(mmgsdi_status == MMGSDI_SUCCESS &&
         mmgsdi_util_build_card_inserted_evt(MMGSDI_SLOT_1,evt_info_ptr) == MMGSDI_SUCCESS)
    {
      if( mmgsdi_util_notify_client_evt(evt_info_ptr) == MMGSDI_SUCCESS)
      {
        ret_value = TRUE;
      }
    }
    MMGSDIUTIL_TMC_MEM_FREE( evt_info_ptr);
  }

  if(ret_value == FALSE)
  {
    mmgsdi_util_select_first_usim_app(MMGSDI_SLOT_1);
  }
  return ret_value;
}


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SEL_AID

   DESCRIPTION:
     This function checks if the client ID is intended for the select AID.
     If client ID points to default channel, return TRUE
     If client ID points to a non default channel but the AID matches the
     default channel application, return TRUE

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
boolean mmgsdi_util_is_sel_aid(
  mmgsdi_client_id_type   client_id
)
{
  mmgsdi_int_app_info_type    *sel_app_ptr    = NULL;
  mmgsdi_static_data_type     *client_aid_ptr = NULL;

  MSG_HIGH("mmgsdi_util_is_sel_aid: client id 0x%x", client_id, 0, 0);

  if ((client_id & MMGSDI_SESSION_ID_MASK) == 0)
  {
    /* Default app because the client ID accessing default channel */
    MSG_HIGH("client id 0x%x is accessing default app", client_id, 0, 0);
    return TRUE;
  }
  sel_app_ptr = mmgsdi_util_get_sel_app();
  if (sel_app_ptr == NULL)
  {
    MSG_ERROR("Sel App Ptr = NULL, No application selected as default app yet", 0, 0, 0);
    return FALSE;
  }

  client_aid_ptr = mmgsdi_util_get_aid(client_id);

  if (client_aid_ptr == NULL)
  {
    MSG_ERROR("Client AID Ptr = NULL, can't map to selected AID", 0, 0, 0);
    return FALSE;
  }

  /* If equals to the selected App */
  if (!memcmp(client_aid_ptr->data_ptr, sel_app_ptr->app_data.aid.data_ptr,
       int32touint32(MIN(client_aid_ptr->data_len, sel_app_ptr->app_data.aid.data_len))))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* mmgsdi_util_is_sel_aid */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTI_PRESET_RETRIES

   DESCRIPTION:
     This function sets the pin retires to 3 and the unblock
     retries to 10

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_preset_retries(mmgsdi_slot_id_enum_type slot)
{
  mmgsdi_int_app_info_type *sel_app_ptr;
  sel_app_ptr = mmgsdi_util_get_sel_app();

  if(sel_app_ptr == NULL)
  {
    MSG_ERROR("No app selected yet",0,0,0);
    return;
  }

  if(sel_app_ptr->pin1 == NULL)
  {
    MSG_ERROR("Default APP , PIN1 is NULL",0,0,0);
    return;
  }

  if(slot >= MMGSDI_SLOT_2)
  {
    MSG_ERROR("Cannot handle slot 0x%x",slot,0,0);
    return;
  }

  sel_app_ptr->pin1->num_retries = 3;
  sel_app_ptr->pin1->num_unblock_retries  =10;

  sel_app_ptr->pin2.num_retries = 3;
  sel_app_ptr->pin2.num_unblock_retries = 10;
}/*mmgsdi_util_preset_retries*/


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_RESET_PIN_STATUS

   DESCRIPTION:
     This function resets the pin status

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_reset_pin_status(void)
{
  mmgsdi_int_app_info_type *sel_app_ptr;
  sel_app_ptr = mmgsdi_util_get_sel_app();

  if(sel_app_ptr == NULL)
  {
    return;
  }

  if(sel_app_ptr->pin1 == NULL)
  {
    return;
  }

  sel_app_ptr->pin1->num_retries = 0;
  sel_app_ptr->pin1->num_unblock_retries  =0;
  sel_app_ptr->pin1->status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;

  sel_app_ptr->pin2.num_retries = 0;
  sel_app_ptr->pin2.num_unblock_retries = 0;
  sel_app_ptr->pin2.status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
}/*mmgsdi_util_reset_pin_status*/


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SEL_APP_PIN_CACHED

   DESCRIPTION:
     This function checks to see if selected app's pin is cached

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_util_is_sel_app_pin_cached(void)
{
  mmgsdi_int_app_info_type *sel_app_ptr;
  sel_app_ptr = mmgsdi_util_get_sel_app();

  if(sel_app_ptr == NULL)
  {
    MSG_ERROR("No App Selected yet!",0,0,0);
    return FALSE;
  }

  /*PIN1 ,PIN2 are mandatory PINS*/
  if(sel_app_ptr->pin1!= NULL &&
     sel_app_ptr->pin1->cached == TRUE &&
     sel_app_ptr->pin2.cached  == TRUE )
  {
    /*Universal PIN may or may not be present, if present,
    then check if it is cached*/
    if( sel_app_ptr->universal_pin!= NULL &&
        sel_app_ptr->universal_pin->cached == FALSE)
    {
      MSG_HIGH("Universal PIN not Cached yet",0,0,0);
      return FALSE;
    }
    return TRUE;
  }

  MSG_HIGH("PINs not cached yet!",0,0,0);
  return FALSE;
}/*mmgsdi_util_is_sel_app_pin_cached*/


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_FILE_ATTR

   DESCRIPTION:
     This function checks the file attributes

   DEPENDENCIES:
     gsdi_ef_access_conditions_check

   LIMITATIONS:
     Checks are Limited to Read and write only, for all others, it will return
     SUCCESS

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     File Read/Write will be blocked if the security conditions have not
     been met.

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_check_file_attr(
  void                 * req_ptr,
  mmgsdi_cmd_enum_type   cmd_type
)
{
#ifdef FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES
  uim_items_enum_type            uim_file_item   = UIM_NO_SUCH_ITEM;
  mmgsdi_access_type            *access_ptr      = NULL;
  byte                           msg_id          = 0;
  mmgsdi_return_enum_type        mmgsdi_status   = MMGSDI_ERROR;

  switch(cmd_type)
  {
    case MMGSDI_READ_REQ:
      access_ptr = &(((mmgsdi_read_req_type *)req_ptr)->access);
      msg_id = GSDI_SIM_READ_REQ;
      break;

    case MMGSDI_WRITE_REQ:
      access_ptr = &(((mmgsdi_write_req_type *)req_ptr)->access);
      msg_id = GSDI_SIM_WRITE_REQ;
      break;

    default:
      return MMGSDI_SUCCESS;
  }

  if(access_ptr->access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    /* Convert to UIM items enum */
    mmgsdi_status = mmgsdi_util_determine_uim_items_enum(access_ptr,
                                                         &uim_file_item,
                                                         FALSE);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
       MSG_ERROR("Cannot convert to uim item enum",0,0,0);
       /* Return SUCCESS since the check for file attr did not fail at
          this point*/
       return MMGSDI_SUCCESS;
    }

    if(gsdi_ef_access_conditions_check(uim_file_item, msg_id) != GSDI_SUCCESS)
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD ACCESS_CONDITIONS",0,0,0);
      return MMGSDI_ERROR;
    }
#else
    (void) req_ptr;
    (void) cmd_type;
    MSG_HIGH("Access rule check for EF access is skipped", 0, 0, 0);
#endif /* FEATURE_MMGSDI_CACHE_FILE_ATTRIBUTES */

  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_util_check_file_attr*/


/*===========================================================================
   FUNCTION mmgsdi_util_is_pin_valid

   DESCRIPTION:
     This function will check the pin meets the minimum length requirement with
     valid data

   PARAMS:
    pin_ptr:          Will contain the code of the PIN.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
      boolean:  TRUE  PIN is valid
                FALSE PIN is not valid

   SIDE EFFECTS:

 ===========================================================================*/
boolean mmgsdi_util_is_pin_valid(
  const uint8 * pin_ptr)
{
  uint32 pin_index = 0;

  if (NULL == pin_ptr)
  {
     MSG_ERROR("PIN pointer is NULL", 0,0,0);
     return FALSE;
  }

  /* check that the pin is populated with valid digits for the minimum
     acceptable length */
  for (pin_index = 0; pin_index < MMGSDI_PIN_MIN_LEN; pin_index++) {
    if ((pin_ptr[pin_index] < 0x30 ||
         pin_ptr[pin_index] > 0x39))
    {
       MSG_ERROR("PIN is of invalid length: %d bytes", pin_index,0,0);
       return FALSE;
    }
  }
  return TRUE;
}


/*===========================================================================
   FUNCTION mmgsdi_util_get_file_path_from_enum

   DESCRIPTION:
     This function gets the file path for a given file enum value.
     This function is based on the uim function to get the path.

   PARAMS:
     file_enum:        UIM File Name
     path_ptr:         UIM File path
     defined_path_ptr: Original File path

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
 #ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_return_enum_type mmgsdi_util_get_file_path_from_enum(
  uim_items_enum_type file_enum,
  word                *path_ptr,
  const word          *defined_path_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  word*                    temp_path_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(path_ptr);

  /* Get the path from enum*/
  temp_path_ptr = (word *)uim_get_file_path(file_enum, defined_path_ptr);
  if(temp_path_ptr != NULL)
  {
    /* copy the temp path to path*/
    memcpy(path_ptr, temp_path_ptr, sizeof(word)* (UIM_MAX_PATH_ELEMENTS));
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  return mmgsdi_status;
}/* mmgsdi_util_get_file_path_from_enum */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
   FUNCTION mmgsdi_util_is_sel_app

   DESCRIPTION:
     This function compares the aid with the given aid and finds out if the
     given aid is the same as default app
   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
boolean mmgsdi_util_is_sel_app(mmgsdi_aid_type aid)
{
  mmgsdi_int_app_info_type  *sel_app_ptr   = NULL;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;

  sel_app_ptr = mmgsdi_util_get_sel_app();
  if(sel_app_ptr == NULL)
    return FALSE;

  /*----------------------------------------------------
     1. Figure out if it is prov app
  ------------------------------------------------------*/
  if (aid.aid.data_len != 0)
  {
    /* check if it is default app */
    mmgsdi_status = mmgsdi_util_cmp_aid(&sel_app_ptr->app_data,
                                        &aid);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /*It is not the default app*/
      return FALSE;
    }
  }
  return TRUE;
}


/*===========================================================================
   FUNCTION mmgsdi_util_get_session_ptr

   DESCRIPTION:
     This function returns the session ptr for the given client id
   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
 #ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
mmgsdi_session_info_type*  mmgsdi_util_get_session_ptr(
  int32                  client_index,
  mmgsdi_aid_type        aid
)
{
  mmgsdi_return_enum_type   mmgsdi_status;
  mmgsdi_session_info_type  *session_ptr  = NULL;
  int32                     i             = 0;

  if(mmgsdi_client_id_reg_table[client_index].client_id == 0)
  {
    /*Invalid Client id*/
    return NULL;
  }

  session_ptr = &(mmgsdi_client_id_reg_table[client_index].default_app);
  session_ptr->app_info_ptr = mmgsdi_util_get_sel_app();

  if(aid.aid.data_len == 0)
  {
    if(session_ptr->app_info_ptr != NULL)
    {
      return session_ptr;
    }
  }
  else if(session_ptr->app_info_ptr != NULL)
  {
    mmgsdi_status = mmgsdi_util_cmp_aid(&session_ptr->app_info_ptr->app_data,
                                         &aid);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      return session_ptr;
    }
  }

  for(i =0;i<mmgsdi_client_id_reg_table[client_index].num_session;i++)
  {
    session_ptr = &(mmgsdi_client_id_reg_table[client_index].session_info[i]);

    mmgsdi_status = mmgsdi_util_cmp_aid(&session_ptr->app_info_ptr->app_data,
                                         &aid);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      return session_ptr;
    }
  }
  return NULL;

}/* mmgsdi_util_get_session_ptr */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/


/*===========================================================================
   FUNCTION mmgsdi_util_print_file_information

   DESCRIPTION:
     This function print file information.
   PARAMS:
     file_access: File Information
     is_error_msg: boolean variable to print message as MSG_ERROR.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
void mmgsdi_util_print_file_information (
  mmgsdi_access_type file_access,
  boolean is_error_msg
)
{
  switch (file_access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      if(is_error_msg)
      {
        MSG_ERROR("For ef_enum: 0x%x",file_access.file.file_enum,0,0);
      }
      else
      {
        MSG_HIGH("For ef_enum: 0x%x",file_access.file.file_enum,0,0);
      }
      break;

    case MMGSDI_BY_PATH_ACCESS:
      if(is_error_msg)
      {
        uint32 mf_child_path;
        switch(file_access.file.path_type.path_len)
        {
          case 1:
           /* Printing 2 byte File ID information : MF ID*/
            MSG_ERROR("For File path 0x%x",
                       file_access.file.path_type.path_buf[0x00],0,0);
            break;

          case 2:
            /* Printing MF and MF Child (EF/DF) File ID information*/
            MSG_ERROR("For File path 0x%x%x",
                       file_access.file.path_type.path_buf[0x00],
                       file_access.file.path_type.path_buf[0x01],0);
            break;

          case 3:
            /* Printing MF, MF Child DF and Child DF/EF File ID information*/
            MSG_ERROR("For File path 0x%x%x%x",
                       file_access.file.path_type.path_buf[0x00],
                       file_access.file.path_type.path_buf[0x01],
            file_access.file.path_type.path_buf[0x02]);
            break;

          case 4:
            /* Printing MF, MF Child DF, DF CHILD DF and DF CHILD EF */
            /* Reading First and secound byte of the path buf
               and update the mf_child_path */
            mf_child_path = file_access.file.path_type.path_buf[0x00];
            mf_child_path <<=0x16;
            mf_child_path |= file_access.file.path_type.path_buf[0x01];
            MSG_ERROR("For File path 0x%x%x%x",
                       mf_child_path,
                       file_access.file.path_type.path_buf[0x02],
                       file_access.file.path_type.path_buf[0x03]);
            break;

          default:
            MSG_ERROR("Invalid Path length 0x%x",
                       file_access.file.path_type.path_len,0,0 );
            break;
        }/*end of switch switch(file_access.file.path_type.path_len)*/
      }/*end of if is_error_msg */
      else
      {
        uint32 mf_child_path;
        switch(file_access.file.path_type.path_len)
        {
          case 1:
          /* Printing 2 byte File ID information : MF ID*/
          MSG_HIGH("For File path 0x%x",
                    file_access.file.path_type.path_buf[0x00],0,0);
          break;

          case 2:
          /* Printing MF and MF Child (EF/DF) File ID information*/
            MSG_HIGH("For File path 0x%x%x",
                      file_access.file.path_type.path_buf[0x00],
                      file_access.file.path_type.path_buf[0x01],0);
            break;

          case 3:
          /* Printing MF, MF Child DF and Child DF/EF File ID information*/
            MSG_HIGH("For File path 0x%x%x%x",
                      file_access.file.path_type.path_buf[0x00],
                      file_access.file.path_type.path_buf[0x01],
                      file_access.file.path_type.path_buf[0x02]);
            break;

          case 4:
          /* Printing MF, MF Child DF, DF CHILD DF and DF CHILD EF */
          /* Reading First and secound byte of the path buf
             and update the mf_child_path */
            mf_child_path = file_access.file.path_type.path_buf[0x00];
            mf_child_path <<=0x16;
              mf_child_path |= file_access.file.path_type.path_buf[0x00];
            MSG_ERROR("For File path 0x%x%x%x",
                       mf_child_path,
                       file_access.file.path_type.path_buf[0x02],
                       file_access.file.path_type.path_buf[0x03]);
            break;

          default:
            MSG_ERROR("Invalid Path length 0x%x",
                       file_access.file.path_type.path_len,0,0 );
            break;
        }/*end of switch switch(file_access.file.path_type.path_len)*/
      }
      break;
    default:
      MSG_ERROR("Invalid File Access Method 0x%x ",
                 file_access.access_method,0,0);
      break;
  }/*end of switch (file_access.access_method)*/
  /* Do Nothing and return*/
}/*end of mmgsdi_util_print_file_information*/


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */


/* ==========================================================================
FUNCTION MMGSDI_CONVERT_PATH_TO_FILE_ENUM

DESCRIPTION
  This function, on providing the complete path, returns the mmgsdi file
  enum

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS
  MMGSDI_ERROR

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_convert_path_to_file_enum(
  mmgsdi_path_type       path_type,
  mmgsdi_file_enum_type *mmgsdi_file_enum_ptr)
{
  uim_items_enum_type     uim_file_enum = UIM_NO_SUCH_ITEM;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_file_enum_ptr);

  *mmgsdi_file_enum_ptr = MMGSDI_MAX_FILE_ENUM;

  uim_file_enum = (uim_items_enum_type)uim_get_enum_value_for_path(
                                         path_type.path_buf);

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /*FEATURE_MMGSDI_UMTS*/

  if (uim_file_enum != UIM_NO_SUCH_ITEM)
  {
    /* convert uim file enum to mmgsdi enum type */
    mmgsdi_status = mmgsdi_util_convert_from_uim_items_enum(uim_file_enum,
                    mmgsdi_file_enum_ptr);
  }
  else
  {
    mmgsdi_status = MMGSDI_ERROR;
  }/* if (uim_file_enum != UIM_NO_SUCH_ITEM) */

  return mmgsdi_status;
}/* mmgsdi_convert_path_to_file_enum */


/*===========================================================================
  FUNCTION mmgsdi_util_malloc_and_copy_file_path_array

  DESCRIPTION:
    This function shall allocate memory for the destination file path
    array and will copy file paths from source array

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    MMGSDI_SUCCESS                     : Successful file copy operation
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED : Malloc Error
    MMGSDI_INCORRECT_PARAMS            : If input parameters are out of range
    MMGSDI_ERROR                       : For all other failure scenarios


  SIDE EFFECTS:
    The calling function should free the allocated memory
 ----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_util_malloc_and_copy_file_path_array(
  mmgsdi_path_type           **dst_file_path_pptr,
  mmgsdi_path_type           *src_file_path_ptr,
  uint32                     num_files
)
{
  uint32                   i                   = 0;
  mmgsdi_return_enum_type  mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_path_type         *dst_file_path_ptr  = NULL;
  uint32                   path_len            = 0;

  if((src_file_path_ptr == NULL) || (num_files == 0) ||
      (dst_file_path_pptr == NULL))
  {
    MSG_ERROR(
      "Bad Param copy file path - src_file_p=0x%x num_files=0x%x dst_file_path_pptr=0x%x",
      src_file_path_ptr, num_files, dst_file_path_pptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate memory for file path data */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(dst_file_path_ptr,
    (num_files * sizeof(mmgsdi_path_type)));

  /* Perform a deep copy for the file path array */
  for(i = 0; i < num_files; i++)
  {
    if(src_file_path_ptr[i].path_len == 0)
    {
      MSG_ERROR("Refresh:Zero path_len at src_file_path_ptr[0x%x].path_len",
                 i,0,0);
      /* Free previous memory allocation */
      MMGSDIUTIL_TMC_MEM_FREE(dst_file_path_ptr);
      return MMGSDI_ERROR;
    }
    path_len = int32touint32(src_file_path_ptr[i].path_len);
    memcpy(dst_file_path_ptr[i].path_buf,
           src_file_path_ptr[i].path_buf,
           MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16));

    dst_file_path_ptr[i].path_len = src_file_path_ptr[i].path_len;
  }

  *dst_file_path_pptr = dst_file_path_ptr;

  return mmgsdi_status;
} /* mmgsdi_util_malloc_and_copy_file_path_array */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_REFRESH_FILE_PATHS

DESCRIPTION
  This function shall accept byte buffer that has list of file paths.
  Each file path begins from MF, hence byte value 0x 3F shall be used
  as a delimiter. The individual file paths shall be stored in the
  array of mmgsdi_path_type. The first byte shall contain the number
  of files
  For example
  Input buffer: 0x02 0x3F 0x00 0x2F 0x02 0x3F 0x00 0x7F 0x10 0x6F 0xD2
                 |
                First byte is number of files
  Should give output -
  *file_path_pptr[0].path_buf={0x3F 0x00 0x2F 0x02 0x00 0x00 0x00 0x00 0x00 0x00}
  *file_path_pptr[0].path_len = 2
  *file_path_pptr[1].path_buf={0x3F 0x00 0x7F 0x10 0x6F 0xD2 0x00 0x00 0x00 0x00}
  *file_path_pptr[1].path_len = 3
  num_files = 2

DEPENDENCIES
  NONE

LIMITATIONS
  The function shall only consider the number of files as mentioned in the
  first byte of the input data_ptr. If the actual buffer has more files than
  the number mentioned in the first byte, the logic in the function will
  ignore the rest of the files.


RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED - Malloc Error
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  Calling function should free memory allocated to file_path_pptr
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_util_convert_to_refresh_file_paths(
  uint32                  data_len,
  const uint8            *data_ptr,
  mmgsdi_path_type       **file_path_pptr,
  uint8                  *num_files_ptr
)
{
  uint8                   file_num                    = 0;
  byte                    index                       = 0;
  byte                    path_index                  = 0;
  mmgsdi_return_enum_type mmgsdi_status               = MMGSDI_ERROR;
  mmgsdi_path_type        *file_path_ptr               = NULL;
  word                    path[MMGSDI_MAX_PATH_LEN];


  /* Check for input params */
  MMGSDIUTIL_RETURN_IF_NULL(num_files_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(file_path_pptr);

  if(data_len == 0)
  {
    MSG_ERROR("mmgsdi_util_convert_to_refresh_file_paths: Malloc Error, num_files: 0x%x",
              *num_files_ptr,0,0);
    return MMGSDI_ERROR;
  }

  memset(path, 0x00,(sizeof(uint16)*(MMGSDI_MAX_PATH_LEN)));

  /* First byte contains number of files in the file list per spec
     102 221 section 8.18. This information comes from the card */

  *num_files_ptr = data_ptr[index++];
  data_len = data_len - 1;

  /* allocate memory to file_path_array */
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(file_path_ptr,
    (*num_files_ptr) * sizeof(mmgsdi_path_type), mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR("Refresh: Malloc Error status=0x%x", mmgsdi_status,0,0);
    return mmgsdi_status;
  }

  while (( index < data_len ) && (file_num < *num_files_ptr))
  {
    path_index = 0;

    /* Parse for each file path */
    do
    {
      /* If input file path for a file exceeds 5 file ID depth, treat it
         as invalid and return */
      if(path_index >= MMGSDI_MAX_PATH_LEN)
      {
        MSG_ERROR("mmgsdi_util_convert_to_refresh_file_paths: Invalid input file path",
                  0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
        return MMGSDI_ERROR;
      }

      path[path_index] = data_ptr[index++] << MMGSDIUTIL_BYTE_SHIFT;
      path[path_index] = path[path_index] | data_ptr[index++];
      MSG_HIGH("File path info - 0x%x", path[path_index],0,0);
      path_index++;
    }while ((index < data_len) && (data_ptr[index] != MMGSDIUTIL_MF_HIGH_BYTE));

    /* Path index was incremented by 1 in the last iteration of do-while loop,
       hence we have path_len = path index here */
    file_path_ptr[file_num].path_len = path_index;

    /* Copy path buffer */
    memcpy(file_path_ptr[file_num].path_buf, path, (path_index) * sizeof(uint16));
    file_num++;
  } /* while (index < data_len ) */

  /* Compare file_num and num_files. However this will not catch case
     where (actual files in buffer) > num_files_p, since while loop condition forces
     file_num < *num_file_p to avoid buffer overflow */
  if(*num_files_ptr != file_num)
  {
    MSG_ERROR("Num of refresh files mismatch *num_files_p=0x%x file_num=0x%x",
            *num_files_ptr,file_num,0);
  }
  *file_path_pptr = file_path_ptr;
  return mmgsdi_status;
} /* mmgsdi_util_convert_to_refresh_file_paths */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_ALLOC_COPY_UIM_RSP

   DESCRIPTION:
     This function will allocate memory and copy the UIM response info

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type  mmgsdi_util_alloc_copy_uim_rsp(
  mmgsdi_uim_report_rsp_type       **output_uim_rsp_pptr,
  const mmgsdi_uim_report_rsp_type  *input_uim_rsp_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(input_uim_rsp_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(output_uim_rsp_pptr);

  /* Memory allocation for output_uim_rsp_pptr and check for allocation */
  MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(*output_uim_rsp_pptr,
                                            sizeof(mmgsdi_uim_report_rsp_type));

  /* Copy the UIM response */
  memcpy((void*)*output_uim_rsp_pptr,
         (void*)input_uim_rsp_ptr,
         sizeof(mmgsdi_uim_report_rsp_type));

  (*output_uim_rsp_pptr)->rsp_data.data_ptr = NULL;
  (*output_uim_rsp_pptr)->rsp_data.data_len = 0;

  /* Copy UIM response data */
  if((input_uim_rsp_ptr->rsp_data.data_ptr != NULL) &&
     (input_uim_rsp_ptr->rsp_data.data_len > 0))
  {
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE((*output_uim_rsp_pptr)->rsp_data.data_ptr,
      input_uim_rsp_ptr->rsp_data.data_len, mmgsdi_status);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      (*output_uim_rsp_pptr)->rsp_data.data_len =
        input_uim_rsp_ptr->rsp_data.data_len;

      memcpy((void*)(*output_uim_rsp_pptr)->rsp_data.data_ptr,
             (void*)input_uim_rsp_ptr->rsp_data.data_ptr,
             int32touint32((*output_uim_rsp_pptr)->rsp_data.data_len));
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_alloc_copy_uim_rsp */
