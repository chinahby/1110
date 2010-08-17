/*===========================================================================

                   D A T A   S E R V I C E S  M I P
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific Mobile IP
  related AT commands.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcmip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIP command.
  Enable or disable Mobile IP.

dsat707_exec_qcmipp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.

dsat707_exec_qcmipt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.

dsat707_exec_qcmipep_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.

dsat707_exec_qcmipmass_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.

dsat707_exec_qcmipmhss_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.

dsat707_exec_qcmipmaspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.

dsat707_exec_qcmipmhspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.

dsat707_exec_qcmiprt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.

dsat707_exec_qcmipnai_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.

dsat707_exec_qcmipha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.

dsat707_exec_qcmippha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.

dsat707_exec_qcmipsha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.

dsat707_exec_qcmipgetp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.

  Copyright (c) 1995 - 2009 by QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mip.c_v   1.10   07 Feb 2003 10:22:40   sramacha  $
$Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707mip.c#12 $ $DateTime: 2009/05/13 00:44:13 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/09   bs      Fixed $QCMIPMHSS command.
11/21/08   bs      Fixed $QCMIPGETP query command.
09/01/08   bs      Fixed $QCMIPMHSS/$QCMIPMASS crash issue.
07/09/08   bs      Added support for CDMA NAARefresh.
06/13/08   bs      Fixed RVCT2.2 compiler warnings.
04/09/08   bs      Fixed Klocwork errors.
12/13/07   sn      Fixed compile error.
11/19/07   sn      Added "AEEstd.h" and fixed compile error.
11/07/07   sn      Replaced banned string APIs.
11/10/06   an      Fixed compilation errors for 6020
09/21/07 sq/mga/yz Merged and made changes to support FEATURE_UIM_SUPPORT_3GPD 
09/07/06   jd      Don't call MIP gen key data from AT$QCMIPP unless sec has
                   indicated random data source has been filled.
09/07/06   jd      Send command to reencrypt DMU keys when QCMIPP changes.
12/25/03   rsl     Changes relating to being in the right context before 
                   calling get_nv_Item.
05/05/03   jd      Removed dependency on dsiface_def.h
02/07/03   rsl     Don't call exec_param in mipha, mipsha, mippha, mipmass, 
                   mipmhss, mipnai.
12/06/02   ak      Added some char * typecasts so that strcpy wouldn't
                   bomb.
12/02/02   ak      Made set_mdr_val and set_crm_val external functions.
11/14/02   jd      Removed prototypes for dsat707_erase_shadow_nv and 
                   dsat707_fill_mip_vals, reference to qcmip_mode
                   (no longer used)
09/17/02   jay     Added break statements in QCMIPRT processing.
07/12/02   ak      Function to check MDR now a global function.  In exec_
                   _qcmip, call ds707_pkt_so_recal(), after changing qcmdr.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DS_MOBILE_IP
#include "AEEstd.h"
#include "dsati.h"
#include "dsat707mipctab.h"
#include "dsat707vendctab.h"
#include "dsat707extctab.h"
#include "dsat707mip.h"
#include "dsatparm.h"
#include "ds707.h"
#include "ds707_so_pkt.h"
#include "ps_ppp_fsm.h"
#include "dsatctab.h"
#include "dstaski.h"
#include "msg.h"
#include "ps_mip_task.h"
#include "ps_svc.h"
#include "dsmip.h"
#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

/* Define an ugly signal to PS to indicate when random data is first
   available. 
*/
#ifdef FEATURE_DS_MOBILE_IP_DMU
  #ifndef FEATURE_DATA_MM
    #include "ps.h"
    #define  MIP_RAND_DATA_READY()     rex_set_sigs(&ps_tcb, PS_SEC_RAND_SIG)
  #else
    //#define FEATURE_USE_OLD_PS_RAND_IFACE
    #ifdef FEATURE_USE_OLD_PS_RAND_IFACE
      #include "ps_svc.h"
      #define  MIP_RAND_DATA_READY()   PS_SET_SIGNAL (PS_MIP_DMU_RAND_SIGNAL)
    #else
      #include "ps_mip_task.h"
    #endif /* FEATURE_USE_OLD_PS_RAND_IFACE */
  #endif /* FEATURE_DATA_MM */
#else
  #define  MIP_RAND_DATA_READY()
#endif /* FEATURE_DS_MOBILE_IP_DMU */
#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
dsat707_mip_result_type mip_status;
nv_stat_enum_type nv_status = NV_DONE_S;
byte nv_pref;

/*---------------------------------------------------------------------------
  The ds_atcop_crm_orig_val variable is used to store the original value
  of CRM.  A value of -1 indicates that it has not been initialized.
---------------------------------------------------------------------------*/
signed int dsat707_crm_orig_val = -1;

LOCAL const dsat_string_item_type mip_string_range [] =
                                                    "(20,21,23-7E),(0-1)";
LOCAL const dsat_string_item_type string_mip_ip_range [] =
                                 "((0-255).(0-255).(0-255).(0-255)),(0-1)";

shadow_nv_type shadow_nv;
byte ss_size;

/*---------------------------------------------------------------------------
  Protypes for local functions:
---------------------------------------------------------------------------*/
LOCAL dsat707_util_result_type dsat707_strip_quotes
(
  const tokens_struct_type *raw_string,
  char                     *parsed_string,
  const dsati_cmd_type     *table_entry
) ;

dsat707_util_result_type dsat707_ip_str_to_uint32
(
  char   *ip_string,
  uint32 *ip_uint32
) ;

byte* dsat707_ip_uint32_to_str
(
  uint32 ip_uint32,
  byte   *rb_ptr
) ;

LOCAL byte* dsat707_qcmipgetp
(
  byte *rb_ptr
) ;

LOCAL dsat_result_enum_type ds_at707_mip_ip
(
  const tokens_struct_type  *tok_ptr,
  byte                      *rb_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ip_val,
  uint32                    *ds_atcop_ip_uint32val,
  uint32                    **shadow_nv_ip
) ;

LOCAL dsat_result_enum_type ds_at707_mip_ss
(
  const tokens_struct_type  *tok_ptr,
  dsm_item_type             *rb_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ss_val,
  byte                      **shadow_nv_ss,
  byte                      **shadow_nv_ss_len
) ;

LOCAL dsat_result_enum_type ds_at707_mip_spi
(
  const dsati_cmd_type  *table_entry,
  dsat_num_item_type    *ds_atcop_qcmip_spi_val,
  uint32                **shadow_nv_spi
) ;

LOCAL dsat_result_enum_type ds_at707_mip_ssx
(
  const dsati_cmd_type  *table_entry,
  dsat_num_item_type    *ds_atcop_qcmip_ssx_val,
  byte                  **shadow_nv_ssx,
  byte                  **shadow_nv_ssx_len
) ;

LOCAL void qcmip_check_crm_val
(
  void
) ;

#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIP_CMD

  DESCRIPTION
  Exec functions take the result from the command line parser
  and execute the command. 

  This is the exec function for AT$QCMIP command.

  AT$QCMIP enables or disables Mobile IP.
  0 - Mobile IP disabled, Simple IP only.
  1 - Mobile IP preferred.
  2 - Mobile IP only.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmip_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  if ( (result == DSAT_OK) &&
       ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
     )
  {
    (void)dsat707_qcmip_update(FALSE);

    /*-----------------------------------------------------------------------
      Write QCMIP value into NV.
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_qcmip = dsat707_qcmip_val;
    (void) dsatutil_put_nv_item( NV_DS_QCMIP_I, &ds_nv_item );
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmip_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipp_cmd
(
  dsat_mode_enum_type      mode,          /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr        /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  if ( (result == DSAT_OK) &&
       ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
     )
  {
    if(FALSE == dsat707_qcmipp_update(TRUE))
    {
      result = DSAT_ERROR;
    }
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipp_cmd */

#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.
  0 - Use of RFC 2002bis authentication is disabled. RFC 2002
  style authentication is used instead.
  1 - Use of RFC 2002bis authentication is enabled.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*-----------------------------------------------------------------------
    If Using R-UIM, 2002bis can be read from the card.   
    -----------------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      /* Do not Allow the 2002bis variable to be written */
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  if ( (result == DSAT_OK) &&
       ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
     )
  {
    /*-----------------------------------------------------------------------
      Write QCMIPT value into NV.
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_mip_2002bis_mn_ha_auth =
      (dsat707_qcmipt_val & MIP_QCMIPT_MN_HA_AUTH_2002BIS_BIT)
      ? TRUE : FALSE;

    (void) dsatutil_put_nv_item( NV_DS_MIP_2002BIS_MN_HA_AUTH_I, &ds_nv_item );
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipt_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPEP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.
  0 - Disable the currently active profile.
  1 - Enable the currently active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipep_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*-----------------------------------------------------------------------
    If Using R-UIM, all profiles are always enabled.   
    -----------------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      /* Do not Allow the Enable Profile variable to be written */
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  if ( (result == DSAT_OK) &&
       ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
     )
  {
    /*-----------------------------------------------------------------------
      Write QCMIPEP value into NV.
    -----------------------------------------------------------------------*/
    dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
    ds_nv_item.ds_mip_enable_prof[dsat707_qcmipp_val] = dsat707_qcmipep_val;
    (void) dsatutil_put_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipep_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmass_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr       /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  result = ds_at707_mip_ss(tok_ptr,
                           rb_ptr,
                           tab_entry,
                           dsat707_qcmipmass_val,
                           &shadow_nv.mn_aaa_ss,
                           &shadow_nv.mn_aaa_ss_len);
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmass_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmhss_cmd
(
  dsat_mode_enum_type      mode,          /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr        /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  result = ds_at707_mip_ss(tok_ptr,
                           rb_ptr,
                           tab_entry,
                           dsat707_qcmipmhss_val,
                           &shadow_nv.mn_ha_ss,
                           &shadow_nv.mn_ha_ss_len);
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmhss_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmaspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*------------------------------------------------------------
    If Using R-UIM, Cannot set SPI  
    ------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  MSG_MED("Processing QCMIPMASPI",0,0,0);

  if( (result == DSAT_OK) &&
      ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) ) 
    )
  {
    result = ds_at707_mip_spi( tab_entry,
                               dsat707_qcmipmaspi_val,
                               &shadow_nv.mn_aaa_spi );
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmaspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmhspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*------------------------------------------------------------
    If Using R-UIM, Cannot set SPI  
    ------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

 result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 MSG_MED("Processing QCMIPMHSPI",0,0,0);

 if ( (result == DSAT_OK) && 
      ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) ) 
    )
 {
   result = ds_at707_mip_spi(tab_entry,
                             dsat707_qcmipmhspi_val,
                             &shadow_nv.mn_ha_spi);
 }
 rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
 return result;
} /* dsat707_exec_qcmipmhspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASSX command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmassx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 byte *res_buff_ptr = rb_ptr->data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 MSG_MED("Processing QCMIPMASSX",0,0,0);
 
 if ( (result == DSAT_OK) &&
      ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
    )
 {

#ifdef FEATURE_UIM_SUPPORT_3GPD
      /*------------------------------------------------------------
      If Using R-UIM, Cannot set SS   
      ------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        return DSAT_ERROR;
      }
#endif /* FEATURE_UIM_SUPPORT_3GPD */ 

   result = ds_at707_mip_ssx( tab_entry,
                              dsat707_qcmipmassx_val,
                              &shadow_nv.mn_aaa_ss,
                              &shadow_nv.mn_aaa_ss_len );
 }
 else if (tok_ptr->op == (NA|QU))
 {    
    
#ifdef FEATURE_UIM_SUPPORT_3GPD
      /*------------------------------------------------------------
        If using RUIM display set
      ------------------------------------------------------------*/
    if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
    {
      (void) std_strlcpy((char *)res_buff_ptr,
                         "$QCMIPMASSX: Set", 
                         rb_ptr->size);
    }
    else
#endif /* FEATURE_UIM_SUPPORT_3GPD */
    {
      if(shadow_nv.mn_aaa_ss != NULL)
      {
        (void) std_strlcpy((char *)res_buff_ptr, 
                           "$QCMIPMASSX: Set", 
                           rb_ptr->size);
      }
      else
      {
        ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
        dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);

        if( ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length != 0)
        {
          (void) std_strlcpy((char *)res_buff_ptr, 
                             "$QCMIPMASSX: Set", 
                             rb_ptr->size);
        }
        else
        {
          (void) std_strlcpy((char *)res_buff_ptr,
                             "$QCMIPMASSX: Unset", 
                             rb_ptr->size);
        }
      }
    }
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmassx_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSSX command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmhssx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 byte* res_buff_ptr = rb_ptr->data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 MSG_MED("Processing QCMIPMHSSX",0,0,0);

 if ( (result == DSAT_OK) && 
      ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
    )
 {
#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*------------------------------------------------------------
    If Using R-UIM, Cannot set SS   
    ------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
        UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
    {
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */  

   result = ds_at707_mip_ssx( tab_entry,
                              dsat707_qcmipmhssx_val,
                              &shadow_nv.mn_ha_ss,
                              &shadow_nv.mn_ha_ss_len );
 }
 else if (tok_ptr->op == (NA|QU))
 {

#ifdef FEATURE_UIM_SUPPORT_3GPD
    if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
           UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
    {
      (void) std_strlcpy((char *)res_buff_ptr, 
                         "$QCMIPMHSSX: Set", 
                         rb_ptr->size);
    }
    else
#endif /* FEATURE_UIM_SUPPORT_3GPD */
    {
      if(shadow_nv.mn_ha_ss != NULL)
      {
        (void) std_strlcpy((char *)res_buff_ptr, 
                           "$QCMIPMHSSX: Set", 
                           rb_ptr->size);
      }
      else
      {
        ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
        dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);

        if( ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length != 0)
        {
          (void) std_strlcpy((char *)res_buff_ptr, 
                             "$QCMIPMHSSX: Set", 
                             rb_ptr->size);
        }
        else
        {
          (void) std_strlcpy((char *)res_buff_ptr, 
                             "$QCMIPMHSSX: Unset", 
                             rb_ptr->size);
        }
      }
    }
  }

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qcmipmhspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPRT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmiprt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*-----------------------------------------------------------------------
    If Using R-UIM, RT is not writeable.   
    -----------------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
       ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
    {
      /* Do not Allow the RT variable to be written */
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

 result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 MSG_MED("Processing QCMIPRT",0,0,0);

  if ( (result == DSAT_OK) && 
       ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
     )
  {
    switch(dsat707_qcmiprt_val[1])
    {
      /*---------------------------------------------------------------------
        Dont Commit to NV
      ---------------------------------------------------------------------*/
      case 0:
        shadow_nv.rev_tun = (boolean*) &dsat707_qcmiprt_val[0];
        break;

      /*---------------------------------------------------------------------
        Commit to NV
      ---------------------------------------------------------------------*/
      case 1:
        shadow_nv.rev_tun=NULL;
        ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
        dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

        ds_nv_item.ds_mip_gen_user_prof.rev_tun_pref =
                                             dsat707_qcmiprt_val[0];
        nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I, &ds_nv_item);
        if( nv_status != NV_DONE_S )
        {
          result = DSAT_ERROR;
          DSATI_ERROR_MSG();
        }
        break;

      /*---------------------------------------------------------------------
        Error
      ---------------------------------------------------------------------*/
      default:
        result = DSAT_ERROR;
        DSATI_ERROR_MSG();
        break;
    }/* switch ()*/
  }
 rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
 return result;
} /* dsat707_exec_qcmiprt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPNAI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipnai_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte  *res_buff_ptr = rb_ptr->data_ptr;
  dsat707_util_result_type util_status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 MSG_MED("Processing QCMIPNAI",0,0,0);

 if (result == DSAT_OK)
 {
   /*------------------------------------------------------------------------
     Is it AT$QCMIPNAI?
   ------------------------------------------------------------------------*/
    if(tok_ptr->op == (NA|QU))
    {
      /*---------------------------------------------------------------------
        If NAI length is 0 return "unset" else display the nai.
      ---------------------------------------------------------------------*/
      if(strlen((char *)dsat707_qcmipnai_val) != 0 )
      {
        (void) std_strlcpy((char *)res_buff_ptr, 
                           (char *)dsat707_qcmipnai_val, 
                           rb_ptr->size);

        /*-------------------------------------------------------------------
          If the shadow_nv is not null nai is not empty.
        -------------------------------------------------------------------*/
        if(shadow_nv.nai != NULL)
        {
          (void) std_strlcat((char *)res_buff_ptr, 
                             ",0", 
                             rb_ptr->size - std_strlen((char *)res_buff_ptr));
        }
        else
        {
          (void) std_strlcat((char *)res_buff_ptr, 
                             ",1",
                             rb_ptr->size - std_strlen((char *)res_buff_ptr));
        }
      }
      else
      {
        (void) std_strlcpy((char *)res_buff_ptr,
                           "$QCMIPNAI: Unset", 
                           rb_ptr->size);
      }
    }/* if AT$QCMIPNAI? */

    /*-----------------------------------------------------------------------
      Is it AT$QCMIPNAI=?
    -----------------------------------------------------------------------*/
    else if(tok_ptr->op == (NA|EQ|QU))
    {
      dsatutil_fmt_string_range(tab_entry,
                                res_buff_ptr,
                                mip_string_range);
    }
    /*-----------------------------------------------------------------------
      Enter a new NAI value
    -----------------------------------------------------------------------*/
    else
    {
#ifdef FEATURE_UIM_SUPPORT_3GPD
      /*-----------------------------------------------------------------------
      If Using R-UIM, NAI is not writeable.   
      -----------------------------------------------------------------------*/
      if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) &&
         ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
      {
        /* Do not Allow the Enable Profile variable to be written */
        return DSAT_ERROR;
      }
#endif /* FEATURE_UIM_SUPPORT_3GPD */
      /*---------------------------------------------------------------------
        Check the number of arguments of the AT command and set default to 0 
        if only one argument
      ---------------------------------------------------------------------*/
      switch(tok_ptr->args_found)
      {
        case 1:
          nv_pref = '0';
          break;

        case 2:
          nv_pref = *(tok_ptr->arg[1]);
          break;

        default:
          MSG_ERROR ("Incorrect number of args found!", 0, 0, 0);
          result = DSAT_ERROR;
          return result;
      }

      /*---------------------------------------------------------------------
        If the second argument is not 1 or 0 then return an error before
        processing further
      ---------------------------------------------------------------------*/
      if( (nv_pref != '0') && (nv_pref != '1') )
      {
        MSG_ERROR("Second argument must be 0 or 1", 0, 0, 0);
        result = DSAT_ERROR;
        return result;
      }

      util_status = dsat707_strip_quotes(tok_ptr,
                                         (char *)dsat707_qcmipnai_val,
                                         tab_entry);
      if(util_status != STRIP_QUOTES_OK)
      {
        result = DSAT_ERROR;
      }
      else
      {
        /*-------------------------------------------------------------------
          Depending on the NV commit preference, carry out actions 
          accordingly.
        -------------------------------------------------------------------*/
        switch(nv_pref)
        {
          case '0':
            shadow_nv.nai = (char *)dsat707_qcmipnai_val;
            break;

          case '1':
            shadow_nv.nai = NULL;
            ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
            dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);
            ds_nv_item.ds_mip_gen_user_prof.nai_length
              = (byte) strlen((char *)dsat707_qcmipnai_val);

            (void) std_strlcpy((char *)ds_nv_item.ds_mip_gen_user_prof.nai,
                               (char *)dsat707_qcmipnai_val, 
                               MAX_NAI_LEN);

            /*---------------------------------------------------------------
              Write the profile with the new value to NV.
            ---------------------------------------------------------------*/
            nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I,
                                             &ds_nv_item);
            if( nv_status != NV_DONE_S )
            {
              MSG_ERROR("NV Write Error",0,0,0);
              result = DSAT_ERROR;
            }
            break;
					default:
						MSG_ERROR ("Invalid NV preference", 0, 0, 0);
						result = DSAT_ERROR;
				}/* switch NV preference */
      }/* else parse success */
    }/* else regular processing */
 }
 rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
 return result;
} /* dsat707_exec_qcmipnai_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipha_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 byte  *res_buff_ptr = rb_ptr->data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 MSG_MED("Processing QCMIPHA",0,0,0);

 result = ds_at707_mip_ip(tok_ptr,
						  res_buff_ptr,
						  tab_entry,
						  dsat707_qcmipha_val,
						  &dsat707_ha_val,
						  &shadow_nv.ha_ip);
 rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
 return result;
} /* dsat707_exec_qcmipha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmippha_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr       /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 byte  *res_buff_ptr = rb_ptr->data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
MSG_MED("Processing QCMIPPHA",0,0,0);

result = ds_at707_mip_ip(tok_ptr,
						 res_buff_ptr,
						 tab_entry,
						 dsat707_qcmippha_val,
						 &dsat707_pri_ha_val,
						 &shadow_nv.pri_ha_ip);
rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
return result;
} /* dsat707_exec_qcmippha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPSHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipsha_cmd
(
  dsat_mode_enum_type      mode,          /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr        /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 byte  *res_buff_ptr = rb_ptr->data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 MSG_MED("Processing QCMIPSHA",0,0,0);
 result = ds_at707_mip_ip(tok_ptr,
						  res_buff_ptr,
						  tab_entry,
						  dsat707_qcmipsha_val,
						  &dsat707_sec_ha_val,
						  &shadow_nv.sec_ha_ip);
 rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
 return result;
} /* dsat707_exec_qcmipsha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPGETP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipgetp_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr       /*  Place to put response       */
)
{
 dsat_result_enum_type result = DSAT_OK;
 byte  *res_buff_ptr = rb_ptr->data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
 MSG_MED("Processing QCMIPGETP",0,0,0);
 result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 if ( (result == DSAT_OK) &&
      ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA)) )
    )
 {
   /*------------------------------------------------------------------------
     Do a "get" on the NV for user profile to check the return value
     If the NV status is not "done_s" then the profile does not exist.
     So return error.
   ------------------------------------------------------------------------*/
   ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipgetp_val;
   nv_status = dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I, &ds_nv_item);
   if(nv_status != NV_DONE_S)
   {
     result = DSAT_ERROR;
     return result;
   }

   /*------------------------------------------------------------------------
     If the second argument is not entered set it to the qcmipp_val
   ------------------------------------------------------------------------*/
   if(tok_ptr->args_found == 0)
   {
     dsat707_qcmipgetp_val = dsat707_qcmipp_val;
   }
   res_buff_ptr = dsat707_qcmipgetp(res_buff_ptr);
 }
 
 rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
 return result;
} /* dsat707_exec_qcmipgetp_cmd */

/*===========================================================================
FUNCTION DSAT707_STRIP_QUOTES

DESCRIPTION
  This fuction is used to strip quotes from a quoted string, and the
  destination string shall be padded with \0 at the end.
  The function also checks for the range of the AT command and throws out an
  error if the range is exceeded.

  For examples,
    "123.12.23.21"           ---> 123.12.23.21\0
    "nobody,one@nowhere.com" ---> nobody,one@nowhere.com\0

DEPENDENCIES
  None

PARAMETERS
   raw_string - Returned by the standard ATCoP parser, ds_atcop_parse_ex().
   parsed_string - Buffer where the parsed argument shall be returned.
   table_entry  -  To find out the size of the argument defined in the table.

RETURN VALUE
  status ( success or error)

SIDE EFFECTS
  None
===========================================================================*/
dsat707_util_result_type dsat707_strip_quotes
(
  const tokens_struct_type *raw_string,
  char                     *parsed_string,
  const dsati_cmd_type     *table_entry
)
{
  dsat707_util_result_type status = STRIP_QUOTES_ERROR;
  boolean loop = TRUE;
  char * temp_ptr;
  uint8 str_len;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure there are no NULL de-reference
  -------------------------------------------------------------------------*/
  if( (raw_string == NULL) || (parsed_string == NULL) )
  {
    MSG_ERROR("NULL pointers", 0, 0, 0);
    return status;
  }

  /*-------------------------------------------------------------------------
    Prepare to process the first argument.
  -------------------------------------------------------------------------*/
  temp_ptr =  (char *) (raw_string->arg[0]);

  /*-------------------------------------------------------------------------
    Check the length of the argument, so that we don't exceed the buf len.
    The actual length is without considering the quotes.
    The "+1" is to account for the null terminator which is included in the
    table entry
  -------------------------------------------------------------------------*/
  str_len = strlen(temp_ptr) + 1;

  /*-------------------------------------------------------------------------
    This is based on the assumption that a opening/closed quote is matched
    by an equivalent closed/opening quote.

    Jayanth: if in trouble check for this
  -------------------------------------------------------------------------*/
  if(*temp_ptr == '"')
  {
    str_len = str_len - 2;
  }

  if(str_len > table_entry->compound)
  {
    MSG_ERROR("Length bigger than defined",0,0,0);
    return status;
  }

  /*-------------------------------------------------------------------------
    Copy string without quotes
  -------------------------------------------------------------------------*/
  while(loop!=FALSE)
  {
    switch(*temp_ptr)
    {
      case '\0':
        loop = FALSE;
        break;

      case '"':
        temp_ptr++;
        break;

      default:
        *parsed_string = *temp_ptr;
        parsed_string++;
        temp_ptr++;
    }
  }/* while */

  /* Terminate the string */
  *parsed_string = '\0';

  status = STRIP_QUOTES_OK;
  return status;
}/* ds_atcop_strip_quotes() */

/*===========================================================================
FUNCTION DSAT707_ERASE_SHADOW_NV

DESCRIPTION
  This functions flushes the shadow NV structure. It is called at the
  initilization of ATCoP,when the active profile has been changed and
  when the call is ended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsat707_erase_shadow_nv
(
  void
)
{
  MSG_MED("Erase shadow NV",0,0,0);
  memset(&shadow_nv, 0, sizeof(shadow_nv));
} /* dsat707_erase_shadow_nv () */

/*===========================================================================
FUNCTION DSAT707_IP_STR_TO_UINT32

DESCRIPTION
  This function takes an IP address in string format (which includes
  "." as delimiters. e.g. 123.23.123.231) and convert the string into
  a uint32 value.

DEPENDENCIES
  None

RETURN VALUE
    status ( success or error)

SIDE EFFECTS
  None
===========================================================================*/
dsat707_util_result_type dsat707_ip_str_to_uint32
(
  char   *ip_string,
  uint32 *ip_uint32
)
{
  byte                   buff[4];
  dsat_num_item_type     ip_val[4];
  atoi_enum_type         atoi_result;
  uint8 buff_index     = 0;
  uint8 ip_index       = 0;
  uint8 n_dots         = 0;
  boolean loop         = TRUE;
  dsat707_util_result_type status = IP_STR_UINT32_ERROR;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    The following loop processes the string representing the IP address. Each
    one of the four-tuples is converted into decimal integer and stored in a
    temporary array. If the value is greater than 255, we return an error,
    because each tuple in the IP cannot be greater than 255.
  -------------------------------------------------------------------------*/
  while(loop != FALSE)
  {
    if( (*ip_string != '.') &&
        (*ip_string != '\0') )
    {
      /* check to see if > 3 digits specified on for this tuple */
      if(buff_index == 3)
      {
        MSG_ERROR("Too many digits",0,0,0);
        return status;
      }
      buff[buff_index] = *ip_string;
      buff_index++;
    }
    else
    {
      n_dots++;
      buff[buff_index]= '\0';  /* null terminate the string */

      /*---------------------------------------------------------------------
        Convert ascii to integer and return error if an invalid ip address
        is entered. eg A.12.14.45
      ---------------------------------------------------------------------*/
      atoi_result = dsatutil_atoi( (ip_val + ip_index),buff,10 );
      if(atoi_result != ATOI_OK)
      {
        MSG_ERROR("Error while converting from atoi",0,0,0);
        return status;
      }
      else if(ip_val[ip_index] > 255)
      {
        MSG_ERROR("IP address tuple greater than 255",0,0,0);
        return status;
      }
      buff_index = 0;
      ip_index++;
      if(*ip_string == '\0')
      {
        loop=FALSE;

        /*-------------------------------------------------------------------
          If the ip address has more than 3 dots it is an invalid 
          ip address eg: 129.23.34  , 129.34.45.65.78 etc.
        -------------------------------------------------------------------*/
        if(n_dots != 4)
        {
          MSG_ERROR("Invalid IP address",0,0,0);
          return status;
        }
      }
    }/* else */
    ip_string++;
  } /* while */

  /*-------------------------------------------------------------------------
    Store the converted IP address into a uint32
  --------------------------------------------------------------------------*/
  *ip_uint32 = (((uint32) ip_val[0]) << 24) |
               (((uint32) ip_val[1]) << 16) |
               (((uint32) ip_val[2]) << 8)  |
               (uint32) ip_val[3];

  MSG_LOW("ip_uint32 is %ld", *ip_uint32,0,0);
  status = IP_STR_UINT32_OK;
  return status;
} /* dsat707_ip_str_to_uint32 () */

/*===========================================================================
FUNCTION DSAT707_IP_UINT32_TO_STR

DESCRIPTION
  This function takes an IP address in uint32 format and returns a string.

DEPENDENCIES
  None

RETURN VALUE
  rb_ptr incremented by the size of the ASCII representation of the IP
  address.

SIDE EFFECTS
  None
===========================================================================*/
byte* dsat707_ip_uint32_to_str
(
  uint32 ip_uint32,
  byte   *rb_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rb_ptr = dsatutil_itoa(((ip_uint32 >> 24) & 0xFF ),rb_ptr,10);
  *rb_ptr++ = '.';
  rb_ptr = dsatutil_itoa(((ip_uint32 >> 16) & 0xFF),rb_ptr,10);
  *rb_ptr++ = '.';
  rb_ptr = dsatutil_itoa(((ip_uint32 >>  8) & 0xFF),rb_ptr,10);
  *rb_ptr++ = '.';
  rb_ptr = dsatutil_itoa((ip_uint32 & 0xFF),rb_ptr,10);

  return rb_ptr;

}/* dsat707_ip_uint32_to_str () */


/*===========================================================================
FUNCTION DSAT707_QCMIP_SET_CRM_VAL

DESCRIPTION
  This function checks the value of QCMIP.  If it is greater than 0, it
  modifies the CRM value to 2, which enables network model operation.
  Changing the value QCMIP back to 0 will restore CRM to its original value
  before it was changed by QCMIP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsat707_qcmip_set_crm_val(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    If QCMIP is greater than 0, CRM is set to 2 which enables network model
    operation.  The original value of CRM is stored so it can be restored
    later when QCMIP is set to 0.
  -------------------------------------------------------------------------*/
  if (dsat707_qcmip_val > 0)
  {
    if (dsat707_crm_orig_val == -1)
    {
      dsat707_crm_orig_val = dsat707_crm_val;
    }
    dsat707_crm_val = DS_CRM_VAL_NETMODEL_MODE;
  }
  /*-------------------------------------------------------------------------
    Else QCMIP is 0 and the original value of CRM is restored if necessary.
    JD - this is broken if crm was !0 to start, then AT$QCMIP=0, AT$QCMIP=0
  -------------------------------------------------------------------------*/
  else
  {
    if (dsat707_crm_orig_val != -1)
    {
      dsat707_crm_val = dsat707_crm_orig_val;
      dsat707_crm_orig_val = -1;
    }
  }
} /*  dsat707_qcmip_set_crm_val() */

/*===========================================================================
  FUNCTION DSAT707_QCMIP_SET_MDR_VAL

  DESCRIPTION
    This function checks the value of QCMIP.  If it is greater than 0, it
    modifies the MDR value to SO 33 preferred.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsat707_qcmip_set_mdr_val(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If QCMIP is greater than 0, MDR is set to 3.
  -------------------------------------------------------------------------*/
  if (dsat707_qcmip_val > 0)
  {
    dsat707_qcmdr_val = DS_MDR_MODE_SO33_PREF;

    /*-----------------------------------------------------------------------
      Modify value of MDR in NV.
    -----------------------------------------------------------------------*/
    ds_nv_item.data_mdr_mode = dsat707_qcmdr_val;
    dsatutil_put_nv_item(NV_DATA_MDR_MODE_I, &ds_nv_item);
  }
} /*  dsat707_qcmip_set_mdr_val() */

/*===========================================================================
FUNCTION QCMIP_CHECK_CRM_VAL

DESCRIPTION
  This function checks the value of CRM.  If it is less than 2, and QCMIP
  is greater than 0, return an error, and restore CRM to the value before
  it was changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void qcmip_check_crm_val()
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    CRM is explicitly set, so set original CRM variable to this value.
  -------------------------------------------------------------------------*/
  dsat707_crm_orig_val = dsat707_crm_val;
  
  /*-------------------------------------------------------------------------
    If CRM is less than DS_CRM_VAL_NETMODEL_MODE, check QCMIP to determine
    correct behaviour.
  -------------------------------------------------------------------------*/
  if (dsat707_crm_val < DS_CRM_VAL_NETMODEL_MODE)
  {
    /*-----------------------------------------------------------------------
      If QCMIP is greater than 0, restore original value of CRM and return
      an error.
    -----------------------------------------------------------------------*/
    if (dsat707_qcmip_val > 0)
    {
      /*---------------------------------------------------------------------
        If QCMIP is 2, CRM had to have taken on the value of 2.
      ---------------------------------------------------------------------*/
      dsat707_crm_val = DS_CRM_VAL_NETMODEL_MODE;
    }
  }
} /* qcmip_check_crm_val() */


/*===========================================================================
  FUNCTION DSAT707_INIT_MIP_PROF

  DESCRIPTION
   This function initializes the MIP user profile and sets the default
   values.

  DEPENDENCIES
   None

  RETURN VALUE
    status ( success or error)

  SIDE EFFECTS
   None
===========================================================================*/
dsat707_mip_result_type dsat707_init_mip_prof
(
 void
)
{
  dsat707_mip_result_type       status = INIT_MIP_PROF_ERROR;
  nv_ds_mip_gen_user_prof_type  *prof_ptr;
  nv_ds_mip_ss_user_prof_type   *ss_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Read the profile num from NV.
  -------------------------------------------------------------------------*/
  nv_status = dsatutil_get_nv_item(NV_DS_MIP_NUM_PROF_I,&ds_nv_item);

  /*-------------------------------------------------------------------------
    If the nv_item is not active, set it to zero
  --------------------------------------------------------------------------*/
  if(nv_status == NV_NOTACTIVE_S)
  {
    
#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*---------------------------------------------------------------------
      If R-UIM is being used, we can't write to it, so fail the operation.
    ---------------------------------------------------------------------*/
    if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
    {
      MSG_ERROR("No MIP user profile on UIM!",0,0,0);
      return status;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */
    ds_nv_item.ds_mip_num_prof = 0;
  }
  else if(nv_status != NV_DONE_S)
  {
    return status;
  }

  /*-------------------------------------------------------------------------
    If there is no existing profile then increment the profile number before
    creating a new one.
  -------------------------------------------------------------------------*/
  if(ds_nv_item.ds_mip_num_prof == 0)
  {

#ifdef FEATURE_UIM_SUPPORT_3GPD
      /*---------------------------------------------------------------------
        If R-UIM is being used, we can't write to it, so fail the operation.
      ---------------------------------------------------------------------*/
      if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
      {
        MSG_ERROR("No MIP user profile on UIM!",0,0,0);
        return status;
      }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    ds_nv_item.ds_mip_num_prof++;
    nv_status = dsatutil_put_nv_item(NV_DS_MIP_NUM_PROF_I,&ds_nv_item);
    if( nv_status != NV_DONE_S )
    {
      MSG_ERROR("NV Write error",0,0,0);
      return  status;
    }

    /*-----------------------------------------------------------------------
      Retreive the user profile based on the current active profile number.
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
    nv_status = dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

    /*-----------------------------------------------------------------------
      If the nv_item is not active set it to zero
    -----------------------------------------------------------------------*/
    switch(nv_status)
    {
      case NV_NOTACTIVE_S:
      case NV_DONE_S:
        MSG_MED("Memset user profile",0,0,0);
        prof_ptr = &ds_nv_item.ds_mip_gen_user_prof;
        memset((void *)prof_ptr, 0, sizeof(ds_nv_item.ds_mip_gen_user_prof));

        /*-------------------------------------------------------------------
          Set the defaults.
        -------------------------------------------------------------------*/
        ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi_set = TRUE;
        ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi = 0x3;
        ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi_set = TRUE;
        ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi = 0x2;
        ds_nv_item.ds_mip_gen_user_prof.home_addr = 0;
        ds_nv_item.ds_mip_gen_user_prof.primary_ha_addr = 0;
        ds_nv_item.ds_mip_gen_user_prof.secondary_ha_addr = 0xFFFFFFFF;

        nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);
        if( nv_status != NV_DONE_S )
        {
          MSG_ERROR("NV Write error %d",nv_status,0,0);
          return  status;
        }
        break;

        default:
          MSG_ERROR("MIP gen profile cannot be retrieved",0,0,0);
          return  status;
    }/* Switch on nv_status */

    /*-----------------------------------------------------------------------
      Erase shared secret profile and set defaults
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
    nv_status = dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
    switch(nv_status)
    {
      case NV_NOTACTIVE_S:
      case NV_DONE_S:
        MSG_MED("Erase shared secret profile",0,0,0);
        ss_ptr = &ds_nv_item.ds_mip_ss_user_prof;
        memset((void *)ss_ptr, 0, sizeof(ds_nv_item.ds_mip_ss_user_prof));

        nv_status = dsatutil_put_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
        if( nv_status != NV_DONE_S )
        {
          MSG_ERROR("NV Write error",0,0,0);
          return  status;
        }
        break;

      default:
        MSG_ERROR("MIP shared secret cannot be retrieved",0,0,0);
        return  status;
    }/* Switch on nv_status */
  }/* if profile num is 0 */

  /*-------------------------------------------------------------------------
    If the profile num is not zero then just retreive the existing profile
    before modifying.
  -------------------------------------------------------------------------*/
  else
  {
    ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
    nv_status = dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);
    switch(nv_status)
    {
      case NV_DONE_S:
        break;

      case NV_NOTACTIVE_S:

#ifdef FEATURE_UIM_SUPPORT_3GPD
      /*---------------------------------------------------------------------
        If R-UIM is being used, we can't write to it, so fail the operation.
      ---------------------------------------------------------------------*/
      if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
      {
        MSG_ERROR("Read profile from RUIM fail ! ",0,0,0);
        return status;
      }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

        MSG_MED("Memset user profile",0,0,0);
        prof_ptr = &ds_nv_item.ds_mip_gen_user_prof;
        memset((void *)prof_ptr,0, sizeof(ds_nv_item.ds_mip_gen_user_prof));
        ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;

        /*-------------------------------------------------------------------
          Set the defaults.
        -------------------------------------------------------------------*/
        ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi_set = TRUE;
        ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi = 0x3;
        ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi_set = TRUE;
        ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi = 0x2;
        ds_nv_item.ds_mip_gen_user_prof.home_addr = 0;
        ds_nv_item.ds_mip_gen_user_prof.primary_ha_addr = 0;
        ds_nv_item.ds_mip_gen_user_prof.secondary_ha_addr = 0xFFFFFFFF;

        dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

        /*-------------------------------------------------------------------
          If the profile is not active disable it by default
        -------------------------------------------------------------------*/
        dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
        ds_nv_item.ds_mip_enable_prof[dsat707_qcmipp_val] = 0;
        dsatutil_put_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
        break;

        default:
          MSG_ERROR("MIP gen profile cannot be retrieved",0,0,0);
          return  status;
      }/* Switch on nv_status */

      /*-----------------------------------------------------------------------
        Erase shared secret profile and set defaults
      -----------------------------------------------------------------------*/
      ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
      nv_status = dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
      switch(nv_status)
      {
        case NV_DONE_S:
          break;

        case NV_NOTACTIVE_S:

#ifdef FEATURE_UIM_SUPPORT_3GPD
      /*---------------------------------------------------------------------
        If R-UIM is being used, we can't write to it, so fail the operation.
      ---------------------------------------------------------------------*/
      if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
      {
        MSG_HIGH("Failed to read ss user profile from RUIM! ",0,0,0);
        break;
      }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

          MSG_MED("Memset shared secret profile",0,0,0);
          ss_ptr = &ds_nv_item.ds_mip_ss_user_prof;
          memset((void *)ss_ptr, 0, sizeof(ds_nv_item.ds_mip_ss_user_prof));

          ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
          nv_status = dsatutil_put_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
          if( nv_status != NV_DONE_S )
          {
            MSG_ERROR("NV Write error",0,0,0);
            return  status;
          }
          break;

        default:
          MSG_ERROR("MIP shared secret cannot be retrieved",0,0,0);
          return  status;

      }/* Switch on nv_status */
  }/* else */
  status= INIT_MIP_PROF_OK;
  return status;
}/* dsat707_init_mip_prof */

/*===========================================================================

FUNCTION DSAT707_QCMIPGETP

DESCRIPTION
  Displays the MIP user profile settings

DEPENDENCIES
  rb_ptr must point to enough space to hold all the values.

RETURN VALUE
  rb_ptr incremented by the size of the ASCII representation of
  the profile, so that it points to the final NULL character of the
  string.

SIDE EFFECTS
  None.

=====================================================================*/
byte * dsat707_qcmipgetp
(
  byte *rb_ptr
)
{
  static const byte prof_num_lbl[] = "Profile:";
  static const byte nai_lbl[]      = "NAI:";
  static const byte ha_lbl[]       = "Home Addr:";
  static const byte pri_ha_lbl[]   = "Primary HA:";
  static const byte sec_ha_lbl[]   = "Secondary HA:";
  static const byte aaa_ss_lbl[]   = "MN-AAA SS:";
  static const byte aaa_spi_lbl[]  = "MN-AAA SPI:";
  static const byte ha_ss_lbl[]    = "MN-HA SS:";
  static const byte ha_spi_lbl[]   = "MN-HA SPI:";
  static const byte rev_tun_lbl[]  = "Rev Tun:";
  static const byte enable_lbl[]   = "Enabled";
  static const byte disable_lbl[]  = "Disabled";
  static const byte set_lbl[]      = "Set";
  static const byte unset_lbl[]    = "Unset";
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Copy the profile label and profile number
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, prof_num_lbl, sizeof(prof_num_lbl));
  rb_ptr += sizeof(prof_num_lbl) - 1;
  rb_ptr = dsatutil_itoa((uint16) dsat707_qcmipgetp_val, rb_ptr, 10);
  *rb_ptr++ = ' ';

  /*-------------------------------------------------------------------------
    Check if the profile is enabled
  -------------------------------------------------------------------------*/
  dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);
  if(ds_nv_item.ds_mip_enable_prof[dsat707_qcmipgetp_val])
  {
    memcpy(rb_ptr, enable_lbl, sizeof(enable_lbl));
    rb_ptr += sizeof(enable_lbl) - 1;
  }
  else
  {
    memcpy(rb_ptr, disable_lbl, sizeof(disable_lbl));
    rb_ptr += sizeof(disable_lbl) - 1 ;
  }
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Get the user profile
  -------------------------------------------------------------------------*/
  ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipgetp_val;
  dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I, &ds_nv_item);

  /*-------------------------------------------------------------------------
    Copy the NAI label and NAI
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, nai_lbl, sizeof(nai_lbl));
  rb_ptr += sizeof(nai_lbl) - 1;


  if(ds_nv_item.ds_mip_gen_user_prof.nai_length != 0)
  {
    (void) std_strlcpy((char *)rb_ptr, 
                       (char *)ds_nv_item.ds_mip_gen_user_prof.nai,
                       ds_nv_item.ds_mip_gen_user_prof.nai_length + 1);
    rb_ptr += ds_nv_item.ds_mip_gen_user_prof.nai_length;
  }
  else
  {
    memcpy(rb_ptr, unset_lbl, sizeof(unset_lbl));
    rb_ptr += sizeof(unset_lbl) - 1 ;
  }

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Copy the Home Addr label and Home Addr
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, ha_lbl, sizeof(ha_lbl));
  rb_ptr += sizeof(ha_lbl) - 1;
  rb_ptr = dsat707_ip_uint32_to_str(
                           ds_nv_item.ds_mip_gen_user_prof.home_addr,rb_ptr);

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Copy the Primary HA label and Primary HA
  --------------------------------------------------------------------------*/
  memcpy(rb_ptr, pri_ha_lbl, sizeof(pri_ha_lbl));
  rb_ptr += sizeof(pri_ha_lbl) - 1;
  rb_ptr = dsat707_ip_uint32_to_str(
    ds_nv_item.ds_mip_gen_user_prof.primary_ha_addr,rb_ptr);
  
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Copy the Secondary HA label and Secondary HA
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, sec_ha_lbl, sizeof(sec_ha_lbl));
  rb_ptr += sizeof(sec_ha_lbl) - 1;
  rb_ptr = dsat707_ip_uint32_to_str(
    ds_nv_item.ds_mip_gen_user_prof.secondary_ha_addr,rb_ptr);

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Copy the MN AAA SPI label and MN AAA SPI
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, aaa_spi_lbl, sizeof(aaa_spi_lbl));
  rb_ptr += sizeof(aaa_spi_lbl) - 1;

  rb_ptr = dsatutil_itoa(ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi,
                         rb_ptr,
                         10);
  
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Copy the MN HA SPI label and MN HA SPI
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, ha_spi_lbl, sizeof(ha_spi_lbl));
  rb_ptr += sizeof(ha_spi_lbl) - 1;

  rb_ptr = dsatutil_itoa(ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi,
                         rb_ptr,
                         10);
  
  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Copy the Reverse Tunelling label and its value
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, rev_tun_lbl, sizeof(rev_tun_lbl));
  rb_ptr += sizeof(rev_tun_lbl) - 1;

  rb_ptr = dsatutil_itoa(ds_nv_item.ds_mip_gen_user_prof.rev_tun_pref,
                         rb_ptr,
                         10);

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  --------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Get the shared secret profile
  -------------------------------------------------------------------------*/

#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
      UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
  {
    /*-----------------------------------------------------------------------
      The MN-AAA SS is set on the card, just set it to a non-zero value
    -----------------------------------------------------------------------*/
    ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length = 1;
    ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length = 1;
  }
  else
#endif /* FEATURE_UIM_SUPPORT_3GPD */
{
  ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipgetp_val;
  dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
} 
  /*-------------------------------------------------------------------------
    Check if the AAA SS length is 0
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, aaa_ss_lbl, sizeof(aaa_ss_lbl));
  rb_ptr += sizeof(aaa_ss_lbl) - 1;

  if(ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length != 0)
  {
    memcpy(rb_ptr, set_lbl, sizeof(set_lbl));
    rb_ptr += sizeof(set_lbl) - 1;
  }
  else
  {
    memcpy(rb_ptr, unset_lbl, sizeof(unset_lbl));
    rb_ptr += sizeof(unset_lbl) - 1;
  }

  /*-------------------------------------------------------------------------
    Copy contents of S3 and S4 registers (typically CR and LF) to the
    response buffer.
  -------------------------------------------------------------------------*/
  *rb_ptr++ = (byte) dsat_s3_val;
  *rb_ptr++ = (byte) dsat_s4_val;

  /*-------------------------------------------------------------------------
    Check if the HA SS length is 0
  -------------------------------------------------------------------------*/
  memcpy(rb_ptr, ha_ss_lbl, sizeof(ha_ss_lbl));
  rb_ptr += sizeof(ha_ss_lbl) - 1;

  if(ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length != 0)
  {
    memcpy(rb_ptr, set_lbl, sizeof(set_lbl));
    rb_ptr += sizeof(set_lbl) - 1;
  }
  else
  {
    memcpy(rb_ptr, unset_lbl, sizeof(unset_lbl));
    rb_ptr += sizeof(unset_lbl) - 1;
  }
  *rb_ptr++ = '\0';
  return rb_ptr;
}/* dsat707_qcmipgetp() */

/*===========================================================================
  FUNCTION DSAT707_FILL_MIP_ATCOP_VALS

  DESCRIPTION
   This function copies the values retreived from NV into atcop_val which is
   used for display.

  DEPENDENCIES
   None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
void dsat707_fill_mip_vals
(
  nv_stat_enum_type (* nv_get_fn)(nv_items_enum_type, nv_item_type *),
  nv_item_type *  nv_item
)
{
  nv_stat_enum_type  nv_get_status;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    This function is used to erase the mip_atcop_val s after the call has
    ended or when QCMIPP is called.
    The profile info is retreived from NV and copied to the mip_atcop_val s
  -------------------------------------------------------------------------*/
  nv_item->ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
  nv_get_status = (*nv_get_fn)(NV_DS_MIP_GEN_USER_PROF_I, nv_item);
  if(nv_get_status != NV_DONE_S)
  {
    MSG_ERROR ("Trouble getting MIP user profile from NV!",0,0,0);
  }

  if(nv_item->ds_mip_gen_user_prof.nai_length != 0 )
  {
    memcpy((char *)dsat707_qcmipnai_val, 
           (char *)nv_item->ds_mip_gen_user_prof.nai, 
           nv_item->ds_mip_gen_user_prof.nai_length);
  }
  else
  {
    memset(dsat707_qcmipnai_val, 0, NV_MAX_NAI_LENGTH);
  }
  dsat707_ha_val        = nv_item->ds_mip_gen_user_prof.home_addr;
  dsat707_pri_ha_val    = nv_item->ds_mip_gen_user_prof.primary_ha_addr;
  dsat707_sec_ha_val    = nv_item->ds_mip_gen_user_prof.secondary_ha_addr;
  dsat707_qcmipmaspi_val[0] = nv_item->ds_mip_gen_user_prof.mn_aaa_spi;
  dsat707_qcmipmaspi_val[1] = 1;
  dsat707_qcmipmhspi_val[0] = nv_item->ds_mip_gen_user_prof.mn_ha_spi;
  dsat707_qcmipmhspi_val[1] = 1;
  dsat707_qcmiprt_val[0]    = nv_item->ds_mip_gen_user_prof.rev_tun_pref;
  dsat707_qcmiprt_val[1]    = 1;
}/* dsat707_fill_mip_vals */


/*===========================================================================
  FUNCTION DS_AT707_MIP_IP

  DESCRIPTION
   This function handles the IP related AT commands of MIP.
   Specifically the function handles the following AT command:
   AT$QCMIPHA, AT$QCMIPPHA, AT$QCMIPSHA

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
dsat_result_enum_type ds_at707_mip_ip
(
  const tokens_struct_type  *tok_ptr,
  byte                      *rb_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ip_val,
  uint32                    *ds_atcop_ip_uint32val,
  uint32                    **shadow_nv_ip
)
{
  dsat707_util_result_type util_status;
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Is it AT$QCMIP_XX_IP?
  -------------------------------------------------------------------------*/
  if(tok_ptr->op == (NA|QU))
  {
    /*-----------------------------------------------------------------------
      rb_ptr contains the IP address in the string format, to be displayed
      on the screen.
    -----------------------------------------------------------------------*/
    rb_ptr = dsat707_ip_uint32_to_str(*ds_atcop_ip_uint32val, rb_ptr);

    *rb_ptr++ = ',';

    if(*shadow_nv_ip != NULL)
    {
      *rb_ptr++ = '0';
    }
    else
    {
      *rb_ptr++ = '1';
    }
    *rb_ptr = '\0';
  }/* if question mark */

  /*-------------------------------------------------------------------------
    Is it AT$QCMIP_XX_IP=?
  -------------------------------------------------------------------------*/
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    dsatutil_fmt_string_range(table_entry, rb_ptr, string_mip_ip_range);
  }

  /*-------------------------------------------------------------------------
    Enter the IP Address Value
  -------------------------------------------------------------------------*/
  else
  {

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*-----------------------------------------------------------------------
    If Using R-UIM, Cannot change IP info.   
    -----------------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
    {
      return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    /*-----------------------------------------------------------------------
      Check the number of arguments of the AT command and set default to 0
      if only one argument
    -----------------------------------------------------------------------*/
    switch(tok_ptr->args_found)
    {
      case 1:
        nv_pref = '0';
        break;

      case 2:
        nv_pref = *(tok_ptr->arg[1]);
        break;

      default:
        MSG_ERROR ("Incorrect number of args found!", 0, 0, 0);
        result = DSAT_ERROR;
        return result;
    }

    /*-----------------------------------------------------------------------
      If the second argument is not 1 or 0 then return an error before
      processing further
    -----------------------------------------------------------------------*/
    if( (nv_pref != '0') && (nv_pref != '1') )
    {
      MSG_ERROR("Second argument must be 0 or 1", 0, 0, 0);
      result = DSAT_ERROR;
      return result;
    }

    util_status= dsat707_strip_quotes( tok_ptr,
                                       (char *)ds_atcop_qcmip_ip_val,
                                       table_entry);
    if(util_status != STRIP_QUOTES_OK)
    {
      result = DSAT_ERROR;
      return result;
    }
    else
    {
      /*---------------------------------------------------------------------
        Convert the string to uint32
      ---------------------------------------------------------------------*/
      util_status = dsat707_ip_str_to_uint32((char *)ds_atcop_qcmip_ip_val,
                                              ds_atcop_ip_uint32val);
      if(util_status != IP_STR_UINT32_OK)
      {
        result = DSAT_ERROR;
        return result;
      }

      /*---------------------------------------------------------------------
        Enter a new HA value
      ---------------------------------------------------------------------*/
      else
      {
        /*-------------------------------------------------------------------
          Depending on the NV commit preference, carry out actions
          accordingly.
        -------------------------------------------------------------------*/
        switch(nv_pref)
        {
          case '0':
            *shadow_nv_ip = ds_atcop_ip_uint32val;
            break;

          case '1':
            /*---------------------------------------------------------------
              Retrieve current active profile from NV.
            ---------------------------------------------------------------*/
            *shadow_nv_ip = NULL;

            ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
            dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

            switch(table_entry->special)
            {
              case SPECIAL_QCMIPHA:
                ds_nv_item.ds_mip_gen_user_prof.home_addr
                  = *ds_atcop_ip_uint32val;
                break;

              case SPECIAL_QCMIPPHA:
                ds_nv_item.ds_mip_gen_user_prof.primary_ha_addr
                  = *ds_atcop_ip_uint32val;
                break;

              case SPECIAL_QCMIPSHA:
                ds_nv_item.ds_mip_gen_user_prof.secondary_ha_addr
                  = *ds_atcop_ip_uint32val;
                break;

              default:
                MSG_ERROR("Unknown AT command",0,0,0);
                result = DSAT_ERROR;
                break;
            }
            
            /*---------------------------------------------------------------
              Write the profile with the new value to NV.
            ---------------------------------------------------------------*/
            nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I,
                                        &ds_nv_item);
            if(nv_status != NV_DONE_S)
            {
              MSG_ERROR("NV Write Error",0,0,0);
              result = DSAT_ERROR;
            }
            break;
					default:
						MSG_ERROR("Invalid NV preference",0,0,0);
						result = DSAT_ERROR;
        }/* switch NV prefernce*/
      }/* else st_to_uint32 success */
    }/* else parse success*/
  }/* else regular processing */
  return result;
} /* ds_atcop_mip_ip */

/*===========================================================================
  FUNCTION DS_AT707_MIP_SS

  DESCRIPTION
   This function handles the Shared Secrets related AT commands of MIP.
   Specifically the AT command handles:
   AT$QCMIPMASS, AT$QCMIPMHSS

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
LOCAL dsat_result_enum_type ds_at707_mip_ss
(
  const tokens_struct_type  *tok_ptr,
  dsm_item_type             *rb_ptr,
  const dsati_cmd_type      *table_entry,
  dsat_string_item_type     *ds_atcop_qcmip_ss_val,
  byte                      **shadow_nv_ss,
  byte                      **shadow_nv_ss_len
)
{
  dsat707_util_result_type util_status;
  dsat_result_enum_type result = DSAT_OK;
  int dsat_qcmip_ss_len = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Processing Question mark. ie AT$QCMIP_XX_SS?
  -------------------------------------------------------------------------*/
  if(tok_ptr->op == (NA|QU))
  {
    /* Check if shadow_nv is null */
    if(
#ifdef FEATURE_UIM_SUPPORT_3GPD
      (UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support()) ||
      (UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()) ||
#endif /* FEATURE_UIM_SUPPORT_3GPD */
	  (*shadow_nv_ss != NULL)
	  )
    {
      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASS:
          (void) std_strlcpy((char *)(rb_ptr->data_ptr), 
                             "$QCMIPMASS: Set", 
                             rb_ptr->size);
          break;

        case SPECIAL_QCMIPMHSS:
          (void) std_strlcpy((char *)(rb_ptr->data_ptr), 
                             "$QCMIPMHSS: Set", 
                             rb_ptr->size);
          break;

        default:
          MSG_ERROR("Unknown AT command",0,0,0);
          result = DSAT_ERROR;
          return result;
      }

    }/* shadow_nv not null */
    else
    {
      ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
      dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);

      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASS:
          if(ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length != 0)
          {
            (void) std_strlcpy((char *)(rb_ptr->data_ptr), 
                               "$QCMIPMASS: Set", 
                               rb_ptr->size);
          }
          else
          {
            (void) std_strlcpy((char *)(rb_ptr->data_ptr), 
                               "$QCMIPMASS: Unset", 
                               rb_ptr->size);
          }
          break;

        case SPECIAL_QCMIPMHSS:
          if(ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length != 0)
          {
            (void) std_strlcpy((char *)(rb_ptr->data_ptr), 
                               "$QCMIPMHSS: Set", 
                               rb_ptr->size);
          }
          else
          {
            (void) std_strlcpy((char *)(rb_ptr->data_ptr), 
                               "$QCMIPMHSS: Unset", 
                               rb_ptr->size);
          }
          break;

        default:
          MSG_ERROR("Unknown AT command",0,0,0);
          result = DSAT_ERROR;
          return result;
      }
    }
  }
  /*-------------------------------------------------------------------------
    Processing Equal to and Question mark.ie AT$QCMIP_XX_SS=?
  -------------------------------------------------------------------------*/
  else if( tok_ptr->op == (NA|EQ|QU) )
  {
    dsatutil_fmt_string_range(table_entry,
                              rb_ptr->data_ptr,
                              mip_string_range);
  }

  /*-------------------------------------------------------------------------
    Processing the remaining cases
  -------------------------------------------------------------------------*/
  else
  {
    /*-----------------------------------------------------------------------
      Check the number of arguments of the AT command and set default to 0
      if only one argument
    -----------------------------------------------------------------------*/

#ifdef FEATURE_UIM_SUPPORT_3GPD
    /*------------------------------------------------------------
      If Using R-UIM, Cannot set SS   
     ------------------------------------------------------------*/
    if((UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         UIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
    {
       return DSAT_ERROR;
    }
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    switch(tok_ptr->args_found)
    {
      case 1:
        nv_pref = '0';
        break;

      case 2:
        nv_pref = *(tok_ptr->arg[1]);
        break;

      default:
        MSG_ERROR ("Incorrect number of args found!", 0, 0, 0);
        result = DSAT_ERROR;
        return result;
    }

    /*-----------------------------------------------------------------------
      If the second argument is not 1 or 0 then return an error before
      processing further
    -----------------------------------------------------------------------*/
    if( (nv_pref != '0') && (nv_pref != '1') )
    {
      MSG_ERROR("Second argument must be 0 or 1", 0, 0, 0);
      result = DSAT_ERROR;
      return result;
    }

    util_status= dsat707_strip_quotes( tok_ptr,
                                       (char *)ds_atcop_qcmip_ss_val,
                                       table_entry );
    if(util_status != STRIP_QUOTES_OK)
    {
      result = DSAT_ERROR;
      return result;
    }
    else
    {
      /*---------------------------------------------------------------------
        Depending on the NV commit preference, carry out actions accordingly.
      ---------------------------------------------------------------------*/
      switch(nv_pref)
      {
        case '0':
          *shadow_nv_ss = ds_atcop_qcmip_ss_val;
          ss_size = (byte) strlen((char *)ds_atcop_qcmip_ss_val);
          *shadow_nv_ss_len = &ss_size;
          break;

        case '1':
          /*-----------------------------------------------------------------
            Retrieve current active profile from NV.
          -----------------------------------------------------------------*/
          *shadow_nv_ss = NULL;

          ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
          dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);

		  dsat_qcmip_ss_len = strlen((char *)ds_atcop_qcmip_ss_val);
          if( dsat_qcmip_ss_len > NV_MAX_MN_AAA_SHARED_SECRET_LEN)
          {
            return DSAT_ERROR;
          }

          switch(table_entry->special)
          {
          case SPECIAL_QCMIPMASS:
              ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length
              = (byte) strlen((char *)ds_atcop_qcmip_ss_val);

              memcpy(
                (char *)ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret,
                ds_atcop_qcmip_ss_val,
                dsat_qcmip_ss_len);
              break;

            case SPECIAL_QCMIPMHSS:
              ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length
              = (byte) strlen((char *)ds_atcop_qcmip_ss_val);

              memcpy(
                (char *)ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret,
                ds_atcop_qcmip_ss_val,
                dsat_qcmip_ss_len);
              break;

            default:
              MSG_ERROR("Unknown AT command",0,0,0);
              result = DSAT_ERROR;
              return result;
          }
          /*-----------------------------------------------------------------
            Write the profile with the new value to NV.
          -----------------------------------------------------------------*/
          nv_status = dsatutil_put_nv_item(NV_DS_MIP_SS_USER_PROF_I, &ds_nv_item);
          if(nv_status != NV_DONE_S)
          {
            MSG_ERROR("NV Write Error",0,0,0);
            result = DSAT_ERROR;
            return result;
          }
          break;
				default:
					MSG_ERROR("Invalid NV preference",0,0,0);
					result = DSAT_ERROR;
			}/* switch NV preference */
    }/* else parse success */
  }/* else regular processing */
  return result;
}/* ds_atcop_mip_ss */

/*===========================================================================
  FUNCTION DS_AT707_MIP_SPI

  DESCRIPTION
   This function handles the SPI related AT commands of MIP.
   Specifically the AT command handles:
   AT$QCMIPMASPI, AT$QCMIPMHSPI

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
dsat_result_enum_type ds_at707_mip_spi
(
  const dsati_cmd_type  *table_entry,
  dsat_num_item_type    *ds_atcop_qcmip_spi_val,
  uint32                **shadow_nv_spi
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(ds_atcop_qcmip_spi_val[1])
  {
    /*-----------------------------------------------------------------------
      Dont Commit
    -----------------------------------------------------------------------*/
    case 0:
      *shadow_nv_spi = (uint32 *) &ds_atcop_qcmip_spi_val[0];
      break;

    /*-----------------------------------------------------------------------
      Commit
    -----------------------------------------------------------------------*/
    case 1:
      *shadow_nv_spi = NULL;

      ds_nv_item.ds_mip_gen_user_prof.index = dsat707_qcmipp_val;
      dsatutil_get_nv_item(NV_DS_MIP_GEN_USER_PROF_I,&ds_nv_item);

      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASPI:
          ds_nv_item.ds_mip_gen_user_prof.mn_aaa_spi_set = TRUE;
          ds_nv_item .ds_mip_gen_user_prof.mn_aaa_spi
            = ds_atcop_qcmip_spi_val[0];
          break;

        case SPECIAL_QCMIPMHSPI:
          ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi_set = TRUE;
          ds_nv_item.ds_mip_gen_user_prof.mn_ha_spi
            = ds_atcop_qcmip_spi_val[0];
          break;

        default:
          MSG_ERROR("Unknown AT command",0,0,0);
          result = DSAT_ERROR;
          return result;
      }
      nv_status = dsatutil_put_nv_item(NV_DS_MIP_GEN_USER_PROF_I, &ds_nv_item);
      if( nv_status != NV_DONE_S )
      {
        MSG_ERROR("Write error",0,0,0);
        result = DSAT_ERROR;
        return result;
      }
      break;

    /*-----------------------------------------------------------------------
      Error
    -----------------------------------------------------------------------*/
    default:
      result = DSAT_ERROR;
      return result;
  }/* switch ()*/
  return result;
}/* ds_atcop_mip_spi */

/*===========================================================================
  FUNCTION DS_AT707_MIP_SSX

  DESCRIPTION
   This function handles the shared secrets (hex format) related AT commands
   of MIP.
   Specifically the AT command handles:
   AT$QCMIPMASSX, AT$QCMIPMHSSX

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
LOCAL dsat_result_enum_type ds_at707_mip_ssx
(
  const dsati_cmd_type  *table_entry,
  dsat_num_item_type    *ds_atcop_qcmip_ssx_val,
  byte                  **shadow_nv_ssx,
  byte                  **shadow_nv_ssx_len
)
{
  dsat_result_enum_type result = DSAT_OK;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check the size of the shared secret value
  -------------------------------------------------------------------------*/
  /*  Check if greater than 2^24 */
  if(ds_atcop_qcmip_ssx_val[0] > 0xFFFFFF)
  {
    ss_size = 4;
  }
  /* Check if greater than 2^16 */
  else if(ds_atcop_qcmip_ssx_val[0] > 0xFFFF)
  {
    ss_size = 3;
  }
  /* Check if greater than 2^8 */
  else if(ds_atcop_qcmip_ssx_val[0] > 0xFF)
  {
    ss_size = 2;
  }
  else
  {
    ss_size = 1;
  }

  switch(ds_atcop_qcmip_ssx_val[1])
  {
    /*-----------------------------------------------------------------------
      Dont Commit
    -----------------------------------------------------------------------*/
    case 0:
      *shadow_nv_ssx = (byte *)&ds_atcop_qcmip_ssx_val[0];
      *shadow_nv_ssx_len = &ss_size;
      break;

    case 1:
      /*---------------------------------------------------------------------
        Retrieve current active profile from NV.
      ---------------------------------------------------------------------*/
      *shadow_nv_ssx = NULL;

      ds_nv_item.ds_mip_ss_user_prof.index = dsat707_qcmipp_val;
      dsatutil_get_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);

      switch(table_entry->special)
      {
        case SPECIAL_QCMIPMASSX:
          ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length =
            ss_size;
         memcpy((byte *)ds_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret,
                &ds_atcop_qcmip_ssx_val[0],
                ss_size);
         break;

        case SPECIAL_QCMIPMHSSX:
          ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length =
            ss_size;
          memcpy((byte *)ds_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret,
                 &ds_atcop_qcmip_ssx_val[0],
                 ss_size);
          break;

        default:
          MSG_ERROR("Unknown AT command",0,0,0);
          result = DSAT_ERROR;
          return result;
      }
      /*---------------------------------------------------------------------
        Write the profile with the new value to NV.
      ---------------------------------------------------------------------*/
      nv_status = dsatutil_put_nv_item(NV_DS_MIP_SS_USER_PROF_I,&ds_nv_item);
      if(nv_status != NV_DONE_S)
      {
        MSG_ERROR("NV Write Error",0,0,0);
        result = DSAT_ERROR;
        return result;
      }
      break;

    /*-----------------------------------------------------------------------
      Error
    -----------------------------------------------------------------------*/
    default:
      result = DSAT_ERROR;
      return result;
  }/* switch ()*/
  return result;
}/* ds_atcop_mip_ssx */


/*===========================================================================
  FUNCTION DSAT707_QCMIP_CHECK_MDR_VAL

  DESCRIPTION
    Function to check if AT$QCMIP and AT$QCMDR are compatible.  
    Call this after changing either value.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsat707_qcmip_check_mdr_val(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check if MDR and QCMIP settings are compatible
  -------------------------------------------------------------------------*/
  if (dsat707_qcmip_val > 0 && dsat707_qcmdr_val != DS_MDR_MODE_SO33_PREF)
  {
    /*-----------------------------------------------------------------------
      If QCMIP is non-0, MDR must be 3 (SO 33 preferred)
    -----------------------------------------------------------------------*/
      return FALSE;
    }
  return TRUE;
} /* dsat707_qcmip_check_mdr_val() */

/*===========================================================================
  FUNCTION DS_ATCOP_MIP_USING_SHADOW_NV

  DESCRIPTION
   Checks if any of the shadow NV values are in use for MIP.

   Called from PS so make sure no inter-task contention issues

  DEPENDENCIES
   None

  RETURN VALUE
    TRUE  - using shadow NV
    FALSE - not using shadow NV

  SIDE EFFECTS
   None
===========================================================================*/
boolean ds_atcop_mip_using_shadow_nv
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* We check for all the fields except reverse tunneling */
  if(shadow_nv.nai        ||
     shadow_nv.mn_aaa_ss  ||
     shadow_nv.mn_ha_ss   ||
     shadow_nv.mn_aaa_spi ||
     shadow_nv.mn_ha_spi  ||
     shadow_nv.ha_ip      ||
     shadow_nv.pri_ha_ip  ||
     shadow_nv.sec_ha_ip
    )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
  FUNCTION DSAT707_QCMIP_UPDATE

  DESCRIPTION
   Called by $QCMIP command and refresh event handler to update
   values associated with NV_DS_QCMIP_I.

  DEPENDENCIES
   None

  RETURN VALUE
    TRUE : if success
    FALSE: if failure

  SIDE EFFECTS
   None
===========================================================================*/
boolean dsat707_qcmip_update
(
  boolean read_flag
)
{
  nv_stat_enum_type status;          /* status from NV call */
  MSG_MED("In function dsat707_qcmip_update",0,0,0);
  if(TRUE == read_flag)
  {
    status = dsatutil_get_nv_item( NV_DS_QCMIP_I, &ds_nv_item);
    if( status == NV_DONE_S)
    {
      dsat707_qcmip_val = ds_nv_item.ds_qcmip;
    }
    else
    {
      MSG_ERROR("Bad NV read status %d for DS QCMIP,pass_fail= FALSE", status, 0, 0 );
      return FALSE;
    }
  }
  dsat707_qcmip_set_mdr_val();
  dsat707_qcmip_set_crm_val();
  ds707_so_pkt_recal();
  return TRUE;
}

/*===========================================================================
  FUNCTION DSAT707_QCMIPP_UPDATE

  DESCRIPTION
   Called by refresh event handler and $QCMIPP command execution
   to update values associated with NV_DS_MIP_ACTIVE_PROF_I.

  DEPENDENCIES
   None

  RETURN VALUE
    TRUE : if success
    FALSE: if failure

  SIDE EFFECTS
   None
===========================================================================*/
boolean dsat707_qcmipp_update
(
  boolean read_write_flag /* flag denoting write/read to NV is required or not*/
)
{
  boolean result= TRUE;
  nv_stat_enum_type status;          /* status from NV call */
  MSG_MED("In function dsat707_qcmipp_update with read_write_flag=%d",
    read_write_flag,0,0);
  /* read the value from NV and update the global variable */
  if(FALSE == read_write_flag)
  {
    status = dsatutil_get_nv_item( NV_DS_MIP_ACTIVE_PROF_I, &ds_nv_item);
    if( status == NV_DONE_S)
    {
      dsat707_qcmipp_val=ds_nv_item.ds_mip_active_prof;
    }
    else
    {
      MSG_ERROR( "Bad NV read status %d for MIP active profile return FASLE", 
         status, 0, 0 );
      return FALSE;
    }
  }
  /*-----------------------------------------------------------------------
    Increment the profile number.
  -----------------------------------------------------------------------*/
  nv_status = dsatutil_get_nv_item(NV_DS_MIP_NUM_PROF_I,&ds_nv_item);

  /*-----------------------------------------------------------------------
    If the atcive profile is greater than the profile number then reset it.
  -----------------------------------------------------------------------*/
  if(ds_nv_item.ds_mip_num_prof <= dsat707_qcmipp_val)
  {
    ds_nv_item.ds_mip_num_prof = dsat707_qcmipp_val + 1;
    dsatutil_put_nv_item(NV_DS_MIP_NUM_PROF_I,&ds_nv_item);
  }
  /*-----------------------------------------------------------------------
    Retrieve the defaults if it was not active, else retrieve the current
    values.
  -----------------------------------------------------------------------*/
  mip_status = dsat707_init_mip_prof();
  if (mip_status != INIT_MIP_PROF_OK)
  {
    MSG_ERROR("Initialization of MIP user profile failed",0,0,0);
    result = FALSE;
  }
  else
  {
    /*---------------------------------------------------------------------
      Copy the retreived values into atcop values for display
    ---------------------------------------------------------------------*/
    dsat707_fill_mip_vals(dsatutil_get_nv_item, &ds_nv_item);

    /*---------------------------------------------------------------------
      Is the profile enabled or disabled?
    ---------------------------------------------------------------------*/
    dsatutil_get_nv_item(NV_DS_MIP_ENABLE_PROF_I,&ds_nv_item);

    dsat707_qcmipep_val = 
        ds_nv_item.ds_mip_enable_prof[dsat707_qcmipp_val];

    /* Write the QCMIPP value into NV if the flag is set */
    if( TRUE == read_write_flag )
    {
      /*---------------------------------------------------------------------
        Write QCMIPP value into NV.
        ---------------------------------------------------------------------*/
      ds_nv_item.ds_mip_active_prof = dsat707_qcmipp_val;
      (void) dsatutil_put_nv_item(NV_DS_MIP_ACTIVE_PROF_I, &ds_nv_item);
    }
    /*-----------------------------------------------------------------
      Once the active profile is changed, erase all the MIP
      shadow settings for the previous active profile
    ----------------------------------------------------------------*/
    dsat707_erase_shadow_nv();

#ifdef FEATURE_DS_MOBILE_IP_DMU
    /*---------------------------------------------------------------------
      Re-encrypt the keys.  Set encrypted to false so RRQ's are
      skipped until encryption is complete.
      Re-using this signal to re-encrypt with new keys; Make sure DMU is
      not in progress,  else currentkeys in use will be messed up.
      ---------------------------------------------------------------------*/
    if (mip_dmu_rand_ready)
    {
      MSG_MED("Changed profile, re-encrypt new keys",0,0,0);
      MIP_RAND_DATA_READY();
    }
    else
    {
      MSG_MED("Skipping pre-encryption - no random data yet",0,0,0);
    }
#endif /* FEATURE_DS_MOBILE_IP_DMU */
  }
  return result;
}

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707   */

