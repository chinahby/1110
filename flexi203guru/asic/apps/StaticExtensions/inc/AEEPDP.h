#ifndef AEE_PDP_H
#define AEE_PDP_H

/*============================================================================
FILE:  AEEPDP.h

SERVICES:  AEE PDP Profile manager.

GENERAL DESCRIPTION:
        Interface definitions, data structures, etc. for BREW PDP Profile Manager.

        Copyright © 2003 - 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEPDP.h#15 $
$DateTime: 2007/06/20 12:12:33 $
$Author: sanjinij $
$Change: 498942 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/07   sj      Addition of two flags:im_cn_flag and dhcp_pcscf flag
05/05/07   mjm     Removed all featurization, converted profile_name to dynamic
                   buffer, added profile_name_len, replaced enums with #defines
12/04/06   sj      Added enum types for PDP header compression and
                   Data compression types.
09/15/06   sj      Featurized pcscf addr changes with FEATURE_IMS
08/31/06   JAS     Removing AEEPDP_MAX_BR_STRING_LEN from featurization, so
                   that it is common.
08/08/06   sj      Added support for PCSCF address
05/17/06   JAS     Added customer.h to ensure correct featurization
04/26/06   np      Removed PACKED from struct for public AEE interface 
10/28/05   RI      Added new fields to AEEPDPContext structure.
08/02/05   sun     Added support for IPV6
05/26/05   sun     Fixed Macros
05/02/05   RI      Added QoS types.
02/09/05   rsb     Add FEATURE_PROFILE_OTAP_ENHANCEMENTS for UTF-16 profile_name
12/02/04   sun     Removed Featurization
10/22/04   sun     Fixed Compilation error
03/04/04   ram     QIS review changes.
03/01/04   ram     Removed unused typedefs.
01/20/04   ram     Created the header file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/*===========================================================================
  Constant definitions

===========================================================================*/
#define AEEPDP_MAX_APN_STRING_LEN    100   /* Max length of APN string    */
#define AEEPDP_MAX_QCPDP_STRING_LEN  32    /* Max len:username/password   */
#define AEEPDP_MAX_BR_STRING_LEN     9     /* Max length of Bit rate strings */

/*---------------------------------------------------------------------------
  PDP Type
---------------------------------------------------------------------------*/
#define AEEPDP_IP                    0     /* PDP type IP                 */
#define AEEPDP_PPP                   1     /* PDP type PPP                */
#define AEEPDP_IPV6                  2
#define AEEPDP_IP_ANY                3

typedef int AEEPDPType;

/*---------------------------------------------------------------------------
  Authentication Type for a PDP context
---------------------------------------------------------------------------*/
#define AEEPDP_AUTH_NONE             0     /* No authentication           */
#define AEEPDP_AUTH_PAP              1     /* PAP authentication          */
#define AEEPDP_AUTH_CHAP             2     /* CHAP authentication         */

typedef int AEEPDPAuthType;

/*---------------------------------------------------------------------------
  QOS Type
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_NONE              0     /* QOS None           */
#define AEEPDP_QOS_UMTS_REQ          1     /* UMTS QOS requested */
#define AEEPDP_QOS_GPRS_REQ          2     /* GPRS QOS requested */
#define AEEPDP_QOS_UMTS_MIN          3     /* UMTS QOS minimum   */
#define AEEPDP_QOS_GPRS_MIN          4     /* GPRS QOS minimum   */

typedef int AEEPDPQOSType;

/*---------------------------------------------------------------------------
  QOS Traffic Class Type
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_TCLASS_SUBSCRIBE  0     /* Subscribed                  */
#define AEEPDP_QOS_TCLASS_CONV       1     /* Conversational              */
#define AEEPDP_QOS_TCLASS_STRM       2     /* Streaming                   */
#define AEEPDP_QOS_TCLASS_INTR       3     /* Interactive                 */
#define AEEPDP_QOS_TCLASS_BACK       4     /* Background                  */

typedef int AEEPDPQOSTClass;

/*---------------------------------------------------------------------------
  QOS Delivery Order Type
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_DORDER_SUBSCRIBE  0     /* Subscribed                  */
#define AEEPDP_QOS_DORDER_YES        1     /* With delivery order         */
#define AEEPDP_QOS_DORDER_NO         2     /* Without delivery order      */

typedef int AEEPDPQOSDOrder;

/*---------------------------------------------------------------------------
  QOS SDU Error Codes  (per 3GPP TS24.008 10.5.6.5)
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_SDU_ERR_SUBSCRIBE 0     /* Subscribed                   */
#define AEEPDP_QOS_SDU_ERR_1E2       1     /* 1E-2                         */
#define AEEPDP_QOS_SDU_ERR_7E3       2     /* 7E-3                         */
#define AEEPDP_QOS_SDU_ERR_1E3       3     /* 1E-3                         */
#define AEEPDP_QOS_SDU_ERR_1E4       4     /* 1E-4                         */
#define AEEPDP_QOS_SDU_ERR_1E5       5     /* 1E-5                         */
#define AEEPDP_QOS_SDU_ERR_1E6       6     /* 1E-4                         */
#define AEEPDP_QOS_SDU_ERR_1E1       7     /* 1E-1                         */

typedef int AEEPDPQOSSDUError;

/*---------------------------------------------------------------------------
  QOS RES BER Codes  (per 3GPP TS24.008 10.5.6.5)
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_RES_BER_SUBSCRIBE 0     /* Subscribed                   */
#define AEEPDP_QOS_RES_BER_5E2       1     /* 5E-2                         */
#define AEEPDP_QOS_RES_BER_1E2       2     /* 1E-2                         */
#define AEEPDP_QOS_RES_BER_5E3       3     /* 5E-3                         */
#define AEEPDP_QOS_RES_BER_4E3       4     /* 4E-3                         */
#define AEEPDP_QOS_RES_BER_1E3       5     /* 1E-3                         */
#define AEEPDP_QOS_RES_BER_1E4       6     /* 1E-4                         */
#define AEEPDP_QOS_RES_BER_1E5       7     /* 1E-5                         */
#define AEEPDP_QOS_RES_BER_1E6       8     /* 1E-6                         */
#define AEEPDP_QOS_RES_BER_6E8       9     /* 6E-8                         */
                                               
typedef int AEEPDPQOSRESBER;

/*---------------------------------------------------------------------------
  QOS Erroneous SDU Delivery Options   (per 3GPP TS2.008 10.5.6.5)
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_SDU_DLVR_SUBSCRIBE 0    /* Subscribed                 */
#define AEEPDP_QOS_SDU_DLVR_NODETECT  1    /* No detection               */
#define AEEPDP_QOS_SDU_DLVR_YES       2    /* Erroneous SDU is delivered */
#define AEEPDP_QOS_SDU_DLVR_NO        3    /* Erroneous SDU not delivered*/

typedef int AEEPDPQOSSDUDlvr;

/*---------------------------------------------------------------------------
  QOS GPRS Precedence class 
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_PRECED_CLASS_SUB  0     /* Subscribed      */
#define AEEPDP_QOS_PRECED_CLASS_1    1
#define AEEPDP_QOS_PRECED_CLASS_2    2
#define AEEPDP_QOS_PRECED_CLASS_3    3

typedef int AEEPDPQOSPrecedenceClass;

/*---------------------------------------------------------------------------
  QOS GPRS Delay class 
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_DELAY_CLASS_SUB   0     /* Subscribed      */
#define AEEPDP_QOS_DELAY_CLASS_1     1
#define AEEPDP_QOS_DELAY_CLASS_2     2
#define AEEPDP_QOS_DELAY_CLASS_3     3
#define AEEPDP_QOS_DELAY_CLASS_4     4

typedef int AEEPDPQOSDelayClass;

/*---------------------------------------------------------------------------
  QOS GPRS Reliability class 
---------------------------------------------------------------------------*/
#define AEEPDP_QOS_REL_CLASS_SUB     0     /* Subscribed      */
#define AEEPDP_QOS_REL_CLASS_1       1
#define AEEPDP_QOS_REL_CLASS_2       2
#define AEEPDP_QOS_REL_CLASS_3       3

typedef int AEEPDPQOSRelClass;

/*---------------------------------------------------------------------------
  PDP header compression values
---------------------------------------------------------------------------*/
#define AEEPDP_UMTS_PDP_HEADER_COMP_OFF     0 /* PDP header compression is OFF.           */
#define AEEPDP_UMTS_PDP_HEADER_COMP_ON      1 /* Manufacturer preferred compression.      */
#define AEEPDP_UMTS_PDP_HEADER_COMP_RFC1144 2 /* PDP header compression based on rfc 1144.*/
#define AEEPDP_UMTS_PDP_HEADER_COMP_RFC2507 3 /* PDP header compression based on rfc 2507.*/
#define AEEPDP_UMTS_PDP_HEADER_COMP_RFC3095 4 /* PDP header compression based on rfc 3095.*/   

typedef byte AEEPDPHeaderCompType;

/*---------------------------------------------------------------------------
  PDP data compression types
---------------------------------------------------------------------------*/
#define AEEPDP_UMTS_PDP_DATA_COMP_OFF     0  /* PDP Data compression is OFF               */
#define AEEPDP_UMTS_PDP_DATA_COMP_ON      1  /* Manufacturer preferred compression.       */
#define AEEPDP_UMTS_PDP_DATA_COMP_V42_BIS 2  /* V.42BIS data compression                  */
#define AEEPDP_UMTS_PDP_DATA_COMP_V44     3  /* V.44 data compression                     */  

typedef byte AEEPDPDataCompType;

/*---------------------------------------------------------------------------
  Structure to store PDP authentication parameters
---------------------------------------------------------------------------*/
typedef struct
{
  AEEPDPAuthType  auth_type;          /* Authentication type   */
  AECHAR          password[AEEPDP_MAX_QCPDP_STRING_LEN+1];/*Passw/secret string */
  AECHAR          username[AEEPDP_MAX_QCPDP_STRING_LEN+1];/* Username string    */
} AEEPDPAuth;

/*---------------------------------------------------------------------------
  Structure to store UMTS Quality of Service  parameters.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean           valid_flg;                /* are the parameters set & valid*/
  AEEPDPQOSTClass   traffic_class;            /* Traffic Class               */
  uint32            max_ul_bitrate;           /* Maximum UL bitrate          */
  uint32            max_dl_bitrate;           /* Maximum DL bitrate          */
  uint32            gtd_ul_bitrate;           /* Guaranteed UL bitrate       */
  uint32            gtd_dl_bitrate;           /* Guaranteed DL bitrate       */
  AEEPDPQOSDOrder   dlvry_order;              /* SDU delivery order          */
  uint32            max_sdu_size;             /* Maximum SDU size            */
  AEEPDPQOSSDUError sdu_err;                  /* SDU error ratio list index  */
  AEEPDPQOSRESBER   res_biterr;               /* Residual biterr list index  */
  AEEPDPQOSSDUDlvr  dlvr_err_sdu;             /* Delivery of erroneous SDU   */
  uint32            trans_delay;              /* Transfer Delay              */
  uint32            thandle_prio;             /* Traffic handling priority   */
} AEEPDPUMTSQOS;

/*---------------------------------------------------------------------------
  Structure to store GPRS Quality of Service  parameters.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean   valid_flg;                        /* R the parameters set & valid*/
  AEEPDPQOSPrecedenceClass    precedence;     /* Precedence class            */
  AEEPDPQOSDelayClass         delay;          /* Delay class                 */
  AEEPDPQOSRelClass           reliability;    /* Reliability class           */
  uint32    peak;                             /* Peak throughput class       */
  uint32    mean;                             /* Mean throughput class       */
} AEEPDPGPRSQOS;

/*---------------------------------------------------------------------------
  IP versions
---------------------------------------------------------------------------*/
#define AEEPDP_IP_V4                 4
#define AEEPDP_IP_V6                 6

typedef int AEEPDPIPVer;

typedef struct
{
  union
  {
    uint8 addr8[16];
    uint16 addr16[8];
    uint32 addr32[4];
    uint64 addr64[2];
  }addr;
}AEEPDPAddrIPV6;


typedef struct
{
  AEEPDPIPVer ipVersion;
  union
  {
    uint32     pdp_addr_ipv4;
    AEEPDPAddrIPV6 pdp_addr_ipv6; 
  }pdp_addr;
  
}AEEPDPUMTSPDPAddr;
/*---------------------------------------------------------------------------
  Structure to store DNS parameters for the PDP profile
---------------------------------------------------------------------------*/
typedef struct
{
   AEEPDPUMTSPDPAddr   primary_dns_addr;                 /* primary DNS address         */
   AEEPDPUMTSPDPAddr   secondary_dns_addr;               /* secondary DNS address       */
} AEEPDPDNSAddress;

/*---------------------------------------------------------------------------
  Structure to store PDP context parameters 
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                valid_flg;                /* are the parameters set & valid      */
  AEEPDPType             pdp_type;                 /* PDP type (IP/PPP)               */
  AEEPDPHeaderCompType   h_comp;                   /* PDP Header Compression support      */
  AEEPDPDataCompType     d_comp;                   /* Data Compression support - v.42     */
  AEEPDPUMTSPDPAddr      pdp_addr;                 /* PDP address                         */
  AECHAR                 apn[AEEPDP_MAX_APN_STRING_LEN+1];  /* APN string              */
  boolean                secondary_flag;           /* this is a secondary profile         */
  uint8                  primary_id;               /* link to the primary  profile        */
  uint8                  pdp_context_number;       /* this is the same as profile number  */

} AEEPDPContext;


/*---------------------------------------------------------------------------
  PDP Profile definition
---------------------------------------------------------------------------*/
typedef  struct
{
  AECHAR            *profile_name;            /* Profile name */
  int               profile_name_len;         /* Profile name length  */
  AEEPDPContext     context;                  /* Context definition   */
  AEEPDPAuth        auth;                     /* Authentication info  */
  AEEPDPUMTSQOS     qos_request_umts;         /* UMTS QOS params:req  */
  AEEPDPUMTSQOS     qos_minimum_umts;         /* UMTS QOS params:min  */
  AEEPDPGPRSQOS     qos_request_gprs;         /* GPRS QOS Params:req  */
  AEEPDPGPRSQOS     qos_minimum_gprs;         /* GPRS QOS params:min  */
  AEEPDPDNSAddress  dns_addr;                 /* DNS addr.-user specfied */
  boolean           req_pcscf_addr;
  boolean           dhcp_pcscf_addr;          /* PCSCF Addr using DHCP */
  boolean           im_cn_flag;               /* IMCN (IP Multimedia Core Network)Flag */
} AEEPDP;

#define AEEPDP_PROFILE_RESET_VALUE              0x10000000
#define AEEPDP_PROFILE_SET_TO_DEFAULT           0x20000000

#define AEEPDP_PROFILE_NAME             0x00000002
#define AEEPDP_PROFILE_ADDRESS          0x00000004
#define AEEPDP_PROFILE_APN              0x00000008
#define AEEPDP_PROFILE_DNS_PRIMARY      0x00000010
#define AEEPDP_PROFILE_DNS_SECONDARY    0x00000020
#define AEEPDP_PROFILE_AUTH_TYPE        0x00000040
#define AEEPDP_PROFILE_AUTH_USERNAME    0x00000080
#define AEEPDP_PROFILE_AUTH_PASSWORD    0x00000100
#define AEEPDP_PROFILE_CONTEXT          0x00000200
#define AEEPDP_PROFILE_CONTEXT_VALID    0x00000400
#define AEEPDP_PROFILE_UMTS_REQ_QOS     0x00000800
#define AEEPDP_PROFILE_UMTS_MIN_QOS     0x00001000
#define AEEPDP_PROFILE_GPRS_REQ_QOS     0x00002000
#define AEEPDP_PROFILE_GPRS_MIN_QOS     0x00004000
#define AEEPDP_PROFILE_PCSCF_ADDR_REQ   0x00008000
#define AEEPDP_PROFILE_IM_CN_FLAG       0x00010000
#define AEEPDP_PROFILE_DHCP_PCSCF_ADDR  0x00020000

#define AEE_QOS_UMTS_MAX_KBR            14000
#define AEE_QOS_GPRS_MAX_KBR             2048

#define AEEPDP_PROFILE_ALL_DATA         0x0FFFFFFF
#define AEEPDP_PROFILE_DNS              (AEEPDP_PROFILE_DNS_PRIMARY | \
                                        AEEPDP_PROFILE_DNS_SECONDARY)

#define AEEPDP_PROFILE_AUTH             (AEEPDP_PROFILE_AUTH_TYPE | \
                                        AEEPDP_PROFILE_AUTH_USERNAME | \
                                         AEEPDP_PROFILE_AUTH_PASSWORD)

typedef struct IPDP IPDP;
//*****************************************************************************
//
// IPDP Interface
//
//*****************************************************************************
#define AEEIID_PDP    	0x01049684

AEEINTERFACE(IPDP)
{
   INHERIT_IQueryInterface(IPDP);

   int  (*GetParm)(IPDP * pIPDP, byte profileNum, uint32 dwFields, AEEPDP *pdpProfile);
   int  (*SetParm)(IPDP * pIPDP, byte profileNum, uint32 dwFields, AEEPDP *pdpProfile);
   int  (*GetNumProfiles)(IPDP *pIPDP);
   int  (*GetDefaultProfileNum)(IPDP * pIPDP, byte *pProfileNum);
   int  (*SetDefaultProfileNum)(IPDP * pIPDP, byte profileNum);
   int  (*GetMaxProfileNameLen)(IPDP * pIPDP);

};

#define IPDP_AddRef(p)                       AEEGETPVTBL(p, IPDP)->AddRef(p)
#define IPDP_Release(p)                      AEEGETPVTBL(p, IPDP)->Release(p)
#define IPDP_QueryInterface(p, i, p2)        AEEGETPVTBL(p, IPDP)->QueryInterface(p, i, p2)


#define IPDP_GetParm(pIPDP, profileNum, dwFields, pProfile)     \
  AEEGETPVTBL(pIPDP, IPDP)->GetParm(pIPDP, profileNum, dwFields, pProfile)
#define IPDP_SetParm(pIPDP, profileNum, dwFields, pProfile)     \
  AEEGETPVTBL(pIPDP, IPDP)->SetParm(pIPDP, profileNum, dwFields, pProfile)
#define IPDP_GetNumProfiles(pIPDP)            \
  AEEGETPVTBL(pIPDP, IPDP)->GetNumProfiles(pIPDP)
#define IPDP_GetDefaultProfileNum(pIPDP, pProfileNum)           \
  AEEGETPVTBL(pIPDP, IPDP)->GetDefaultProfileNum(pIPDP, pProfileNum)
#define IPDP_SetDefaultProfileNum(pIPDP, num)            \
  AEEGETPVTBL(pIPDP, IPDP)->SetDefaultProfileNum(pIPDP, num)
#define IPDP_GetMaxProfileNameLen(pIPDP)            \
  AEEGETPVTBL(pIPDP, IPDP)->GetMaxProfileNameLen(pIPDP)

/*=============================================================================
FUNCTION: IPDP_SetParm

DESCRIPTION:  
  
  Set the PDP parameters.

PARAMETERS:
  profileNum[in] : The Profile number for which we want the data.
  dwFields[in]   : The flag which specifies the profile parameters to set.
                   It should be a valid combination of one of these.
                      AEEPDP_PROFILE_NAME           : Set the profile name.
                      AEEPDP_PROFILE_ADDRESS        : Set the profile address.
                      AEEPDP_PROFILE_APN            : Set APN.
                      AEEPDP_PROFILE_DNS_PRIMARY    : Set Primary DNS Address.
                      AEEPDP_PROFILE_DNS_SECONDARY  : Set Secondary DNS Address.
                      AEEPDP_PROFILE_AUTH_TYPE      : Set Authentication type.
                      AEEPDP_PROFILE_AUTH_USERNAME  : Set Authentication username.
                      AEEPDP_PROFILE_AUTH_PASSWORD  : Set Authentication password.
                      AEEPDP_PROFILE_CONTEXT        : Set Profile Context.
                      AEEPDP_PROFILE_UMTS_REQ_QOS   : Set UMTS required QOS
                      AEEPDP_PROFILE_UMTS_MIN_QOS   : Set UMTS minimum QOS
                      AEEPDP_PROFILE_GPRS_REQ_QOS   : Set GPRS required QOS
                      AEEPDP_PROFILE_GPRS_MIN_QOS   : Set GPRS minimum QOS
                      AEEPDP_PROFILE_ALL_DATA       : Set all the data.
                      
                    These fields could be combined with one of the following flags
                      AEEPDP_PROFILE_SET_TO_DEFAULT : Set the requested field(s) to default.
                      AEEPDP_PROFILE_RESET_VALUE    : Set the requested field(s) to be invalid.
                    If none of the above flags is specified, then the values provided in the 
                    structure are used.
                    
                    The valid fields to be used with AEEPDP_PROFILE_SET_TO_DEFAULT are combination of
                      AEEPDP_PROFILE_ALL_DATA
                      AEEPDP_PROFILE_AUTH
                      AEEPDP_PROFILE_DNS
                      AEEPDP_PROFILE_CONTEXT

                    The valid fields to be used with AEEPDP_PROFILE_RESET_VALUE are combination of
                      AEEPDP_PROFILE_ALL_DATA
                      AEEPDP_PROFILE_UMTS_REQ_QOS
                      AEEPDP_PROFILE_UMTS_MIN_QOS
                      AEEPDP_PROFILE_GPRS_REQ_QOS
                      AEEPDP_PROFILE_GPRS_MIN_QOS
                      AEEPDP_PROFILE_CONTEXT

  pProfile[in]: The requested parameters are found here.

  RETURN VALUE:
  AEE_SUCCESS:  If the requested parameters were set in the registry.
  EBADPARM:     If input parameter is NULL or the dwFields is not a
                valid combination of the flags.
  EFAILED:      Otherwise.

COMMENTS:
  When setting the profile name (specified by AEEPDP_PROFILE_NAME), the
  profile_name member must be non-NULL and the profile_name_len member
  must specify the size of the profile_name buffer.

SIDE EFFECTS:

SEE ALSO:
  IPDP_GetParm
=============================================================================*/

/*=============================================================================
FUNCTION: IPDP_GetParm

DESCRIPTION:  
  Get the PDP Parameters

PARAMETERS:
  profileNum[in] : The Profile number for which we want the data.

  dwFields[in]   : The flag which specifies the profile parameters to get.
                   It should be a valid combination of one of these.
                      AEEPDP_PROFILE_NAME           : Get the profile name.
                      AEEPDP_PROFILE_ADDRESS        : Get the profile address.
                      AEEPDP_PROFILE_APN            : Get APN.
                      AEEPDP_PROFILE_DNS_PRIMARY    : Get Primary DNS Address.
                      AEEPDP_PROFILE_DNS_SECONDARY  : Get Secondary DNS Address.
                      AEEPDP_PROFILE_AUTH_TYPE      : Get Authentication type.
                      AEEPDP_PROFILE_AUTH_USERNAME  : Get Authentication username.
                      AEEPDP_PROFILE_AUTH_PASSWORD  : Get Authentication password.
                      AEEPDP_PROFILE_CONTEXT        : Get Profile Context.
                      AEEPDP_PROFILE_CONTEXT_VALID  : Get if Context is valid.
                      AEEPDP_PROFILE_UMTS_REQ_QOS   : Get UMTS required QOS
                      AEEPDP_PROFILE_UMTS_MIN_QOS   : Get UMTS minimum QOS
                      AEEPDP_PROFILE_GPRS_REQ_QOS   : Get GPRS required QOS
                      AEEPDP_PROFILE_GPRS_MIN_QOS   : Get GPRS minimum QOS
                      AEEPDP_PROFILE_ALL_DATA       : Get all the data.

  pProfile[out]: The requested parameters are stored here.

  RETURN VALUE:
  AEE_SUCCESS:  If the requested parameters were retrieved from the pdp registry.
  EBADPARM:     If input parameter is NULL or the dwFields is not a
                valid combination of the flags.
  EFAILED:      Otherwise.

COMMENTS:
  When getting the profile name (specified by AEEPDP_PROFILE_NAME), the
  profile_name member of the PDP structure is examined first to determine what
  action to take:
    - if it is NULL, the profile_name_len member will be filled in with the
      number of bytes required for the buffer. The function will return ENEEDMORE.
    - If it is non-NULL, the buffer is filled up to the number of bytes specified
      in profile_name_len.

SIDE EFFECTS:

SEE ALSO:
  IPDP_SetParm
=============================================================================*/

/*===========================================================================
FUNCTION IPDP_GetNumProfiles

DESCRIPTION
  Get the total number of profiles supported.

DEPENDENCIES
  None

RETURN VALUE:
  returns the total number of profiles supported.
SIDE EFFECTS
  None
===========================================================================*/

/*=============================================================================
FUNCTION: IPDP_GetDefaultProfileNum

DESCRIPTION:  
  
  Get the profile number that is used for socket calls as a default.

PARAMETERS:
  pProfileNum[out] : The default profile number used for socket calls is stored here.
  
  
RETURN VALUE:
  AEE_SUCCESS:  If the profile number was retrieved sucessfully.
  EBADPARM:     If pProfileNum parameter is NULL
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  
=============================================================================*/

/*=============================================================================
FUNCTION: IPDP_SetDefaultProfileNum

DESCRIPTION:  
  
  Set the profile number that is used for socket calls as a default profile number.

PARAMETERS:
  pProfileNum[in] : The default profile number used for socket calls to be set.
  
  
RETURN VALUE:
  AEE_SUCCESS:  If the profile number was set sucessfully.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  
=============================================================================*/

/*===========================================================================
FUNCTION IPDP_GetMaxProfileNameLen

DESCRIPTION
  Returns the max length of profile name.

DEPENDENCIES
  None

RETURN VALUE:
  Max length of profile name in characters (not bytes), not including the
  null-terminating character.

SIDE EFFECTS
  None
===========================================================================*/



#endif //AEE_PDP_H
