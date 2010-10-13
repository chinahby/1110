/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G S D I   U S I M    F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions that provide 3G USIM specific procedures
  for accessing a 3G compliant USIM.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdiusim.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/09   kk      Fixed merge problem - compilation error
06/10/09   kk      Added service table check before sending down SELECT
05/15/09   js      Fixed compiler warning for non-UMTS build
04/29/09   js      Added prototype for local functions, fixed compiler warnings
02/26/09   mib     Path for sim read callback is wrong if internal mapping
07/10/08   sp      Update the cache properly for read on record based files
06/20/08   sp      Add data offset messages for read
03/26/08   kk      Buffer allocation for select response made dynamic
01/15/07   sk      Fixed featurization for 1000 phonebook entries
10/31/07   sk      Fixed typo with local phonebook
10/19/07   sk      Fixed compile error
10/18/07   sk      Fixed flag for multiple pb support
06/25/07   sp      Changed static variable to ptr to reduce stack overflow
05/31/07   sun     If the PIN is cached, then return the PIN Status also.
02/21/07   sun     Removed GSDI Global PIN
02/09/07   sk      Added support for 1000 phonebook entries.
02/01/07   tml     Checked for null
02/01/07   sun     Back out caching of pin
01/25/07   sun     Fixed PIN parsing and set the cached flag if PIN1 Status
                   an PIN2 status has been sent.
01/17/07   sp      Removed pin_just-verifed variable
01/11/07   sun     If the pin is perm blocked, then do not overwrite the event
                   blocked.
12/07/06   tml     Do not set pin1_just_verified to FALSE in case of
                   a change / enable / disable failure if the initial
                   condition was a success
10/24/06   tml     Remove updating pin1_verified in old GSDI.  Update will be
                   done in MMGSDI only
10/02/06   sp      Changed me supports fdn from gsdi to mmgsdi global data
09/05/06   pv      Return received status when
                   gsdi_util_decode_umts_file_header is called
08/07/06   tml     Fixed lobal phonebook mis-assignment issue
07/26/06   tml     Add support for > 255 PBR support back in
07/14/06   tml     Back out > 255 PBR support until pbm support is ready
07/05/06   jar     Fixes to allow for more than 4 ANRs to be used.
07/05/06   tml     support for > 255 PBR
05/16/06   wli     Fixed a possible memory leak.
03/23/06   jar     Fixed EF-PBR Init issue where multiple ANR Files could not
                   be supported.
03/21/06   tml     populate aid for virtual sim and lint
03/08/06   jar     Lint Fixes.
                   Merge from Branch for HZI Support
02/16/06   sp      Support for EF_EHPLMN
02/14/06   nk      Merged for: ARR caching and by path changes
12/29/05   tml     Fixed USIM AID search algo
11/17/05   tml     Reset gsdi_status to SUCCESS if path not in lookup table
                   for search
11/17/05   tml     fixed select and write issue when file is not found in the
                   path lookup table
11/14/05    pv     use new signature for functions that needs lookup
                   to return gsdi_returns_T and accept an output parameter.
                   also return an error for files in the lookup table but
                   if the path is not initialized.
11/09/05   sun     Fixed Lint Errors
10/18/05   tml     Added in_synch info to the cache
10/03/05   tml     BDN enabled/disabled check
08/03/05   tml     Fixed BDN check
07/15/05   sun     Added checks for FDN and BDN in GET_SIM_CAPABILITIES
07/12/05   tml     Populate  mmgsdi global data for USIMEF_to_select application ID
07/11/05   sun     Fixed Memory Leaks
06/10/05   tml     Added 256 MMGSDI support
05/11/05   sst     Lint fixes
03/08/05   jar     Fixed gsdi_umts_populate_fid_lookup_table to keep it from
                   indicting two Phonebooks are available when
                   gsdi_get_sim_capabilities is called more than once for a USIM.
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
01/05/05   jar     Modified to support setting the File Mapping in the USIM
                   Lookup table.
12/27/04   jk      Fixed Compiler warnings.
10/08/04   tml     Added CBMID caching
09/29/04   sk      Modifications made to all pin functions to facilitate adding
                   application specific pin information to the newly created
                   application pin information table.
09/28/04   tml     Fixed pbr parsing issue
08/09/04   jar     Fixed memory overwrite issue by switching UST and EST from
                   Fixed Buffers to Dynamically allocated buffers of
                   GSDI_MAX_DATA_BLOCK_LEN.
08/02/04   jar     Modifications made to avoid getting stuck in FDN, BDN, ACL
                   when EF-EST is not present and the bits are on in EF-UST.
07/27/04   tml     Fixed FDN enable/disable slot issue
07/20/04   tml     Fixed PIN status for disabled PIN during power up.
07/13/04   tml     Fixed PIN enable/disable status issue
07/12/04   tml     Populate number of retries for both PIN and PUK during the
                   initial get pin status request
06/16/04   tml     Fixed pin block status issue
04/20/04   tml     Default read request to SLOT1 during get usim capability
04/13/04   jar     Lint Fixes:  gsdi_umts_proc_usim_read(),
                   gsdi_umts_proc_usim_write(),gsdi_umts_proc_usim_search()
04/11/04   jar     Make a Verify Call with NULL PINs and Unblock Call with
                   NULL PINs to update number of retries left.
03/18/04   tml     Merge from branch
                   Removed CBMID from Post PIN1 Init Cache Procedures.
03/12/04   jar     Added Single Slot Initialization Support for Dual Slot
                   Targets.
03/01/04   tml     Added rec num accessed support for write
01/24/04   tml     Fixed compilation warnings
12/18/03  jar/tml  Allowed Get Sim capability to go to global data immediately
                   if there is no need to synch up the global data.
12/12/03   jar     MEMSET the data_p to 0x00 in the USIM Authentication to ensure
                   there are not any unitialized values.
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT
06/27/03   jar     Modified gsdi_util_decode_umts_file_header() to allow for
                   EF_to_select and data_p to be passed in as parameters.
06/19/03   jar     Set the EF Category in global GSDI Data for
                   get_file_attribute requests.
                   Fixed ACM Record Number Problem
05/20/03   tml     linted
04/30/03   tml     Added image support
04/30/03   tml     Added caching for ACM anc ACM Max EFs
04/15/03   tml     Evaluated ERR usage
04/10/03   jar     Added function gsdi_umts_is_uicc_usim_adf_present
01/17/03   tml     Set Data Len in EST write for Enable/Disable FDN
12/04/02   jar     Check USIM Capabilities before reading other EFs.
10/16/02   tml     Fixed the pin_status return value when pin is blocked or
                   permanently blocked in gsdi_umts_proc_get_pin_status
10/16/02   jar     Fixed problem with not copying EST Table information and
                   changing EST Bit before setting it in gsdi_disable_fdn
                   and gsdi_enable_fdn.
10/12/02   jar     Added boolean est_allocated to be used to determine what
                   services are available.  If EST is not present, don't
                   hang the mobile, use UST only to determine services.
                   Reset gsdi_status if EF-EST is not present.
10/07/02   tml     Added check for whether GSDI_PIN1 and GSDI_PIN2 pin status
                   have been cached or not.  When these 2 pin id have been cached
                   the get pin status function returns the information stored
                   in the gsdi_data instead of going all the way to uim driver
09/12/02   tml     Clean up for gsdi_umts_populate_fid_lookup_table function
09/12/02   tml     Fixed gsdi_umts_populate_fid_lookup_table function such that
                   the parsing will stop based on TLV length
08/31/02   jar     Modified gsdi_umts_post_pin1_init to read UIM_USIM_RPLMNLACT
                   regardless of whether UIM_USIM_NETPAR is present.  USIM spec
                   version will still get set accordingly.
08/12/02   tml     Added num or retries for pin related api
08/11/02   jar     Added USIM Service Table check for the gsdi_umts_proc_get_
                   file_attributes function.  Should attempt select if UST doesn't
                   allow it.
07/24/02   tml     Added Select api
07/16/02   jar     Attempt to cache UIM_USIM_KC as part of post PIN1 Procedures.
07/13/02   jar     Removed Telecom PBR Workaround.
                   Modified gsdi_umts_post_pin1_init() to handle changes in
                   the gsdi_util_is_file_in_ust_ok() function.
06/12/02   jar     Reset all accesses that fail for Optional EFs in gsdi_umts_
                   post_pin1_init.
06/10/02   jar     Modified method used to reset status for CBMID.
05/30/02   jar     Reset gsdi_status if Read on Optional EF CBMID fails.
05/13/02   jar     Wrapped Local Phonebook request for UIM_USIM_PBR around
                   the USIM Service Table Result.  Modified loop control
                   of gsdi_umts_populate_fid_lookup_table to stop decoding
                   when a tag of 0x00 is reached.
05/01/02   jar     Merged USIM Increase fix: Modifications were made to
                   gsdi_umts_proc_usim_increase to not send the 2nd byte of
                   req_p->increase_value as the Most Signficant Byte to
                   gsdi_uim_server_usim_increase.
04/04/02   jar     Modified gsdi_umts_proc_get_pin_status to not execute code
                   when gsdi_data.init_completed bool is set to true.
03/23/02   jar     Modified gsdi_umts_post_pin1_init to not fail init if the
                   optional EFs: HPLMNWACT,PLMNWACT, OPLMNWACT are NOT on
                   the USIM.
02/27/02   jar     Added imsi_len, and an IMSI check in
                   gsdi_umts_post_pin1_init to verify it is valid.  It shoudl be
                   provisioned, less than 15 digits and have at least 6 Digits.
02/08/02   jar     Added a check to not read TELECOM/USIM EF-PBR after init
                   has completed.  Added check to insure initialization is not
                   halted by a FAILED Attempt to read EF-PBR USIM or TELECOM.
                   Added a select back to MF before a TELECOM Read on EF-PBR.
02/03/02   jar     Added Phonebook Support Functionality which is featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.
12/03/01   jar     Modified get_pin_status to only check PIN_APPL_1 (Key Ref 1)
                   and SEC_PIN_APPL_1 (Key_ref 81) per Release 99 Specifications.
                   Ignore all other PINS until Release 2000 Implementation.
09/13/01   jc      Add length param to usim authenticate proc.
08/29/01   jc      Update diag F3 MSGs.


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Common Interface Files */
#include "customer.h"
#include "rex.h"
#include "memheap.h"
#include "string.h"


#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif



#include "msg.h"
#include "task.h"
#include "dsm.h"
#include "err.h"
#include "gsdi_exp.h"
#include "gsdi.h"
#include "uim.h"
#include "mmgsdiutil.h"

#include "intconv.h"
/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef  FEATURE_USIM_1000_PBM

#define GSDI_PB_FILE_TYPE1               0x01
#define GSDI_PB_FILE_TYPE2               0x02
#define GSDI_PB_FILE_TYPE3               0x03

#define GSDI_ATLEAST_TWO_CNT_ADN_IN_PBR  0x02
#define GSDI_MAX_ADN_CNT_SUPPORTED       0x04

#else /* FEATURE_USIM_1000_PBM */

#define GSDI_USE_ANR        0x00
#define GSDI_USE_ANR_A      0x01
#define GSDI_USE_ANR_B      0x02
#define GSDI_USE_ANR_C      0x03

#endif /* FEATURE_USIM_1000_PBM */

#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
