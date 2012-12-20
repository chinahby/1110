/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D  P R O F I L E                        
                        

GENERAL DESCRIPTION
  This module contains AT commands funcions and variables if the main AT 
  commands code is featurized out of the build 

  Copyright (c) 2001 - 2011 by QUALCOMM, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatprofile.c#7 $ $DateTime: 2011/03/31 04:12:21 $ $Author: c_pthoot $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/11   pt      Included "assert.h".
03/05/09   bs      Added support for mmgsdi interface.
01/30/09   sn      Added support to read CTA timer, SCRM, TRTL values from NV.
07/11/08   sn      Changed FEATURE_DS_SIP_MULTIPLE_PROFILE to 
                   FEATURE_DS_MULTIPLE_PROFILES.
06/24/08   sn      Modified to use updated data session function name.
03/20/08   psng    Updated feature macro for multiple sip profile
02/11/08   psng    Added NV Support for multiple sip profileS.
10/15/07 sq/mga/yz Changed ERR_FATAL to MSG_ERROR
11/10/06   an      Fixed compilation errors for 6020
11/01/06   an      Created this file for providing stub functions when AT 
                   commands code is stripped out of the build



*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_STRIP_ATCOP
#include "assert.h"
#include "dsatprofile.h"
#include "ds707.h"
#include "task.h"
#include "dstaski.h"
#include "ps_utils.h"
#include "ds707_so_pkt.h"
#include "err.h"
#include "sio.h"
#include "dsati.h"

#ifdef FEATURE_DS_PZID_HYSTERESIS
#include "ds707_pzid_hyst.h"
#endif /* FEATURE_DS_PZID_HYSTERESIS */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
#include "ds707_data_session_profile.h"
#endif /* FEATURE_DS_MULTIPLE_PROFILES */

#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "nvruimi.h"
#include "gsdi_exp.h"
#include "ps_svc.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */
/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
#define DS707_CTA_DEFAULT_VALUE  30
#define DS707_CTA_LOWER_LIMIT     0
#define DS707_CTA_UPPER_LIMIT   255

#ifdef FEATURE_UIM_SUPPORT_3GPD
#define UIM_SUPPORT_SIMPLE_IP_MASK 0x80
#define UIM_SUPPORT_MOBILE_IP_MASK 0x40
#define UIM_SUPPORT_FALLBACK_MASK 0x20

/* This struct contains UIM related Global Variables for this file */
LOCAL struct   
{
  uim_cmd_type      uim_cmd;
  byte              operational_capabilities;
  uim_rpt_status    report_status;
  byte              cdma_svc_table_buffer[UIM_CDMA_SVC_TABLE_SIZE];
} dsatprofile_uim;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

nv_item_type       ds_nv_item;                  /* actual data item     */

dsat_num_item_type dsat707_cta_val = DS707_CTA_DEFAULT_VALUE;
dsat_num_item_type dsat707_qcqnc_val = DS_QNC_DEFAULT;

#ifdef FEATURE_DS_IS707A_CMUX
/* Look-up table for the numeric value of the fwd mux option. This table  */
/* must contain the same number of items as the "cmux_fwd_valstr" table,  */
/* and the value and order of the items must match exactly.               */
const word dsat707_cmux_fwd_table[15] =
{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 0 };


/* The values stored in the "cmux_val" array are the indices to the fwd &  */
/* rev mux options in the "valstr" tables (which contain strings). Use the */
/* "cmux_fwd_table" & "cmux_rev_table" to lookup the actual numeric value. */
dsat_num_item_type dsat707_cmux_val[2] = {11, 1};

/* Look-up table for the numeric value of the rev mux option. This table  */
/* must contain the same number of items as the "cmux_rev_valstr" table,  */
/* and the value and order of the items must match exactly.               */
const word dsat707_cmux_rev_table[2] =
{ 1, 2 };

#else
dsat_num_item_type dsat707_cmux_val = 2;
#endif

byte dsat707_pkt_orig_str[NV_MAX_PKT_ORIG_DIGITS] = "#777";

#ifdef FEATURE_DS_NET_MODEL
#ifdef FEATURE_JCDMA_1X
dsat_num_item_type dsat707_crm_val = 2;
#else
dsat_num_item_type dsat707_crm_val = 0;
#endif
#else
dsat_num_item_type dsat707_crm_val = 0;
#endif

dsat_num_item_type dsat707_qcmdr_val = DS_MDR_MODE_DEFAULT;
dsat_num_item_type dsat707_qcmip_val = 0;
/* Which s.o. set */
dsat_num_item_type dsat707_qcso_val = DS_SO_SET_DEFAULT;

uint32 dsat_qcdns_val[2] = {0, 0};

extern dsat_num_item_type dsat_ipr_val = 17;

#ifdef FEATURE_IS2000_CHS
/* Enable/disable Mobile-originated Control/Hold */
dsat_num_item_type dsat707_qcchs_val = DS_CHS_DEFAULT;
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_IS2000_R_SCH
/* TURN SCRM on/off */
dsat_num_item_type dsat707_qcscrm_val = DS_SCRM_DEFAULT;

/* TURN R-SCH throttle on/off */
dsat_num_item_type dsat707_qctrtl_val = DS_TRTL_DEFAULT;
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_UIM_SUPPORT_3GPD
LOCAL void dsatprofile_register_cb_func ( void );
LOCAL void dsatprofile_gsdi_event_cb_func 
(
  gsdi_sim_events_T             event            /* Event ID              */
);
LOCAL void dsatprofile_uim_3gpd_op_cap_report
(
  uim_rpt_type *report           /* R-UIM command status report */
);
LOCAL void dsatprofile_uim_operational_capability
(
  void
);
LOCAL void dsatprofile_uim_3gpd_cdma_svc_init_report
(
  uim_rpt_type *report           /* R-UIM command status report */
);
LOCAL void dsatprofile_uim_check_cdma_svc_table
(
  boolean *support_sip, 
  boolean *support_mip,
  boolean *service_3gpd_extensions
);
#endif /* FEATURE_UIM_SUPPORT_3GPD */
/*===========================================================================
FUNCTION      DSAT707_NV_SYNC

DESCRIPTION   This function reads all the 707-specific NV items out of
              NV.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsatprofile_nv_sync(void)
{
  nv_stat_enum_type status;                         /* status from NV call */

  #ifdef FEATURE_IS95B_MDR
  /*-------------------------------------------------------------------------
    Get the MDR Mode from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_MDR_MODE_I, &ds_nv_item);

   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
      At the time of first boot, the NV item may not have been initialized
      yet. Initialize it from the statically initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_mdr_mode = DS_MDR_MODE_DEFAULT;
     dsatutil_put_nv_item(NV_DATA_MDR_MODE_I, &ds_nv_item);
   }
   else if( status != NV_DONE_S )
   {
     MSG_ERROR("Bad NV read status %d for MDR Mode", status, 0, 0);
   }

  /*-------------------------------------------------------------------------
    Store the value retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcmdr_val = ds_nv_item.data_mdr_mode;
  #endif /* FEATURE_IS95B_MDR */

  status = dsatutil_get_nv_item( NV_DS_QCMIP_I, &ds_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /* At time of first boot, the NV item may not have been initialized
       yet.  Initialize it from statically-initialized default. */
    ds_nv_item.ds_qcmip = 0;
    (void) dsatutil_put_nv_item( NV_DS_QCMIP_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR("Bad NV read status %d for DS QCMIP", status, 0, 0 );
  }
  /*-------------------------------------------------------------------------
    Store $qcmip val retrieved from NV in default field, which will
    later (in init_table) be put in dsat707_qcmip_val.
  -------------------------------------------------------------------------*/
  dsat707_qcmip_val = ds_nv_item.ds_qcmip;

  #ifdef FEATURE_DS_CHOOSE_SO
  /*-------------------------------------------------------------------------
    Get the service option selection from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_DATA_SO_SET_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S)
   {
    /*-----------------------------------------------------------------------
      At time of first boot, the NV item may not have been initialized yet.
      Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_so_set = DS_SO_SET_DEFAULT;
     (void) dsatutil_put_nv_item( NV_DATA_SO_SET_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for SO SET", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcso_val = ds_nv_item.data_so_set;

  #endif /* FEATURE_DS_CHOOSE_SO */

  /*-------------------------------------------------------------------------
    Get Primary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_PRIMARY_DNS_I, &ds_nv_item );
  if ( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
     -----------------------------------------------------------------------*/
    dsat_qcdns_val[0] = ds_nv_item.primary_dns;
  }
  else if ( status == NV_NOTACTIVE_S )
  {
    /* NV item not populated; use default */
  }
  else 
  {
    MSG_ERROR( "Bad NV read status %d for Pri DNS NV", status, 0, 0 );
  }

  /*-------------------------------------------------------------------------
    Get Secondary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  
  status = dsi_get_nv_item( NV_SECONDARY_DNS_I, &ds_nv_item );
  if( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
     -----------------------------------------------------------------------*/
    dsat_qcdns_val[1] = ds_nv_item.secondary_dns;
  }
  else if ( status == NV_NOTACTIVE_S )
  {
    /* NV item not populated; use default */
  }
  else 
  {
    MSG_ERROR( "Bad NV read status %d for Sec. DNS NV", status, 0, 0 );
  }

  /*-----------------------------------------------------------------------
  Read the stored Baud rate from the NV. If NV is not written, then use 
  the default values.
  -----------------------------------------------------------------------*/
  status = dsi_get_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
  
  /*-----------------------------------------------------------------------
  Check if the NV has valid data inside and load the data from NV to 
  local item.
  
  If NV is ACTIVE, confirm that the value inside NV is the valid value 
  by calling sio_is_baud_valid()       
  -----------------------------------------------------------------------*/
  if( status == NV_DONE_S)
  {
    if( sio_is_baud_valid((sio_bitrate_type)ds_nv_item.ds_default_baudrate) 
      == TRUE)
    {
      dsat_ipr_val = ds_nv_item.ds_default_baudrate;      
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
    If NV was never written then copy the default values into the ipr_val. 
    -----------------------------------------------------------------------*/
    MSG_HIGH("Baud rate not written to NV", 0, 0, 0);
    
    dsat_ipr_val  = SIO_BITRATE_115200;    
    
    /*-----------------------------------------------------------------------
      Initialize NV from statically-initialized default.
    -----------------------------------------------------------------------*/
    if( status == NV_NOTACTIVE_S )
    {
      ds_nv_item.ds_default_baudrate = dsat_ipr_val;
      (void) dsatutil_put_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
    }
  }

  #ifdef FEATURE_DS_QNC
  /*-------------------------------------------------------------------------
    Get QNC enable flag from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item( NV_DATA_QNC_ENABLED_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_qnc_enabled = dsat707_qcqnc_val;
     (void) dsatutil_put_nv_item( NV_DATA_QNC_ENABLED_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for QNC enable flag", status, 0, 0 );
   }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
   dsat707_qcqnc_val = ds_nv_item.data_qnc_enabled;
   #endif /* FEATURE_DS_QNC */

#ifdef FEATURE_DS_PZID_HYSTERESIS
  /*------------------------------------------------------------------------
    Process the NV items to see if HAT/ HT changed. If they are
    not set in QXDM then use the default values of 30sec for HAT and 
    60sec for HT
  --------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_HYSTERISIS_ACTIVATION_TIMER_I, &ds_nv_item );
  if( status == NV_NOTACTIVE_S ) 
  {
    ds_nv_item.hysterisis_activation_timer = PZID_HYSTERESIS_HAT_TIMER_IN_SEC;
    (void) dsi_put_nv_item( NV_HYSTERISIS_ACTIVATION_TIMER_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S) 
  {
    MSG_ERROR( "Bad NV read status %d for HAT timer", status, 0, 0 );
  }

  /*------------------------------------------------------------------------
     Make sure the value is within the bounds. 
  ------------------------------------------------------------------------*/
  if(ds_nv_item.hysterisis_activation_timer > PZID_HYSTERESIS_HAT_TIMER_IN_SEC)
  {
    ds_nv_item.hysterisis_activation_timer = PZID_HYSTERESIS_HAT_TIMER_IN_SEC;
  }
  ds707_pzid_hysteresis_hat_timer_in_sec = ds_nv_item.hysterisis_activation_timer;

  status = dsi_get_nv_item( NV_HYSTERISIS_TIMER_I, &ds_nv_item );
  if( status == NV_NOTACTIVE_S ) 
  {
     ds_nv_item.hysterisis_timer = PZID_HYSTERESIS_HT_TIMER_IN_SEC;
    (void) dsi_put_nv_item( NV_HYSTERISIS_TIMER_I, &ds_nv_item );
  }
  else if( status != NV_DONE_S) {
    MSG_ERROR( "Bad NV read status %d for HT timer", status, 0, 0 );
  }
  
  /*------------------------------------------------------------------------
     Make sure the value is within the bounds. 
  ------------------------------------------------------------------------*/
  if(ds_nv_item.hysterisis_timer > PZID_HYSTERESIS_HT_TIMER_IN_SEC)
  {
    ds_nv_item.hysterisis_timer = PZID_HYSTERESIS_HT_TIMER_IN_SEC;
  }
  ds707_pzid_hysteresis_ht_timer_in_sec = ds_nv_item.hysterisis_timer;

#endif /* FEATURE_DS_PZID_HYSTERESIS */

   status = dsatutil_get_nv_item( NV_DSAT707_CTA_TIMER_I, &ds_nv_item );
   if( status == NV_NOTACTIVE_S ) 
   {
     ds_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;  
   }
   else if( status != NV_DONE_S) 
   {
     MSG_ERROR( "Bad NV read status %d for CTA timer", status, 0, 0 );
     ds_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;
   }

   if( ds_nv_item.dsat707_cta_timer > DS707_CTA_UPPER_LIMIT )
   {
     ds_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;   
   }
   dsat707_cta_val = ds_nv_item.dsat707_cta_timer;

#if defined(FEATURE_IS2000_R_SCH)
  /*-------------------------------------------------------------------------
    Get the SCRM Mode from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_SCRM_ENABLED_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
      At time of first boot, the NV item may not have been initialized yet.
      Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_scrm_enabled = dsat707_qcscrm_val;
     (void) dsatutil_put_nv_item( NV_DATA_SCRM_ENABLED_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for SCRM mode", status, 0, 0 );
   }

   dsat707_qcscrm_val = ds_nv_item.data_scrm_enabled;

  /*-------------------------------------------------------------------------
    Get the R-SCH throttle (QCTRTL) Mode from NV.
  -------------------------------------------------------------------------*/
   status = dsatutil_get_nv_item(NV_DATA_TRTL_ENABLED_I, &ds_nv_item);
   if( status == NV_NOTACTIVE_S )
   {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
     ds_nv_item.data_trtl_enabled = dsat707_qctrtl_val;
     (void) dsatutil_put_nv_item( NV_DATA_TRTL_ENABLED_I, &ds_nv_item );
   }
   else if( status != NV_DONE_S)
   {
     MSG_ERROR( "Bad NV read status %d for QCTRTL mode", status, 0, 0 );
   }

   dsat707_qctrtl_val = ds_nv_item.data_trtl_enabled;
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_DS_MULTIPLE_PROFILES
   ds707_data_session_read_profile_info();
#endif /* FEATURE_DS_MULTIPLE_PROFILES */  
}


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
  FUNCTION DSAT707_SET_MDR_VAL

  DESCRIPTION
  This function sets the MDR value and synchronizes the available
  service options.
  
  This function can be called from UI, if UI decides to have an interface
  to change the value.
  
  DEPENDENCIES
  None.
  
  RETURN VALUE
  Boolean indicating success/fail.
  
  SIDE EFFECTS
  None.
===========================================================================*/
boolean dsat707_set_mdr_val
(
  unsigned int    qcmdr_val
)
{
  boolean             ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsat707_qcmdr_val = qcmdr_val;

#ifdef FEATURE_DS_MOBILE_IP
  ret_val = dsat707_qcmip_check_mdr_val();

  /*-------------------------------------------------------------------------
    If QCMIP is greater than 0, force MDR to 3.
  -------------------------------------------------------------------------*/
  if (ret_val == FALSE)
  {
    dsat707_qcmdr_val = DS_MDR_MODE_SO33_PREF;
  }
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Re-synchronize available service options
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  return(ret_val);
} /* dsat707_set_mdr_val() */

/*===========================================================================

FUNCTION DSAT_CHANGE_BAUD

DESCRIPTION
  This is a stub function provided. This function does not change the baud rate
  as if the feature FEATURE_DATA_STRIP_ATCOP is defined the baud rate change is
  not supported
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_change_baud
(
    sio_bitrate_type new_baud_rate,     /* Requested baud rate   */
    boolean change_baud_now             /* Immediate or post ack */
)
{
  MSG_HIGH("Cannot change baud", 0, 0, 0);
  return;
}

#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================

FUNCTION  DSATPROFILE_INIT_ME

DESCRIPTION
  ME initialization function. This function does the following:
  - Registers the client call back function for GSDI events,
  - Register as a client of MMGSDI.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatprofile_init_me ( void )
{
  /* Call the registration function to register call back functions */
  dsatprofile_register_cb_func ( );

  return;
}/* dsatprofile_init_me */

/*===========================================================================

FUNCTION  DSATPROFILE_REGISTER_CB_FUNC

DESCRIPTION
  Registers the callback functions.with other tasks.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatprofile_register_cb_func ( void )
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  /* Register the GSDI event callback function */
  gsdi_status = gsdi_reg_callback ( dsatprofile_gsdi_event_cb_func );

  /* This should not happen, raise an error */
  if (gsdi_status != GSDI_SUCCESS)
  {
    MSG_ERROR( "Could not register PIN event callback function with GSDI", 
               0, 0, 0 );
  }

  return;
}/* dsatprofile_register_cb_func */ 

/*===========================================================================
FUNCTION  DSATPROFILE_GSDI_PIN_EVENT_CB_FUNC 

DESCRIPTION
  GSDI PIN status callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
LOCAL void dsatprofile_gsdi_event_cb_func 
(
  gsdi_sim_events_T             event            /* Event ID              */
)
{
  ds_cmd_type * cmd_buf;
    
  /* Filter events of interest to ATCOP */
  switch (event)
  {
  case GSDI_SIM_INIT_COMPLETED:
    if( (cmd_buf = ds_get_cmd_buf()) == NULL)
    {
      ERR_FATAL("No ds command buffer",0,0,0);
    }
    else
    {
      /* send this command to ATCOP for PIN event */
      cmd_buf->hdr.cmd_id = DS_AT_GSDI_INFO_CMD;
      cmd_buf->cmd.gsdi_event.event = event;
      ds_put_cmd(cmd_buf);
    }
    break;

  default:
    /* Ignore event */
    break;
  }
}  /* dsatprofile_gsdi_event_cb_func */

/*===========================================================================

FUNCTION  DSATPROFILE_GSDI_EVENT_HANDLER

DESCRIPTION
  This is the handler function for CDMA specific GSDI events.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatprofile_gsdi_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;
  boolean support_3gpd_sip=FALSE;
  boolean support_3gpd_mip=FALSE;
  boolean service_3gpd_extensions=FALSE;

  ASSERT(NULL != cmd_ptr);

  switch (cmd_ptr->cmd.gsdi_event.event)
  {
  /*---------------------------------------------------------------------------
    In order for 3GPD to initialize properly, Data Services must recieve the 
    following event (even in 1x Only Builds).  We should get this event as long
    as FEATURE_MMGSDI is defined - Therefore, in order for 3GPD to work, MMGSDI
    must be defined
  ----------------------------------------------------------------------------*/
    case GSDI_SIM_INIT_COMPLETED:

      dsatprofile_uim_check_cdma_svc_table(&support_3gpd_sip, &support_3gpd_mip,
                                      &service_3gpd_extensions);
      uim_init_3gpd_preference(support_3gpd_sip, support_3gpd_mip);
      nvruim_data_3gpd_init_extensions_support(service_3gpd_extensions);

      if(UIM_3GPD_MIP_NV_SIP_NV != uim_3gpd_support())
      {
        dsatprofile_nv_sync();
        dsatprofile_uim_operational_capability();
      }
      break;
    
    default:
      MSG_LOW("Unsupported GSDI event: %d",
              cmd_ptr->cmd.gsdi_event.event,0,0);
      break;
  }

  return result;
} /* dsatprofile_gsdi_event_handler */

/*===========================================================================

FUNCTION DSATPROFILE_UIM_3GPD_OP_CAP_REPORT

DESCRIPTION
  Accept status reports from R-UIM and prints out message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatprofile_uim_3gpd_op_cap_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{
  if(report->rpt_status == UIM_PASS)
  {
    MSG_LOW("3GPD Operational Capabilities Set on Card",0,0,0);
  }
  else
  {
    MSG_HIGH("3GPD Operational Capabilities NOT SET ON CARD!",0,0,0);
  }

} /* dsatprofile_uim_3gpd_op_cap_report */


/*===========================================================================

FUNCTION DSATPROFILE_UIM_OPERATIONAL_CAPABILITY

DESCRIPTION
  This function initializes the operational capability elementary file
  on an IS-820 Release A card to indicate 3GPD capabilities supported based
  on the features defined.
  It creates and issues a UIM ACCESS command to write to the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatprofile_uim_operational_capability
(
  void
)
{

 dsatprofile_uim.operational_capabilities =0;
#ifdef FEATURE_DATA_PS
  dsatprofile_uim.operational_capabilities |= UIM_SUPPORT_SIMPLE_IP_MASK;
#ifdef FEATURE_DS_MOBILE_IP
  dsatprofile_uim.operational_capabilities |= UIM_SUPPORT_FALLBACK_MASK;
  dsatprofile_uim.operational_capabilities |= UIM_SUPPORT_MOBILE_IP_MASK;
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_PS */


  dsatprofile_uim.uim_cmd.access_uim.num_bytes   = 1;
  dsatprofile_uim.uim_cmd.access_uim.data_ptr    = &dsatprofile_uim.operational_capabilities;
  dsatprofile_uim.uim_cmd.access_uim.offset      = 0;
  dsatprofile_uim.uim_cmd.access_uim.item        = UIM_CDMA_3GPD_ME3GPDOPC;
  dsatprofile_uim.uim_cmd.access_uim.access      = UIM_WRITE_F;
  dsatprofile_uim.uim_cmd.access_uim.hdr.command = UIM_ACCESS_F;
  dsatprofile_uim.uim_cmd.access_uim.hdr.cmd_hdr.task_ptr
                                      = NULL;
  dsatprofile_uim.uim_cmd.access_uim.hdr.cmd_hdr.sigs
                                      = 0;
  dsatprofile_uim.uim_cmd.access_uim.hdr.cmd_hdr.done_q_ptr
                                      = NULL;
  dsatprofile_uim.uim_cmd.hdr.rpt_function       = dsatprofile_uim_3gpd_op_cap_report;
  dsatprofile_uim.uim_cmd.hdr.protocol           = UIM_CDMA;
  dsatprofile_uim.uim_cmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
  dsatprofile_uim.uim_cmd.access_uim.hdr.slot    = UIM_SLOT_AUTOMATIC;

  uim_cmd( &dsatprofile_uim.uim_cmd );
  return;

} /* dsatprofile_uim_operational_capability */

 /*===========================================================================

FUNCTION dsatprofile_uim_3gpd_cdma_svc_init_report         INTERNAL FUNCTION

DESCRIPTION
  Accept status reports from R-UIM and prints out message for CDMA SVC table 
  check


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void dsatprofile_uim_3gpd_cdma_svc_init_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{ 
  dsatprofile_uim.report_status = report->rpt_status;

  if(report->rpt_status == UIM_PASS)
  {
    MSG_LOW("CDMA service table read",0,0,0);
  }
  else
  {
    MSG_HIGH("Could Not Read CDMA Service Table!",0,0,0);
  }
  (void)rex_set_sigs( &ds_tcb, DS_UIM_CMD_SIG );
} /* dsatprofile_uim_3gpd_cdma_svc_init_report */

/*===========================================================================

FUNCTION dsatprofile_uim_check_cdma_svc_table
                                        
DESCRIPTION
  This function issues a read to the CDMA service table to see if 3GPD is 
  supported on the R-UIM.  It is called during initialization.
  
PARAMETERS

  boolean   *support_flag       a pointer to a boolean type to be set if support
                                is available
  
DEPENDENCIES
  None.

RETURN VALUE

                                                 
SIDE EFFECTS


===========================================================================*/ 
LOCAL void dsatprofile_uim_check_cdma_svc_table(boolean *support_sip, 
                                     boolean *support_mip,
                                     boolean *service_3gpd_extensions)
{
  uim_svc_table_return_type support;

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  if (NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
  {
    *support_sip = FALSE;
    *support_mip = FALSE;
    *service_3gpd_extensions=FALSE;
    return;
  }
  memset(dsatprofile_uim.cdma_svc_table_buffer, 0, UIM_CDMA_SVC_TABLE_SIZE);
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  /* Read the CDMA service table */
  dsatprofile_uim.uim_cmd.access_uim.num_bytes        = UIM_CDMA_SVC_TABLE_SIZE;
  dsatprofile_uim.uim_cmd.access_uim.data_ptr    = dsatprofile_uim.cdma_svc_table_buffer;
  dsatprofile_uim.uim_cmd.access_uim.offset= 0;
  dsatprofile_uim.uim_cmd.access_uim.item             = UIM_CDMA_CDMA_SVC_TABLE;
  dsatprofile_uim.uim_cmd.access_uim.access           = UIM_READ_F;

  dsatprofile_uim.uim_cmd.access_uim.hdr.command                 = UIM_ACCESS_F;
  dsatprofile_uim.uim_cmd.access_uim.hdr.cmd_hdr.task_ptr        = NULL;
  dsatprofile_uim.uim_cmd.access_uim.hdr.cmd_hdr.sigs            = 0;
  dsatprofile_uim.uim_cmd.access_uim.hdr.cmd_hdr.done_q_ptr      = NULL;
  dsatprofile_uim.uim_cmd.hdr.rpt_function       = dsatprofile_uim_3gpd_cdma_svc_init_report;
  dsatprofile_uim.uim_cmd.hdr.protocol           = UIM_CDMA;
  dsatprofile_uim.uim_cmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
  dsatprofile_uim.uim_cmd.access_uim.hdr.slot    = UIM_SLOT_AUTOMATIC;

  /*-----------------------------------------------------------------------
    Send the command to the R-UIM:
    Clear the "command done signal"
    Send the command
    Wait for the command to be done
  -----------------------------------------------------------------------*/
  (void) rex_clr_sigs( rex_self(), DS_UIM_CMD_SIG );

  uim_cmd( &dsatprofile_uim.uim_cmd );
  MSG_MED("BLOCKING DS TASK ON UIM CMD SIGNAL - UIM READ",0,0,0);
  ASSERT(rex_self() == &ds_tcb);
  (void)dsi_wait(DS_UIM_CMD_SIG);
  MSG_MED("UNBLOCKING DS TASK ON UIM CMD SIGNAL - UIM READ",0,0,0);
  if(dsatprofile_uim.report_status == UIM_PASS)
  {
    MSG_HIGH("UIM-READ Success", 0, 0, 0);
  }
  else
  {
    MSG_HIGH("UIM-READ FAILURE", 0, 0, 0);
    /* Failed reading the service table */
    MSG_HIGH("Could not read CDMA Service Table", 0, 0, 0);
   return;
  }

  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_SIP,
                            dsatprofile_uim.cdma_svc_table_buffer);

  if(support.allocated && support.activated)
  {
    *support_sip = TRUE;
  }
  else
  {
    *support_sip = FALSE;
  }
#ifdef FEATURE_UIM_3GPD_MOBILE_IP
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MIP,
                            dsatprofile_uim.cdma_svc_table_buffer);

  if(support.allocated && support.activated)
  {
    *support_mip = TRUE;
  }
  else
  {
    *support_mip = FALSE;
  }
#else
  *support_mip = FALSE;
#endif /* FEATURE_UIM_3GPD_MOBILE_IP */
  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_3GPD_MSG_EXT,
                                               dsatprofile_uim.cdma_svc_table_buffer);
  if(support.allocated && support.activated)
  {
    *service_3gpd_extensions = TRUE;
  }
  else 
  {
    *service_3gpd_extensions = FALSE;
  }
  
  return;
} /* dsatprofile_uim_check_cdma_svc_table */
#endif  /* FEATURE_UIM_SUPPORT_3GPD */

#endif /* FEATURE_DATA_STRIP_ATCOP */
