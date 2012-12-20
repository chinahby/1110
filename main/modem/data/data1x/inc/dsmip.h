#ifndef _DSMIP_H
#define _DSMIP_H

/*===========================================================================

                         D S M I P . H

DESCRIPTION

 The Data Services Mobile IP configuration source file.

EXTERNALIZED DATA
   mip_ses_info_array       -  MIP session info array
   mip_fa_info              -  MIP foreign agent info
   mip_pwd_info             -  holds MIP shared secrets entered via UI
   mip_get_session_info_cb  -  callback registered by ui which provides
                               session info for next call
   mip_get_pwd_cb           -  callback registered by ui which provides
                               password info for next call

EXTERNALIZED FUNCTIONS
   mip_config_session()
     Load MIP session defaults from NV to all sessions.
   mip_reg_ses_info_cb()
     Register a callback to fill in MIP session information
   mip_reg_pwd_info_cb()
     Register a callback to fill in passwords for MIP session
   mip_get_rm_nai()
     Retrieves the NAIs for a given MIP profile for the
     tethered device and stores them in the mip session info.  

Copyright (c) 2000-2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.h_v   1.9   10 Oct 2002 15:54:16   jayanthm  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/dsmip.h#4 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
04/29/09    sn     Merged MIP event registration support and Ported support 
                   for call throttle feature (DCTM).
04/16/09    sn     Merged support for separate NAI for Tethered SIP and MIP 
                   calls. Featurized under FEATURE_DS_MULTIPLE_NAI.
06/11/06    as     Added MIP_DLFT_PROFILE_NUM for use by otasp
04/28/04    jd     Added definitions for shared secret lengths, linted file
06/25/03    jd     Mainlined FEATURE_DS_IS_835_B since all targets using this
                   archive enable the feature.
06/11/03    jd     Added RTT estimate to rsm_info
10/09/02    jay    Moved #defines from resgsm.h
08/16/02    jay    Created a bit field for various MIP related test AT
                   command
07/18/02    jay    Added DMU flag to the MIP session
04/29/02    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================

                             PUBLIC CONSTANTS

===========================================================================*/

/*---------------------------------------------------------------------------
  MIP_MAX_SESSIONS is the number of MIP sessions allowed to run
  concurrently on the mobile.  This is hard coded to one, as multiple
  sessions are not supported in the current software revision.
---------------------------------------------------------------------------*/
#define MIP_MAX_SESSIONS  (1)

/*---------------------------------------------------------------------------
   Maximum length of an NAI as per RFC 2794
---------------------------------------------------------------------------*/
#define MAX_NAI_LEN  (72)

/*---------------------------------------------------------------------------
   Max # of HA's
---------------------------------------------------------------------------*/
#define MAX_HA_ADDRS  (2)

/*---------------------------------------------------------------------------
  Home Agent IP address constants
---------------------------------------------------------------------------*/
#define UNSET_HA_ADDR       0
#define DYNAMIC_HA_ADDR     0xFFFFFFFF

#define DYNAMIC_HOME_ADDR  0

#define PRIMARY_HA_INDEX 0

#if MAX_HA_ADDRS > 1
#define SECONDARY_HA_INDEX 1
#endif /* MAX_HA_ADDRS > 1 */

#define MIP_RRQ_RETRY_BACKOFF_MULT 2 /* from Section 3.6.3 rfc2002         */

/*---------------------------------------------------------------------------
  Shared secret lengths are 16 bytes, corresponding to the following
  definitions in nv.h:

    MV_MAX_MN_HA_SHARED_SECRET_LEN
    MV_MAX_MN_AAA_SHARED_SECRET_LEN
---------------------------------------------------------------------------*/
#define MIP_MN_HA_SS_MAX_LEN   (16)
#define MIP_MN_AAA_SS_MAX_LEN  (16)

/*---------------------------------------------------------------------------
  SPI definitions

  entities     SPI     Algorithm        Auth len    Reference
  --------     -----   ------------     --------    ----------
  MN/AAA        2      CHAP              16         RFC 1994
  MN/HA         3      MD5               16         RFC 1321
---------------------------------------------------------------------------*/
#define MIP_CHAP_SPI       (2)                 /* SPI for CHAP (MN-AAA)    */
#define MIP_AUTH_CHAP_LEN  (0)                 /* RFC 3012 - ? bits?       */

#define MIP_MD5_SPI        (3)                 /* SPI for MD5 (MN-HA)      */
#define MIP_AUTH_MD5_LEN   (16)                /* RFC 2002 - 128-bit auth. */

#define MIP_AUTH_MAX_LEN   (MIP_AUTH_MD5_LEN)  /* max of lengths above     */


/*---------------------------------------------------------------------------
  qcmipt value is a bit field and can be used for testing various Mobile
  IP  features
  
  | QCMIPT | MN_HA_Auth_2002bis |  Clear_txt DMU |   
  ------------------------------------------------
     0           No                    No 
     1           Yes                   No 
     2           No                    Yes
     3           Yes                   Yes
---------------------------------------------------------------------------*/
#define MIP_QCMIPT_MN_HA_AUTH_2002BIS_BIT 0x01  /* RFC2002bis MN-HA Auth   */
#define MIP_QCMIPT_DMU_CLEARTEXT_BIT      0x02  /* clear text mode in DMU  */
#define MIP_QCMIPT_RSVD_03_BIT            0x04  /* reserved for future use */
#define MIP_QCMIPT_RSVD_04_BIT            0x08  /* reserved for future use */
#define MIP_QCMIPT_RSVD_05_BIT            0x10  /* reserved for future use */
#define MIP_QCMIPT_RSVD_06_BIT            0x20  /* reserved for future use */
#define MIP_QCMIPT_RSVD_07_BIT            0x40  /* reserved for future use */
#define MIP_QCMIPT_RSVD_08_BIT            0x80  /* reserved for future use */

#define MIP_EVENT_CLIENT_MAX                 2

#define MIP_DLFT_PROFILE_NUM               (0)

/*===========================================================================

                         PUBLIC DATA STRUCTURES

===========================================================================*/

/*---------------------------------------------------------------------------
   Registration state machine states (internal)
---------------------------------------------------------------------------*/
typedef enum
{
  RSMI_MIN_STATE = -1,
  RSMI_NO_SESSION_STATE,             /* No Session state                   */
  RSMI_INIT_RRQ_STATE,               /* Initial RRQ state                  */
  RSMI_REGISTERED_STATE,             /* Registered state                   */
  RSMI_RE_RRQ_STATE,                 /* Re-RRQ state                       */
  RSMI_DE_RRQ_STATE,                 /* De-registration RRQ state          */
  RSMI_DEREGISTERED_STATE,           /* De-registered state                */
                                     /* for future enhancements            */
  RSMI_MAX_STATE
} rsmi_state_type;

/*---------------------------------------------------------------------------
   Registration state machine info (internal)
---------------------------------------------------------------------------*/
typedef struct
{
  rsmi_state_type state;             /* state machine's state              */
  uint32  pre_re_rrq_time;           /* pre-registration retry time (ms)   */
  uint32  re_rrq_timeout;            /* period between RRQ's (ms)          */
  int     life_timer;                /* Registration lifetime timer handle */
  int     reg_timer;                 /* Registration timeout timer handle  */
  int     dereg_timer;               /* Deregistration timeout timer handle*/
                                     /* RRQ holdoff (1s) timer handle      */
  int     rrq_holdoff_timer;         /* RRQ holdoff (1s) timer handle      */
  uint16  rtt_estimate;              /* Estimated RTT from RRQ -> RRP      */ 
  sint15  app_id;                    /* Stores the socket App ID           */
  byte    rrq_cnt;                   /* retries sent                       */
  byte    max_rrqs;                  /* max number of rrqs                 */
  byte    max_derrqs;                /* max number of dereg rrqs           */
  boolean one_second_since_last_rrq; /* false when holdoff timer is active */
  boolean force_dormancy;            /* This boolean determines if the     */
                                     /* callis forced to go dormant on     */
                                     /* completion of registration         */
} rsm_info_type;

/*---------------------------------------------------------------------------

DESCRIPTION
  Enum describing reason for MIP failure
---------------------------------------------------------------------------*/
typedef enum mip_fail_reason_e_type
{
  MIP_FAIL_REASON_NONE         = 0,
  MIP_FAIL_REASON_PDSN_FAILURE = 1,
  MIP_FAIL_REASON_SOL_TIMEOUT  = 2,
  MIP_FAIL_REASON_RRQ_TIMEOUT  = 3,
  MIP_FAIL_REASON_FA_FAILURE   = 4,
  MIP_FAIL_REASON_HA_FAILURE   = 5,
  MIP_FAIL_REASON_UNKNOWN      = MIP_FAIL_REASON_NONE
} mip_fail_reason_e_type;

/*---------------------------------------------------------------------------
TYPEDEF MIP_EVENT_E_TYPE

DESCRIPTION
  enum of PPP events that can be registered for.
---------------------------------------------------------------------------*/
typedef enum
{
  MIP_MIN_EV       = 0,
  MIP_SUCCESS_EV   = 0,
  MIP_FAILURE_EV   = 1,
  MIP_DEREGED_EV   = 2,
  MIP_MAX_EV
} mip_event_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_MASK_E_TYPE

DESCRIPTION
  enum of PPP event mask that can be registered for
---------------------------------------------------------------------------*/
typedef enum
{
  MIP_FAILURE_EV_MASK = (1 << MIP_FAILURE_EV),
  MIP_SUCCESS_EV_MASK = (1 << MIP_SUCCESS_EV),
  MIP_DEREGED_EV_MASK = (1 << MIP_DEREGED_EV)
} mip_event_mask_e_type;

/*---------------------------------------------------------------------------
TYPEDEF MIP_EVENT_CBACK_F_PTR_TYPE

DESCRIPTION
  type of the callback function that is called when a MIP event occurs.
---------------------------------------------------------------------------*/
typedef void (*mip_event_cback_f_ptr_type)
(
  mip_event_e_type        mip_event,
  mip_fail_reason_e_type  reason,
  void                    *user_data
);

/*---------------------------------------------------------------------------
  Type for MIP event information - allocated from ps_mem
---------------------------------------------------------------------------*/
typedef struct mip_event_info_s_type
{
  mip_event_cback_f_ptr_type  cback;
  void                       *user_data;
  mip_event_mask_e_type       event_mask;
} mip_event_info_s_type;

/*---------------------------------------------------------------------------
   Mobile IP session information - one per session
---------------------------------------------------------------------------*/
typedef struct
{
  char nai[MAX_NAI_LEN];        /* Network Access Identifier            */

  rsm_info_type rsm_info;          /* registration state control info      */

  uint32  ha_addrs[MAX_HA_ADDRS];  /* Home Agent IP address array          */
  uint32  home_addr;               /* current home IP address              */
  uint32  home_addr_subnet_len;    /* current home IP address subnet len   */
  uint32  reg_id_msw;              /* ID sent with registration request    */
  uint32  reg_id_lsw;              /*  (second half of RRQ ID field)       */
  uint32  time_offset;             /* time difference between MN-HA        */
  uint32  mn_ha_spi;               /* SPI value for the MN-HA extension    */
  uint32  mn_aaa_spi;              /* SPI value for the AAA extension      */
  uint16  reg_lifetime;            /* reg lifetime: from RRP (seconds)     */
  uint16  max_reg_time;            /* maximum time spent trying to reg (ms)*/
  uint16  base_retry_interval;     /* the base interval for retries (ms)   */
  uint16  retry_interval_offset;   /* this offset is added to the base     */
                                   /*   interval for initial rrq           */
  byte    curr_ha_index;           /* index into the ha_addrs array        */
  byte    nai_len;                 /* length of NAI field above            */
  boolean mn_request_rev_tun;      /* MN rev tunneling requested?          */
  boolean rrq_if_tfrk;             /* Send RRQ only if some data has been  */
                                   /* transferred since last registration  */

  boolean valid;                   /* contents are valid (nv read ok)      */
  byte    profile_index;           /* current NV profile index             */
#ifdef FEATURE_DS_MOBILE_IP_DMU
  boolean dmu;                     /* flag true when DMU in progress       */
#endif /* FEATURE_DS_MOBILE_IP_DMU */
  mip_fail_reason_e_type mip_fail_reason; /* reason for mip call fail      */
  byte    mip_event_client_cnt;    /* no. of clients for MIP events        */
} mip_session_info_type;


/*---------------------------------------------------------------------------
   Mobility Agent (foreign and/or home agent) Info Block
   - all sessions transmit use the same ("active") FA
---------------------------------------------------------------------------*/

typedef struct
{
  uint32   ma_addr;             /* IP address of mobility agent            */
  uint16   ma_lifetime;         /* registration lifetime granted by MA     */
  uint16   last_seq_num;        /* sequence number of last agent adv.      */
  byte     mn_AAA_SPI[4];       /* Security Parameter Index for AAA auth   */
  boolean  ma_supp_rev_tun;     /* MA supports reverse tunneling           */
  byte     ma_prefix_len;       /* subnet prefix len of ma_addr            */
  byte     challenge_length;    /* if 0 there was no challenge             */
  byte     fa_challenge[255];   /* the challenge itself                    */
} mip_ma_info_type;


/*---------------------------------------------------------------------------
  Structure to hold password info obtained via user interface.
---------------------------------------------------------------------------*/
typedef struct
{
  byte  mn_ha_ss[MIP_MN_HA_SS_MAX_LEN];    /* MN-HA shared secret          */
  byte  mn_aaa_ss[MIP_MN_AAA_SS_MAX_LEN];  /* MN-AAA shared secret         */
  byte  mn_ha_ss_len;                      /* MN-HA shared secret length   */
  byte  mn_aaa_ss_len;                     /* MN-AAA shared secret length  */
} mip_pwd_info_type;


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Session info array, each entry holds all parameters related to a single 
  MIP session
---------------------------------------------------------------------------*/
extern mip_session_info_type  mip_ses_info_array[MIP_MAX_SESSIONS];

/*---------------------------------------------------------------------------
   Foreign Agent currently serving the mobile
---------------------------------------------------------------------------*/
extern mip_ma_info_type  ma_info;

/*---------------------------------------------------------------------------
  Holds password info, passed to mip_get_pwd_cb. 
---------------------------------------------------------------------------*/
extern mip_pwd_info_type  mip_pwd_info;

/*---------------------------------------------------------------------------
  Callback to retrieve session information - if non-null, this callback
  will be called to have the UI modify any parameters it wants after 
  the MIP session information has been retrieved from NV
---------------------------------------------------------------------------*/
extern void (* mip_get_session_info_cb)( byte, mip_session_info_type * );

/*---------------------------------------------------------------------------
  Callback to retrieve shared secret information.  
  Externalized so dsmip_outmsg can check if a function is registered.
---------------------------------------------------------------------------*/
extern void (* mip_get_pwd_cb)( byte, mip_pwd_info_type * );


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_CONFIG_SESSION()

DESCRIPTION
  Load MIP session defaults from NV to all sessions. Note: this function
  be invoked before the traffic channel is established.

PARAMETERS
  None

RETURN VALUE
  TRUE: No errors encoutered
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_config_session 
( 
  void 
);


/*===========================================================================
FUNCTION MIP_REG_SES_INFO_CB()

DESCRIPTION
  Registers a user callback to populate MIP session info control block.

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_session_info_type * ses_info)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_ses_info_cb
(
  void (* callback)( byte profile_num, mip_session_info_type * ses_info )
);


/*===========================================================================
FUNCTION MIP_REG_PWD_INFO_CB()

DESCRIPTION
  Registers a user callback to provide MIP password info. 

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_pwd_info_type * pwd_info )

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_pwd_info_cb
(
  void (* callback)( byte profile_num, mip_pwd_info_type * pwd_info )
);

/*===========================================================================
FUNCTION MIP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a MIP event information structure and
  register callback functions.

PARAMETERS
  mip_cb_f:         callback function to call
  user_data:        user data to be returned with the callback fn.

RETURN VALUE
  NULL on error
  handle to the MIP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a MIP event information structure and places it on the event
  queue.  Initializes event masks and registers callbacks for MIP protocol.
===========================================================================*/
void *mip_event_alloc
(
  mip_event_cback_f_ptr_type  mip_cb_f,
  void                        *user_data
);

/*===========================================================================
FUNCTION MIP_CALL_EVENT_CALLBACK()

DESCRIPTION
  Traverses the MIP event queue and calls the passed event.

  NOTE: This function TASKLOCKs inside and need not be called with TASKLOCK

  NOTE: The callback function should not dealloc the event notification
        structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Generates MIP events
===========================================================================*/
void mip_call_event_callback
(
  mip_event_e_type    event
);

/*===========================================================================
FUNCTION MIP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a  MIP session.
  It will  generate an UP event if the event mask is registering for an UP
  event and the UP event was previously not registerd for this protocol and
  the associated protocol is already up.

PARAMETERS
  handle      PPP event structure to add this event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 mip_event_register
(
  void                  *handle,
  mip_event_mask_e_type  event_mask
);

#ifdef FEATURE_DS_MULTIPLE_NAI
/*===========================================================================
FUNCTION MIP_GET_RM_NAI()

DESCRIPTION
  This function retrieves the NAIs for a given MIP profile for the
  tethered device and stores them in the mip session info.

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, the current profile no. that was passed in as argument.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 mip_get_rm_nai
(
  mip_session_info_type *  session,
  int                      active_mip_profile
);
#endif /* FEATURE_DS_MULTIPLE_NAI */

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* _DSMIP_H */
