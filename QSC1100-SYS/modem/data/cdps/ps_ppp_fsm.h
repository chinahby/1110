#ifndef PS_PPP_FSM_H
#define PS_PPP_FSM_H
/*===========================================================================

                           P S _ P P P _ F S M . H

DESCRIPTION
  Header file for the PPP protocol suite finite state machine

  Copyright 1992-1995 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

  Copyright (c) 1995-2009 by QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_fsm.h_v   1.8   15 Nov 2002 23:40:02   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_fsm.h#2 $ $DateTime: 2009/05/27 05:07:18 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
05/13/03    usb    Moved MIP policy info to dssnet control block.
04/08/03    mvl    Added support for not moving to the network phase if auth
                   info not received.
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
11/14/02    jd     Split qcmip_mode into will_sip, want_mip
11/04/02    mvl    Fixed type of the guard timer handle.
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user_data to ppp control block tx and rx functions
09/18/02    mvl    Added user_data to ppp event callbacks.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    Added the setup timeout field and the timer handle for
                   this timeout to the PPP CB.
08/05/02  dwp/mvl  Add tail_full to asy_in_var_type.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Moved some definitions to ps_ppp_defs.h to clean up
                   dependencies.  Removed network model dependency.  Updates
                   for PPP renaming.
07/15/02    aku    Removed include for pstimer.h
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added event callback and bridging support.
05/23/02    mvl    reworked PPP
05/20/02     vr    Added hdlc_mode in ppp control block
03/08/02    mvl    Fixed some formatting problems (no content change).
03/01/02    vr     Added support for PPP passive mode.
02/05/02    vr     Added flag do_ipcp for configuring No IPCP on AN stream
12/01/01    vas    Added do_auth field to ppp ctl blk
08/31/01    mvl    Changed the #defines of the msg types to an enum for
                   easier debugging.
08/29/01  pjb/mvl  Modified the values option bits to support DNS options.
08/02/01    mvl    iface field in fsm_type is not dependent on network model.
                   removed the ppp_cb_ptr - only ppp_cb_array remains
                   removed prototype for fsm_log(), general code cleanup
07/27/01    mvl    Modified the timer type in the fsm_type so pstimers could
                   be used.  Also reordered fields for better packing.
07/26/01    na     Added do_mip and qcmip_mode to struct ppp_s.
07/22/01    mvl    Added iface field to FSM struct, put conditional compile
                   flags around some fields in the FSM struct to conserve
                   memory, removed obsolete debug macros, and code cleanup 
05/02/01    mvl    Added booleans 'in_mip_mode', and 'force_sip_next_call'
                   for MIP.
04/27/01    mvl    Reoredered fields in ppp_cb for more efficient packing.
08/20/99    mvl    Added macro CONFIG_HDR_LEN
08/09/99    mvl    Added network model changes.
04/08/99    na     Added dsm_kind field to the fsm_constant_s struct
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_task.h"
#include "ps_iface.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  The code values for LCP/IPCP packets
---------------------------------------------------------------------------*/
enum
{
 CONFIG_REQ     =  1,
 CONFIG_ACK     =  2,
 CONFIG_NAK     =  3,
 CONFIG_REJ     =  4,
 TERM_REQ       =  5,
 TERM_ACK       =  6,
 CODE_REJ       =  7,
 PROT_REJ       =  8,
 ECHO_REQ       =  9,
 ECHO_REPLY     = 10,
 DISCARD_REQ    = 11,
 IDENTIFICATION = 12,
 TIME_REMAINING = 13
};

/*---------------------------------------------------------------------------
  Config packet header
---------------------------------------------------------------------------*/
struct config_hdr 
{
  uint8  code;
  uint8  identifier;
  uint16 length;
};
typedef struct config_hdr ppp_config_hdr_type;

/* Length of config packet header */
#define CONFIG_HDR_LEN 4

/*---------------------------------------------------------------------------
  This is the max. number of bytes per PPP option that this implementation
  supports. This is used to declare byte arrays to store option values.
---------------------------------------------------------------------------*/
#define PPP_OPT_MAX_LEN 4 

/*---------------------------------------------------------------------------
  config option header
---------------------------------------------------------------------------*/
struct option_hdr 
{
  uint8  type;          /* protocol dependent types */
  uint8  length;
};
typedef struct option_hdr ppp_opt_hdr_type;

/* Length of option header */
#define OPTION_HDR_LEN 2 

/*---------------------------------------------------------------------------
  Supported Configuration Protocol index
---------------------------------------------------------------------------*/
enum 
{
  LCP  = PPP_PROTOCOL_LCP,
  IPCP = PPP_PROTOCOL_IPCP,
  Lcp  = LCP,
  IPcp = IPCP,
  fsmi_Size
};

struct fsm_s;          /* forward declaration */
struct fsm_side_s;     /* forward declaration */

/*---------------------------------------------------------------------------
  Protocol Constants needed by State Machine
---------------------------------------------------------------------------*/
struct fsm_constant_s
{
#ifndef T_ARM
  char   *name;            /* Name of protocol                     */
  char   **codes;          /* names of codes                       */
#endif /* T_ARM */
  uint16 protocol;         /* Protocol number                      */
  uint8 dsm_kind;          /* The DSM kind field for this protocol */
  uint16 recognize;        /* Codes to use (bits)                  */

  uint8  fsmi;             /* Finite State Machine index           */
  uint8  try_configure;    /* # tries for configure                */
  uint8  try_nak;          /* # tries for nak substitutes          */
  uint8  try_terminate;    /* # tries for terminate                */
  uint16 timeout;          /* Time for timeouts (milliseconds)     */

  uint8  option_limit;     /* maximum option number                */
  uint8  n_option_limit;   /* maximum mask number                  */
  uint8  * n_option;       /* mask table                           */

  /* To free structure */
  void (*free)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* Lower Layer Down event */
  void (*down)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Start: entering Starting */
  void (*starting)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Up: entering Opened */
  void (*opening)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Down: leaving Opened */
  void (*closing)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Finish: entering Closed or Stopped */
  void (*stopping)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  void (*option)
  (
    struct fsm_s      *fsm_ptr,        /* ptr to a FSM structure */
    void              *vp,           /*   */
    struct dsm_item_s **bpp,         /*   */
    uint8             o_type         /*   */
  );

  int (*check)
  (
    struct fsm_s      *fsm_ptr,      /*  */
    struct option_hdr *ohp,        /*  */
    struct dsm_item_s **bpp,       /*  */
    int               request      /*  */
  );

  int (*reject)
  (
    struct fsm_s      *fsm_ptr,      /*   */
    struct option_hdr *ohp         /*   */
  );
};

/*---------------------------------------------------------------------------
  FSM states
---------------------------------------------------------------------------*/
enum
{
  fsmINITIAL  = 0,
  fsmSTARTING = 1,
  fsmCLOSED   = 2,
  fsmSTOPPED  = 3, 
  fsmCLOSING  = 4,
  fsmSTOPPING = 5,
  fsmREQ_Sent = 6,
  fsmACK_Rcvd = 7,
  fsmACK_Sent = 8,
  fsmOPENED   = 9,
  fsmState_Size
};

/*---------------------------------------------------------------------------
  local.will:  Options to accept in a NAK from remote,
      which may be different from the desired value.
  local.want:  Options to request.
      Contains desired value.
      Only non-default options need to be negotiated.
      Initially, all are default.
  local.work:  Options currently being negotiated.
      Value is valid only when negotiate bit is set.

  remote.will:  Options to accept in a REQ from remote,
      which may be different from the desired value.
  remote.want:  Options to suggest by NAK if not present in REQ.
      Contains desired value.
  remote.work:  Options currently being negotiated.
      Value is valid only when negotiate bit is set.
---------------------------------------------------------------------------*/
struct fsm_side_s
{
  negotiate_t will;
  negotiate_t want;
  void  *want_pdv;
  negotiate_t work;
  void  *work_pdv;
};

typedef struct fsm_side_s ppp_fsm_side_type;

/*---------------------------------------------------------------------------
  State Machine Control Block
---------------------------------------------------------------------------*/
struct fsm_s
{
  struct fsm_side_s local;     /* local protocol option information        */
  struct fsm_side_s remote;    /* local protocol option information        */

  struct fsm_constant_s *pdc;  /* protocol dependent constants             */
  void *pdv;                   /* protocol dependent variables             */
  int timer;                   /* Handle for this FSMs timer               */

  ppp_dev_enum_type device;    /* the interface this struct corresponds to */
  cp_mode_type mode;           /* the mode this fsm                        */

  uint8  state;                /* FSM state                                */
  uint8  lastid;               /* ID of last REQ we sent                   */

  uint8  flags;                                                            

  ppp_fail_reason_e_type fail_reason;
                              /* cause of failure to bring up FSM          */

  uint8  retry_request;        /* counter for timeouts                     */
  uint8  try_configure;        /* # tries for configure                    */
  uint8  try_terminate;        /* # tries for terminate                    */

  uint8  retry_nak;            /* counter for naks of requests             */
  uint8  try_nak;              /* # tries for nak substitutes              */

  boolean passive_mode;        /* whether to open this fsm in passive mode */
  
};
typedef struct fsm_s ppp_fsm_type;

/*---------------------------------------------------------------------------
  Link Phases
---------------------------------------------------------------------------*/ 
typedef enum
{
  pppDEAD,          /* Waiting for physical layer                          */
  pppESTABLISH,     /* Link Establishment Phase                            */
  pppAUTHENTICATE,  /* Authentication Phase                                */
  pppNETWORK,       /* Network Protocol Phase                              */
  pppTERMINATE,     /* Termination Phase                                   */
  pppPhase_Size
} ppp_link_phase_type;


/*---------------------------------------------------------------------------
  Enum for the unframe_mode state for ppp_asy_in.

  The state machine is described in the header for the ppp_asy_in()
  function.
---------------------------------------------------------------------------*/
typedef enum
{
  FINDING_7E  = 0,              /* looking for 7E                          */
  FINDING_SOP,                  /* looking for start of packet - after 7Es */
  SNOOPING,                     /* early proto detect - looking for proto  */
  FINDING_EOP,                  /* looking for end of packet               */
  UNFRAMING                     /* unframing to pass to proto msg detect   */
} ppp_unframe_mode_enum_type;

/*---------------------------------------------------------------------------
  This struct maintains all the static variables for the function 
  ppp_hdlc_sw_unframe().
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type* pkt_head_ptr;    /* pointer to packet being build         */
  dsm_item_type* pkt_tail_ptr;    /* pointer to tail of packet being built */
  dsm_item_type* orig_head_ptr;   /* pointer to packet that was passed in  */
  dsm_item_type* orig_tail_ptr;   /* pointer to tail of passed in packet   */
  uint16 prev_pkt_size;           /* size of the previous packet           */
  uint16 rx_pkt_size;             /* received packet size                  */
  uint16 calc_fcs;                /* the FCS for a packet on this iface    */
  ppp_unframe_mode_enum_type unframe_mode; /* current unframing mode       */
  boolean tail_full;              /* time to get another dsm item for pkt? */
  boolean escape;                 /* was last character the escape char?   */
} hdlc_unframe_var_type;


/*===========================================================================
                              PPP Control Block

TYPEDEF PPP_TYPE
===========================================================================*/
struct ppp_s
{
  ppp_fsm_type   fsm[fsmi_Size];    /* finite state machines               */
                                    
  /*-------------------------------------------------------------------------
    Function pointers for the PPP event callbacks.  The events and fucntion
    signatures are defined in ps_ppp_defs.h
  -------------------------------------------------------------------------*/
  struct
  {
    ppp_event_cback_f_ptr_type cback;
    void                      *user_data;
  } event_info[PPP_MAX_EV];
  hdlc_unframe_var_type input_params;/* variables use for HDLC unframing   */

  /*-------------------------------------------------------------------------
    Authentication related information
  -------------------------------------------------------------------------*/
  struct ppp_cb_auth_s
  {
    ppp_auth_info_type info;          /* auth info: MUST BE FIRST ELEMENT  */
    dsm_item_type *chap_challenge_ptr;/* to store CHAP challenge           */
    void (*retry_proc)(struct ppp_s*);/* function ptr to handle retry proc */
    uint8              mode;          /* auth negotiated:CHAP, PAP or NONE */
    uint8              active_id;     /* in use ID value for auth          */
    uint8              retry_counter; /* retry counter for auth attempts   */
#define PPP_AP_LOCAL   0x01           /* waiting to be authenticated       */
#define PPP_AP_REMOTE  0x02           /* waiting to authenticate peer      */
#define PPP_AP_SUCCESS 0x04           /* authentication succeeded          */
#define PPP_AP_FAILED  0x08           /* authentication failed             */
    uint8              flags;         

  } auth;

  /*-------------------------------------------------------------------------
    Configuration Info
  -------------------------------------------------------------------------*/
  tx_f_ptr_type       tx_f_ptr;     /* function user to tx PPP packets     */
  rx_f_ptr_type       rx_f_ptr;     /* Function to receive next data item  */
  tx_f_ptr_type       pend_tx_f_ptr;/* pending area for tx function        */
  rx_f_ptr_type       pend_rx_f_ptr;/* pending area for rx function        */
  void               *tx_user_data; /* user date for tx_function           */
  void               *rx_user_data; /* user date for tx_function           */
  void           *pend_tx_user_data;/* User data for tx_f pending update   */
  void           *pend_rx_user_data;/* User data for rx_f pending update   */
  ps_iface_type      *rx_iface_ptr; /* pointer to the receive interface    */
  ppp_sig_type        rx_signal;    /* signal to use for RX                */
  int32               setup_timer;  /* handle for the setup timer          */
  uint16              setup_timeout;/* time (ms) before giving up on setup */
  uint16              session_handle;/* session handle to id iface calls   */
  ppp_link_phase_type phase;        /* phase of link initialization        */
  uint8               id;           /* id counter for connection           */
  boolean             serves_addrs; /* does this PPP serve IP addresses?   */
  ppp_dev_enum_type   bridge_dev;   /* interface to bridge to if snooping  */
  ppp_mode_enum_type  mode;         /* the mode this device is in          */
  hdlc_mode_enum_type hdlc_mode;    /* HDLC framing in HW/SW or none       */

  /*-------------------------------------------------------------------------
    Misc
  -------------------------------------------------------------------------*/
  uint32         upsince;           /* Timestamp when Link Opened          */
                                    
  uint32         OutOpenFlag;       /* # of open flags sent                */
  uint32         OutNCP[fsmi_Size]; /* # NCP packets sent by protocol      */
                                    
  uint32         InGoodOctets;      /* # of octets in good packets         */
  uint32         InOpenFlag;        /* # of open flags                     */
  uint32         InNCP[fsmi_Size];  /* # NCP packets by protocol           */
  uint32         InUnknown;         /* # unknown packets received          */
  uint32         InChecksum;        /* # packets with bad checksum         */
  uint32         InFrame;           /* # packets with frame error          */
};
typedef struct ppp_s ppp_type;

extern ppp_type ppp_cb_array[PPP_MAX_DEV];

#ifndef T_ARM
extern char *fsmStates[];
extern char *fsmCodes[];
#endif /* T_ARM */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
negotiate_t fsm_n_option
(
  struct fsm_constant_s *fsm_constants_p,
  uint8 option
);

int ntohcnf
(
  struct config_hdr *cnf,
  dsm_item_type    **bpp
);

void htonopt_copy
(
  dsm_item_type    **bpp,
  struct option_hdr *opt,
  dsm_item_type    **copy_bpp
);

int ntohopt
(
  struct option_hdr *opt,
  dsm_item_type    **bpp
);

void fsm_timer
(
  struct fsm_s *fsm_ptr
);

int fsm_send
(
  struct fsm_s  *fsm_ptr,
  uint8          code,
  uint8          id,
  dsm_item_type *bp
);

void fsm_proc
(
  struct fsm_s  *fsm_ptr,
  dsm_item_type *bp
);

boolean fsm_timeout
(
  struct fsm_s *fsm_ptr
);

void fsm_passive_wakeup
(
  ppp_fsm_type *fsm_ptr
);

void fsm_up
(
  struct fsm_s *fsm_ptr
);

void fsm_down
(
  struct fsm_s *fsm_ptr
);

void fsm_open
(
  struct fsm_s *fsm_ptr
);

void fsm_close
(
  struct fsm_s *fsm_ptr
);

void fsm_init
(
  struct fsm_s *fsm_ptr
);

void fsm_free
(
  struct fsm_s *fsm_ptr
);

#endif /* _PS_PPP_FSM_H */
