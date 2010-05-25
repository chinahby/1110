/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

       C O M M O N   P C N   H A N D S E T   S P E C I F I C A T I O N S

                   ( C P H S )   P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the Common PCN Handset Specifications (CPHS)
  procedures.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_cphs.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   js      Added prototypes for static functions
02/19/09   ssr     Fixed parsing of CSP data
11/14/08   ssr     Fixed parsing of value added service group
04/29/08   sun     Added support for invalid 3G configuration
02/21/08   sun     Added new parameters for individual reads
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
07/07/06   sun     Removed Reference to mc.h
05/11/05   sst     Lint fixes
11/15/04   jar     Code clean up to use Debug MACROS
09/23/04   jar     Added retrieval of CPHS Information and parsing of data.
09/07/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rex.h"
#include "task.h"

/* System and Interfaces */
#include "msg.h"
#include "err.h"
#include "msg.h"
#include "err.h"
#include "dsm.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"

#ifdef FEATURE_MMGSDI_CPHS
#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */
#include "intconv.h"
/* GSDI */
#include "gsdi_exp.h"
#include "gsdi.h"
#include "gsdi_cphs.h"
/* MMGSDI */
#include "mmgsdiutil.h"

/* UIM */
#include "uim.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static gsdi_returns_T gsdi_cphs_parse_cphsi_data(
  int32  data_len,
  uint8* data_ext_p
);

static gsdi_returns_T gsdi_cphs_parse_csp_data(
  int32  data_len,
  uint8* data_ext_p
);

/*===========================================================================
FUNCTION GSDI_CPHS_PARSE_CPHSI_DATA

DESCRIPTION
  This function is a local function used to parse the CPHSI Data and store
  the results in gsdi_data (The global Data Structure).

  The information stored in the Global Data Structure will be utilized
  by the CPHS GET INFO API.

INPUT:
  uint8 *  data_ext_p:     The pointer to the buffer used to store the
                           read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_cphs_parse_cphsi_data(
  int32  data_len,
  uint8* data_ext_p
)
{
  /* --------------------------------------------------------------------------
    Perform Basic Parameter Checking
    data_len:   Can not exceed GSDI_MAX_DATA_BLOCK_LEN
    data_exp_p: Can not be NULL
    -------------------------------------------------------------------------*/
  if ( data_len > GSDI_MAX_DATA_BLOCK_LEN )
  {
    MSG_HIGH("Reset Data Len from 0x%x to 0x%x",data_len,GSDI_MAX_DATA_BLOCK_LEN,0);
    data_len = GSDI_MAX_DATA_BLOCK_LEN;
  }

  if ( data_ext_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL POINTER BUFFER",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     CPHS Information was read which indicates that CPHS is supported to some
     degree.  It is up to the (EF-CSP) Customer Service Profile to indicate
     exactly which services are supported.
     Logically, as far as GSDI is concerned, reading CPHS Information is an
     indication that the card is CPHS Capable.
     ------------------------------------------------------------------------*/
  gsdi_data.cphs_info.cphs_capable = TRUE;

  /* --------------------------------------------------------------------------
     Store the CPHS Phase Information
     ------------------------------------------------------------------------*/
  gsdi_data.cphs_info.services.phase = data_ext_p[GSDI_CPHS_INFO_PHASE_OFFSET];

  /* --------------------------------------------------------------------------
     Now parse the CPHS Information Data to determine which "Services" are
     allocated and activated
     ------------------------------------------------------------------------*/
  gsdi_data.cphs_info.services.csp =
    (gsdi_current_service_enum_type)(data_ext_p[GSDI_CPHS_INFO_SERVICES_OFFSET_B2] & GSDI_CPHS_INFO_CSP_MASK);
  gsdi_data.cphs_info.services.sst =
    (gsdi_current_service_enum_type)((data_ext_p[GSDI_CPHS_INFO_SERVICES_OFFSET_B2] & GSDI_CPHS_INFO_SST_MASK) >> 2);
  gsdi_data.cphs_info.services.mbn =
    (gsdi_current_service_enum_type)((data_ext_p[GSDI_CPHS_INFO_SERVICES_OFFSET_B2] & GSDI_CPHS_INFO_MBN_MASK) >> 4);
  gsdi_data.cphs_info.services.ops =
    (gsdi_current_service_enum_type)((data_ext_p[GSDI_CPHS_INFO_SERVICES_OFFSET_B2] & GSDI_CPHS_INFO_OP_NAME_MASK) >> 6);
  gsdi_data.cphs_info.services.ifn =
    (gsdi_current_service_enum_type)(data_ext_p[GSDI_CPHS_INFO_SERVICES_OFFSET_B3] & GSDI_CPHS_INFO_INFO_NUM_MASK);

  return GSDI_SUCCESS;
}


/*===========================================================================
FUNCTION GSDI_CPHS_PARSE_CSP_DATA

DESCRIPTION
  This function is a local function used to parse the CSP Data and store
  the results in gsdi_data (The global Data Structure).

  The information stored in the Global Data Structure will be utilized
  by the CPHS GET INFO API.

INPUT:
  uint8 *  data_ext_p:     The pointer to the buffer used to store the
                           read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
static gsdi_returns_T gsdi_cphs_parse_csp_data(
  int32  data_len,
  uint8* data_ext_p
)
{
  uint32 iteration    = 0x0000;
  uint32 svcs_index   = 0x0000;

  /* --------------------------------------------------------------------------
    Perform Basic Parameter Checking
    data_len:   Can not exceed GSDI_MAX_DATA_BLOCK_LEN
    data_exp_p: Can not be NULL
    -------------------------------------------------------------------------*/
  if ( data_len > GSDI_MAX_DATA_BLOCK_LEN )
  {
    MSG_HIGH("Reset Data Len from 0x%x to 0x%x",data_len,GSDI_MAX_DATA_BLOCK_LEN,0);
    data_len = GSDI_MAX_DATA_BLOCK_LEN;
  }

  if ( data_ext_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL DATA BUFFER",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* -------------------------------------------------------------------------
   In case if Mandatory Data Objects are not present in CSP File then 
   we will pick the default value.
   For example default value of PLMN mode is ON, So in case where 
   the PLMN bit is not present in file then we will allow to show manual option 
  ----------------------------------------------------------------------------*/

  /* Default value is ON for Value Added Service group */
  gsdi_data.cphs_info.csp.value_added_PLMN_MODE    = GSDI_CSP_IND_ON;
  gsdi_data.cphs_info.csp.value_added_VPS          = GSDI_CSP_IND_ON;
  gsdi_data.cphs_info.csp.value_added_SM_MO_PAGING = GSDI_CSP_IND_ON;
  gsdi_data.cphs_info.csp.value_added_SM_MO_EMAIL  = GSDI_CSP_IND_ON;
  gsdi_data.cphs_info.csp.value_added_FAX          = GSDI_CSP_IND_ON;
  gsdi_data.cphs_info.csp.value_added_DATA         = GSDI_CSP_IND_ON;
  gsdi_data.cphs_info.csp.valued_added_Language    = GSDI_CSP_IND_ON;

  /* --------------------------------------------------------------------------
     Iterate through the Service Group offsets, and set the Global
     data Structure Information appropriately
     ------------------------------------------------------------------------*/
  while ( (iteration + 1) < int32touint32(data_len))
  {
    /* Service Group offset and its data should be there.*/
    svcs_index = iteration + 1;
    switch ( data_ext_p[iteration] )
    {
    case GSDI_CPHS_CSP_SVC_GROUP_1:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CFU_MASK )
        gsdi_data.cphs_info.csp.call_offering_CFU = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_offering_CFU = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CFB_MASK )
        gsdi_data.cphs_info.csp.call_offering_CFB = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_offering_CFB = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CFNRy_MASK )
        gsdi_data.cphs_info.csp.call_offering_CFNRy = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_offering_CFNRy = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CFNRc_MASK)
        gsdi_data.cphs_info.csp.call_offering_CFNRc = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_offering_CFNRc = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CT_MASK)
        gsdi_data.cphs_info.csp.call_offering_CT = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_offering_CT = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_2:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_BOAC_MASK )
        gsdi_data.cphs_info.csp.call_restrict_BOAC = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_restrict_BOAC = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_BOIC_MASK )
        gsdi_data.cphs_info.csp.call_restrict_BOIC = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_restrict_BOIC = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_BOICexHC_MASK )
        gsdi_data.cphs_info.csp.call_restrict_BOIC_exHC = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_restrict_BOIC_exHC = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_BAIC_MASK )
        gsdi_data.cphs_info.csp.call_restrict_BAIC = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_restrict_BAIC = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_BIC_ROAM_MASK )
        gsdi_data.cphs_info.csp.call_restrict_BIC_Roam = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_restrict_BIC_Roam = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_3:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_MPTY_MASK )
        gsdi_data.cphs_info.csp.other_supps_MPTY = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.other_supps_MPTY = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CUG_MASK )
        gsdi_data.cphs_info.csp.other_supps_CUG = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.other_supps_CUG = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_AOC_MASK )
        gsdi_data.cphs_info.csp.other_supps_AoC = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.other_supps_AoC = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_PREF_CUG_MASK )
        gsdi_data.cphs_info.csp.other_supps_Pref_CUG = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.other_supps_Pref_CUG = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CUG_OA_MASK )
        gsdi_data.cphs_info.csp.other_supps_CUG_OA = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.other_supps_CUG_OA = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_4:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_HOLD_MASK )
        gsdi_data.cphs_info.csp.call_completion_HOLD = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_completion_HOLD = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CW_MASK )
        gsdi_data.cphs_info.csp.call_completion_CW = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_completion_CW = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CCBS_MASK )
        gsdi_data.cphs_info.csp.call_completion_CCBS = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_completion_CCBS = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_USER_SIG_MASK )
        gsdi_data.cphs_info.csp.call_completion_User_User_Signalling = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.call_completion_User_User_Signalling = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_5:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_SM_MT_MASK )
        gsdi_data.cphs_info.csp.teleservices_SM_MT = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_SM_MT = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_SM_MO_MASK )
        gsdi_data.cphs_info.csp.teleservices_SM_MO = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_SM_MO = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_SM_CB_MASK )
        gsdi_data.cphs_info.csp.teleservices_SM_CB = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_SM_CB = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_REPLY_PATH_MASK )
        gsdi_data.cphs_info.csp.teleservices_Reply_Path = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_Reply_Path = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_DEL_CONF_MASK )
        gsdi_data.cphs_info.csp.teleservices_Del_Conf = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_Del_Conf = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_PROTOCOL_ID_MASK )
        gsdi_data.cphs_info.csp.teleservices_Protocol_ID = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_Protocol_ID = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_VALIDITY_PER_MASK )
        gsdi_data.cphs_info.csp.teleservices_Validity_Period = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.teleservices_Validity_Period = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_6:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_ALS_MASK )
        gsdi_data.cphs_info.csp.cphs_teleservices_als = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.cphs_teleservices_als = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_7:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_RSVD_SST_PHASE_1_MASK )
        gsdi_data.cphs_info.csp.cphs_features_SST = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.cphs_features_SST = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_8:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CLIP_MASK )
        gsdi_data.cphs_info.csp.number_identify_CLIP = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.number_identify_CLIP = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CoLR_MASK )
        gsdi_data.cphs_info.csp.number_identify_CoLR = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.number_identify_CoLR = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CoLP_MASK )
        gsdi_data.cphs_info.csp.number_identify_CoLP = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.number_identify_CoLP = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_MCI_MASK )
        gsdi_data.cphs_info.csp.number_identify_MCI = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.number_identify_MCI = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CLI_SEND_MASK )
        gsdi_data.cphs_info.csp.number_identify_CLI_send = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.number_identify_CLI_send = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_CLI_BLOCK_MASK )
        gsdi_data.cphs_info.csp.number_identify_CLI_block = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.number_identify_CLI_block = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_9:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_GPRS_MASK )
        gsdi_data.cphs_info.csp.p2plus_services_GPRS = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.p2plus_services_GPRS = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_HSCSD_MASK )
        gsdi_data.cphs_info.csp.p2plus_services_HSCSD = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.p2plus_services_HSCSD = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_VOICE_GRP_CALL_MASK )
        gsdi_data.cphs_info.csp.p2plus_services_Voice_Group_call = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.p2plus_services_Voice_Group_call = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_VOICE_BRDCST_SVC_MASK )
        gsdi_data.cphs_info.csp.p2plus_services_Voice_Broadcast_Svc = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.p2plus_services_Voice_Broadcast_Svc = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_MULTI_SUB_PROFILE_MASK )
        gsdi_data.cphs_info.csp.p2plus_services_Muli_Sub_Profile = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.p2plus_services_Muli_Sub_Profile = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_MULTI_BAND_MASK )
        gsdi_data.cphs_info.csp.p2plus_services_Multiband = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.p2plus_services_Multiband = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_C0:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_PLMN_MODE_MASK )
        gsdi_data.cphs_info.csp.value_added_PLMN_MODE = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.value_added_PLMN_MODE = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_VPS_MASK )
        gsdi_data.cphs_info.csp.value_added_VPS = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.value_added_VPS = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_SM_MO_PAGING_MASK )
        gsdi_data.cphs_info.csp.value_added_SM_MO_PAGING = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.value_added_SM_MO_PAGING = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_SM_MO_EMAIL_MASK )
        gsdi_data.cphs_info.csp.value_added_SM_MO_EMAIL = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.value_added_SM_MO_EMAIL = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_FAX_MASK )
        gsdi_data.cphs_info.csp.value_added_FAX = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.value_added_FAX = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_DATA_MASK )
        gsdi_data.cphs_info.csp.value_added_DATA = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.value_added_DATA = GSDI_CSP_IND_OFF;

      if ( data_ext_p[svcs_index] & GSDI_CPSH_CSP_LANGUAGE_MASK )
        gsdi_data.cphs_info.csp.valued_added_Language = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.valued_added_Language = GSDI_CSP_IND_OFF;

      break;

    case GSDI_CPHS_CSP_SVC_GROUP_D5:

      if ( data_ext_p[svcs_index] & GSDI_CPHS_CSP_INFO_NUMBERS_MASK )
        gsdi_data.cphs_info.csp.information_numbers = GSDI_CSP_IND_ON;
      else
        gsdi_data.cphs_info.csp.information_numbers = GSDI_CSP_IND_OFF;

      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR("Bad Service Group: 0x%x Iteration: 0x%x",
                data_ext_p[iteration],iteration,0);
    }
    iteration += 2;
  }

  return GSDI_SUCCESS;
}
/*===========================================================================
FUNCTION GSDI_CPHS_READ_FILE

DESCRIPTION
  This function is a wrapper function used to encapsulate the details of
  performing the read on a File.

  This function will allocate memory for a gsdi_sim_read_req_T and
  populate the read request accordingly.

  This function utilizes the Extended READ APIs used to support reads of
  256+ bytes.

INPUT:
  gsdi_slot_id_type slot:  The slot ID for SIM in which the CPHS Information
                           is to retrieved from.
  uint8 *  data_ext_p:     The pointer to the buffer used to store the
                           read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T.

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_cphs_read_file(
  gsdi_technology_type tech,
  gsdi_slot_id_type    slot,
  gsdi_file_enum_type  gsdi_file,
  int32                rec_num,
  int32 *              returned_data_len_p,
  uint8 *              data_ext_p
)
{
  gsdi_sim_read_req_T * read_req    = NULL;
  gsdi_returns_T        gsdi_status = GSDI_ERROR;
  uim_items_enum_type   uim_file    = UIM_NO_SUCH_ITEM;
  uint8                 temp_var1   = 0x00;
  uint8                 temp_var2   = 0x00;

  /* --------------------------------------------------------------------------
     Perform basic parameter checking
     slot:                 Can only be GSDI_SLOT_1 or GSDI_SLOT_2
     returned_data_len_p:  Can not be NULL
     data_ext_p:           Can not be NULL
     ------------------------------------------------------------------------*/
  if ( slot != GSDI_SLOT_1 &&
       slot != GSDI_SLOT_2 )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( data_ext_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL DATA BUFFER POINTER",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( tech != GSDI_UMTS &&
       tech != GSDI_GSM )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD TECHNOLOGY PARAMETER",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if (returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("NULL RETURNED_DATA_LEN_P",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for the internal read request and memset it to 0x00 once
     the memory is allocated.
     ------------------------------------------------------------------------*/
  read_req = mmgsdi_malloc(sizeof(gsdi_sim_read_req_T));

  if ( read_req == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("COULD NOT ALLOCATE MEMORY FOR READ REQ",0,0,0);
    return GSDI_ERROR;
  }

  memset(read_req,0x00,sizeof(gsdi_sim_read_req_T));

  /* --------------------------------------------------------------------------
     Convert the GSDI Filename to a UIM File Name as
     we prep the request to be sent to UIM.
     ------------------------------------------------------------------------*/
  gsdi_status = gsdi_util_map_file_enum_to_uim_enum(gsdi_file,&uim_file);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("COULD NOT MAP TO UIM FILENAME: 0x%x",gsdi_file,0,0);
  }

  /* --------------------------------------------------------------------------
     No Populate the read request so that it can be used to send a command to
     the UIM Interface
     ------------------------------------------------------------------------*/
  read_req->sim_filename        = (word)uim_file;
  read_req->rec_num             = (byte)rec_num;
  read_req->search_string_len   = 0;
  read_req->data_offset         = 0;
  read_req->required_data_len   = GSDI_STANDARD_READ_LEN;
  read_req->message_header.slot = slot;
  if ( rec_num == 0 )
  {
    read_req->index_mode        = GSDI_NO_INDEX;
  }
  else
  {
    read_req->index_mode        = GSDI_INDEX_ABSOLUTE;
  }

  /* --------------------------------------------------------------------------
     Send the request to the UIM Server using the GSM Path or the USIM
     PATH.  The RUIM Path is not yet supported.
     ------------------------------------------------------------------------*/
  if ( tech == GSDI_UMTS )
  {
    gsdi_status = gsdi_umts_proc_usim_read(read_req,
                                           (byte *)&temp_var1,
                                           (byte *)&temp_var2,
                                           (byte *)returned_data_len_p,
                                           data_ext_p);
  }
  else
  {
    gsdi_status = gsdi_gsm_proc_sim_read( read_req,
                                          (byte *)&temp_var1,
                                          (byte *)&temp_var2,
                                          (byte *)returned_data_len_p,
                                          data_ext_p );
  }

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("READ ATTEMPT FILE 0x%x FAILURE: 0x%x",read_req->sim_filename,gsdi_status,0);
  }

  /* Free up the memory allocated */
  MMGSDIUTIL_TMC_MEM_FREE(read_req);

  return gsdi_status;
}


/*===========================================================================
FUNCTION GSDI_PROC_CPHS_POST_PIN1_INIT

DESCRIPTION
  This function will run the CPHS Intialization procedures.  The purpose
  of this function is to cache the relevant CPHS Information for use
  by other Tasks for later usage.



INPUT:
  gsdi_slot_id_type slot:       The slot ID for SIM in which the CPHS
                                Information is to retrieved from.

  uint8 *           data_ext_p: This is the pointer to the data buffer that
                                requires the appropriate function to
                                allocate the space for the buffer as well
                                as free the buffer space.
                                The memory allocated will be allocated
                                upon entering this function and will be
                                released accordingly at the end of the
                                function.
OUTPUT:
  None.  See Return Value

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_cphs_post_pin1_init(
  gsdi_slot_id_type    slot,
  int32 *              returned_data_len_p,
  uint8 *              data_ext_p,
  boolean              read_cphsi,
  boolean              read_csp
)
{
  gsdi_file_enum_type  filename           = GSDI_NO_SUCH_ITEM;
  gsdi_technology_type technology         = GSDI_GSM;
  gsdi_returns_T       read_cphsi_status  = GSDI_ERROR;
  gsdi_returns_T       gsdi_status        = GSDI_ERROR;
  uint8              * apps_enabled_p     = NULL_PTR;
  uint8                application        = 0x00;


  /* --------------------------------------------------------------------------
     Perform Paramater Checks and allocate memory for the cphs Requests
     ------------------------------------------------------------------------*/
  if ( ( slot != GSDI_SLOT_1 ) &&
       ( slot != GSDI_SLOT_2 ) )
  {
    MMGSDI_DEBUG_MSG_ERROR("CPHS: BAD SLOT ID: %x", slot,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  if ( returned_data_len_p == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("CPHS:  NULL RETURNED_DATA_LEN_P",0,0,0);
    return GSDI_INCORRECT_PARAMS;
  }
  /*data_ext_p already malloced*/
  memset(data_ext_p, 0x00, (sizeof(uint8) * GSDI_MAX_DATA_BLOCK_LEN));

  /* --------------------------------------------------------------------------
     Determine the card type to be used to differentiate between the
     GSDI Filenames Used.  Also determine the technology type used to be used
     to determine which path to use to the UIM Server.
     ------------------------------------------------------------------------*/
  switch ( slot )
  {
  case GSDI_SLOT_1:
    apps_enabled_p = &gsdi_data.card_applications.apps_enabled;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    apps_enabled_p = &gsdi_data.card_applications.apps2_enabled;
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  case GSDI_SLOT_AUTOMATIC:
  case GSDI_SLOT_NONE:
  default:
    /* Error Case for Bad Slot ID...free memory and return
    ** to calling function
    */
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: 0x%x",slot,0,0);
    return GSDI_ERROR;
  }

  application = *apps_enabled_p & GSDI_USIM_APP_MASK;

  if ( application == GSDI_USIM_APP_MASK )
  {
    technology = GSDI_UMTS;
  }

  if(read_cphsi)
  {
    /* --------------------------------------------------------------------------
       First Determine if CPHS is supported on this SIM/USIM by trying to
       read EF CPHS Information Files to access the CPHS Service Table
       ------------------------------------------------------------------------*/
    if ( application == GSDI_USIM_APP_MASK )
    {
      filename = GSDI_USIM_CPHSI;
    }
    else
    {
      filename = GSDI_GSM_CPHSI;
    }

    read_cphsi_status = gsdi_cphs_read_file(technology,
                                            slot,
                                            filename,
                                            0,
                                            returned_data_len_p,
                                            data_ext_p);

    if (read_cphsi_status == GSDI_SUCCESS )
    {
      gsdi_status = gsdi_cphs_parse_cphsi_data(*returned_data_len_p,
                                             data_ext_p);

    }
    else
    {
       /*If Mode is USIM, then the File may not be present, do not error out 
         just yet. Read Explanation below*/
       if(application == GSDI_USIM_APP_MASK)
       {
         gsdi_status = GSDI_SUCCESS;
       }
       else
       {
         MMGSDI_DEBUG_MSG_ERROR("Could not read CPHSI: 0x%x", gsdi_status,0,0);
         return GSDI_ERROR;
       }
    
    }

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("Failure parsing CPHSI: 0x%x, CPHSI Not Valid", 
                              gsdi_status,0,0);
      return GSDI_ERROR;
    }
  }

  if(read_csp)
  {
    memset(data_ext_p, 0x00, (sizeof(uint8) * GSDI_MAX_DATA_BLOCK_LEN));
    /* --------------------------------------------------------------------------
       Next Read and Cache the Customer Service Profile File:
       ------------------------------------------------------------------------*/
    if ( application == GSDI_USIM_APP_MASK )
    {
      filename = GSDI_USIM_CSP;
    }
    else
    {
      filename = GSDI_GSM_CSP;
    }

    gsdi_status = gsdi_cphs_read_file(technology,
                                      slot,
                                      filename,
                                      0,
                                      returned_data_len_p,
                                      data_ext_p);

    if (gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("Failure reading CSP: 0x%x, CSP Not Supported", gsdi_status,0,0);
      return GSDI_ERROR;
    }

    /* --------------------------------------------------------------------------
       At this point, we will Parse the CSP File in order to save the information
       to the gsdi_data type for future use as needed.
       ------------------------------------------------------------------------*/
    gsdi_status = gsdi_cphs_parse_csp_data(*returned_data_len_p,
                                           data_ext_p);

    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("Failure parsing CSP: 0x%x, CSP Not Valid", gsdi_status,0,0);
      return GSDI_ERROR;
    }

    /*-------------------------------------------------------------------------
      2G CARD Configuration:
      DF GSM -> EF CPHSI, EF CSP

      3g Card Configuration
      ADF-> EF CSP
      DF_GSM -> EF CPHSI

      With 3G cards ,the CPHSI file is only present under DF GSM and not under 
      ADF, hence the read of the USIM CPHSI will result in an error. Since CSP
      is still present in the ADF , it implies that the card supports CPHS.

      gsdi_data.cphs_info.cphs_capable suggest if the card supports CPHS or 
      not.
      
      Set the CPHS Available to TRUE so that the applications can read the 
      other information from gsdi_data.cphs_info
      -----------------------------------------------------------------------*/
    if(read_cphsi_status != GSDI_SUCCESS &&
       application == GSDI_USIM_APP_MASK)
    {
       gsdi_data.cphs_info.cphs_capable = TRUE;
       gsdi_data.cphs_info.services.csp = GSDI_ACTIVATED_AND_ALLOCATED;
    }
  }

  /* --------------------------------------------------------------------------
     Cleanup procedures to free up any memory allocated for this function
     ------------------------------------------------------------------------*/

  return GSDI_SUCCESS;
}

#endif /* FEATURE_MMGSDI_CPHS */
