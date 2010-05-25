#ifndef PS_PPP_DEFS_H
#define PS_PPP_DEFS_H
/*===========================================================================

                          P S _ P P P _ D E F S . H

DESCRIPTION
  This file definitions that are used for PPP both internally and externally.

  Copyright (c) 2001-2009 by QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_defs.h_v   1.7   03 Feb 2003 15:01:04   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_defs.h#3 $
  $Author: nsivakum $ $DateTime: 2009/05/27 05:07:18 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/09    sn     Ported support for call throttle feature (DCTM).
11/23/08    scb    Added fix for OMH package to read correct nai_entry_index
06/03/04    jd     Added CCP and Individual link CCP protocols
06/03/03    aku    Changed value of PPP_MAX_USER_ID_LEN to 127 from 64.
04/08/03    mvl    Moved PPP_CHAP_MD5 to ps_ppp_defs.h
04/07/03    usb    Added new PPP mode SNOOP_INTERNAL
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
01/31/03    jd     Changed MAX_PASSWD_LEN to reflect NV_PPP_PASSWORD_I length
                   since this is now used instead of NV_PAP_PASSWORD_I
11/04/02    mvl    Added comment explaining use of TX function.  Moved PPP
                   protocol definitions here.
10/11/02    usb    Changed HDLC hardware feature to FEATURE_HDLC_HW_ACCEL
09/24/02    usb    Added user_data to ppp tx and rx function prototypes
09/18/02    mvl    Added user_data to ppp event callbacks.
08/13/02    mvl    Added session handle.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Moved more definitions in here to clean up include
                   dependencies.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added support for event callbacks.
05/27/02    mvl    Renamed file, and moved some more definitions in here.
08/29/01  pjb/mvl  Added fields for DNS servers addresses.
07/28/01    mvl    created module
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "dsm.h"

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PPP_DEV_ENUM_TYPE

DESCRIPTION
  Enum defining the PPP "devices" that are available
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_MIN_DEV                   = 0,   /* the lower bound value            */
  PPP_UM_SN_DEV                 = 0,   /* Um Service Network instance      */
  PPP_RM_DEV                    = 1,   /* the RM instance                  */
#ifdef FEATURE_HDR_AN_AUTH                                                
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */                                          
  PPP_MAX_DEV,                         /* max value - array sizing         */
  PPP_INVALID_DEV               = 255  /* invalid device                   */
} ppp_dev_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF NEGOTIATE_T

DESCRIPTION
  Type for the bitmasks
---------------------------------------------------------------------------*/
typedef uint16 negotiate_t;

/*---------------------------------------------------------------------------
TYPEDEF FSM_SIDE_ENUM_TYPE

DESCRIPTION
  Type that enumerates the local and remote sides of the PPP FSM data
  structure.
---------------------------------------------------------------------------*/
typedef enum side_e
{
  FSM_LOCAL  = 0,
  FSM_REMOTE = 1,
  FSM_SIDE_MAX
} fsm_side_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF RX_F_PTR_TYPE

DESCRIPTION
  typedef for the PPP receive fucntion pointers.
---------------------------------------------------------------------------*/
typedef dsm_item_type* (*rx_f_ptr_type)( void * );

/*---------------------------------------------------------------------------
TYPEDEF TX_F_PTR_TYPE

DESCRIPTION
  typedef for the PPP TX fucntion pointers.

  NOTE: the META INFO is passed in the APP field of the dsm item as a double
        pointer, that is: **meta_info is required to get to the item.
---------------------------------------------------------------------------*/  
typedef void  (*tx_f_ptr_type)( dsm_item_type **, void * );

/*---------------------------------------------------------------------------
TYPEDEF CP_MODE_TYPE

DESCRIPTION
  Enum defining the modes of operation for LCP and IPCP.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_CP_ACTIVE   = 0,
  PPP_CP_PASSIVE  = 1,
  PPP_CP_DISABLED = 2, /* this is not meaningful for LCP */
  PPP_CP_INVALID_MODE
} cp_mode_type;

/*---------------------------------------------------------------------------
TYPEDEF HDLC_MODE_ENUM_TYPE

DESCRIPTION
  Enum defining the hdlc modes: off, use software, or try for hardware - if
  the hardware mode will only be used if another instance is not using it.
---------------------------------------------------------------------------*/
typedef enum
{
  HDLC_OFF       = 0,
  HDLC_SW        = 1,
  HDLC_HW_PREF   = 2
#ifdef FEATURE_HDLC_HW_ACCEL
  , HDLC_HW      = 2
#endif /* FEATURE_HDLC_HW_ACCEL */

  , HDLC_INVALID_MODE /* this should always be the last entry! */

} hdlc_mode_enum_type;

/*---------------------------------------------------------------------------
  Enum for the PPP mode (i.e. used for snooping, and defining data path)
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_NONE_MODE,            /* When there is no PPP on the interface       */
  PPP_RELAY_MODE,           /* No snooping, bypass ppp pkt processing      */
  PPP_PSEUDO_NET_MODE,      /* By default forward pkt to bridge device,    */
                            /* unframe only if required by snoop           */
  PPP_FULL_NET_MODE,        /* By default forward pkt to bridge device,    */
                            /* always unframe, snoop if required           */
  PPP_INTERNAL_SNOOP_MODE,  /* By default pass pkt up the stack in phone,  */
                            /* always unframe, snoop if required           */
  PPP_INTERNAL_MODE         /* Pass all PPP pkts up the stack in phone,    */
                            /* always unframe, never snoop                 */
} ppp_mode_enum_type;


/*---------------------------------------------------------------------------
  enums defining the LCP and IPCP bitmasks for the options that we can
  negotiate.
---------------------------------------------------------------------------*/
enum
{
  LCP_N_MRU  = 1 << 0,
  LCP_N_ACCM = 1 << 1,
  LCP_N_AP   = 1 << 2,
  LCP_N_QMP  = 1 << 3,
  LCP_N_MAGIC= 1 << 4,
  LCP_N_PFC  = 1 << 5,
  LCP_N_ACFC = 1 << 6,
  LCP_N_OPTION_LIMIT = 7     /* this value should always be last shift + 1 */
};

enum
{
  IPCP_N_COMPRESS      = 1 << 0,
  IPCP_N_1ADDRESS      = 1 << 1,
  IPCP_N_PRIMARY_DNS   = 1 << 2,
  IPCP_N_SECONDARY_DNS = 1 << 3,
  IPCP_N_OPTION_LIMIT  = 4   /* this value should always be last shift + 1 */
};

/*---------------------------------------------------------------------------
TYPEDEF LCP_VALUE_TYPE

DESCRIPTION
  Table for LCP configuration requests
  Note that the chap_digest is not a option, as other members of this table.
---------------------------------------------------------------------------*/
typedef struct lcp_value_s
{
  uint32 accm;                  /* Async Control Char Map                  */
  uint32 magic_number;          /* Magic number value                      */
  uint16 mru;                   /* Maximum Receive Unit                    */
  uint16 authentication;        /* Authentication protocol                 */
  uint16 monitoring;            /* Quality Monitoring protocol             */
  uint8  chap_digest;           /* Digest algorithm used for CHAP          */
} lcp_value_type;


/*---------------------------------------------------------------------------
TYPEDEF IPCP_VALUE_TYPE

DESCRIPTION
  Table for IPCP configuration requests
---------------------------------------------------------------------------*/
typedef struct ipcp_value_s
{
  uint32 address;                  /* address for this side         */
  uint32 primary_dns;		   /* primary dns server address */
  uint32 secondary_dns;            /* secondary dns server address */
  uint16 compression;              /* Compression protocol          */
  uint8  slots;                    /* Slots (0-n)                   */
  uint8  slot_compress;            /* Slots may be compressed (flag)*/
} ipcp_value_type;


/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_E_TYPE

DESCRIPTION
  enum of PPP events that can be registered for.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_MIN_EV      = 0,
  PPP_DOWN_EV     = 0,
  PPP_UP_EV       = 1,
  PPP_RESYNC_EV   = 2,
  PPP_FAILURE_EV  = 3,
  PPP_SUCCESS_EV  = 4,
  PPP_MAX_EV
} ppp_event_e_type;

/*---------------------------------------------------------------------------
  Supported Configuration Protocols
---------------------------------------------------------------------------*/
typedef enum ppp_protocol_e_type
{
  PPP_PROTOCOL_MIN    = 0,
  PPP_PROTOCOL_LCP    = PPP_PROTOCOL_MIN,
  PPP_PROTOCOL_IPCP   = 1,
  PPP_PROTOCOL_AUTH   = 2,
  PPP_PROTOCOL_MAX
} ppp_protocol_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_FAIL_REASON_E_TYPE

DESCRIPTION
  Enum describing reason for PPP failure
---------------------------------------------------------------------------*/
typedef enum ppp_fail_reason_e_type
{
  PPP_FAIL_REASON_NONE            = 0,
  PPP_FAIL_REASON_TIMEOUT         = 1,
  PPP_FAIL_REASON_AUTH_FAILURE    = 2,
  PPP_FAIL_REASON_OPTION_MISMATCH = 3
} ppp_fail_reason_e_type;


/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_CBACK_F_PTR_TYPE

DESCRIPTION
  type of the callback function that is called when a PPP event occurs.  
---------------------------------------------------------------------------*/
typedef void (*ppp_event_cback_f_ptr_type)
(
  ppp_dev_enum_type       device,
  ppp_protocol_e_type     protocol,              /* PPP Protocol           */
  ppp_event_e_type        ppp_event,
  void                    *user_data,
  uint32                  session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
);

/*---------------------------------------------------------------------------
  Maximum length permissable for user id. Note that these should be equal to
  or greater than NV_MAX_PPP_USER_ID_LENGTH. The greater than case, may be
  necessary to store the TE2 user-id for network model packet calls.

  This must correspond to NV_MAX_PPP_USER_ID_LENGTH in nv.h
---------------------------------------------------------------------------*/
#define PPP_MAX_USER_ID_LEN 127

/*---------------------------------------------------------------------------
  Maximum length permissable for password used during authentication, i.e.
  PAP or CHAP. Note that this should be equal to or greater than
  NV_MAX_PAP_PASSWORD_LENGTH.

  Note that in network model, the Password used during resync for CHAP is
  different than that used initially.

  This must correspond to NV_MAX_PPP_PASSWORD_LENGTH in nv.h
---------------------------------------------------------------------------*/
#define PPP_MAX_PASSWD_LEN  127


/*---------------------------------------------------------------------------
  The maximum challenge length we support is 16 bytes as is the maximum
  length of the challenge name length.
---------------------------------------------------------------------------*/
#define PPP_CHAP_CHAL_LEN      16
#define PPP_CHAP_CHAL_NAME_LEN 16

/*---------------------------------------------------------------------------
  enum defining auth mode
---------------------------------------------------------------------------*/
enum
{
  PPP_NO_AUTH = 1,
  PPP_DO_PAP  = 2,
  PPP_DO_CHAP = 3
};

/*---------------------------------------------------------------------------
TYPEDEF PPP_AUTH_INFO_TYPE

DESCRIPTION
  This contains the authentication information (i.e. username & password)

  How this is to be used:
  All the lengths should indicate the  amount of data in the corresponding
  buffers. If nothing is stored they should be set to 0.
  Client Mode:
    user_id_info and passwd_info MUST be filled (i.e. are input parameters)
    in if was accept authentication. challenge_info and challenge_name are
    NOT used either as input or output.
  Server Mode:
    challenge_name SHOULD be filled in if doing CHAP - whatever is passed
    here will be in the challange name field of the CHAP challenge.  The
    other three fields are filled in when the client sends its information.
---------------------------------------------------------------------------*/
typedef struct
{
  char  user_id_info[PPP_MAX_USER_ID_LEN];
  char  passwd_info[PPP_MAX_PASSWD_LEN];
  char  challenge_info[PPP_CHAP_CHAL_LEN];
  char  challenge_name[PPP_CHAP_CHAL_NAME_LEN];
  uint8 user_id_len;
  uint8 passwd_len;
  uint8 challenge_len;
  uint8 challenge_name_len;
 #ifdef FEATURE_UIM_SUPPORT_3GPD
  uint8 nai_entry_index;
#endif /*FEATURE_UIM_SUPPORT_3GPD*/
} ppp_auth_info_type;

/*---------------------------------------------------------------------------
  define for the CHAP digests that we support
---------------------------------------------------------------------------*/
#define PPP_CHAP_MD5               5  /* Value of the MD5 algorithm        */

/*---------------------------------------------------------------------------
  defines for the PPP protocol field - the ones we care about
---------------------------------------------------------------------------*/
#define PPP_NO_PROTOCOL       0x0000  /* used for internal processing      */
#define PPP_IP_PROTOCOL       0x0021  /* Internet Protocol                 */
#define PPP_VJCH_PROTOCOL     0x002d  /* Van Jacobson Compressed TCP/IP    */
#define PPP_VJUCH_PROTOCOL    0x002f  /* Van Jacobson Uncompressed TCP/IP  */
#define PPP_IPCP_PROTOCOL     0x8021  /* Internet Protocol Control Protocol*/
#define PPP_LCP_PROTOCOL      0xc021  /* Link Control Protocol             */
#define PPP_PAP_PROTOCOL      0xc023  /* Password Authentication Protocol  */
#define PPP_CHAP_PROTOCOL     0xc223  /* Challenge Handshake Auth Protocol */

/*---------------------------------------------------------------------------
  defines for the PPP protocol field - the ones we don't care about
---------------------------------------------------------------------------*/
#define PPP_OSINL_PROTOCOL    0x0023  /* OSI Network Layer                 */
#define PPP_XNS_PROTOCOL      0x0025  /* Xerox NS IDP                      */
#define PPP_DN4_PROTOCOL      0x0027  /* DECnet Phase IV                   */
#define PPP_AT_PROTOCOL       0x0029  /* Appletalk                         */
#define PPP_IPX_PROTOCOL      0x002b  /* Novell IPX                        */
#define PPP_BPDU_PROTOCOL     0x0031  /* Bridging PDU                      */
#define PPP_ST2_PROTOCOL      0x0033  /* Stream Protocol (ST-II)           */
#define PPP_BV_PROTOCOL       0x0035  /* Banyan Vines                      */

#define PPP_8021D_PROTOCOL    0x0201  /* 802.1d Hello Packets              */
#define PPP_LUXCOM_PROTOCOL   0x0231  /* Luxcom                            */
#define PPP_SIGMA_PROTOCOL    0x0233  /* Sigma Network Systems             */

#define PPP_OSINLCP_PROTOCOL  0x8023  /* OSI Network Layer Control Protocol*/
#define PPP_XNSCP_PROTOCOL    0x8025  /* Xerox NS IDP Control Protocol     */
#define PPP_DN4CP_PROTOCOL    0x8027  /* DECnet Phase IV Control Protocol  */
#define PPP_ATCP_PROTOCOL     0x8029  /* Appletalk Control Protocol        */
#define PPP_IPXCP_PROTOCOL    0x802b  /* Novell IPX Control Protocol       */
#define PPP_BNCP_PROTOCOL     0x8031  /* Bridging NCP                      */
#define PPP_ST2CP_PROTOCOL    0x8033  /* Stream Protocol Control Protocol  */
#define PPP_BVCP_PROTOCOL     0x8035  /* Banyan Vines Control Protocol     */
#define PPP_CCP_PROTOCOL      0x80FD  /* Compression Control Protocol      */
#define PPP_ILCCP_PROTOCOL    0x80FB  /* Individual Link CCP               */

#define PPP_LQR_PROTOCOL      0xc025  /* Link Quality Report               */

#endif /* PS_PPP_DEFS_H */

