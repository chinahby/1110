
#ifndef SYS_GW_H
#define SYS_GW_H

/*===========================================================================

              S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the GSM/WCDMA protocol stacks.

Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/sys_gw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/08   rm      Fixing HTORPC warning
10/10/08   aj      fix typo in UMA-GAN Connection Status enumeration
09/18/08   aj      Support for UMA GAN 
07/01/08   rm      Mainlining of MBMS changes
02/19/08   sk      Moved cause.h out of FEATURE_MM_SUPERSET
10/15/07   rn      Added support for UE initiated dormancy
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
03/16/05   ic      Include cmwmsg.h in cm.h directly instead of indirectly 
                   (through sys_gw.h) as cm.h uses MAXNO_FORWARD_TO_NUMBER 
                   that is defined in cmwmsg.h
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
08/29/02   PRK     Featurized include files.
05/07/02   PRK     Initial version.
===========================================================================*/
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MM_SUPERSET))
#include "sm_common.h"      /* Contains Session Management common types    */
#include "sm_cm.h"          /* Contains Sesssion Management - Call Manager */
                            /* interface primitives                        */
#include "rabm_cm.h"        /* Contains Radio Access Bearer Manager -      */
                            /* Call Manager interface primitives           */
#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_MM_SUPERSET*/

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) MACRO definitions.   
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ 2

/* TMGI is a parameter used in 
** MBMS service act req. Gives
** the max length of TMGI.
** TMGI stands for ???
*/
#define SYS_GW_MBMS_MAX_TMGI_LENGTH 6

/* Maximum number of MBMS services
** that can simultaneously exist.
*/
#define SYS_GW_MBMS_MAX_SERVICES 16

/* Maximum length of USZ STR 
** used in GAN display mode.
*/
#define SYS_GW_GAN_USZ_STR_MAX_LENGTH 34


/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) Enums.   
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

typedef enum
{

  SYS_GW_MBMS_SRV_STATUS_NONE = -1,
    /* For internal use only */

  SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_NETWORK_FAILURE,
    /* MBMS service in unavailable due to network failure */

  SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_SETUP_FAILURE,
    /* MBMS service in unavailable due to setup failure on UE side */

  SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER,
    /* MBMS service in unavailable due to some other failure */

  SYS_GW_MBMS_SRV_STATUS_AVAILABLE,
    /* MBMS service in available */

  SYS_GW_MBMS_SRV_STATUS_MONITORED,
    /* MBMS service monitored */

  SYS_GW_MBMS_SRV_STATUS_OTHER,
    /* Reserved for future use */
    
  SYS_GW_MBMS_SRV_STATUS_RESERVED1,
    /* Reserved for future use */
    
  SYS_GW_MBMS_SRV_STATUS_MAX
    /* For internal use only */

} sys_gw_mbms_srv_status_e_type;

/* MBMS mode of service */
typedef enum
{
  SYS_GW_MBMS_SRV_MODE_NONE = -1,
    /*For internal use only */

  SYS_GW_MBMS_SRV_MODE_UNKNOWN,
    /* Service mode unknown */

  SYS_GW_MBMS_SRV_MODE_BROADCAST,
    /* Service mode broadcast */

  SYS_GW_MBMS_SRV_MODE_MULTICAST,
    /* Service mode multicast */

  SYS_GW_MBMS_SRV_MODE_MAX
    /*For internal use only */

}sys_gw_mbms_srv_mode_e_type;

/* MBMS type of service */
typedef enum
{
  SYS_GW_MBMS_SRV_TYPE_NONE = -1,
    /* For internal use only */

  SYS_GW_MBMS_SRV_TYPE_UNKNOWN = 0,
    /* Service type unknown */

  SYS_GW_MBMS_SRV_TYPE_STREAMING,
    /* Service type streaming */

  SYS_GW_MBMS_SRV_TYPE_DOWNLOAD,
    /* Service type download */

  SYS_GW_MBMS_SRV_TYPE_OTHER,
    /* Service type other*/

  SYS_GW_MBMS_SRV_TYPE_MAX
    /* For internal use only */

}sys_gw_mbms_srv_type_e_type;

/* MBMS type of service */
typedef enum
{
  SYS_GW_MBMS_ABORT_NONE = -1,
    /* For internal use only */

  SYS_GW_MBMS_ABORT_ALL,
    /* Abort all */

  SYS_GW_MBMS_ABORT_CONTEXT_ACT,
    /* Abort context activation */

  SYS_GW_MBMS_ABORT_CONTEXT_DEACT,
    /* Abort context deactivation */

  SYS_GW_MBMS_ABORT_TRAFFIC_ACT,
    /* Abort traffic activation*/

  SYS_GW_MBMS_ABORT_TRAFFIC_DEACT,
    /* Abort traffic deactivation */

  SYS_GW_MBMS_ABORT_MAX
    /* For internal use only */
}sys_gw_mbms_abort_type_e_type;


/* Enum for MBMS context activation deactivation failure reason
*/
typedef enum
{
  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_NONE = -1,
    /* For internal use only */

  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_UNKNOWN,
    /* Unknown reason*/

  SYS_GW_MBMS_CONTEXT_FAILURE_NO_NETWORK_RESPONSE,
    /* No network response*/

  SYS_GW_MBMS_CONTEXT_FAILURE_INVALID_PDP_CONTEXT_STATE,
    /* Invalid PDP context state*/

  SYS_GW_MBMS_CONTEXT_FAILURE_PDP_CONTEXT_ERROR,
    /* PDP context error*/

  SYS_GW_MBMS_CONTEXT_FAILURE_APN_ERROR,
    /* APN error*/

  SYS_GW_MBMS_CONTEXT_FAILURE_INSUFFICIENT_RESOURCES,
    /* Insufficient resources*/

  SYS_GW_MBMS_CONTEXT_FAILURE_AUTHENTICATION_ERROR,
    /*  Authentication error*/

  SYS_GW_MBMS_CONTEXT_FAILURE_ACT_DEACT_REJECTED,
    /* Activate/Deactivate rejected */

  SYS_GW_MBMS_CONTEXT_FAILURE_DUPLICATE_MBMS,
    /* Duplicate MBMS*/

  SYS_GW_MBMS_CONTEXT_FAILURE_CONTEXT_SYNC_ERROR,
    /* Context sync error*/

  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_OTHER,
    /* Other */

  SYS_GW_MBMS_CONTEXT_FAILURE_REASON_MAX
    /* For internal use only */
}sys_gw_mbms_context_failure_reason_e_type;

/* Type that identifies the specific MBMS request sent by
** the client. 
*/
typedef enum 
{
  /* FOR INTERNAL USE ONLY!
  */
  SYS_GW_MBMS_REQ_NONE = -1,

  /* MBMS Service Activate command  
  */
  SYS_GW_MBMS_REQ_ACTIVATE,

  /* MBMS Service Deactivate command 
  */
  SYS_GW_MBMS_REQ_DEACTIVATE,

  /* MBMS Session Complete Ind 
  */
  SYS_GW_MBMS_REQ_SESSION_COMPLETE_IND,

  /* MBMS Abort Req  (to abort MBMS contex activation procedure in NAS)
  */
  SYS_GW_MBMS_REQ_ABORT,

  /* MBMS Service Suspend command  
  */
  SYS_GW_MBMS_REQ_SUSPEND,

  /* MBMS Service Resume command  
  */
  SYS_GW_MBMS_REQ_RESUME,

  /* MBMS Available Services List Request 
  */
  SYS_GW_MBMS_REQ_AVAIL_SERVICES_LIST,

  /* MBMS Capability enquiry
  */
  SYS_GW_MBMS_REQ_GET_CAPABILITY ,

  /* MBMS Set Attribute 
  */
  SYS_GW_MBMS_REQ_SET_ATTR ,

  /* FOR INTERNAL USE ONLY!
  */
  SYS_GW_MBMS_REQ_MAX

} sys_gw_mbms_req_e_type;


/* MBMS report type */
typedef enum 
{
  /* FOR INTERNAL USE ONLY!
  */
  SYS_GW_MBMS_RESP_NONE = -1,

  /* MBMS Service Activate Cnf 
  */
  SYS_GW_MBMS_RESP_ACTIVATE_CNF ,

  /* MBMS Service Deactivate Cnf  
  */
  SYS_GW_MBMS_RESP_DEACTIVATE_CNF ,

  /* MBMS Service Ind (change in status)
  */
  SYS_GW_MBMS_RESP_SERVICE_IND ,

  /* MBMS Capability RPTication - from RRC, NAS 
  */
  SYS_GW_MBMS_RESP_CAPABILITY_IND ,

  /* MBMS Context Act Ind - from NAS after it received TMGI for a service 
  */
  SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND ,

  /* MBMS Context Deact Ind (MBMS context deactivation, PDP context 
  ** deactivation, etc)  - from NAS 
  */
  SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND ,

  /* MBMS Abort cnf
  */
  SYS_GW_MBMS_RESP_ABORT_CNF,

  /* MBMS Service Suspend Cnf 
  */
  SYS_GW_MBMS_RESP_SUSPEND_CNF ,

  /* MBMS Service Resume Cnf 
  */
  SYS_GW_MBMS_RESP_RESUME_CNF ,

  /* MBMS Available Services List Cnf 
  */
  SYS_GW_MBMS_RESP_AVAIL_SERVICES_LIST_CNF ,

  /* MBMS Available Services List Cnf 
  */
  SYS_GW_MBMS_RESP_LL_READY_IND ,

  /* MBMS Rpt Set Attribute
  */
  SYS_GW_MBMS_RESP_SET_ATTR ,

  /* FOR INTERNAL USE ONLY!
  */
  SYS_GW_MBMS_RESP_MAX

} sys_gw_mbms_resp_e_type;

/**
*  Enumeration of UMA-GAN client states
*/

typedef enum sys_gw_gan_state_e {

  SYS_GW_GAN_STATE_NONE = -1,               
  /**< @internal */

  SYS_GW_GAN_STATE_INIT,                 
  /**< GAN client is initialized 
  */
                                  
  SYS_GW_GAN_STATE_SHUTDOWN,
  /**< GAN client shutdown 
  */

  SYS_GW_GAN_STATE_MAX 
   /**< @internal */
  
} sys_gw_gan_state_e_type;


/**
** Enumeration of UMA-GAN signal channel status
*/

typedef enum sys_gw_gan_sig_chan_status_e {

  SYS_GW_GAN_SIG_CHAN_STATUS_NONE = -1,               
  /**< @internal */

  SYS_GW_GAN_SIG_CHAN_STATUS_UP,                 
  /**< GAN service is possible pending a successful 
  ** GAN registration procedure. 
  */
                                  
  SYS_GW_GAN_SIG_CHAN_STATUS_DOWN,
  /**< MS can not attempt to register with GAN Client and 
  ** no GAN service is possible. 
  */

  SYS_GW_GAN_SIG_CHAN_STATUS_MAX 
   /**< @internal */
  
} sys_gw_gan_sig_chan_status_e_type;

/** 
** Type for UMA-GAN Connection Status 
*/

typedef enum sys_gw_gan_conn_status_e {

  SYS_GW_GAN_CONN_STATUS_NONE = -1,
  /**< @internal*/
 
  /* 0 */
  SYS_GW_GAN_CONN_STATUS_AP_MS_NOT_ASSOCIATED = 0 ,
    /**< AP MS NOT ASSOCIATED */

  SYS_GW_GAN_CONN_STATUS_AP_MS_ASSOCIATED    = SYS_BM(1),
    /**< AP MS ASSOCIATED */

  SYS_GW_GAN_CONN_STATUS_CS_ROVE_IN_STARTED  = SYS_BM(2),
    /**< CS ROVE IN STARTED */
  
  SYS_GW_GAN_CONN_STATUS_PS_ROVE_IN_STARTED  = SYS_BM(3),
    /**< PS ROVE IN STARTED */

  /* 4 */
  SYS_GW_GAN_CONN_STATUS_CS_HAND_IN_STARTED  = SYS_BM(4),
    /**< CS HAND IN STARTED */

  SYS_GW_GAN_CONN_STATUS_PS_HAND_IN_STARTED  = SYS_BM(5),
    /**< PS HAND IN STARTED*/

  SYS_GW_GAN_CONN_STATUS_CS_ROVE_OUT_STARTED = SYS_BM(6),
    /**< CS ROVE OUT STARTED*/

  SYS_GW_GAN_CONN_STATUS_PS_ROVE_OUT_STARTED = SYS_BM(7),
    /**< PS ROVE OUT STARTED */

   SYS_GW_GAN_CONN_STATUS_CS_HAND_OUT_STARTED =SYS_BM(8),
    /**< CS HAND OUT STARTED */

  SYS_GW_GAN_CONN_STATUS_PS_HAND_OUT_STARTED = SYS_BM(9),
    /**< PS HAND OUT STARTED*/

  SYS_GW_GAN_CONN_STATUS_CS_HAND_OUT_OVER   = SYS_BM(10),
    /**< CS HAND OUT OVER */

  SYS_GW_GAN_CONN_STATUS_PS_HAND_OUT_OVER   = SYS_BM(11),
    /**< PS HAND OUT OVER */

  SYS_GW_GAN_CONN_STATUS_MAX
  /** @internal */

} sys_gw_gan_conn_status_e_type;


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/**
*  Type for UMA-GAN Mode Display
*  Describes if WLAN or GERAN is active RAT
*  and the parameters if WLAN is the active RAT
*/

typedef struct sys_gw_gan_mode_display_s{

  boolean                                         is_mode_display_on;
  /**< if is_mode_display_on is set to TRUE, WLAN is the active RAT
  ** if it is FALSE, GERAN is the active RAT
  */

  /**< The parameters below are valid only if 
  ** is_mode_display_on = TRUE
  */

  uint8                                           gan_usz_icon_indicator;
     /**< GAN USZ icon indicator */

  uint8                                           gan_ms_location_status;
     /**< GAN MS location status  */

  uint8                                           usz_str[SYS_GW_GAN_USZ_STR_MAX_LENGTH];
     /**< GAN USZ string */

}sys_gw_gan_mode_display_s_type;

/*
** Type for UMA-GAN Error prompt
*/

typedef struct sys_gw_gan_error_s{

  uint8                                           gan_error_category;
     /**< GAN error category*/

  uint8                                           gan_error_codepoint;
     /**< GAN specific error
     **  within the category  
     */

} sys_gw_gan_error_s_type;



#if (defined(FEATURE_MODEM_MBMS) || defined(FEATURE_MM_SUPERSET))

/* MBMS logical channel id 
*/
typedef struct
{
  /* TRUE  -  Log chan id is present
  ** FALSE -  Log chan id is not present
  */
  boolean present;

  /* Logical channel id. 
  ** Used between DS and RLC on traffic plane.
  */
  uint32  log_chan_id;

}sys_gw_mbms_log_chan_id_s_type;


/* MBMS session id */
typedef struct
{
  /* TRUE  -  Session id is present
  ** FALSE -  Session id is not present
  */
  boolean present;

  /* Session id_type */
  uint32  session_id;

}sys_gw_mbms_session_id_s_type;

/* TMGI structure in int format */
typedef struct
{ 

  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* TMGI in uint64 format */
  uint64  tmgi;

}sys_gw_mbms_tmgi_uint_s_type;


/* TMGI structure in array format */
typedef struct
{
  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* Length of TMGI */
  uint8   length;

  /* Actual TMGI value */
  uint8   value[SYS_GW_MBMS_MAX_TMGI_LENGTH];

}sys_gw_mbms_tmgi_s_type;


/* MBMS service priority */
typedef struct
{
  /* TRUE  -  Priority is present
  ** FALSE -  Priority is not present
  */
  boolean present;

  /* Priority of MBMS service request */
  uint32   priority;

}sys_gw_mbms_srv_priority_s_type;


/*
** Struct to hold parameters for MBMS activate request for each service
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Multicast IP*/
  sys_ip_address_s_type            mip;

  /* Port number */
  uint32                           port;

  /* Service Priority */  
  sys_gw_mbms_srv_priority_s_type     priority; 

  /* Service Mode */
  sys_gw_mbms_srv_mode_e_type         srv_mode;

  /* Service Type */
  sys_gw_mbms_srv_type_e_type         srv_type;

  /* Generic parameter */
  uint32                           mbms_parm;

}sys_gw_mbms_activate_req_entry_s_type;


/* Struct to hold MBMS activate request parameters for num_services elements
*/
typedef struct 
{
  /* Number of services in the array */
  uint8                                  num_services;

  /* Services that need to be activated */
  sys_gw_mbms_activate_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];

}sys_gw_mbms_activate_req_s_type;

/* Struct to hold MBMS deactivate req parameters
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Generic parameter */
  uint32                           mbms_parm;        

}sys_gw_mbms_deactivate_req_entry_s_type;

/* Struct to hold MBMS deactivate req parameters for "num_services" MBMS 
** services
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will deactivate 
  ** all active MBMS services 
  */
  uint8                                    num_services;

  /* Services that need to be de-activated */
  sys_gw_mbms_deactivate_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_deactivate_req_s_type;

/* Struct to hold parameters for MBMS session complete ind
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Completed Session id */
  sys_gw_mbms_session_id_s_type       session_id;

  /* Generic parameter */
  uint32                           mbms_parm;
      
}sys_gw_mbms_session_complete_ind_s_type;

/* Struct to hold parameters for MBMS abort req
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Multicast IP */
  sys_ip_address_s_type            mip;

  /* Port number */
  uint32                           port;

  /* Abort type */
  sys_gw_mbms_abort_type_e_type       abort_type; 

  /* Generic parameter */
  uint32                           mbms_parm;
      
}sys_gw_mbms_abort_req_entry_s_type;

/* To hold Abort req parameters for "num_services" MBMS services.
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will abort 
  ** all active MBMS services 
  */
  uint8                               num_services;

  /* Services list */
  sys_gw_mbms_abort_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
      
}sys_gw_mbms_abort_req_s_type;

/* MBMS suspend request
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Generic parameter */
  uint32                           mbms_parm;
      
}sys_gw_mbms_suspend_req_entry_s_type;

/* Struct to hold parameters for MBMS suspend req cmd.
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will suspend 
  ** all active MBMS services 
  */
  uint8                                 num_services;

  /* Services list */
  sys_gw_mbms_suspend_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
      
}sys_gw_mbms_suspend_req_s_type;

/* MBMS resume request
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Generic parameter */
  uint32                           mbms_parm;

}sys_gw_mbms_resume_req_entry_s_type;

/* Struct to hold parameters for MBMS resume req cmd.
*/
typedef struct 
{
  /* Number of services in the array. Setting this to 0xFF will resume 
  ** all suspended MBMS services 
  */
  uint8                                num_services;

  /* Services list */
  sys_gw_mbms_resume_req_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];
      
}sys_gw_mbms_resume_req_s_type;

/* Struct to hold parameters for MBMS available services list request command
*/
typedef struct 
{
  /* Placeholder for future use */
  uint32 mbms_parm;

}sys_gw_mbms_avail_srv_list_req_s_type;


/* Struct for clients to query MBMS capability
*/
typedef struct 
{
  /* Placeholder for future use */
  uint32 mbms_parm;

} sys_gw_mbms_capability_req_s_type;

/* Generic MBMS command
*/
typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;
  uint32 mbms_parm2;
  uint32 mbms_parm3;
  uint32 mbms_parm4;
  uint32 mbms_parm5;
  uint32 mbms_parm6;

}sys_gw_mbms_set_attr_req_s_type;

/* MBMS activate cnf
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Multicast IP*/
  sys_ip_address_s_type            mip;

  /* Port number */
  uint32                           port;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean                          cnf_status;

  /* Service status */
  sys_gw_mbms_srv_status_e_type       srv_status;

  /* Logical channel id that was added */
  sys_gw_mbms_log_chan_id_s_type      log_chan_id;

  /* Generic parameter */
  uint32                           mbms_parm;        

}sys_gw_mbms_resp_activate_cnf_entry_s_type;

/* Struct to hold parameters for MBMS activate conf report
*/
typedef struct 
{
  /* Number of services in the array */
  uint8                                  num_services;

  /* Services list */
  sys_gw_mbms_resp_activate_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];        

}sys_gw_mbms_resp_activate_cnf_s_type;


typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Multicast IP*/
  sys_ip_address_s_type            mip;

  /* Port number */
  uint32                           port;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean                          cnf_status;

  /* Service status */
  sys_gw_mbms_srv_status_e_type       srv_status;

  /* Logical channel id that was removed */
  sys_gw_mbms_log_chan_id_s_type      log_chan_id;

  /* Generic parameter */
  uint32                           mbms_parm;        

}sys_gw_mbms_resp_deactivate_cnf_entry_s_type;

/* Struct to hold parameters for MBMS deactivate cnf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8                                   num_services;

  /* Services list */
  sys_gw_mbms_resp_deactivate_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_resp_deactivate_cnf_s_type;

/* MBMS Service Indication
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Multicast IP*/
  sys_ip_address_s_type            mip;

  /* Port number */
  uint32                           port;

  /* Logical channel id that was added/removed */
  sys_gw_mbms_log_chan_id_s_type      log_chan_id;

  /* Service status */
  sys_gw_mbms_srv_status_e_type       srv_status;

  /* Generic parameter */
  uint32                           mbms_parm;        

}sys_gw_mbms_resp_service_ind_entry_s_type;

/* Struct to hold parameters for service indication report
*/
typedef struct sys_gw_mbms_resp_service_ind_s
{
  /* Number of services in the array */
  uint8                                 num_services;

  /* Services list */
  sys_gw_mbms_resp_service_ind_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_resp_service_ind_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD sys_gw_mbms_resp_service_ind_s.services VARRAY SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ
    LENGTH sys_gw_mbms_resp_service_ind_s.num_services */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* Struct to hold parameters for capability indication
*/
typedef struct 
{
  /* Indication that core network is MBMS capable */
  boolean network_capability;

  /* Indication that cell is MBMS capable */
  boolean cell_capability;

  /* Generic parameter */
  uint32 mbms_parm;

}sys_gw_mbms_resp_capability_ind_s_type;

/* Struct to hold parameters for Context Activated Indication report
*/
typedef struct 
{
  /* TMGI reported by NAS for MIP specified below */
  sys_gw_mbms_tmgi_uint_s_type              tmgi;

  /* Multicast IP*/
  sys_ip_address_s_type                  mip;

  /* Port number */
  uint32                                 port;

  /* Context activation status. TRUE is successful, FALSE otherwise */
  boolean                                act_status;

  /* Failure reason of context activation req  if act_status is FALSE */
  sys_gw_mbms_context_failure_reason_e_type context_failure_reason;   

}sys_gw_mbms_resp_context_activated_ind_s_type;

/* Struct to hold parameters for context deactivation
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type               tmgi;

  /* Multicast IP*/
  sys_ip_address_s_type                   mip;

  /* Port number */
  uint32                                  port;

  /* Context de-activation status. TRUE is successful, FALSE otherwise */
  boolean                                 deact_status;

  /* Failure reason of context deactivation req. Valid if deact_status is
  ** is FALSE
  */
  sys_gw_mbms_context_failure_reason_e_type  context_failure_reason;   

}sys_gw_mbms_resp_context_deactivated_ind_s_type;

/* MBMS Abort cnf
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi_uint;

  /* Multicast IP*/
  sys_ip_address_s_type            mip;

  /* Port number */
  uint32                           port;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean                          cnf_status;

}sys_gw_mbms_resp_abort_cnf_entry_s_type;

/* Struct to hold parameters for MBMS Abort conf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8                               num_services;
 
  /* Services list */
  sys_gw_mbms_resp_abort_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_resp_abort_cnf_s_type;

/* MBMS suspend cnf
*/
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean                          cnf_status;

  /* Generic parameter */
  uint32                           mbms_parm;

}sys_gw_mbms_resp_suspend_cnf_entry_s_type;


/* Struct to hold parameters for MBMS suspend cnf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8 num_services;

  /* Services list */
  sys_gw_mbms_resp_suspend_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_resp_suspend_cnf_s_type;

/* MBMS resume cnf */
typedef struct 
{
  /* TMGI of service */
  sys_gw_mbms_tmgi_uint_s_type        tmgi;

  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean                          cnf_status;

  /* Service status after resumption */
  sys_gw_mbms_srv_status_e_type       srv_status;

  /* Generic parameter */
  uint32                           mbms_parm;

}sys_gw_mbms_resp_resume_cnf_entry_s_type;

/* Struct to hold parameters for MBMS resume cnf
*/
typedef struct 
{
  /* Number of services in the array */
  uint8                                num_services;

  /* Services list */
  sys_gw_mbms_resp_resume_cnf_entry_s_type services[SYS_GW_MBMS_NUM_MAX_SRVS_PER_REQ];      

}sys_gw_mbms_resp_resume_cnf_s_type;



/*
Struct to hold parameters for Available services list cnf
*/
typedef struct 
{
  /* Status of req - TRUE indicates success, FALSE indicates failure */
  boolean                          cnf_status;

  /* List of service ids that are available in the cell currently */
  uint32                           service_id[SYS_GW_MBMS_MAX_SERVICES];

  /* Generic parameter */
  uint32                           mbms_parm;

}sys_gw_mbms_resp_avail_srv_list_cnf_s_type;

typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;
  uint32 mbms_parm2;
  uint32 mbms_parm3;
  uint32 mbms_parm4;
  uint32 mbms_parm5;
  uint32 mbms_parm6;

}sys_gw_mbms_resp_set_attr_s_type;

/* Type to indicate the Lower Layer (RRC) is in a READY (IDLE) state to 
** service MBMS requests
*/
typedef struct 
{
  /* Generic parameters */
  uint32 mbms_parm1;

} sys_gw_mbms_resp_ll_ready_ind_s_type;

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
FUNCTION      sys_gw_mbms_conv_tmgi_to_tmgi_uint64

DESCRIPTION   Given the TMGI in array format, converts it to TMGI in uint64 
              format.

              Index 0 in the array becomes the MSB of the uint64 value.
              For eg:-
              
              Input: tmgi->value[0] = 0x01, tmgi->value[1] = 0x02, ...
              Output: tmgi_uint->tmgi = 0x0102...
 
              
PARAMETERS    tmgi      -- TMGI from Network. TMGI is character array of digits.
              tmgi_uint -- TMGI in uint64 format. 


DEPENDENCIES  NONE

RETURN VALUE  TRUE- if TMGI passed is correct.
              FALSE - if TMGI passed is incorrect.

SIDE EFFECTS  None.
===========================================================================*/

extern boolean sys_gw_mbms_conv_tmgi_to_tmgi_uint64
(
  sys_gw_mbms_tmgi_s_type *tmgi,
  sys_gw_mbms_tmgi_uint_s_type *tmgi_uint
);


/*===========================================================================
MACRO         sys_gw_mbms_conv_tmgi_uint64_to_tmgi

DESCRIPTION   Given the TMGI in the UINT64 format , converts it to an array 
              of bytes
              MSB of the input is stored in index 0 of the output array. 
              For eg:-

              Input: tmgi_uint->tmgi = 0x0102...
              Output: tmgi->value[0] = 0x01, tmgi->value[1] = 0x02 and so on.

PARAMETERS    tmgi      -- TMGI in uint64 format
              tmgi_uint -- TMGI in array format


DEPENDENCIES  

RETURN VALUE  TRUE- if TMGI passed is correct.
              FALSE - if TMGI passed is incorrect.

SIDE EFFECTS  None.
===========================================================================*/

extern boolean sys_gw_mbms_conv_tmgi_uint64_to_tmgi
(
  sys_gw_mbms_tmgi_uint_s_type *tmgi_uint,
  sys_gw_mbms_tmgi_s_type *tmgi
);

#ifdef __cplusplus
}
#endif

#else /* (defined(FEATURE_MODEM_MBMS) || defined(FEATURE_MM_SUPERSET)) */

typedef uint8  sys_gw_mbms_log_chan_id_s_type;
typedef uint8  sys_gw_mbms_session_id_s_type;
typedef uint8  sys_gw_mbms_tmgi_uint_s_type;
typedef uint8  sys_gw_mbms_tmgi_s_type;
typedef uint8  sys_gw_mbms_srv_priority_s_type;
typedef uint8  sys_gw_mbms_activate_req_entry_s_type;
typedef uint8  sys_gw_mbms_activate_req_s_type;
typedef uint8  sys_gw_mbms_deactivate_req_entry_s_type;
typedef uint8  sys_gw_mbms_deactivate_req_s_type;
typedef uint8  sys_gw_mbms_session_complete_ind_s_type;
typedef uint8  sys_gw_mbms_abort_req_entry_s_type;
typedef uint8  sys_gw_mbms_abort_req_s_type;
typedef uint8  sys_gw_mbms_suspend_req_entry_s_type;
typedef uint8  sys_gw_mbms_suspend_req_s_type;
typedef uint8  sys_gw_mbms_resume_req_entry_s_type;
typedef uint8  sys_gw_mbms_resume_req_s_type;
typedef uint8  sys_gw_mbms_avail_srv_list_req_s_type;
typedef uint8  sys_gw_mbms_capability_req_s_type;
typedef uint8  sys_gw_mbms_set_attr_req_s_type;
typedef uint8  sys_gw_mbms_resp_activate_cnf_entry_s_type;
typedef uint8  sys_gw_mbms_resp_activate_cnf_s_type;
typedef uint8  sys_gw_mbms_resp_deactivate_cnf_entry_s_type;
typedef uint8  sys_gw_mbms_resp_deactivate_cnf_s_type;
typedef uint8  sys_gw_mbms_resp_service_ind_entry_s_type;
typedef uint8  sys_gw_mbms_resp_service_ind_s_type;
typedef uint8  sys_gw_mbms_resp_capability_ind_s_type;
typedef uint8  sys_gw_mbms_resp_context_activated_ind_s_type;
typedef uint8  sys_gw_mbms_resp_context_deactivated_ind_s_type;
typedef uint8  sys_gw_mbms_resp_abort_cnf_entry_s_type;
typedef uint8  sys_gw_mbms_resp_abort_cnf_s_type;
typedef uint8  sys_gw_mbms_resp_suspend_cnf_entry_s_type;
typedef uint8  sys_gw_mbms_resp_suspend_cnf_s_type;
typedef uint8  sys_gw_mbms_resp_resume_cnf_entry_s_type;
typedef uint8  sys_gw_mbms_resp_resume_cnf_s_type;
typedef uint8  sys_gw_mbms_resp_avail_srv_list_cnf_s_type;
typedef uint8  sys_gw_mbms_resp_set_attr_s_type;
typedef uint8  sys_gw_mbms_resp_ll_ready_ind_s_type;

#endif /* (defined(FEATURE_MODEM_MBMS) || defined(FEATURE_MM_SUPERSET)) */

/* --------------------------------------------------------------------------
 Enumeration that identifies the status of ps signal release. It has to be identical to
 struct ps_sig_rel_status_T in sm_cm.h because the values coming from NAS is mapped 
 to this by CM
** ------------------------------------------------------------------------*/
typedef enum
{
   PS_SIG_REL_SUCCESS,
   PS_SIG_REL_FAILURE
} cm_ps_sig_rel_status_T;

#endif /* #ifndef SYS_GW_H */
