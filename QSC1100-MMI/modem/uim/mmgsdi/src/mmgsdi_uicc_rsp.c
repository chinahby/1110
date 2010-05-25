/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I C C   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uicc_rsp.c#5 $$ $DateTime: 2009/05/15 02:20:16 $ $Author: c_pkamuj $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
02/12/09   sun     Fixed the index issue with read_arr for read_by_path files
02/10/09   tml     Fixed security access condition population issue with
                   expand or reference expand format
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/03/08   tml     Returned error if the get all pin status command processing
                   was erroneous and lint fixes
10/20/08   sun     If WRONG_CLASS is received at APP Activation, then do  a 
                   card reset and change protocol to GSM.
10/01/08   ssr     Fixed for UICC Search where search is performed successfully
                   with no response data.
09/25/08   js      Fixed Klockwork errors
09/22/08   js      Fixed Lint Errors
09/08/08   sun     Fixed Compilation
09/04/08   sp      Added Null pointer check
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Compile Fixes related to PKCS15
08/20/08   kk      Calls in create_pkcs15_table made to async
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. Max path length in MMGSDI is set to 5.
                   Renamed MMGSDI_MAX_PATH_SIZE to MMGSDI_NUM_ARR_FILES
07/30/08   SSR     Update MMGSDI Status for seek on 0x6282 card warning.
07/20/08   sp      Update the cache properly for read on record based files
07/16/08   jk      Fixed Compile Warnings
06/12/08   ssr     Support of MMGSDI Search functionality
06/04/08   tml     Added sw support to cnf and message cleanup
05/12/08   tml     Fixed null pointer access when select request is BY_PATH
03/24/08   sun     Added support for OMA Bcast
03/13/08   tml     fixed memory leak
01/10/07   nk      Fixed link error
11/30/07   sun     Added support for NAA refresh
11/19/07   tml     Fixed memory leak
09/19/07   sun     Fixed PIN Header Decoding
09/10/07   tml     Add MFLO support
08/14/07   sun     Handled new error codes
06/28/07   sun     Use Extra Param ptr instead of uim to get the read file info
06/18/07   sun     Added Checks for UIM_CMD_PTR - it should never be NULL
05/11/07   sun     If the PIN is already changed from not init, then do not
                   change the status, this maybe a new select on an already
                   selected application
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/26/07   sp      Added support for ARR files inthe second DF level.
04/07/07   sp      Added parsing of the life cycle byte
04/04/07   sp      Send status command before terminating an app.
04/04/07   sun     Reflect the changes for Replacement in UNIV PIN pointer
                   as well
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
03/02/07   sun     Update App PIN with correct PS_DO information relevant to
                   that app only
02/21/07   sun     Removed Global PIN from GSDI
01/31/07   sp      Removed Access_denied from mmgsdi_uicc_rsp_parse_pin_num
01/31/07   tml     Fixed ARR path issues
01/17/07   tml     Reset the mmgsdi status after trying to find the path
                   pointer
01/16/07   sk      Added check in mmgsdi_uicc_rsp_decode_ref_exp_sec_attr()
                   to see if file is init in lookup table
01/12/07   sun     Determine the SEID and then look for the Record Number
                   for the ARR File while parsing security attributes
01/11/07   sun     Handle Code Blocked and CODE_PERM_BLOCKED separately.
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
11/21/06   sun     Fixed Parsing of the PIN Header data.
11/14/06   tml     Allow decoding PIN header to be done on non USIM App
11/12/06   sun     Increment offset within the while loop
10/27/06   tml     Allow decoding PIN header to be done on non USIM App
10/25/06   tml     Added initial support for Universal PIN replacement support
10/25/06   tml     Update cache only if default app is being modified.
10/14/06   jar     Set the report status to MMGSDI_SUCCESS when found to
                   avoid an incorrect status to the client.
                   Fixed Abort Issue seen in Univeral PIN Operations
10/13/06   tml     Update aid length in gsdi_data global structure with the
                   current_aid member
09/07/06   sun     CHANGED ACCESS_DENIED error to blocked/perm blocked status
                   For get_all_pin_status,reset status for errors other than
                   BLOCKED/PERM_BLOCKED
09/06/06   tml     Fixed Expand Security Parsing issue
08/20/06   tml     Fix select AID issue
08/18/06   sun     For Unblock PIN request, update the num_unblock_retries.
08/14/06   sun     Set the cache flag if universal pin status fails
08/07/06   sun     Added PIN and valid_pin to app_pin_info_type
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/08/06   tml     Fixed featurization
03/08/06   sun     Do not parse the UIM Response data for Streaming APDU
02/15/06   tml     Lint fix and add data fr card boolean to the write cache
02/14/06   nk      Merged for: ARR caching and by path changes
12/21/05   sun     Return Proper error ISIM Authentication failure
12/14/05   tml     MMGSDI Cleanup
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header and compilation issue
11/03/05   tml     Fixed status word 0x63 for write and increase
09/21/05   tml     Fixed status word for pin related status word
09/15/05   tml     Fixed status word issue for 0x68 and 0x63
08/30/05   jar     Added support to handle a Status Command Response from the
                   Card
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT suppoort
07/14/05   tml     Added original request type
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "intconv.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"
#include "mmgsdi_refresh.h"
#include "uim.h"
#include "gsdi_exp.h"
#include "gsdi.h"
#include "rex.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_session.h"
#include "gsdi_convert.h"
#include "mmgsdi_gen.h"

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

