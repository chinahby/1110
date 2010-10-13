/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    G S D I  P E R S O N A L I Z A T I O N

                                      A N D

                  D E P E S O N A L I Z A T I O N  E N G I N E


GENERAL DESCRIPTION

  This source file contains the GSDI PERSONALIZATION and DEPERSONALIZATION
  Engines required to support the 02.22 and 32.222 Specifications to support:
  Network Lock
  Network Subsystem Lock
  Service Provider Lock
  Corporate Lock
  IMSI Lock

  This term SIM Lock is occassional used interchangeable with personalization.

  This source file will contain the following functionality:
  1. Generation of and Broadcasting of Personalization Events to all registered
     clients.
  2. Personalization Logic

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso.c#2 $$ $DateTime: 2009/01/16 06:47:27 $ $Author: nbandwar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/09   nb      Changed gsdi_fs_name_test() to gsdi_efs_name_test() and 
                   gsdi_fs_mkdir() to gsdi_efs_mkdir()
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
01/15/07   nk      Updated EFS Path depending if apps mount feature defined
10/06/06   sk      Added support to perform gsdi perso refresh on Refresh 
                   SIM INIT Req w/wo Full File Change Notification/File Change
                   Notification
05/15/06   sun     Removed Redundant Check
04/18/06   sun     Fixed featurization
11/09/05   sun     Fixed lint errors
11/03/05   tml     Fixed compilation warning
07/26/05   sst     Fixed lint errors.
07/12/05   sun     Fixed Perso File Compatibility issues with Perso-engine Files
07/11/05   sun     Fixed Memory Leaks
06/17/05   sun     Send GSDI_PERSO_INIT_COMPLETED on Perso Complete
05/20/05   jk      Properly handle IMSI EF read failure
05/11/05   sst     Lint fixes
04/28/05   jar     Added New GSDI SIM Lock Architecutre to support Plug In
                   Proprietary Security Libraries driven by a Perso Engine.
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/18/04   sun     Send error to Client on GSDI_PERSO_ERROR_STATE
12/03/04   jar     Fix to keep from reading EF-DCK when not necessary.
11/15/04   jar     Code cleanup to use MACROS
08/24/04   jar     Fixed Self Personalization Data to keep consistent with
                   31.102 or 11.11 for EF-CNL.
08/11/04   tml     Added support for get unblock key
08/02/04   jar     Fixes for REFRESH, EF-DCK usage and EF-SMS PP OTA DEPERSO
07/21/04   jar     Modified unblock attempts remaining filename to make it
                   shorter.
07/17/04   jar     Added Depersonalization REFRESH Procedures.
                   Added Subset Checking for Personalization procedures.
07/02/04   tml     Fixed GSM number of MCC digit issue
06/28/04 jar/tml   Fix to move get sim capabilities into the
                   GSDI_PERSO_NOT_INIT_STATE. Fixed initial key size during
                   file generation
06/21/04   jar     Fixed Compile Error feature -> perso_feature.
06/20/04 jar/tml   Added Self Personalization and a ciphering interface
                   between MMGSDI and FS storage.
06/09/04   jar     Replaced Test Keys with the Autogeneration of Control Keys
                   Added remaining Personalization checks.
05/18/04   jar     Initial Revision

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
#ifdef FEATURE_PERSO_SIM
#include "intconv.h"
#include <stdlib.h>            /* rand(), RAND_MAX */
#include "gsdi_perso.h"
#include "gsdi_exp.h"
#include "gsdi_efs.h"
#include "gsdi_nv.h"
#include "gsdi.h"
#include "string.h"
#include "msg.h"
#include "err.h"
#include "tmc.h"

#if defined (FEATURE_MMGSDI_GSM ) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATuRE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                        GLOBAL VARIABLES

===========================================================================*/
int32 num_ck_attempts_remaining = 0;
/* ----------------------------------------------------------------------------
   GSDI Personalization File Structure existing in FS
   --------------------------------------------------------------------------*/
/* Check if Features are defined at the build level, if so then mmgsdi folder
   needs to be created under the apps directory */
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
static char *mmgsdi_efs_mmgsdi_dir  = "./apps/mmgsdi";
static char *mmgsdi_efs_perso_dir   = "./apps/mmgsdi/perso";
static char * array_of_ck_files[5]  = {"/apps/mmgsdi/perso/nw_ck.txt",
                                       "/apps/mmgsdi/perso/ns_ck.txt",
                                       "/apps/mmgsdi/perso/sp_ck.txt",
                                       "/apps/mmgsdi/perso/cp_ck.txt",
                                       "/apps/mmgsdi/perso/sim_ck.txt"};
static char * array_of_unblock_ck_files[5]  = {"/apps/mmgsdi/perso/nw_unblock_ck.txt",
                                               "/apps/mmgsdi/perso/ns_unblock_ck.txt",
                                               "/apps/mmgsdi/perso/sp_unblock_ck.txt",
                                               "/apps/mmgsdi/perso/cp_unblock_ck.txt",
                                               "/apps/mmgsdi/perso/sim_unblock_ck.txt"};
static char * array_of_code_files[5] = {"/apps/mmgsdi/perso/nw_codes.txt",
                                        "/apps/mmgsdi/perso/ns_codes.txt",
                                        "/apps/mmgsdi/perso/sp_codes.txt",
                                        "/apps/mmgsdi/perso/cp_codes.txt",
                                        "/apps/mmgsdi/perso/sim_codes.txt"};
static char *mmgsdi_efs_retries_rem = "/apps/mmgsdi/perso/num_retries_remaining.txt";
static char *mmgsdi_efs_unblock_retries_rem = "/apps/mmgsdi/perso/unblk_retry_rem.txt";
static char *mmgsdi_efs_feature_ind = "/apps/mmgsdi/perso/feature_ind.txt";
static char *mmgsdi_efs_perm_dis_ind= "/apps/mmgsdi/perso/perm_dis_feature_ind.txt";
static char *mmgsdi_efs_master_ck   = "/apps/mmgsdi/perso/master_ck.txt";
#else
static char *mmgsdi_efs_mmgsdi_dir  = "mmgsdi";
static char *mmgsdi_efs_perso_dir   = "mmgsdi/perso";
static char * array_of_ck_files[5]  = {"mmgsdi/perso/nw_ck.txt",
                                       "mmgsdi/perso/ns_ck.txt",
                                       "mmgsdi/perso/sp_ck.txt",
                                       "mmgsdi/perso/cp_ck.txt",
                                       "mmgsdi/perso/sim_ck.txt"};
static char * array_of_unblock_ck_files[5]  = {"mmgsdi/perso/nw_unblock_ck.txt",
                                               "mmgsdi/perso/ns_unblock_ck.txt",
                                               "mmgsdi/perso/sp_unblock_ck.txt",
                                               "mmgsdi/perso/cp_unblock_ck.txt",
                                               "mmgsdi/perso/sim_unblock_ck.txt"};
static char * array_of_code_files[5] = {"mmgsdi/perso/nw_codes.txt",
                                        "mmgsdi/perso/ns_codes.txt",
                                        "mmgsdi/perso/sp_codes.txt",
                                        "mmgsdi/perso/cp_codes.txt",
                                        "mmgsdi/perso/sim_codes.txt"};
static char *mmgsdi_efs_retries_rem = "mmgsdi/perso/num_retries_remaining.txt";
static char *mmgsdi_efs_unblock_retries_rem = "mmgsdi/perso/unblk_retry_rem.txt";
static char *mmgsdi_efs_feature_ind = "mmgsdi/perso/feature_ind.txt";
static char *mmgsdi_efs_perm_dis_ind= "mmgsdi/perso/perm_dis_feature_ind.txt";
static char *mmgsdi_efs_master_ck   = "mmgsdi/perso/master_ck.txt";
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
/* ---------------------------------------------------------------------------
   IMSI Buffer used to store the formatted IMSI
   -------------------------------------------------------------------------*/
static uint8 imsi_buffer[8];
/* ---------------------------------------------------------------------------
   Test IMSI Checked against the IMSI to allow for Personalization operation
   with Features activated
   --------------------------------------------------------------------------*/
static uint8 test_imsi_buffer[3]    = {0x00,0xF1,0x10};
/* ----------------------------------------------------------------------------
   Default Data Array Checked against
   --------------------------------------------------------------------------*/
static uint8 mmgsdi_ck_not_set[GSDI_PERSO_DCK_CK_LEN]= {0xFF,0xFF,0xFF,0xFF};
/* ----------------------------------------------------------------------------
   Table to hold the list of event handlers
   --------------------------------------------------------------------------*/
gsdi_perso_events_cb gsdi_perso_event_handler_table[GSDI_PERSO_MAX_REG_CLIENTS];
/* ----------------------------------------------------------------------------
   Buffer to store encrypted Data
   --------------------------------------------------------------------------*/
static uint8 gsdi_encrypted_data_buffer[GSDI_MAX_DATA_BLOCK_LEN *2];
/* ---------------------------------------------------------------------------
   Buffer to store Encryption Key
   -------------------------------------------------------------------------*/
static uint8 gsdi_encryption_key_buffer[GSDI_MAX_DATA_BLOCK_LEN *2];

/*===========================================================================
FUNCTION GSDI_PERSO_IS_KEY_DEFAULT_KEY


DESCRIPTION
  This function will be used to determine if the Key provided is the default
  key.

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE  - Key is the default CK of 0xFF ... 0xFF
            FALSE - Keys is not the default CK
===========================================================================*/
boolean gsdi_perso_is_key_default_ck(
  gsdi_perso_enum_type feature,
  int32                data_len,
  uint8 *              data_p
)
{
  int8   start_index = 0;

  /* --------------------------------------------------------------------------
     Make sure the CK Len doesn't exceed the maximum number of Control Key
     bytes allowed
     ------------------------------------------------------------------------*/
  if ( (data_len > (GSDI_PERSO_DCK_NUM_CKS * 4)) ||
        data_len <= 0 )
  {
    /* Return False so that the error can be caught be the verify attempt */
    return FALSE;
  }

  /* --------------------------------------------------------------------------
     Calculate the Start Index.  Because EF-DCK is a transparent file,
     we must move the index for the feature checked
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    start_index = GSDI_PERSO_DCK_NW_CK_OFFSET;
    break;

  case GSDI_PERSO_NS:
    start_index = GSDI_PERSO_DCK_NS_CK_OFFSET;
    break;

  case GSDI_PERSO_SP:
    start_index = GSDI_PERSO_DCK_SP_CK_OFFSET;
    break;

  case GSDI_PERSO_CP:
    start_index = GSDI_PERSO_DCK_CP_CK_OFFSET;
    break;

  default:
    MMGSDI_DEBUG_ERR("BAD FEATURE ID: %x",feature,0,0);
    return FALSE;
  }

  if (!(memcmp(&data_p[start_index],
               mmgsdi_ck_not_set,
               GSDI_PERSO_DCK_CK_LEN)))
  {
    /* DCK is the default CK of 0xFF ... 0xFF */
    return TRUE;
  }

  /* Default CK of 0xFF ... 0xFF was not found */
  return FALSE;
}

/*===========================================================================
FUNCTION GSDI_PERSO_CLEAR_CK_FROM_EF_DCK


DESCRIPTION
  This function will be used to clear the Control Key from EF-DCK.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void gsdi_perso_clear_ck_from_ef_dck(
  gsdi_slot_id_type  slot,
  int32 *            returned_data_len_p
)
{
  gsdi_sim_write_req_T * write_req_p     = NULL;
  gsdi_returns_T        gsdi_status     = GSDI_ERROR;
  uint8                 card_apps       = 0x00;

  /* --------------------------------------------------------------------------
     Verify the slot provided
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
     && slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    return;
  }

  /* --------------------------------------------------------------------------
     Allocate Memory for the Write Request and set the parameters to prepare it
     for the read and verify loop.
     ------------------------------------------------------------------------*/
  write_req_p = mmgsdi_malloc( sizeof(gsdi_sim_write_req_T));
  if ( write_req_p == NULL )
  {
    ERR_FATAL("Could not allocate memory for the write Request",0,0,0);
  }
  memset(write_req_p,0xFF,sizeof(gsdi_sim_write_req_T));
  /* --------------------------------------------------------------------------
     Build the read request based on the information provided in the
     call.  We'll let the SIM handle any requests with record numbers out of
     bounds with its structure.
     ------------------------------------------------------------------------*/
  write_req_p->message_header.slot = slot;
  write_req_p->index_mode          = (byte)GSDI_NO_INDEX;
  write_req_p->rec_num             = 0;
  write_req_p->search_string_len   = 0;
  write_req_p->data_len            = (byte)GSDI_PERSO_DCK_CK_LEN * 4;
  write_req_p->data_offset         = 0;

  /* --------------------------------------------------------------------------
     Determine which file should be read and read it
     ------------------------------------------------------------------------*/
  card_apps = gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK;

  if ( card_apps )
  {
    write_req_p->sim_filename = (word)UIM_USIM_DCK;
    gsdi_status = gsdi_umts_proc_usim_write(write_req_p,
                                            (byte *)returned_data_len_p);
  }
  else
  {
    write_req_p->sim_filename = (word)UIM_GSM_DCK;
    gsdi_status = gsdi_gsm_proc_sim_write(write_req_p,
                                          (byte *)returned_data_len_p);
  }
  if ( gsdi_status != GSDI_SUCCESS )
    MMGSDI_DEBUG_MSG_ERROR("Could not clear EF-DCK ",0,0,0);

  MMGSDIUTIL_TMC_MEM_FREE(write_req_p);
}


/*===========================================================================
FUNCTION GSDI_PERSO_READ_EF_DCK_RECORD


DESCRIPTION
  This function will be used to read the EF-DCK File and return the data
  retreved from the Card.

  This function will perform the mmgsdi_malloc for the read request and
  free it up prior to exiting this function.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_read_ef_dck_record(
  gsdi_slot_id_type slot,
  int32 *           returned_data_len_p,
  uint8 *           data_p
)
{
  gsdi_sim_read_req_T * read_req_p      = NULL;
  gsdi_returns_T        gsdi_status     = GSDI_ERROR;
  uint8                 card_apps       = 0x00;
  byte                  dummy           = 0;

  /* --------------------------------------------------------------------------
     Verify the slot provided
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
     && slot != GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
     )
  {
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate Memory for the Read Request and set the parameters to prepare it
     for the read and verify loop.
     ------------------------------------------------------------------------*/
  read_req_p = mmgsdi_malloc( sizeof(gsdi_sim_read_req_T));
  if ( read_req_p == NULL )
  {
    ERR_FATAL("Could not allocate memory for the read Request",0,0,0);
  }
  memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));

  /* --------------------------------------------------------------------------
     Build the read request based on the information provided in the
     call.  We'll let the SIM handle any requests with record numbers out of
     bounds with its structure.
     ------------------------------------------------------------------------*/
  read_req_p->message_header.slot  = slot;
  read_req_p->index_mode           = GSDI_NO_INDEX;
  read_req_p->rec_num              = 0;
  read_req_p->search_string_len    = 0;
  read_req_p->required_data_len    = (byte) GSDI_PERSO_DCK_CK_LEN * 4;
  read_req_p->data_offset          = 0;

  /* --------------------------------------------------------------------------
     Determine which file should be read and read it
     ------------------------------------------------------------------------*/
  card_apps = gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK;

  if ( card_apps )
  {
    read_req_p->sim_filename = (word)UIM_USIM_DCK;
    gsdi_status = gsdi_umts_proc_usim_read(read_req_p,
                                           &dummy,
                                           &dummy,
                                           (byte *)returned_data_len_p,
                                           (byte *)data_p);
  }
  else
  {
    read_req_p->sim_filename = (word)UIM_GSM_DCK;
    gsdi_status = gsdi_gsm_proc_sim_read(read_req_p,
                                         &dummy,
                                         &dummy,
                                         (byte *)returned_data_len_p,
                                         (byte *)data_p);
  }
  /* --------------------------------------------------------------------------
    Check the record len to ensure it is valid.  It must return the correct
    number of bytes to be useful.
    -------------------------------------------------------------------------*/
  if ( gsdi_status == GSDI_SUCCESS )
  {
    if ( *returned_data_len_p != GSDI_PERSO_DCK_CK_LEN * 4)
    {
      /* We have a bad length returned...we can't use it */
      gsdi_status = GSDI_ERROR;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);

  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_IS_ISMI_VALID

DESCRIPTION
  This function will be used to determine whether or not the IMSI provided is
  valid.

DEPENDENCIES
  None

RETURN VALUE
  boolean
===========================================================================*/
boolean gsdi_perso_is_imsi_valid(
  int32   imsi_len,
  uint8 * data_p
)
{
  /* --------------------------------------------------------------------------
     Check validity of the data pointer
     ------------------------------------------------------------------------*/
  if ( data_p == NULL )
    ERR_FATAL("No recovery method available:  NULL data_p",0,0,0);

  /* Is IMSI Provisioned */
  if ( data_p[0] == GSDI_IMSI_NOT_PROVISIONED )
  {

      MMGSDI_DEBUG_ERR("IMSI Not Provisionsed, %x",imsi_len,0,0);
      return FALSE;

  }
  else if ( imsi_len < GSDI_MIN_VALID_IMSI_LEN )
  {
      MMGSDI_DEBUG_ERR("IMSI Len %x less than ME MIN %x",imsi_len,GSDI_MIN_VALID_IMSI_LEN,0);
      return FALSE;

  }
  else if ( imsi_len > GSDI_MAX_VALID_IMSI_LEN  )
  {
      MMGSDI_DEBUG_ERR("IMSI Len %x greater than ME MAX %x",imsi_len,GSDI_MAX_VALID_IMSI_LEN,0);
      return FALSE;

  }
  return TRUE;
}
/*===========================================================================
FUNCTION GSDI_PERSO_IS_TEST_IMSI_DETECTED

DESCRIPTION
  This function will detect whether or not the TEST IMSI of
  001 01 is inserted.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
boolean gsdi_perso_is_test_imsi_detected(
  void )
{
  if ( memcmp(imsi_buffer,test_imsi_buffer,3))
    return FALSE;
  else
    return TRUE;
}
/*===========================================================================
FUNCTION GSDI_PERSO_DECRYPT_DATA

DESCRIPTION
  This function is called to decrypt the data that is currently encrypted.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void gsdi_perso_decrypt_data(
  int32       encrypted_data_len,
  uint8 *     encrypted_data_p,
  int32 *     raw_data_len_p,
  uint8 *     raw_data_p
)
{
  int32   index            = 0;

  if ( encrypted_data_p     == NULL ||
       raw_data_len_p       == NULL ||
       raw_data_p           == NULL )
  {
    return;
  }

  /* --------------------------------------------------------------------------
     Copy the Cipher Key to the cipher_key_buffer
     ------------------------------------------------------------------------*/
  memcpy(gsdi_encryption_key_buffer,
         encrypted_data_p+(encrypted_data_len/2),
         int32touint32(encrypted_data_len/2));

  while ( index < encrypted_data_len/2 )
  {
    raw_data_p[index] = gsdi_encryption_key_buffer[index] ^ encrypted_data_p[index];
    index++;
  }

  *raw_data_len_p = index;
}
/*===========================================================================
FUNCTION GSDI_PERSO_ACTIVATE_STATE_PROCEDURES_CB

DESCRIPTION
  This function is the callback used to determine the status of the attempt
  to activate the currently deactivated indicator as part of the self
  persoanlziation procdures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void gsdi_perso_self_perso_activate_state_procedures_cb(
  gsdi_cnf_T * gsdi_cnf_p
)
{
  gsdi_perso_int_init_procedure_req_T int_init_req;
  /* --------------------------------------------------------------------------
     Sanity Check...did we get the correct response
     ------------------------------------------------------------------------*/
  if ( gsdi_cnf_p->message_header.resp_type != GSDI_PERSO_ACT_IND_RSP )
  {
    /* Just returning can potentially hang the GSDI Task.  A SELF Perso sanity
    ** timer will be used to ensure we don't hang
    */
    return;
  }
  /* --------------------------------------------------------------------------
     Set Generic Data for the Internal Initialization Request to send back
     ------------------------------------------------------------------------*/
  int_init_req.message_header.message_set         = 13;
  int_init_req.message_header.slot                = GSDI_SLOT_1;
  int_init_req.message_header.message_id          = GSDI_PERSO_INT_PROC_REQ;
  int_init_req.message_header.gsdi_async_callback = gsdi_perso_int_init_handler_cb;
  int_init_req.message_header.message_len_msb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T )-sizeof(gsdi_cmdhdr_T))/0x100);
  int_init_req.message_header.message_len_lsb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T)-sizeof(gsdi_cmdhdr_T))%0x100);
  /* --------------------------------------------------------------------------
     Was the Deactivation Attempt successful.  If not, we must return and
     generate a command to let the self perso state change to error for proper
     error handling
     ------------------------------------------------------------------------*/
  if ( gsdi_cnf_p->message_header.gsdi_status != GSDI_SUCCESS )
  {
    int_init_req.deactivated      = TRUE;
    int_init_req.self_perso_state = GSDI_PERSO_ERROR_STATE;
  }
  else
  {
    int_init_req.deactivated      = FALSE;
    int_init_req.self_perso_state = GSDI_PERSO_WAIT_FOR_ACT_STATE;
    int_init_req.int_init_feature = gsdi_cnf_p->dact_ind_cnf.perso_feature;
  }
  int_init_req.feature = GSDI_PERSO_NONE;
  (void)gsdi_cmd_ext((void *)&int_init_req);
}
/*===========================================================================
FUNCTION GSDI_PERSO_DEACTIVATE_STATE_PROCEDURES_CB

DESCRIPTION
  This function is the callback used to determine the status of the attempt
  to deactivate the currently active indicator as part of the self
  persoanlziation procdures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void gsdi_perso_self_perso_deactivate_state_procedures_cb(
  gsdi_cnf_T * gsdi_cnf_p
)
{
  gsdi_perso_int_init_procedure_req_T int_init_req;
  /* --------------------------------------------------------------------------
     Sanity Check...did we get the correct response
     ------------------------------------------------------------------------*/
  if ( gsdi_cnf_p->message_header.resp_type != GSDI_PERSO_DEACT_IND_RSP )
  {
    /* Just returning can potentially hang the GSDI Task.  A SELF Perso sanity
    ** timer will be used to ensure we don't hang
    */
    return;
  }
  /* --------------------------------------------------------------------------
     Set Generic Data for the Internal Initialization Request to send back
     ------------------------------------------------------------------------*/
  int_init_req.message_header.message_set         = 13;
  int_init_req.message_header.slot                = GSDI_SLOT_1;
  int_init_req.message_header.message_id          = GSDI_PERSO_INT_PROC_REQ;
  int_init_req.message_header.gsdi_async_callback = gsdi_perso_int_init_handler_cb;
  int_init_req.message_header.message_len_msb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T )-sizeof(gsdi_cmdhdr_T))/0x100);
  int_init_req.message_header.message_len_lsb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T)-sizeof(gsdi_cmdhdr_T))%0x100);
  /* --------------------------------------------------------------------------
     Was the Deactivation Attempt successful.  If not, we must return and
     generate a command to let the self perso state change to error for proper
     error handling
     ------------------------------------------------------------------------*/
  if ( gsdi_cnf_p->message_header.gsdi_status != GSDI_SUCCESS )
  {
    int_init_req.deactivated      = FALSE;
    int_init_req.self_perso_state = GSDI_PERSO_ERROR_STATE;
  }
  else
  {
    int_init_req.deactivated      = TRUE;
    int_init_req.self_perso_state = GSDI_PERSO_WAIT_FOR_DEACT_STATE;
    int_init_req.int_init_feature = gsdi_cnf_p->dact_ind_cnf.perso_feature;
  }
  int_init_req.feature = GSDI_PERSO_NONE;
  (void)gsdi_cmd_ext((void *)&int_init_req);
}
/*===========================================================================
FUNCTION GSDI_PERSO_READ_AND_FORMAT_IMSI

DESCRIPTION
  This function will be called to read the IMSI and convert it to a usable
  format.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_read_and_format_imsi(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_sim_read_req_T * read_req_p        = NULL;
  gsdi_returns_T        gsdi_status       = GSDI_SUCCESS;
  byte                  digit1            = 0;
  byte                  digit2            = 0;
  uint8                 num_mnc_digits    = 0;

  read_req_p = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));
  if ( read_req_p == NULL )
    ERR_FATAL("Could not get memory for read request",0,0,0);

  memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));
  read_req_p->message_header.slot   = GSDI_SLOT_1;
  read_req_p->index_mode            = GSDI_NO_INDEX;
  read_req_p->rec_num               = 0;
  read_req_p->data_offset           = 0;
  read_req_p->required_data_len     = 0;      /* Return the Max Possible */
  read_req_p->search_string_len     = 0x00;

  /* --------------------------------------------------------------------------
     Read the Administrative Data Elementary File
     ------------------------------------------------------------------------*/
  if ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK )
  {
    read_req_p->sim_filename = (word)UIM_GSM_AD;
    read_req_p->search_string_len     = 0x00;
    gsdi_status = gsdi_gsm_proc_sim_read( read_req_p,
                                          &digit1,
                                          &digit2,
                                          (byte *)returned_data_len_p,
                                          (byte *)data_p );

    if ( gsdi_status == GSDI_SUCCESS )
    {
      num_mnc_digits = data_p[3];
      if ( (num_mnc_digits != 0x02 ) &&
           (num_mnc_digits != 0x03 ) )
      {
        num_mnc_digits = 0x02;
      }
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR("Could not read GSM AD",0,0,0);
      gsdi_status = GSDI_ERROR;
    }
  }
  else if ( gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK )
  {
    read_req_p->sim_filename = (word)UIM_USIM_AD;
    read_req_p->search_string_len     = 0x00;
    gsdi_status = gsdi_umts_proc_usim_read( read_req_p,
                                            &digit1,
                                            &digit2,
                                            (byte *)returned_data_len_p,
                                            (byte *)data_p );
    if ( gsdi_status == GSDI_SUCCESS )
    {
      num_mnc_digits = data_p[3];
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR("Could not read USIM AD",0,0,0);
      gsdi_status = GSDI_ERROR;
    }
  }
  else
    gsdi_status = GSDI_ERROR;

  /* --------------------------------------------------------------------------
     Read the IMSI Elementary File
     ------------------------------------------------------------------------*/
  if ( gsdi_status == GSDI_SUCCESS )
  {
    if ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK )
    {
      read_req_p->sim_filename = (word)UIM_GSM_IMSI;
      read_req_p->search_string_len     = 0x00;
      gsdi_status = gsdi_gsm_proc_sim_read( read_req_p,
                                            &digit1,
                                            &digit2,
                                           (byte *)returned_data_len_p,
                                           (byte *)data_p );
      if ( gsdi_status != GSDI_SUCCESS )
        MMGSDI_DEBUG_MSG_ERROR("Could not read GSM AD",0,0,0);
    }
    else if ( gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK )
    {
      read_req_p->sim_filename = (word)UIM_USIM_IMSI;
      read_req_p->search_string_len     = 0x00;
      gsdi_status = gsdi_umts_proc_usim_read( read_req_p,
                                              &digit1,
                                              &digit2,
                                              (byte *)returned_data_len_p,
                                              (byte *)data_p );
      if ( gsdi_status != GSDI_SUCCESS )
        MMGSDI_DEBUG_MSG_ERROR("Could not read USIM IMSI",0,0,0);
    }
    else
      gsdi_status = GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Now format the IMSI from:
     B1       B2       B3       B4     B5   B6   B7    B8
     MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3 D3D2 D5D4 D7D6 D9D8

     TO (2 Digit MNC)
     B1       B2       B3       B4    B5   B6   B7    B8
     MCC2MCC1 MNC1MCC3 FFMNC2   D1NC2 D3D2 D5D4 D7D6 D9D8

     OR (3 Digit MNC)
     B1       B2       B3       B4    B5   B6   B7    B8
     MCC2MCC1 MNC1MCC3 MNC3MNC2 D1NC2 D3D2 D5D4 D7D6 D9D8
     ------------------------------------------------------------------------*/
  digit1 = data_p[GSDI_PERSO_IMSI_DIGIT_1_OFFSET] & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
  digit2 = data_p[GSDI_PERSO_IMSI_DIGIT_2_OFFSET] & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
  digit1 = (digit1 >> 4) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
  digit2 = (digit2 << 4) & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
  imsi_buffer[0] = digit1 | digit2;

  digit1 = data_p[GSDI_PERSO_IMSI_DIGIT_3_OFFSET] & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
  if ( num_mnc_digits == 0x02 )
  {
    digit2 = 0xF0;
    digit1 = (digit1 >> 4) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    imsi_buffer[1] = digit1 | digit2;

    imsi_buffer[2] = data_p[3];
    imsi_buffer[3] = data_p[4];
    imsi_buffer[4] = data_p[5];
    imsi_buffer[5] = data_p[6];
    imsi_buffer[6] = data_p[7];
    imsi_buffer[7] = data_p[8];
  }
  else
  {
    digit2  = data_p[GSDI_PERSO_IMSI_DIGIT_6_OFFSET] & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    digit2 = (digit2 << 4) & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[1] = digit1 | digit2;

    imsi_buffer[2] = data_p[GSDI_PERSO_IMSI_DIGITS_4_5_OFFSET];

    digit1 = ( data_p[4] >> 4 ) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    digit2 = ( data_p[5] << 4 ) & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[3] = digit1 | digit2;

    digit1 = ( data_p[5] >> 4 ) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    digit2 = ( data_p[6] << 4 ) & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[4] = digit1 | digit2;

    digit1 = ( data_p[6] >> 4 ) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    digit2 = ( data_p[7] << 4 ) & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[5] = digit1 | digit2;

    digit1 = ( data_p[7] >> 4 ) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    digit2 = ( data_p[8] << 4 ) & GSDI_PERSO_IMSI_HIGH_DIGIT_MASK;
    imsi_buffer[6] = digit1 | digit2;

    digit1 = ( data_p[8] >> 4 ) & GSDI_PERSO_IMSI_LOW_DIGIT_MASK;
    digit2 = 0xF0;
    imsi_buffer[7] = digit1 | digit2;

  }
  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);

  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_GET_RAND_VALUE

DESCRIPTION
  This function is used to get a Unsigned 32 Bit Integer Value.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
uint32 gsdi_get_rand_value(
void
)
{
  return (uint32)rand();
}
/*===========================================================================
FUNCTION GSDI_PERSO_CREATE_CONTROL_KEYS

DESCRIPTION
  This function will be called only once to generate the control keys

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void gsdi_perso_create_control_key(
  uint8 * data_p
)
{
  uint32    rand_value = 0;
  int32     index      = 0;

  while ( index < GSDI_PERSO_MAX_CK )
  {
    rand_value = (uint32)gsdi_get_rand_value(); /* Get the Random 32 Bit Value     */
    rand_value = (rand_value % 9) + 0x30;       /* Convert random value to ascii 0-9*/
    data_p[index] = (uint8)rand_value;
    index++;
  }
}

/* *===========================================================================
FUNCTION GSDI_PERSO_ENCRYPT_DATA

DESCRIPTION
  This function is called to encrypt the data provided.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void gsdi_perso_encrypt_data (
int32    raw_data_len,
uint8 *  raw_data_buffer_p,
int32 *  encrypted_data_len_p,
uint8 *  encrypted_data_buffer_p
)
{
  int32 index            = 0x00;

  if ( raw_data_buffer_p == NULL ||
       encrypted_data_len_p == NULL ||
       encrypted_data_buffer_p == NULL )
  {
    ERR_FATAL("NULL Pointer Provided",0,0,0);
  }

  while ( index < raw_data_len )
  {
    gsdi_encryption_key_buffer[index] = (uint8)gsdi_get_rand_value()%256;
    encrypted_data_buffer_p[index]    = gsdi_encryption_key_buffer[index] ^ raw_data_buffer_p[index];
    index++;
  }

  /* Now Paste the Cipher Key to the end of the Data */
  memcpy(&encrypted_data_buffer_p[index],gsdi_encryption_key_buffer,int32touint32(index));
  *encrypted_data_len_p = index *2;
}
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_CODES

DESCRIPTION
  This function is a function used to read the SIM Contents and check them
  against the contents stored in FS.

  This particular function will check the MCC and MNC + the reset of th IMSI

  This requires that IMSI is read and compared against the IMSI List in the
  mobile.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_check_sim_sim_codes(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_returns_T        gsdi_status    = GSDI_ERROR;
  int32                 i              = 0;
  int32                 loop_control   = 0;
  boolean               mismatch       = FALSE;
  int32                 enc_data_len   = 0;


  /* --------------------------------------------------------------------------
     Now check these codes against those stored in the codes file stored
     in the file system
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_read_file(array_of_code_files[GSDI_PERSO_SIM_INDEX],
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);
  /* --------------------------------------------------------------------------
     From the returned data Len calculate the number of codes to be check
     *returned_data_len_p / 4 -> loop control index
    *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
    ------------------------------------------------------------------------*/
  if ( (*returned_data_len_p % 8 ) != 0 &&
        gsdi_status                 == GSDI_SUCCESS )
  {
    /* Badly formatted Codes */
    gsdi_status =  GSDI_ERROR;
  }
  if ( gsdi_status == GSDI_SUCCESS )
  {
    loop_control = *returned_data_len_p / 8;

    for (i = 0; i < loop_control; i++ )
    {
      if ( memcmp(&data_p[i * 8],imsi_buffer,8) )
      {
        /* NW Code is not matching...try the next one */
        mismatch = TRUE;
      }
      else
      {
        mismatch = FALSE;
        break;
      }
    }
    if ( mismatch )
    {
      gsdi_status = GSDI_ERROR;
    }
  }
  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_CP_CODES

DESCRIPTION
  This function is a function used to read the SIM Contents and check them
  against the contents stored in FS.

  This particular function will check the MCC and MNC + the Corporate
  code stored in the SIM/USIM in Elementary File GID2.

  This requires that IMSI is read (extract MCC + MNC).
  This requires that the GID1 file is read and that the Service Provider
  codes are extracted from it.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_check_sim_cp_codes(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_returns_T          gsdi_status    = GSDI_ERROR;
  gsdi_sim_read_req_T *   read_req_p     = NULL;
  int32                   i              = 0;
  int32                   loop_control   = 0;
  boolean                 mismatch       = FALSE;
  byte                    dummy1         = 0;
  byte                    dummy2         = 0;
  uint8                   cp_code_buffer[5];
  int32                   num_sp_bytes   = 0;
  int32                   num_cp_bytes   = 0;
  int32                   cp_index       = 0;
  int32                   sp_index       = 0;
  uint8 *                 data_p2        = NULL;
  uint8 *                 data_p3        = NULL;
  uint8                   temp_byte      = 0;
  int32                   enc_data_len   = 0;

  /* --------------------------------------------------------------------------
     Retrieve memory for the 2nd data pointer
     ------------------------------------------------------------------------*/
  data_p2 = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
  if ( data_p2 == NULL )
    ERR_FATAL("No memory for data pointer 2",0,0,0);

  data_p3 = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
  if ( data_p3 == NULL )
    ERR_FATAL("No memory for data pointer 3",0,0,0);

  /* --------------------------------------------------------------------------
     Retrieve memory for the read request.
     Set the Data to read GID2 for the Corporate code
     ------------------------------------------------------------------------*/
  read_req_p = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));
  if ( read_req_p == NULL )
    ERR_FATAL("Could not allocate memory for read req",0,0,0);

  read_req_p->message_header.slot = GSDI_SLOT_1;
  read_req_p->index_mode          = GSDI_NO_INDEX;
  read_req_p->search_string_len   = 0x00;
  read_req_p->data_offset         = 0x00;
  read_req_p->rec_num             = 0x00;
  if ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK )
  {
    read_req_p->sim_filename = (word)UIM_GSM_GID2;
    gsdi_status = gsdi_gsm_proc_sim_read( read_req_p,
                                          &dummy1,
                                          &dummy2,
                                          (byte *)returned_data_len_p,
                                          (byte *)data_p );
  }
  else
  {
    read_req_p->sim_filename = (word)UIM_USIM_GID2;
    gsdi_status = gsdi_umts_proc_usim_read( read_req_p,
                                            &dummy1,
                                            &dummy2,
                                            (byte *)returned_data_len_p,
                                            (byte *)data_p );
  }
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
    MMGSDIUTIL_TMC_MEM_FREE(data_p2);
    MMGSDIUTIL_TMC_MEM_FREE(data_p3);
    return gsdi_status;
  }

  num_cp_bytes = *returned_data_len_p;

  if ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK )
  {
    read_req_p->sim_filename = (word)UIM_GSM_GID1;
    gsdi_status = gsdi_gsm_proc_sim_read( read_req_p,
                                          &dummy1,
                                          &dummy2,
                                          (byte *)returned_data_len_p,
                                          (byte *)data_p2 );
  }
  else
  {
    read_req_p->sim_filename = (word)UIM_USIM_GID1;
    gsdi_status = gsdi_umts_proc_usim_read( read_req_p,
                                            &dummy1,
                                            &dummy2,
                                            (byte *)returned_data_len_p,
                                            (byte *)data_p2 );
  }
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
    MMGSDIUTIL_TMC_MEM_FREE(data_p2);
    MMGSDIUTIL_TMC_MEM_FREE(data_p3);
    return gsdi_status;
  }

  num_sp_bytes = *returned_data_len_p;

  /* --------------------------------------------------------------------------
     Now build the buffer to be used for comparison
     ------------------------------------------------------------------------*/
  while ( sp_index < num_sp_bytes )
  {
    memcpy(cp_code_buffer,imsi_buffer,3);
    temp_byte = ((data_p2[sp_index] << 4) & 0xF0) | ((data_p2[sp_index] >> 4) & 0x0F);
    cp_code_buffer[3] = temp_byte;

    cp_index = 0;
    while ( cp_index < num_cp_bytes )
    {
      temp_byte = ((data_p[cp_index] << 4) & 0xF0 ) | ((data_p[cp_index] >> 4) & 0x0F);
      cp_code_buffer[4] = temp_byte;

      /* --------------------------------------------------------------------------
         Now check these codes against those stored in the codes file stored
         in the file system
        ------------------------------------------------------------------------*/
      gsdi_status = gsdi_efs_read_file(array_of_code_files[GSDI_PERSO_CP_INDEX],
                                       &enc_data_len,
                                       gsdi_encrypted_data_buffer);

      gsdi_perso_decrypt_data(enc_data_len,
                              gsdi_encrypted_data_buffer,
                              returned_data_len_p,
                              data_p3);

      /* --------------------------------------------------------------------------
         From the returned data Len calculate the number of codes to be check
         *returned_data_len_p / 4 -> loop control index
         *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
        ------------------------------------------------------------------------*/
      if ( (*returned_data_len_p % 5 ) != 0 &&
            gsdi_status                 == GSDI_SUCCESS )
      {
        /* Badly formatted Codes */
        gsdi_status =  GSDI_ERROR;
      }
      if ( gsdi_status == GSDI_SUCCESS )
      {
        loop_control = *returned_data_len_p / 5;

        for (i = 0; i < loop_control; i++ )
        {
          if ( memcmp(&data_p3[i * 5],cp_code_buffer,5) )
          {
            /* CP Code is not matching...try the next one */
            mismatch = TRUE;
          }
          else
          {
            mismatch = FALSE;
            break; /* Break For Loop */
          }
        }
      }
      if ( mismatch == FALSE )
        break; /* Break the While Loop */
      cp_index++;
    }
    if ( mismatch == FALSE )
      break;
    sp_index++;
  }
  if ( mismatch )
    gsdi_status = GSDI_ERROR;

  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
  MMGSDIUTIL_TMC_MEM_FREE(data_p2);
  MMGSDIUTIL_TMC_MEM_FREE(data_p3);

  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_SP_CODES

DESCRIPTION
  This function is a function used to read the SIM Contents and check them
  against the contents stored in FS.

  This particular function will check the MCC and MNC + the Service Provide
  code stored in the SIM/USIM in Elementary File GID1.

  This requires that IMSI is read (extract MCC + MNC).
  This requires that the GID1 file is read and that the Service Provider
  codes are extracted from it.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_check_sim_sp_codes(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_sim_read_req_T *   read_req_p     = NULL;
  gsdi_returns_T          gsdi_status    = GSDI_ERROR;
  int32                   i              = 0;
  int32                   loop_control   = 0;
  boolean                 mismatch       = FALSE;
  byte                    dummy1         = 0;
  byte                    dummy2         = 0;
  uint8                   sp_code_buffer[4];
  int32                   num_sp_bytes   = 0;
  int32                   sp_index       = 0;
  uint8 *                 data_p2        = NULL;
  uint8                   temp_byte      = 0;
  int32                   enc_data_len   = 0;
  /* --------------------------------------------------------------------------
     Retrieve memory for the 2nd data pointer
     ------------------------------------------------------------------------*/
  data_p2 = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
  if ( data_p2 == NULL )
    ERR_FATAL("No memory for data pointer 2",0,0,0);

  /* --------------------------------------------------------------------------
     Retrieve memory for the read request.
     Set the Data to read GID1 for the Service Provider code
     ------------------------------------------------------------------------*/
  read_req_p = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));
  if ( read_req_p == NULL )
    ERR_FATAL("Could not allocate memory for read req",0,0,0);

  read_req_p->message_header.slot = GSDI_SLOT_1;
  read_req_p->index_mode          = GSDI_NO_INDEX;
  read_req_p->search_string_len   = 0x00;
  read_req_p->data_offset         = 0x00;
  read_req_p->rec_num             = 0x00;
  if ( gsdi_data.card_applications.apps_enabled & GSDI_GSM_SIM_MASK )
  {
    read_req_p->sim_filename = (word)UIM_GSM_GID1;
    gsdi_status = gsdi_gsm_proc_sim_read( read_req_p,
                                          &dummy1,
                                          &dummy2,
                                          (byte *)returned_data_len_p,
                                          (byte *)data_p );
  }
  else
  {
    read_req_p->sim_filename = (word)UIM_USIM_GID1;
    gsdi_status = gsdi_umts_proc_usim_read( read_req_p,
                                          &dummy1,
                                          &dummy2,
                                          (byte *)returned_data_len_p,
                                          (byte *)data_p );
  }
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
    MMGSDIUTIL_TMC_MEM_FREE(data_p2);
    return gsdi_status;
  }

  num_sp_bytes = *returned_data_len_p;

  /* --------------------------------------------------------------------------
     Now build the buffer to be used for comparison
     ------------------------------------------------------------------------*/
  while ( sp_index < num_sp_bytes &&
          data_p[sp_index] != 0xFF )
  {
    memcpy(sp_code_buffer,imsi_buffer,3);
    temp_byte = ((data_p[sp_index] << 4) & 0xF0) | ((data_p[sp_index] >> 4) & 0x0F);
    sp_code_buffer[3] = temp_byte;

    /* --------------------------------------------------------------------------
       Now check these codes against those stored in the codes file stored
       in the file system
       ------------------------------------------------------------------------*/
    gsdi_status = gsdi_efs_read_file(array_of_code_files[GSDI_PERSO_SP_INDEX],
                                    &enc_data_len,
                                    gsdi_encrypted_data_buffer);

    gsdi_perso_decrypt_data(enc_data_len,
                            gsdi_encrypted_data_buffer,
                            returned_data_len_p,
                            data_p2);

    /* --------------------------------------------------------------------------
      From the returned data Len calculate the number of codes to be check
      *returned_data_len_p / 4 -> loop control index
     *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
     ------------------------------------------------------------------------*/
    if ( (*returned_data_len_p % 4 ) != 0 &&
           gsdi_status                 == GSDI_SUCCESS )
    {
       /* Badly formatted Codes */
       gsdi_status =  GSDI_ERROR;
    }
    if ( gsdi_status == GSDI_SUCCESS )
    {
       loop_control = *returned_data_len_p / 4;

      for (i = 0; i < loop_control; i++ )
      {
        if ( memcmp(&data_p2[i * 4],sp_code_buffer,4) )
        {
           /* NS Code is not matching...try the next one */
           mismatch = TRUE;
        }
        else
        {
          mismatch = FALSE;
          break;
        }
      }
    }
    sp_index++;
    /* Reset i for the for loop */
    i = 0;
    if ( mismatch == FALSE )
      break;
  }
  if ( mismatch ||
       sp_index == 0)
    gsdi_status = GSDI_ERROR;

  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
  MMGSDIUTIL_TMC_MEM_FREE(data_p2);
  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_NS_CODES

DESCRIPTION
  This function is a function used to read the SIM Contents and check them
  against the contents stored in FS.

  This particular function will check the MCC and MNC + Digit 6 and Digit 7
  of the SIM against Network Subset Codes stored in FS.

  This requires that IMSI is read (extract MCC + MNC + Digit 6 & Digit 7).
  This requires that AD is read (determines whether MNC Checks against a 2 digit
  MNC or a 3 Digit MNC).

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_check_sim_ns_codes(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_returns_T        gsdi_status    = GSDI_ERROR;
  int32                 i              = 0;
  int32                 loop_control   = 0;
  int32                 enc_data_len   = 0;
  boolean               mismatch       = FALSE;


  /* --------------------------------------------------------------------------
     Now check these codes against those stored in the codes file stored
     in the file system
     ------------------------------------------------------------------------*/

  gsdi_status = gsdi_efs_read_file(array_of_code_files[GSDI_PERSO_NS_INDEX],
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);
  /* --------------------------------------------------------------------------
     From the returned data Len calculate the number of codes to be check
     *returned_data_len_p / 4 -> loop control index
    *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
    ------------------------------------------------------------------------*/
  if ( (*returned_data_len_p % 4 ) != 0 &&
        gsdi_status                 == GSDI_SUCCESS )
  {
    /* Badly formatted Codes */
    gsdi_status =  GSDI_ERROR;
  }
  if ( gsdi_status == GSDI_SUCCESS )
  {
    loop_control = *returned_data_len_p / 4;

    for (i = 0; i < loop_control; i++ )
    {
      if ( memcmp(&data_p[i * 4],imsi_buffer,4) )
      {
        /* NW Code is not matching...try the next one */
        mismatch = TRUE;
      }
      else
      {
        mismatch = FALSE;
        break;
      }
    }
    if ( mismatch )
    {
      gsdi_status = GSDI_ERROR;
    }
  }
  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_NW_CODES

DESCRIPTION
  This function is a function used to read the SIM Contents and check them
  against the contents stored in FS.

  This particular function will check the MCC and MNC of the SIM against
  the MCC and MNC stored in FS.

  This requires that IMSI is read (extract MCC + MNC).
  This requires that AD is read (determines whether MNC Checks against a 2 digit
  MNC or a 3 Digit MNC).

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_check_sim_nw_codes(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_returns_T        gsdi_status    = GSDI_ERROR;
  int32                 i              = 0;
  int32                 loop_control   = 0;
  int32                 enc_data_len   = 0;
  boolean               mismatch       = FALSE;


  /* --------------------------------------------------------------------------
     Now check these codes against those stored in the codes file stored
     in the file system
     ------------------------------------------------------------------------*/

  gsdi_status = gsdi_efs_read_file(array_of_code_files[GSDI_PERSO_NW_INDEX],
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);
  /* --------------------------------------------------------------------------
     From the returned data Len calculate the number of codes to be check
     *returned_data_len_p / 3 -> loop control index
    *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
    ------------------------------------------------------------------------*/
  if ( (*returned_data_len_p % 3 ) != 0 &&
       gsdi_status                 == GSDI_SUCCESS )
  {
    /* Badly formatted Codes */
    gsdi_status =  GSDI_ERROR;
  }
  if ( gsdi_status == GSDI_SUCCESS )
  {
    loop_control = *returned_data_len_p / 3;

    for (i = 0; i < loop_control; i++ )
    {
      if ( memcmp(&data_p[i * 3],imsi_buffer,3) )
      {
        /* NW Code is not matching...try the next one */
        mismatch = TRUE;
      }
      else
      {
        mismatch = FALSE;
        break;
      }
    }
    if ( mismatch )
    {
      gsdi_status = GSDI_ERROR;
    }
  }
  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_INT_INIT_HANDLER

DESCRIPTION
  This function is the callback function used to handle the response for the
  personalization procedures which in turn kicks off the next personalization
  init procedure.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void gsdi_perso_int_init_handler_cb(
  gsdi_cnf_T * gsdi_cnf_p
)
{
  gsdi_perso_int_init_procedure_req_T req;

  /* --------------------------------------------------------------------------
     Check for NULL Cnf
     ------------------------------------------------------------------------*/
  if ( gsdi_cnf_p == NULL )
  {
    return;
  }

  /* --------------------------------------------------------------------------
     Update the Personalization State Variable
     ------------------------------------------------------------------------*/
  switch ( gsdi_data.perso_state )
  {
  case GSDI_PERSO_INIT_STATE:
    /* Are we currently running Self Personalization */
    switch ( gsdi_cnf_p->int_init_cnf.int_init_s )
    {
    case GSDI_PERSO_SELF_INIT_STATE:
      MMGSDI_DEBUG_ERR("Got GSDI_PERSO_SELF_INIT_STATE in Callback",0,0,0);
      /* This will put is into gsdimain() ... mainly used to handle
      ** a card with an unprovisioned IMSI
      */
      return;
    case GSDI_PERSO_DEACTIVATE_STATE:
      /* The callback indicates we must now build a Deactivation req
      ** for the Internal Initialization Procedures to handle
      */
      req.feature          = GSDI_PERSO_NONE;
      req.self_perso_state = GSDI_PERSO_DEACTIVATE_STATE;
      req.int_init_feature = gsdi_cnf_p->int_init_cnf.int_init_feature;
      break;
    case GSDI_PERSO_SAVE_SIM_TO_FS_STATE:
      /* We need to generate an internal command to read the CNL */
      req.feature          = GSDI_PERSO_NONE;
      req.self_perso_state = GSDI_PERSO_SAVE_SIM_TO_FS_STATE;
      req.int_init_feature = gsdi_cnf_p->int_init_cnf.int_init_feature;
      break;
    case GSDI_PERSO_ACTIVATE_STATE:
      /* The callback indicates we must now build a Activation req
      ** for the Internal Initialization Procedures to handle
      */
      req.feature          = GSDI_PERSO_NONE;
      req.self_perso_state = GSDI_PERSO_ACTIVATE_STATE;
      req.int_init_feature = gsdi_cnf_p->int_init_cnf.int_init_feature;
      break;
    case GSDI_PERSO_WAIT_FOR_DEACT_STATE:
    case GSDI_PERSO_WAIT_FOR_ACT_STATE:
      /* Do nothing...wait for our state to change */
      return;
    case GSDI_PERSO_DONE_STATE:
      /* Maintain the Perso Init State and Initial Feature Indication
      ** information to continue the self personalization
      */
      req.feature          = GSDI_PERSO_NONE;
      req.self_perso_state = GSDI_PERSO_SELF_INIT_STATE;
      /* Move to the next Feature */
      switch ( gsdi_cnf_p->int_init_cnf.int_init_feature )
      {
      case GSDI_PERSO_NW:
        req.int_init_feature = GSDI_PERSO_NS;
        break;
      case GSDI_PERSO_NS:
        req.int_init_feature = GSDI_PERSO_SP;
        break;
      case GSDI_PERSO_SP:
        req.int_init_feature = GSDI_PERSO_CP;
        break;
      case GSDI_PERSO_CP:
        req.int_init_feature = GSDI_PERSO_SIM;
        break;
      case GSDI_PERSO_SIM:
        /* Done */
        break;
      default:
        MMGSDI_DEBUG_MSG_ERROR("Bad internal init feature: %x",gsdi_cnf_p->int_init_cnf.int_init_feature,0,0);
        break;
      }
      if ( gsdi_cnf_p->int_init_cnf.int_init_s == GSDI_PERSO_DONE_STATE &&
           gsdi_cnf_p->int_init_cnf.int_init_feature == GSDI_PERSO_SIM  &&
         ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
           gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
           gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT ))
      {
        /* Ok to move to the next state */
        gsdi_data.perso_state = GSDI_PERSO_NW_INIT_STATE;
        req.feature           = GSDI_PERSO_NW;
      }

      break;
    case GSDI_PERSO_ERROR_STATE:
      ERR("Sanity Error occurred during Self Personalization",0,0,0);
      /* OK to move to the Done State, and complete initialization even
      ** though the UI will display NO CARD
      */
      gsdi_data.perso_state = GSDI_PERSO_COMPLETE_STATE;
      return;
    }
    break;
  case GSDI_PERSO_NW_INIT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS  ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      /* Ok to move to the next state */
      gsdi_data.perso_state = GSDI_PERSO_NS_INIT_STATE;
      req.feature           = GSDI_PERSO_NS;
    }
    else
    {
      gsdi_data.perso_state = GSDI_PERSO_NW_WAIT_DEACT_STATE;
      return;
    }
    break;

  case GSDI_PERSO_NS_INIT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      /* Ok to move to the next state */
      gsdi_data.perso_state =  GSDI_PERSO_SP_INIT_STATE;
      req.feature           = GSDI_PERSO_SP;
    }
    else
    {
      gsdi_data.perso_state = GSDI_PERSO_NS_WAIT_DEACT_STATE;
      return;
    }
    break;

  case GSDI_PERSO_SP_INIT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      /* Ok to move to the next state */
      gsdi_data.perso_state = GSDI_PERSO_CP_INIT_STATE;
      req.feature           = GSDI_PERSO_CP;
    }
    else
    {
      gsdi_data.perso_state = GSDI_PERSO_SP_WAIT_DEACT_STATE;
      return;
    }
    break;

  case GSDI_PERSO_CP_INIT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      /* Ok to move to the next state */
      gsdi_data.perso_state = GSDI_PERSO_SIM_INIT_STATE;
      req.feature           = GSDI_PERSO_SIM;
    }
    else
    {
      gsdi_data.perso_state = GSDI_PERSO_CP_WAIT_DEACT_STATE;
      return;
    }
    break;

  case GSDI_PERSO_SIM_INIT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      /* Ok to move to the next state */
      gsdi_data.perso_state = GSDI_PERSO_COMPLETE_STATE;
    }
    else
    {
      gsdi_data.perso_state = GSDI_PERSO_SIM_WAIT_DEACT_STATE;
    }
    return;

  case GSDI_PERSO_COMPLETE_STATE:
    MSG_HIGH("GSDI PERSO Procedures have compelted",0,0,0);
    (void)gsdi_util_send_mmi_sim_ind(GSDI_PERSO_INIT_COMPLETED);
    return;

  case GSDI_PERSO_NW_WAIT_DEACT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      req.feature           = GSDI_PERSO_NS;
      gsdi_data.perso_state = GSDI_PERSO_NS_INIT_STATE;
    }
    else
    {
      return;
    }
    break;
  case GSDI_PERSO_NS_WAIT_DEACT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      req.feature           = GSDI_PERSO_SP;
      gsdi_data.perso_state = GSDI_PERSO_SP_INIT_STATE;
    }
    else
    {
      return;
    }
    break;
  case GSDI_PERSO_SP_WAIT_DEACT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      req.feature           = GSDI_PERSO_CP;
      gsdi_data.perso_state = GSDI_PERSO_CP_INIT_STATE;
    }
    else
    {
      return;
    }
    break;
  case GSDI_PERSO_CP_WAIT_DEACT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      req.feature           = GSDI_PERSO_SIM;
      gsdi_data.perso_state = GSDI_PERSO_SIM_INIT_STATE;
    }
    else
    {
      return;
    }
    break;
  case GSDI_PERSO_SIM_WAIT_DEACT_STATE:
    if ( gsdi_cnf_p->message_header.gsdi_status == GSDI_SUCCESS ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_PERM_DISABLED ||
         gsdi_cnf_p->message_header.gsdi_status == GSDI_PERSO_CURRENTLY_DEACT )
    {
      req.feature           = GSDI_PERSO_NONE;
      gsdi_data.perso_state = GSDI_PERSO_COMPLETE_STATE;
    }
    else
    {
      return;
    }
    return;
  case GSDI_PERSO_NOT_INIT_STATE:
    break;
  }

  req.message_header.message_set         = 13;
  req.message_header.slot                = GSDI_SLOT_1;
  req.message_header.message_id          = GSDI_PERSO_INT_PROC_REQ;
  req.message_header.gsdi_async_callback = gsdi_perso_int_init_handler_cb;
  req.message_header.message_len_msb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T )-sizeof(gsdi_cmdhdr_T))/0x100);
  req.message_header.message_len_lsb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T)-sizeof(gsdi_cmdhdr_T))%0x100);
  (void)gsdi_cmd_ext(&req);
}

/*===========================================================================
FUNCTION GSDI_PERSO_NOTIFY_CLIENTS_OF_PERSO_EVENT

DESCRIPTION
  This function is used to notify the clients of any Personalization
  Events.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void gsdi_perso_notify_clients_of_perso_event(
  gsdi_perso_event_enum_type event,
  boolean                    additional_info_avail,
  int32                      num_bytes,
  uint8 *                    data_p)
{
  uint32 index = 0x00;
  gsdi_perso_additional_info_type  additional_info;

  /* --------------------------------------------------------------------------
     Memset additional Information for the purpose of having all values set
     to NULL
     ------------------------------------------------------------------------*/
  memset(&additional_info, 0x00, sizeof(gsdi_perso_additional_info_type));
  /* --------------------------------------------------------------------------
     Perform usual parameter checking
     ------------------------------------------------------------------------*/
  if ( additional_info_avail )
  {
    if ( num_bytes > GSDI_MAX_DATA_BLOCK_LEN )
    {
      /* Reset the num_bytes to be no more that GSDI_MAX_DATA_BLOCK_LEN */
      num_bytes = GSDI_MAX_DATA_BLOCK_LEN;
    }
    if ( data_p == NULL )
    {
      /* Reset the additional_info_avail boolean */
      additional_info_avail = FALSE;
    }
  }

  /* --------------------------------------------------------------------------
     Build the additional information structure if it is still set to true
     ------------------------------------------------------------------------*/
  if ( additional_info_avail )
  {
    /* Get memory for the Data Pointer */
    additional_info.buffer_p = mmgsdi_malloc(int32touint32(num_bytes));
    if ( additional_info.buffer_p == NULL )
    {
      additional_info_avail = FALSE;
    }
    else if (data_p != NULL) // Just to make lint happy :-)
    {
      memcpy(additional_info.buffer_p,
             data_p,
             int32touint32(num_bytes));
      additional_info.num_bytes = num_bytes;
    }
  }
  while ( gsdi_perso_event_handler_table[index] != NULL )
  {
    (gsdi_perso_event_handler_table[index])(event,
                                            GSDI_SLOT_1,
                                            additional_info_avail,
                                            &additional_info);
    index++;
    if ( index >= GSDI_PERSO_MAX_REG_CLIENTS )
      break;
  }
  if ( additional_info.buffer_p != NULL )
  {
    MMGSDIUTIL_TMC_MEM_FREE(additional_info.buffer_p);
  }
}


/*===========================================================================
FUNCTION GSDI_PERSO_FS_FILE_TEST

DESCRIPTION
  This function is used to test whether the file already exists
  in FS.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
LOCAL boolean gsdi_perso_fs_file_test(
  char * file_to_test
)
{
  gsdi_returns_T gsdi_status = GSDI_ERROR;

  gsdi_status = gsdi_efs_name_test(file_to_test); /* Pointer to File Name */

  if ( gsdi_status == GSDI_SUCCESS )
    return TRUE;
  else
    return FALSE;
}


/*===========================================================================
FUNCTION GSDI_PERSO_FS_DIR_TEST

DESCRIPTION
  This function is used to test whether the directory already exists
  in FS.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
LOCAL boolean gsdi_perso_fs_dir_test(
  char * dir_to_test
)
{
  gsdi_returns_T gsdi_status = GSDI_ERROR;

  if ( dir_to_test == NULL )
    return FALSE;

  gsdi_status = gsdi_efs_name_test(dir_to_test); /* Pointer to Directory Name */

  if ( gsdi_status == GSDI_SUCCESS )
    return TRUE;
  else
    return FALSE;

}
/*===========================================================================
FUNCTION GSDI_PERSO_CREATE_DIR_AND_FILES

DESCRIPTION
  This function is used to create the directories and files.  This function
  should only be called once during the personalization phase.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
boolean gsdi_perso_create_dir_and_files(
  void
)
{
  gsdi_returns_T gsdi_status;
  int32          index       = 0x00;
  /* --------------------------------------------------------------------------
     We now have to create the file Structure since this is the first
     time it is being created.  Once created, it will exist permanently
     ------------------------------------------------------------------------*/
  /* --------------------------------------------------------------------------
     Create the directory for the first file
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_mkdir(mmgsdi_efs_mmgsdi_dir);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Perso directory",0,0,0);
    return FALSE;
  }
  gsdi_status = gsdi_efs_mkdir(mmgsdi_efs_perso_dir);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Perso directory",0,0,0);
    return FALSE;
  }
  /* --------------------------------------------------------------------------
     Create the Feature Indications File
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_feature_ind);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Perm Disabled Feature Indicators",0,0,0);
    return FALSE;
  }
  /* --------------------------------------------------------------------------
     Create the Permanently Disabled Feature Indicator Filed
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_perm_dis_ind);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Feature Indicators",0,0,0);
    return FALSE;
  }
  /* --------------------------------------------------------------------------
     Create the Master Control Key file
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_master_ck);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Master Control Key",0,0,0);
    return FALSE;
  }
  /* --------------------------------------------------------------------------
     Create the file which contains the number of retries remaining
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_retries_rem);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Num Retries",0,0,0);
    return FALSE;
  }
  /* --------------------------------------------------------------------------
     Create all of the Control Key Files
     All files are define in array_of_ck_files[]
     ------------------------------------------------------------------------*/
  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    gsdi_status = gsdi_efs_create_file(array_of_ck_files[index]);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_ERR("Could not create file: %s",array_of_ck_files[index],0,0);
      return FALSE;
    }
    index++;
  }

  index = 0;
  /* --------------------------------------------------------------------------
     Create the file which contains the number of retries remaining for unblock
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_create_file(mmgsdi_efs_unblock_retries_rem);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_ERR("Could not create Master Control Unblock Key Remain Tries",0,0,0);
    return FALSE;
  }
  /* --------------------------------------------------------------------------
     Create all of the Control Key Unblock Files
     All files are define in array_of_unblock_ck_files[]
     ------------------------------------------------------------------------*/
  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    gsdi_status = gsdi_efs_create_file(array_of_unblock_ck_files[index]);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_ERR("Could not create file: %s",array_of_unblock_ck_files[index],0,0);
      return FALSE;
    }
    index++;
  }

  /* -------------------------------------------------------------------------
     Create all files used to contain the codes to check against the SIM/USIM
     -----------------------------------------------------------------------*/
  index = 0;
  while ( index <= GSDI_PERSO_SIM_INDEX)
  {
    gsdi_status = gsdi_efs_create_file(array_of_code_files[index]);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_ERR("Could not create %s Codes",array_of_code_files[index],0,0);
      return FALSE;
    }
    index++;
  }
  return gsdi_status == GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_PERSO_ARE_ALL_FILES_PRESENT

DESCRIPTION
  This function is used to verify whether or not all files are present
  that are required for use in the file system in the personalization
  solution.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_are_all_files_present(
  void
)
{
  boolean present = TRUE;
  uint32  index   = 0x00;
  int missing = 0;
  int total_tested =0;

  /* --------------------------------------------------------------------------
     Determine if the Feature Indicators File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_fs_file_test(mmgsdi_efs_feature_ind);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS FEATURE IND MISSING",0,0,0);
    ++missing;
  }
  ++total_tested;
  /* --------------------------------------------------------------------------
     Determine if the Permanetly Disabled Feature Indicators File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_fs_file_test(mmgsdi_efs_perm_dis_ind);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS PERM DISABLED FEATURE IND MISSING",0,0,0);
    ++missing;
  }
  ++total_tested;

  /* --------------------------------------------------------------------------
     Determine if the Number of retries remaining File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_fs_file_test(mmgsdi_efs_retries_rem);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS RETRIES REMAINING MISSING",0,0,0);
    ++missing;
  }
  ++total_tested;
  /* --------------------------------------------------------------------------
     Determine if the Number of retries remaining for Unblock File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_fs_file_test(mmgsdi_efs_unblock_retries_rem);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS UNBLOCK RETRIES REMAINING MISSING",0,0,0);
    ++missing;
  }
  ++total_tested;
  /* --------------------------------------------------------------------------
     Determine if all control keys were created
     ------------------------------------------------------------------------*/
  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    present = gsdi_perso_fs_file_test(array_of_ck_files[index]);
    if ( !present )
    {
      MMGSDI_DEBUG_ERR("EFS %s CK MISSING",array_of_ck_files[index],0,0);
      ++missing;
      ++total_tested;
      break;
    }
    index++;
    ++total_tested;
  }

  /* --------------------------------------------------------------------------
     Determine if all FS Files used to store codes is properly created
     ------------------------------------------------------------------------*/
  index = 0x00;
  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    present = gsdi_perso_fs_file_test(array_of_unblock_ck_files[index]);
    if ( !present )
    {
      MMGSDI_DEBUG_ERR("EFS NW UNBLOCK CODES",0,0,0);
      ++missing;
      ++total_tested;
      break;
    }
    index++;
    ++total_tested;
  }

  /* --------------------------------------------------------------------------
     Determine if all FS Files used to store codes is properly created
     ------------------------------------------------------------------------*/
  index = 0x00;
  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    present = gsdi_perso_fs_file_test(array_of_code_files[index]);
    if ( !present )
    {
      MMGSDI_DEBUG_ERR("EFS NW CODES",0,0,0);
      ++missing;
      ++total_tested;
      break;
    }
    index++;
    ++total_tested;
  }

  /*None of the files present*/
  if(total_tested == missing)
  {
    return GSDI_NOT_FOUND;
  }

  /*Som of the files are present*/
  if(missing != 0)
  {
    return GSDI_ERROR;
  }

  /*All files are present*/
  return GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_PERSO_ARE_ALL_NEW_FILES_PRESENT

DESCRIPTION
  This function is used to verify whether or not all files are present
  that are required for use in the file system in the personalization
  solution.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_are_all_new_files_present(
  void
)
{
  boolean present = TRUE;
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
  char *mmgsdi_efs_perso_file  = "/apps/mmgsdi/perso/perso.txt";
#else
  char *mmgsdi_efs_perso_file  = "mmgsdi/perso/perso.txt";
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
  /* --------------------------------------------------------------------------
     Determine if the Feature Indicators File was created
     ------------------------------------------------------------------------*/
  present = gsdi_perso_fs_file_test(mmgsdi_efs_perso_file);
  if ( !present )
  {
    MMGSDI_DEBUG_ERR("EFS PERSO_FILE MISSING",0,0,0);
      return GSDI_NOT_FOUND;
  }

  return GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_SET_DATA_IN_FILES

DESCRIPTION
  This function is used to autogenerate the control keys, indicators,
  and number of retries.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_set_data_in_files(
  int32 *     returned_data_len_p,
  uint8 *     data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_ERROR;
  int32  index                = 0;
  int32  encrypted_len        = 0;

  uint8  num_retries[5]       = {GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES};

  uint8  unblock_num_retries[5]  = {GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES};

  uint8  indicators[5]        = {GSDI_PERSO_NW_INDICATOR,
                                 GSDI_PERSO_NS_INDICATOR,
                                 GSDI_PERSO_SP_INDICATOR,
                                 GSDI_PERSO_CP_INDICATOR,
                                 GSDI_PERSO_SIM_INDICATOR};

  uint8 perm_dis_ind[5]       = {GSDI_PERSO_PERM_DIS_NW_INDICATOR,
                                 GSDI_PERSO_PERM_DIS_NS_INDICATOR,
                                 GSDI_PERSO_PERM_DIS_SP_INDICATOR,
                                 GSDI_PERSO_PERM_DIS_CP_INDICATOR,
                                 GSDI_PERSO_PERM_DIS_SIM_INDICATOR};
  /* --------------------------------------------------------------------------
     First Autogenerate the control Keys
     ------------------------------------------------------------------------*/
  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    /* First determine if file is empty...it should be since the calling
       function just created it
    */
    gsdi_status = gsdi_fs_get_file_size(array_of_ck_files[index],
                                        returned_data_len_p);
    if ( gsdi_status != GSDI_SUCCESS)
      return GSDI_ERROR;

    if ( *returned_data_len_p != 0 )
      return GSDI_ERROR;

    /* Use the random number generate to populate the ck_buffer */
    gsdi_perso_create_control_key(data_p);

    /* Encrypt the Data Accordingly */
    gsdi_perso_encrypt_data(GSDI_PERSO_MAX_CK,
                            data_p,
                            &encrypted_len,
                            gsdi_encrypted_data_buffer);

    gsdi_status = gsdi_efs_write_file(array_of_ck_files[index],
                                      encrypted_len,
                                      gsdi_encrypted_data_buffer);
    if ( gsdi_status != GSDI_SUCCESS )
      return GSDI_ERROR;

    index++;
  }

  index = 0;
  /* Use the random number generate to populate the unblock ck_buffer */
  gsdi_perso_create_control_key(data_p);

  while ( index <= GSDI_PERSO_SIM_INDEX )
  {
    gsdi_status = gsdi_fs_get_file_size(array_of_unblock_ck_files[index],
                                        returned_data_len_p);
    if ( gsdi_status != GSDI_SUCCESS)
      return GSDI_ERROR;

    if ( *returned_data_len_p != 0 )
      return GSDI_ERROR;

    /* Encrypt the Data Accordingly */
    gsdi_perso_encrypt_data(GSDI_PERSO_MAX_CK,
                            data_p,
                            &encrypted_len,
                            gsdi_encrypted_data_buffer);

    gsdi_status = gsdi_efs_write_file(array_of_unblock_ck_files[index],
                                      encrypted_len,
                                      gsdi_encrypted_data_buffer);
    if ( gsdi_status != GSDI_SUCCESS )
      return GSDI_ERROR;

    index++;
  }
  /* --------------------------------------------------------------------------
     Now write the Number of retries for the different Levels
     ------------------------------------------------------------------------*/
  gsdi_perso_encrypt_data(5,
                          num_retries,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_retries_rem,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
    return GSDI_ERROR;

  gsdi_perso_encrypt_data(5,
                          unblock_num_retries,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_unblock_retries_rem,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
    return GSDI_ERROR;


/* --------------------------------------------------------------------------
   Now write the Feature Indicators
   ------------------------------------------------------------------------*/
  gsdi_perso_encrypt_data(5,
                          indicators,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_feature_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
    return GSDI_ERROR;

/* --------------------------------------------------------------------------
   Now write the Perm Disabled Feature Indicators
   ------------------------------------------------------------------------*/
  gsdi_perso_encrypt_data(5,
                          perm_dis_ind,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_perm_dis_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);
  if ( gsdi_status != GSDI_SUCCESS )
    return GSDI_ERROR;

  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_INIT

DESCRIPTION
  This function is used to create all of the relevant Personalization Files
  used for storing information in EFS.

  This procedure is ran every time the mobile is powered up, but will not
  recreate any files already created.

  If all personalization files are not created properly or upon entering
  this function again, only some of the files exist...then an Error Fatal
  will occur.  This could be evidence of tampering.

  It will be acceptable for the file to be empty.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_init(
  int32 * returned_data_len_p,
  uint8 * data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_SUCCESS;
  boolean        sanity_check = FALSE;
  boolean        perso_files_present = FALSE;
  boolean        perso_engine_files_present = FALSE;

  /* --------------------------------------------------------------------------
     Does the Directory structure already exist
     ------------------------------------------------------------------------*/
  sanity_check = gsdi_perso_fs_dir_test(mmgsdi_efs_perso_dir);

  /* --------------------------------------------------------------------------
     If the directory structure exists...then the file system was at one time
     created.  If any of the files required for this feature are not existent
     we must assume it was tampered with.
     If the directory does not exist, it must be created, and all files must
     also be created.
     ------------------------------------------------------------------------*/
  if ( sanity_check )
  {
    gsdi_status = gsdi_perso_are_all_files_present();
    
    /*Some of the files are present but not all*/
    if(gsdi_status == GSDI_ERROR)
    {
      MMGSDI_DEBUG_MSG_ERROR("File System Sanity Check Failed",0,0,0);
      return gsdi_status;
    }

    perso_files_present = (gsdi_status == GSDI_SUCCESS)?TRUE:FALSE;

    /*Directory present but not the old files*/
    if(!perso_files_present)
    {
      /*If the new files are present, then create the old file structure*/
      gsdi_status = gsdi_perso_are_all_new_files_present();
      perso_engine_files_present = (gsdi_status == GSDI_SUCCESS)?TRUE:FALSE;
    }
   
    /*None of the files are present*/
    if (!perso_engine_files_present && !perso_files_present )
    {
      /*Directory is present without any files.*/
      MMGSDI_DEBUG_MSG_ERROR("File System Sanity Check Failed",0,0,0);
      return gsdi_status;
    }
  }

  /*The new file boolean will be set to TRUE ONLY if the old files are not present
    If the OLD files are not present or the entire directory structure is not present,
    then create.*/
  if(perso_engine_files_present || !sanity_check)
  {
    if (!gsdi_perso_create_dir_and_files())
    {
      MMGSDI_DEBUG_ERR("Could not create dirs or files",0,0,0);
      return GSDI_ERROR;
    }
    gsdi_status = gsdi_set_data_in_files(returned_data_len_p,
                                         data_p);
    if ( gsdi_status != GSDI_SUCCESS)
    {
      MMGSDI_DEBUG_ERR("Could not write data to files: %x",0,0,0);
      return gsdi_status;
    }
  }

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SAVE_EVENT_HANDLER

DESCRIPTION
  This function is used to save the event handler provided by the client
  into the perso events event handling table to be called at a later time
  when a personalization event needs to be generated.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_save_event_handler (
  int32                index,
  gsdi_perso_events_cb event_handler_p
)
{
  /* --------------------------------------------------------------------------
     Check the parameters for the index and event handler to be stored
     ------------------------------------------------------------------------*/
  if ( index > GSDI_PERSO_MAX_REG_CLIENTS ||
       index < GSDI_PERSO_MIN_TABLE_INDEX )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD Index : %x ",
      index,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( event_handler_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("event_handler_p == NULL",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  gsdi_perso_event_handler_table[index] = event_handler_p;
  return GSDI_SUCCESS;

} /* gsdi_perso_save_event_handler */

/*===========================================================================
FUNCTION GSDI_PERSO_GET_FREE_EVENT_HANDLER_INDEX

DESCRIPTION
  This function is used to search the gsdi_perso_event_handler_table global
  for a free index.

  This function will also validate the current event handler pointer against
  all event handlers currently stored to search for duplicate event handlers.

DEPENDENCIES
  None

RETURN VALUE
  int32
===========================================================================*/
LOCAL int32 gsdi_perso_get_free_event_handler_index(
  gsdi_perso_events_cb event_handler_p
)
{
  int32 table_index = 0;
  while ( table_index < GSDI_PERSO_MAX_REG_CLIENTS )
  {
    if ( gsdi_perso_event_handler_table[table_index] == event_handler_p )
    {
      /* Client's Event Handler is already registered for Perso
      ** Events
      */
      MSG_HIGH("Client Callback already registered: %x",event_handler_p,0,0);
      return GSDI_PERSO_EVENT_HANDLER_ALREADY_REGISTERD;
    }
    else if ( gsdi_perso_event_handler_table[table_index] == NULL )
    {
      /* An empty slot in the Event Handler Table was Found */
      return table_index;
    }
    else
    {
      table_index++;
    }
  }
  return GSDI_PERSO_NO_EVENT_HANDLER_TABLE_INDEX_AVAILABLE;

} /* gsdi_perso_get_free_event_handler_index */


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_REGISTER_TASK

DESCRIPTION
  This function is used to process the Personalization Request from a client
  to register for Personalization Events.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_INCORRECT_PARMS: Bad input Parameters
                   GSDI_ERROR          : Event Handler Table Full
                   GSDI_SUCCESS        : Successfully saved event handler
                                         - or -
                                         Duplicate Event Handler Detected
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_reg_task (
  gsdi_perso_reg_task_req_T * req_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  int32          index       = 0x00;
  /* -----------------------------------------------------------------------
     Check the Request Parameters to ensure no corruption during
     the context switching
     ---------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Request Pointer",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Due to not define FEATURE_MMGSDI_DUAL_SLOT for Dual SIM Target
     builds, the GSDI will default the slot to slot 1 for any task
     that inadvertently leaves this blank
     ------------------------------------------------------------------------*/
  if ( req_p->message_header.slot != GSDI_SLOT_NONE &&
       req_p->message_header.slot != GSDI_SLOT_1 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_p->message_header.slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( req_p->reg_task_cb_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("null reg_task_cb_p %x",req_p->reg_task_cb_p,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Search the Table which stores event handlers for a free index
     to store the current Event Handler.
     The function will search for duplicate event handlers
     ------------------------------------------------------------------------*/
  index = gsdi_perso_get_free_event_handler_index(req_p->reg_task_cb_p);

 if ( index == GSDI_PERSO_NO_EVENT_HANDLER_TABLE_INDEX_AVAILABLE )
  {
    MMGSDI_DEBUG_MSG_ERROR("Perso Event Handler Table Full",0,0,0);
    return GSDI_ERROR;
  }
  else if ( index == GSDI_PERSO_EVENT_HANDLER_ALREADY_REGISTERD )
  {
    MSG_HIGH("Perso Event Handler already registered",0,0,0);
    return GSDI_SUCCESS;
  }
  else if ( index > GSDI_PERSO_MAX_REG_CLIENTS )
  {
    MMGSDI_DEBUG_ERR("Invalid Index received: %x",index,0,0);
    return GSDI_ERROR;
  }
  else
  {
    /* Index is Valid store the Index Received */
    gsdi_status = gsdi_perso_save_event_handler(index,
                                                req_p->reg_task_cb_p);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_ERR("Error can't save to Event Handler Table: %x",gsdi_status,0,0);
    }
  }
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_FS_GET_FEATURE_INDICATORS

DESCRIPTION
  This function is a wrapper to the FS Interface used to retrieve the
  feature indicators.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_fs_get_feature_indicators(
  char  *                            file_name,
  int32 *                            returned_data_len_p,
  uint8 *                            data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_ERROR;
  int32          enc_data_len = 0;
  /* --------------------------------------------------------------------------
    Performed some basic parameter checking...ensure they are not NULL
    -------------------------------------------------------------------------*/
  if ( file_name           == NULL ||
       returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Parameter recieved",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Read the File from FS
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_read_file(file_name,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status == GSDI_SUCCESS &&
       *returned_data_len_p == 0 )
  {
    MMGSDI_DEBUG_MSG_ERROR("0 Length returned for Feature Indicator File",0,0,0);
    gsdi_status = GSDI_ERROR;
  }
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_GET_FEATURE_KEY

DESCRIPTION
  This function is used to execute the Get control key of a Personalization
  Feature Indicator.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_get_feature_key(
    gsdi_perso_get_feature_key_req_T   * req_p,
    int32 *                              returned_data_len_p,
    uint8 *                              data_p )
{
  gsdi_returns_T             gsdi_status      = GSDI_SUCCESS;
  char *                     file_name_data_p = NULL;
  nv_item_type               gsdi_nv_data_buf; /* NV data buffer */
  nv_stat_enum_type          read_status;
  uint8                      ue_imei[GSDI_PERSO_MAX_IMEI_LEN] = {0};
  uint8                      dec_index = 0;
  uint8                      bcd_index = 1;
  uint8                      temp_ck_array[GSDI_PERSO_MAX_CK];
  int32                      temp_returned_ck_len;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     1.  req_p can not be NULL
     2.  returned_data_len_p can not be NULL
     3.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null req_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null returned_data_len_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("data_p is NULL",0,0,0);
    return GSDI_ERROR;
  }

  if (req_p->is_unblock_key) {
    switch ( req_p->feature )
    {
    case GSDI_PERSO_NW:
      file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_NW_INDEX];
      break;
    case GSDI_PERSO_NS:
      file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_NS_INDEX];
      break;
    case GSDI_PERSO_SP:
      file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_SP_INDEX];
      break;
    case GSDI_PERSO_CP:
      file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_CP_INDEX];
      break;
    case GSDI_PERSO_SIM:
      file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_SIM_INDEX];
      break;
    default:
      MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->feature,0,0);
      return GSDI_ERROR;
    }
  }
  else {
    switch ( req_p->feature )
    {
    case GSDI_PERSO_NW:
      file_name_data_p = array_of_ck_files[GSDI_PERSO_NW_INDEX];
      break;
    case GSDI_PERSO_NS:
      file_name_data_p = array_of_ck_files[GSDI_PERSO_NS_INDEX];
      break;
    case GSDI_PERSO_SP:
      file_name_data_p = array_of_ck_files[GSDI_PERSO_SP_INDEX];
      break;
    case GSDI_PERSO_CP:
      file_name_data_p = array_of_ck_files[GSDI_PERSO_CP_INDEX];
      break;
    case GSDI_PERSO_SIM:
      file_name_data_p = array_of_ck_files[GSDI_PERSO_SIM_INDEX];
      break;
    default:
      MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->feature,0,0);
      return GSDI_ERROR;
    }
  }

  gsdi_status = gsdi_efs_read_file(file_name_data_p,
                                   returned_data_len_p,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(*returned_data_len_p,
                          gsdi_encrypted_data_buffer,
                          &temp_returned_ck_len,
                          data_p);

  if (temp_returned_ck_len > GSDI_PERSO_MAX_CK)
  {
    MMGSDI_DEBUG_MSG_ERROR("CK Too Long 0x%x", temp_returned_ck_len, 0, 0);
    return GSDI_ERROR;
  }

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Reading %s",file_name_data_p,0,0);
    return gsdi_status;
  }

  memcpy(temp_ck_array, data_p, int32touint32(temp_returned_ck_len));

  /* ------------------------------------------------------------------------
    Is the IMEI Provided a valid IMEI
    ----------------------------------------------------------------------*/
  read_status = gsdi_get_nv(NV_UE_IMEI_I,
                            &gsdi_nv_data_buf);

  if ( read_status != NV_DONE_S )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not get IMEI",0,0,0);
    return GSDI_ERROR;
  }
  if ( gsdi_nv_data_buf.ue_imei.ue_imei[0] == 0x00 ||
       gsdi_nv_data_buf.ue_imei.ue_imei[0] == 0xFF ||
       gsdi_nv_data_buf.ue_imei.ue_imei[0] > GSDI_PERSO_MAX_IMEI_LEN )
  {
    MMGSDI_DEBUG_MSG_ERROR("IMEI is not set or improperly set",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Convert the IMEI from BCD To Decimal
     ------------------------------------------------------------------------*/
  while ( bcd_index <= gsdi_nv_data_buf.ue_imei.ue_imei[0])
  {
    /* Place High Byte in Index 0 (Even) Low Byte in Index 1 (Odd) ... */
    if ( dec_index % 2 == 0 )
    {
      ue_imei[dec_index] = 0x30 +
            (((gsdi_nv_data_buf.ue_imei.ue_imei[bcd_index] & GSDI_MASK_FOR_HIGH_BYTE) >> 0x04) & 0x0F);
      dec_index++;
      bcd_index++;
    }
    else
    {
      /* Get the Low Byte of BCD  0x0N */
      ue_imei[dec_index] = 0x30 + (gsdi_nv_data_buf.ue_imei.ue_imei[bcd_index] & GSDI_MASK_FOR_LOW_BYTE);
      dec_index++;
    }
  }
  memcpy(data_p, ue_imei, dec_index);
  *returned_data_len_p = dec_index;
  data_p[dec_index] = 0x3A; /* Add ':' in between */
  *returned_data_len_p += 1;
  memcpy(&data_p[dec_index+1],temp_ck_array,int32touint32(temp_returned_ck_len));
  *returned_data_len_p += temp_returned_ck_len;

  data_p[*returned_data_len_p] = (uint8)req_p->feature;

  return GSDI_SUCCESS;
} /* gsdi_proc_perso_get_feature_key */

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_GET_FEATURE_IND

DESCRIPTION
  This function is used to retrieve the Feature Indicators from Persistent
  memory which is then reported back to the client.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_get_feature_ind(
  gsdi_perso_get_feature_ind_req_T * req_p,
  int32 *                            returned_data_len_p,
  uint8 *                            data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_ERROR;
  /* --------------------------------------------------------------------------
     Check the Parameters Provided
     1.  req_p can not be NULL
     2.  slot can only be GSDI_SLOT_1 or GSDI_SLOT_2
     3.  retunred_data_len_p can not be NULL
     4.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Parameter provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( req_p->message_header.slot != GSDI_SLOT_1 &&
       req_p->message_header.slot != GSDI_SLOT_2 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_p->message_header.slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL returned_data_len_p provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL data_p provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_feature_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("EFS Read for Perso Feature Indicators Failed: %x",
      gsdi_status,0,0);
    return gsdi_status;
  }

  if ( *returned_data_len_p > GSDI_PERSO_MAX_FEATURE_IND_LEN )
  {
    MMGSDI_DEBUG_MSG_ERROR("Too many Personalization Indicators: %x ",
      returned_data_len_p,0,0);
    return GSDI_ERROR;
  }

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_GET_PERM_FEATURE_IND

DESCRIPTION
  This function is used to retrieve the Perm Feature Indicators from Persistent
  memory which is then reported back to the client.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_get_perm_feature_ind(
  gsdi_perso_get_perm_feature_ind_req_T * req_p,
  int32 *                            returned_data_len_p,
  uint8 *                            data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_ERROR;
  /* --------------------------------------------------------------------------
     Check the Parameters Provided
     1.  req_p can not be NULL
     2.  slot can only be GSDI_SLOT_1 or GSDI_SLOT_2
     3.  retunred_data_len_p can not be NULL
     4.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL Parameter provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( req_p->message_header.slot != GSDI_SLOT_1 &&
       req_p->message_header.slot != GSDI_SLOT_2 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_p->message_header.slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL returned_data_len_p provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL data_p provided",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_perm_dis_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("EFS Read for Perm Perso Feature Indicators Failed: %x",
      gsdi_status,0,0);
    return gsdi_status;
  }

  if ( *returned_data_len_p > GSDI_PERSO_MAX_FEATURE_IND_LEN )
  {
    MMGSDI_DEBUG_MSG_ERROR("Too many Personalization Indicators: %x ",
      returned_data_len_p,0,0);
    return GSDI_ERROR;
  }

  return gsdi_status;
} /* gsdi_proc_perso_get_perm_feature_ind */



/*===========================================================================
FUNCTION GSDI_PROC_PERSO_IS_FEATURE_BLOCKED

DESCRIPTION
  This function is used to determine if the feature control key has been
  blocked

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_is_feature_blocked(
  gsdi_perso_enum_type feature,
  int32   *            returned_data_len_p,
  uint8   *            data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_PERSO_CURRENTLY_DEACT;
  uint8          offset       = 0x00;
  int32          enc_data_len = 0;

  if ( returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Parameter Checks Failed",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  switch ( feature )
  {
  case GSDI_PERSO_NW:
    offset = GSDI_PERSO_NW_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_NS:
    offset = GSDI_PERSO_NS_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_SP:
    offset = GSDI_PERSO_SP_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_CP:
    offset = GSDI_PERSO_CP_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_SIM:
    offset = GSDI_PERSO_SIM_STATUS_IND_OFFSET;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Read the File from FS which contains the number of retries remaining
     ------------------------------------------------------------------------*/
 gsdi_status = gsdi_efs_read_file(mmgsdi_efs_retries_rem,
                                 &enc_data_len,
                                 gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

 if ( gsdi_status != GSDI_SUCCESS )
 {
   MMGSDI_DEBUG_MSG_ERROR("Attempt to read Retries Remaining Failed",0,0,0);
   return gsdi_status;
 }

 if (data_p[offset] != GSDI_PERSO_NO_MORE_CK_RETRIES) {
   data_p[0] = FALSE;
 }
 else {
   data_p[0] = TRUE;
 }
 return gsdi_status;

} /* gsdi_perso_is_feature_blocked */

/*===========================================================================
FUNCTION GSDI_PPERSO_IS_FEATURE_ACTIVATED

DESCRIPTION
  This function is used to determine whether or not the feature is currently
  active.

  Input:  feature   -> Personalization Indicator
  Output:

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
LOCAL gsdi_returns_T gsdi_perso_is_feature_activated(
  gsdi_perso_enum_type feature,
  int32   *            returned_data_len_p,
  uint8   *            data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_PERSO_CURRENTLY_DEACT;
  uint8          indicator   = GSDI_PERSO_FEATURE_NOT_ACTIVATED;

  if ( returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Parameter Checks Failed",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* ------------------------------------------------------------------------
     Retrieve from FS the Feature Indicators presently set in
     the Personalization File Structure
     Data is stored in EFS in the following structure
     data_p[0] -> Network Personalization           0x01 (ON) 0x00 (OFF)
     data_p[1] -> Network Subsystem Personalization 0x01 (ON) 0x00 (OFF)
     data_p[2] -> Service Provider Personalization  0x01 (ON) 0x00 (OFF)
     data_p[3] -> Corporate Personalization         0x01 (ON) 0x00 (OFF)
     data_p[4] -> SIM Personalization               0x01 (ON) 0x00 (OFF)
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_feature_ind,
                                                     returned_data_len_p,
                                                     data_p);
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    indicator = data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_NS:
    indicator = data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_SP:
    indicator = data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_CP:
    indicator = data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_SIM:
    indicator = data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET];
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }
  if ( indicator == GSDI_PERSO_FEATURE_ACTIVATED )
  {
    MSG_HIGH("Feature %x is Activated",feature,0,0);
    gsdi_status = GSDI_PERSO_CURRENTLY_ACTIVE;
  }
  else
  {
    MSG_HIGH("Feature %x is Not active",feature,0,0);
    gsdi_status = GSDI_PERSO_CURRENTLY_DEACT;
  }

  return gsdi_status;
} /* gsdi_perso_is_feature_activated */

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_IS_FEATURE_PERM_DISABLED

DESCRIPTION
  This function is used to determine whether or not a feature is
  permanetly disabled or not.

  Input:  feature   -> Personalization Indicator
  Output:

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
LOCAL gsdi_returns_T gsdi_perso_is_feature_perm_disabled(
  gsdi_perso_enum_type feature,
  int32   *            returned_data_len_p,
  uint8   *            data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint8          indicator   = GSDI_PERSO_FEATURE_NOT_PERM_DISABLED;

  if ( returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Parameter Checks Failed",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* ------------------------------------------------------------------------
     Retrieve from FS the Permanently Disabled Feature Indicators
     presently set in the Personalization File Structure Data is stored in
     EFS in the following structure
     data_p[0] -> Network Personalization           0x01 (PERM DISABLED)
     data_p[1] -> Network Subsystem Personalization 0x01 (PERM DISABLED)
     data_p[2] -> Service Provider Personalization  0x01 (PERM DISABLED)
     data_p[3] -> Corporate Personalization         0x01 (PERM DISABLED)
     data_p[4] -> SIM Personalization               0x01 (PERM DISABLED)
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_perm_dis_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("GSDI - FS Get Feature Ind Failure: %x",gsdi_status,0,0);
    return gsdi_status;
  }
  /* --------------------------------------------------------------------------
     Retrieve the Indicator status
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    indicator = data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_NS:
    indicator = data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_SP:
    indicator = data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_CP:
    indicator = data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET];
    break;
  case GSDI_PERSO_SIM:
    indicator = data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET];
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    gsdi_status = GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     check to see if perm disabled and set status accordingly
     ------------------------------------------------------------------------*/
  if ( indicator == GSDI_PERSO_FEATURE_PERM_DISABLED )
  {
    MSG_HIGH("Feature %x is Perm Disabled",0,0,0);
    gsdi_status = GSDI_PERSO_PERM_DISABLED;
  }

  return gsdi_status;
} /* gsdi_perso_is_feature_perm_disabled */


/*===========================================================================
FUNCTION GSDI_PERSO_DECREASE_INCORRECT_CK_COUNT

DESCRIPTION
  Decrease the incorrect CK Count

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T  gsdi_perso_decrease_incorrect_ck_count(
  gsdi_perso_enum_type feature,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_returns_T             gsdi_status   = GSDI_SUCCESS;
  gsdi_perso_event_enum_type event         = GSDI_PERSO_NO_EVENT;
  uint8                      offset        = 0x00;
  int32                      encrypted_len = 0;

  /* --------------------------------------------------------------------------
     Set the offset, and the event so that the event doesn't have to be
     figured out later in the case that it is blocked
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    offset = GSDI_PERSO_NW_STATUS_IND_OFFSET;
    event  = GSDI_PERSO_NCK_BLOCKED;
    break;
  case GSDI_PERSO_NS:
    offset = GSDI_PERSO_NS_STATUS_IND_OFFSET;
    event  = GSDI_PERSO_NSK_BLOCKED;
    break;
  case GSDI_PERSO_SP:
    offset = GSDI_PERSO_SP_STATUS_IND_OFFSET;
    event  = GSDI_PERSO_SPK_BLOCKED;
    break;
  case GSDI_PERSO_CP:
    offset = GSDI_PERSO_CP_STATUS_IND_OFFSET;
    event  = GSDI_PERSO_CCK_BLOCKED;
    break;
  case GSDI_PERSO_SIM:
    offset = GSDI_PERSO_SIM_STATUS_IND_OFFSET;
    event  = GSDI_PERSO_PPK_BLOCKED;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Read the File from FS which contains the number of retries remaining
     ------------------------------------------------------------------------*/
 gsdi_status = gsdi_efs_read_file(mmgsdi_efs_retries_rem,
                                 &encrypted_len,
                                 gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(encrypted_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

 if ( gsdi_status != GSDI_SUCCESS )
 {
   MMGSDI_DEBUG_MSG_ERROR("Attempt to read Retries Remaining Failed",0,0,0);
   return gsdi_status;
 }

 /* ---------------------------------------------------------------------------
    Data Successfully obtained.
    1.  Check to see if it is already blocked
    -------------------------------------------------------------------------*/
 if ( data_p[offset] == GSDI_PERSO_NO_MORE_CK_RETRIES )
 {
   MMGSDI_DEBUG_MSG_ERROR("CK for Feature %x PERM BLOCKED", feature,0,0);
   return GSDI_PERSO_CK_BLOCKED;
 }

 /* ---------------------------------------------------------------------------
    Not currently blocked...ok to decriment
    -------------------------------------------------------------------------*/
 data_p[offset]--;

 /* ---------------------------------------------------------------------------
    Write data back to the file
    -------------------------------------------------------------------------*/
    /* Encrypt the Data Accordingly */
    gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                            data_p,
                            &encrypted_len,
                            gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_retries_rem,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not write back Data: %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }

  if ( data_p[offset] == GSDI_PERSO_NO_MORE_CK_RETRIES )
  {
    gsdi_perso_notify_clients_of_perso_event(event,
                                             FALSE,
                                             0x00,
                                             NULL);
  }
  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_DECREASE_INCORRECT_UNBLOCK_CK_COUNT

DESCRIPTION
  Decrease the incorrect CK Count

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T  gsdi_perso_decrease_incorrect_unblock_ck_count(
  gsdi_perso_enum_type feature,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_returns_T  gsdi_status  = GSDI_SUCCESS;
  uint8           offset       = 0x00;
  int32           enc_data_len = 0;

  /* --------------------------------------------------------------------------
     Set the offset, and the event so that the event doesn't have to be
     figured out later in the case that it is blocked
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    offset = GSDI_PERSO_NW_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_NS:
    offset = GSDI_PERSO_NS_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_SP:
    offset = GSDI_PERSO_SP_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_CP:
    offset = GSDI_PERSO_CP_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_SIM:
    offset = GSDI_PERSO_SIM_STATUS_IND_OFFSET;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Read the File from FS which contains the number of retries remaining
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_read_file(mmgsdi_efs_unblock_retries_rem,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Attempt to read Unblock Retries Remaining Failed",0,0,0);
    return gsdi_status;
  }

  /* ---------------------------------------------------------------------------
    Data Successfully obtained.
    1.  Check to see if it is already blocked
    -------------------------------------------------------------------------*/
  if ( data_p[offset] == GSDI_PERSO_NO_MORE_CK_RETRIES )
  {
    MMGSDI_DEBUG_MSG_ERROR("CK for Feature %x PERM BLOCKED", feature,0,0);
    return GSDI_PERSO_UNBLOCK_CK_BLOCKED;
  }

  /* ---------------------------------------------------------------------------
    Not currently blocked...ok to decriment
    -------------------------------------------------------------------------*/
  data_p[offset]--;

  /* ---------------------------------------------------------------------------
    Write data back to the file
    -------------------------------------------------------------------------*/
  /* Encrypt the Data Accordingly */
  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                          data_p,
                          &enc_data_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_unblock_retries_rem,
                                    enc_data_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not write back Data: %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }

  return gsdi_status;
}


/*===========================================================================
FUNCTION gsdi_perso_reset_num_retries_rem

DESCRIPTION
  This function is used to reset the retries remaining for the Feature presented

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_reset_num_retries_rem(
  gsdi_perso_enum_type feature,
  int32 *              returned_data_len_p,
  uint8 *              data_p)
{
  gsdi_returns_T  gsdi_status  = GSDI_SUCCESS;
  uint8           offset       = 0x00;
  int32           enc_data_len = 0;

  /* --------------------------------------------------------------------------
     Set the offset, and the event so that the event doesn't have to be
     figured out later in the case that it is blocked
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    offset = GSDI_PERSO_NW_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_NS:
    offset = GSDI_PERSO_NS_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_SP:
    offset = GSDI_PERSO_SP_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_CP:
    offset = GSDI_PERSO_CP_STATUS_IND_OFFSET;
    break;
  case GSDI_PERSO_SIM:
    offset = GSDI_PERSO_SIM_STATUS_IND_OFFSET;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Read the File from FS which contains the number of retries remaining
     ------------------------------------------------------------------------*/
 gsdi_status = gsdi_efs_read_file(mmgsdi_efs_retries_rem,
                                  &enc_data_len,
                                  gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

 if ( gsdi_status != GSDI_SUCCESS )
 {
   MMGSDI_DEBUG_MSG_ERROR("Attempt to read Retries Remaining Failed",0,0,0);
   return gsdi_status;
 }

 /* ---------------------------------------------------------------------------
    reset the number retries
    -------------------------------------------------------------------------*/
 data_p[offset] = GSDI_PERSO_MAX_NUMBER_OF_RETRIES;

 /* ---------------------------------------------------------------------------
    Write data back to the file
    -------------------------------------------------------------------------*/

  /* Encrypt the Data Accordingly */
  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                          data_p,
                          &enc_data_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_retries_rem,
                                    enc_data_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not write back Data: %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }

  return gsdi_status;
}

/*===========================================================================
FUNCTION gsdi_perso_verify_unblock_ck

DESCRIPTION
  This function is used to verify the Unblock Control Key for the Feature presented

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
LOCAL gsdi_returns_T gsdi_perso_verify_unblock_ck
(
  gsdi_perso_enum_type feature,
  int32                num_bytes,
  uint8 *              buffer_p,
  int32 *              returned_data_len_p,
  uint8 *              data_p
 )
{
  gsdi_returns_T gsdi_status      = GSDI_SUCCESS;
  char *         file_name_data_p = NULL;
  int32          enc_data_len     = 0;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     ------------------------------------------------------------------------*/
  if ( buffer_p            == NULL ||
       returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Bad Parameters",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Get the Control Keys from FS and compare with the Control Keys
     provided
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    file_name_data_p = array_of_unblock_ck_files[GSDI_PERSO_SIM_INDEX];
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }

  gsdi_status = gsdi_efs_read_file(file_name_data_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Reading %s",file_name_data_p,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Perform the CK Comparison Procedures
     Are the number of CK bytes correct
     ------------------------------------------------------------------------*/
  if ( *returned_data_len_p != num_bytes )
  {
    MMGSDI_DEBUG_MSG_ERROR("Len of CKs: %x != %x",*returned_data_len_p,num_bytes,0);
    gsdi_status =  gsdi_perso_decrease_incorrect_unblock_ck_count(feature,
                                                          returned_data_len_p,
                                                          data_p);
    if ( gsdi_status == GSDI_SUCCESS )
    {
      gsdi_status = GSDI_PERSO_INVALID_CK;
    }
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Do the CKs Match
     ------------------------------------------------------------------------*/
  if (memcmp(buffer_p,data_p,int32touint32(num_bytes)))
  {
    MMGSDI_DEBUG_MSG_ERROR("CKs don't match",0,0,0);
    (void) gsdi_perso_decrease_incorrect_unblock_ck_count(feature,
                                                  returned_data_len_p,
                                                  data_p);
    if ( gsdi_status == GSDI_SUCCESS )
    {
      gsdi_status = GSDI_PERSO_INVALID_CK;
    }
    return gsdi_status;
  }

  gsdi_status= gsdi_perso_reset_num_retries_rem(feature,
                                                returned_data_len_p,
                                                data_p);
  if (gsdi_status != GSDI_SUCCESS)
  {
    MMGSDI_DEBUG_MSG_ERROR("Could Not Reset Num Retries from FS",0,0,0);
  }

  /* --------------------------------------------------------------------------
     Perform a fineal read on the Number of Unblock Retries to populate
     data_p for use when building the Cnf
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_read_file(mmgsdi_efs_unblock_retries_rem,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
    MMGSDI_DEBUG_MSG_ERROR("Could Not Read Num Retries from FS",0,0,0);

  return gsdi_status;
} /* gsdi_perso_verify_unblock_ck */



/*===========================================================================
FUNCTION GSDI_perso_verify_ck

DESCRIPTION
  This function is used to verify the Control Key for the Feature presented

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
LOCAL gsdi_returns_T gsdi_perso_verify_ck
(
  gsdi_perso_enum_type feature,
  int32                num_bytes,
  uint8 *              buffer_p,
  int32 *              returned_data_len_p,
  uint8 *              data_p
 )
{
  gsdi_returns_T gsdi_status      = GSDI_SUCCESS;
  char *         file_name_data_p = NULL;
  int32          enc_data_len     = 0;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     ------------------------------------------------------------------------*/
  if ( buffer_p            == NULL ||
       returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Bad Parameters",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Get the Control Keys from FS and compare with the Control Keys
     provided
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    file_name_data_p = array_of_ck_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    file_name_data_p = array_of_ck_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    file_name_data_p = array_of_ck_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    file_name_data_p = array_of_ck_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    file_name_data_p = array_of_ck_files[GSDI_PERSO_SIM_INDEX];
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",feature,0,0);
    return GSDI_ERROR;
  }

  gsdi_status = gsdi_efs_read_file(file_name_data_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Reading %s",file_name_data_p,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Perform the CK Comparison Procedures
     Are the number of CK bytes correct
     ------------------------------------------------------------------------*/
  if ( *returned_data_len_p != num_bytes )
  {
    MMGSDI_DEBUG_MSG_ERROR("Len of CKs: %x != %x",*returned_data_len_p,num_bytes,0);
    gsdi_status =  gsdi_perso_decrease_incorrect_ck_count(feature,
                                                          returned_data_len_p,
                                                          data_p);
    if ( gsdi_status == GSDI_SUCCESS )
    {
      gsdi_status = GSDI_PERSO_INVALID_CK;
    }
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Do the CKs Match
     ------------------------------------------------------------------------*/
  if (memcmp(buffer_p,data_p,int32touint32(num_bytes)))
  {
    MMGSDI_DEBUG_MSG_ERROR("CKs don't match",0,0,0);
    (void) gsdi_perso_decrease_incorrect_ck_count(feature,
                                                  returned_data_len_p,
                                                  data_p);
    if ( gsdi_status == GSDI_SUCCESS )
    {
      gsdi_status = GSDI_PERSO_INVALID_CK;
    }
    return gsdi_status;
  }

  gsdi_status = gsdi_efs_read_file(mmgsdi_efs_retries_rem,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
    MMGSDI_DEBUG_MSG_ERROR("Could Not Read Num Retries from FS",0,0,0);

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_UNBLOCK_FEATURE_IND

DESCRIPTION
  This function is used to execute the Unblock of a Personalization
  Feature Indicator.

  This function will perform the following:
  1.  Check to see if the Feature is already Permanently Deactivated
  2.  Check to see if the Feature is already Block
  3.  Unblock the feature, if successful, deactivate the feature

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_unblock_feature_ind(
  gsdi_perso_unblock_feature_ind_req_T * req_p,
  int32 *                                returned_data_len_p,
  uint8 *                                data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint8          num_retries[5];
  int32          encrypted_len        = 0;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     1.  req_p can not be NULL
     2.  returned_data_len_p can not be NULL
     3.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null req_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null returned_data_len_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("data_p is NULL",0,0,0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Blocked
     If Indicator is true, it is activated and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_blocked(req_p->feature,
                                          returned_data_len_p,
                                          data_p);

  /* first byte returned from feature_blocked in data_p indicates if the feature is
     blocked or not */
  if((gsdi_status != GSDI_SUCCESS) || (data_p[0] != 1))
  {
    MMGSDI_DEBUG_MSG_ERROR("Feature unblocked: %x",req_p->feature,0,0);
    /* Reset status to GSDI_ERROR */
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Verify the Personalization Control Key
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_verify_unblock_ck(req_p->feature,
                                             req_p->num_bytes,
                                             req_p->unblock_ck_buffer,
                                             returned_data_len_p,
                                             data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Personalization Feature: %x, failed CK Check",req_p->feature,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Save into temp buffer because it is lost in the get feature indicators
     ------------------------------------------------------------------------*/
  memcpy(num_retries,
         data_p,
         5);

  /* --------------------------------------------------------------------------
     All checks were successful, it is ok to turn on the personalization
     indication for this feature.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_feature_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Getting Feature Indicators: %x",gsdi_status,0,0);
    return gsdi_status;
  }

  switch ( req_p->feature )
  {
  case GSDI_PERSO_NW:
    data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_NS:
    data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_SP:
    data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_CP:
    data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_SIM:
    data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] = 0x00;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->feature,0,0);
    return GSDI_ERROR;
  }

  /* Encrypt the Data Accordingly */
  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                            data_p,
                            &encrypted_len,
                            gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_feature_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Saving Personalization indicators %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Save into indicators from num_retries back into data_p for use in the
     conf.
     ------------------------------------------------------------------------*/
  memcpy(data_p,
         num_retries,
         5);

  return GSDI_SUCCESS;
} /* gsdi_proc_perso_unblock_feature_ind */


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_PERM_DISABLE_FEATURE_IND

DESCRIPTION
  This function is used to execute the Permanently disabling of a Personalization
  Feature Indicator.

  This function will perform the following:
  1.  Check to see if the Feature is already Permanently Deactivated
  2.  Permanetly disable the feature

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_perm_disable_feature_ind(
  gsdi_perso_perm_disabled_feature_req_T * req_p,
  int32 *                                  returned_data_len_p,
  uint8 *                                  data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  uint8          num_retries[5];
  int32          encrypted_len        = 0;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     1.  req_p can not be NULL
     2.  returned_data_len_p can not be NULL
     3.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null req_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null returned_data_len_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("data_p is NULL",0,0,0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Blocked
     If Indicator is true, it is activated and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_blocked(req_p->perso_feature,
                                              returned_data_len_p,
                                              data_p);

  /* first byte returned from feature_blocked in data_p indicates if the feature is
     blocked or not */
  if((gsdi_status != GSDI_SUCCESS) || (data_p[0] != 0))
  {
    MMGSDI_DEBUG_MSG_ERROR("Feature blocked: %x",req_p->perso_feature,0,0);
    /* Reset status to GSDI_ERROR */
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Perm Disabled
     If Indicator is true, it is perm disabled and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->perso_feature,
                                                    returned_data_len_p,
                                                    data_p);

  /* --------------------------------------------------------------------------
     GSDI_SUCCESS             -> indication that the Feature is not
                                 Perm Disabled
     GSDI_PERSO_PERM_DISABLED -> At one point the feature was disabled
     ------------------------------------------------------------------------*/
  if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Perm Disabled Feature already disable: %x Status: %x",
               req_p->perso_feature,gsdi_status,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Verify the Personalization Control Key
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_verify_ck( req_p->perso_feature,
                                      req_p->num_bytes,
                                      req_p->ck_buffer,
                                      returned_data_len_p,
                                      data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Personalization Feature: %x, failed CK Check",req_p->perso_feature,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Save into temp buffer because it is lost in the get feature indicators
     ------------------------------------------------------------------------*/
  memcpy(num_retries,
         data_p,
         5);

  /* --------------------------------------------------------------------------
     All checks were successful, it is ok to turn on the permanent disable
     indicator for the feature
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_perm_dis_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Getting Perm Disable Feature Indicators: %x",gsdi_status,0,0);
    return gsdi_status;
  }

  switch ( req_p->perso_feature )
  {
  case GSDI_PERSO_NW:
    data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_NS:
    data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_SP:
    data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_CP:
    data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_SIM:
    data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] = 0x01;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->perso_feature,0,0);
    return GSDI_ERROR;
  }

  /* Encrypt the Data Accordingly */
  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                          data_p,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_perm_dis_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Saving Personalization Perm indicators %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Tried to deactivate the feature now.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_feature_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Getting Feature Indicators: %x",gsdi_status,0,0);
    return gsdi_status;
  }

  switch ( req_p->perso_feature )
  {
  case GSDI_PERSO_NW:
    data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_NS:
    data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_SP:
    data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_CP:
    data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] = 0x00;
    break;
  case GSDI_PERSO_SIM:
    data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] = 0x00;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->perso_feature,0,0);
    return GSDI_ERROR;
  }

  /* Encrypt the Data Accordingly */
  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                            data_p,
                            &encrypted_len,
                            gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_feature_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Saving Personalization indicators %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Save into indicators from num_retries back into data_p for use in the
     conf.
     ------------------------------------------------------------------------*/
  memcpy(data_p,
         num_retries,
         5);

  return GSDI_SUCCESS;
} /* gsdi_proc_perso_perm_disable_feature_ind */


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_ACTIVATE_FEATURE_IND

DESCRIPTION
  This function is used to execute the Activation of a Personalization
  Feature Indicator.

  This function will perform the following:
  1.  Check to see if the Feature is already Permanently Deactivated
  2.  Check to see if the Feature is already Activated
  3.  See if the the Feature Contains any Data so that it can be activated

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_activate_feature_ind(
  gsdi_perso_act_feature_ind_req_T * req_p,
  int32 *                            returned_data_len_p,
  uint8 *                            data_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_SUCCESS;
  uint8          num_retries[5];
  int32          encrypted_len = 0;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     1.  req_p can not be NULL
     2.  returned_data_len_p can not be NULL
     3.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null req_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null returned_data_len_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("data_p is NULL",0,0,0);
    return GSDI_ERROR;
  }
  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Perm Disabled
     If Indicator is true, it is perm disabled and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                    returned_data_len_p,
                                                    data_p);
  /* --------------------------------------------------------------------------
     GSDI_SUCCESS             -> indication that the Feature is not
                                 Perm Disabled
     GSDI_PERSO_PERM_DISABLED -> At one point the feature was disabled
     ------------------------------------------------------------------------*/
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Perm Disabled Feature: %x Status: %x",req_p->feature,gsdi_status,0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Activated
     If Indicator is true, it is activated and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                returned_data_len_p,
                                                data_p);
 /* --------------------------------------------------------------------------
     GSDI_PERSO_CURRENTLY_DEACT  -> indication that the Feature is not currently
                                    active
     GSDI_PERSO_CURRENTLY_ACTIVE -> The feature is already active...can not
                                    activate it again.
     ------------------------------------------------------------------------*/
  if ( gsdi_status == GSDI_PERSO_CURRENTLY_ACTIVE ||
       gsdi_status == GSDI_ERROR )
  {
    MMGSDI_DEBUG_MSG_ERROR("Activated Feature: %x Status: %x",req_p->feature,gsdi_status,0);
    /* Reset status to GSDI_SUCCESS */
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Verify the Personalization Control Key
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_verify_ck(req_p->feature,
                                     req_p->num_bytes,
                                     req_p->ck_buffer,
                                     returned_data_len_p,
                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Personalization Feature: %x, failed CK Check",req_p->feature,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Save into temp buffer because it is lost in the get feature indicators
     ------------------------------------------------------------------------*/
  memcpy(num_retries,
         data_p,
         5);

  /* --------------------------------------------------------------------------
     All checks were successful, it is ok to turn on the personalization
     indication for this feature.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_feature_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Getting Feature Indicators: %x",gsdi_status,0,0);
    return gsdi_status;
  }

  switch ( req_p->feature )
  {
  case GSDI_PERSO_NW:
    data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_NS:
    data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_SP:
    data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_CP:
    data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] = 0x01;
    break;
  case GSDI_PERSO_SIM:
    data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] = 0x01;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->feature,0,0);
    return GSDI_ERROR;
  }
  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                          data_p,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_feature_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Saving Personalization indicators %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Save into indicators from num_retries back into data_p for use in the
     conf.
     ------------------------------------------------------------------------*/
  memcpy(data_p,
         num_retries,
         5);

  return GSDI_SUCCESS;
} /* gsdi_proc_perso_activate_feature_ind */

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_DEACTIVATE_FEATURE_IND

DESCRIPTION
  This function is used to execute the Activation of a Personalization
  Feature Indicator.

  This function will perform the following:
  1.  Check to see if the Feature is already Permanently Deactivated
  2.  Check to see if the Feature is already Activated
  3.  See if the the Feature Contains any Data so that it can be activated

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_deactivate_feature_ind(
  gsdi_perso_deact_feature_ind_req_T * req_p,
  int32 *                              returned_data_len_p,
  uint8 *                              data_p
)
{
  gsdi_perso_int_init_procedure_req_T  req_int;
  gsdi_perso_event_enum_type event            = GSDI_PERSO_NO_EVENT;
  gsdi_returns_T             gsdi_status      = GSDI_SUCCESS;
  uint8                      num_retries[5];
  int32                      encrypted_len    = 0;

  /* --------------------------------------------------------------------------
     Perform some basic parameter checking
     1.  req_p can not be NULL
     2.  returned_data_len_p can not be NULL
     3.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null req_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null returned_data_len_p",0,0,0);
    return GSDI_ERROR;
  }
  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("data_p is NULL",0,0,0);
    return GSDI_ERROR;
  }
  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Perm Disabled
     If Indicator is true, it is perm disabled and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                    returned_data_len_p,
                                                    data_p);
  /* --------------------------------------------------------------------------
     GSDI_SUCCESS             -> indication that the Feature is not
                                 Perm Disabled
     GSDI_PERSO_PERM_DISABLED -> At one point the feature was disabled
     ------------------------------------------------------------------------*/
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Perm Disabled Feature: %x Status: %x",req_p->feature,gsdi_status,0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Activated
     If Indicator is true, it is activated and it is ok to proceed with the
     deactivation procedures.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                returned_data_len_p,
                                                data_p);
 /* --------------------------------------------------------------------------
     GSDI_SUCCESS                -> indication that the Feature is not currently
                                    active
     GSDI_PERSO_CURRENTLY_ACTIVE -> The feature is already active...can not
                                    activate it again.
     ------------------------------------------------------------------------*/
  if ( gsdi_status != GSDI_PERSO_CURRENTLY_ACTIVE )
  {
    MMGSDI_DEBUG_MSG_ERROR("Feature: %x Status: %x",req_p->feature,gsdi_status,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Verify the Personalization Control Key
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_verify_ck(req_p->feature,
                                     req_p->num_bytes,
                                     &req_p->ck_buffer[0],
                                     returned_data_len_p,
                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Personalization Feature: %x, failed CK Check",req_p->feature,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Save into temp buffer because it is lost in the get feature indicators
     ------------------------------------------------------------------------*/
  memcpy(num_retries,
         data_p,
         5);

  /* --------------------------------------------------------------------------
     All checks were successful, it is ok to turn off the personalization
     indication for this feature.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_fs_get_feature_indicators(mmgsdi_efs_feature_ind,
                                                     returned_data_len_p,
                                                     data_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Getting Feature Indicators: %x",gsdi_status,0,0);
    return gsdi_status;
  }

  switch ( req_p->feature )
  {
  case GSDI_PERSO_NW:
    data_p[GSDI_PERSO_NW_STATUS_IND_OFFSET] = 0x00;
    event = GSDI_PERSO_NW_DEACTIVATED; /* NW Personalization Deactivated        */
    break;
  case GSDI_PERSO_NS:
    data_p[GSDI_PERSO_NS_STATUS_IND_OFFSET] = 0x00;
    event = GSDI_PERSO_NS_DEACTIVATED; /* NS Personalization Deactivated        */
    break;
  case GSDI_PERSO_SP:
    data_p[GSDI_PERSO_SP_STATUS_IND_OFFSET] = 0x00;
    event = GSDI_PERSO_SP_DEACTIVATED; /* PS Personalization Deactivated        */
    break;
  case GSDI_PERSO_CP:
    data_p[GSDI_PERSO_CP_STATUS_IND_OFFSET] = 0x00;
    event = GSDI_PERSO_CP_DEACTIVATED; /* CP Personalization Deactivated        */
    break;
  case GSDI_PERSO_SIM:
    data_p[GSDI_PERSO_SIM_STATUS_IND_OFFSET] = 0x00;
    event = GSDI_PERSO_SIM_DEACTIVATED; /* SIM/USIM Personalization Deactivated */
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->feature,0,0);
    return GSDI_ERROR;
  }

  gsdi_perso_encrypt_data(GSDI_PERSO_MAX_FEATURE_IND_LEN,
                          data_p,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(mmgsdi_efs_feature_ind,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Error Saving Personalization indicators %x",gsdi_status,0,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Notify all tasks that the Personalization Feature has been
     Deactivated.
     ------------------------------------------------------------------------*/
  gsdi_perso_notify_clients_of_perso_event(event,
                                           FALSE,
                                           0x00,
                                           NULL);
  /* --------------------------------------------------------------------------
     What is our current Perso State
     ------------------------------------------------------------------------*/
  req_int.feature = GSDI_PERSO_NONE;
  switch ( gsdi_data.perso_state )
  {
  case GSDI_PERSO_NW_WAIT_DEACT_STATE:
    if ( req_p->feature == GSDI_PERSO_NW )
    {
      req_int.feature     = GSDI_PERSO_NW;
      req_int.deactivated = TRUE;
    }
    break;
  case GSDI_PERSO_NS_WAIT_DEACT_STATE:
    if ( req_p->feature  == GSDI_PERSO_NS )
    {
      req_int.feature     = GSDI_PERSO_NS;
      req_int.deactivated = TRUE;
    }
    break;
  case GSDI_PERSO_SP_WAIT_DEACT_STATE:
    if ( req_p->feature  == GSDI_PERSO_SP )
    {
      req_int.feature     = GSDI_PERSO_SP;
      req_int.deactivated = TRUE;
    }
    break;
  case GSDI_PERSO_CP_WAIT_DEACT_STATE:
    if ( req_p->feature  == GSDI_PERSO_CP )
    {
      req_int.feature     = GSDI_PERSO_CP;
      req_int.deactivated = TRUE;
    }
    break;
  case GSDI_PERSO_SIM_WAIT_DEACT_STATE:
    if ( req_p->feature  == GSDI_PERSO_SIM )
    {
      req_int.feature     = GSDI_PERSO_SIM;
      req_int.deactivated = TRUE;
    }
    break;
  default:
    MSG_HIGH("Deactivate before/after Init procedures",0,0,0);
    break;
  }
  if ( req_int.feature != GSDI_PERSO_NONE )
  {
    /* Continue building request and submit to Task */
    req_int.message_header.message_set         = 13;
    req_int.message_header.slot                = GSDI_SLOT_1;
    req_int.message_header.message_id          = GSDI_PERSO_INT_PROC_REQ;
    req_int.message_header.gsdi_async_callback = gsdi_perso_int_init_handler_cb;
    req_int.message_header.message_len_msb     = (byte)((sizeof(
      gsdi_perso_int_init_procedure_req_T )-sizeof(gsdi_cmdhdr_T))/0x100);
    req_int.message_header.message_len_lsb     = (byte)((sizeof(
      gsdi_perso_int_init_procedure_req_T)-sizeof(gsdi_cmdhdr_T))%0x100);
    gsdi_status = gsdi_cmd_ext(&req_int);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }
  }

  /* --------------------------------------------------------------------------
     Save into indicators from num_retries back into data_p for use in the
     conf.
     ------------------------------------------------------------------------*/
  memcpy(data_p,
         num_retries,
         5);

  return GSDI_SUCCESS;
} /* gsdi_proc_perso_deactivate_feature_ind */

/*===========================================================================
FUNCTION GSDI_PERSO_VALIDATE_IMSI_AGAINST_FS_NW

DESCRIPTION
  This function is used to validate the IMSI Data against the NETWORK and
  Netowrk Subset codes stored in FS.  If no match is found, an error is
  returned.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_validate_imsi_against_fs(
  gsdi_perso_enum_type feature,
  int32                validate_num_bytes,
  uint8 *              validate_data_p,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_ERROR;
  int32          i             = 0;
  int32          j             = 0;
  int32          i_data        = 0;
  int32          j_fs          = 0;
  int32          enc_data_len  = 0;
  boolean        match_found   = FALSE;
  char *         fs_filename_p = NULL;


  /* --------------------------------------------------------------------------
     Perform Basic Parameter Checking
     ------------------------------------------------------------------------*/
  if ( validate_data_p     == NULL ||
       returned_data_len_p == NULL ||
       data_p              == NULL ||
       validate_num_bytes  >= GSDI_MAX_DATA_BLOCK_LEN)
  {
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Validate validate_num_bytes is of the correct length.  The number
     of bytes must be properly divisable by:
     NW: 3
     ------------------------------------------------------------------------*/
  if ( validate_num_bytes % 8)
  {
    return GSDI_INCORRECT_PARAMS;
  }
  else
  {
    validate_num_bytes = validate_num_bytes / 8;
  }
  /* --------------------------------------------------------------------------
     Read the NW Codes from FS
     ------------------------------------------------------------------------*/

  fs_filename_p = array_of_code_files[GSDI_PERSO_NW];
  gsdi_status = gsdi_efs_read_file(fs_filename_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not Read FS: 0x%x Validating 0x%x",gsdi_status,feature,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Validate the returned_data_len_p from FS is properly divisable by
     the correct number:
     NW:  3
     ------------------------------------------------------------------------*/
  if ( *returned_data_len_p % 3 )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not read correct number of bytes: 0x%x",*returned_data_len_p,0,0);
    return GSDI_ERROR;
  }
  else
  {
    *returned_data_len_p = *returned_data_len_p / 3;
  }
  /* --------------------------------------------------------------------------
     data_p now contains the FS Contents of the NW Codes.  Check the data
     provided against this data
     ------------------------------------------------------------------------*/
  while (i <  validate_num_bytes ||
         i == GSDI_MAX_DATA_BLOCK_LEN )
  {
    i_data = i * 8;
    match_found = FALSE;
    while ( j < *returned_data_len_p )
    {
      j_fs = j * 3;

      /* Compare the memory */
      if ( !(memcmp(&validate_data_p[i_data],&data_p[j_fs],3)) )
      {
        match_found = TRUE;
      }
      j++;
    }
    if ( match_found == FALSE )
    {
      break;
    }
    /* Reset Internal Loop */
    j = 0;
    i++;
  }

  if ( match_found == FALSE )
  {
    return GSDI_ERROR;
  }


  /* --------------------------------------------------------------------------
     Read the NS Codes from FS
     ------------------------------------------------------------------------*/
  fs_filename_p = array_of_code_files[GSDI_PERSO_NS];
  gsdi_status = gsdi_efs_read_file(fs_filename_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not Read FS: 0x%x Validating 0x%x",gsdi_status,feature,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Validate the returned_data_len_p from FS is properly divisable by
     the correct number:
     NS:  4
     ------------------------------------------------------------------------*/
  if ( *returned_data_len_p % 4 )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not read correct number of bytes: 0x%x",*returned_data_len_p,0,0);
    return GSDI_ERROR;
  }
  else
  {
    *returned_data_len_p = *returned_data_len_p / 4;
  }

  /* --------------------------------------------------------------------------
     data_p now contains the FS Contents of the NS Codes.  Check the data
     provided against this data
     ------------------------------------------------------------------------*/
  i = 0;
  while (i <  validate_num_bytes ||
         i == GSDI_MAX_DATA_BLOCK_LEN )
  {
    i_data = i * 8;
    match_found = FALSE;
    while ( j < *returned_data_len_p )
    {
      j_fs   = j * 4;

      /* Compare the memory */
      if ( !(memcmp(&validate_data_p[i_data],&data_p[j_fs],4)) )
      {
        match_found = TRUE;
      }
      j++;
    }
    if ( match_found == FALSE )
    {
      break;
    }
    /* Reset Internal Loop */
    j = 0;
    i++;
  }
  if ( match_found )
  {
    return GSDI_SUCCESS;
  }
  else
  {
    return GSDI_ERROR;
  }
}


/*===========================================================================
FUNCTION GSDI_PERSO_VALIDATE_DATA_AGAINST_FS

DESCRIPTION
  This function is used to validate the Date provided against the
  Codes stored in FS.  If a match is not found, then an error is returned.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_validate_data_against_fs(
  gsdi_perso_enum_type feature,
  char  *              fs_filename_p,
  int32                validate_num_bytes,
  uint8 *              validate_data_p,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_ERROR;
  int32          i             = 0;
  int32          j             = 0;
  int32          i_data        = 0;
  int32          data_offset   = 0;
  int32          fs_offset     = 0;
  int32          j_fs          = 0;
  int32          enc_data_len  = 0;
  boolean        match_found   = FALSE;

  /* --------------------------------------------------------------------------
     Perform Basic Parameter Checking
     ------------------------------------------------------------------------*/
  if ( fs_filename_p       == NULL ||
       validate_data_p     == NULL ||
       returned_data_len_p == NULL ||
       data_p              == NULL ||
       validate_num_bytes  >= GSDI_MAX_DATA_BLOCK_LEN)
  {
    return GSDI_INCORRECT_PARAMS;
  }

  switch ( feature )
  {
  case GSDI_PERSO_NS:
  case GSDI_PERSO_SP:
    data_offset = 4;
    fs_offset   = 3;
    break;

  case GSDI_PERSO_CP:
    data_offset = 5;
    fs_offset   = 4;
    break;

  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknonw Feature: %x",feature,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Validate validate_num_bytes is of the correct length.  The number
     of bytes must be properly divisable by:
     NS: 4 Bytes
     SP: 4 Bytes
     CP: 5 Bytes
     ------------------------------------------------------------------------*/
  if ( validate_num_bytes % data_offset )
  {
    return GSDI_INCORRECT_PARAMS;
  }
  else
  {
    validate_num_bytes = validate_num_bytes / data_offset;
  }

  /* --------------------------------------------------------------------------
     Read the NW Codes from FS
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_efs_read_file(fs_filename_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not Read FS: 0x%x Validating 0x%x",gsdi_status,feature,0);
    return GSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Validate the returned_data_len_p from FS is properly divisable by
     the correct number:
     NS:  3
     SP:  3
     CP:  4
     ------------------------------------------------------------------------*/
  if ( *returned_data_len_p % fs_offset )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not read correct number of bytes: 0x%x",*returned_data_len_p,0,0);
    return GSDI_ERROR;
  }
  else
  {
    /* Update the loop control */
    *returned_data_len_p = *returned_data_len_p / fs_offset;
  }


  /* --------------------------------------------------------------------------
     data_p now contains the FS Contents of the NW Codes.  Check the data
     provided against this data
     ------------------------------------------------------------------------*/
  while (i <  validate_num_bytes ||
         i == GSDI_MAX_DATA_BLOCK_LEN )
  {
    i_data = i * data_offset;

    match_found = FALSE;
    while ( j < *returned_data_len_p )
    {
      j_fs = j * fs_offset;

      /* Compare the memory */
      if ( !(memcmp(&validate_data_p[i_data],&data_p[j_fs],int32touint32(fs_offset))) )
      {
        match_found = TRUE;
      }
      j++;
    }
    if ( match_found == FALSE )
    {
      break;
    }
    /* Reset Internal Loop */
    j = 0;
    i++;
  }

  if ( match_found )
  {
    return GSDI_SUCCESS;
  }
  else
  {
    return GSDI_ERROR;
  }
}


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_VALIDATE_DATA

DESCRIPTION
  This function is used to validate the data provided against the data
  already stored in the File system.

  1) If the Data to personalize is NW, no additional checking is
     performed.
  2) If the Data to personalize is NS, then the NW Component of every item
     provided in the NS "string" is checked against the NW Codes stored
     in FS.
  3) If the Data to personalize is SP, then the NW Component of every item
     provied in the SP  "string" is checked against the NW Codes stored in FS.
     Every NS Component of the SP "string" is checked against the NS Codes
     stored in FS.
  4) If the Data to personalize is CP, then the NW Component of every item
     provied in the CP  "string" is checked against the NW Codes stored in FS.
     Every NS Component of the CP "string" is checked against the NS Codes
     stored in FS.
     Every SP Component of the CP "String" is checked against the SP Codes
     stored in FS.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
boolean gsdi_perso_validate_data(
  gsdi_perso_enum_type feature,
  int32                validate_num_bytes,
  uint8 *              validate_data_p,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_SUCCESS;
  boolean data_valid  = TRUE;
  char *         fs_filename_p = NULL;
  /* --------------------------------------------------------------------------
     Perform function parameter checks
     1.  filename_p or data_p can not be null;
     2.  num_bytes can not exceed GSDI_MAX_DATA_BLOCK_LEN
     ------------------------------------------------------------------------*/
  if ( data_p == NULL )
  {
    return FALSE;
  }

  if ( validate_num_bytes >= GSDI_MAX_DATA_BLOCK_LEN )
  {
    return FALSE;
  }

  switch ( feature )
  {
  case GSDI_PERSO_NW:
    break;

  case GSDI_PERSO_NS:
    fs_filename_p = array_of_code_files[GSDI_PERSO_NW_INDEX];
    gsdi_status   = gsdi_perso_validate_data_against_fs(GSDI_PERSO_NS,
                                                        fs_filename_p,
                                                        validate_num_bytes,
                                                        validate_data_p,
                                                        returned_data_len_p,
                                                       data_p);
    break;
  case GSDI_PERSO_SP:
    fs_filename_p = array_of_code_files[GSDI_PERSO_NW_INDEX];
    gsdi_status   = gsdi_perso_validate_data_against_fs(GSDI_PERSO_SP,
                                                        fs_filename_p,
                                                        validate_num_bytes,
                                                        validate_data_p,
                                                        returned_data_len_p,
                                                        data_p);

    break;

  case GSDI_PERSO_CP:
    fs_filename_p = array_of_code_files[GSDI_PERSO_SP_INDEX];
    gsdi_status   = gsdi_perso_validate_data_against_fs(GSDI_PERSO_CP,
                                                        fs_filename_p,
                                                        validate_num_bytes,
                                                        validate_data_p,
                                                        returned_data_len_p,
                                                        data_p);

    break;

  case GSDI_PERSO_SIM:
    gsdi_status  = gsdi_perso_validate_imsi_against_fs(GSDI_PERSO_SIM,
                                                       validate_num_bytes,
                                                       validate_data_p,
                                                       returned_data_len_p,
                                                       data_p);

    break;

  default:
    MMGSDI_DEBUG_ERR("Invalid Feature Received: %x",feature,0,0);
    return FALSE;
  }

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Validation Error encountered: Feature: %x",feature,0,0);
    data_valid = FALSE;
  }

  return data_valid;
}


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_SET_FEATURE_DATA

DESCRIPTION
  This function is used to retrieve the Feature Indicators from Persistent
  memory which is then reported back to the client.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_set_feature_data(
  gsdi_perso_set_feature_data_req_T * req_p,
  int32 *                           returned_data_len_p,
  uint8 *                           data_p
)
{
  gsdi_returns_T gsdi_status       = GSDI_SUCCESS;
  char *         file_name_data_p  = NULL;
  int32          encrypted_len     = 0;
  boolean        is_valid          = FALSE;
  /* --------------------------------------------------------------------------
     Perform basic parameter checking
     ------------------------------------------------------------------------*/
  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL req_p",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL returned_data_len_p",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  if ( data_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL data_p",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( req_p->num_bytes < 0 ||
       req_p->num_bytes > GSDI_MAX_DATA_BLOCK_LEN )
  {
    MMGSDI_DEBUG_MSG_ERROR("Bad num_bytes provided :%d",req_p->num_bytes,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Perm Disabled
     If Indicator is true, it is perm disabled and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                    returned_data_len_p,
                                                    data_p);
  /* --------------------------------------------------------------------------
     GSDI_SUCCESS             -> indication that the Feature is not
                                 Perm Disabled
     GSDI_PERSO_PERM_DISABLED -> At one point the feature was disabled
     ------------------------------------------------------------------------*/
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("Perm Disabled Feature: %x Status: %x",req_p->feature,gsdi_status,0);
    return GSDI_ERROR;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Activated
     If Indicator is true, it is activated and it is ok to proceed with the
     deactivation procedures.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                returned_data_len_p,
                                                data_p);
 /* --------------------------------------------------------------------------
     GSDI_SUCCESS                -> indication that the Feature is not currently
                                    active
     GSDI_PERSO_CURRENTLY_ACTIVE -> The feature is already active...can not
                                    activate it again.
     ------------------------------------------------------------------------*/
  if ( gsdi_status == GSDI_PERSO_CURRENTLY_ACTIVE )
  {
    MMGSDI_DEBUG_MSG_ERROR("Feature: %x Status: %x",req_p->feature,gsdi_status,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     OK to start saving Information
     - Get the filename to update for the Personalization Codes
     ------------------------------------------------------------------------*/
  switch ( req_p->feature )
  {
  case GSDI_PERSO_NW:
    file_name_data_p = array_of_code_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    file_name_data_p = array_of_code_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    file_name_data_p = array_of_code_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    file_name_data_p = array_of_code_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    file_name_data_p = array_of_code_files[GSDI_PERSO_SIM_INDEX];
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Personalization Feature Ind: %x",req_p->feature,0,0);
    return GSDI_ERROR;
  }

  is_valid = gsdi_perso_validate_data(req_p->feature,
                                      req_p->num_bytes,
                                      req_p->data_buffer,
                                      returned_data_len_p,
                                      data_p);
  if ( is_valid )
  {
    gsdi_perso_encrypt_data(req_p->num_bytes,
                            req_p->data_buffer,
                            &encrypted_len,
                            gsdi_encrypted_data_buffer);

    gsdi_status = gsdi_efs_write_file(file_name_data_p,
                                      encrypted_len,
                                      gsdi_encrypted_data_buffer);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("Update of Feature: %x, File %s Failed",
                req_p->feature,
                file_name_data_p,0);
      return GSDI_ERROR;
    }
  }
  else
  {
    MMGSDI_DEBUG_ERR("Data is not Subset of valid data",0,0,0);
    return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_PERSO_GET_OTA_DEPERSO_CK

DESCRIPTION
  This function is used to retrieve the depersonalization key based on the
  gsdi_perso_control_key_data_type and information provided.

  The User Data is provided by SMS and is not encoded.

  TS 23.040 [6] with the TP UD field coded as follows:
  Character Description
  1    40 Operator specific text padded with spaces to character 40.
  41   48 Network control key
  49   56 Network subset control key
  57   64 SP control key
  65   72 Corporate control key
  73   88 IMEI

  A Key of "FFFFFFFF" will indicate that personalization for this feature
  is not required.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T   gsdi_perso_get_ota_deperso_ck(
  gsdi_perso_ota_deperso_req_T     * req_p,
  gsdi_perso_control_key_data_type * ck_p )
{
  char * no_ck = "FFFFFFFF";
  int32 offset = 0x0000;

  if ( req_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null req_p provided",0,0,0);
    return GSDI_ERROR;
  }
  if ( ck_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("Null ck_p provided",0,0,0);
    return GSDI_ERROR;
  }

  switch ( ck_p->feature )
  {
  case GSDI_PERSO_NW:
    offset = GSDI_PERSO_OTA_DEPERSO_NW_CK_OFFSET;
    break;
  case GSDI_PERSO_NS:
    offset = GSDI_PERSO_OTA_DEPERSO_NS_CK_OFFSET;
    break;
  case GSDI_PERSO_SP:
    offset = GSDI_PERSO_OTA_DEPERSO_SP_CK_OFFSET;
    break;
  case GSDI_PERSO_CP:
    offset = GSDI_PERSO_OTA_DEPERSO_CP_CK_OFFSET;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("Unknown Feature: %x",ck_p->feature,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Check to see if the Control Key is set to the Default of "FFFFFFFF"
     which indicates no Control Key was provided
     If Default, return 0 bytes for the number of OTA Deperso CK Bytes
     ------------------------------------------------------------------------*/
  if (! (memcmp(&req_p->data_buffer[offset],
         (uint8 *)no_ck,
         GSDI_PERSO_OTA_DEPERSO_CK_LEN ) ) )
  {
    ck_p->num_bytes = 0x00;
  }
  else
  {
    ck_p->num_bytes     = (int32)GSDI_PERSO_OTA_DEPERSO_CK_LEN;
    ck_p->control_key_p = &req_p->data_buffer[offset];
  }

  return GSDI_SUCCESS;
}

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_OTA_DEPERSO

DESCRIPTION
  This function is used to perform the OTA Depersonalization Procedures as
  required to do during an SMS PP OTA Depersonalization Message

  The generated result is packed into the data_p buffer provied for Delivery
  back to the network.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_ota_deperso(
  gsdi_perso_ota_deperso_req_T * req_p,
  int32 *                        returned_data_len_p,
  uint8 *                        data_p
)
{
  gsdi_returns_T                     gsdi_status = GSDI_SUCCESS;
  gsdi_perso_deact_feature_ind_req_T deperso_req;
  gsdi_perso_control_key_data_type   control_key;
  nv_item_type                       gsdi_nv_data_buf; /* NV data buffer */
  nv_stat_enum_type                  read_status;      /* NV read status */
  uint8                              ue_imei[GSDI_PERSO_MAX_IMEI_LEN] = {0};
  uint8                              sms_status[GSDI_PERSO_MAX_SMS_STATUS_LEN];
  uint8                              dec_index = 0;
  uint8                              bcd_index = 1;
  uint32                             feature   = 0x00;
  uint8                              offset    = 0x00;

  /* ------------------------------------------------------------------------
     Is the IMEI Provided a valid IMEI
     ----------------------------------------------------------------------*/
  read_status = gsdi_get_nv(NV_UE_IMEI_I,
                            &gsdi_nv_data_buf);

  if ( read_status != NV_DONE_S )
  {
    MMGSDI_DEBUG_MSG_ERROR("Could not get IMEI",0,0,0);
    return GSDI_ERROR;
  }
  if ( gsdi_nv_data_buf.ue_imei.ue_imei[0] == 0x00 ||
       gsdi_nv_data_buf.ue_imei.ue_imei[0] == 0xFF ||
       gsdi_nv_data_buf.ue_imei.ue_imei[0] > GSDI_PERSO_MAX_IMEI_LEN )
  {
    MMGSDI_DEBUG_MSG_ERROR("IMEI is not set or improperly set",0,0,0);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Convert the IMEI from BCD To Decimal
     ------------------------------------------------------------------------*/
  while ( bcd_index <= gsdi_nv_data_buf.ue_imei.ue_imei[0])
  {
    /* Place High Byte in Index 0 (Even) Low Byte in Index 1 (Odd) ... */
    if ( dec_index % 2 == 0 )
    {
      ue_imei[dec_index] = 0x30 +
            (((gsdi_nv_data_buf.ue_imei.ue_imei[bcd_index] & GSDI_MASK_FOR_HIGH_BYTE) >> 0x04) & 0x0F);
      dec_index++;
      bcd_index++;
    }
    else
    {
      /* Get the Low Byte of BCD  0x0N */
      ue_imei[dec_index] = 0x30 + (gsdi_nv_data_buf.ue_imei.ue_imei[bcd_index] & GSDI_MASK_FOR_LOW_BYTE);
      dec_index++;
    }
  }
  /* --------------------------------------------------------------------------
     Compare the IMEIs to see if they match...otherwise indicate an IMEI
     mismatch to the caller
     ------------------------------------------------------------------------*/
  if (memcmp(ue_imei,&req_p->data_buffer[GSDI_PERSO_OTA_DEPERSO_IMEI_OFFSET],GSDI_PERSO_MAX_IMEI_LEN))
  {
    /* IMEI Mismatch
       data_p buffer information:
       data_p[0:15] IMEI
       data_p[16]   NW Status
       data_p[17]   NS Status
       data_p[18]   SP Status
       data_p[19]   CP Status
    */
    MMGSDI_DEBUG_MSG_ERROR("IMEI Does not match ",0,0,0);
    memcpy(data_p,
           ue_imei,
           GSDI_PERSO_MAX_IMEI_LEN);

    data_p[GSDI_PERSO_OTA_DEPERSO_RSP_NW_OFFSET] = (uint8)GSDI_PERSO_IMEI_MISMATCH;
    data_p[GSDI_PERSO_OTA_DEPERSO_RSP_NS_OFFSET] = (uint8)GSDI_PERSO_IMEI_MISMATCH;
    data_p[GSDI_PERSO_OTA_DEPERSO_RSP_SP_OFFSET] = (uint8)GSDI_PERSO_IMEI_MISMATCH;
    data_p[GSDI_PERSO_OTA_DEPERSO_RSP_CP_OFFSET] = (uint8)GSDI_PERSO_IMEI_MISMATCH;

    return GSDI_SUCCESS;
  }
  /* --------------------------------------------------------------------------
     Go through the Loop as many times as there are keys (not to exceed 4
     iterations)  to do the following:
     1.  Get the Control Key Data
     2.  Check to see if it is Perm Disabled
     3.  Check to see if it is active...if active, deactivate it
     4.  Now attempt to deactivate it
     5.  Report Success/Failure
     ------------------------------------------------------------------------*/
  /* --------------------------------------------------------------------------
     Copy ME IMEI to SMS Status
     ------------------------------------------------------------------------*/
  memcpy(sms_status,
         ue_imei,
         GSDI_PERSO_MAX_IMEI_LEN);


  /* --------------------------------------------------------------------------
     Per the 22.022 specifications and 2.22 specifications, it is required that
     the ME Software set the 16th Byte (Index) 15 to 0 since it is ignored
     due to the IMEI consisting of 15 bytes (Indexes 0 - 14).
     ------------------------------------------------------------------------*/
  sms_status[15] = 0x30;

  while ( feature < 0x04 )
  {
    control_key.feature = (gsdi_perso_enum_type)feature;
    switch ((gsdi_perso_enum_type)feature )
    {
    case GSDI_PERSO_NW:
      offset = GSDI_PERSO_OTA_DEPERSO_RSP_NW_OFFSET;
      break;
    case GSDI_PERSO_NS:
      offset = GSDI_PERSO_OTA_DEPERSO_RSP_NS_OFFSET;
      break;
    case GSDI_PERSO_SP:
      offset = GSDI_PERSO_OTA_DEPERSO_RSP_SP_OFFSET;
      break;
    case GSDI_PERSO_CP:
      offset = GSDI_PERSO_OTA_DEPERSO_RSP_CP_OFFSET;
      break;
    default:
      MMGSDI_DEBUG_MSG_ERROR("Bad Perso ID: %x",(gsdi_perso_enum_type)feature,0,0);
      break;
    }
    gsdi_status = gsdi_perso_get_ota_deperso_ck(req_p,&control_key);

    if ( gsdi_status == GSDI_SUCCESS )
    {
      /* Check to see if Perm Disabaled */
      gsdi_status = gsdi_perso_is_feature_perm_disabled((gsdi_perso_enum_type)feature,
                                                        returned_data_len_p,
                                                        data_p);
      if ( gsdi_status != GSDI_PERSO_PERM_DISABLED &&
           gsdi_status != GSDI_ERROR )
      {
        /* Check to see if Feature is active */
        gsdi_status = gsdi_perso_is_feature_activated((gsdi_perso_enum_type)feature,
                                                       returned_data_len_p,
                                                       data_p);
        if ( gsdi_status == GSDI_PERSO_CURRENTLY_ACTIVE )
        {
          if ( control_key.num_bytes != 0 )
          {
            /* build the Depersonalization Request */
            deperso_req.message_header.slot = GSDI_SLOT_1;
            deperso_req.feature    = (gsdi_perso_enum_type)feature;
            deperso_req.num_bytes  = control_key.num_bytes;
            memcpy(deperso_req.ck_buffer,
                   control_key.control_key_p,
                   int32touint32(control_key.num_bytes));
            gsdi_status = gsdi_proc_perso_deactivate_feature_ind(&deperso_req,
                                                                 returned_data_len_p,
                                                                 data_p);
            if ( gsdi_status == GSDI_SUCCESS )
            {
              MSG_HIGH("Depersonalized Feature: %x",control_key.feature,0,0);
              sms_status[offset] = (uint8)GSDI_PERSO_CURRENTLY_NOT_PERSO;
            }
            else
            {
              MSG_HIGH("CK Verification Failed for Feature: %x",control_key.feature,0,0);
              sms_status[offset] = (uint8)GSDI_PERSO_PERSONALIZED;
            }
          }
          else
          {
            MSG_HIGH("NO CK Present for Feature: %x",control_key.feature,0,0);
            sms_status[offset] = (uint8)GSDI_PERSO_PERSONALIZED;
          }
        }
        else
        {
          MSG_HIGH("Perso Feature: %x Is Not Active, can't deperso",control_key.feature,0,0);
          sms_status[offset] = (uint8)GSDI_PERSO_CURRENTLY_NOT_PERSO;
        }
      }
      else
      {
        /* This feature was perm disabled...report back as is */
        sms_status[offset] = (uint8)GSDI_PERSO_PERMANENTLY_NOT_PERS;
      }
    }
    else
    {
      MMGSDI_DEBUG_ERR("Error getting Control Keys for SMS OTA MSG",0,0,0);
    }
    /* Incriment the Loop Control */
    feature++;
  } /* While Loop */

  /* --------------------------------------------------------------------------
     We now have an SMS Status, and SMS Status Length that needs to be
     returned to SMS for an ACK back to the network.

     Overwrite data_p and set the returned_data_len_p accordingly
     ------------------------------------------------------------------------*/
  memcpy(data_p,
         sms_status,
         GSDI_PERSO_OTA_DEPERSO_RSP_LEN);

  *returned_data_len_p = GSDI_PERSO_OTA_DEPERSO_RSP_LEN;

  return GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_PERSO_IS_SELF_PERSO_REQUIRED

DESCRIPTION
  This function will be called for the purpose of checking the feature against
  the different feature indicators (perm disabled, active/inactive indicators)

  This function is specific to the sole purpose of checking to see if self
  perso is required.

  INPUT:
  int32 *   returned_data_len_p -> Point to structure used to store lenghts
  uint8 *   data_p -> Pointer to storage spaced used in procedures

  OUTPUT:
  boolean * self_perso -> TRUE or FALSE Value used to let caller know whether
                          or not self personalization is required.
                          TRUE:  Feature is not Perm Disabled & Feature is active
                                 & the codes file size = 0.
                          FALSE: If any of the true conditions is false

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T  GSDI_SUCCESS: If procedures are performed correctly
                  GSDI_ERROR:   If procedures fail at GSDI or FS levels.
===========================================================================*/
gsdi_returns_T gsdi_perso_is_self_perso_required(
  gsdi_perso_enum_type  feature,
  int32 *               returned_data_len_p,
  uint8 *               data_p,
  boolean *             self_perso
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  char *         filename_p  = NULL;
  /* ------------------------------------------------------------------------
     Perform Parameter checking
     ----------------------------------------------------------------------*/
  if (returned_data_len_p == NULL ||
      data_p              == NULL ||
      self_perso          == NULL )
  {
    return GSDI_INCORRECT_PARAMS;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Perm Disabled
     If Indicator is true, it is perm disabled and the activation
     procedure must be terminated.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_perm_disabled(feature,
                                                    returned_data_len_p,
                                                    data_p);
  /* --------------------------------------------------------------------------
     GSDI_SUCCESS             -> indication that the Feature is not
                                 Perm Disabled
     GSDI_PERSO_PERM_DISABLED -> At one point the feature was disabled
     ------------------------------------------------------------------------*/
  switch (gsdi_status)
  {
  case GSDI_PERSO_PERM_DISABLED:
    /* The Feature is perm disabled, we can not self personalize */
    *self_perso = FALSE;
    return GSDI_SUCCESS;

  case GSDI_SUCCESS:
    /* The Feature is not Perm Disabled and we can continue with self perso*/
    break;

  default:
    /* An error, GSDI or FS was encountered abort procedures and to to
    ** the error handling state
    */
    return gsdi_status;
  }

  /* ------------------------------------------------------------------------
     Determine whether or not the Feature is Activated
     If Indicator is true, it is activated and it is ok to proceed with the
     deactivation procedures.
     ----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_is_feature_activated(feature,
                                                returned_data_len_p,
                                                data_p);
 /* --------------------------------------------------------------------------
     GSDI_SUCCESS                -> indication that the Feature is not currently
                                    active
     GSDI_PERSO_CURRENTLY_ACTIVE -> The feature is already active...can not
                                    activate it again.
     ------------------------------------------------------------------------*/
  switch ( gsdi_status )
  {
  case GSDI_PERSO_CURRENTLY_DEACT:
    /* The feature is not active, we must stop the self personalization prcs */
    *self_perso = FALSE;
    return GSDI_SUCCESS;

  case GSDI_PERSO_CURRENTLY_ACTIVE:
    /* The feature is active, we can proceed with the procedures*/
    gsdi_status = GSDI_SUCCESS;
    break;

  default:
    /* GSDI or FS Error occurred */
    *self_perso = FALSE;
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Now check whether or not the file has been Created and that it contains
     a file size of > 0.  If > 0, this means that it has already been
     personalized.
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    filename_p = array_of_code_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    filename_p = array_of_code_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    filename_p = array_of_code_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    filename_p = array_of_code_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    filename_p = array_of_code_files[GSDI_PERSO_SIM_INDEX];
    break;
  case GSDI_PERSO_NONE:
  case GSDI_PERSO_PROP1:
    break;
  }
  gsdi_status = gsdi_fs_get_file_size(filename_p,
                                      returned_data_len_p);
  if ( gsdi_status == GSDI_SUCCESS )
  {
    if ( *returned_data_len_p == 0 )
    {
      /* Self Personalization can continue */
      *self_perso = TRUE;
    }
    else
    {
      /* Self Personalization can not continue */
      *self_perso = FALSE;
    }
  }
  else
  {
    /* An error occurred...GSDI or FS */
    *self_perso = FALSE;
  }
  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SELF_PERSO_ACTIVATE_STATE_PROCEDURES

DESCRIPTION
  This function will be called to perform the Self Personalization Activation
  procedures.

  It will be used to get the depersonalization control key from FS to Activate
  the feature so that it can be appropriately updated and used at a later time.

  INPUT:
  gsdi_perso_enum_type feature:  Used to determine which control keys to
                                 retrieve.
  int32 *  returned_data_len_p:  Used to store any lenghts returned by any
                                 functions.
  uint8 *  data_p:               Buffer to location to hold contents of FS

  OUTPUT:
  None

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:          Procedures executed properly
                   GSDI_INCORRECT_PARMS:  Bad parameters were received
                   GSDI_ERROR:            Function failed due to GSDI
                                          Procedures or FS procedures.
===========================================================================*/
gsdi_returns_T gsdi_perso_self_perso_activate_state_procedures(
  gsdi_perso_enum_type feature,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_perso_act_feature_ind_req_T act_req;
  gsdi_returns_T                   gsdi_status  = GSDI_SUCCESS;
  char *                           filename_p   = NULL;
  int32                            enc_data_len = 0;
  /* --------------------------------------------------------------------------
     Perform basic parameter checking
     1.  returned_data_len_p can not be NULL
     2.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Start building a Deactive Request
     ------------------------------------------------------------------------*/
  act_req.message_header.message_set      = 13;
  act_req.message_header.slot             = GSDI_SLOT_1;
  act_req.message_header.message_id       = GSDI_PERSO_ACT_FEATURE_IND_REQ;
  act_req.message_header.message_len_msb  =
    (byte)((sizeof(gsdi_perso_act_feature_ind_req_T )- sizeof(gsdi_cmdhdr_T))/0x100);
  act_req.message_header.message_len_lsb  =
    (byte)((sizeof(gsdi_perso_act_feature_ind_req_T) - sizeof(gsdi_cmdhdr_T))%0x100);
  act_req.feature = feature;
  act_req.message_header.gsdi_async_callback = gsdi_perso_self_perso_activate_state_procedures_cb;
  /* --------------------------------------------------------------------------
     Now use an FS Function to retrieve the control keys.
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    filename_p = array_of_ck_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    filename_p = array_of_ck_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    filename_p = array_of_ck_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    filename_p = array_of_ck_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    filename_p = array_of_ck_files[GSDI_PERSO_SIM_INDEX];
    break;
  case GSDI_PERSO_NONE:
  case GSDI_PERSO_PROP1:
    break;
  }
  gsdi_status = gsdi_efs_read_file(filename_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          act_req.ck_buffer);

  if ( gsdi_status == GSDI_SUCCESS )
  {
    act_req.num_bytes = *returned_data_len_p;
    /* Continue by sending the request for GSDI to process further */
    gsdi_status = gsdi_cmd_ext((void *)&act_req);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }
  }

  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_SELF_PERSO_DEACTIVATE_STATE_PROCEDURES

DESCRIPTION
  This function will be called to perform the Self Personalization Deactivation
  procedures.

  It will be used to get the depersonalization control key from FS to deactivate
  the feature so that it can be appropriately updated and activated at a later
  time.

  INPUT:
  gsdi_perso_enum_type feature:  Used to determine which control keys to
                                 retrieve.
  int32 *  returned_data_len_p:  Used to store any lenghts returned by any
                                 functions.
  uint8 *  data_p:               Buffer to location to hold contents of FS

  OUTPUT:
  None

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS:          Procedures executed properly
                   GSDI_INCORRECT_PARMS:  Bad parameters were received
                   GSDI_ERROR:            Function failed due to GSDI
                                          Procedures or FS procedures.
===========================================================================*/
gsdi_returns_T gsdi_perso_self_perso_deactivate_state_procedures(
  gsdi_perso_enum_type feature,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_perso_deact_feature_ind_req_T deact_req;
  gsdi_returns_T                     gsdi_status  = GSDI_SUCCESS;
  char *                             filename_p   = NULL;
  int32                              enc_data_len = 0;
  /* --------------------------------------------------------------------------
     Perform basic parameter checking
     1.  returned_data_len_p can not be NULL
     2.  data_p can not be NULL
     ------------------------------------------------------------------------*/
  if ( returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    return GSDI_INCORRECT_PARAMS;
  }
  /* --------------------------------------------------------------------------
     Start building a Deactive Request
     ------------------------------------------------------------------------*/
  deact_req.message_header.message_set      = 13;
  deact_req.message_header.slot             = GSDI_SLOT_1;
  deact_req.message_header.message_id       = GSDI_PERSO_DEACT_FEATURE_IND_REQ;
  deact_req.message_header.message_len_msb  =
    (byte)((sizeof(gsdi_perso_deact_feature_ind_req_T )- sizeof(gsdi_cmdhdr_T))/0x100);
  deact_req.message_header.message_len_lsb  =
    (byte)((sizeof(gsdi_perso_deact_feature_ind_req_T) - sizeof(gsdi_cmdhdr_T))%0x100);
  deact_req.feature = feature;
  deact_req.message_header.gsdi_async_callback = gsdi_perso_self_perso_deactivate_state_procedures_cb;
  /* --------------------------------------------------------------------------
     Now use an FS Function to retrieve the control keys.
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    filename_p = array_of_ck_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    filename_p = array_of_ck_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    filename_p = array_of_ck_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    filename_p = array_of_ck_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    filename_p = array_of_ck_files[GSDI_PERSO_SIM_INDEX];
    break;
  case GSDI_PERSO_NONE:
  case GSDI_PERSO_PROP1:
    break;
  }
  gsdi_status = gsdi_efs_read_file(filename_p,
                                   &enc_data_len,
                                   gsdi_encrypted_data_buffer);

  gsdi_perso_decrypt_data(enc_data_len,
                          gsdi_encrypted_data_buffer,
                          returned_data_len_p,
                          deact_req.ck_buffer);

  if ( gsdi_status == GSDI_SUCCESS )
  {
    deact_req.num_bytes = *returned_data_len_p;
    /* Continue by sending the request for GSDI to process further */
    gsdi_status = gsdi_cmd_ext((void *)&deact_req);
    if(gsdi_status != GSDI_SUCCESS)
    {
      return GSDI_ERROR;
    }
  }

  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PERSO_PARSE_DATA_AND_BUILD_PERSO_DATA

DESCRIPTION
  This function will used the imsi buffer or raw cnl data read to generate
  the codes for the feature provided in the function.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void   gsdi_perso_parse_data_and_build_perso_data(
  gsdi_perso_enum_type feature,
  boolean              use_imsi_buffer,
  int32 *              data_len_p,
  uint8 *              data_p,
  int32 *              formatted_data_len_p,
  uint8 *              formatted_data_p
)
{
  gsdi_sim_read_req_T * read_req_p       = NULL;
  gsdi_returns_T        gsdi_status      = GSDI_SUCCESS;
  uint8 *               gid1_data_p      = NULL;
  uint8 *               gid2_data_p      = NULL;
  int32                 index            = 0;
  int32                 cp_index         = 0;
  int32                 perso_data_index = 0;
  byte                  dummy1           = 0;
  byte                  dummy2           = 0;
  byte                  gid1_data_len    = 0;
  byte                  gid2_data_len    = 0;
  /* --------------------------------------------------------------------------
     BASIC Parameter Checking
     None of the pointered provided can be NULL
     ------------------------------------------------------------------------*/
  if ( data_len_p           == NULL ||
       data_p               == NULL ||
       formatted_data_len_p == NULL ||
       formatted_data_p     == NULL )
  {
    ERR_FATAL("No recovery for NULL Parameters",0,0,0);
  }

  if ( use_imsi_buffer )
  {
    switch ( feature )
    {
    case GSDI_PERSO_NW:
      *formatted_data_len_p = 0x03;
      memcpy(formatted_data_p,imsi_buffer,0x03);
      break;
    case GSDI_PERSO_NS:
      *formatted_data_len_p = 0x04;
      memcpy(formatted_data_p,imsi_buffer,0x04);
      break;
    case GSDI_PERSO_SP:
      read_req_p = mmgsdi_malloc( sizeof(gsdi_sim_read_req_T));
      if ( read_req_p == NULL )
      {
        ERR_FATAL("Get Read Request Memory ",0,0,0);
      }
      else
      {
        memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));
      }
      read_req_p->index_mode        = GSDI_NO_INDEX;
      read_req_p->search_string_len = 0;
      read_req_p->data_offset       = 0;
      gid1_data_p = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
      if ( gid1_data_p == NULL )
      {
        ERR_FATAL("Get Read Request Memory ",0,0,0);
      }
      else
      {
        memset(gid1_data_p,0x00,GSDI_MAX_DATA_BLOCK_LEN);
      }

      if ( gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK )
      {
        read_req_p->sim_filename = (word)UIM_USIM_GID1;
        gsdi_status = gsdi_umts_proc_usim_read(read_req_p,
                                               &dummy1,
                                               &dummy2,
                                               &gid1_data_len,
                                               (byte*)gid1_data_p);
      }
      else
      {
        read_req_p->sim_filename = (word)UIM_GSM_GID1;
        gsdi_status = gsdi_gsm_proc_sim_read(read_req_p,
                                             &dummy1,
                                             &dummy2,
                                             &gid1_data_len,
                                             (byte*)gid1_data_p);
      }
      if ( gsdi_status != GSDI_SUCCESS ||
           gid1_data_len == 0          ||
           gid1_data_p[0] == 0x00      ||
           gid1_data_p[0] == 0xFF)
      {
        /* Free up any memory allocated */
        MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
        MMGSDIUTIL_TMC_MEM_FREE(gid1_data_p);
        *formatted_data_len_p = 0;
        return;
      }
      while ( index < gid1_data_len &&
              ( gid1_data_p[index] != 0xFF ||
                gid1_data_p[index] != 0x00 ))
      {
        memcpy((formatted_data_p + (index * 4)),
               imsi_buffer,
               0x03);
        formatted_data_p[(index + 1)* 4 - 1 ] =( ( ( gid1_data_p[index] << 4 ) & 0xF0 ) |
                                                 ( ( gid1_data_p[index] >> 4 ) & 0x0F ) );
        index++;
      }
      *formatted_data_len_p = index * 4;
      break;
    case GSDI_PERSO_CP:
      read_req_p = mmgsdi_malloc( sizeof(gsdi_sim_read_req_T));
      if ( read_req_p == NULL )
      {
        ERR_FATAL("Get Read Request Memory ",0,0,0);
      }
      else
      {
        memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));
      }
      read_req_p->index_mode        = GSDI_NO_INDEX;
      read_req_p->search_string_len = 0;
      read_req_p->data_offset       = 0;
      gid1_data_p = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
      if ( gid1_data_p == NULL )
      {
        ERR_FATAL("Get Read Request Memory ",0,0,0);
      }
      else
      {
        memset(gid1_data_p,0x00,GSDI_MAX_DATA_BLOCK_LEN);
      }

      if ( gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK )
      {
        read_req_p->sim_filename = (word)UIM_USIM_GID1;
        gsdi_status = gsdi_umts_proc_usim_read(read_req_p,
                                               &dummy1,
                                               &dummy2,
                                               &gid1_data_len,
                                               (byte*)gid1_data_p);
      }
      else
      {
        read_req_p->sim_filename = (word)UIM_GSM_GID1;
        gsdi_status = gsdi_gsm_proc_sim_read(read_req_p,
                                             &dummy1,
                                             &dummy2,
                                             &gid1_data_len,
                                             (byte*)gid1_data_p);
      }
      if ( gsdi_status != GSDI_SUCCESS ||
           gid1_data_len == 0          ||
           gid1_data_p[0] == 0x00      ||
           gid1_data_p[0] == 0xFF)
      {
        /* Free up any memory allocated */
        MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
        MMGSDIUTIL_TMC_MEM_FREE(gid1_data_p);
        *formatted_data_len_p = 0;
        return;
      }
      gid2_data_p = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
      if ( gid2_data_p == NULL )
      {
        ERR_FATAL("Get Read Request Memory ",0,0,0);
      }
      else
      {
        memset(gid2_data_p,0x00,GSDI_MAX_DATA_BLOCK_LEN);
      }

      if ( gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK )
      {
        read_req_p->sim_filename = (word)UIM_USIM_GID2;
        gsdi_status = gsdi_umts_proc_usim_read(read_req_p,
                                               &dummy1,
                                               &dummy2,
                                               &gid2_data_len,
                                               (byte*)gid2_data_p);
      }
      else
      {
        read_req_p->sim_filename = (word)UIM_GSM_GID2;
        gsdi_status = gsdi_gsm_proc_sim_read(read_req_p,
                                             &dummy1,
                                             &dummy2,
                                             &gid2_data_len,
                                             (byte*)gid2_data_p);
      }
      if ( gsdi_status != GSDI_SUCCESS ||
           gid2_data_len == 0          ||
           gid2_data_p[0] == 0x00      ||
           gid2_data_p[0] == 0xFF)
      {
        /* Free up any memory allocated */
        MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
        MMGSDIUTIL_TMC_MEM_FREE(gid1_data_p);
        MMGSDIUTIL_TMC_MEM_FREE(gid2_data_p);
        *formatted_data_len_p = 0;
        return;
      }
      while ( cp_index < gid2_data_len &&
             (gid2_data_p[cp_index] != 0xFF ||
              gid2_data_p[cp_index] != 0x00))
      {
        while ( index < gid1_data_len &&
               (gid1_data_p[index] != 0xFF ||
                gid1_data_p[index] != 0x00 ))
        {
          /* Should check index length to ensure we don't overshoot our
          ** formatted_data_p Len
          */
          memcpy((formatted_data_p + (index * 5)),
                imsi_buffer,
                0x03);
          formatted_data_p[(index + 1)* 5 - 2 ] =( ( ( gid1_data_p[index] << 4 ) & 0xF0 ) |
                                                   ( ( gid1_data_p[index] >> 4 ) & 0x0F ) );
          formatted_data_p[(index + 1)* 5 - 1 ] =( ( ( gid2_data_p[index] << 4 ) & 0xF0 ) |
                                                   ( ( gid2_data_p[index] >> 4 ) & 0x0F ) );
          index++;
        }
        cp_index++;
        /* reset the sp_index */
        index = 0;
      }
      *formatted_data_len_p = cp_index * 5;
      break;
    case GSDI_PERSO_SIM:
      *formatted_data_len_p = 0x08;
      memcpy(formatted_data_p,imsi_buffer,0x08);
      break;
    default:
      ERR_FATAL("No recovery for bad feature",0,0,0);
      break;
    }
  }
  else
  {
    switch ( feature )
    {
    case GSDI_PERSO_NW:
      while ( index < *data_len_p )
      {
        if ( data_p[index] != 0xFF ||
             data_p[index] != 0x00 )
        {
          /* Memcopy the first set of Network Codes from the CNL Data */
          /* only if it is provisioned properly                       */
          memcpy((formatted_data_p + perso_data_index),
                 (data_p + index),
                  0x03);
          /* Increment the perso_data_index by 3 to move to the next
          ** NW Code Data Block of 3 Bytes
          */
          perso_data_index += 3;
        }

        /* Increment the index by 6 bytes every time to move the pointer
        ** down each Set of CNL Data Blocks
        */
        index += 6;
      }
      *formatted_data_len_p = perso_data_index;
      break;
    case GSDI_PERSO_NS:
      while ( index < *data_len_p )
      {
        if ( data_p[index] != 0xFF ||
             data_p[index] != 0x00 )
        {
          /* Memcopy the first set of Network Codes from the CNL Data */
          memcpy((formatted_data_p + perso_data_index),
                 (data_p + index),
                  0x04);

          /* Increment the perso_data_index by 3 to move to the next
          ** NW Code Data Block of 3 Bytes
          */
          perso_data_index += 4;
        }
        /* Increment the index by 6 bytes every time to move the pointer
        ** down each Set of CNL Data Blocks
        */
        index += 6;
      }
      *formatted_data_len_p = perso_data_index;
      break;
    case GSDI_PERSO_SP:
      while ( index < *data_len_p )
      {
        if ( data_p[index] != 0xFF ||
             data_p[index] != 0x00 )
        {
          /* Memcopy the first set of Network Codes from the CNL Data */
          memcpy(&formatted_data_p[perso_data_index],
                 &data_p[index],
                 0x03);

          formatted_data_p[perso_data_index + 3] = data_p[index + 4];
          /* Increment the perso_data_index by 3 to move to the next
          ** NW Code Data Block of 3 Bytes
          */
          perso_data_index += 4;
        }
        /* Increment the index by 6 bytes every time to move the pointer
        ** down each Set of CNL Data Blocks
        */
        index += 6;
      }
      *formatted_data_len_p = perso_data_index;
      break;
    case GSDI_PERSO_CP:
      while ( index < *data_len_p )
      {
        if ( data_p[index] != 0xFF ||
             data_p[index] != 0x00 )
        {
          /* Memcopy the first set of Network Codes from the CNL Data */
          memcpy(&formatted_data_p[perso_data_index],
                 &data_p[index],
                  0x03);
          /* Two bytes of additional Data need special formatting
          ** before proceeding on
          */
          formatted_data_p[perso_data_index + 3] = data_p[index + 4];
          formatted_data_p[perso_data_index + 4] = data_p[index + 5];

          /* Increment the perso_data_index by 6 to move to the next
          ** NW Code Data Block of 3 Bytes
          */
          perso_data_index += 5;
        }
        /* Increment the index by 6 bytes every time to move the pointer
        ** down each Set of CNL Data Blocks
        */
        index += 6;
      }
      *formatted_data_len_p = perso_data_index;
      break;
    case GSDI_PERSO_SIM:
      *formatted_data_len_p = 0x08;
      memcpy(formatted_data_p,imsi_buffer,0x08);
      break;
    default:
      ERR_FATAL("No recovery for bad feature",0,0,0);
      break;
    }
  }

   /* Free up any memory allocated */
  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);

  MMGSDIUTIL_TMC_MEM_FREE(gid1_data_p);

  MMGSDIUTIL_TMC_MEM_FREE(gid2_data_p);

  return;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SAVE_SIM_PERSO_INFO_TO_FS

DESCRIPTION
  This function will read the SIM Information and store it to the SIM/USIM.

  It will attempt to read the Co-operative Network List as stored in the SIM
  and generate the appropriate codes for the feature provided into this
  function.

  If the Co-operative network list is not found, the IMSI obtained during
  the Read and Format Function is used instead.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_save_sim_perso_info_to_fs (
  gsdi_perso_enum_type feature,
  int32 *              returned_data_len_p,
  uint8 *              data_p
)
{
  gsdi_sim_read_req_T * read_req_p       = NULL;
  gsdi_returns_T        gsdi_status      = GSDI_SUCCESS;
  byte                  dummy1           = 0;     /* not used in this f()     */
  byte                  dummy2           = 0;     /* not used in this f()     */
  int32                 perso_data_len   = 0;
  uint8 *               perso_data_p     = NULL;
  boolean               use_imsi_buffer  = FALSE;
  char *                filename_p       = NULL;
  int32                 encrypted_len    = 0;
  /* Allocate Buffer for storage of information to FS */
  perso_data_p = mmgsdi_malloc(GSDI_MAX_DATA_BLOCK_LEN);
  if ( perso_data_p == NULL )
    ERR_FATAL("Could not get memory for perso_data_p",0,0,0);

  memset(perso_data_p,0x00,GSDI_MAX_DATA_BLOCK_LEN);
  /* --------------------------------------------------------------------------
     Read Information from the SIM/USIM
     ------------------------------------------------------------------------*/
  read_req_p = mmgsdi_malloc( sizeof(gsdi_sim_read_req_T));
  if ( read_req_p == NULL )
    ERR_FATAL("Could not get memory for read access",0,0,0);

  memset(read_req_p,0x00,sizeof(gsdi_sim_read_req_T));

  read_req_p->index_mode        = GSDI_NO_INDEX;
  read_req_p->rec_num           = 0;
  read_req_p->search_string_len = 0;
  if ( gsdi_data.card_applications.apps_enabled & GSDI_USIM_APP_MASK )
  {
    read_req_p->sim_filename = (word)UIM_USIM_CNL;
    gsdi_status = gsdi_umts_proc_usim_read(read_req_p,
                                           &dummy1,
                                           &dummy2,
                                           (byte *)returned_data_len_p,
                                           data_p);
  }
  else
  {
    read_req_p->sim_filename = (word)UIM_GSM_CNL;
    gsdi_status = gsdi_gsm_proc_sim_read(read_req_p,
                                         &dummy1,
                                         &dummy2,
                                         (byte *)returned_data_len_p,
                                         data_p);
  }
  switch ( gsdi_status )
  {
  case GSDI_NOT_FOUND:
    /* The file is not found, the Mobile can only self personalize
    ** based on IMSI Data
    */
    use_imsi_buffer      = TRUE;
    data_p               = imsi_buffer;
    *returned_data_len_p = 0x04;
    /* Ok to allocate memory for the perso_data_buffer */
    break;
  case GSDI_SUCCESS:
    /* The file is found, the Mobile can personalize to CNL if
    ** it is properly set
    */

    /* First quickly check the CNL Data to see if it is properly provisioned */
    if (data_p[0] == 0x00 ||
        data_p[0] == 0xFF )
    {
      use_imsi_buffer = TRUE;
    }
    else
    {
      use_imsi_buffer = FALSE;
    }
    break;
  default:
    /* A GSDI Error was encountered for which we can not
    ** handle.  Return and free any memory allocated
    */
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
    MMGSDIUTIL_TMC_MEM_FREE(perso_data_p);
    return gsdi_status;
  }

  /* ------------------------------------------------------------------------
     Generate the personalization codes based on the Feature provided
     ----------------------------------------------------------------------*/
  gsdi_perso_parse_data_and_build_perso_data(feature,
                                             use_imsi_buffer,
                                             returned_data_len_p,
                                             data_p,
                                             &perso_data_len,
                                             perso_data_p);
  if ( perso_data_len <= 0 )
  {
    /* Set the Data to return for the Callback to Handle */
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     The Personalization Data to FS
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    filename_p = array_of_code_files[GSDI_PERSO_NW_INDEX];
    break;
  case GSDI_PERSO_NS:
    filename_p = array_of_code_files[GSDI_PERSO_NS_INDEX];
    break;
  case GSDI_PERSO_SP:
    filename_p = array_of_code_files[GSDI_PERSO_SP_INDEX];
    break;
  case GSDI_PERSO_CP:
    filename_p = array_of_code_files[GSDI_PERSO_CP_INDEX];
    break;
  case GSDI_PERSO_SIM:
    filename_p = array_of_code_files[GSDI_PERSO_SIM_INDEX];
    break;
  default:
    MMGSDI_DEBUG_ERR("BAD Feature: %x",feature,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
    MMGSDIUTIL_TMC_MEM_FREE(perso_data_p);
    return GSDI_ERROR;
  }
  gsdi_perso_encrypt_data(perso_data_len,
                          perso_data_p,
                          &encrypted_len,
                          gsdi_encrypted_data_buffer);

  gsdi_status = gsdi_efs_write_file(filename_p,
                                    encrypted_len,
                                    gsdi_encrypted_data_buffer);

  if ( gsdi_status != GSDI_SUCCESS )
    MMGSDI_DEBUG_ERR("Fail Write Perso Codes Feature %x Filename %x",feature,filename_p,0);
  /* ------------------------------------------------------------------------
     Free up all memory allocated during this procedure
     read_req_p and perso_data_p
     ----------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(read_req_p);
  MMGSDIUTIL_TMC_MEM_FREE(perso_data_p);
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SELF_PERSONALIZE

DESCRIPTION
  This function will be called during the initialization procedures, after
  the PIN has been verified in the case that it is required that the mobile
  perform self personalization.

  State Behavior:
     GSDI_PERSO_SELF_INIT_STATE:
     1.  Determine if the Feature is Activated & the file exists & the file
         size is 0.  Return TRUE iff the criteria matches to do self perso.
     2.  If TRUE set next state to GSDI_PERSO_DEACTIVATE_STATE
     3.  If FALSE set the next state to GSDI_PERSO_DONE_STATE, either it
         is not activated, or has already been personalized
     4.  If at any time an error is encountered, set the sate to ERROR


DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_self_personalize (
  gsdi_perso_enum_type                  feature,
  gsdi_perso_self_perso_state_enum_type self_perso_state,
  boolean                               deactivated,
  int32 *                               returned_data_len_p,
  uint8 *                               data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  boolean        self_perso  = FALSE;

  switch ( self_perso_state )
  {
  case GSDI_PERSO_SELF_INIT_STATE:
    MSG_MED("GSDI_PERSO_SELF_INIT_STATE - FEATURE: %x",feature,0,0);
    gsdi_status = gsdi_perso_is_self_perso_required(feature,
                                                    returned_data_len_p,
                                                    data_p,
                                                    &self_perso);
    switch ( gsdi_status )
    {
    case GSDI_PERSO_INVALID_DATA:
      data_p[0] = (uint8)GSDI_PERSO_SELF_INIT_STATE;
      data_p[1] = (uint8)feature;
      break;
    case GSDI_SUCCESS:
      if ( self_perso )
      {
        /* Plase the next state pointer into the first byte so that
        ** the callback knows what to do next
        */
        data_p[0] = (uint8)GSDI_PERSO_DEACTIVATE_STATE;
        data_p[1] = (uint8)feature;
      }
      else
      {
        data_p[0] = (uint8)GSDI_PERSO_DONE_STATE;
        data_p[1] = (uint8)feature;
      }
      break;
    default:
      MMGSDI_DEBUG_ERR("Could perform is_self_perso_required: %x",gsdi_status,0,0);
      data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
      break;
    }
    break;
  case GSDI_PERSO_DEACTIVATE_STATE:
    /* ------------------------------------------------------------------------
       GSDI_PERSO_DEACTIVATE_STATE:
       1.  Go through deactivation state
           This function will build get the required data to build a Deactive
           Inidicator Command
       2.  The callback of the Deactivation command will be used to determine
           whether we continue to the next state, done state, or error handling
           state
       -----------------------------------------------------------------------*/
    gsdi_status = gsdi_perso_self_perso_deactivate_state_procedures(
                                                         feature,
                                                         returned_data_len_p,
                                                         data_p);

    /* ------------------------------------------------------------------------
       Based on the result of the Self Perso Deativate Information choose
       go to the next state.
       ----------------------------------------------------------------------*/
    if ( gsdi_status != GSDI_SUCCESS )
    {
      data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
      data_p[1] = (uint8)feature;
    }
    else
    {
      data_p[0] = (uint8)GSDI_PERSO_WAIT_FOR_DEACT_STATE;
      data_p[1] = (uint8)feature;
    }
    break;
  case GSDI_PERSO_WAIT_FOR_DEACT_STATE:
    if ( deactivated )
    {
      data_p[0] = (uint8)GSDI_PERSO_SAVE_SIM_TO_FS_STATE;
      data_p[1] = (uint8)feature;
    }
    else
    {
      data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
      data_p[1] = (uint8)feature;
    }
    break;

  case GSDI_PERSO_SAVE_SIM_TO_FS_STATE:
    gsdi_status = gsdi_perso_save_sim_perso_info_to_fs(feature,
                                                       returned_data_len_p,
                                                       data_p);
    switch ( gsdi_status )
    {
    case GSDI_PERSO_INVALID_DATA:
      data_p[0] = (uint8)GSDI_PERSO_DONE_STATE;
      break;
    case GSDI_SUCCESS:
      data_p[0] = (uint8)GSDI_PERSO_ACTIVATE_STATE;
      break;
    default:
      data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
      break;
    }
    data_p[1] = (uint8)feature;
    break;
  case GSDI_PERSO_ACTIVATE_STATE:
    gsdi_status = gsdi_perso_self_perso_activate_state_procedures(
                                                       feature,
                                                       returned_data_len_p,
                                                       data_p);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
      data_p[1] = (uint8)feature;
    }
    else
    {
      data_p[0] = (uint8)GSDI_PERSO_WAIT_FOR_ACT_STATE;
      data_p[1] = (uint8)feature;
    }
    break;
  case GSDI_PERSO_WAIT_FOR_ACT_STATE:
    if ( !(deactivated) )
    {
      data_p[0] = (uint8)GSDI_PERSO_DONE_STATE;
      data_p[1] = (uint8)feature;
    }
    else
    {
      data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
      data_p[1] = (uint8)feature;
    }
    break;
  case GSDI_PERSO_ERROR_STATE:
    /* A Sanity Error Occurred...notify all clients */
    gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                             FALSE,
                                             0,
                                             NULL);
    data_p[0] = (uint8)GSDI_PERSO_ERROR_STATE;
    data_p[1] = (uint8)feature;
    gsdi_status = GSDI_SUCCESS;
    break;

  default:
    break;
  }
  return gsdi_status;
}
/*===========================================================================
FUNCTION GSDI_PROC_PERSO_INTERNAL_INIT_PROCEDURES

DESCRIPTION
  This function will be called for each personalization procedure that occurs.
  It will be the main personalization handling function used durint the init
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_internal_init_procedures(
  gsdi_perso_int_init_procedure_req_T * req_p,
  int32 *                               returned_data_len_p,
  uint8 *                               data_p
)
{

  gsdi_returns_T    gsdi_status     = GSDI_SUCCESS;
  boolean           imsi_valid      = FALSE;

  /* --------------------------------------------------------------------------
     Parameter checking fail will cause the Personalization check on this
     feature to fail.  Causing a general personalizaiton failure which in turn
     will cause the UI to display an Error
     ------------------------------------------------------------------------*/
  if ( req_p               == NULL ||
       returned_data_len_p == NULL ||
       data_p              == NULL )
  {
    gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                             FALSE,
                                             0,
                                             NULL);
    return GSDI_ERROR;
  }
  /* --------------------------------------------------------------------------
     Start the GSDI Personalization Checks to see if the SIM/USIM is the valid
     SIM/USIM the UE was previously personalized with.
     --------------------------------------------------------------------------
     check to see if the Feature is set to none...indication that if active
     MMGSDI needs to self personalize
     ------------------------------------------------------------------------*/

  switch ( req_p->feature )
  {
  case GSDI_PERSO_NONE:
    /* Special case if Perso state is in correct state */
    if ( gsdi_data.perso_state == GSDI_PERSO_INIT_STATE )
    {
      /* Read and format the IMSI accordingly */
      gsdi_status = gsdi_perso_read_and_format_imsi(returned_data_len_p,
                                                    data_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_ERR("Failed to read and format IMSI: %x",gsdi_status,0,0);
        imsi_valid = FALSE;
      }
      else
      {
        imsi_valid = gsdi_perso_is_imsi_valid(*returned_data_len_p,
                                            data_p);
      }

      if ( ! (imsi_valid ) )
      {
        /* Signal Main Task that there is an Error so that the Main Task
        ** can send out the GSDI_CARD_ERROR Event.  Reusing the
        ** GSDI_UIM_ERROR_SIG will allow us to signal GSDI
        */
        (void)rex_set_sigs(&gsdi_task_tcb,GSDI_UIM_ERROR_SIG);

        /* Return a GSDI_PERSO_INVALID_DATA so that it can skipt out of the
        ** remaining personalization procedures
        */
        data_p[0] = (uint8)GSDI_PERSO_SELF_INIT_STATE;
        data_p[1] = (uint8)req_p->feature;
        return GSDI_PERSO_INVALID_DATA;
      }
      gsdi_status = gsdi_perso_self_personalize(req_p->int_init_feature,
                                                req_p->self_perso_state,
                                                req_p->deactivated,
                                                returned_data_len_p,
                                                data_p);
      if ( gsdi_status != GSDI_SUCCESS )
      {
        MMGSDI_DEBUG_ERR("Failed to self personalize",0,0,0);
        return gsdi_status;
      }
    }
    break;
  case GSDI_PERSO_NW:
    if ( gsdi_data.perso_state == GSDI_PERSO_NW_INIT_STATE )
    {
      /* Is the Network Indicator Perm Disabled */
      gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                        returned_data_len_p,
                                                        data_p);
      if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
       return GSDI_PERSO_PERM_DISABLED;

      /* Is it activated */
      gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                   returned_data_len_p,
                                                   data_p);
      if ( gsdi_status != GSDI_PERSO_CURRENTLY_ACTIVE )
        return gsdi_status;

      /* Are we using the Test IMSI */
      if ( gsdi_perso_is_test_imsi_detected() )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {

        /* Verify the FS Contents of the Network Codes Stored
        ** against the contents in the SIM/USIM.  In this case,
        ** from the IMSI
        */
        gsdi_status = gsdi_perso_check_sim_nw_codes(returned_data_len_p,
                                                    data_p);
      }
      switch ( gsdi_status )
      {
      case GSDI_ERROR:
      case GSDI_NOT_FOUND:
      case GSDI_NOT_SUPPORTED:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_NW_FAILURE,
                                                FALSE,
                                                0,
                                                NULL);
        break;
      case GSDI_SUCCESS:
      case GSDI_PERSO_PERM_DISABLED:
      case GSDI_PERSO_CURRENTLY_DEACT:
        break;
      default:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                                 FALSE,
                                                 0,
                                                 NULL);
        break;
      }
    }
    else if ( gsdi_data.perso_state == GSDI_PERSO_NW_WAIT_DEACT_STATE )
    {
      if ( req_p->deactivated )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = GSDI_ERROR;
      }
    }
    break;
  case GSDI_PERSO_NS:
    if ( gsdi_data.perso_state == GSDI_PERSO_NS_INIT_STATE )
    {
      /* Is the Network Subset Indicator Perm Disabled */
      gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                        returned_data_len_p,
                                                        data_p);
      if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
       return GSDI_PERSO_PERM_DISABLED;

      /* Now check to see if NS is activated. */
      gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                   returned_data_len_p,
                                                   data_p);
      if ( gsdi_status != GSDI_PERSO_CURRENTLY_ACTIVE )
      {
        return gsdi_status;
      }
      /* Are we using the Test IMSI */
      if ( gsdi_perso_is_test_imsi_detected() )
      {
        return GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = gsdi_perso_check_sim_ns_codes(returned_data_len_p,
                                                     data_p);
      }
      switch ( gsdi_status )
      {
      case GSDI_ERROR:
      case GSDI_NOT_FOUND:
      case GSDI_NOT_SUPPORTED:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_NS_FAILURE,
                                                FALSE,
                                                0,
                                                NULL);
        break;
      case GSDI_SUCCESS:
      case GSDI_PERSO_PERM_DISABLED:
      case GSDI_PERSO_CURRENTLY_DEACT:
        break;
      default:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                                 FALSE,
                                                 0,
                                                 NULL);
        break;
      }
    }
    else if ( gsdi_data.perso_state == GSDI_PERSO_NS_WAIT_DEACT_STATE )
    {
      if ( req_p->deactivated )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = GSDI_ERROR;
      }
    }
    break;
  case GSDI_PERSO_SP:
    if ( gsdi_data.perso_state == GSDI_PERSO_SP_INIT_STATE )
    {
      /* Is the Network Subset Indicator Perm Disabled */
      gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                        returned_data_len_p,
                                                        data_p);
      if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
       return GSDI_PERSO_PERM_DISABLED;

      /* Now check to see if NS is activated. */
      gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                   returned_data_len_p,
                                                   data_p);
      if ( gsdi_status != GSDI_PERSO_CURRENTLY_ACTIVE )
      {
        return gsdi_status;
      }
      /* Are we using the Test IMSI */
      if ( gsdi_perso_is_test_imsi_detected() )
      {
       return GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = gsdi_perso_check_sim_sp_codes(returned_data_len_p,
                                                     data_p);
      }
      switch ( gsdi_status )
      {
      case GSDI_ERROR:
      case GSDI_NOT_FOUND:
      case GSDI_NOT_SUPPORTED:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SP_FAILURE,
                                                FALSE,
                                                0,
                                                NULL);
        break;
      case GSDI_SUCCESS:
      case GSDI_PERSO_PERM_DISABLED:
      case GSDI_PERSO_CURRENTLY_DEACT:
        break;
      default:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                                 FALSE,
                                                 0,
                                                 NULL);
        break;
      }
    }
    else if ( gsdi_data.perso_state == GSDI_PERSO_SP_WAIT_DEACT_STATE )
    {
      if ( req_p->deactivated )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = GSDI_ERROR;
      }
    }
    break;
  case GSDI_PERSO_CP:
    if ( gsdi_data.perso_state == GSDI_PERSO_CP_INIT_STATE )
    {
      /* Is the Network Subset Indicator Perm Disabled */
      gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                        returned_data_len_p,
                                                        data_p);
      if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
       return GSDI_PERSO_PERM_DISABLED;

      /* Now check to see if CP is activated. */
      gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                   returned_data_len_p,
                                                   data_p);
      if ( gsdi_status != GSDI_PERSO_CURRENTLY_ACTIVE )
      {
        return gsdi_status;
      }
      /* Are we using the Test IMSI */
      if ( gsdi_perso_is_test_imsi_detected() )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = gsdi_perso_check_sim_cp_codes(returned_data_len_p,
                                                    data_p);
      }
      switch ( gsdi_status )
      {
      case GSDI_ERROR:
      case GSDI_NOT_FOUND:
      case GSDI_NOT_SUPPORTED:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_CP_FAILURE,
                                                FALSE,
                                                0,
                                                NULL);
        break;
      case GSDI_SUCCESS:
      case GSDI_PERSO_PERM_DISABLED:
      case GSDI_PERSO_CURRENTLY_DEACT:
        break;
      default:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                                 FALSE,
                                                 0,
                                                 NULL);
        break;
      }
    }
    else if ( gsdi_data.perso_state == GSDI_PERSO_CP_WAIT_DEACT_STATE )
    {
      if ( req_p->deactivated )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = GSDI_ERROR;
      }
    }
    break;
  case GSDI_PERSO_SIM:
    if ( gsdi_data.perso_state == GSDI_PERSO_SIM_INIT_STATE )
    {
      /* Is the Network Subset Indicator Perm Disabled */
      gsdi_status = gsdi_perso_is_feature_perm_disabled(req_p->feature,
                                                        returned_data_len_p,
                                                        data_p);
      if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
       return GSDI_PERSO_PERM_DISABLED;

      /* Now check to see if CP is activated. */
      gsdi_status = gsdi_perso_is_feature_activated(req_p->feature,
                                                   returned_data_len_p,
                                                   data_p);
      if ( gsdi_status != GSDI_PERSO_CURRENTLY_ACTIVE )
      {
        return gsdi_status;
      }
      /* Are we using the Test IMSI */
      if ( gsdi_perso_is_test_imsi_detected() )
      {
       return GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = gsdi_perso_check_sim_sim_codes(returned_data_len_p,
                                                     data_p);
      }
      switch ( gsdi_status )
      {
      case GSDI_ERROR:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SIM_FAILURE,
                                                FALSE,
                                                0,
                                                NULL);
        break;
      case GSDI_SUCCESS:
      case GSDI_PERSO_PERM_DISABLED:
      case GSDI_PERSO_CURRENTLY_DEACT:
        break;
      case GSDI_NOT_FOUND:
      case GSDI_NOT_SUPPORTED:
      default:
        gsdi_perso_notify_clients_of_perso_event(GSDI_PERSO_SANITY_ERROR,
                                                 FALSE,
                                                 0,
                                                 NULL);
        break;
      }
    }
    else if ( gsdi_data.perso_state == GSDI_PERSO_SIM_WAIT_DEACT_STATE )
    {
      if ( req_p->deactivated )
      {
        gsdi_status = GSDI_SUCCESS;
      }
      else
      {
        gsdi_status = GSDI_ERROR;
      }
    }
    break;
  default:
    gsdi_status = GSDI_ERROR;
    break;
  }

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_POST_PIN1

DESCRIPTION
  This function is the callback function used to handle the response for the
  personalization procedures which in turn kicks off the next personalization
  init procedure.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_post_pin1(
  gsdi_slot_id_type slot
)
{
  gsdi_perso_int_init_procedure_req_T req;
  gsdi_returns_T                      gsdi_status = GSDI_SUCCESS;
  uint8                               card_apps   = 0x00;

  switch ( gsdi_data.perso_state )
  {
  case GSDI_PERSO_NOT_INIT_STATE:
#ifdef FEATURE_MMGSDI_UMTS
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */

#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /* FEATURE_MMGSDI_GSM */

#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */
    MSG_HIGH("Checking for Personalization",0,0,0);
    req.feature = GSDI_PERSO_NONE;
    gsdi_data.perso_state++;
    break;
  case GSDI_PERSO_INIT_STATE:
  case GSDI_PERSO_NW_INIT_STATE:
  case GSDI_PERSO_NS_INIT_STATE:
  case GSDI_PERSO_SP_INIT_STATE:
  case GSDI_PERSO_CP_INIT_STATE:
  case GSDI_PERSO_SIM_INIT_STATE:
  case GSDI_PERSO_COMPLETE_STATE:
   return GSDI_SUCCESS;
  default:
    return GSDI_ERROR;
  }
  req.message_header.message_set         = 13;
  req.message_header.slot                = GSDI_SLOT_1;
  req.message_header.message_id          = GSDI_PERSO_INT_PROC_REQ;
  req.self_perso_state                   = GSDI_PERSO_SELF_INIT_STATE;
  req.int_init_feature                   = GSDI_PERSO_NW;
  req.message_header.gsdi_async_callback = gsdi_perso_int_init_handler_cb;
  req.message_header.message_len_msb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T )-sizeof(gsdi_cmdhdr_T))/0x100);
  req.message_header.message_len_lsb     = (byte)((sizeof(
    gsdi_perso_int_init_procedure_req_T)-sizeof(gsdi_cmdhdr_T))%0x100);
  req.self_perso_state                   = GSDI_PERSO_SELF_INIT_STATE;
  (void)gsdi_cmd_ext(&req);
  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_CONVERT_SIM_EF_DCK_TO_FS_CK

DESCRIPTION
  This function is used to take in the Personalization Feature checked, the
  length of the entire EF-DCK Contents, and the data returned by the read
  previous to this function, to convert the values to 8 ASCII Characters
  to be used for the Depersonalization Procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
void gsdi_perso_convert_sim_ef_dck_to_fs_ck(
  gsdi_perso_enum_type feature,
  int32 *              length_p,
  uint8 *              data_p)
{
  uint8 temp_buffer[GSDI_PERSO_MAX_CK] = {0};
  int32 temp_len    = 0;
  int32 start_index = 0;
  int32 stop_index  = 0;

  /* --------------------------------------------------------------------------
     Check the Pointers to ensure they are not NULL
     ------------------------------------------------------------------------*/
  if ( length_p == NULL ||
       data_p   == NULL )
  {
    MMGSDI_DEBUG_ERR("NULL Pointers in Convert Function: %x %x",length_p,data_p,0);
    return;
  }

  /* --------------------------------------------------------------------------
     Determine the required offset
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_NW:
    start_index = GSDI_PERSO_DCK_NW_CK_OFFSET;
    break;

  case GSDI_PERSO_NS:
    start_index = GSDI_PERSO_DCK_NS_CK_OFFSET;
    break;

  case GSDI_PERSO_SP:
    start_index = GSDI_PERSO_DCK_SP_CK_OFFSET;
    break;

  case GSDI_PERSO_CP:
    start_index = GSDI_PERSO_DCK_CP_CK_OFFSET;
    break;

  default:
    /* Allow the depersonalization procedures to fail the
    ** procedures due to a bad key
    */
    MMGSDI_DEBUG_ERR("BAD FEATURE ID: %x",feature,0,0);
    return;
  }

  /* --------------------------------------------------------------------------
     Calculate the upper bound or stop_index.  It should be the
     start_index + 4 Bytes
     ------------------------------------------------------------------------*/
  stop_index = GSDI_PERSO_DCK_CK_LEN + start_index;

  /* --------------------------------------------------------------------------
     Start converting from to ASCII
     ------------------------------------------------------------------------*/
  while ( start_index < stop_index )
  {
    temp_buffer[temp_len++] = (uint8)(data_p[start_index]  & 0x0F ) + 0x30;
    temp_buffer[temp_len++] = (uint8)((data_p[start_index] & 0xF0 ) >> 4) + 0x30;
    start_index++;
  }

  /* --------------------------------------------------------------------------
     Update the Data Len and Data Buffer Provided with the new Data
     ------------------------------------------------------------------------*/
  memcpy(data_p,temp_buffer,int32touint32(temp_len));
  *length_p = temp_len;

} /* gsdi_perso_convert_sim_ef_dck_to_fs_ck */


/*===========================================================================
FUNCTION GSDI_PERSO_REFRESH_PROCEDURES

DESCRIPTION
  This function will be used to control the reading of EF-DCK and
  depersonalize the features for which Control Keys exist.

  This function will also perform the clearing of the control keys.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_refresh_procedures(
  gsdi_slot_id_type slot,
  int32 *           returned_data_len_p,
  uint8 *           data_p
)
{
  gsdi_perso_deact_feature_ind_req_T    deact_req;
  gsdi_returns_T                        gsdi_status  = GSDI_ERROR;
  boolean                               do_deperso   = FALSE;
  boolean                               is_default_ck= FALSE;
  uint8                                 i            = 0;

  /* --------------------------------------------------------------------------
     In a loop, for the max number of records supported in EF-DCK as
     indicated in 11.11 and 31.102, perform the following:
     1.  Determine if PERM Disabled
     2.  Determine if Active
     3.  Read the EF-DCK File (Transparent)
     4.  Check to see if it is default Key
     5.  Depersonalize
     6.  Wipe out EF-DCK Key for feature just depersonalized
     ------------------------------------------------------------------------*/
  for ( i = 0; i < GSDI_PERSO_DCK_NUM_CKS; i++)
  {
    /* Check if the Feature is perm disabled */
    gsdi_status = gsdi_perso_is_feature_perm_disabled((gsdi_perso_enum_type)i,
                                                      returned_data_len_p,
                                                      data_p);

    /* If not perm disabled, status returned is GSDI_SUCCESS */
    if ( gsdi_status == GSDI_SUCCESS )
    {
      do_deperso = TRUE;
    }

    /* Check if the feature is currently active */
    gsdi_status = gsdi_perso_is_feature_activated((gsdi_perso_enum_type)i,
                                                  returned_data_len_p,
                                                  data_p);

    if ( gsdi_status == GSDI_PERSO_CURRENTLY_ACTIVE)
    {
      do_deperso &= TRUE;
    }
    else
    {
      /* Incase it was set to true because it was not perm disabled
      ** go ahead and not active reset to false
      */
      do_deperso = FALSE;
    }

    /* If Active and Not Perm Disabled, run refresh deperso procs */
    if ( do_deperso )
    {
      gsdi_status = gsdi_perso_read_ef_dck_record(slot,
                                                  returned_data_len_p,
                                                  data_p);
      /* If the file is not found, not supported (i.e. Service Table
      ** check Fails then we need to return out of here.  Retry to
      ** read again later
      */
      if ( gsdi_status != GSDI_SUCCESS       &&
           gsdi_status != GSDI_NOT_SUPPORTED &&
           gsdi_status != GSDI_NOT_FOUND )
      {
        /* Can't read the Depersonalization Keys, must fail the procedure */
        return gsdi_status;
      }
      else if ( gsdi_status == GSDI_NOT_SUPPORTED ||
                gsdi_status == GSDI_NOT_FOUND )
      {
        return GSDI_SUCCESS;
      }

      /* Now determine if Data read is the default key */
      is_default_ck = gsdi_perso_is_key_default_ck((gsdi_perso_enum_type)i,
                                                    *returned_data_len_p,
                                                    data_p);


      if ( ! (is_default_ck ) )
      {
        /* The CK Retrieved is not the default...go ahead and formate the
        ** data as required by the deactiveatre_feature_ind function
        */
        gsdi_perso_convert_sim_ef_dck_to_fs_ck((gsdi_perso_enum_type)i,
                                               returned_data_len_p,
                                               data_p);

        /* Assumption is...DCK is 8 Digits of Control Keys in ASCII Format  */
        memcpy(deact_req.ck_buffer,data_p,int32touint32(*returned_data_len_p));
        deact_req.feature   = (gsdi_perso_enum_type)i;
        deact_req.num_bytes = GSDI_PERSO_MAX_CK;
        gsdi_status = gsdi_proc_perso_deactivate_feature_ind(&deact_req,
                                                             returned_data_len_p,
                                                             data_p );
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("Could not depersonalize feature: 0x%x",i,0,0);
        }
      }
      else
      {
        MSG_HIGH("CK for Feature 0x%x: DEFAULT CK",i,0,0);
      }
    }
    else
    {
      MSG_HIGH("Feature: 0x%x is not set up for Deperso",i,0,0);
    }
  }
  /* Now Wipe Out the Control Key Information from the Record */
  gsdi_perso_clear_ck_from_ef_dck(slot,
                                  returned_data_len_p);
  return gsdi_status;
}

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* FEATURE_PERSO_SIM*/
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
