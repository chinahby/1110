/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G S D I   G S M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions that provide GSM specific procedures
  for accessing a GSM compliant SIM.

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

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdigsm.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/09   kk      Reverted back extra check for fdn enabled for rehabilitation
06/10/09   kk      Fixed merge problem - compilation error
06/10/09   kk      Added service table check before sending down SELECT
05/26/09   kk      Extra check on fdn for rehabilitation of imsi/loci
04/29/09   js      Added prototype for static functions
01/21/09   kk      Added check if FDN is enabled for IMSI & LOCI Rehabilitation
10/06/08   jk      Allow GSM SIM Access on 1X
07/10/08   sp      Update the cache properly for read on record based files
06/20/08   sp      Add data offset messages for read
06/25/07   sun     Check for Buffer Overflow
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
02/21/07   sun     Remove GSDI Global PIN
01/17/07   sp      Removed pin_just_verified flag
10/31/06   tml     Allow ME to continue with initialization even if card has
                   contradicting BDN info
10/02/06   sp      Changed me supports fdn from gsdi to mmgsdi global data
07/26/06   sun     Added support for MMGSDI PIN and lint fixes
06/26/06   sp      Set gsdi_status to GSDI_SUCCESS always for get file attributes
                   on EF ADN
05/23/06   tml     Lint and BDN support
04/18/06   jar     Fixed Auth Support for ONCHIP SIM.
04/14/06   tml     Added auth support for onchip
03/08/06   jar     Merged in Branch Code for SIM Lock, Lint Fixes as well.
02/02/06   nk      Merged for ARR caching and by path changes
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Added check for gsdi_status before setting pin cached or not
10/18/05   tml     Added in_synch info to the cache
10/04/05   tml     BDN capability fix
06/10/05   tml     Added 256 MMGSDI suppor
05/11/05   sst     Lint fixes
01/28/05   jk      Added functionality to pass card status words back to application
                   for JAVA
11/15/04   jar     Code cleanup to use MACROS
10/18/04   sk      Added parameter checkings for search mode and search direction
                   for function gsdi_gsm_proc_sim_search.
08/09/04   jar     Memory overwrite fix when SST exceeds #define Lenghts.  Moved
                   HPLMN Selector to the Optional EFs for GSM Post PIN1 Init.
04/13/04   jar     Lint Fixes
04/07/04   jar     Don't just wipe out FDN for GSM Card if EF Phase is not
                   present and CDMA DF is present.
03/18/04   jar     Merged from branch:  Removed Caching of BCCH
03/12/04   jar     Added Independent SIM Initialization for Dual Slot Targets.
03/01/04   tml     Added rec num accessed support for write
02/04/04 jar/tml   Properly set & handled just_invalidated_2 & just_rehabilitated_2
02/04/04 jar/tml   Properly set phase 2 for dual slot.
01/25/04   tml     Fixed power up issue
01/23/03   jar     Enable / Disabled (_2) events need to be under FEATURE
                   FEATURE_MMGSDI_DUAL_SLOT
01/24/04   tml     Dual slot refresh support
12/18/03  jar/tml  Allowed Get Sim capability to go to global data immediately
                   if there is no need to synch up the global data.
12/04/03   tml     lint
10/01/03   tml     removed SKUI dependencies
09/07/03   jar     Removed ERR_FATALS & Dual Slot Code Cleanup.
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT
06/19/03   jar     Set the EF Category in global GSDI Data for
                   get_file_attribute requests.
05/19/03   tml     linted
05/01/03   jar     Added support for additional REFRESH FCN Modes
04/30/03   tml     add image supports
04/30/03   tml     Added caching for ACM and ACM Max EFs
04/11/03   jar     Added gsdi_gsm_is_icc_gsm_df_present.
02/24/03   jar     Added function gsdi_gsm_proc_refresh for a GSDI_REFRESH_REQ.
10/15/02   jar     Problem with gsdi_gsm_proc_get_sim_capabilities
                   prevented FDN settings from being properly set upon
                   power up - changed
                   if ( gsdi_data.sim_capability.imsi_invalidated ) to
                   if ( !gsdi_data.sim_capability.imsi_invalidated )
10/07/02   tml     Added check in pin status function such that is the
                   pin status requested is either GSDI_PIN1 ot GSDI_PIN2
                   and the values had already been stored in the gsdi_data
                   global data, the get pin status function call doesn't have
                   to go all the way down to uim driver anymore
09/24/02   tml     Added check for disable_chv1_allowed allowed in
                   gsdi_gsm_proc_get_sim_capabilities
09/12/02   tml     Added read for GSDI_GSM_KCGPRS and GSDI_GSM_LOCIGPRS
                   in gsdi_gsm_post_pin1_init
08/12/02   tml     Added num of retries left for pin related request
08/11/02   jar     Added SIM Service Table check for the gsdi_gsm_proc_get_
                   file_attributes function.  Should attempt select if SST doesn't
                   allow it.
07/24/02   tml     Added Select api
07/16/02   jar     Attempt to Cache PLMNWACT, OPLMNWACT, and HPLMNWACT if SST
                   check succeeds.  If not supported or not found, reset
                   gsdi_status to continue initialization procedures.
05/01/02   jar     Modifications were made to gsdi_gsm_proc_sim_increase to
                   not send the 2nd byte of req_p->increase_value as the Most
                   Signficant Byte to gsdi_uim_server_gsm_increase (merged code).
                   Added code to set sim phonebook status information
                   Removed FEATURE_GSDI_PHONEBOOK_SUPPORT.
04/04/02   jar     Modified gsdi_gsm_proc_get_pin_status to not execute code
                   when gsdi_data.init_completed bool is set to true.
02/27/02   jar     Added imsi_len, and an IMSI check in
                   gsdi_gsm_post_pin1_init to verify it is valid.  It shoudl be
                   provisioned, less than 15 digits and have at least 6 Digits.
02/10/02   jar     Modified gsdi_gsm_proc_get_pin_status to properly handle a
                   status request for PIN2.
02/03/02   jar     Added Phonebook Support Functionality which is featurized with
                   FEATURE_GSDI_PHONEBOOK_SUPPORT.
08/28/01   jc      Update a few diag MSGs.


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "rex.h"
#include "memheap.h"
#include "msg.h"
#include "task.h"
#include "dsm.h"
#include "err.h"
#include "string.h"

#if defined (FEATURE_MMGSDI_GSM) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif

#include "gsdi_exp.h"
#include "gsdi.h"

#ifdef FEATURE_UIM
#include "uim.h"
#endif

#include "mmgsdiutil.h"

#if defined (FEATURE_MMGSDI_GSM) || defined(FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_UIM_RUIM_W_GSM_ACCESS */


