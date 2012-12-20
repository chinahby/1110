
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   P E R S O N A L I Z A T I O N   E N G I N E


GENERAL DESCRIPTION

  This source file contains the GSDI PERSONALIZATION ENGINE which adheres to
  the personalization interface to do the personalization procedures.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2011 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/mmgsdi/src/gsdi_perso_sec.c#2 $$ $DateTime: 2011/02/24 16:39:02 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/11   nmb     Fixed perso check of EFs GID1 and GID2 
03/04/10   nmb     Fixed range of perso keys between 0 and 9
05/04/09   js      Fixed warnings
04/16/09   mib     Reset perso counter when control key is correct
04/01/09   mib     Fixed if condition checking first byte of GID1 file
03/27/09   mib     Fixed perso notification when counter reaches zero
02/28/09   ssr     Fixed sim lock code length
02/24/09   kk      Featurizing SEC RAND Call back API invocation
02/18/09   tml     Fixed memory leak issues
02/05/09   jar     Removed a declarations of the REX Timer created for SECRND
02/03/09   ssr     Fixed sim lock code length
01/22/09   sun     Support for handling async cb in perso sec
01/07/09   ssr     Update change of gsdi_perso_ruim_security_create_data
09/25/08   kk      Added support for extracting unblock dck retries available
09/19/08   ssr     Fixed indicators in gsdi_perso_ruim_security_create_data
08/27/08   sp      Added support for get_dck_num_retries
07/21/08   sp      Added debug messages.
05/17/08   sun     Fixed warning
05/08/08   sun     Fixed return of DCK retry for Activate/Deactivate/Unblock
04/16/08   kk      Added support of get command on sim lock data
11/15/07   sun     Lint Fixes
07/20/07   sun     Fixed Return Status for Blocked Feature
05/16/07   sun     Fixed Calculation of Length
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Fixed Calculation of Length
03/23/07   sun     Fixed potential data abort
03/02/07   sun     Fixed Lint Errors
02/27/07   sun     Activate only the features that were autolocked. Even if
                   codes have been generate for others, do not activatate
                   indicator.
02/08/07   sun     Changed functionality of autolock. Autolock to only
                   one feature.
01/29/07   sun     Checked for NULLS
09/28/06   sp      Fixed Lint error
09/27/06   sun     Added new random number generator using Security API.
09/21/06   wli     Extended a mask to cover perso RUIM
09/13/06   wli     Fixed bugs in RUIM lock
08/23/06   sun     Fixed Lint Errors
08/14/06   tml     Fixed alignment
08/08/06   sun     Fixed Lint Error
08/02/06   sun     Do not continue with checking for locks if a feature is
                   blocked
07/17/06   sun     Backing out SEC API Randome Number Generator function
07/07/06   sun     Fixed match found error, generate random number using secapi
06/21/06   tml     Fixed unblock and disable issue
05/18/06   sun     Do not send out the event if a mismatch in control key occurs.
                   The cmdCb will take care of this.
05/04/06   sun     Fixed dck copy in perso_refresh
04/28/06   sun     Use ran_reg for random number generators
                   Fixed match_found errors
                   Fixed OTA errors
                   On Unblock, save to FS and send out the new set of
                   retry values
                   On a Get phone code status, if the retry is 0, then send
                   out a blocked event as well.
04/13/06   sun     Replaced feat_inds with perm_feat_ind
04/12/06   sp      Fixed syntax error
04/10/06   sun     Fixed reset of number of retries after a successfull unblock
04/06/06   sun     Added support for RUIM Lock
03/27/06   tml     Turn off CP/SP feature is the GID1 or GID2 are not set
03/24/06   sun     Fixed Validate IMSI function
03/21/06   tml     lint
03/16/06   sun     Add only first byte of GID1/GID2 for SP/CP locks,
                   Skip SP/CP in Autolock if GID1/GID2 is not set
03/10/06   jar     Lint Fixes
03/08/06   jar     Merged into mainline
03/02/06   sun     Do not overwrite the perso_event in OTA_Deperso.
02/27/05   tml     Fixed memory leak issue
02/16/06   sun     Save the changes to filesystem after autolocking.
02/16/05   sun     Made local functions static, fixed unblock count check
02/01/06   tml     fixed refresh dck logic so that we won't try to deactivate
                   when dck ef is not found
01/25/06   sun     Write to FS only if the data was modified.
01/12/06   sp      Reset gsdi_status in gsdi_perso_security_refresh
12/20/05   sun     Removed unnecessary booleans from gsdi_perso_sec_data_mod_inds_type
12/15/05   sun     Should not allow activation on an activate(or autlocked)feature
12/14/05   sun     Added missing break
11/09/05   sun     Fixed lint errors
11/03/05   sun     Fixed Compiler Warning
10/03/05   sun     Pass the event from the perso engine to the security and back
06/01/05   jar     Removed Architecture Drawing.  Moved #5 back to tip.
05/05/05   sun     Added support for Perso Refresh
05/03/05   sun     Fixes to allow for proper bootup
04/30/05   sun     Personalization and Autolock fixes
04/28/05   sun     Initial release of the GSDI SIM Lock Architecutre to support
                   Plug In Proprietary Security Libraries driven by a Perso Engine.


===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "tmc.h"

#if  defined(FEATURE_MMGSDI_PERSONALIZATION) && \
    defined  (FEATURE_MMGSDI_PERSONALIZATION_ENGINE ) && \
    !defined(FEATURE_MMGSDI_PERSONALIZATION_NON_QC)
#include "gsdi_perso_engine.h"
#include "gsdi_perso_sec.h"
#include "gsdi_exp.h"
#include "gsdi.h"
#include "gsdi_efs.h"
#include "intconv.h"
#include "gsdi_nv.h"
#include "ran.h"

#ifdef FEATURE_SEC
#include "secapi.h"
#endif

#include "mmgsdiutil.h"

static uint8 test_imsi_buffer[3]    = {0x00,0xF1,0x10};

/* ----------------------------------------------------------------------------
   Default Data Array Checked against
   --------------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_SIM
static uint8 mmgsdi_ck_not_set[GSDI_PERSO_ENGINE_DCK_CK_LEN]= {0xFF,0xFF,0xFF,0xFF};
#endif

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_ACTIVATE

DESCRIPTION:
  Function called to perform the activation of the Feature.
  This function will write back to the NV or the Secure
  file system.  Therefore, the caller doesn't have to try
  and save any of the information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_activate(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_DEACTIVATE

DESCRIPTION:
  Function called to perform the activation of the Feature.
  This function will write back to the NV or the Secure
  file system.  Therefore, the caller doesn't have to try
  and save any of the information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_deactivate(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_FEATURE_INDS

DESCRIPTION:
    Get all the feature indicators and their status
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_get_feature_inds(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_FEATURE_KEY

DESCRIPTION:
   Get the key for the given feature indicator
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_get_feature_key(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_SET_FEATURE_DATA

DESCRIPTION:
   Set the feature data provided that the feature in the right state and is
   not already active
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_set_feature_data(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_FEATURE_DATA

DESCRIPTION:
    Get SIM Lock code for the feature indicated in the request
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_get_feature_data(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8                                * ret_data_p,
 int32                                * ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_DISABLE_FEATURE_DATA

DESCRIPTION:
   Disable the feature indicator
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_disable_feature_data(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
 );

/* ============================================================================
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
=============================================================================*/
static gsdi_returns_T gsdi_perso_security_ota_deperso(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_CHECK_ALL_LOCKS

DESCRIPTION
  This function will be called for each personalization procedure that occurs.
  It will be the main personalization handling function used durint the init
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_check_all_locks(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_UNBLOCK_DCK

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
static gsdi_returns_T gsdi_perso_security_unblock_dck(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_GET_DCK_NUM_RETRIES

DESCRIPTION
  This function will be used to get the dck number of
  retries.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_get_dck_num_retries(
  const gsdi_perso_sec_client_req_type * req_data_ptr,
  gsdi_perso_sec_me_data_type          * encrypted_data_ptr,
  const gsdi_perso_sec_sim_data_type   * sim_data_ptr,
  uint8 *                                ret_data_ptr,
  int32*                                 ret_data_len_ptr,
  gsdi_perso_event_enum_type           * perso_event_ptr
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_GET_DCK_UNBLOCK_NUM_RETRIES

DESCRIPTION
  This function will be used to get the dck unblock number of
  retries.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_get_dck_unblock_num_retries(
  const gsdi_perso_sec_client_req_type * req_data_ptr,
  gsdi_perso_sec_me_data_type          * encrypted_data_ptr,
  const gsdi_perso_sec_sim_data_type   * sim_data_ptr,
  uint8 *                                ret_data_ptr,
  int32*                                 ret_data_len_ptr,
  gsdi_perso_event_enum_type           * perso_event_ptr
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_AUTOLOCK

DESCRIPTION
  This function will be called for each personalization procedure that occurs.
  It will be the main personalization handling function used durint the init
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_autolock(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_REFRESH

DESCRIPTION
  This function will be used to control the reading of EF-DCK and
  depersonalize the features for which Control Keys exist.

  This function will also perform the clearing of the control keys.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_refresh(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_VERIFY_PHONE_CODE

DESCRIPTION
  This function will be used to compare the phone code given by the client
  with the existing phone code.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_verify_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_CHANGE_PHONE_CODE

DESCRIPTION
  This function will be used to update the phone code and then update it
  update it with the new phone code.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_change_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_UNBLOCK_PHONE_CODE

DESCRIPTION
  This function will be used to unblock the phone code

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_unblock_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_SET_PHONE_CODE

DESCRIPTION
  This function will be used to set the phone code, this will write to the
  code only if it is not already active.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_set_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_SET_PHONE_CODE

DESCRIPTION
  This function will be used to get the phone code status and number of
  retries.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_get_phone_code_status(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

typedef gsdi_returns_T (* gsdi_perso_security_handler_func_type)(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_HANDLER

DESCRIPTION
  Commoin entry point for all the security engine functions

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static  gsdi_perso_security_handler_func_type gsdi_perso_security_handler[] =
{
  gsdi_perso_security_activate,                   /*GSDI_PERSO_ENGINE_ACTIVATE_FEATURE*/
  gsdi_perso_security_deactivate,                 /*GSDI_PERSO_ENGINE_DEACTIVATE_FEATURE*/
  gsdi_perso_security_get_feature_inds,           /*GSDI_PERSO_ENGINE_GET_FEATURE_IND*/
  gsdi_perso_security_get_feature_inds,           /*GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS*/
  gsdi_perso_security_set_feature_data,           /*GSDI_PERSO_ENGINE_SET_DATA_FEATURE*/
  gsdi_perso_security_unblock_dck,                /*GSDI_PERSO_ENGINE_UNBLOCK_DCK*/
  gsdi_perso_security_ota_deperso,                /*GSDI_PERSO_ENGINE_OTA_DERPERSO_REQ*/
  gsdi_perso_security_deactivate,                 /*GSDI_PERSO_ENGINE_INT_PROC_REQ*/
  gsdi_perso_security_disable_feature_data,       /*GSDI_PERSO_ENGINE_PERM_DISABLE_FEATURE_IND_REQ*/
  gsdi_perso_security_get_feature_inds,           /*GSDI_PERSO_ENGINE_GET_PERM_FEATURE_IND_REQ*/
  gsdi_perso_security_get_dck_num_retries,        /*GSDI_PERSO_ENGINE_GET_DCK_NUM_RETRIES*/
  gsdi_perso_security_get_feature_key,            /*GSDI_PERSO_ENGINE_GET_FEATURE_KEY_REQ*/
  gsdi_perso_security_check_all_locks,            /*GSDI_PERSO_ENGINE_CHECK_LOCKS_ALL_REQ*/
  gsdi_perso_security_autolock,                   /*GSDI_PERSO_ENGINE_AUTO_LOCK_REQ*/
  gsdi_perso_security_refresh,                    /*GSDI_PERSO_ENGINE_REFRESH_REQ*/
  NULL,                                           /*GSDI_PERSO_ENGINE_GET_SW_LIB_VERSION*/
  gsdi_perso_security_verify_phone_code,          /*GSDI_PERSO_ENGINE_VERIFY_PHONE_CODE_REQ*/
  gsdi_perso_security_change_phone_code,          /*GSDI_PERSO_ENGINE_CHANGE_PHONE_CODE_REQ*/
  gsdi_perso_security_unblock_phone_code,         /*GSDI_PERSO_ENGINE_UNBLOCK_PHONE_CODE_REQ*/
  gsdi_perso_security_set_phone_code,             /*GSDI_PERSO_ENGINE_SET_PHONE_CODE_REQ*/
  gsdi_perso_security_get_phone_code_status,      /*GSDI_PERSO_ENGINE_GET_PHONE_CODE_STATUS */
  gsdi_perso_security_check_all_locks,            /*GSDI_PERSO_ENGINE_RUIM_CHECK_LOCKS_ALL*/
  gsdi_perso_security_autolock,                   /*GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK*/
  gsdi_perso_security_get_feature_data,           /* GSDI_PERSO_ENGINE_GET_FEATURE_DATA */
  gsdi_perso_security_get_dck_unblock_num_retries /*GSDI_PERSO_ENGINE_GET_DCK_UNBLOCK_NUM_RETRIES */
};

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
  return (uint32)ran_next();
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
static boolean gsdi_perso_sec_is_test_imsi_detected(
  const uint8 *imsi_buffer_p )
{
  if(NULL == imsi_buffer_p)
  {
    MSG_ERROR("PERSO: imsi buffer is NULL , gsdi_perso_sec_is_test_imsi_detected",0,0,0);
    return FALSE;
  }

  if ( memcmp(imsi_buffer_p,test_imsi_buffer,3))
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SEC_GET_OFFSET

DESCRIPTION
  This function will get the offset into the file given the mask

DEPENDENCIES
  None

RETURN VALUE
  int32 (offset)
===========================================================================*/
static int32 gsdi_perso_sec_get_offset
(
  uint32 feature_mask
)
{
  int32 offset = 0;
  switch ( feature_mask)
  {
#ifdef FEATURE_PERSO_SIM
   case GSDI_PERSO_SEC_MASK_FEAT_NW:
     offset = GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_NS:
     offset = GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_SP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_CP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_SIM:
     offset = GSDI_PERSO_SEC_IND_OFFSET_SIM_OFFSET;
     break;
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW1_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW2_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_HRPD_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_SP_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_CP_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_RUIM_OFFSET;
     break;
#endif /*FEATURE_PERSO_RUIM*/
   case GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT:
   default:
     offset = 0xFFFF;
     MSG_ERROR("PERSO: Wrong feature_mask 0x%x, setting offset to 0xFFFF",
                feature_mask,0,0);
     break;

  } /* switch */
  return offset;

}
/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*==========================================================================
FUNCTION GSDI_PERSO_SEC_CALC_MEM_REQUIRED

DESCRIPTION
  This function will caculate the memory required given the type of
  calculation to be done.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_calc_mem_required(
  gsdi_perso_sec_calc_enum_type   calc_type,
  const gsdi_perso_sec_data_type *encrypted_data_p,
  uint32                         *required_len_p,
  uint32                          feature_mask
)
{
  int32  offset        = 0;
  uint32 pos           = 0;
  uint32 i             = 0;
  uint32 curr_lock_len = 0;

  /* -----------------------------------------------------------------------
     Validate the Parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p->buffer_p);
  MMGSDI_RETURN_IF_EXCEEDS(encrypted_data_p->len,GSDI_PERSO_SEC_ENCRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(required_len_p);

  /* --------------------------------------------------------------------------
     Initialize all variables
     ------------------------------------------------------------------------*/
  *required_len_p = 0;

  /* --------------------------------------------------------------------------
     Perform Length calculations.  Note, if the algorithm and storage of the
     method of storing the ciphering keys is changed, then this algorithm
     must also appropriately change
     ------------------------------------------------------------------------*/
  switch ( calc_type )
  {
    case GSDI_PERSO_SEC_CALC_INDS:
      *required_len_p = (uint32)(encrypted_data_p->len / 2);
      break;

    case GSDI_PERSO_SEC_CALC_CKS:
#ifdef FEATURE_PERSO_SIM
      *required_len_p = (uint32)(((encrypted_data_p->len - (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE)*5)/2)/5);
#endif
#ifdef FEATURE_PERSO_RUIM
      *required_len_p = (uint32)(((encrypted_data_p->len - (GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE)*6)/2)/6);
#endif
#if defined(FEATURE_PERSO_SIM) && defined(FEATURE_PERSO_RUIM)
      #error /*Need to reevaluate*/
#endif
      break;

    case GSDI_PERSO_SEC_CALC_DATA:
      offset = gsdi_perso_sec_get_offset(feature_mask);
      if(offset == 0xFFFF)
      {
        return GSDI_PERSO_INVALID_DATA;
      }
     /*Increment the length by the TLV of the previous features plus the size of TL of the current feature.*/
     pos = 0;

     do
     {
       pos += GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;
       *required_len_p = encrypted_data_p->buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN] ;   //lo_len
       *required_len_p += encrypted_data_p->buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] * 255; // hi_len
       pos += (uint32)*required_len_p;
       i++;
     }
     while(i<=(uint32)offset);

     if(i -1> (uint32)offset)
     {
       MSG_ERROR("PERSO: Invalid Perso data for GSDI_PERSO_SEC_CALC_DATA",0,0,0);
       return GSDI_PERSO_INVALID_DATA;
     }

     *required_len_p = *required_len_p/2;

      break;

    case GSDI_PERSO_SEC_CALC_FOR_GET_DATA:
      offset = gsdi_perso_sec_get_offset(feature_mask);
      if(offset == 0xFFFF)
      {
        return GSDI_PERSO_INVALID_DATA;
      }
      /*Increment the length by the TLV of the previous features plus the size of TL of the current feature.*/
      pos = 0;

      do
      {
        pos += GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;
        if (pos >= encrypted_data_p->len)
        {
          MSG_ERROR("PERSO: Invalid Perso data for GSDI_PERSO_SEC_CALC_FOR_GET_DATA",0,0,0);
          return GSDI_PERSO_INVALID_DATA;
        }
        curr_lock_len  = encrypted_data_p->buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN] ;   //lo_len
        curr_lock_len += encrypted_data_p->buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] * 255; // hi_len
        pos += curr_lock_len;
        i++;
      }while(i<=(uint32)offset);

      pos -= (curr_lock_len + GSDI_PERSO_ENGINE_LEN_SIZE);
      *required_len_p = encrypted_data_p->buffer_p[pos+GSDI_PERSO_ENGINE_LO_LEN] ;   //lo_len
      *required_len_p += encrypted_data_p->buffer_p[pos] * 255; // hi_len

      *required_len_p = *required_len_p/2;
      break;

    default:
      *required_len_p = 0x00;
      MSG_ERROR("PERSO: Invalid calc_type 0x%x in gsdi_perso_sec_calc_mem_required",
                calc_type,0,0);
      return GSDI_PERSO_INVALID_DATA;

  }

  return GSDI_SUCCESS;
}

/*==========================================================================
FUNCTION GSDI_PERSO_SECURITY_HANDLE_REQUEST

DESCRIPTION
  This function is the main entry to the security engine

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_security_handle_request
(
  gsdi_perso_engine_proc_enum_type feature_type,
  gsdi_perso_sec_client_req_type  * req_data_p,
  gsdi_perso_sec_me_data_type    * encrypted_data_p,
  gsdi_perso_sec_sim_data_type   * sim_data_p,
  uint8 *                          ret_data_p,
  int32*                           ret_data_len_p,
  gsdi_perso_event_enum_type      * perso_event_p

)
{
  if(feature_type <= GSDI_PERSO_ENGINE_GET_DCK_UNBLOCK_NUM_RETRIES)
  {
    if(gsdi_perso_security_handler[feature_type] != NULL)
    {
      return (gsdi_perso_security_handler[feature_type](req_data_p,/*lint !e746*/
                                                        encrypted_data_p,
                                                        sim_data_p,
                                                        ret_data_p,
                                                        ret_data_len_p,
                                                        perso_event_p));
    }
  }
  return GSDI_PERSO_INVALID_DATA;
}


/*===========================================================================
FUNCTION GSDI_PERSO_DECRYPT_TLV

DESCRIPTION
  This function is called to decrypt the data that is currently encrypted.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_decrypt_tlv(
  uint32                          mask,
  const gsdi_perso_sec_data_type *encrypted_data_p,
  gsdi_perso_sec_data_type       *data_p
)
{
  uint8 *        working_buff_p    = NULL;
  uint32         cipher_key_len    = 0;
  uint32         cipher_key_offset = 0;
  int32          feature_offset    = 0;
  uint32         index             = 0;
  int32          i                 = 0;
  uint32         pos               = 0;


  /* -----------------------------------------------------------------------
     Validate the Parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p->buffer_p);
  MMGSDI_RETURN_IF_EXCEEDS(encrypted_data_p->len,GSDI_PERSO_SEC_ENCRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(data_p);
  MMGSDI_RETURN_IF_NULL(data_p->buffer_p);


  /* -----------------------------------------------------------------------
     Allocate memory for a working buffer and memset it once completed.
     ---------------------------------------------------------------------*/
  working_buff_p = mmgsdi_malloc(GSDI_PERSO_SEC_DECRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(working_buff_p);

  /* --------------------------------------------------------------------------
     1. Calculate the Cipher Key Len and Cipher Key Offsets
     2. Ensure they meet our Max Criterias
     3. Ensure boundaries are overshot.
     4. Copy the Cipher Key to the working buffer.
     ------------------------------------------------------------------------*/
  feature_offset    = gsdi_perso_sec_get_offset(mask);

  if(feature_offset == 0xFFFF)
  {
     return GSDI_PERSO_INVALID_DATA;
  }

  /*Increment the length by the TLV of the previous features plus the size of TL of the current feature.*/
  pos = 0;

  do
  {
    pos += GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;
    cipher_key_len = encrypted_data_p->buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN];   //lo_len
    cipher_key_len += encrypted_data_p->buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] * 255; // lo_len
    pos += cipher_key_len;
    i++;
  }
  while(i<=feature_offset);

  pos               = pos - cipher_key_len;
  cipher_key_len    = cipher_key_len/2;
  cipher_key_offset = pos + cipher_key_len;

  if (cipher_key_len > GSDI_PERSO_SEC_DECRYPT_MAX_LEN)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Cipher Key Len Exceeds allowable Max: 0x%x",
                           cipher_key_len,0,0);

    /* Free up the working buffer */
    MMGSDIUTIL_TMC_MEM_FREE(working_buff_p);

    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(working_buff_p,
         (encrypted_data_p->buffer_p+cipher_key_offset),
         (uint32)cipher_key_len);

  /* ---------------------------------------------------------------------
     The current decryption algorithm uses the XOR Method.  To be replaced
     once SECURE FS is made available
     -------------------------------------------------------------------*/
  while ( index < cipher_key_len )
  {
    working_buff_p[index] = encrypted_data_p->buffer_p[pos + index] ^
                            working_buff_p[index];
    index++;
  }

  /* --------------------------------------------------------------------
     The working buffer has the decrypted data.  It is now ok to
     copy it over to the data_p->buffer_p provided by the client
     ------------------------------------------------------------------*/
  memcpy(data_p->buffer_p,
         working_buff_p,
         index);

  data_p->len = index;

  MMGSDIUTIL_TMC_MEM_FREE(working_buff_p);

  return GSDI_SUCCESS;
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
static gsdi_returns_T gsdi_perso_security_decrypt_data(
  const gsdi_perso_sec_data_type * encrypted_data_p,
  gsdi_perso_sec_data_type       * data_p
)
{
  uint8 *        working_buff_p    = NULL;
  uint32         cipher_key_len    = 0;
  uint32         cipher_key_offset = 0;
  uint32         index             = 0;

  /* -----------------------------------------------------------------------
     Validate the Parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p->buffer_p);
  MMGSDI_RETURN_IF_EXCEEDS(encrypted_data_p->len,GSDI_PERSO_SEC_ENCRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(data_p);
  MMGSDI_RETURN_IF_NULL(data_p->buffer_p);


  /* -----------------------------------------------------------------------
     Allocate memory for a working buffer and memset it once completed.
     ---------------------------------------------------------------------*/
  working_buff_p = mmgsdi_malloc(GSDI_PERSO_SEC_DECRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(working_buff_p);

  /* --------------------------------------------------------------------------
     1. Calculate the Cipher Key Len and Cipher Key Offsets
     2. Ensure they meet our Max Criterias
     3. Ensure boundaries are overshot.
     4. Copy the Cipher Key to the working buffer.
     ------------------------------------------------------------------------*/
  cipher_key_len    = encrypted_data_p->len/2;
  cipher_key_offset = encrypted_data_p->len/2;

  if (cipher_key_len > GSDI_PERSO_SEC_DECRYPT_MAX_LEN)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Cipher Key Len Exceeds allowable Max: 0x%x",
                           cipher_key_len,0,0);

    /* Free up the working buffer */
    MMGSDIUTIL_TMC_MEM_FREE(working_buff_p);

    return GSDI_ERROR;
  }

  memcpy(working_buff_p,
         (encrypted_data_p->buffer_p+cipher_key_offset),
         cipher_key_len);

  /* ---------------------------------------------------------------------
     The current decryption algorithm uses the XOR Method.  To be replaced
     once SECURE FS is made available
     -------------------------------------------------------------------*/
  while ( index < cipher_key_len )
  {
    working_buff_p[index] = encrypted_data_p->buffer_p[index] ^
                            working_buff_p[index];
    index++;
  }

  /* --------------------------------------------------------------------
     The working buffer has the decrypted data.  It is now ok to
     copy it over to the data_p->buffer_p provided by the client
     ------------------------------------------------------------------*/
  memcpy(data_p->buffer_p,
         working_buff_p,
         index);

  data_p->len = index;

  MMGSDIUTIL_TMC_MEM_FREE(working_buff_p);

  return GSDI_SUCCESS;
}


#if defined(FEATURE_SEC) && defined(FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API)
 /*===========================================================================
  FUNCTION GSDI_PERSO_SEC_GET_RANDOM_CB

  DESCRIPTION
    Sets the GSDI_PERSO_SECRND_READY_SIG when Secrnd task is started.

  DEPENDENCIES
    SECRND

  RETURN VALUE
    NONE
===========================================================================*/
void gsdi_perso_sec_get_random_cb( void* dummy)
{
  (void)dummy;
  (void)rex_clr_timer(&gsdi_perso_secrnd_timer);
  (void)rex_set_sigs(&gsdi_task_tcb, GSDI_PERSO_SECRND_READY_SIG);
}/*gsdi_perso_sec_get_random_cb*/
#endif /*FEATURE_SEC  && FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API */


/*===========================================================================
  FUNCTION GSDI_PERSO_CREATE_CONTROL_KEYS

  DESCRIPTION
    This function will be called only once to generate the control keys

  DEPENDENCIES
    None

  RETURN VALUE
    gsdi_returns_T
===========================================================================*/
static void gsdi_perso_sec_create_control_key(
    uint8 * data_p
)
{
    uint32             rand_value = 0;
    int32              index      = 0;
#ifdef FEATURE_SEC
    uint8              random[GSDI_PERSO_MAX_CK];
    uint16             length   = GSDI_PERSO_MAX_CK;
#ifdef FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API
    rex_sigs_type      sigs     = 0x0;
#endif /* FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API */
    secerrno_enum_type err_num  = E_SUCCESS;
#endif /*FEATURE_SEC*/


    if(NULL ==data_p)
    {
      MSG_ERROR("PERSO: NULL pointer in gsdi_perso_sec_create_control_key",0,0,0);
      return;
    }

#ifdef FEATURE_SEC
    /*------------------------------------------------------------------
      Call this function once - this will return E_AGAIN incase
      the task is not ready or the input is not available for generating
      the random number.

      If E_AGAIN is returned, then we will wait for SECRND task to be ready

      After that we will try only ONE more time to get the random number
    ----------------------------------------------------------------------- */
    err_num = secapi_get_random(SECAPI_SECURE_RANDOM,random,length);
#ifdef FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API
    if( err_num ==E_AGAIN)
    {
      secapi_get_random_set_cb(gsdi_perso_sec_get_random_cb, 0);
      (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_PERSO_SECRND_READY_SIG);

      /*Start timer incase SEC RND takes long*/
      (void)rex_set_timer(&gsdi_perso_secrnd_timer, GSDI_PERSO_SEC_RND_TIMER );

      sigs = gsdi_wait(GSDI_PERSO_SECRND_READY_SIG);

      (void) rex_clr_sigs(&gsdi_task_tcb, GSDI_PERSO_SECRND_READY_SIG);

      if ( sigs & GSDI_PERSO_SECRND_READY_SIG )
      {
        /*Clear the timer just in case it has not expired*/
        (void)rex_clr_timer(&gsdi_perso_secrnd_timer);

        MSG_HIGH("Random Number Should be generated now", 0, 0, 0);

        /*Try one more time*/
        err_num = secapi_get_random(SECAPI_SECURE_RANDOM,random,length);
      }
    }
#endif /* FEATURE_MMGSDI_PERSO_SEC_RAND_CB_API */
    if(err_num == E_SUCCESS)
    {
      while ( index < GSDI_PERSO_MAX_CK )
      {
        data_p[index] = (random[index] % 10) + '0';       /* Convert random value to ascii 0-9*/
        index++;
      }
    }
    else
#endif /*FEATURE_SEC */
    {
      while ( index < GSDI_PERSO_MAX_CK )
      {
        rand_value = (uint32)gsdi_get_rand_value(); /* Get the Random 32 Bit Value     */
        rand_value = (rand_value % 10) + '0';       /* Convert random value to ascii 0-9*/
        data_p[index] = (uint8)rand_value;
        index++;
      }
    }
}

/*===========================================================================
FUNCTION GSDI_PERSO_DECRYPT_TLV

DESCRIPTION
  This function is called to decrypt the data that is currently encrypted.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_encrypt_tlv(
  uint8                          * buffer_p,
  const gsdi_perso_sec_data_type * raw_data_p,
  uint32                         * len_p,
  uint32                           feature
)
{
  uint8 *        working_buff_p    = NULL;
  uint32         index             = 0;
  uint32         pos               = 0;

  /* -----------------------------------------------------------------------
     Validate the Parameters provided
     ---------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(buffer_p);
  MMGSDI_RETURN_IF_NULL(raw_data_p);
  MMGSDI_RETURN_IF_NULL(raw_data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(len_p);

 /* ---------------------------------------------------------------------------
     Allocate the working Buffer Size.  It must be
     2x the raw_data_p len.
     -------------------------------------------------------------------------*/
  working_buff_p = mmgsdi_malloc(GSDI_PERSO_SEC_ENCRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(working_buff_p);

  pos += GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;

  gsdi_perso_sec_create_control_key(working_buff_p);

  while ( index < raw_data_p->len )
  {
    buffer_p[pos+index]             = working_buff_p[index] ^
                                            raw_data_p->buffer_p[index];
    index++;
  }

  /* Now Paste the Cipher Key to the end of the Data */
  memcpy((buffer_p + pos + index),
          working_buff_p,
          index);

  *len_p = (index *2);

  buffer_p[0] = (uint8)feature; /*Code*/
  buffer_p[1] = (uint8)(*len_p/255);
  buffer_p[2] = (uint8)(*len_p%255);


  *len_p += (int32)(GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE);

  MMGSDIUTIL_TMC_MEM_FREE(working_buff_p);


  return GSDI_SUCCESS;
}

/*============================================================================
FUNCTION GSDI_PERSO_SECURITY_ENCRYPT_DATA

DESCRIPTION
  This function is called to encrypt the data provided.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_encrypt_data (
  const gsdi_perso_sec_data_type * raw_data_p,
  gsdi_perso_sec_data_type       * encrypted_data_p
)
{
  uint32   index             = 0x00;
  uint8 * working_buffer_p   = NULL;

  /* --------------------------------------------------------------------------
     Validate the Parameters Provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(raw_data_p);
  MMGSDI_RETURN_IF_NULL(raw_data_p->buffer_p);

  /* ---------------------------------------------------------------------------
     Allocate the working Buffer Size.  It must be
     2x the raw_data_p len.
     -------------------------------------------------------------------------*/
  working_buffer_p = mmgsdi_malloc(encrypted_data_p->len);
  if ( working_buffer_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL Pointer in gsdi_perso_security_encrypt_data",0,0,0);
    return GSDI_ERROR;
  }

  gsdi_perso_sec_create_control_key(working_buffer_p);
  while ( index < raw_data_p->len )
  {
    encrypted_data_p->buffer_p[index] = working_buffer_p[index] ^
                                        raw_data_p->buffer_p[index];
    index++;
  }

  /* Now Paste the Cipher Key to the end of the Data */
  memcpy((encrypted_data_p->buffer_p + index),
          working_buffer_p,
          index);
  encrypted_data_p->len = index *2;

  MMGSDIUTIL_TMC_MEM_FREE(working_buffer_p);

  return GSDI_SUCCESS;

} /* gsdi_perso_security_encrypt_data */


/* ============================================================================
FUNCTION:  GSDI_PERSO_SEC_GET_FEATURE_INDS

DESCRIPTION:
  This function will get take the encrypted feature indicators and resolve
  it accordingly based on the decryption algorithms.  The decryption
  algorithms allow for proprietary solutions to be implimented.

DEPENDENCIES:
  Data Encryption algorithm must match the data decryption algorithms.
  Otherwise, the data received will not match the expected data.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_sec_get_data(
  const gsdi_perso_sec_data_type *  encrypted_data_p,
  gsdi_perso_sec_data_type       *  decrypted_data_p
)
{
  gsdi_returns_T           gsdi_status       = GSDI_SUCCESS;

  /* -------------------------------------------------------------------------
     Validate all parameters provided
     -----------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p->buffer_p);
  MMGSDI_RETURN_IF_EXCEEDS(encrypted_data_p->len,GSDI_PERSO_SEC_ENCRYPT_MAX_LEN);
  MMGSDI_RETURN_IF_NULL(decrypted_data_p);

  gsdi_status = gsdi_perso_security_decrypt_data(encrypted_data_p,
                                                decrypted_data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Decryption Algorithm Failure",0,0,0);
    return GSDI_ERROR;
  }

  return GSDI_SUCCESS;
}



/* ============================================================================
FUNCTION:  GSDI_PERSO_SEC_ACTION_ALLOWED

DESCRIPTION:
  Function will determine if the state of the indicators is in the apporpriate
  state for the action requested.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_sec_action_allowed(
  gsdi_perso_sec_procedures_type        action,
  const gsdi_perso_sec_data_type *      features_p,
  gsdi_perso_sec_inds_state_type *      states_p
)
{
  int32 index = 0;

  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(features_p);
  MMGSDI_RETURN_IF_NULL(states_p);


  /* --------------------------------------------------------------------------
     Loop through the indicators and determine if the action is appropriate
     for the state of the indicator
     ------------------------------------------------------------------------*/
  while ( index < GSDI_PERSO_SEC_MAX_NUM_INDS)
  {
    switch ( action )
    {
      case GSDI_PERSO_SEC_PROC_ACTIVATE_FEATURE:
        switch ( features_p->buffer_p[index] )
        {

          case GSDI_PERSO_SEC_DEACTIVATE_FEATURE:
            states_p->is_allowed[index] = TRUE;
            break;

          case GSDI_PERSO_SEC_MAX_FEATURE:
          case GSDI_PERSO_SEC_ACTIVATE_FEATURE:
          case GSDI_PERSO_SEC_DISABLE_FEATURE:
          case GSDI_PERSO_SEC_AUTOLOCK_FEATURE:
            states_p->is_allowed[index] = FALSE;
            break;

          default:
            ERR_FATAL("NO CODE TO HANDLE THE REST",0,0,0);
            break;

        } /* switch */
        break;

      case GSDI_PERSO_SEC_PROC_DEACTIVATE_FEATURE:
        switch ( features_p->buffer_p[index] )
        {
          case GSDI_PERSO_SEC_ACTIVATE_FEATURE:
          case GSDI_PERSO_SEC_AUTOLOCK_FEATURE:
            states_p->is_allowed[index] = TRUE;
            break;

          case GSDI_PERSO_SEC_DEACTIVATE_FEATURE:
          case GSDI_PERSO_SEC_DISABLE_FEATURE:
          case GSDI_PERSO_SEC_MAX_FEATURE:
            states_p->is_allowed[index] = FALSE;
            break;

          default:
            ERR_FATAL("NO CODE TO HANDLE THE REST",0,0,0);
            break;

        } /* switch */
        break;

      case GSDI_PERSO_SEC_PROC_SET_FEATURE_DATA:
        switch ( features_p->buffer_p[index] )
        {

          case GSDI_PERSO_SEC_DEACTIVATE_FEATURE:
          case GSDI_PERSO_SEC_AUTOLOCK_FEATURE:
            states_p->is_allowed[index] = TRUE;
            break;

          case GSDI_PERSO_SEC_MAX_FEATURE:
          case GSDI_PERSO_SEC_ACTIVATE_FEATURE:
          case GSDI_PERSO_SEC_DISABLE_FEATURE:
            states_p->is_allowed[index] = FALSE;
            break;

          default:
            ERR_FATAL("NO CODE TO HANDLE THE REST",0,0,0);
            break;

        } /* switch */
        break;

      case GSDI_PERSO_SEC_PROC_DISABLE_FEATURE:
         if(features_p->buffer_p[index] == (int32)GSDI_PERSO_SEC_DISABLE_FEATURE)
           states_p->is_allowed[index] = FALSE;
         else
           states_p->is_allowed[index] = TRUE;
         break;

      default:
        ERR_FATAL("NO CODE TO HANDLE THE REST",0,0,0);

    } /* switch */

    index++;

  } /* while */

  return GSDI_SUCCESS;
}
/* ============================================================================
FUNCTION:  GSDI_PERSO_SEC_SET_PERSO_EVENT

DESCRIPTION:
  Sets the perso event for failure messages

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_sec_set_perso_event
(
  uint32                              feature,
  gsdi_returns_T                      gsdi_perso_status,
  gsdi_perso_event_enum_type        * perso_event_p

)
{
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  switch(gsdi_perso_status)
  {
    case GSDI_PERSO_CK_BLOCKED:
      switch(feature)
      {
#ifdef FEATURE_PERSO_SIM
       case GSDI_PERSO_SEC_MASK_FEAT_NW:
         *perso_event_p = GSDI_PERSO_NCK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_NS:
         *perso_event_p = GSDI_PERSO_NSK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_SP:
         *perso_event_p = GSDI_PERSO_SPK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_CP:
         *perso_event_p = GSDI_PERSO_CCK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_SIM:
         *perso_event_p = GSDI_PERSO_PPK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_PROP1:
         *perso_event_p = GSDI_PERSO_PROP1_BLOCKED;
          break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
         *perso_event_p = GSDI_PERSO_RUIM_NCK1_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
         *perso_event_p = GSDI_PERSO_RUIM_NCK2_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
         *perso_event_p = GSDI_PERSO_RUIM_HNCK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
         *perso_event_p = GSDI_PERSO_RUIM_SPCK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
         *perso_event_p = GSDI_PERSO_RUIM_CCK_BLOCKED;
          break;
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
         *perso_event_p = GSDI_PERSO_RUIM_PCK_BLOCKED;
          break;
#endif /* FEATURE_PERSO_RUIM */
       default:
         break;
      }
      break;
    default:
      switch(feature)
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_MASK_FEAT_NW:
          *perso_event_p = GSDI_PERSO_NW_FAILURE;
           break;
        case GSDI_PERSO_SEC_MASK_FEAT_NS:
          *perso_event_p = GSDI_PERSO_NS_FAILURE;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_SP:
          *perso_event_p = GSDI_PERSO_SP_FAILURE;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_CP:
          *perso_event_p = GSDI_PERSO_CP_FAILURE;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_SIM:
          *perso_event_p = GSDI_PERSO_SIM_FAILURE;
          break;
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
          *perso_event_p = GSDI_PERSO_RUIM_NW1_FAILURE;
           break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
          *perso_event_p = GSDI_PERSO_RUIM_NW2_FAILURE;
           break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
          *perso_event_p = GSDI_PERSO_RUIM_HRPD_FAILURE;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
          *perso_event_p = GSDI_PERSO_RUIM_SP_FAILURE;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
          *perso_event_p = GSDI_PERSO_RUIM_CP_FAILURE;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
          *perso_event_p = GSDI_PERSO_RUIM_RUIM_FAILURE;
          break;
#endif /* FEATURE_PERSO_RUIM */
        default:
          break;
      }
      break;
  }
  return GSDI_SUCCESS;
}
/* ============================================================================
FUNCTION:  GSDI_PERSO_SEC_STATE_CONSISTENT

DESCRIPTION:
  Function will determine if the state of the indicators is in the apporpriate
  state for the action requested.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_sec_state_consistent(
  uint32                                 feature,
  const gsdi_perso_sec_data_type *       feat_inds_p,
  const gsdi_perso_sec_data_type *       dck_counters_p,
  const gsdi_perso_sec_inds_state_type * features_p,
  gsdi_perso_event_enum_type           * perso_event_p
)
{
  uint8 offset          = 0x00;
  uint8 offset_status   = 0x00;
  gsdi_perso_sec_data_type count;
  gsdi_returns_T        gsdi_status = GSDI_SUCCESS;
  uint32                len =0;

  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(feat_inds_p);
  MMGSDI_RETURN_IF_NULL(dck_counters_p);
  MMGSDI_RETURN_IF_NULL(features_p);

  /* --------------------------------------------------------------------------
     Assumption is:
     the Mask will only contain 1 single Feature.  The External APIs can
     not yet handle multiple Activations at one time.
     ------------------------------------------------------------------------*/
 switch ( feature)
 {
#ifdef FEATURE_PERSO_SIM
   case GSDI_PERSO_SEC_MASK_FEAT_NW:
     offset = GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET;
     offset_status = GSDI_PERSO_SEC_NW_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_NS:
     offset = GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET;
     offset_status = GSDI_PERSO_SEC_NS_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_SP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET;
     offset_status = GSDI_PERSO_SEC_SP_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_CP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET;
     offset_status = GSDI_PERSO_SEC_CP_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_SIM:
     offset = GSDI_PERSO_SEC_IND_OFFSET_SIM_OFFSET;
     offset_status = GSDI_PERSO_SEC_SIM_STATUS_IND_OFFSET;
     break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW1_OFFSET;
     offset_status = GSDI_PERSO_SEC_RUIM_NW1_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW2_OFFSET;
     offset_status = GSDI_PERSO_SEC_RUIM_NW2_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_HRPD_OFFSET;
     offset_status = GSDI_PERSO_SEC_RUIM_HRPD_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_SP_OFFSET;
     offset_status = GSDI_PERSO_SEC_RUIM_SP_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_CP_OFFSET;
     offset_status = GSDI_PERSO_SEC_RUIM_CP_STATUS_IND_OFFSET;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
     offset = GSDI_PERSO_SEC_IND_OFFSET_RUIM_RUIM_OFFSET;
     offset_status = GSDI_PERSO_SEC_RUIM_RUIM_STATUS_IND_OFFSET;
     break;
#endif /* FEATURE_PERSO_RUIM */
   case GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT:
   default:
     offset = 0xFF;
     offset_status = 0xFF;
     return GSDI_PERSO_INVALID_DATA;

 } /* switch */

 /* -------------------------------------------------------------------------
    Decrypt the DCK Counter.
   -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 dck_counters_p,
                                                 &len,
                                                 (uint32)feature);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  count.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(count.buffer_p);

  gsdi_status = gsdi_perso_sec_get_data(dck_counters_p,
                                        &count);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(count.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

 /*Check the count*/
 if(count.buffer_p[offset_status] == GSDI_PERSO_NO_MORE_CK_RETRIES)
 {
   MSG_ERROR("PERSO: No more ck retries left",0,0,0);
   MMGSDIUTIL_TMC_MEM_FREE(count.buffer_p);
   (void)gsdi_perso_sec_set_perso_event(feature, GSDI_PERSO_CK_BLOCKED, perso_event_p);
   return GSDI_PERSO_CK_BLOCKED;
 }

 gsdi_status = gsdi_perso_sec_get_data(feat_inds_p,
                                       &count);
 if ( gsdi_status != GSDI_SUCCESS )
 {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(count.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
 }

 if(count.buffer_p[offset] == (int32)GSDI_PERSO_SEC_DISABLE_FEATURE)
 {
   MSG_ERROR("PERSO: perso sec feature disabled",0,0,0);
   MMGSDIUTIL_TMC_MEM_FREE(count.buffer_p);
   return GSDI_PERSO_PERM_DISABLED;
 }

 MMGSDIUTIL_TMC_MEM_FREE(count.buffer_p);

 if ( features_p->is_allowed[offset] == FALSE )
 {

   MMGSDI_DEBUG_MSG_ERROR("PERSO: IND STATE NOT CONSISTENT STATE: 0x%x  NOT ALLOWED: 0x%x",
                          features_p->inds[offset], features_p->is_allowed[offset],0);

   return GSDI_PERSO_INCONSISTENT_W_IND;
 }

 return GSDI_SUCCESS;

}


/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_CONTROL_KEY

DESCRIPTION:
  Function will be used to extract the control key from the encrypted data
  provided.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_sec_get_control_key(
  const gsdi_perso_sec_client_req_type * req_data_p,
  const gsdi_perso_sec_data_type       * encrypted_control_keys_p,
  gsdi_perso_sec_data_type             * data_p
)
{
  gsdi_returns_T            gsdi_status = GSDI_SUCCESS;
  gsdi_perso_sec_data_type  enc_ck;
  int32                     offset      = 0;
  uint32                    pos         = 0;
  uint32                    len         = 0;
  uint32                    i           = 0;

  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(data_p);
  MMGSDI_RETURN_IF_NULL(data_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(encrypted_control_keys_p);
  MMGSDI_RETURN_IF_NULL(encrypted_control_keys_p->buffer_p);

  /* --------------------------------------------------------------------------
     Based on the Mask provided, calculate the starting offset of the
     control key
     ------------------------------------------------------------------------*/
 offset = gsdi_perso_sec_get_offset(req_data_p->feature_mask);
 if(offset == 0xFFFF)
 {
   return GSDI_PERSO_INVALID_DATA;
 }

 /*Increment the length by the TLV of the previous features plus the size of TL of the current feature.*/
 pos = GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;

 for(i =0;i<(uint32)offset;i++)
 {
   len = encrypted_control_keys_p->buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN] ;   // lo_len
   len += encrypted_control_keys_p->buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] *255; // hi_len
   pos += len;
   pos += GSDI_PERSO_ENGINE_TAG_SIZE;
   pos += GSDI_PERSO_ENGINE_LEN_SIZE;
 }

 /* ---------------------------------------------------------------------------
    Allocate memory for the working copy of the encrypted data
    which will then be decrypted
    -------------------------------------------------------------------------*/
 enc_ck.len      = GSDI_PERSO_SEC_ENCRYPT_CK_LEN;
 enc_ck.buffer_p = mmgsdi_malloc(GSDI_PERSO_SEC_ENCRYPT_CK_LEN);
 if ( enc_ck.buffer_p == NULL )
 {
   MMGSDI_DEBUG_MSG_ERROR("PERSO: NULL pointer in gsdi_perso_sec_get_control_key",0,0,0);
   return GSDI_ERROR;
 }

 /* ---------------------------------------------------------------------------
    Using the offset provided, copy the encrypted control key
    information over from the data
    -------------------------------------------------------------------------*/
 memcpy(enc_ck.buffer_p,
        (encrypted_control_keys_p->buffer_p + pos),
        GSDI_PERSO_SEC_ENCRYPT_CK_LEN);

 /* ---------------------------------------------------------------------------
    Now decrypt the encrypted control key
    -------------------------------------------------------------------------*/
 gsdi_status = gsdi_perso_security_decrypt_data(&enc_ck,
                                                data_p);

 if ( gsdi_status != GSDI_SUCCESS )
 {
   MMGSDIUTIL_TMC_MEM_FREE(enc_ck.buffer_p);
   return GSDI_PERSO_INVALID_DATA;
 }

 /* ---------------------------------------------------------------------------
    Free up any memory allocated in this procedure
    -------------------------------------------------------------------------*/
 MMGSDIUTIL_TMC_MEM_FREE(enc_ck.buffer_p);

 return GSDI_SUCCESS;

} /* gsdi_perso_sec_get_control_key */



/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_COMPARE_KEYS

DESCRIPTION:
  Function called to perform CK Comparisons.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_compare_keys(
  const gsdi_perso_sec_client_req_type * req_data_p,
  const gsdi_perso_sec_data_type       * control_keys_p
)
{
  /* --------------------------------------------------------------------------
     Perform the validation of both parameters
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(control_keys_p);

  /* --------------------------------------------------------------------------
     First compare the lengths of the two keys
     ------------------------------------------------------------------------*/
  if ( req_data_p->dck.len != control_keys_p->len )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Control Key Failure, bad lengths 0x%x != 0x%x",
                           req_data_p->dck.len, control_keys_p->len, 0);
    return GSDI_PERSO_INVALID_CK;
  }

  /* --------------------------------------------------------------------------
     Lengths check out...verify the data
     ------------------------------------------------------------------------*/
  if ( !(memcmp(req_data_p->dck.buffer_p,
                control_keys_p->buffer_p,
                GSDI_PERSO_SEC_MAX_DCK_LEN )
     )  )
  {
  /* ---------------------------------------------------------------------------
     Personalization Control Keys Match,
     the verification and comparison process was
     successful
     -------------------------------------------------------------------------*/
    return GSDI_SUCCESS;
  }

  MSG_ERROR("PERSO: Control Key Failure Check...wrong DCK",0,0,0);
  return GSDI_PERSO_INVALID_CK;

}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_UPDATE_COUNTERS

DESCRIPTION
  Decrease the incorrect CK Count

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T  gsdi_perso_security_update_counters(
  const gsdi_perso_sec_client_req_type    * req_data_p,
  gsdi_perso_sec_data_type                * encrypted_ck_counters_p,
  gsdi_perso_sec_data_mod_inds_type       * mod_feat_inds_p
)
{
  gsdi_returns_T             gsdi_status   = GSDI_SUCCESS;
  int32                      offset        = 0x00;
  gsdi_perso_sec_data_type   decrypted_data;
  uint8                      retries       = 0;

  /* --------------------------------------------------------------------------
     Validate the Parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_ck_counters_p);
  MMGSDI_RETURN_IF_NULL(mod_feat_inds_p);

  /* --------------------------------------------------------------------------
     Based on the Mask provided, calculate the starting offset of the
     control key counter
     ------------------------------------------------------------------------*/
  offset = gsdi_perso_sec_get_offset(req_data_p->feature_mask);
  if (offset == 0xFFFF)
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  /* ---------------------------------------------------------------------------
    Allocate memory to hold the uncrypted values
    -------------------------------------------------------------------------*/
  decrypted_data.buffer_p = mmgsdi_malloc((encrypted_ck_counters_p->len/2));
  MMGSDI_RETURN_IF_NULL(decrypted_data.buffer_p);

  /* ---------------------------------------------------------------------------
    Get the Control Key Counters
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_decrypt_data(encrypted_ck_counters_p,
                                                 &decrypted_data);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
    return gsdi_status;
  }

  /* Get number of retries */
  retries = decrypted_data.buffer_p[offset];

  /* ---------------------------------------------------------------------------
    If not currently blocked...ok to decrement
    -------------------------------------------------------------------------*/
  if (retries != GSDI_PERSO_NO_MORE_CK_RETRIES)
  {
    retries--;
    decrypted_data.buffer_p[offset] = retries;

    /* ---------------------------------------------------------------------------
      Write data back to the file
      -------------------------------------------------------------------------*/
    /* Encrypt the Data Accordingly */
    gsdi_status = gsdi_perso_security_encrypt_data(&decrypted_data,
                                                   encrypted_ck_counters_p);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
      return gsdi_status;
    }

    /* --------------------------------------------------------------------------
       Indicate which data was modified so that the personalization
       engine can write back to FS.
       ------------------------------------------------------------------------*/
    mod_feat_inds_p->update_fs = TRUE;
  }

  /* ---------------------------------------------------------------------------
    1.  Check to see if it is already blocked
    -------------------------------------------------------------------------*/
  if ( retries == GSDI_PERSO_NO_MORE_CK_RETRIES )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: CK for Feature 0x%x BLOCKED/PERM BLOCKED",
                           req_data_p->feature_mask,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
    return GSDI_PERSO_CK_BLOCKED;
  }

  MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
  return gsdi_status;
} /* gsdi_perso_security_update_counters */


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_RESET_COUNTERS

DESCRIPTION
  Reset the CK Count

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T  gsdi_perso_security_reset_counters(
  const gsdi_perso_sec_client_req_type    * req_data_p,
  gsdi_perso_sec_data_type                * encrypted_ck_counters_p,
  gsdi_perso_sec_data_mod_inds_type       * mod_feat_inds_p,
  uint8                                     num_retries
)
{
  gsdi_returns_T             gsdi_status   = GSDI_SUCCESS;
  int32                      offset        = 0x00;
  gsdi_perso_sec_data_type   decrypted_data;

  /* --------------------------------------------------------------------------
     Validate the Parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_ck_counters_p);
  MMGSDI_RETURN_IF_NULL(mod_feat_inds_p);

  /* --------------------------------------------------------------------------
     Based on the Mask provided, calculate the starting offset of the
     control key counter
     ------------------------------------------------------------------------*/
  offset = gsdi_perso_sec_get_offset(req_data_p->feature_mask);
  if (offset == 0xFFFF)
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  /* ---------------------------------------------------------------------------
    Allocate memory to hold the uncrypted values
    -------------------------------------------------------------------------*/
  decrypted_data.buffer_p = mmgsdi_malloc((encrypted_ck_counters_p->len/2));
  MMGSDI_RETURN_IF_NULL(decrypted_data.buffer_p);

  /* ---------------------------------------------------------------------------
    Get the Control Key Counters
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_decrypt_data(encrypted_ck_counters_p,
                                                 &decrypted_data);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
    return gsdi_status;
  }

  decrypted_data.buffer_p[offset] = num_retries;

  /* ---------------------------------------------------------------------------
    Write data back to the file
    -------------------------------------------------------------------------*/
  /* Encrypt the Data Accordingly */
  gsdi_status = gsdi_perso_security_encrypt_data(&decrypted_data,
                                                 encrypted_ck_counters_p);
  if (gsdi_status == GSDI_SUCCESS)
  {
    /* --------------------------------------------------------------------------
       Indicate which data was modified so that the personalization
       engine can write back to FS.
       ------------------------------------------------------------------------*/
    mod_feat_inds_p->update_fs = TRUE;
  }

  MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
  return gsdi_status;
} /* gsdi_perso_security_reset_counters */


/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_VERIFY_CK

DESCRIPTION:
  Function called to verify the control key when a perso
  feature is activated/deactivated/unblocked.
  This function will write back to the NV or the Secure
  file system the number of remaining attempts, therefore,
  the caller doesn't have to try and save any of the
  information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_verify_ck(
  const gsdi_perso_sec_client_req_type    * req_data_p,
  const gsdi_perso_sec_data_type          * encrypted_control_keys_p,
  gsdi_perso_sec_data_type                * encrypted_ck_counters_p,
  gsdi_perso_sec_data_mod_inds_type       * mod_feat_inds_p,
  gsdi_perso_sec_data_type                * feature_control_key_p,
  gsdi_perso_event_enum_type              * perso_event_p,
  uint8                                     num_retries_ok
)
{
  gsdi_returns_T           gsdi_status = GSDI_SUCCESS;
  gsdi_returns_T           act_gsdi_status = GSDI_SUCCESS;

  MSG_HIGH("PERSO: gsdi_perso_security_verify_ck",0,0,0);

  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(req_data_p->dck.buffer_p);
  MMGSDI_RETURN_IF_NULL(encrypted_control_keys_p);
  MMGSDI_RETURN_IF_NULL(encrypted_control_keys_p->buffer_p);
  MMGSDI_RETURN_IF_NULL(mod_feat_inds_p);
  MMGSDI_RETURN_IF_NULL(feature_control_key_p);
  MMGSDI_RETURN_IF_NULL(feature_control_key_p->buffer_p);

  /* --------------------------------------------------------------------------
    Extract the control key from the Request provided and
    store a decrypted value in the feature_control_key_p buffer in this
    structure.
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_control_key(req_data_p,
                                               encrypted_control_keys_p,
                                               feature_control_key_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Extraction of the control key failed",0,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Perform the Verification / Comparison Procedures
     ------------------------------------------------------------------------*/
  act_gsdi_status = gsdi_perso_security_compare_keys(req_data_p,
                                                     feature_control_key_p);

  if ( act_gsdi_status == GSDI_SUCCESS )
  {
    /* At this point, the comparison of the Control Key was successfully
       completed, so the counter needs to be reset to its original value. */
    act_gsdi_status = gsdi_perso_security_reset_counters(
                                       req_data_p,
                                       encrypted_ck_counters_p,
                                       mod_feat_inds_p,
                                       num_retries_ok);
    return act_gsdi_status;
  }


  /* --------------------------------------------------------------------------
     At this point, it has been determined that the Control Key
     Comparison Method failed.  Thus, the counters need to be updated
     accordingly
     ------------------------------------------------------------------------*/
  MMGSDI_DEBUG_MSG_ERROR("PERSO: CK Comparison Failure",0,0,0);

  /* Need to perform the appropriate updates to the Control Key Counters */
  gsdi_status = gsdi_perso_security_update_counters(req_data_p,
                                                    encrypted_ck_counters_p,
                                                    mod_feat_inds_p);
  if ( gsdi_status != GSDI_SUCCESS &&
       gsdi_status != GSDI_PERSO_CK_BLOCKED )
  {
    /* Failure occurred trying to update the counters */
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failure to update counters 0x%x",
                           gsdi_status,0,0);
  }

  if(gsdi_status == GSDI_PERSO_CK_BLOCKED)
  {
    (void)gsdi_perso_sec_set_perso_event(req_data_p->feature_mask, gsdi_status, perso_event_p);
  }

  return act_gsdi_status;

} /* gsdi_perso_security_verify_ck */


/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_CHANGE_FEATURE_INDS

DESCRIPTION:
  Function called to perform the activation of the Feature.
  This function will write back to the NV or the Secure
  file system.  Therefore, the caller doesn't have to try
  and save any of the information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_change_feature_inds(
  uint32                                     feature_mask,
  gsdi_perso_sec_me_data_type          *     enc_me_data_p,
  gsdi_perso_sec_inds_state_enum_type        feature_state
)
{
  gsdi_perso_sec_data_type decrypted_data;
  gsdi_returns_T           gsdi_status     = GSDI_SUCCESS;
  int32                    offset          = 0;
  uint32                   len             = 0;
  uint32                   pos             = 0;
  uint32                   i               = 0;

  MSG_HIGH(" PERSO: gsdi_perso_security_change_feature_inds feature_state 0x%x",
           feature_state,0,0);
  /* --------------------------------------------------------------------------
     Validate the Parameters provided.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(enc_me_data_p);

  /* --------------------------------------------------------------------------
     Calculate the required offset into the uncrypted data
     ------------------------------------------------------------------------*/
  offset = gsdi_perso_sec_get_offset(feature_mask);
  if(offset == 0xFFFF)
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Allocate a small buffer to hold the uncrypted data
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &enc_me_data_p->feat_inds,
                                                 &len,
                                                 feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  decrypted_data.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(decrypted_data.buffer_p);

  /* --------------------------------------------------------------------------
     Decrypt the data provided in the ME Encrypted
     Data Structure and set the feature indicator
     to the new state.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_decrypt_data(&enc_me_data_p->feat_inds,
                                                 &decrypted_data);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /*If the feature is to be activated but there are no SIM Lock Codes, go to the Auto Lock State.
    On Power Up, the Initialization function will create the sim lock codes.*/
  if(feature_state == GSDI_PERSO_SEC_ACTIVATE_FEATURE)
  {
    do
    {
      pos += GSDI_PERSO_ENGINE_TAG_SIZE + GSDI_PERSO_ENGINE_LEN_SIZE;
      len = enc_me_data_p->sim_lock_codes.buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN];   /*lo_len*/
      len += enc_me_data_p->sim_lock_codes.buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] *255; /* hi_len */
      pos += len;
      i++;
    }
    while(i<=(uint32)offset);

    if(len == 0)
    {
      decrypted_data.buffer_p[(uint32)offset] = (int32)GSDI_PERSO_SEC_AUTOLOCK_FEATURE;
    }
    else
    {
      decrypted_data.buffer_p[(uint32)offset] = (int32)feature_state;
    }
  }
  else
  {
    decrypted_data.buffer_p[(uint32)offset] = (int32)feature_state;
  }

  /* -------------------------------------------------------------------------
     Encrypt the data and place it back into the
     Enccrypted ME Data Structure so that it can
     be written to FS by the Personalization Engine.
     ------------------------------------------------------------------------*/

  gsdi_status = gsdi_perso_security_encrypt_data(&decrypted_data,
                                                 &enc_me_data_p->feat_inds);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    gsdi_status = GSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SEC_GET_INDICATOR

DESCRIPTION
  This function will get the indicator for the feature given the mask

DEPENDENCIES
  None

RETURN VALUE
  uint8 (indicator)
===========================================================================*/
static uint32 gsdi_perso_sec_get_indicator
(
  uint32 feature_mask
)
{
  uint32 indicator = 0;
  switch ( feature_mask)
  {
#ifdef FEATURE_PERSO_SIM
   case GSDI_PERSO_SEC_MASK_FEAT_NW:
     indicator = GSDI_PERSO_NW_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_NS:
     indicator = GSDI_PERSO_NS_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_SP:
     indicator = GSDI_PERSO_SP_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_CP:
     indicator = GSDI_PERSO_CP_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_SIM:
     indicator = GSDI_PERSO_SIM_INDICATOR;
     break;
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
     indicator = GSDI_PERSO_RUIM_NW1_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
     indicator = GSDI_PERSO_RUIM_NW2_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
     indicator = GSDI_PERSO_RUIM_HRPD_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
     indicator = GSDI_PERSO_RUIM_SP_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
     indicator = GSDI_PERSO_RUIM_CP_INDICATOR;
     break;

   case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
     indicator = GSDI_PERSO_RUIM_RUIM_INDICATOR;
     break;
#endif /*FEATURE_PERSO_RUIM*/
    default:
     indicator = GSDI_PERSO_INDICATOR_NONE;
     break;

  } /* switch */
  return indicator;

}


/* ============================================================================
FUNCTION:  GSDI_PERSO_SEC_CHANGE_SIM_LOCK_CODE

DESCRIPTION:
  Function called to perform the activation of the Feature.
  This function will write back to the NV or the Secure
  file system.  Therefore, the caller doesn't have to try
  and save any of the information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_sec_change_sim_lock_code(
  const gsdi_perso_sec_client_req_type *     req_data_p,
  gsdi_perso_sec_me_data_type          *     enc_me_data_p
)
{
  gsdi_perso_sec_data_type enc_ck;
  gsdi_perso_sec_data_type enc_ck_dup;
  gsdi_returns_T           gsdi_status     = GSDI_SUCCESS;
  uint32                    feature_len     = 0;
  uint32                    pos             = 0;
  int32                     offset          = 0;
  uint32                    i               = 0;
  uint32                    len             = 0;

  MSG_HIGH("PERSO: gsdi_perso_sec_change_sim_lock_code",0,0,0);

  /* --------------------------------------------------------------------------
     Validate the Parameters provided.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(enc_me_data_p);


  /* --------------------------------------------------------------------------
     Calculate the required offset into the uncrypted data
     ------------------------------------------------------------------------*/
  offset = gsdi_perso_sec_get_offset(req_data_p->feature_mask);
  if(offset == 0xFFFF)
  {
      return GSDI_PERSO_INVALID_DATA;
  }
     /*Increment the length by the TLV of the previous features plus the size of TL of the current feature.*/
  pos = 0;

  do
  {
    pos += GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;
    len = enc_me_data_p->sim_lock_codes.buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN];   //lo_len
    len += enc_me_data_p->sim_lock_codes.buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] *255; // hi_len
    pos += len;
    i++;
  }
  while(i<=(uint32)offset);

  feature_len = req_data_p->feature_data.len *2;
  pos -= len;

  /* ---------------------------------------------------------------------------
   Allocate memory for the working copy of the encrypted data
   which will then be decrypted
    -------------------------------------------------------------------------*/
   enc_ck.len      = feature_len ;
   enc_ck.buffer_p = mmgsdi_malloc(feature_len);
   MMGSDI_RETURN_IF_NULL(enc_ck.buffer_p);

 /* -------------------------------------------------------------------------
     Encrypt the data and place it back into the
     Enccrypted ME Data Structure so that it can
     be written to FS by the Personalization Engine.
     ------------------------------------------------------------------------*/

  gsdi_status = gsdi_perso_security_encrypt_data(&req_data_p->feature_data,
                                                 &enc_ck);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(enc_ck.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* ---------------------------------------------------------------------------
    Using the offset provided, copy the encrypted control key
    information over from the data
    -------------------------------------------------------------------------*/
  enc_ck_dup.buffer_p = mmgsdi_malloc(enc_me_data_p->sim_lock_codes.len);
  if(enc_ck_dup.buffer_p  == NULL)
  {
    MSG_ERROR("PERSO: Could not allocate memory",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(enc_ck.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(enc_ck_dup.buffer_p,enc_me_data_p->sim_lock_codes.buffer_p, enc_me_data_p->sim_lock_codes.len);

  /*Set the feature code*/
  enc_me_data_p->sim_lock_codes.buffer_p[pos - (GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE)] =
     (uint8)gsdi_perso_sec_get_indicator(req_data_p->feature_mask);

  memcpy((enc_me_data_p->sim_lock_codes.buffer_p + pos),
           enc_ck.buffer_p,
           feature_len);


  /* Copy the Original data that was in the dup back to enc_me_data_p,
     after the pos and the new feature len value. We need to copy remaining
     enc_ck_dup data of (sim_lock_codes.len -(pos + len)) length */

  memcpy((enc_me_data_p->sim_lock_codes.buffer_p + pos +feature_len),
          enc_ck_dup.buffer_p + pos + len,
          enc_me_data_p->sim_lock_codes.len - (pos + len));


  enc_me_data_p->sim_lock_codes.buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] = (uint8)(feature_len/255);        //hi_len
  enc_me_data_p->sim_lock_codes.buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN]   = (uint8)(feature_len %255);       //lo_len

  /* Update the sim_lock_codes.len by adding newer data length (feature_len) and by subtracting
     old data length (len). */
  enc_me_data_p->sim_lock_codes.len = enc_me_data_p->sim_lock_codes.len + feature_len - len;

  /* If newer data length is smaller than the older data length then remove
     junk values from last  */
  if (len > feature_len)
  {
    memset((enc_me_data_p->sim_lock_codes.buffer_p + enc_me_data_p->sim_lock_codes.len),
           0x00, len - feature_len);
  }

  MMGSDIUTIL_TMC_MEM_FREE(enc_ck.buffer_p);
  MMGSDIUTIL_TMC_MEM_FREE(enc_ck_dup.buffer_p);

  return gsdi_status;
}/* gsdi_perso_sec_change_sim_lock_code */


#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_SEC_VALIDATE_DATA_AGAINST_FS

DESCRIPTION
  This function is used to validate the Date provided against the
  Codes stored in FS.  If a match is not found, then an error is returned.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_validate_data_against_fs(
  const gsdi_perso_sec_client_req_type  * req_data_p,
  const gsdi_perso_sec_me_data_type     * encrypted_data_p,
  int32 *                                 ret_data_len_p,
  uint8 *                                 data_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_ERROR;
  uint32         i             = 0;
  uint32         j             = 0;
  uint32         i_data        = 0;
  uint32         data_offset   = 0;
  uint32         fs_offset     = 0;
  uint32         j_fs          = 0;
  uint32         len           = 0;
  uint32         validate_num  = 0;
  boolean        match_found   = FALSE;
  uint32         mask          = 0xFF;
  gsdi_perso_sec_data_type       features;

  /* --------------------------------------------------------------------------
     Perform Basic Parameter Checking
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);

  if (req_data_p->feature_data.len  >= GSDI_MAX_DATA_BLOCK_LEN)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Wrong data length 0x%x",req_data_p->feature_data.len,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  switch ( req_data_p->feature_mask )
  {

#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_SEC_MASK_FEAT_NS:
    case GSDI_PERSO_SEC_MASK_FEAT_SP:
      mask = GSDI_PERSO_SEC_MASK_FEAT_NW;
      data_offset = 4;
      fs_offset   = 3;
      break;

    case GSDI_PERSO_SEC_MASK_FEAT_CP:
      mask = GSDI_PERSO_SEC_MASK_FEAT_SP;
      data_offset = 5;
      fs_offset   = 4;
      break;
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
      mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
      data_offset = 4;
      fs_offset   = 3;
      break;
#endif /*FEATURE_PERSO_RUIM*/

    default:
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Unknown Feature: 0x%x",
                             req_data_p->feature_mask,0,0);
      return GSDI_PERSO_INVALID_DATA;
  }

    /* --------------------------------------------------------------------------
     Validate validate_num_bytes is of the correct length.  The number
     of bytes must be properly divisable by:
     NS: 4 Bytes
     SP: 4 Bytes
     CP: 5 Bytes
     ------------------------------------------------------------------------*/
  if ( req_data_p->feature_data.len % data_offset )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Invalid data length", 0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  else
  {
    validate_num = req_data_p->feature_data.len / data_offset;
  }

/* --------------------------------------------------------------------------
     Descrypt the NW Codes from FS
 ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_DATA,
                                                 &encrypted_data_p->sim_lock_codes,
                                                 &len,
                                                 mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  if(len == 0)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Wrong length 0x%x",len,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);


  gsdi_status = gsdi_perso_sec_decrypt_tlv(mask,
                                           &encrypted_data_p->sim_lock_codes,
                                           &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(data_p,&features.buffer_p, len);
  *ret_data_len_p =(int32)len;

  /* --------------------------------------------------------------------------
     Validate the returned_data_len_p from FS is properly divisable by
     the correct number:
     NS:  3
     SP:  3
     CP:  4
     ------------------------------------------------------------------------*/
  if ( len % fs_offset )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not read correct number of bytes: 0x%x",
                           len,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }
  else
  {
    /* Update the loop control */
    len = len / fs_offset;
  }



  /* --------------------------------------------------------------------------
     data_p now contains the FS Contents of the NW Codes.  Check the data
     provided against this data
     ------------------------------------------------------------------------*/
  while (i <  validate_num ||
         i == GSDI_MAX_DATA_BLOCK_LEN )
  {
    i_data = i * data_offset;

    match_found = FALSE;
    while ( j < len )
    {
      j_fs = j * fs_offset;

      /* Compare the memory */
      if ( !(memcmp(&req_data_p->feature_data.buffer_p[i_data],
                    &features.buffer_p[j_fs],fs_offset)) )
      {
        match_found = TRUE;
        break;
      }
      j++;
    }
    /*No need to test any further, the entry was not found*/
    if ( match_found == FALSE )
    {
      break;
    }
    /* Reset Internal Loop */
    j = 0;
    i++;
  }

  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

  if ( match_found )
  {
    return GSDI_SUCCESS;
  }
  else
  {
    return GSDI_PERSO_INVALID_DATA;
  }
}
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_SIM

/*===========================================================================
FUNCTION GSDI_PERSO_SEC_VALIDATE_IMSI

DESCRIPTION
  This function is used to validate the IMSI Data against the NETWORK and
  Netowrk Subset codes stored in FS.  If no match is found, an error is
  returned.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_validate_imsi(
  const gsdi_perso_sec_client_req_type    * req_data_p,
  const gsdi_perso_sec_me_data_type       * encrypted_data_p,
  int32 *                                   ret_data_len_p,
  uint8 *                                   data_p,
  boolean                                   umts
)
{
  gsdi_returns_T gsdi_status   = GSDI_ERROR;
  uint32         i             = 0;
  uint32         j             = 0;
  uint32         i_data        = 0;
  uint32         j_fs          = 0;
  boolean        match_found   = FALSE;
  uint32         len           = 0;
  gsdi_perso_sec_data_type       sim_lock_codes_p;
  uint32         req_len;
  uint32         feature_mask = 0;

  /* --------------------------------------------------------------------------
     Perform Basic Parameter Checking
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);

  if (req_data_p->feature_data.len  >= GSDI_MAX_DATA_BLOCK_LEN)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Data len: 0x%x is >= GSDI_MAX_DATA_BLOCK_LEN",
                           req_data_p->feature_data.len,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Validate validate_num_bytes is of the correct length.  The number
     of bytes must be properly divisable by:
     NW: 3
     ------------------------------------------------------------------------*/
  if ( req_data_p->feature_data.len % 8)
  {
    return GSDI_INCORRECT_PARAMS;
  }
  else
  {
    req_len = req_data_p->feature_data.len / 8;
  }
  /* --------------------------------------------------------------------------
     Read the NW Codes from FS
     ------------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_SIM
  if(umts)
    feature_mask = GSDI_PERSO_SEC_MASK_FEAT_NW;
#endif
#ifdef FEATURE_PERSO_RUIM
  if(!umts)
    feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
#endif
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_DATA,
                                                 &encrypted_data_p->sim_lock_codes,
                                                 &len,
                                                 feature_mask);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  sim_lock_codes_p.buffer_p = mmgsdi_malloc(len);

  MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);

  gsdi_status = gsdi_perso_sec_decrypt_tlv(feature_mask,
                                           &encrypted_data_p->sim_lock_codes,
                                           &sim_lock_codes_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(sim_lock_codes_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Validate the returned_data_len_p from FS is properly divisable by
     the correct number:
     NW:  3
     ------------------------------------------------------------------------*/
  *ret_data_len_p = uint32toint32(len);

  if ( *ret_data_len_p % 3 )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not read correct number of bytes: 0x%x",
                           *ret_data_len_p,0,0);
    MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }
  else
  {
    *ret_data_len_p = *ret_data_len_p / 3;
  }

  memcpy(data_p, sim_lock_codes_p.buffer_p,len);
/* --------------------------------------------------------------------------
   data_p now contains the FS Contents of the NW Codes.  Check the data
   provided against this data
 ------------------------------------------------------------------------*/
  while (i <  req_len ||
         i == GSDI_MAX_DATA_BLOCK_LEN )
  {
    i_data = i * 8;
    match_found = FALSE;
    while ( j < int32touint32(*ret_data_len_p) )
    {
      j_fs = j * 3;

      /* Compare the memory */
      if ( !(memcmp(&req_data_p->feature_data.buffer_p[i_data],
                    &data_p[j_fs],3)) )
      {
        match_found = TRUE;
        break;
      }
      j++;
    }
    if ( match_found == FALSE )
    {
      /*The current data is not a match, no need to check further*/
      break;
    }
    /* Reset Internal Loop */
    j = 0;
    i++;
  }

  if ( match_found == FALSE )
  {
    MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  MMGSDIUTIL_TMC_MEM_FREE(sim_lock_codes_p.buffer_p);

#ifdef FEATURE_PERSO_SIM
  if(umts)
    feature_mask = GSDI_PERSO_SEC_MASK_FEAT_NS;
#endif
#ifdef FEATURE_PERSO_RUIM
   if(!umts)
     feature_mask = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2;
#endif
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_DATA,
                                                 &encrypted_data_p->sim_lock_codes,
                                                 &len,
                                                 feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  sim_lock_codes_p.buffer_p = mmgsdi_malloc(len);

  MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);

  gsdi_status = gsdi_perso_sec_decrypt_tlv(feature_mask,
                                           &encrypted_data_p->sim_lock_codes,
                                           &sim_lock_codes_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(sim_lock_codes_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  *ret_data_len_p = uint32toint32(len);

  if ( *ret_data_len_p % 4 )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not read correct number of bytes: 0x%x",
                           *ret_data_len_p,0,0);
    MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }
  else
  {
    *ret_data_len_p = *ret_data_len_p / 4;
  }

  //Skip the length field as well.
  memcpy(data_p, sim_lock_codes_p.buffer_p,len);

  /* --------------------------------------------------------------------------
    data_p now contains the FS Contents of the NS Codes.  Check the data
    provided against this data
 ------------------------------------------------------------------------*/
  i = 0;
  while (i <  req_len ||
         i == GSDI_MAX_DATA_BLOCK_LEN )
  {
    i_data = i * 8;
    match_found = FALSE;
    while ( j < int32touint32(*ret_data_len_p) )
    {
      j_fs   = j * 4;

      /* Compare the memory */
      if ( !(memcmp(&req_data_p->feature_data.buffer_p[i_data],
                    &data_p[j_fs],4)) )
      {
        match_found = TRUE;
        break;
      }
      j++;
    }
    if ( match_found == FALSE)
    {
      /*the current data did not match, no need to look further*/
      break;
    }
    /* Reset Internal Loop */
    j = 0;
    i++;
  }
  if ( match_found )
  {
    MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);
    return GSDI_SUCCESS;
  }
  else
  {
    MSG_ERROR("PERSO: IMSI doesn't match",0,0,0);
    MMGSDI_RETURN_IF_NULL(sim_lock_codes_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }
}
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_SP_CODES

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
static gsdi_returns_T gsdi_perso_sec_check_sim_sp_codes(
    const gsdi_perso_sec_sim_data_type    * sim_data_p,
    const gsdi_perso_sec_data_type        * features
)
{
  uint8                  data_p[GSDI_PERSO_SEC_SP_CODE_LEN];
  gsdi_returns_T         gsdi_status    = GSDI_SUCCESS;
  uint32                 i              = 0;
  uint32                 loop_control   = 0;
  boolean                mismatch       = TRUE;

  MMGSDI_RETURN_IF_NULL(features);
  MMGSDI_RETURN_IF_NULL(sim_data_p);

  if ( (features->len % GSDI_PERSO_SEC_SP_CODE_LEN ) != 0)
  {
     /* Badly formatted Codes */
     return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(data_p,sim_data_p->imsi.buffer_p,3);

  if ( sim_data_p->gid1.len  > 0 &&
        data_p[0] != 0xFF )
  {
    data_p[3] = ((sim_data_p->gid1.buffer_p[0] << GSDI_PERSO_SEC_SP_CODE_LEN) & 0xF0) |
                 ((sim_data_p->gid1.buffer_p[0] >> GSDI_PERSO_SEC_SP_CODE_LEN) & 0x0F);

   /* --------------------------------------------------------------------------
     Now check these codes against those stored in the codes file stored
     in the file system

     From the returned data Len calculate the number of codes to be check
     *returned_data_len_p / GSDI_PERSO_SEC_SP_CODE_LEN -> loop control index
     *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
     ------------------------------------------------------------------------*/
     loop_control = features->len / GSDI_PERSO_SEC_SP_CODE_LEN;

     for (i = 0; i < loop_control; i++ )
     {
       if ( memcmp(&features->buffer_p[i * GSDI_PERSO_SEC_SP_CODE_LEN],data_p,
                   GSDI_PERSO_SEC_SP_CODE_LEN) )
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

  if (mismatch)
    gsdi_status = GSDI_PERSO_INVALID_DATA;

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_CP_CODES

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
static gsdi_returns_T gsdi_perso_sec_check_cp_sim_codes(
    const gsdi_perso_sec_sim_data_type    * sim_data_p,
    const gsdi_perso_sec_data_type        * features
)
{
  uint8                 data_p[GSDI_PERSO_SEC_CP_CODE_LEN];
  gsdi_returns_T        gsdi_status    = GSDI_SUCCESS;
  uint32                i              = 0;
  uint32                loop_control   = 0;
  boolean               mismatch       = TRUE;


  MMGSDI_RETURN_IF_NULL(features);
  MMGSDI_RETURN_IF_NULL(sim_data_p);

  if ( (features->len % GSDI_PERSO_SEC_CP_CODE_LEN ) != 0)
  {
     /* Badly formatted Codes */
     return GSDI_PERSO_INVALID_DATA;
  }
  memcpy(data_p,sim_data_p->imsi.buffer_p,3);

  if ( sim_data_p->gid1.len > 0)
  {
    data_p[3] = ((sim_data_p->gid1.buffer_p[0] << 4) & 0xF0) |
                ((sim_data_p->gid1.buffer_p[0] >> 4) & 0x0F);

    if ( sim_data_p->gid2.len > 0)
    {
      data_p[GSDI_PERSO_SEC_CP_CODE_LEN-1] = ((sim_data_p->gid2.buffer_p[0] << 4) & 0xF0 ) |
                                           ((sim_data_p->gid2.buffer_p[0] >> 4) & 0x0F);


      /* --------------------------------------------------------------------------
        Now check these codes against those stored in the codes file stored
        in the file system

        From the returned data Len calculate the number of codes to be check
        *returned_data_len_p / GSDI_PERSO_SEC_CP_CODE_LEN -> loop control index
        *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
       ------------------------------------------------------------------------*/
        loop_control = features->len / GSDI_PERSO_SEC_CP_CODE_LEN;

        for (i = 0; i < loop_control; i++ )
        {
          if ( memcmp(&features->buffer_p[i * GSDI_PERSO_SEC_CP_CODE_LEN],data_p,
                       GSDI_PERSO_SEC_CP_CODE_LEN) )
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
  }

  if ( mismatch)
    gsdi_status = GSDI_PERSO_INVALID_DATA;

  return gsdi_status;
}
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_RUIM_NW2_CODES

DESCRIPTION
  This function is a function used to read the RUIM NW2 codes and check them
  against the contents stored in FS.


DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_check_ruim_nw2_codes(
    const gsdi_perso_sec_sim_data_type    * sim_data_p,
    const gsdi_perso_sec_data_type        * features
)
{
  uint8                  data_p[GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN];
  gsdi_returns_T         gsdi_status    = GSDI_SUCCESS;
  uint32                 i              = 0;
  uint32                 loop_control   = 0;
  boolean                mismatch       = FALSE;

  MMGSDI_RETURN_IF_NULL(features);
  MMGSDI_RETURN_IF_NULL(sim_data_p);

  if ( (features->len % GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN ) != 0)
  {
     /* Badly formatted Codes */
     return GSDI_PERSO_INVALID_DATA;
  }
  MMGSDI_RETURN_IF_NULL(sim_data_p->imsi.buffer_p);
  memcpy(data_p, sim_data_p->imsi.buffer_p+GSDI_PERSO_SEC_RUIM_NW1_CODE_LEN,
         GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN);


   /* --------------------------------------------------------------------------
     Now check these codes against those stored in the codes file stored
     in the file system

     From the returned data Len calculate the number of codes to be check
     *returned_data_len_p / GSDI_PERSO_SEC_SP_CODE_LEN -> loop control index
     *returned_data_len_p cannot exceed GSDI_MAX_DATA_BLOCK_LEN
     ------------------------------------------------------------------------*/
     loop_control = features->len / GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN;

     for (i = 0; i < loop_control; i++ )
     {
       if ( memcmp(&features->buffer_p[i * GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN],
                   data_p, GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN) )
       {
         /* NW2 Code is not matching...try the next one */
         mismatch = TRUE;
       }
       else
       {
         mismatch = FALSE;
         break;
       }
     }

  if (mismatch)
    gsdi_status = GSDI_PERSO_INVALID_DATA;

  return gsdi_status;
}

#endif /* FEATURE_PERSO_RUIM*/
/*===========================================================================
FUNCTION GSDI_PERSO_CHECK_SIM_CODES

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
static gsdi_returns_T gsdi_perso_sec_check_sim_codes(
  const gsdi_perso_sec_me_data_type    * encrypted_data_p,
  const gsdi_perso_sec_sim_data_type   * sim_data_p,
  uint32                                  mask
)
{
  gsdi_returns_T           gsdi_status    = GSDI_SUCCESS;
  uint32                   i              = 0;
  uint32                   loop_control   = 0;
  boolean                  mismatch       = FALSE;
  uint32                   len            = 0;
  gsdi_perso_sec_data_type features;
  uint32                   fs_offset      = 0;


  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);

  /* --------------------------------------------------------------------------
     Descrypt the NW Codes from FS
 ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_DATA,
                                                 &encrypted_data_p->sim_lock_codes,
                                                 &len,
                                                 mask);
  if ( gsdi_status != GSDI_SUCCESS)
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  if(len ==0)
  {
     return gsdi_status;
  }

  features.buffer_p = mmgsdi_malloc(len);


  gsdi_status = gsdi_perso_sec_decrypt_tlv(mask,
                                           &encrypted_data_p->sim_lock_codes,
                                           &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return gsdi_status;
  }

  switch(mask)
  {
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_SEC_MASK_FEAT_NW:
      fs_offset = 3;
      break;
    case GSDI_PERSO_SEC_MASK_FEAT_NS:
      fs_offset = 4;
      break;
    case GSDI_PERSO_SEC_MASK_FEAT_SIM:
      fs_offset = 8;
      break;
    case GSDI_PERSO_SEC_MASK_FEAT_SP:
      gsdi_status = gsdi_perso_sec_check_sim_sp_codes(sim_data_p, &features);
      MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
      return gsdi_status;

    case GSDI_PERSO_SEC_MASK_FEAT_CP:
      gsdi_status =  gsdi_perso_sec_check_cp_sim_codes(sim_data_p, &features);
      MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
      return gsdi_status;
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
  case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
      fs_offset = 3;
      break;
    case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
      gsdi_status = gsdi_perso_sec_check_ruim_nw2_codes(sim_data_p, &features);
      MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
      return gsdi_status;
    case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
      fs_offset = 8;
      break;
 #endif /*FEATURE_PERSO_RUIM*/
    default:
      MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
      return GSDI_PERSO_INVALID_DATA;
  }

  if(len % fs_offset != 0)
  {
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  loop_control = len / fs_offset;

  for (i = 0; i < loop_control; i++ )
  {
    if ( memcmp(&features.buffer_p[i * fs_offset],
                sim_data_p->imsi.buffer_p,fs_offset) )
    {
      /* Code is not matching...try the next one */
      mismatch = TRUE;
    }
    else
    {
      mismatch = FALSE;
      break;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

  if ( mismatch)
    gsdi_status = GSDI_PERSO_INVALID_DATA;

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_VALIDATE_DATA

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
static gsdi_returns_T gsdi_perso_sec_validate_data(
  const gsdi_perso_sec_client_req_type  * req_data_p,
  const gsdi_perso_sec_me_data_type     * encrypted_data_p,
  uint8                                 * ret_data_p,
  int32                                 * ret_data_len_p
)
{
  gsdi_returns_T gsdi_status   = GSDI_SUCCESS;
#ifdef FEATURE_PERSO_SIM
  boolean umts = FALSE;
#endif
  /* --------------------------------------------------------------------------
     Perform function parameter checks
     1.  filename_p or data_p can not be null;
     2.  num_bytes can not exceed GSDI_MAX_DATA_BLOCK_LEN
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  if ( req_data_p->feature_data.len >= GSDI_MAX_DATA_BLOCK_LEN )
  {
    MSG_ERROR("PERSO: Wrong data length 0x%x",req_data_p->feature_data.len,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  switch ( req_data_p->feature_mask )
  {
#ifdef FEATURE_PERSO_SIM
    case GSDI_PERSO_SEC_MASK_FEAT_NW:
      break;

    case GSDI_PERSO_SEC_MASK_FEAT_NS:
    case GSDI_PERSO_SEC_MASK_FEAT_SP:
    case GSDI_PERSO_SEC_MASK_FEAT_CP:
      gsdi_status   = gsdi_perso_sec_validate_data_against_fs
                                                    (req_data_p,
                                                     encrypted_data_p,
                                                     ret_data_len_p,
                                                     ret_data_p
                                                    );
      break;

    case GSDI_PERSO_SEC_MASK_FEAT_SIM:
      umts = TRUE;
      gsdi_status  = gsdi_perso_sec_validate_imsi(req_data_p,
                                                  encrypted_data_p,
                                                  ret_data_len_p,
                                                  ret_data_p,
                                                  umts);

    break;
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
    case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
      MSG_HIGH("PERSO: Perso RUIM NW1 validated",0,0,0);
      break;
    case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
      MSG_HIGH("PERSO: Perso RUIM NW2 validated",0,0,0);
      break;
    case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
      MSG_HIGH("PERSO: Perso RUIM RUIM validated",0,0,0);
      break;
#endif /*FEATURE_PERSO_RUIM*/
    default:
      MMGSDI_DEBUG_ERR("PERSO: Invalid Feature Received: 0x%x",
                       req_data_p->feature_mask,0,0);
      return GSDI_PERSO_INVALID_DATA;
  }

  return gsdi_status;
}

#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_SEC_GET_OTA_DEPERSO_CK

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
static gsdi_returns_T   gsdi_perso_sec_get_ota_deperso_ck(
  const gsdi_perso_sec_client_req_type * req_data_p,
  gsdi_perso_sec_data_type             * ck_p )
{
  gsdi_returns_T  gsdi_status = GSDI_ERROR;

  char no_ck[GSDI_PERSO_OTA_DEPERSO_CK_LEN];
  int32 offset = 0x0000;

  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(ck_p);

  memset(no_ck, 0xFF, GSDI_PERSO_OTA_DEPERSO_CK_LEN);

  switch (req_data_p->feature_mask)
  {
  case GSDI_PERSO_SEC_MASK_FEAT_NW:
    offset = GSDI_PERSO_SEC_OTA_DEPERSO_NW_CK_OFFSET;
    break;
  case GSDI_PERSO_SEC_MASK_FEAT_NS:
    offset = GSDI_PERSO_SEC_OTA_DEPERSO_NS_CK_OFFSET;
    break;
  case GSDI_PERSO_SEC_MASK_FEAT_SP:
    offset = GSDI_PERSO_SEC_OTA_DEPERSO_SP_CK_OFFSET;
    break;
  case GSDI_PERSO_SEC_MASK_FEAT_CP:
    offset = GSDI_PERSO_SEC_OTA_DEPERSO_CP_CK_OFFSET;
    break;
  default:
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Unknown Feature mask: 0x%x",
                           req_data_p->feature_mask,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Check to see if the Control Key is set to the Default of "FFFFFFFF"
     which indicates no Control Key was provided
     If Default, return 0 bytes for the number of OTA Deperso CK Bytes
     ------------------------------------------------------------------------*/
  if (! (memcmp(&req_data_p->feature_data.buffer_p[offset],
         (uint8 *)no_ck,
         GSDI_PERSO_OTA_DEPERSO_CK_LEN ) ) )
  {
    ck_p->len = 0x00;
  }
  else
  {
    ck_p->len     = (int32)GSDI_PERSO_OTA_DEPERSO_CK_LEN;
    ck_p->buffer_p= &req_data_p->feature_data.buffer_p[offset];
  }
  gsdi_status = GSDI_SUCCESS;
  return gsdi_status;
}
#endif /*FEATURE_PERSO_SIM*/

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_ACTIVATE

DESCRIPTION:
  Function called to perform the activation of the Feature.
  This function will write back to the NV or the Secure
  file system.  Therefore, the caller doesn't have to try
  and save any of the information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_activate(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_perso_sec_data_type       features;
  gsdi_perso_sec_inds_state_type feature_states;
  gsdi_perso_sec_data_type       control_keys;
  gsdi_returns_T                 gsdi_status     = GSDI_SUCCESS;
  gsdi_returns_T                 act_gsdi_status = GSDI_SUCCESS;
  uint32                         len             = 0;

  MSG_HIGH("PERSO: gsdi_perso_security_activate",0,0,0);
  /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);


  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  /* --------------------------------------------------------------------------
     Next resolve the data provided in the feature indicators and
     determine the state of each one.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                        &features);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Does the state of the indicators match the requested
     action.  Simply populate and indicate so.  The logic to allow
     or not allow this procedure is handled in another function.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_action_allowed(GSDI_PERSO_SEC_PROC_ACTIVATE_FEATURE,
                                              &features,
                                              &feature_states);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required, ok to
     Free Now
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
  features.len      = 0;

  /* --------------------------------------------------------------------------
    Is the state of the indicator consistent with the ind provided
    by the requesting client
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_state_consistent(req_data_p->feature_mask,
                                                &encrypted_data_p->feat_inds,
                                                &encrypted_data_p->dck_counts,
                                                &feature_states,
                                                perso_event_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: STATE CONSISTENCY CHECK ERROR 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }

  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_CKS,
                                                 &encrypted_data_p->dcks,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  control_keys.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(control_keys.buffer_p);

  /* --------------------------------------------------------------------------
     Indicator State is valid, can now proceed with the
     Depersonalization Control Key Validation Procedures
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_verify_ck(req_data_p,
                                              &encrypted_data_p->dcks,
                                              &encrypted_data_p->dck_counts,
                                              &encrypted_data_p->data_mod,
                                              &control_keys,
                                              perso_event_p,
                                              GSDI_PERSO_MAX_NUMBER_OF_RETRIES);

  /* --------------------------------------------------------------------------
     Information for the Control Keys is no longer required, ok to
      Free Now
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(control_keys.buffer_p);
  control_keys.len      = 0;

  if ( gsdi_status != GSDI_SUCCESS )
  {
    /*Invalid CK, Do not need a CK*/
    if(gsdi_status == GSDI_PERSO_INVALID_CK)
    {
      *perso_event_p = GSDI_PERSO_NO_EVENT;
    }
    MMGSDI_DEBUG_MSG_ERROR("PERSO: DCK VERIFY FAILURE: 0x%x",
                           gsdi_status,0,0);
    act_gsdi_status = gsdi_status;
  }

  /*Change Feature Indicator only if the above condition is successful*/
  if(gsdi_status == GSDI_SUCCESS)
  {
    /* --------------------------------------------------------------------------
       Update the Indicator data so that it can be written back
       to the file system by the personalization engine.
       ------------------------------------------------------------------------*/
    act_gsdi_status = gsdi_perso_security_change_feature_inds(req_data_p->feature_mask,
                                                              encrypted_data_p,
                                                              GSDI_PERSO_SEC_ACTIVATE_FEATURE);

    if ( act_gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: ACTIVATION FAILURE: 0x%x",
                             act_gsdi_status,0,0);
    }
    else
    {
      /*Feature Indicator has changed, update the File System*/
      encrypted_data_p->data_mod.update_fs = TRUE;
    }
  }

  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->dck_counts,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->dck_counts, &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Activate: Get DCK Count Failed: 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(ret_data_p,features.buffer_p,features.len);
  *ret_data_len_p = uint32toint32(features.len);

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required, ok to
     Free Now
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
  features.len      = 0;
  return act_gsdi_status;
}



/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_DEACTIVATE

DESCRIPTION:
  Function called to perform the deactivation of the Feature.
  This function will write back to the NV or the Secure
  file system.  Therefore, the caller doesn't have to try
  and save any of the information.

DEPENDENCIES:
  The feature's state must be in the proper state.

  To activate a Feature, it must be in either 1 of the 2 states:
  AUTOLOCK:  Personalize to the First SIM Inserted
  INACTIVE:  Personalize to the data provided.

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_deactivate(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_perso_sec_data_type       features;
  gsdi_perso_sec_inds_state_type feature_states;
  gsdi_perso_sec_data_type       control_keys;
  gsdi_returns_T                 gsdi_status     = GSDI_SUCCESS;
  gsdi_returns_T                 act_gsdi_status = GSDI_SUCCESS;
  uint32                         len             = 0;

  MSG_HIGH("PERSO: gsdi_perso_security_deactivate",0,0,0);

  /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);


  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  /* --------------------------------------------------------------------------
     Next resolve the data provided in the feature indicators and
     determine the state of each one.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                        &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Does the state of the indicators match the requested
     action.  Simply populate and indicate so.  The logic to allow
     or not allow this procedure is handled in another function.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_action_allowed(GSDI_PERSO_SEC_PROC_DEACTIVATE_FEATURE,
                                              &features,
                                              &feature_states);

  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
      Information for the Feature Indicators is no longer required
      ------------------------------------------------------------------------*/
  features.len      = 0;


  /* --------------------------------------------------------------------------
    Is the state of the indicator consistent with the ind provided
    by the requesting client
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_state_consistent(req_data_p->feature_mask,
                                                &encrypted_data_p->feat_inds,
                                                &encrypted_data_p->dck_counts,
                                                &feature_states,
                                                perso_event_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: STATE CONSISTENCY CHECK ERROR 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }
  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_CKS,
                                                 &encrypted_data_p->dcks,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  control_keys.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(control_keys.buffer_p);

  /* --------------------------------------------------------------------------
     Indicator State is valid, can now proceed with the
     Depersonalization Control Key Validation Procedures
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_verify_ck(req_data_p,
                                              &encrypted_data_p->dcks,
                                              &encrypted_data_p->dck_counts,
                                              &encrypted_data_p->data_mod,
                                              &control_keys,
                                              perso_event_p,
                                              GSDI_PERSO_MAX_NUMBER_OF_RETRIES);

  /* --------------------------------------------------------------------------
     Information for the Control Keys is no longer required
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(control_keys.buffer_p);
  control_keys.len      = 0;

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: DCK VERIFY FAILURE: 0x%x",
                           gsdi_status,0,0);
    control_keys.len = 0;
    act_gsdi_status  = gsdi_status;
  }

  /*Change Feature Indicators only if the DCK was verified successfully*/

  if(gsdi_status == GSDI_SUCCESS)
  {
    /* --------------------------------------------------------------------------
       Update the Indicator data so that it can be written back
       to the file system by the personalization engine.
       ------------------------------------------------------------------------*/
    act_gsdi_status = gsdi_perso_security_change_feature_inds(
                        req_data_p->feature_mask,
                        encrypted_data_p,
                        GSDI_PERSO_SEC_DEACTIVATE_FEATURE);

    if ( act_gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: FAILURE TO UPDATE ENCRYPTED INDS",0,0,0);
    }
    else
    {
      /*Feature Indicator has changedm update the File System*/
      encrypted_data_p->data_mod.update_fs = TRUE;

      switch(req_data_p->feature_mask)
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_MASK_FEAT_NW:
           *perso_event_p = GSDI_PERSO_NW_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_NS:
           *perso_event_p = GSDI_PERSO_NS_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_SP:
           *perso_event_p = GSDI_PERSO_SP_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_CP:
           *perso_event_p = GSDI_PERSO_CP_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_SIM:
           *perso_event_p = GSDI_PERSO_SIM_DEACTIVATED;
            break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
           *perso_event_p = GSDI_PERSO_RUIM_NW1_DEACTIVATED;
            break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
           *perso_event_p = GSDI_PERSO_RUIM_NW2_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
           *perso_event_p = GSDI_PERSO_RUIM_HRPD_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
           *perso_event_p = GSDI_PERSO_RUIM_SP_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
           *perso_event_p = GSDI_PERSO_RUIM_CP_DEACTIVATED;
            break;
         case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
           *perso_event_p = GSDI_PERSO_RUIM_RUIM_DEACTIVATED;
            break;
#endif /* FEATURE_PERSO_RUIM */
         default:
           break;
      }
    }
  }

  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->dck_counts,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->dck_counts,
                                        &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Deactivate Get DCK Count Failed: 0x%x",
                            gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(ret_data_p,features.buffer_p,features.len);
  *ret_data_len_p = uint32toint32(features.len);

  /* --------------------------------------------------------------------------
      Information for the Feature Indicators is no longer required, ok to
      Free Now
      ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
  features.len      = 0;

  return act_gsdi_status;
}


/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_FEATURE_KEY

DESCRIPTION:
   Get the key for the given feature indicator
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_get_feature_key(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T                 gsdi_status     = GSDI_SUCCESS;
  uint32                         len             = 0;
  gsdi_perso_sec_data_type       data_p;

  MSG_HIGH("PERSO: gsdi_perso_security_get_feature_key",0,0,0);

  /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  * perso_event_p = GSDI_PERSO_NO_EVENT;
  encrypted_data_p->data_mod.update_fs = FALSE;
/* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_CKS,
                                                 &encrypted_data_p->dcks,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  data_p.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(data_p.buffer_p);

  data_p.len = len;
  /* --------------------------------------------------------------------------
     Next resolve the data provided in the feature indicators and
     determine the state of each one.
     ------------------------------------------------------------------------*/
  if(req_data_p->get_unblock_key)
  {
    gsdi_status = gsdi_perso_sec_decrypt_tlv(req_data_p->feature_mask,
                                             &encrypted_data_p->unblock_cks,
                                             &data_p);
  }
  else
  {
   gsdi_status = gsdi_perso_sec_decrypt_tlv(req_data_p->feature_mask,
                                             &encrypted_data_p->dcks,
                                             &data_p);
  }

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(data_p.buffer_p);
    return gsdi_status;
  }

  if(encrypted_data_p->me_imei.len >0)
  {
    memcpy(ret_data_p, encrypted_data_p->me_imei.buffer_p,encrypted_data_p->me_imei.len);
  }
  *ret_data_len_p = uint32toint32(encrypted_data_p->me_imei.len);
  ret_data_p[*ret_data_len_p] = 0x3A; /* Add ':' in between */
  *ret_data_len_p += 1;
  memcpy(&ret_data_p[*ret_data_len_p],data_p.buffer_p,data_p.len);
  *ret_data_len_p += uint32toint32(data_p.len);

  MMGSDIUTIL_TMC_MEM_FREE(data_p.buffer_p);
  return GSDI_SUCCESS;
}
/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_FEATURE_INDS

DESCRIPTION:
    Get all the feature indicators and their status
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_get_feature_inds(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T                 gsdi_status     = GSDI_SUCCESS;
  uint32                         len             = 0;
  gsdi_perso_sec_data_type       data_p;

  MSG_HIGH("PERSO: gsdi_perso_security_get_feature_inds",0,0,0);

  /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  *perso_event_p = GSDI_PERSO_NO_EVENT;
  encrypted_data_p->data_mod.update_fs = FALSE;

  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  data_p.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(data_p.buffer_p);

  data_p.len = len;
  /* --------------------------------------------------------------------------
     Next resolve the data provided in the feature indicators and
     determine the state of each one.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                        &data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(data_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(ret_data_p,data_p.buffer_p,data_p.len);
  *ret_data_len_p = uint32toint32(data_p.len);
  MMGSDIUTIL_TMC_MEM_FREE(data_p.buffer_p);

  return GSDI_SUCCESS;
}

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_SET_FEATURE_DATA

DESCRIPTION:
   Set the feature data provided that the feature in the right state and is
   not already active
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_set_feature_data(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
 )
{
   gsdi_perso_sec_data_type       features;
   gsdi_perso_sec_inds_state_type feature_states;
   gsdi_returns_T                 gsdi_status     = GSDI_SUCCESS;
   uint32                         len             = 0;

   MSG_HIGH("PERSO: gsdi_perso_security_set_feature_data",0,0,0);
   /* --------------------------------------------------------------------------
      Validate all relevant and required Pointers.  The validation
      of the pointers within the structure will be based on the Feature's
      State.  Not all pointers
      ------------------------------------------------------------------------*/
   MMGSDI_RETURN_IF_NULL(req_data_p);
   MMGSDI_RETURN_IF_NULL(encrypted_data_p);
   MMGSDI_RETURN_IF_NULL(sim_data_p);
   MMGSDI_RETURN_IF_NULL(perso_event_p);

   /* -------------------------------------------------------------------------
      Determine how much memory is required to store the
      decrypted Featuer Indications.
      -----------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                  &encrypted_data_p->feat_inds,
                                                  &len,
                                                  req_data_p->feature_mask);
   if ( gsdi_status != GSDI_SUCCESS )
   {
     return GSDI_PERSO_INVALID_DATA;
   }

   features.buffer_p = mmgsdi_malloc(len);
   MMGSDI_RETURN_IF_NULL(features.buffer_p);

   /* --------------------------------------------------------------------------
      Next resolve the data provided in the feature indicators and
      determine the state of each one.
      ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                         &features);
   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                            gsdi_status,0,0);
     MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
     return GSDI_PERSO_INVALID_DATA;
   }

   /* --------------------------------------------------------------------------
      Does the state of the indicators match the requested
      action.  Simply populate and indicate so.  The logic to allow
      or not allow this procedure is handled in another function.
      ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_action_allowed(GSDI_PERSO_SEC_PROC_ACTIVATE_FEATURE,
                                               &features,
                                               &feature_states);

   MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
     return gsdi_status;
   }

   /* --------------------------------------------------------------------------
      Information for the Feature Indicators is no longer required
      ------------------------------------------------------------------------*/
   features.len      = 0;


   /* --------------------------------------------------------------------------
     Is the state of the indicator consistent with the ind provided
     by the requesting client
     -------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_state_consistent(req_data_p->feature_mask,
                                                 &encrypted_data_p->feat_inds,
                                                 &encrypted_data_p->dck_counts,
                                                 &feature_states,
                                                 perso_event_p);
   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: STATE CONSISTENCY CHECK ERROR 0x%x",
                            gsdi_status,0,0);
     return gsdi_status;
   }

   gsdi_status = gsdi_perso_sec_validate_data(req_data_p,
                                              encrypted_data_p,
                                              ret_data_p,
                                              ret_data_len_p);

   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: VALIDATE Data FAILURE: 0x%x",gsdi_status,0,0);
     return gsdi_status;
   }

   /* --------------------------------------------------------------------------
      Update the Indicator data so that it can be written back
      to the file system by the personalization engine.
      ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_change_sim_lock_code((void *)req_data_p,
                                                     encrypted_data_p);

   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: CHANGE Sim Lock Code FAILURE: 0x%x",gsdi_status,0,0);
     return gsdi_status;
   }

   /*data has been changed, update the file system as well.*/
   encrypted_data_p->data_mod.update_fs = TRUE;

   return GSDI_SUCCESS;

}


/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_GET_FEATURE_DATA

DESCRIPTION:
  Get SIM Lock code for the feature indicated in the request
DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_get_feature_data(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8                                * ret_data_p,
 int32                                * ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T                 gsdi_status     = GSDI_SUCCESS;
  uint32                         len             = 0;
  gsdi_perso_sec_data_type       data_p          = {0};

  MSG_HIGH("PERSO: gsdi_perso_security_get_feature_data",0,0,0);

  /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  *perso_event_p = GSDI_PERSO_NO_EVENT;
  encrypted_data_p->data_mod.update_fs = FALSE;

  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications data.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_FOR_GET_DATA,
                                                 &encrypted_data_p->sim_lock_codes,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  data_p.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(data_p.buffer_p);

  data_p.len = len;
  /* --------------------------------------------------------------------------
     Extract the data from the excrypted buffer.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_decrypt_tlv(req_data_p->feature_mask,
                                           &encrypted_data_p->sim_lock_codes,
                                           &data_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(ret_data_p,data_p.buffer_p,data_p.len);
  *ret_data_len_p = uint32toint32(data_p.len);
  MMGSDIUTIL_TMC_MEM_FREE(data_p.buffer_p);

  return GSDI_SUCCESS;
}/* gsdi_perso_security_get_feature_data */

/* ============================================================================
FUNCTION:  GSDI_PERSO_SECURITY_DISABLE_FEATURE_DATA

DESCRIPTION:
   Disable the feature indicator
DEPENDENCIES:


LIMITATIONS:
  None

RETURN_TYPE:
  gsdi_returns_T

=============================================================================*/
static gsdi_returns_T gsdi_perso_security_disable_feature_data(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
 )
{
  gsdi_perso_sec_data_type       features;
  gsdi_perso_sec_inds_state_type feature_states;
  gsdi_perso_sec_data_type       control_keys;
  gsdi_returns_T                 gsdi_status          = GSDI_SUCCESS;
  gsdi_returns_T                 disable_gsdi_status  = GSDI_SUCCESS;
  uint32                         len                  = 0;

  MSG_HIGH("PERSO: gsdi_perso_security_disable_feature_data",0,0,0);
      /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  /* --------------------------------------------------------------------------
     Next resolve the data provided in the feature indicators and
     determine the state of each one.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                        &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Does the state of the indicators match the requested
     action.  Simply populate and indicate so.  The logic to allow
     or not allow this procedure is handled in another function.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_action_allowed(GSDI_PERSO_SEC_PROC_DISABLE_FEATURE,
                                              &features,
                                              &feature_states);

  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required
     ------------------------------------------------------------------------*/
  features.len      = 0;

  /* --------------------------------------------------------------------------
    Is the state of the indicator consistent with the ind provided
    by the requesting client
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_state_consistent(req_data_p->feature_mask,
                                                &encrypted_data_p->feat_inds,
                                                &encrypted_data_p->dck_counts,
                                                &feature_states,
                                                perso_event_p);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: STATE CONSISTENCY CHECK ERROR 0x%x",
                           gsdi_status,0,0);
    return gsdi_status;
  }

  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
     -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_CKS,
                                                 &encrypted_data_p->dcks,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  control_keys.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(control_keys.buffer_p);

  /* --------------------------------------------------------------------------
     Indicator State is valid, can now proceed with the
     Depersonalization Control Key Validation Procedures
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_verify_ck(req_data_p,
                                              &encrypted_data_p->dcks,
                                              &encrypted_data_p->dck_counts,
                                              &encrypted_data_p->data_mod,
                                              &control_keys,
                                              perso_event_p,
                                              GSDI_PERSO_MAX_NUMBER_OF_RETRIES);

  /* -------------------------------------------------------------------------
     Information for the Control Keys is no longer required
   ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(control_keys.buffer_p);
  control_keys.len      = 0;

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: DCK VERIFY FAILURE: 0x%x",gsdi_status,0,0);
    disable_gsdi_status = gsdi_status;
  }

  if(gsdi_status == GSDI_SUCCESS)
  {
    /* --------------------------------------------------------------------------
       Update the Indicator data so that it can be written back
       to the file system by the personalization engine.
       ------------------------------------------------------------------------*/
    disable_gsdi_status = gsdi_perso_security_change_feature_inds(
                            req_data_p->feature_mask,
                            encrypted_data_p,
                            GSDI_PERSO_SEC_DISABLE_FEATURE);

    if ( disable_gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: DISABLE FEATURE FAILURE: 0x%x",
                             disable_gsdi_status,0,0);
    }
    else
    {
      /*Feature Indicator has changed, update the File System*/
      encrypted_data_p->data_mod.update_fs = TRUE;
    }
  }

  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                &encrypted_data_p->feat_inds,
                                                &len,
                                                req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                        &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Disable: Get Data Failed: 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(ret_data_p,features.buffer_p,features.len);
  *ret_data_len_p = uint32toint32(features.len);

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required, ok to
     Free Now
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
  features.len      = 0;

  return disable_gsdi_status;
} /* gsdi_perso_security_disable_feature_data */


/* ============================================================================
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
=============================================================================*/
static gsdi_returns_T gsdi_perso_security_ota_deperso(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T                     gsdi_status       = GSDI_ERROR;

#ifdef FEATURE_PERSO_SIM
  boolean                            event_set         = FALSE;
  gsdi_perso_event_enum_type         local_perso_event = GSDI_PERSO_NO_EVENT;
  int32                              feature           = 0;
  uint8                              sms_status[GSDI_PERSO_MAX_SMS_STATUS_LEN];
  uint8                              offset            = 0x00;
  gsdi_perso_sec_client_req_type     deperso_req;
  gsdi_perso_sec_client_req_type     req_copy;
#endif

  MSG_HIGH("PERSO: gsdi_perso_security_ota_deperso",0,0,0);
 /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

   encrypted_data_p->data_mod.update_fs = FALSE;
  *perso_event_p = GSDI_PERSO_NO_EVENT;
  *ret_data_len_p = 0;
  *ret_data_p     = 0;

#ifdef FEATURE_PERSO_SIM
   memcpy(&req_copy, req_data_p, sizeof(gsdi_perso_sec_client_req_type));

  if(encrypted_data_p->me_imei.len == 0)
  {
    MSG_ERROR("PERSO: me_imei length is zero",0,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Compare the IMEIs to see if they match...otherwise indicate an IMEI
     mismatch to the caller
     ------------------------------------------------------------------------*/
  if (memcmp(encrypted_data_p->me_imei.buffer_p,
             &req_data_p->feature_data.buffer_p[GSDI_PERSO_OTA_DEPERSO_IMEI_OFFSET],
             GSDI_PERSO_MAX_IMEI_LEN))
  {
    /* IMEI Mismatch
       data_p buffer information:
       data_p[0:15] IMEI
       data_p[16]   NW Status
       data_p[17]   NS Status
       data_p[18]   SP Status
       data_p[19]   CP Status
    */
    MMGSDI_DEBUG_MSG_ERROR("PERSO: IMEI Does not match ",0,0,0);
    memcpy(ret_data_p,
           encrypted_data_p->me_imei.buffer_p,
           GSDI_PERSO_MAX_IMEI_LEN);

    ret_data_p[GSDI_PERSO_OTA_DEPERSO_RSP_NW_OFFSET] = (int32)GSDI_PERSO_IMEI_MISMATCH;
    ret_data_p[GSDI_PERSO_OTA_DEPERSO_RSP_NS_OFFSET] = (int32)GSDI_PERSO_IMEI_MISMATCH;
    ret_data_p[GSDI_PERSO_OTA_DEPERSO_RSP_SP_OFFSET] = (int32)GSDI_PERSO_IMEI_MISMATCH;
    ret_data_p[GSDI_PERSO_OTA_DEPERSO_RSP_CP_OFFSET] = (int32)GSDI_PERSO_IMEI_MISMATCH;

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
         encrypted_data_p->me_imei.buffer_p,
         GSDI_PERSO_MAX_IMEI_LEN);


  /* --------------------------------------------------------------------------
     Per the 22.022 specifications and 2.22 specifications, it is required that
     the ME Software set the 16th Byte (Index) 15 to 0 since it is ignored
     due to the IMEI consisting of 15 bytes (Indexes 0 - 14).
     ------------------------------------------------------------------------*/
  sms_status[15] = 0x30;

  while ( feature < 0x04 )
  {
    switch (feature )
    {
    case GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET:
      offset = GSDI_PERSO_SEC_OTA_DEPERSO_RSP_NW_OFFSET;
      req_copy.feature_mask = GSDI_PERSO_SEC_MASK_FEAT_NW;
      break;
    case GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET:
      offset = GSDI_PERSO_SEC_OTA_DEPERSO_RSP_NS_OFFSET;
      req_copy.feature_mask = GSDI_PERSO_SEC_MASK_FEAT_NS;
      break;
    case GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET:
      offset = GSDI_PERSO_SEC_OTA_DEPERSO_RSP_SP_OFFSET;
      req_copy.feature_mask = GSDI_PERSO_SEC_MASK_FEAT_SP;
      break;
    case GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET:
      offset = GSDI_PERSO_SEC_OTA_DEPERSO_RSP_CP_OFFSET;
      req_copy.feature_mask = GSDI_PERSO_SEC_MASK_FEAT_CP;
      break;
    default:
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Bad Perso ID: 0x%x",
                             feature,0,0);
      break;
    }
    gsdi_status = gsdi_perso_sec_get_ota_deperso_ck(&req_copy,
                                                    &deperso_req.dck);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_ERR("PERSO: Error getting Control Keys for SMS OTA MSG",0,0,0);
      MSG_HIGH("PERSO: Perso Feature: 0x%x Is Not Active, can't deperso",
               feature,0,0);
      sms_status[offset] = (int32)GSDI_PERSO_CURRENTLY_NOT_PERSO;
      feature++;
      continue;
    }

    deperso_req.feature_mask = req_copy.feature_mask;

    if ( deperso_req.dck.len != 0 )
    {

      gsdi_status = gsdi_perso_security_deactivate(&deperso_req,
                                                    encrypted_data_p,
                                                    sim_data_p,
                                                    ret_data_p,
                                                    ret_data_len_p,
                                                    &local_perso_event
                                                   );
      if ( gsdi_status == GSDI_SUCCESS )
      {
         MSG_HIGH("PERSO: Depersonalized Feature: 0x%x",feature,0,0);
         sms_status[offset] = (int32)GSDI_PERSO_CURRENTLY_NOT_PERSO;
      }
      else
      {
        if(event_set == FALSE)
        {
          *perso_event_p = local_perso_event;
          event_set = TRUE;
        }

        if(gsdi_status == GSDI_PERSO_PERM_DISABLED)
        {
         /* This feature was perm disabled...report back as is */
         sms_status[offset] = (int32)GSDI_PERSO_PERMANENTLY_NOT_PERS;
        }
        else if (gsdi_status == GSDI_PERSO_INCONSISTENT_W_IND)
        {
          MSG_HIGH("PERSO: CK Verification Failed, not active Feature: 0x%x",
                   feature,0,0);
          sms_status[offset] = (int32)GSDI_PERSO_CURRENTLY_NOT_PERSO;
        }
        else
        {
          MSG_HIGH("PERSO: CK Verification Failed for Feature: 0x%x",
                   feature,0,0);
          sms_status[offset] = (int32)GSDI_PERSO_PERSONALIZED;
        }
      }

    }
    else
    {
      MSG_HIGH("PERSO: NO CK Present for Feature: 0x%x",feature,0,0);
      sms_status[offset] = (int32)GSDI_PERSO_PERSONALIZED;
    }
    /* Increment the Loop Control */
    feature++;
  } /* While Loop */

  if(event_set == FALSE)
  {
    *perso_event_p =local_perso_event;
  }


  /* --------------------------------------------------------------------------
     We now have an SMS Status, and SMS Status Length that needs to be
     returned to SMS for an ACK back to the network.

     Overwrite data_p and set the returned_data_len_p accordingly
     ------------------------------------------------------------------------*/
  memcpy(ret_data_p,
         sms_status,
         GSDI_PERSO_OTA_DEPERSO_RSP_LEN);

  *ret_data_len_p = GSDI_PERSO_OTA_DEPERSO_RSP_LEN;
   gsdi_status = GSDI_SUCCESS;
#endif /*FEATURE_PERSO_SIM*/

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_CHECK_ALL_LOCKS

DESCRIPTION
  This function will be called for each personalization procedure that occurs.
  It will be the main personalization handling function used durint the init
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_check_all_locks(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{

  gsdi_returns_T    gsdi_status     = GSDI_SUCCESS;
#ifdef FEATURE_PERSO_SIM
  uint32            mask            = GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT + 1;
#elif defined(FEATURE_PERSO_RUIM)
  uint32            mask            = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
#endif
  gsdi_perso_sec_data_type            features;
  gsdi_perso_sec_inds_state_type      feature_states;
  uint32                              len =0 ,i =0;
  uint32                              feature;

  MSG_HIGH("PERSO: gsdi_perso_security_check_all_locks",0,0,0);

 /* --------------------------------------------------------------------------
     Validate all relevant and required Pointers.  The validation
     of the pointers within the structure will be based on the Feature's
     State.  Not all pointers
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  encrypted_data_p->data_mod.update_fs = FALSE;

  if ( gsdi_perso_sec_is_test_imsi_detected(sim_data_p->imsi.buffer_p) )
  {
    return GSDI_SUCCESS;
  }

/* -------------------------------------------------------------------------
   Determine how much memory is required to store the
   decrypted Featuer Indications.
  -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 0);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }
  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  /* --------------------------------------------------------------------------
    Next resolve the data provided in the feature indicators and
    determine the state of each one.
   ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                         &features);
   if ( gsdi_status != GSDI_SUCCESS )
   {
     MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                            gsdi_status,0,0);
     MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
     return GSDI_PERSO_INVALID_DATA;
   }
   /* --------------------------------------------------------------------------
     Does the state of the indicators match the requested
     action.  Simply populate and indicate so.  The logic to allow
     or not allow this procedure is handled in another function.
    ------------------------------------------------------------------------*/
   gsdi_status = gsdi_perso_sec_action_allowed(GSDI_PERSO_SEC_PROC_ACTIVATE_FEATURE,
                                                &features,
                                                &feature_states);

   MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

   if(gsdi_status != GSDI_SUCCESS )
   {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
      return gsdi_status;
   }

   /* --------------------------------------------------------------------------
      Information for the Feature Indicators is no longer required
        ------------------------------------------------------------------------*/
   features.len = 0;

  /* --------------------------------------------------------------------------
     Start the GSDI Personalization Checks to see if the SIM/USIM is the valid
     SIM/USIM the UE was previously personalized with.
     --------------------------------------------------------------------------
     check to see if the Feature is set to none...indication that if active
     MMGSDI needs to self personalize
     ------------------------------------------------------------------------*/
   for(i=0;i<GSDI_PERSO_SEC_MAX_NUM_INDS;i++)
   {
     feature = req_data_p->feature_mask & mask;
     switch (feature)
     {
#ifdef FEATURE_PERSO_SIM
       case GSDI_PERSO_SEC_MASK_FEAT_NW:
       case GSDI_PERSO_SEC_MASK_FEAT_NS:
       case GSDI_PERSO_SEC_MASK_FEAT_SP:
       case GSDI_PERSO_SEC_MASK_FEAT_CP:
       case GSDI_PERSO_SEC_MASK_FEAT_SIM:
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
         continue;

       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
       case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
#endif /* FEATURE_PERSO_RUIM */
         ret_data_p[0] = (uint8)GSDI_PERSO_SELF_INIT_STATE;
         ret_data_p[1] = uint32touint8(feature);

         /* --------------------------------------------------------------------------
         Is the state of the indicator Perm Blocked?
         -------------------------------------------------------------------------*/
         gsdi_status = gsdi_perso_sec_state_consistent(feature,
                                                      &encrypted_data_p->feat_inds,
                                                      &encrypted_data_p->unblock_ck_counts,
                                                      &feature_states,
                                                      perso_event_p);
         if (gsdi_status == GSDI_PERSO_CK_BLOCKED)
         {
           MSG_ERROR("PERSO: Feature %d is Perm Blocked, cannot proceed with checking locks",
                      feature,0,0);
          *perso_event_p = GSDI_PERSO_SANITY_ERROR;
           return GSDI_ERROR;
         }

         /* --------------------------------------------------------------------------
         Is the state of the indicator consistent with the ind provided
         by the requesting client
         -------------------------------------------------------------------------*/
         gsdi_status = gsdi_perso_sec_state_consistent(feature,
                                                      &encrypted_data_p->feat_inds,
                                                      &encrypted_data_p->dck_counts,
                                                      &feature_states,
                                                      perso_event_p);
         if (gsdi_status == GSDI_PERSO_CK_BLOCKED)
         {
           MSG_ERROR("PERSO: Feature %d is Blocked, cannot proceed with checking locks",
                      feature,0,0);
           return gsdi_status;
         }

        /* Is the Network Indicator Perm Disabled */
         if ( gsdi_status == GSDI_PERSO_PERM_DISABLED )
           continue;

         /* Is it activated */
         if ( gsdi_status == GSDI_PERSO_INCONSISTENT_W_IND )
         {
         /* Verify the FS Contents of the Network Codes Stored
         ** against the contents in the SIM/USIM.  In this case,
         ** from the IMSI
          */

           gsdi_status = gsdi_perso_sec_check_sim_codes(encrypted_data_p,
                                                        sim_data_p,
                                                        feature);
           if(gsdi_status != GSDI_SUCCESS)
           {
             MSG_ERROR("PERSO: Feature %d does not match card, cannot proceed with checking locks",
                        feature,0,0);
             (void)gsdi_perso_sec_set_perso_event(feature, GSDI_ERROR, perso_event_p);
             return gsdi_status;
           }
         }
         break;
       default:
         break;
     }
     mask = mask << 1;
   }
   *ret_data_len_p = 0;
   return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SEC_BUILD_SIM_CODES

DESCRIPTION
  This function is a function used to build the SIM Contents.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_build_sim_codes(
  const gsdi_perso_sec_sim_data_type    * sim_data_p,
  gsdi_perso_sec_data_type              * new_sim_code_p,
  boolean                               * sp_locked,
  boolean                               * cp_locked,
  uint32                                  feature_mask
)
{
  gsdi_returns_T           gsdi_status    = GSDI_SUCCESS;
  uint32                   i              = 0;

#ifdef FEATURE_PERSO_SIM
  uint8                   *buffer;
  uint32                   mask          = GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT + 1;
#elif defined(FEATURE_PERSO_RUIM)
   uint32                   mask          = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
#endif
  uint32                    fs_offset      = 0;
  uint32                    feature;

  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(new_sim_code_p);
  MMGSDI_RETURN_IF_NULL(sp_locked);
  MMGSDI_RETURN_IF_NULL(cp_locked);

  new_sim_code_p->len = 0;

  MSG_HIGH("PERSO: Building codes from IMSI , features - 0x%x",
           feature_mask,0,0);

  for(i=0;i<GSDI_PERSO_SEC_MAX_NUM_INDS;i++)
  {
    feature = feature_mask & mask;
    new_sim_code_p->buffer_p[new_sim_code_p->len]  = (uint8)mask;

    switch(feature)
    {
#ifdef FEATURE_PERSO_SIM
      case GSDI_PERSO_SEC_MASK_FEAT_NW:
        fs_offset = 3;
        break;

      case GSDI_PERSO_SEC_MASK_FEAT_NS:
        fs_offset = 4;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_SIM:
        fs_offset = 8;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_SP:
        if ( sim_data_p->gid1.len == 0          ||
             sim_data_p->gid1.buffer_p[0] == 0xFF)
        {
          fs_offset = 0;
          *sp_locked = FALSE;
          break;
        }

        buffer = new_sim_code_p->buffer_p + new_sim_code_p->len + GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE;

        if ( sim_data_p->gid1.len  > 0 &&
             sim_data_p->gid1.buffer_p[0] != 0xFF )
        {
          memcpy(buffer,
                  sim_data_p->imsi.buffer_p,
                  GSDI_PERSO_SEC_SP_CODE_LEN - 1);
          buffer[GSDI_PERSO_SEC_SP_CODE_LEN -1]
              =( ( ( sim_data_p->gid1.buffer_p[0] << 4 ) & 0xF0 ) |
                 ( ( sim_data_p->gid1.buffer_p[0] >> 4 ) & 0x0F ) );
        }
        new_sim_code_p->buffer_p[new_sim_code_p->len+1]  = 0;
        new_sim_code_p->buffer_p[new_sim_code_p->len +2] = GSDI_PERSO_SEC_SP_CODE_LEN;
        new_sim_code_p->len += GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE +
                               GSDI_PERSO_SEC_SP_CODE_LEN;
        *sp_locked =TRUE;

        mask = mask << 1;

        continue;

      case GSDI_PERSO_SEC_MASK_FEAT_CP:
        if ( sim_data_p->gid1.len == 0          ||
             sim_data_p->gid1.buffer_p[0] == 0xFF)
        {
          fs_offset = 0;
          *cp_locked = FALSE;
          break;
        }
        if ( sim_data_p->gid2.len == 0          ||
             sim_data_p->gid2.buffer_p[0] == 0xFF)
        {
          fs_offset = 0;
          *cp_locked = FALSE;
          break;
        }

        buffer = new_sim_code_p->buffer_p + new_sim_code_p->len + GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE;

        if ( sim_data_p->gid2.len  > 0 &&
             sim_data_p->gid2.buffer_p[0] != 0xFF )
        {
          if ( sim_data_p->gid1.len  > 0 &&
               sim_data_p->gid1.buffer_p[0] != 0xFF )
          {
            memcpy(buffer,
                  sim_data_p->imsi.buffer_p,
                  0x03);

            buffer[GSDI_PERSO_SEC_CP_CODE_LEN -2] =
              ( ( ( sim_data_p->gid1.buffer_p[0] << 4 ) & 0xF0 ) |
                ( ( sim_data_p->gid1.buffer_p[0] >> 4 ) & 0x0F ) );

            buffer[GSDI_PERSO_SEC_CP_CODE_LEN -1] =
              ( ( ( sim_data_p->gid2.buffer_p[0] << 4 ) & 0xF0 ) |
                ( ( sim_data_p->gid2.buffer_p[0] >> 4 ) & 0x0F ) );

          }
        }
        new_sim_code_p->buffer_p[new_sim_code_p->len+1]  = 0;
        new_sim_code_p->buffer_p[new_sim_code_p->len +2] = GSDI_PERSO_SEC_CP_CODE_LEN;
        new_sim_code_p->len += GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE +
                               GSDI_PERSO_SEC_CP_CODE_LEN;
        *cp_locked = TRUE;

        mask = mask << 1;

        continue;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
      case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
        fs_offset = GSDI_PERSO_SEC_RUIM_NW1_CODE_LEN;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
        fs_offset = GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
        fs_offset = GSDI_PERSO_SEC_RUIM_RUIM_CODE_LEN;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
        fs_offset = 0;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
        *sp_locked = FALSE;
        fs_offset = 0;
        break;
      case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
        *cp_locked = FALSE;
        fs_offset = 0;
        break;

#endif /*FEATURE_PERSO_RUIM*/
      default:
        fs_offset = 0;
        break;
    }

    new_sim_code_p->buffer_p[new_sim_code_p->len+1] = (uint8)(fs_offset/255); /*LEN*/
    new_sim_code_p->buffer_p[new_sim_code_p->len+2] = (uint8)(fs_offset%255); /*LEN*/
    new_sim_code_p->len += GSDI_PERSO_ENGINE_LEN_SIZE + GSDI_PERSO_ENGINE_TAG_SIZE;

    if(new_sim_code_p->len + fs_offset > GSDI_PERSO_BUFFER_LEN)
    {
      MSG_ERROR("PERSO: Exceeding the amount of memory allocated, Len= 0x%x",
                new_sim_code_p->len,0,0 );
      return GSDI_PERSO_INVALID_DATA;
    }

    memcpy(&new_sim_code_p->buffer_p[new_sim_code_p->len],
          sim_data_p->imsi.buffer_p,fs_offset);

    new_sim_code_p->len += fs_offset;
    mask = mask << 1;
  }
  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SEC_BUILD_NW_CODE

DESCRIPTION
  This function will build the NW codes from CNL

DEPENDENCIES
  None

RETURN VALUE
  boolean
===========================================================================*/
static boolean gsdi_perso_sec_build_nw_code(
  gsdi_perso_sec_data_type             * new_sim_lock_code_p,
  const gsdi_perso_sec_sim_data_type   * sim_data_p
)
{
  uint32    index              = 0;
  uint32    len_index          = 0;

  if (sim_data_p          == NULL ||
      new_sim_lock_code_p == NULL ||
      new_sim_lock_code_p->buffer_p == NULL)
  {
    MSG_ERROR("PERSO: Null Data While building NW Codes",0,0,0);
    return FALSE;
  }

  if(sim_data_p->cnl.buffer_p[index] == 0xFF)
  {
    MSG_HIGH("PERSO: CNL data invalid while building NW Codes",0,0,0);
    return FALSE;
  }

  MSG_HIGH("PERSO: Building NW Codes from CNL",0,0,0);
  new_sim_lock_code_p->len ++;

  len_index               = new_sim_lock_code_p->len;
  new_sim_lock_code_p->len += GSDI_PERSO_ENGINE_LEN_SIZE;

  while ( index < (uint32)sim_data_p->cnl.len )
  {
    if(new_sim_lock_code_p->len + GSDI_PERSO_SEC_CNL_NW_BYTES > GSDI_PERSO_BUFFER_LEN)
    {
      MSG_ERROR("PERSO: Exceeding the amount of memory allocated, Len= 0x%x",
                new_sim_lock_code_p->len,0,0 );
      return FALSE;
    }

    /*Copy the NW bytes over and increment the length*/
    new_sim_lock_code_p->buffer_p[len_index+1] += GSDI_PERSO_SEC_CNL_NW_BYTES;
    memcpy((new_sim_lock_code_p->buffer_p + new_sim_lock_code_p->len),
           (sim_data_p->cnl.buffer_p + index),
           GSDI_PERSO_SEC_CNL_NW_BYTES);

    new_sim_lock_code_p->len += GSDI_PERSO_SEC_CNL_NW_BYTES;

    /* Increment the index by 6 bytes every time to move the pointer
     ** down each Set of CNL Data Blocks
     */
    index += GSDI_PERSO_SEC_CNL_BLOCK_SIZE;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SEC_BUILD_NS_CODE

DESCRIPTION
  This function will build the NS/NW2 codes from CNL

DEPENDENCIES
  None

RETURN VALUE
  boolean
===========================================================================*/
static boolean gsdi_perso_sec_build_ns_code(
  gsdi_perso_sec_data_type             * new_sim_lock_code_p,
  const gsdi_perso_sec_sim_data_type   * sim_data_p
)
{
  uint32    index              = 0;
  uint32    len_index          = 0;


  if (sim_data_p          == NULL ||
      new_sim_lock_code_p == NULL ||
      new_sim_lock_code_p->buffer_p == NULL)
  {
    MSG_ERROR("PERSO: Null Data While building NS Codes",0,0,0);
    return FALSE;
  }

  if(sim_data_p->cnl.buffer_p[index] == 0xFF)
  {
    MSG_HIGH("PERSO: CNL data invalid while building NS Codes",0,0,0);
    return FALSE;
  }

  MSG_HIGH("PERSO: Building the NS Codes from CNL",0,0,0);
  new_sim_lock_code_p->len ++;

  len_index                 = new_sim_lock_code_p->len;
  new_sim_lock_code_p->len += GSDI_PERSO_ENGINE_LEN_SIZE;

  while ( index < (uint32)sim_data_p->cnl.len )
  {
    if(new_sim_lock_code_p->len + GSDI_PERSO_SEC_CNL_NS_BYTES > GSDI_PERSO_BUFFER_LEN)
    {
      MSG_ERROR("PERSO: Exceeding the amount of memory allocated, Len= 0x%x",
                new_sim_lock_code_p->len,0,0 );
      return FALSE;
    }
    /* Increment the index by 6 bytes every time to move the pointer
     ** down each Set of CNL Data Blocks
     */
    new_sim_lock_code_p->buffer_p[len_index+1] += GSDI_PERSO_SEC_CNL_NS_BYTES;

    memcpy((new_sim_lock_code_p->buffer_p + new_sim_lock_code_p->len),
           (sim_data_p->cnl.buffer_p + index),
           GSDI_PERSO_SEC_CNL_NS_BYTES);

    new_sim_lock_code_p->len += GSDI_PERSO_SEC_CNL_NS_BYTES;
    index += GSDI_PERSO_SEC_CNL_BLOCK_SIZE;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SEC_BUILD_CARD_CODE

DESCRIPTION
  This function will build the Card codes from IMSI

DEPENDENCIES
  None

RETURN VALUE
  boolean
===========================================================================*/
static boolean gsdi_perso_sec_build_card_code(
  gsdi_perso_sec_data_type             *new_sim_lock_code_p,
  const gsdi_perso_sec_sim_data_type   * sim_data_p
)
{
  if (sim_data_p          == NULL ||
      new_sim_lock_code_p == NULL ||
      new_sim_lock_code_p->buffer_p == NULL)
  {
    MSG_ERROR("PERSO: Null Data While building SIM Codes",0,0,0);
    return FALSE;
  }

  MSG_HIGH("PERSO: Building the SIM Codes from IMSI",0,0,0);

  if(new_sim_lock_code_p->len + sim_data_p->imsi.len > GSDI_PERSO_BUFFER_LEN)
  {
    MSG_ERROR("PERSO: Exceeding the amount of memory allocated, Len= 0x%x",
              new_sim_lock_code_p->len,0,0 );
    return FALSE;
  }

  new_sim_lock_code_p->len ++;

  new_sim_lock_code_p->buffer_p[new_sim_lock_code_p->len +1] = (uint8)sim_data_p->imsi.len;
  new_sim_lock_code_p->len += GSDI_PERSO_ENGINE_LEN_SIZE;

  memcpy(new_sim_lock_code_p->buffer_p + new_sim_lock_code_p->len,
         sim_data_p->imsi.buffer_p,sim_data_p->imsi.len);
  new_sim_lock_code_p->len += sim_data_p->imsi.len;
  return TRUE;

}


#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_SEC_SET_DEP_MASK

DESCRIPTION
  This function will set the dependency masks for a feature i.e
  NS  -> Depends on NW ( if NS is set, NW will also get set)
  SP  -> Depends on NW ( If SP is set, NW will also get set)
  CP  -> Depends on NW,SP ( if CP is set, NW, SP will also get set)
  SIM -> Depends on NW,NS ( If SIM is set, NW,NS will also get set)

DEPENDENCIES
  None

RETURN VALUE
  boolean
===========================================================================*/
static void gsdi_perso_sec_set_dep_mask
(
  gsdi_perso_sec_client_req_type * client_data_p
)
{
  uint32  mask     = GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT + 1;
  uint32  feature;
  uint32  i        = 0;

  if (client_data_p == NULL)
  {
    MSG_ERROR("PERSO: Client Data NULL while Setting Mask",0,0,0);
    return;
  }

  for(i=0;i<GSDI_PERSO_SEC_MAX_NUM_INDS;i++)
  {
    feature = client_data_p->feature_mask & mask;

    switch (feature)
    {
      case GSDI_PERSO_SEC_MASK_FEAT_NW:
        break;

      case GSDI_PERSO_SEC_MASK_FEAT_NS:
        client_data_p->feature_mask |= GSDI_PERSO_SEC_MASK_FEAT_NW;
        break;

      case GSDI_PERSO_SEC_MASK_FEAT_SP:
        client_data_p->feature_mask |= GSDI_PERSO_SEC_MASK_FEAT_NW;
        break;

      case GSDI_PERSO_SEC_MASK_FEAT_CP:
        client_data_p->feature_mask |= GSDI_PERSO_SEC_MASK_FEAT_NW;
        client_data_p->feature_mask |= GSDI_PERSO_SEC_MASK_FEAT_SP;
        break;

      case GSDI_PERSO_SEC_MASK_FEAT_SIM:
        client_data_p->feature_mask |= GSDI_PERSO_SEC_MASK_FEAT_NW;
        client_data_p->feature_mask |= GSDI_PERSO_SEC_MASK_FEAT_NS;
        break;
      default:
        break;
    }
    mask = mask << 1;
  }
}
#endif /* FEATURE_PERSO_SIM */


/*===========================================================================
FUNCTION GSDI_PERSO_SEC_UPDATE_LOCK_CODES

DESCRIPTION
  This function takes the new lock codes and overwrites the original
  encrypted data with the lock codes of the given feature.

DEPENDENCIES
  None

RETURN VALUE
  boolean
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_update_lock_codes(
  const gsdi_perso_sec_data_type *sim_lock_code_p,
  uint32                          feature,
  gsdi_perso_sec_me_data_type    * encrypted_data_p)
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    uint32 pos = 0;
    uint32 len = 0;
    uint32 i   = 0;
    int32 offset = 0;
    gsdi_perso_sec_client_req_type   new_req_data;

    MMGSDI_RETURN_IF_NULL(sim_lock_code_p);
    MMGSDI_RETURN_IF_NULL(encrypted_data_p);

    offset = gsdi_perso_sec_get_offset(feature);

    if(offset == 0xFFFF)
    {
      return GSDI_PERSO_INVALID_DATA;
    }

    /*Find the Right location for the Feature in the new lock codes*/
    do
    {
      pos += GSDI_PERSO_ENGINE_TAG_SIZE +GSDI_PERSO_ENGINE_LEN_SIZE;
      len =  sim_lock_code_p->buffer_p[pos-GSDI_PERSO_ENGINE_LO_LEN];   /*lo_len*/
      len += sim_lock_code_p->buffer_p[pos-GSDI_PERSO_ENGINE_LEN_SIZE] *255; /*hi_len*/
      pos += len;
      i++;
    }
    while(i<=(uint32)offset);

    if(len == 0)
     return GSDI_SUCCESS; /* Nothing to Update*/

    new_req_data.feature_mask = feature;
    new_req_data.feature_data.buffer_p = mmgsdi_malloc(len);
    MMGSDI_RETURN_IF_NULL(new_req_data.feature_data.buffer_p);

    memcpy(new_req_data.feature_data.buffer_p, &sim_lock_code_p->buffer_p[pos-len],len);
    new_req_data.feature_data.len = len;

    /*Updated the Original Encrypted Data with the new lock codes*/
    gsdi_status = gsdi_perso_sec_change_sim_lock_code(&new_req_data, encrypted_data_p);
    MMGSDIUTIL_TMC_MEM_FREE(new_req_data.feature_data.buffer_p);
    return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_AUTOLOCK

DESCRIPTION
  This function will be called for each personalization procedure that occurs.
  It will be the main personalization handling function used durint the init
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_autolock(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8*                                 ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p)
{

  gsdi_returns_T            gsdi_status        = GSDI_SUCCESS;
  uint32                    i                  = 0;
  boolean                   cnl_valid          = TRUE;
  boolean                   sp_locked          = FALSE;
  boolean                   cp_locked          = FALSE;
  uint32                    feature;

  gsdi_perso_sec_client_req_type   req_copy;
  gsdi_perso_sec_data_type         new_sim_lock_code;

#ifdef FEATURE_PERSO_SIM
  uint32            mask            = GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT + 1;
  uint32            len_index       = 0;
  uint32            index           = 0;
#elif defined(FEATURE_PERSO_RUIM)
  uint32            mask            = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
#endif

  gsdi_perso_sec_inds_state_enum_type ind_state = GSDI_PERSO_SEC_ACTIVATE_FEATURE;

  MSG_HIGH("PERSO: gsdi_perso_security_autolock",0,0,0);

  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(req_data_p);

  new_sim_lock_code.buffer_p = mmgsdi_malloc(GSDI_PERSO_BUFFER_LEN);
  MMGSDI_RETURN_IF_NULL(new_sim_lock_code.buffer_p);
  new_sim_lock_code.len = 0;
  *perso_event_p = GSDI_PERSO_NO_EVENT;

  /*Copy the Original Request Data*/
  memcpy(&req_copy, req_data_p, sizeof(gsdi_perso_sec_client_req_type));

#ifdef FEATURE_PERSO_SIM
  /*Set the Mask for the features that this feature depends on*/
  gsdi_perso_sec_set_dep_mask(&req_copy);
#endif


  if(sim_data_p->cnl.len > 0)
  {
    for(i=0;i<GSDI_PERSO_SEC_MAX_NUM_INDS;i++)
    {
      feature = req_copy.feature_mask & mask;
      switch (feature)
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_MASK_FEAT_NW:
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_NW_INDICATOR; /*CODE */
          cnl_valid = gsdi_perso_sec_build_nw_code(&new_sim_lock_code, sim_data_p);
          break;

        case GSDI_PERSO_SEC_MASK_FEAT_NS:
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_NS_INDICATOR; /*Code*/
          cnl_valid = gsdi_perso_sec_build_ns_code(&new_sim_lock_code, sim_data_p);
          break;

        case GSDI_PERSO_SEC_MASK_FEAT_SP:
          index =0;
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_SP_INDICATOR; /*Code*/
          new_sim_lock_code.len ++;
          len_index = new_sim_lock_code.len;
          new_sim_lock_code.len += GSDI_PERSO_ENGINE_LEN_SIZE;

          if(sim_data_p->cnl.buffer_p[index] == 0xFF)
          {
            cnl_valid = FALSE;
            break;
          }

          while ( index < (uint32)sim_data_p->cnl.len )
          {
            if(new_sim_lock_code.len + GSDI_PERSO_SEC_SP_CODE_LEN > GSDI_PERSO_BUFFER_LEN)
            {
              MSG_ERROR("PERSO: Exceeding the amount of memory allocated, Len= 0x%x",
                        new_sim_lock_code.len,0,0 );
              cnl_valid = FALSE;
              break;
            }

            new_sim_lock_code.buffer_p[len_index +1] += GSDI_PERSO_SEC_SP_CODE_LEN;

            memcpy((new_sim_lock_code.buffer_p + new_sim_lock_code.len),
                   (sim_data_p->cnl.buffer_p + index),
                    GSDI_PERSO_SEC_CNL_SP_BYTES);

            new_sim_lock_code.len += GSDI_PERSO_SEC_CNL_SP_BYTES;

            new_sim_lock_code.buffer_p[new_sim_lock_code.len] =
                        sim_data_p->cnl.buffer_p[index +GSDI_PERSO_SEC_SP_CODE_LEN] ;

            new_sim_lock_code.len ++;

            /* Increment the index by 6 bytes every time to move the pointer
            ** down each Set of CNL Data Blocks
            */
            index += GSDI_PERSO_SEC_CNL_BLOCK_SIZE;
            sp_locked = TRUE;

          }
          break;

        case GSDI_PERSO_SEC_MASK_FEAT_CP:
          index =0;
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_CP_INDICATOR; /*Code*/
          new_sim_lock_code.len ++;
          len_index = new_sim_lock_code.len;
          new_sim_lock_code.len += GSDI_PERSO_ENGINE_LEN_SIZE;

          if(sim_data_p->cnl.buffer_p[index] == 0xFF)
          {
            cnl_valid = FALSE;
            break;
          }

          while ( index < (uint32)sim_data_p->cnl.len )
          {
            if(new_sim_lock_code.len + GSDI_PERSO_SEC_CP_CODE_LEN > GSDI_PERSO_BUFFER_LEN)
            {
              MSG_ERROR("PERSO: Exceeding the amount of memory allocated, Len= 0x%x",
                        new_sim_lock_code.len,0,0 );
              cnl_valid = FALSE;
              break;
            }

            new_sim_lock_code.buffer_p[len_index +1]+= GSDI_PERSO_SEC_CP_CODE_LEN;

            memcpy((new_sim_lock_code.buffer_p + new_sim_lock_code.len),
                   (sim_data_p->cnl.buffer_p + index),
                    GSDI_PERSO_SEC_CNL_CP_BYTES);

            new_sim_lock_code.len += GSDI_PERSO_SEC_CNL_CP_BYTES;

            new_sim_lock_code.buffer_p[new_sim_lock_code.len] =
                        sim_data_p->cnl.buffer_p[index +GSDI_PERSO_SEC_CP_CODE_LEN -1] ;

            new_sim_lock_code.len ++;

            new_sim_lock_code.buffer_p[new_sim_lock_code.len] =
                        sim_data_p->cnl.buffer_p[index +GSDI_PERSO_SEC_CP_CODE_LEN] ;

            new_sim_lock_code.len ++;

            /* Increment the index by 6 bytes every time to move the pointer
            ** down each Set of CNL Data Blocks
            */
            index += GSDI_PERSO_SEC_CNL_BLOCK_SIZE;
            cp_locked = TRUE;
          }
          break;

        case GSDI_PERSO_SEC_MASK_FEAT_SIM:
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_SIM_INDICATOR; /*Code*/
          cnl_valid = gsdi_perso_sec_build_card_code(&new_sim_lock_code, sim_data_p);
          break;
#endif

#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_RUIM_NW1_INDICATOR; /*CODE*/
          cnl_valid = gsdi_perso_sec_build_nw_code(&new_sim_lock_code, sim_data_p);
          break;

        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_RUIM_NW2_INDICATOR; /*Code*/
          cnl_valid = gsdi_perso_sec_build_ns_code(&new_sim_lock_code, sim_data_p);
          break;

        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
          new_sim_lock_code.buffer_p[new_sim_lock_code.len] = GSDI_PERSO_RUIM_RUIM_INDICATOR; /*Code*/
          cnl_valid = gsdi_perso_sec_build_card_code(&new_sim_lock_code, sim_data_p);

          break;
#endif

        default:
          break;
      }/*End of Switch*/
      if(!cnl_valid)
      {
        /*if CNL is not Valid, do not continue in this for loop*/
        break;
      }
      mask = mask << 1;
    }/*End of For*/
  }/*if sim_data_p->cnl.len*/
  else
  {
    cnl_valid = FALSE;
  }

  if(!cnl_valid)
  {
    /*Build the Sim Lock Code from IMSI/GID1and GID2*/
    if(gsdi_perso_sec_build_sim_codes(sim_data_p,&new_sim_lock_code, &sp_locked,
                                      &cp_locked,
                                      req_copy.feature_mask) != GSDI_SUCCESS)
    {
       MSG_ERROR("PERSO: Could not Build codes from IMSI",0,0,0);
       MMGSDIUTIL_TMC_MEM_FREE(new_sim_lock_code.buffer_p);
       return GSDI_PERSO_INVALID_DATA;
    }
  }

  if(new_sim_lock_code.len == 0)
  {
    MSG_ERROR("PERSO: No Build Codes, Length is 0",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(new_sim_lock_code.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /*--------------------------------------------------------------------------
   Update the Feature Indicators and the SIM Lock Codes
   --------------------------------------------------------------------------*/

#ifdef FEATURE_PERSO_SIM
  mask            = GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT + 1;
#elif defined(FEATURE_PERSO_RUIM)
  mask            = GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1;
#endif


  for(i=0;i<GSDI_PERSO_SEC_MAX_NUM_INDS;i++)
  {
    ind_state = GSDI_PERSO_SEC_ACTIVATE_FEATURE;
    feature = req_copy.feature_mask & mask;
    mask = mask << 1;

    switch(feature)
    {
#ifdef FEATURE_PERSO_SIM
      /*--------------------------------------------------------------------------
          NS, CP, SP and SIM all depend on NW. If NW was not in the original
          request, then do not activate NW, only the codes should be copied over.
      --------------------------------------------------------------------------*/
      case GSDI_PERSO_SEC_MASK_FEAT_NW:

        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_NW )
          break;

        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_NS)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
          break;
        }

        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_CP &&
           cp_locked)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
          break;
        }

        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_SP &&
           sp_locked)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
          break;
        }

        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_SIM)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
          break;
        }
        continue;

      /*--------------------------------------------------------------------------
          SIM depends on NS. If NS was not in the original request, then do not
          activate NS, only the codes should be copied over.
       --------------------------------------------------------------------------*/
      case GSDI_PERSO_SEC_MASK_FEAT_NS:
        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_NS )
          break;

        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_SIM)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
        }
        break;
      /*--------------------------------------------------------------------------
         NO other feature depends on SIM
       --------------------------------------------------------------------------*/
      case GSDI_PERSO_SEC_MASK_FEAT_SIM:
        break;

      /*--------------------------------------------------------------------------
          NO other feature depends on CP
       --------------------------------------------------------------------------*/
      case GSDI_PERSO_SEC_MASK_FEAT_CP:
        if(!cp_locked)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
        }
        break;

      /*--------------------------------------------------------------------------
         CP depends on SP. If CP was in the original request and CP could not
         be locked, then back out SP as well regardless of sp_locked state
       --------------------------------------------------------------------------*/
      case GSDI_PERSO_SEC_MASK_FEAT_SP:
        if(req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_CP &&
           !cp_locked)
        {
          continue;
        }

        if(((req_data_p->feature_mask & GSDI_PERSO_SEC_MASK_FEAT_SP) == GSDI_PERSO_SEC_MASK_FEAT_SP)
           || !sp_locked)
        {
          ind_state = GSDI_PERSO_SEC_DEACTIVATE_FEATURE;
        }
        break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
      case GSDI_PERSO_SEC_IND_OFFSET_RUIM_HRPD_OFFSET:
      case GSDI_PERSO_SEC_IND_OFFSET_RUIM_CP_OFFSET:
      case GSDI_PERSO_SEC_IND_OFFSET_RUIM_SP_OFFSET:
        continue;

      case GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW1_OFFSET:
      case GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW2_OFFSET:
      case GSDI_PERSO_SEC_IND_OFFSET_RUIM_RUIM_OFFSET:
        break;

#endif /* FEATURE_PERSO_RUIM */

      default:
        continue;
    }

   /* --------------------------------------------------------------------------
      Update the SIM Lock Codes for this Feature so that it can be written back
      to the file system by the personalization engine.
     ------------------------------------------------------------------------*/
    gsdi_status = gsdi_perso_sec_update_lock_codes(&new_sim_lock_code,feature,encrypted_data_p);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not updated Lock Codes for Feature 0x%x",
                             gsdi_status,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(new_sim_lock_code.buffer_p);
      return GSDI_PERSO_INVALID_DATA;
    }

   /* --------------------------------------------------------------------------
      Update the Indicator data so that it can be written back
      to the file system by the personalization engine.
     ------------------------------------------------------------------------*/
    gsdi_status = gsdi_perso_security_change_feature_inds(feature,
                                                         encrypted_data_p,
                                                         ind_state);

     if ( gsdi_status != GSDI_SUCCESS )
     {
        MMGSDI_DEBUG_MSG_ERROR("PERSO: AUTOLOCK FAILURE: 0x%x",gsdi_status,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(new_sim_lock_code.buffer_p);
        return gsdi_status;
     }
  }

  *ret_data_len_p = 0;
  *ret_data_p     = 0;

  /*Feature Indicator has changed, update the File System*/
  encrypted_data_p->data_mod.update_fs = TRUE;
  MMGSDIUTIL_TMC_MEM_FREE(new_sim_lock_code.buffer_p);

  return GSDI_SUCCESS;
}/*lint !e715*/

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_CREATE_DATA

DESCRIPTION
  This function is used to autogenerate the control keys, indicators,
  and number of retries.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_security_create_data(
  gsdi_perso_sec_me_data_type    * encrypted_data_p,
  const uint8                    * data_p,
  int32                          * returned_data_len_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_ERROR;
#ifdef FEATURE_PERSO_SIM
  int32  index                = 0;
  gsdi_perso_sec_data_type      raw_buffer_p;
  uint32  pos                  =0;
  uint32  len                  =0;

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
#endif


#ifdef FEATURE_PERSO_SIM
  /* --------------------------------------------------------------------------
     First Autogenerate the control Keys
     ------------------------------------------------------------------------*/
  pos  = 0;

  /* Use the random number generate to populate the ck_buffer */
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
  *returned_data_len_p = 0;

  raw_buffer_p.buffer_p = mmgsdi_malloc(GSDI_PERSO_MAX_CK);
  MMGSDI_RETURN_IF_NULL(raw_buffer_p.buffer_p);
  raw_buffer_p.len      = GSDI_PERSO_MAX_CK;

  while ( index < GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS )
  {

    memset(raw_buffer_p.buffer_p,0,GSDI_PERSO_MAX_CK);
    raw_buffer_p.len = GSDI_PERSO_MAX_CK;

    gsdi_perso_sec_create_control_key(raw_buffer_p.buffer_p);

   /* Encrypt the Data Accordingly */
    gsdi_status = gsdi_perso_sec_encrypt_tlv(&(encrypted_data_p->dcks.buffer_p[pos]),
                                             &raw_buffer_p,
                                             &len,
                                             indicators[index]);

    pos += len;

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
      return GSDI_PERSO_INVALID_DATA;
    }

    index++;
  }

  /*Unblock Keys*/

  pos  = 0;
  index =0;

  /* Use the random number generate to populate the ck_buffer */
  raw_buffer_p.len      = GSDI_PERSO_MAX_CK;

  while ( index < GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS )
  {

    memset(raw_buffer_p.buffer_p,0,GSDI_PERSO_MAX_CK);
    raw_buffer_p.len = GSDI_PERSO_MAX_CK;

    gsdi_perso_sec_create_control_key(raw_buffer_p.buffer_p);

   /* Encrypt the Data Accordingly */
    gsdi_status = gsdi_perso_sec_encrypt_tlv(&(encrypted_data_p->unblock_cks.buffer_p[pos]),
                                             &raw_buffer_p,
                                             &len,
                                             indicators[index]);

    pos += len;

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
      return GSDI_PERSO_INVALID_DATA;
    }

    index++;
  }


  /* --------------------------------------------------------------------------
     Now write the Number of retries for the different Levels
     ------------------------------------------------------------------------*/
  raw_buffer_p.len = 5;
  memcpy(raw_buffer_p.buffer_p, num_retries, 5);

  gsdi_status = gsdi_perso_security_encrypt_data(&raw_buffer_p,
                                                 &encrypted_data_p->dck_counts);


  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  encrypted_data_p->data_mod.update_fs = TRUE;

  memcpy(raw_buffer_p.buffer_p, unblock_num_retries, 5);
  gsdi_status = gsdi_perso_security_encrypt_data(&raw_buffer_p,
                                                 &encrypted_data_p->unblock_ck_counts);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Now write the Feature Indicators
     ------------------------------------------------------------------------*/
  raw_buffer_p.len = 5;
  memset(raw_buffer_p.buffer_p, (uint8)GSDI_PERSO_ENG_NOT_ACTIVE_STATE, 5);

  gsdi_status = gsdi_perso_security_encrypt_data(&raw_buffer_p,
                                                 &encrypted_data_p->feat_inds);

  MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

#endif /*FEATURE_PERSO_SIM*/
  return gsdi_status;

}/*lint !e715*/

#ifdef FEATURE_PERSO_RUIM
/*===========================================================================
FUNCTION GSDI_PERSO_RUIM_SECURITY_CREATE_DATA

DESCRIPTION
  This function is used to autogenerate the control keys, indicators,
  and number of retries.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_ruim_security_create_data(
  gsdi_perso_sec_me_data_type    * encrypted_data_p,
  const uint8                    * data_p,
  int32                          * returned_data_len_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_ERROR;
  int32  index                = 0;
  gsdi_perso_sec_data_type      raw_buffer_p;
  uint32  pos                  =0;
  uint32  len                  =0;

  uint8  num_retries[6]       = {GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES,
                                 GSDI_PERSO_MAX_NUMBER_OF_RETRIES};

  uint8  unblock_num_retries[6]  = {GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES,
                                    GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES};

  uint8  indicators[6]        = {GSDI_PERSO_RUIM_NW1_INDICATOR,
                                 GSDI_PERSO_RUIM_NW2_INDICATOR,
                                 GSDI_PERSO_RUIM_HRPD_INDICATOR,
                                 GSDI_PERSO_RUIM_SP_INDICATOR,
                                 GSDI_PERSO_RUIM_CP_INDICATOR,
                                 GSDI_PERSO_RUIM_RUIM_INDICATOR};

  /* --------------------------------------------------------------------------
     First Autogenerate the control Keys
     ------------------------------------------------------------------------*/
  pos  = 0;

  /* Use the random number generate to populate the ck_buffer */
  raw_buffer_p.buffer_p = mmgsdi_malloc(GSDI_PERSO_MAX_CK);
  MMGSDI_RETURN_IF_NULL(raw_buffer_p.buffer_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  raw_buffer_p.len      = GSDI_PERSO_MAX_CK;

  while ( index < GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS )
  {

    memset(raw_buffer_p.buffer_p,0,GSDI_PERSO_MAX_CK);
    raw_buffer_p.len = GSDI_PERSO_MAX_CK;

    gsdi_perso_sec_create_control_key(raw_buffer_p.buffer_p);

   /* Encrypt the Data Accordingly */
    gsdi_status = gsdi_perso_sec_encrypt_tlv(&(encrypted_data_p->dcks.buffer_p[pos]),
                                             &raw_buffer_p,
                                             &len,
                                             indicators[index]);

    pos += len;

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
      return GSDI_PERSO_INVALID_DATA;
    }

    index++;
  }

  /*Unblock Keys*/

  pos  = 0;
  index =0;

  /* Use the random number generate to populate the ck_buffer */
  raw_buffer_p.len      = GSDI_PERSO_MAX_CK;

  while ( index < GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS )
  {

    memset(raw_buffer_p.buffer_p,0,GSDI_PERSO_MAX_CK);
    raw_buffer_p.len = GSDI_PERSO_MAX_CK;

    gsdi_perso_sec_create_control_key(raw_buffer_p.buffer_p);

   /* Encrypt the Data Accordingly */
    gsdi_status = gsdi_perso_sec_encrypt_tlv(&(encrypted_data_p->unblock_cks.buffer_p[pos]),
                                             &raw_buffer_p,
                                             &len,
                                             indicators[index]);

    pos += len;

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
      return GSDI_PERSO_INVALID_DATA;
    }

    index++;
  }


  /* --------------------------------------------------------------------------
     Now write the Number of retries for the different Levels
     ------------------------------------------------------------------------*/
  raw_buffer_p.len = GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  memcpy(raw_buffer_p.buffer_p, num_retries, GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS);

  gsdi_status = gsdi_perso_security_encrypt_data(&raw_buffer_p,
                                                 &encrypted_data_p->dck_counts);


  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  encrypted_data_p->data_mod.update_fs = TRUE;

  memcpy(raw_buffer_p.buffer_p, unblock_num_retries, GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS);
  gsdi_status = gsdi_perso_security_encrypt_data(&raw_buffer_p,
                                                 &encrypted_data_p->unblock_ck_counts);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Now write the Feature Indicators
     ------------------------------------------------------------------------*/
  raw_buffer_p.len = GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS;
  memset(raw_buffer_p.buffer_p, (uint8)GSDI_PERSO_ENG_NOT_ACTIVE_STATE,
         GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS);

  gsdi_status = gsdi_perso_security_encrypt_data(&raw_buffer_p,
                                                 &encrypted_data_p->feat_inds);

  MMGSDIUTIL_TMC_MEM_FREE(raw_buffer_p.buffer_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  *returned_data_len_p = 0;
  return gsdi_status;

}/*lint !e715*/
#endif /* FEATURE_PERSO_RUIM */

/*===========================================================================
FUNCTION GSDI_PERSO_SEC_RESET_NUM_RETRIES

DESCRIPTION
  Resets The number of retries for DCK and Unblock CK

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_sec_reset_num_retries(
  uint32                       feature,
  gsdi_perso_sec_me_data_type *encrypted_data_p)
{
  gsdi_returns_T                 gsdi_status = GSDI_ERROR;
  int32                          offset        = 0x00;
  gsdi_perso_sec_data_type       decrypted_data;

  MMGSDI_RETURN_IF_NULL(encrypted_data_p);

  offset = gsdi_perso_sec_get_offset(feature);
  if(offset == 0xFFFF)
  {
     return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
    Allocate memory to hold the uncrypted values
    -------------------------------------------------------------------------*/
  decrypted_data.buffer_p = mmgsdi_malloc((encrypted_data_p->dck_counts.len/2));
  MMGSDI_RETURN_IF_NULL(decrypted_data.buffer_p);

 /* ---------------------------------------------------------------------------
    Get the Control Key Counters
    -------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_security_decrypt_data(&encrypted_data_p->dck_counts,
                                                 &decrypted_data);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
    return gsdi_status;
  }

  decrypted_data.buffer_p[offset] =  GSDI_PERSO_MAX_NUMBER_OF_RETRIES;

  gsdi_status = gsdi_perso_security_encrypt_data(&decrypted_data,
                                                &encrypted_data_p->dck_counts);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

/* ---------------------------------------------------------------------------
    Get the Control Key Counters
    -------------------------------------------------------------------------*/
 gsdi_status = gsdi_perso_security_decrypt_data(&encrypted_data_p->unblock_ck_counts,
                                                &decrypted_data);

 if ( gsdi_status != GSDI_SUCCESS )
 {
   MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
   return gsdi_status;
 }

 decrypted_data.buffer_p[offset] =  GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES;

 gsdi_status = gsdi_perso_security_encrypt_data(&decrypted_data,
                                                &encrypted_data_p->unblock_ck_counts);


  MMGSDIUTIL_TMC_MEM_FREE(decrypted_data.buffer_p);
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_UNBLOCK_DCK

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
static gsdi_returns_T gsdi_perso_security_unblock_dck(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T gsdi_status                       = GSDI_SUCCESS;
  gsdi_returns_T unlock_status                     = GSDI_SUCCESS;
  gsdi_returns_T unblock_status                    = GSDI_SUCCESS;
  uint32         len                               = 0;
  gsdi_perso_sec_data_type       control_keys      = {0};
  gsdi_perso_sec_data_type       features          = {0};
  gsdi_perso_sec_inds_state_type feature_states;
  gsdi_perso_event_enum_type     local_perso_event = GSDI_PERSO_NO_EVENT;

  MSG_HIGH("PERSO: gsdi_perso_security_unblock_dck",0,0,0);

  /* --------------------------------------------------------------------------
     Ensure the Pointer provided is not NULL
     and initialize the data.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  /* -------------------------------------------------------------------------
      Determine how much memory is required to store the
      decrypted Featuer Indications.
      -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  /* --------------------------------------------------------------------------
     Next resolve the data provided in the feature indicators and
     determine the state of each one.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                        &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                            gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Does the state of the indicators match the requested
     action.  Simply populate and indicate so.  The logic to allow
     or not allow this procedure is handled in another function.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_action_allowed(
                  GSDI_PERSO_SEC_PROC_DEACTIVATE_FEATURE,
                  &features,
                  &feature_states);

  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required
     ------------------------------------------------------------------------*/
  features.len      = 0;

  /*Is it really Blocked?*/
   unlock_status = gsdi_perso_sec_state_consistent(req_data_p->feature_mask,
                                                &encrypted_data_p->feat_inds,
                                                &encrypted_data_p->dck_counts,
                                                &feature_states,
                                                &local_perso_event);
  if ( unlock_status != GSDI_PERSO_CK_BLOCKED )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Feature Not Blocked 0x%x",
                           unlock_status,0,0);

    unlock_status = GSDI_PERSO_CHECK_FAILED;
  }

  /*Check for Unblock counts only if dck count = 0*/
  if(unlock_status == GSDI_PERSO_CK_BLOCKED)
  {
    /* ----------------------------------------------------------------------
      Is the state of the indicator consistent with the ind provided
      by the requesting client
      ----------------------------------------------------------------------*/
    unblock_status = gsdi_perso_sec_state_consistent(
                       req_data_p->feature_mask,
                       &encrypted_data_p->feat_inds,
                       &encrypted_data_p->unblock_ck_counts,
                       &feature_states,
                       perso_event_p);

    if ( unblock_status == GSDI_PERSO_CK_BLOCKED )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Feature Perm Blocked 0x%x",
                             gsdi_status,0,0);

      *perso_event_p = GSDI_PERSO_SANITY_ERROR;
      unblock_status =  GSDI_ERROR;
    }

    if (unblock_status == GSDI_PERSO_INCONSISTENT_W_IND)
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: STATE CONSISTENCY CHECK ERROR 0x%x",
                              unblock_status,0,0);

      /*Reset the event with the Blocked event from above*/
      *perso_event_p = local_perso_event;

      /*This will happen when the user is trying to activate the feature
        and then it becomes blocked. At this time, we should allow unblocking
        to happen*/
      unblock_status = GSDI_SUCCESS;
    }
  }

  if(unlock_status  == GSDI_PERSO_CK_BLOCKED &&
     unblock_status == GSDI_SUCCESS)
  {
    control_keys.buffer_p = mmgsdi_malloc(len);
    MMGSDI_RETURN_IF_NULL(control_keys.buffer_p);
    /* --------------------------------------------------------------------------
      Verify the Personalization Control Key
      ------------------------------------------------------------------------*/
    unblock_status = gsdi_perso_security_verify_ck(
                       req_data_p,
                       &encrypted_data_p->unblock_cks,
                       &encrypted_data_p->unblock_ck_counts,
                       &encrypted_data_p->data_mod,
                       &control_keys,
                       perso_event_p,
                       GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES
                     );

    /* --------------------------------------------------------------------------
      Information for the Control Keys is no longer required, ok to
      Free Now
    ------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(control_keys.buffer_p);
    control_keys.buffer_p = NULL;
    control_keys.len      = 0;

    if ( unblock_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Personalization Feature: 0x%x, failed CK Check",
                              req_data_p->feature_mask,0,0);

      if(unblock_status == GSDI_PERSO_CK_BLOCKED)
      {
        /*Reset the event with the Blocked event from above*/
        *perso_event_p  = GSDI_PERSO_SANITY_ERROR;
        unblock_status =  GSDI_ERROR;
      }

      /*Reset the event with the Blocked event from above*/
      *perso_event_p = local_perso_event;

      encrypted_data_p->data_mod.update_fs = TRUE;
    }
    else
    {
      /*Reset the Unlock status as well*/
      unlock_status = GSDI_SUCCESS;
    }
  }

  if(unblock_status == GSDI_SUCCESS  &&
     unlock_status  == GSDI_SUCCESS)
  {
    /* -----------------------------------------------------------------------
        Update the Indicator data so that it can be written back
        to the file system by the personalization engine.
        ---------------------------------------------------------------------*/
    gsdi_status = gsdi_perso_security_change_feature_inds(
                    req_data_p->feature_mask,
                    encrypted_data_p,
                    GSDI_PERSO_SEC_DEACTIVATE_FEATURE);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: DEACTIVATE FEATURE FAILURE: 0x%x",
                             gsdi_status,0,0);
      return gsdi_status;
    }
    else
    {
      switch(req_data_p->feature_mask)
      {
#ifdef FEATURE_PERSO_SIM
        case GSDI_PERSO_SEC_MASK_FEAT_NW:
          *perso_event_p = GSDI_PERSO_NCK_UNBLOCKED;
           break;
        case GSDI_PERSO_SEC_MASK_FEAT_NS:
          *perso_event_p = GSDI_PERSO_NSK_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_SP:
          *perso_event_p = GSDI_PERSO_SPK_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_CP:
          *perso_event_p = GSDI_PERSO_CCK_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_SIM:
          *perso_event_p = GSDI_PERSO_PPK_UNBLOCKED;
          break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1:
          *perso_event_p = GSDI_PERSO_RUIM_NCK1_UNBLOCKED;
           break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2:
          *perso_event_p = GSDI_PERSO_RUIM_NCK2_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD:
          *perso_event_p = GSDI_PERSO_RUIM_HNCK_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP:
          *perso_event_p = GSDI_PERSO_RUIM_SPCK_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP:
          *perso_event_p = GSDI_PERSO_RUIM_CCK_UNBLOCKED;
          break;
        case GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM:
          *perso_event_p = GSDI_PERSO_RUIM_PCK_UNBLOCKED;
          break;
#endif /* FEATURE_PERSO_RUIM */
        default:
          break;
      }
    }

    gsdi_status =  gsdi_perso_sec_reset_num_retries(req_data_p->feature_mask,
                                                    encrypted_data_p);

    if(gsdi_status != GSDI_SUCCESS)
    {
      MSG_ERROR("PERSO: Could not reset number of retries",0,0,0);
      return GSDI_PERSO_INVALID_DATA;
    }

    encrypted_data_p->data_mod.update_fs = TRUE;
  }

  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->unblock_ck_counts,
                                                 &len,
                                                 req_data_p->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->unblock_ck_counts,
                                        &features);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: GET Data FAILURE: 0x%x",gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  memcpy(ret_data_p,features.buffer_p,features.len);
  *ret_data_len_p = uint32toint32(features.len);

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required, ok to
     Free Now
      ------------------------------------------------------------------------*/
  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
  features.buffer_p = NULL;
  features.len      = 0;

  if(unblock_status != GSDI_SUCCESS)
  {
    return unblock_status;
  }
  else if(unlock_status != GSDI_SUCCESS)
  {
    return unlock_status;
  }
  return GSDI_SUCCESS;
} /* gsdi_perso_security_unblock_dck */


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_GET_DCK_NUM_RETRIES

DESCRIPTION
  This function will be used to get the dck number of retries.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_get_dck_num_retries(
  const gsdi_perso_sec_client_req_type * req_data_ptr,
  gsdi_perso_sec_me_data_type          * encrypted_data_ptr,
  const gsdi_perso_sec_sim_data_type   * sim_data_ptr,
  uint8 *                                ret_data_ptr,
  int32*                                 ret_data_len_ptr,
  gsdi_perso_event_enum_type           * perso_event_ptr
)
{
  gsdi_returns_T                 gsdi_status       = GSDI_SUCCESS;
  uint32                         len               = 0;
  gsdi_perso_sec_data_type       dck_retries       = {0};

  MSG_HIGH("PERSO: gsdi_perso_security_get_dck_num_retries",0,0,0);

  /* --------------------------------------------------------------------------
     Ensure the Pointer provided is not NULL
     and initialize the data.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_ptr);
  MMGSDI_RETURN_IF_NULL(encrypted_data_ptr);
  MMGSDI_RETURN_IF_NULL(sim_data_ptr);
  MMGSDI_RETURN_IF_NULL(ret_data_ptr);
  MMGSDI_RETURN_IF_NULL(ret_data_len_ptr);
  MMGSDI_RETURN_IF_NULL(perso_event_ptr);

  /* -------------------------------------------------------------------------
    Determine how much memory is required to store the
    decrypted dck counts.
   -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_ptr->dck_counts,
                                                 &len,
                                                 req_data_ptr->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  dck_retries.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(dck_retries.buffer_p);

  /* Decode the encryted dck counts */
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_ptr->dck_counts,
                                        &dck_retries);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: dck_num_retries Get DCK Count Failed: 0x%x",
                            gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(dck_retries.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* No event is generated */
  *perso_event_ptr = GSDI_PERSO_NO_EVENT;

  /* Copy the data to the return buffer */
  memcpy(ret_data_ptr,dck_retries.buffer_p,dck_retries.len);
  *ret_data_len_ptr = uint32toint32(dck_retries.len);

  MMGSDIUTIL_TMC_MEM_FREE(dck_retries.buffer_p);

  return GSDI_SUCCESS;

} /* gsdi_perso_security_get_dck_num_retries*/


/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_GET_DCK_UNBLOCK_NUM_RETRIES

DESCRIPTION
  This function will be used to get the dck unblock number of retries.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_get_dck_unblock_num_retries(
  const gsdi_perso_sec_client_req_type * req_data_ptr,
  gsdi_perso_sec_me_data_type          * encrypted_data_ptr,
  const gsdi_perso_sec_sim_data_type   * sim_data_ptr,
  uint8 *                                ret_data_ptr,
  int32*                                 ret_data_len_ptr,
  gsdi_perso_event_enum_type           * perso_event_ptr
)
{
  gsdi_returns_T                 gsdi_status         = GSDI_SUCCESS;
  uint32                         len                 = 0;
  gsdi_perso_sec_data_type       dck_unblock_retries = {0};

  MSG_HIGH("PERSO: gsdi_perso_security_get_dck_unblock_num_retries",0,0,0);

  /* --------------------------------------------------------------------------
     Ensure the Pointer provided is not NULL
     and initialize the data.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_ptr);
  MMGSDI_RETURN_IF_NULL(encrypted_data_ptr);
  MMGSDI_RETURN_IF_NULL(sim_data_ptr);
  MMGSDI_RETURN_IF_NULL(ret_data_ptr);
  MMGSDI_RETURN_IF_NULL(ret_data_len_ptr);
  MMGSDI_RETURN_IF_NULL(perso_event_ptr);

  /* -------------------------------------------------------------------------
    Determine how much memory is required to store the
    decrypted dck ublock counts.
   -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_ptr->unblock_ck_counts,
                                                 &len,
                                                 req_data_ptr->feature_mask);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }

  dck_unblock_retries.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(dck_unblock_retries.buffer_p);

  /* Decode the encryted unblock dck counts */
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_ptr->unblock_ck_counts,
                                        &dck_unblock_retries);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: dck_num_retries Get DCK Count Failed: 0x%x",
                            gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(dck_unblock_retries.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* No event is generated */
  *perso_event_ptr = GSDI_PERSO_NO_EVENT;

  /* Copy the data to the return buffer */
  memcpy(ret_data_ptr,dck_unblock_retries.buffer_p,dck_unblock_retries.len);
  *ret_data_len_ptr = uint32toint32(dck_unblock_retries.len);

  MMGSDIUTIL_TMC_MEM_FREE(dck_unblock_retries.buffer_p);

  return GSDI_SUCCESS;

} /* gsdi_perso_security_get_dck_unblock_num_retries */


#ifdef FEATURE_PERSO_SIM
/*===========================================================================
FUNCTION GSDI_PERSO_SEC_IS_KEY_DEFAULT_KEY


DESCRIPTION
  This function will be used to determine if the Key provided is the default
  key.

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE  - Key is the default CK of 0xFF ... 0xFF
            FALSE - Keys is not the default CK
===========================================================================*/
static boolean gsdi_perso_sec_is_key_default_ck(
  uint32                                 feature,
  const gsdi_perso_sec_sim_ele_data_type *data_p
)
{
  uint8   start_index = 0;

  if(NULL == data_p)
     return FALSE;

  /* --------------------------------------------------------------------------
     Make sure the CK Len doesn't exceed the maximum number of Control Key
     bytes allowed
     ------------------------------------------------------------------------*/
  if ( (data_p->len !=
       (GSDI_PERSO_ENGINE_SIM_DCK_NUM_CKS * GSDI_PERSO_ENGINE_DCK_CK_LEN)))
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
  case GSDI_PERSO_SEC_MASK_FEAT_NW:
    start_index = GSDI_PERSO_SEC_DCK_NW_CK_OFFSET;
    break;

  case GSDI_PERSO_SEC_MASK_FEAT_NS:
    start_index = GSDI_PERSO_SEC_DCK_NS_CK_OFFSET;
    break;

  case GSDI_PERSO_SEC_MASK_FEAT_SP:
    start_index = GSDI_PERSO_SEC_DCK_SP_CK_OFFSET;
    break;

  case GSDI_PERSO_SEC_MASK_FEAT_CP:
    start_index = GSDI_PERSO_SEC_DCK_CP_CK_OFFSET;
    break;

  default:
    MMGSDI_DEBUG_ERR("BAD FEATURE ID: 0x%x",feature,0,0);
    return FALSE;
  }

  if (!(memcmp(&data_p->buffer_p[start_index],
               mmgsdi_ck_not_set,
               GSDI_PERSO_ENGINE_DCK_CK_LEN)))
  {
    /* DCK is the default CK of 0xFF ... 0xFF */
    return TRUE;
  }

  /* Default CK of 0xFF ... 0xFF was not found */
  return FALSE;
}


/*===========================================================================
FUNCTION GSDI_PERSO_SEC_CONVERT_SIM_EF_DCK_TO_FS_CK

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
static gsdi_returns_T gsdi_perso_sec_convert_sim_ef_dck_to_fs_ck(
  uint32 feature,
  gsdi_perso_sec_data_type *data_p)
{
  uint32 start_index = 0;
  uint32 stop_index  = 0;
  gsdi_perso_sec_data_type temp_buf;

  /* --------------------------------------------------------------------------
     Check the Pointers to ensure they are not NULL
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(data_p);


  /* --------------------------------------------------------------------------
     Determine the required offset
     ------------------------------------------------------------------------*/
  switch ( feature )
  {
  case GSDI_PERSO_SEC_MASK_FEAT_NW:
    start_index = GSDI_PERSO_SEC_DCK_NW_CK_OFFSET;
    break;

  case GSDI_PERSO_SEC_MASK_FEAT_NS:
    start_index = GSDI_PERSO_SEC_DCK_NS_CK_OFFSET;
    break;

  case GSDI_PERSO_SEC_MASK_FEAT_SP:
    start_index = GSDI_PERSO_SEC_DCK_SP_CK_OFFSET;
    break;

  case GSDI_PERSO_SEC_MASK_FEAT_CP:
    start_index = GSDI_PERSO_SEC_DCK_CP_CK_OFFSET;
    break;

  default:
    /* Allow the depersonalization procedures to fail the
    ** procedures due to a bad key
    */
    MMGSDI_DEBUG_ERR("BAD FEATURE ID: 0x%x",feature,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

  /* --------------------------------------------------------------------------
     Calculate the upper bound or stop_index.  It should be the
     start_index + 4 Bytes
     ------------------------------------------------------------------------*/
  stop_index = GSDI_PERSO_ENGINE_DCK_CK_LEN + start_index;

  temp_buf.buffer_p = mmgsdi_malloc(GSDI_PERSO_MAX_CK);
  if (temp_buf.buffer_p == NULL)
  {
    MMGSDI_DEBUG_ERR("PERSO SEC: Memory allocation failed for temp_buf.buffer_p",
                     0,0,0);
    return GSDI_ERROR;
  }

  temp_buf.len =0;
  /* --------------------------------------------------------------------------
     Start converting from to ASCII
     ------------------------------------------------------------------------*/
  while ( start_index < stop_index )
  {
    temp_buf.buffer_p[temp_buf.len++] = (uint8)(data_p->buffer_p[start_index]  & 0x0F ) + 0x30;
    temp_buf.buffer_p[temp_buf.len++] = (uint8)((data_p->buffer_p[start_index] & 0xF0 ) >> 4) + 0x30;
    start_index++;
  }

  /* --------------------------------------------------------------------------
     Update the Data Len and Data Buffer Provided with the new Data
     ------------------------------------------------------------------------*/
  memcpy(data_p->buffer_p,temp_buf.buffer_p,temp_buf.len);
  data_p->len = temp_buf.len;


  MMGSDIUTIL_TMC_MEM_FREE(temp_buf.buffer_p);
  return GSDI_SUCCESS;
} /* gsdi_perso_sec_convert_sim_ef_dck_to_fs_ck */
#endif /*FEATURE_PERSO_SIM*/

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_REFRESH

DESCRIPTION
  This function will be used to control the reading of EF-DCK and
  depersonalize the features for which Control Keys exist.

  This function will also perform the clearing of the control keys.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_refresh(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p)
{
  gsdi_returns_T                        gsdi_status  = GSDI_ERROR;
#ifdef FEATURE_PERSO_SIM
  boolean                               is_default_ck= FALSE;
  uint8                                 i            = 0;
  uint32                                len =0;
  gsdi_perso_sec_data_type              features;
  gsdi_perso_sec_inds_state_type        feature_states;
  uint32                                feature = 1;
  gsdi_perso_sec_client_req_type        deact_req;
#endif

  MSG_HIGH("PERSO: gsdi_perso_security_refresh",0,0,0);

  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
  *perso_event_p  = GSDI_PERSO_NO_EVENT;
  *ret_data_len_p = 0;
  *ret_data_p     = 0;

#ifdef FEATURE_PERSO_SIM
  if (sim_data_p->dck.len == 0)
  {
    /* No need to perform deactivation or any check because
       there is no DCK data */
    return GSDI_SUCCESS;
  }

  /* -------------------------------------------------------------------------
     Determine how much memory is required to store the
     decrypted Featuer Indications.
  -----------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_calc_mem_required(GSDI_PERSO_SEC_CALC_INDS,
                                                 &encrypted_data_p->feat_inds,
                                                 &len,
                                                 0);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    return GSDI_PERSO_INVALID_DATA;
  }
  features.buffer_p = mmgsdi_malloc(len);
  MMGSDI_RETURN_IF_NULL(features.buffer_p);

  /* --------------------------------------------------------------------------
    Next resolve the data provided in the feature indicators and
    determine the state of each one.
   ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_get_data(&encrypted_data_p->feat_inds,
                                         &features);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Resolve Feature Indicators Failure 0x%x",
                           gsdi_status,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);
    return GSDI_PERSO_INVALID_DATA;
  }
  /* --------------------------------------------------------------------------
     Does the state of the indicators match the requested
     action.  Simply populate and indicate so.  The logic to allow
     or not allow this procedure is handled in another function.
   ------------------------------------------------------------------------*/
  gsdi_status = gsdi_perso_sec_action_allowed(GSDI_PERSO_SEC_PROC_DEACTIVATE_FEATURE,
                                              &features,
                                              &feature_states);

  MMGSDIUTIL_TMC_MEM_FREE(features.buffer_p);

  if(gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not Get Indication State Info",0,0,0);
    return gsdi_status;
  }

  /* --------------------------------------------------------------------------
     Information for the Feature Indicators is no longer required
    ------------------------------------------------------------------------*/
  features.len = 0;

  deact_req.dck.buffer_p = mmgsdi_malloc(
                                      MAX(sim_data_p->dck.len,
                                          GSDI_PERSO_MAX_CK));

  if(deact_req.dck.buffer_p == NULL)
  {
    MSG_ERROR("PERSO: NULL Pointer in gsdi_perso_security_refresh",0,0,0);
    return GSDI_PERSO_INVALID_DATA;
  }

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
  for ( i = 0; i < GSDI_PERSO_ENGINE_SIM_DCK_NUM_CKS; i++)
  {
    /* Now determine if Data read is the default key */
    is_default_ck = gsdi_perso_sec_is_key_default_ck(feature,
                                                     (const gsdi_perso_sec_sim_ele_data_type *)
                                                     &sim_data_p->dck);

    if (!is_default_ck )
    {
      /* need to wipe out the DCK if any of the key was not default
      ** regardless of whether the deactivation is required/success
      ** or fail
      */
      encrypted_data_p->data_mod.refresh = TRUE;

      /* The CK Retrieved is not the default... check for state to see
      ** if refresh is required
      */
      gsdi_status = gsdi_perso_sec_state_consistent(feature,
                                                    &encrypted_data_p->feat_inds,
                                                    &encrypted_data_p->dck_counts,
                                                    &feature_states,
                                                    perso_event_p);

      /* If Active and Not Perm Disabled, run refresh deperso procs */
      if ( gsdi_status == GSDI_SUCCESS)
      {
        /* go ahead and format the
        ** data as required by the deactiveatre_feature_ind function
        */
            memcpy(deact_req.dck.buffer_p,
               sim_data_p->dck.buffer_p,
               sim_data_p->dck.len);
          deact_req.dck.len = sim_data_p->dck.len;

        /*deact_req.dck_buffer_p is an IN-OUT buffer. It will contain the dcks
          as they are stored in the EF and will get converted to DCKS as they
          are stored in FS*/
        gsdi_status = gsdi_perso_sec_convert_sim_ef_dck_to_fs_ck(feature,
                                                                 &deact_req.dck);

        if(gsdi_status != GSDI_SUCCESS)
        {
          MMGSDIUTIL_TMC_MEM_FREE(deact_req.dck.buffer_p);
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not convert dck, feature 0x%x",i,0,0);
          return gsdi_status;
        }
        /* Assumption is...DCK is 8 Digits of Control Keys in ASCII Format  */
        deact_req.dck.len = GSDI_PERSO_MAX_CK;

        deact_req.feature_mask = feature;

        gsdi_status = gsdi_perso_security_deactivate(&deact_req,
                                                     encrypted_data_p,
                                                     sim_data_p,
                                                     ret_data_p,
                                                     ret_data_len_p,
                                                     perso_event_p );
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not deactivate feature: 0x%x",i,0,0);
          /* memfree */
          MMGSDIUTIL_TMC_MEM_FREE(deact_req.dck.buffer_p);
          return GSDI_PERSO_INVALID_DATA;
        }

      }
      else
      {
        /* reset status to gsdi success */
        gsdi_status = GSDI_SUCCESS;
        MSG_HIGH("PERSO: Feature: 0x%x is not set up for Deperso",i,0,0);
      }
    }
    else
    {
      MSG_HIGH("PERSO: CK for Feature 0x%x: DEFAULT CK",i,0,0);
      /* reset status to gsdi success */
      gsdi_status = GSDI_SUCCESS;
    }
    feature = feature << 1;
  } /* end of for loop */
  MMGSDIUTIL_TMC_MEM_FREE(deact_req.dck.buffer_p);
#endif /*FEATURE_PERSO_SIM*/
  return gsdi_status;
}/*lint !e715*/

/*===========================================================================
FUNCTION GSDI_PERSO_VERIFY_PHONE_CODE

DESCRIPTION
  This function will be used to compare the phone code given by the client
  with the existing phone code.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_security_verify_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T           gsdi_status        = GSDI_SUCCESS;
#ifdef FEATURE_PERSO_SIM
  nv_stat_enum_type        read_status;
  nv_stat_enum_type        read_ext_status;
  nv_item_type             gsdi_nv_lock_code_buf; /* NV data buffer */
  nv_item_type             gsdi_nv_lock_code_ext_buf; /* NV data buffer */
  nv_stat_enum_type        write_status;
  uint32                   feature_mask;
#endif /*FEATURE_PERSO_SIM*/

  MSG_HIGH("PERSO: gsdi_perso_security_verify_phone_code",0,0,0);
  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
  *perso_event_p  = GSDI_PERSO_NO_EVENT;
  *ret_data_len_p = 0;
  *ret_data_p     = 0;

#ifdef FEATURE_PERSO_SIM
  /* --------------------------------------------------------------------------
    First find out out if we can make the comparison or not i.e. is the max
    number of counts reached?
    -------------------------------------------------------------------------*/
  read_status = gsdi_get_nv(NV_LOCK_CODE_I,
                            &gsdi_nv_lock_code_buf);

  read_ext_status = gsdi_get_nv(NV_LOCK_CODE_EXT_I,
                            &gsdi_nv_lock_code_ext_buf);

  if ( read_status     == NV_NOTACTIVE_S ||
       read_ext_status == NV_NOTACTIVE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NV Item not Active - Should not HAPPEN!!!",0,0,0);
    return GSDI_ERROR;
  }
  else if(read_status     != NV_DONE_S ||
          read_ext_status != NV_DONE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NV Lock Code Item could not be read",0,0,0);
    gsdi_status = GSDI_PERSO_INVALID_CK;
    gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries++;
  }
  else
  {
    if(gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries == GSDI_PERSO_PHONE_CODE_EXHAUSTED)
    {
      return GSDI_PERSO_CK_BLOCKED;
    }
  }

  /* --------------------------------------------------------------------------
    Extract the control key from the Request provided and
    store a decrypted value in the feature_control_key_p buffer in this
    structure.
    -------------------------------------------------------------------------*/

  if(gsdi_status == GSDI_SUCCESS)
  {
    if(!memcmp(req_data_p->phone_code.buffer_p, (char*)gsdi_nv_lock_code_buf.lock_code.digits, NV_LOCK_CODE_SIZE))
    {
      gsdi_status = GSDI_SUCCESS;

      /*Reset the counter*/
      gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries = GSDI_PERSO_MAX_CK_RETRIES;
    }
    else{
      gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries--;
      gsdi_status = GSDI_PERSO_INVALID_CK;
    }
  }

  /* --------------------------------------------------------------------------
    Update the counter
    -------------------------------------------------------------------------*/
  write_status = gsdi_put_nv(NV_LOCK_CODE_EXT_I, &gsdi_nv_lock_code_ext_buf);

  if(write_status != NV_DONE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Failure to update counters 0x%x",
                           write_status,0,0);
    gsdi_status = GSDI_ERROR;
  }
  else if(gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries == GSDI_PERSO_PHONE_CODE_EXHAUSTED)
  {
    gsdi_status = GSDI_PERSO_CK_BLOCKED;
  }

  if(gsdi_status == GSDI_PERSO_CK_BLOCKED)
  {
    feature_mask = GSDI_PERSO_SEC_MASK_FEAT_PROP1;
    (void)gsdi_perso_sec_set_perso_event(feature_mask, gsdi_status, perso_event_p);
  }
  *ret_data_len_p = 2;
  *ret_data_p     = gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries;
#endif /*FEATURE_PERSO_SIM*/
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_CHANGE_PHONE_CODE

DESCRIPTION
  This function will be used to change the phone code.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_change_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{

  gsdi_returns_T           gsdi_status        = GSDI_SUCCESS;
  nv_stat_enum_type        write_status;
  nv_item_type             gsdi_nv_lock_code_buf; /* NV data buffer */

  MSG_HIGH("PERSO: gsdi_perso_security_change_phone_code",0,0,0);
  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
  *perso_event_p = GSDI_PERSO_NO_EVENT;

  gsdi_status = gsdi_perso_security_verify_phone_code(req_data_p,
                                                      encrypted_data_p,
                                                      sim_data_p,
                                                      ret_data_p,
                                                      ret_data_len_p,
                                                      perso_event_p);
  /*--------------------------------------------------------------------------
    The Phone code is the same as the one supplied by the client, update it
    with the new one.
    -------------------------------------------------------------------------*/
  if(gsdi_status == GSDI_SUCCESS)
  {

    memcpy((char*)gsdi_nv_lock_code_buf.lock_code.digits, req_data_p->new_phone_code.buffer_p, NV_LOCK_CODE_SIZE);

    write_status = gsdi_put_nv(NV_LOCK_CODE_I, &gsdi_nv_lock_code_buf);
    if ( write_status != NV_DONE_S )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not set NV Lock code",0,0,0);
      gsdi_status =  GSDI_ERROR;
    }
  }
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_UNBLOCK_PHONE_CODE

DESCRIPTION
  This function will be used to unblock the phone code

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_unblock_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{


  gsdi_returns_T           gsdi_status        = GSDI_SUCCESS;
#ifdef FEATURE_PERSO_SIM
  nv_stat_enum_type        read_ext_status;
  nv_stat_enum_type        read_ext_digits_status;
  nv_item_type             gsdi_nv_lock_code_ext_buf; /* NV data buffer */
  nv_item_type             gsdi_nv_lock_code_ext_digits_buf; /* NV data buffer */
  nv_stat_enum_type        write_status;
#endif /*FEATURE_PERSO_SIM*/

  MSG_HIGH("PERSO: gsdi_perso_security_unblock_phone_code",0,0,0);

  /* --------------------------------------------------------------------------
     Validate the parameters provided
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(encrypted_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
  *perso_event_p = GSDI_PERSO_NO_EVENT;
#ifdef FEATURE_PERSO_SIM
  if(req_data_p->master_key.len != NV_LOCK_CODE_SIZE*2)
  {
    MSG_ERROR("PERSO: Wrong master_key length 0x%x",
              req_data_p->master_key.len,0,0);
    return GSDI_PERSO_INVALID_CK;
  }
  /* --------------------------------------------------------------------------
    First find out out if we can make the comparison or not i.e. is the max
    number of counts reached?
    -------------------------------------------------------------------------*/
  read_ext_status = gsdi_get_nv(NV_LOCK_CODE_EXT_I,
                                &gsdi_nv_lock_code_ext_buf);
  read_ext_digits_status = gsdi_get_nv(NV_LOCK_CODE_EXT_DIGITS_I,
                                &gsdi_nv_lock_code_ext_digits_buf);

  if ( read_ext_status  == NV_NOTACTIVE_S ||
       read_ext_digits_status == NV_NOTACTIVE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NV Item not Active - SHould not HAPPEN!!!",0,0,0);
    return GSDI_ERROR;
  }
  else if(read_ext_status != NV_DONE_S ||
          read_ext_digits_status != NV_DONE_S)
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NV Lock Code Item could not be read",0,0,0);
    return GSDI_PERSO_INVALID_CK;
  }

  if(gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries != GSDI_PERSO_PHONE_CODE_EXHAUSTED)
  {
    MSG_HIGH("PERSO: Lock Code Not blocked",0,0,0);
    return GSDI_PERSO_INVALID_CK;
  }

  /* --------------------------------------------------------------------------
    Extract the Unblock code from NV and verify it against the code of the
  -------------------------------------------------------------------------*/

  if(!memcmp(req_data_p->master_key.buffer_p, (char*)gsdi_nv_lock_code_ext_digits_buf.lock_code_ext_digits, GSDI_PERSO_MAX_MASTER_PHONE_CODE_LEN))
  {
    gsdi_status = GSDI_SUCCESS;
      *perso_event_p = GSDI_PERSO_PROP1_UNBLOCKED;
  }
  else
  {
    gsdi_status = GSDI_PERSO_INVALID_CK;
  }

  if(gsdi_status == GSDI_SUCCESS)
  {
    /*Reset the counter*/
    gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries = GSDI_PERSO_MAX_CK_RETRIES;

    write_status = gsdi_put_nv(NV_LOCK_CODE_EXT_I, &gsdi_nv_lock_code_ext_buf);
    if ( write_status != NV_DONE_S )
    {
      MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not set NV Lock code",0,0,0);
      gsdi_status =  GSDI_ERROR;
    }
  }
#endif /*FEATURE_PERSO_SIM*/
  *ret_data_len_p = 0;
  *ret_data_p     = 0;
  return gsdi_status;

}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_SET_PHONE_CODE

DESCRIPTION
  This function will be used to set the phone code, this will write to the
  code only if it is not already active.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_security_set_phone_code(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
  gsdi_returns_T           gsdi_status        = GSDI_SUCCESS;
#ifdef FEATURE_PERSO_SIM
  nv_stat_enum_type        write_status;
  nv_stat_enum_type        read_status;
  nv_item_type             gsdi_nv_lock_code_buf; /* NV data buffer */
  nv_stat_enum_type        read_ext_status;
  nv_item_type             gsdi_nv_lock_code_ext_buf; /* NV data buffer */
#endif /*FEATURE_PERSO_SIM*/

  MSG_HIGH("PERSO: gsdi_perso_security_set_phone_code",0,0,0);

  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
 *perso_event_p = GSDI_PERSO_NO_EVENT;
#ifdef FEATURE_PERSO_SIM
  read_status = gsdi_get_nv(NV_LOCK_CODE_I,
                            &gsdi_nv_lock_code_buf);
  read_ext_status = gsdi_get_nv(NV_LOCK_CODE_EXT_I,
                            &gsdi_nv_lock_code_ext_buf);


  /* --------------------------------------------------------------------------
     Read Status should be not active
  ------------------------------------------------------------------------*/
  if ( read_status     != NV_NOTACTIVE_S ||
       read_ext_status != NV_NOTACTIVE_S )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: NV Item Active, cannot set it again!!",0,0,0);
    return GSDI_PERSO_INVALID_CK;
  }

  memcpy((char*)gsdi_nv_lock_code_buf.lock_code.digits, req_data_p->phone_code.buffer_p, NV_LOCK_CODE_SIZE);

  /*Reset the Counter as well*/
  gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries = GSDI_PERSO_MAX_CK_RETRIES;

  write_status = gsdi_put_nv(NV_LOCK_CODE_I, &gsdi_nv_lock_code_buf);
  if ( write_status != NV_DONE_S )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not set NV Lock code",0,0,0);
    gsdi_status =  GSDI_PERSO_INVALID_CK;
  }

  write_status = gsdi_put_nv(NV_LOCK_CODE_EXT_I, &gsdi_nv_lock_code_ext_buf);
  if ( write_status != NV_DONE_S )
  {
    MMGSDI_DEBUG_MSG_ERROR("PERSO: Could not update counter",0,0,0);
    gsdi_status =  GSDI_PERSO_INVALID_CK;
  }
#endif /*FEATURE_PERSO_SIM*/
  *ret_data_len_p = 0;
  *ret_data_p     = 0;

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PERSO_SECURITY_SET_PHONE_CODE

DESCRIPTION
  This function will be used to get the phone code status and number of
  retries.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
static gsdi_returns_T gsdi_perso_security_get_phone_code_status(
 const gsdi_perso_sec_client_req_type * req_data_p,
 gsdi_perso_sec_me_data_type          * encrypted_data_p,
 const gsdi_perso_sec_sim_data_type   * sim_data_p,
 uint8 *                                ret_data_p,
 int32*                                 ret_data_len_p,
 gsdi_perso_event_enum_type           * perso_event_p
)
{
#ifdef FEATURE_PERSO_SIM
  nv_stat_enum_type        read_status;
  nv_item_type             gsdi_nv_lock_code_buf;     /* NV data buffer */
  nv_stat_enum_type        read_ext_status;
  nv_item_type             gsdi_nv_lock_code_ext_buf; /* NV data buffer */
#endif /*FEATURE_PERSO_SIM*/

  MSG_HIGH("PERSO: gsdi_perso_security_get_phone_code_status",0,0,0);
  /* --------------------------------------------------------------------------
     Validate the input parmameters.
     ------------------------------------------------------------------------*/
  MMGSDI_RETURN_IF_NULL(req_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_p);
  MMGSDI_RETURN_IF_NULL(ret_data_len_p);
  MMGSDI_RETURN_IF_NULL(sim_data_p);
  MMGSDI_RETURN_IF_NULL(perso_event_p);

  encrypted_data_p->data_mod.update_fs = FALSE;
  *perso_event_p = GSDI_PERSO_NO_EVENT;
  *ret_data_len_p = 0;
  *ret_data_p     = 0;

#ifdef FEATURE_PERSO_SIM
  /* --------------------------------------------------------------------------
     Retrieve the Lock Code information
     ------------------------------------------------------------------------*/
  read_status     = gsdi_get_nv(NV_LOCK_CODE_I,
                                &gsdi_nv_lock_code_buf);
  read_ext_status = gsdi_get_nv(NV_LOCK_CODE_EXT_I,
                                &gsdi_nv_lock_code_ext_buf);

  if ( read_status     == NV_NOTACTIVE_S ||
       read_ext_status == NV_NOTACTIVE_S )
  {
    *ret_data_p     = (uint8)GSDI_PERSO_ENG_NOT_INIT_STATE;
    *ret_data_len_p = (uint8)1;
    return GSDI_SUCCESS;
  }

  /* --------------------------------------------------------------------------
     Lock Code Status information is active. Now determine whether it is
     enabled or disabled
     ------------------------------------------------------------------------*/
  *ret_data_p     = (uint8)GSDI_PERSO_ENG_ACTIVE_STATE;
  *(ret_data_p+1) = gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries;
  *ret_data_len_p = (uint8)2;

  if(gsdi_nv_lock_code_ext_buf.lock_code_ext.numLockRetries == 0)
  {
   (void)gsdi_perso_sec_set_perso_event(GSDI_PERSO_SEC_MASK_FEAT_PROP1,
                                        GSDI_PERSO_CK_BLOCKED,
                                        perso_event_p);
  }
#endif /*FEATURE_PERSO_SIM*/
  return GSDI_SUCCESS;
}
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
