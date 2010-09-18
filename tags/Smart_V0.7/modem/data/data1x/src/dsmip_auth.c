/*===========================================================================
                                      
                          D S M I P _ A U T H . C

DESCRIPTION

 The Data Services Mobile IP authentication lookup table implementation.
 Contains API to perform lookups on tables matching security parameter 
 indices and authentication algorithms as required by RFC 2002.

EXTERNALIZED FUNCTIONS

  mip_auth_lookup_alg()
    Returns a function pointer to the authentication algorithm associated
    with a specified security parameter index (SPI).

  mip_auth_lookup_spi()
    Returns the security parameter index (SPI) associated with the specified 
    authentication algorithm function.

  mip_auth_calc_md5_prefix_plus_suffix
    Generates the MD5 "postfix + suffix" result for the provided data 
    (ref RFC2002 S 3.5.1).

  mip_auth_calc_chap
    Generates the CHAP authenticator for the provided data (ref ?).

  mip_auth_retrieve_ss_data
    Determine which shared secrets to use for this MIP registration,
    and retrieve them wherever they are stored (if necesaary)

Copyright (c) 2000-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_auth.c_v   1.9   16 Nov 2002 00:30:40   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_auth.c#2 $ $DateTime: 2007/12/10 21:46:00 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/21/07 sq/mga/yz Merged changes to support FEATURE_UIM_SUPPORT_3GPD.
11/10/06    an     Fixed compilation errors for 6020
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
07/18/02    jay    If DMU is active, use the temporary key values from 
                   DMU session info for shared secrets.
05/07/02    jd     Created mip_auth_retrieve_ss_data to centralized logic 
                   to determining password to be used for MIP registration
10/30/01    jd     Include MSB of FAC in lower-order 237 byte of FAC portion
                   of MN-AAA hash calculation.
10/08/01    jd     Calculate offset correctly for challenge > 237 bytes. 
07/17/01    jd     Authentication functions return 16-bit (was 32) length.
07/04/01    jd     Code cleanup and naming made consistent with coding std
06/27/01    mvl    Changed the message level for these routines to LOW
03/05/01    jd     Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "dsmip_auth.h"
#ifdef __unix
#error code not present
#endif
#include "md5.h"
#include "msg.h"
#include "assert.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#include "dsmip_regsm.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#ifdef FEATURE_DATA_MM
#include "dsat707mip.h"
#include "ps_utils.h"
#include "ps_mip_compat.h"
#else
#include "dsatcop.h"
#endif /* FEATURE_DATA_MM */


#if defined(FEATURE_UIM_SUPPORT_3GPD) || !defined(FEATURE_DATA_MM)
#include "psi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_DATA_MM */

#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "uim.h"
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */



/*===========================================================================

                             INTERNAL DATA

===========================================================================*/
#ifdef FEATURE_UIM_SUPPORT_3GPD
/*---------------------------------------------------------------------------
  This variables are used to hold the status and data set in a uim 
  callback function.
---------------------------------------------------------------------------*/
LOCAL byte mip_uim_response[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH];
LOCAL uim_rpt_status mip_uim_report_status;
#endif /* FEATURE_UIM_SUPPORT_3GPD */



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION MIP_AUTH_LOOKUP_ALG

DESCRIPTION
  This function searches an authorisation lookup table for the specified 
  key (SPI) and returns the associated handler if found.

PARAMETERS
  table - pointer to the appropriate authentication lookup table 
  key   - key (SPI) to look up

RETURN VALUE
  Pointer to the authentication function corresponding to 'key', if found.
  NULL if not found.

DEPENDENCIES
  The last entry in 'table' must have the handler field = NULL.  
  This delineates the bound of the table.

SIDE EFFECTS
  None.

===========================================================================*/
void * mip_auth_lookup_alg
(
  mip_auth_lookup_table *  table,
  uint32                   key
)
{
  while (table->handler != NULL)
  {
    if (table->key == key)
    {
      break;  
    }
    table++;
  }

  return ((void *) table->handler);

} /* mip_auth_lookup_alg() */


/*===========================================================================

FUNCTION MIP_AUTH_LOOKUP_SPI

DESCRIPTION
  This function searches an authorisation lookup table for the specified 
  handler (function) and returns the associated security parameter index,
  if found.

PARAMETERS
  table - pointer to the appropriate authentication lookup table 
  handler - pointer to the authentication function to look up

RETURN VALUE
  key   - key (SPI) corresponding to 'handler', if found.
          set to SPI of last entry in table if not found.

DEPENDENCIES
  The last entry in 'table' must have the handler field = NULL.  
  This delineates the bound of the table.  the SPI should be configured
  to not fool the software or message recipients.

SIDE EFFECTS
  None.

===========================================================================*/

uint32 mip_auth_lookup_spi
(
  mip_auth_lookup_table *  table,
  uint16                   (* handler)()
)
{
  while (table->handler != NULL)
  {
    if (table->handler == handler)
    {
      break;
    }
    table++;
  }

  return (table->key);

} /* mip_auth_lookup_spi() */


#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================
FUNCTION MIP_UIM_COMMAND_REPORT()

DESCRIPTION
  Sets the signal to indicate that command has completed, and copies the
  report status into a variable local to this file.  This callback is
  used when no data is expected back from the command response.  
  This callback is called from the UIM context
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
    Sets the mip_uim_report_status variable
    Sets the PS_UIM_CMD_SIGNAL
===========================================================================*/
static void mip_uim_command_report
(
  uim_rpt_type *report
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Copy the report status (Pass or Fail) from the report to a variable
    local to the file
  -------------------------------------------------------------------------*/

  mip_uim_report_status = report->rpt_status;
  if(mip_uim_report_status == UIM_PASS)
  {
    MSG_MED("UIM SUCCESS", 0, 0, 0);
  }
  else
  {
    MSG_HIGH("UIM FAILURE", 0, 0, 0);
  }
  /*-------------------------------------------------------------------------
    This signal is set to indicate that the UIM has finished processing the
    command, and that the suspended PS task may resume
  -------------------------------------------------------------------------*/
  (void) rex_set_sigs( &ps_tcb, 1 << PS_UIM_CMD_SIGNAL );

} /* calculate_mip_uim_command_report() */


/*===========================================================================
FUNCTION MIP_UIM_COMMAND_REPORT_WITH_DATA()

DESCRIPTION
  Sets the signal to indicate that command has completed, copies the
  report status into a variable local to this file, and copies response data
  from the report type to a variable.  This callback is
  used when data is expected back from the command response.
  This callback is called from the UIM context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies mip_uim_report_status, and mip_uim_response
  Sets the PS_UIM_CMD_SIGNAL
===========================================================================*/
static void mip_uim_command_report_with_data
(
  uim_rpt_type *report
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Copy the report status (Pass or Fail) from the report to a variable
    local to the file .  If the report passes, copy the report data to
    a variable local to the file
  -------------------------------------------------------------------------*/
  mip_uim_report_status = report->rpt_status;
  if(mip_uim_report_status == UIM_PASS)
  {
    memcpy(mip_uim_response, report->rpt.compute_ip_auth.data,
            UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
    MSG_MED("UIM SUCCESS", 0, 0, 0);
  }
  else
  {
    MSG_HIGH("UIM-CHAP FAILURE", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    This signal is set to indicate that the UIM has finished processing the
    command, and that the suspended PS task may resume
  -------------------------------------------------------------------------*/
  (void) rex_set_sigs( &ps_tcb, 1 << PS_UIM_CMD_SIGNAL );

} /* calculate_mip_uim_command_report_with_data() */
#endif /* FEATURE_UIM_SUPPORT_3GPD */


/*===========================================================================

FUNCTION MIP_AUTH_CALC_MD5_PREFIX_PLUS_SUFFIX

DESCRIPTION

  This function calculates the "MD5 prefix+suffix" authenticator value
  on the data as follows:

    -  the shared secret provided
    -  the data provided
    -  the shared secret again.

  NOTE:  The caller should anticipate the length of the authenticator it 
         expects returned and make sufficient memory available at the 
         address pointed to by the authenticator parameter (see below).

DEPENDENCIES
  None

PARAMETERS
  data          - ptr to the data for which the authenticator is calculated
  data_len      - the length of 'data'
  secret        - pointer to the shared secret value
  secret_len    - the length of 'secret'
  authenticator - the calculated authenticator is returned via this pointer

RETURN VALUE
  Returns the length of the generated authenticator (in bytes)

SIDE EFFECTS
  None

===========================================================================*/
uint16 mip_auth_calc_md5_prefix_plus_suffix
(
  void *   data,                   /* start of data to authenticate        */
  uint32   data_len,               /* length of data                       */
  void *   secret,                 /* shared secret used for calculation   */
  uint32   secret_len,             /* length of shared secret              */
  void *   authenticator,           /* storage for calculated authenticator */
  uint32   profile_index           /* nai entry index on RUIM to use       */
)
{
  MD5_CTX   auth_ctx;                            /* authentication context */
#ifdef FEATURE_UIM_SUPPORT_3GPD
  uim_cmd_type *mip_uim_cmd;
#endif /* FEATURE_UIM_SUPPORT_3GPD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Calculate the authenticator value for the provided data.
    Check whether authentication should be done in software, or on the card
  -------------------------------------------------------------------------*/

#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  {
  MSG_LOW ("MD5 << init ", 0,0,0);
  MD5Init (&auth_ctx);

  MSG_LOW ("MD5 << secret (%d bytes) ", secret_len,0,0);
  MD5Update (&auth_ctx, (unsigned char *) secret, secret_len);

  MSG_LOW ("MD5 << data (%d bytes) ", data_len,0,0);
  MD5Update (&auth_ctx, (unsigned char *) data, data_len);

  MSG_LOW ("MD5 << secret (%d bytes) ", secret_len,0,0);
  MD5Update (&auth_ctx, (unsigned char *) secret, secret_len);

  MSG_LOW ("MD5 << final ", 0,0,0);
  MD5Final ((unsigned char *) authenticator, &auth_ctx);
  }
#ifdef FEATURE_UIM_SUPPORT_3GPD
  else
  {
    mip_uim_cmd = (uim_cmd_type*)q_get(&uim_free_q);
    if(mip_uim_cmd == NULL)
    {
      return NULL;
    }

    /*-----------------------------------------------------------------------
      Build the UIM command
    -----------------------------------------------------------------------*/
    mip_uim_cmd->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MN_HA_AUTH;
    mip_uim_cmd->compute_ip_auth.cmpt_ip_data.mn_ha_data.nai_entry_index =
      profile_index;
    mip_uim_cmd->
      compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data_length =
        data_len;
    memcpy(mip_uim_cmd->
             compute_ip_auth.cmpt_ip_data.mn_ha_data.registration_data,
           data,
           data_len);

    mip_uim_cmd->hdr.command = UIM_COMPUTE_IP_AUTH_F;
    mip_uim_cmd->hdr.cmd_hdr.task_ptr = NULL;
    mip_uim_cmd->hdr.cmd_hdr.sigs = 0;
    mip_uim_cmd->hdr.cmd_hdr.done_q_ptr = &uim_free_q;
    mip_uim_cmd->hdr.rpt_function = mip_uim_command_report_with_data;
    mip_uim_cmd->hdr.protocol = UIM_CDMA;
    mip_uim_cmd->hdr.options = UIM_OPTION_ALWAYS_RPT;
    mip_uim_cmd->hdr.slot = UIM_SLOT_AUTOMATIC;

    /*-----------------------------------------------------------------------
      Send the command to the R-UIM:  mn_ha authenticator
      Clear the "command done signal"
      Send the command
      Wait for the command to be done
    -----------------------------------------------------------------------*/
    PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

    uim_cmd( mip_uim_cmd );
    MSG_MED("Blocking PS Task on UIM cmd signal - MN-HA",0,0,0);
    psi_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL);
    MSG_MED("Unblocking PS Task on UIM cmd signal- MN-HA",0,0,0);
    if(mip_uim_report_status == UIM_PASS)
    {
      memcpy(authenticator, 
             mip_uim_response, 
             UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
    }
    else
    {
      MSG_HIGH("UIM-MIP-MN-HA FAILURE", 0, 0, 0);
      return NULL;
    }
  }
#endif /* FEATURE_UIM_SUPPORT_3GPD */



  /*-------------------------------------------------------------------------
    Return the authenticator length
  -------------------------------------------------------------------------*/
  return (MIP_AUTH_MD5_LEN);

} /* mip_auth_calc_md5_prefix_plus_suffix */



/*===========================================================================

FUNCTION MIP_AUTH_CALC_CHAP

DESCRIPTION

  This function calculates the "chap" authenticator value
  on the data as follows:

  NOTE:  The caller should anticipate the length of the authenticator it 
         expects returned and make sufficient memory available at the 
         address pointed to by the authenticator parameter (see below).

DEPENDENCIES
  None

PARAMETERS
  data          - ptr to the data for which the authenticator is calculated
  data_len      - the length of 'data'
  secret        - pointer to the shared secret value
  secret_len    - the length of 'secret'
  authenticator - the calculated authenticator is returned via this pointer

RETURN VALUE
  Returns the length of the generated authenticator (in bytes)

SIDE EFFECTS
  None

===========================================================================*/
uint16 mip_auth_calc_chap
(
  void *   data,                   /* start of data to authenticate        */
  uint32   data_len,               /* length of data                       */
  void *   secret,                 /* shared secret used for calculation   */
  uint32   secret_len,             /* length of shared secret              */
  void *   challenge,              /* FA challenge value                   */
  uint32   challenge_len,          /* length of FA challenge               */
  void *   authenticator,           /* storage for calculated authenticator */
uint32   profile_index           /* nai entry index on RUIM to use       */
)
{
  MD5_CTX   auth_ctx;              /* authentication context               */
  char      imdt_val[16];          /* intermediate MD5 hash value          */
  uint32    offset;                /* start of 237 LSbytes of challenge    */

#ifdef FEATURE_UIM_SUPPORT_3GPD
  uim_cmd_type *mip_uim_cmd;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(UIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  {

  /*-------------------------------------------------------------------------
    Calculate the byte offset into challenge where the 237 
    least-significant bytes begin
  -------------------------------------------------------------------------*/
  MSG_LOW ("Challenge len = %d", challenge_len,0,0);

  if (challenge_len > 237)
  {
    offset = challenge_len - 237;
  }
  else 
  {
    offset = 0;
  }

  MSG_LOW ("Offset = %d", offset,0,0);
  /*-------------------------------------------------------------------------
    Calculate the intermediate MD5 value
  -------------------------------------------------------------------------*/
  MSG_LOW ("MD5 << init ", 0,0,0);
  MD5Init( &auth_ctx );

  MSG_LOW ("MD5 << data (%d bytes) ", data_len,0,0);
  MD5Update( &auth_ctx, (unsigned char *) data,   data_len);

  MSG_LOW ("MD5 << final ", 0,0,0);
  MD5Final ((unsigned char *) imdt_val, &auth_ctx);

  /*-------------------------------------------------------------------------
    Calculate the authenticator value to be returned
  -------------------------------------------------------------------------*/
  MSG_LOW ("MD5 << init ", 0,0,0);
  MD5Init( &auth_ctx );

  if (challenge_len != 0)
  {
    MSG_LOW ("MD5 << MSB(challenge) (1 byte) ",0,0,0);
    MD5Update (&auth_ctx, (unsigned char *) challenge, 1);
  }

  MSG_LOW ("MD5 << secret (%d bytes) ", secret_len,0,0);
  MD5Update( &auth_ctx, (unsigned char *) secret, secret_len);

  MSG_LOW ("MD5 << intermediate (16 bytes) ",0,0,0);
  MD5Update( &auth_ctx, (unsigned char *) imdt_val, 16);

  if (challenge_len != 0)
  {
    MSG_LOW ("MD5 << challenge(LSB's) (%d bytes)", challenge_len - offset,0,0);
    MD5Update( &auth_ctx, 
               &(((unsigned char *) challenge)[offset]), 
               challenge_len - offset);
  }

  MSG_LOW ("MD5 << final ", 0,0,0);
  MD5Final ((unsigned char *) authenticator, &auth_ctx);
}
#ifdef FEATURE_UIM_SUPPORT_3GPD
  else
  {
    mip_uim_cmd = (uim_cmd_type*)q_get(&uim_free_q);
    if(mip_uim_cmd == NULL)
    {
      return NULL;
    }

    /*-----------------------------------------------------------------------
      Build the UIM command

      Send the MIP-RRQ Command to calculate the intermediate data
      This data is stored on the card and not returned.  It is then used
      by the card for the last command
    -----------------------------------------------------------------------*/
    mip_uim_cmd->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MIP_RRQ;
    mip_uim_cmd->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data_length
      = data_len;
    memcpy(mip_uim_cmd->compute_ip_auth.cmpt_ip_data.mip_rrq_data.rrq_data,
           data,
           data_len);
    mip_uim_cmd->hdr.command = UIM_COMPUTE_IP_AUTH_F;
    mip_uim_cmd->hdr.cmd_hdr.task_ptr = NULL;
    mip_uim_cmd->hdr.cmd_hdr.sigs = 0;
    mip_uim_cmd->hdr.cmd_hdr.done_q_ptr = &uim_free_q;
    mip_uim_cmd->hdr.rpt_function = mip_uim_command_report;
    mip_uim_cmd->hdr.protocol = UIM_CDMA;
    mip_uim_cmd->hdr.options = UIM_OPTION_ALWAYS_RPT;
    mip_uim_cmd->hdr.slot = UIM_SLOT_AUTOMATIC;

    /*-----------------------------------------------------------------------
      Send the command to the R-UIM:
      Clear the "command done signal"
      Send the command
      Wait for the command to be done
    -----------------------------------------------------------------------*/
    PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

    uim_cmd( mip_uim_cmd );
    MSG_MED("BLOCKING PS TASK ON UIM CMD SIGNAL - MIP-RRQ HASH",0,0,0);
    psi_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL);
    MSG_MED("UNBLOCKING PS TASK ON UIM CMD SIGNAL - MIP-RRQ-HASH",0,0,0);
    if(mip_uim_report_status == UIM_PASS)
    {
      MSG_HIGH("UIM-MIP-RRQ-HASH Success", 0, 0, 0);
    }
    else
    {
      MSG_HIGH("UIM-MIP-RRQ-HASH FAILURE", 0, 0, 0);
      return NULL;
    }

  /*-----------------------------------------------------------------------
      Build the UIM command

      Send the Last Command - MN-AAA Authenticator
    -----------------------------------------------------------------------*/
    mip_uim_cmd->compute_ip_auth.cmpt_ip_operation = UIM_CMPT_IP_MN_AAA_AUTH;
    mip_uim_cmd->compute_ip_auth.cmpt_ip_data.mn_aaa_data.nai_entry_index
                                                            = profile_index;
    mip_uim_cmd->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge_length
                                                            = challenge_len;
    memcpy(mip_uim_cmd->compute_ip_auth.cmpt_ip_data.mn_aaa_data.challenge,
                                    challenge, challenge_len);
    mip_uim_cmd->hdr.command = UIM_COMPUTE_IP_AUTH_F;
    mip_uim_cmd->hdr.cmd_hdr.task_ptr = NULL;
    mip_uim_cmd->hdr.cmd_hdr.sigs = 0;
    mip_uim_cmd->hdr.cmd_hdr.done_q_ptr = NULL;
    mip_uim_cmd->hdr.rpt_function = mip_uim_command_report_with_data;
    mip_uim_cmd->hdr.protocol = UIM_CDMA;
    mip_uim_cmd->hdr.options = UIM_OPTION_ALWAYS_RPT;
    mip_uim_cmd->hdr.slot = UIM_SLOT_AUTOMATIC;

  /*-----------------------------------------------------------------------
      Send the command to the R-UIM:
      Clear the "command done signal"
      Send the command
      Wait for the command to be done
    -----------------------------------------------------------------------*/
    PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

    uim_cmd( mip_uim_cmd );
    MSG_MED("BLOCKING PS TASK ON UIM CMD SIGNAL - MN-AAA",0,0,0);
    psi_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL);
    MSG_MED("UNBLOCKING PS TASK ON UIM CMD SIGNAL - MN-AAA",0,0,0);
    if(mip_uim_report_status == UIM_PASS)
    {
      memcpy(authenticator, 
             mip_uim_response, 
             UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
    }
    else
    {
      MSG_HIGH("UIM-MIP-MN-AAA FAILURE", 0, 0, 0);
      return NULL;
    }
  }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  /*-------------------------------------------------------------------------
    Return the authenticator length
  -----------------------------------------------------------------------*/
  return (MIP_AUTH_MD5_LEN);

} /* mip_auth_calc_chap() */


/*===========================================================================

FUNCTION MIP_AUTH_RETRIEVE_SS_DATA()
 
DESCRIPTION
  Determines (and retrieves if necessary) the shared secret data to be
  used when composing the Mobile IP registration request.

PARAMETERS
  mn_ha_ss      - where to store MN-HA shared secret pointer
  mn_ha_ss_len  - where to store length for above
  mn_aaa_ss     - where to store MN-HA shared secret pointer
  mn_aaa_ss_len - where to store length for above

RETURN VALUE
  mn_ha_ss      - set to point to MN-HA shared secret
  mn_ha_ss_len  - set to length of above
  mn_aaa_ss     - set to point to MN-AAA shared secret
  mn_aaa_ss_len - set to length of above

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies global ps_nv_item 

===========================================================================*/
void mip_auth_retrieve_ss_data
(
  mip_session_info_type * session,
  byte ** mn_ha_ss, 
  byte *  mn_ha_ss_len, 
  byte ** mn_aaa_ss, 
  byte *  mn_aaa_ss_len
)
{
  nv_stat_enum_type  nv_status;            /* return val for ps_nv_read    */

#ifdef FEATURE_DS_MOBILE_IP_DMU
  byte               dmu_ix;
#endif /* FEATURE_DS_MOBILE_IP_DMU */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Determine which shared secret data to use for this registration

    In order of priority,
    1. If the UI has registered MIP session info and/or password callbacks, 
       they were called in meta_sm_config_session.  Use the values that were
       written to the shared secret profile by this callback.
    2. If using shadow NV, use the shadow variables.
    3. If in DMU, use the temporary keys stored in mip_dmu_key_data
    4. Otherwise, use the keys stored in the MIP session in NV.
  -------------------------------------------------------------------------*/
  ASSERT (mn_ha_ss != NULL);
  ASSERT (mn_ha_ss_len != NULL);
  ASSERT (mn_aaa_ss != NULL);
  ASSERT (mn_aaa_ss_len != NULL);

  /*-------------------------------------------------------------------------
    If UI SS callback is set, use these values
  -------------------------------------------------------------------------*/ 
  if (mip_get_pwd_cb != NULL)
  {
    /*-----------------------------------------------------------------------
      Use the info entered via the UI.  Note that this information is not as 
      secure as the NV info - it remains in RAM while the call is up, since 
      the information is required for re-registration and RRQ retries.
    -----------------------------------------------------------------------*/
    *mn_ha_ss      = mip_pwd_info.mn_ha_ss;
    *mn_aaa_ss     = mip_pwd_info.mn_aaa_ss;
    *mn_ha_ss_len  = mip_pwd_info.mn_ha_ss_len;
    *mn_aaa_ss_len = mip_pwd_info.mn_aaa_ss_len;
    return;
  }

  /*-------------------------------------------------------------------------
    If shadow NV is set, use these values;
    Otherwise, use NULL passwords unless found elsewhere 
  -------------------------------------------------------------------------*/ 
  if (shadow_nv.mn_aaa_ss != NULL)
  {
    MSG_MED ("Using shadow NV for AAA-SS",0,0,0);
    *mn_aaa_ss     = (byte *) (shadow_nv.mn_aaa_ss);
    *mn_aaa_ss_len = *shadow_nv.mn_aaa_ss_len;
  }
  else
  {
    *mn_aaa_ss     = NULL;
    *mn_aaa_ss_len = 0;
  }

  if (shadow_nv.mn_ha_ss != NULL)
  {
    MSG_MED ("Using shadow NV item for HA-SS",0,0,0);
    *mn_ha_ss     = (byte *) (shadow_nv.mn_ha_ss);
    *mn_ha_ss_len = *shadow_nv.mn_ha_ss_len;
  }
  else
  {
    *mn_ha_ss     = NULL;
    *mn_ha_ss_len = 0;
  }

#ifdef FEATURE_DS_MOBILE_IP_DMU
  /*-------------------------------------------------------------------------
    If DMU is active, use the temporary key values
  -------------------------------------------------------------------------*/  
  if (session->dmu)
  {
    /*-----------------------------------------------------------------------
      Note, if shadow NV values are active, DMU should have failed and the
      call dropped before arriving here.
    -----------------------------------------------------------------------*/  
    ASSERT (*mn_ha_ss == NULL);
    ASSERT (*mn_aaa_ss == NULL);

    dmu_ix = MIP_SES_PTR_2_INDEX (session);

    *mn_ha_ss      = mip_dmu_info[dmu_ix].key_data.mn_ha_key;
    *mn_ha_ss_len  = MIP_DMU_MN_HA_KEY_LEN;
    *mn_aaa_ss     = mip_dmu_info[dmu_ix].key_data.mn_aaa_key;
    *mn_aaa_ss_len = MIP_DMU_MN_AAA_KEY_LEN;
    return;
  }
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  /*-------------------------------------------------------------------------
    If passwords have not been obtained by other means, 
    get them from NV now. 
  -------------------------------------------------------------------------*/  
  if (!(*mn_aaa_ss) || !(*mn_ha_ss))
  {
    /*-----------------------------------------------------------------------
      Check if the profile is enabled
    -----------------------------------------------------------------------*/  
    nv_status = psi_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ps_nv_item);
    if( nv_status != NV_DONE_S )
    {
      MSG_ERROR ("MIP profile enable flag unreadable",
                 session->profile_index, 0, 0);
      return;
    }

    /*-----------------------------------------------------------------------
      If profile is disabled, return and use NULL password
    -----------------------------------------------------------------------*/  
    if (!ps_nv_item.ds_mip_enable_prof[session->profile_index])
    {
      MSG_ERROR ("Profile %d disabled, using NULL p/w",0,0,0);
      return;
    }
 
    /*-----------------------------------------------------------------------
      Read the shared secret data from NV.  

      JD - looks like a bug in nvmio.c:5176,5177, size is too big 
         (incl. index) (ref nv.h, nv_ds_mip_gen_user_prof_type)
    -----------------------------------------------------------------------*/
    ps_nv_item.ds_mip_ss_user_prof.index = session->profile_index;
    nv_status = psi_get_nv_item(NV_DS_MIP_SS_USER_PROF_I, &ps_nv_item);
    if( nv_status != NV_DONE_S)
    {
      /*---------------------------------------------------------------------
        If NV is not written, we send 0 len shared secrets.
      ---------------------------------------------------------------------*/
      MSG_ERROR ("SS profile %d unreadable, using NULL p/w",
                 session->profile_index, 0, 0);
      return;
    }

    /*-----------------------------------------------------------------------
      If we're not using shadow NV for the MN-AAA SS, use the NV value
    -----------------------------------------------------------------------*/
    if (!(*mn_aaa_ss))
    {
      *mn_aaa_ss = (byte *) ps_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret;
      *mn_aaa_ss_len = ps_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length;
    }

    /*-----------------------------------------------------------------------
      If we're not using shadow NV for the MN-HA SS, use the NV value
    -----------------------------------------------------------------------*/
    if (!(*mn_ha_ss))
    {
      *mn_ha_ss = (byte *) ps_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret;
      *mn_ha_ss_len = ps_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length;
    }
  } /* endif (unresolved shared secret) */
} /* mip_auth_retrieve_ss_data */


#endif  /* FEATURE_DS_MOBILE_IP */

