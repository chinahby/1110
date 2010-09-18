#ifndef DSS_NETPOLICY_H
#define DSS_NETPOLICY_H
/*===========================================================================

         D A TA   S E R V I C E S   N E T W O R K   P O L I C Y  I N F O   
                       H E A D E R  F I L E
                   
DESCRIPTION

 The Network policy info Header File contains shared variables 
 and enums, as well as declarations for functions related to network polciy.

Copyright (c) 2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dss_netpolicy.h#2 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/25/08    sp     Newly added to ULC from SC1x code base. 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS
#include "ps_iface_defs.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                    Definition of iface names.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_CDMA_SN      = CDMA_SN_IFACE,
  DSS_IFACE_CDMA_AN      = CDMA_AN_IFACE,
  DSS_IFACE_UMTS         = UMTS_IFACE,
  DSS_IFACE_SIO          = SIO_IFACE,
  DSS_IFACE_LO           = LO_IFACE,
  DSS_IFACE_WWAN         = WWAN_GROUP,
  DSS_IFACE_ANY_DEFAULT  = ANY_DEFAULT_GROUP,
  DSS_IFACE_ANY          = ANY_IFACE_GROUP,
  DSS_IFACE_RM           = RM_GROUP
} dss_iface_name_enum_type;


/*---------------------------------------------------------------------------
                       Interface Id structure
---------------------------------------------------------------------------*/

typedef uint32 dss_iface_id_type;

typedef enum
{
  DSS_IFACE_ID = 0,
  DSS_IFACE_NAME = 1
} dss_iface_id_kind_enum_type;

/*---------------------------------------------------------------------------
  dss_iface_id_type will be treated as opaque and typedef'ed to int
  16 bits for iface name, 8 bits for instance and 8 for phys link
  the user will get the iface_id by supplying iface and instance to a 
  function that will return dss_iface_id_type. an attempt to get the
  iface_id for "ANY" iface will return error.
---------------------------------------------------------------------------*/

typedef struct
{
  dss_iface_id_kind_enum_type kind;
  union
  {
    dss_iface_id_type id;
    dss_iface_name_enum_type name;
  } info;
} dss_iface_type;

/*---------------------------------------------------------------------------
                       Network policy data structures.
---------------------------------------------------------------------------*/

typedef enum
{
  DSS_IFACE_POLICY_ANY       = 0,
  DSS_IFACE_POLICY_UP_ONLY   = 1,
  DSS_IFACE_POLICY_UP_SPEC   = 2 
} dss_iface_policy_flags_enum_type;

typedef struct
{
  dss_iface_policy_flags_enum_type policy_flag; /* Desired policy behavior */ 
  dss_iface_type                   iface;                 /* Desired IFACE */
  boolean                          ipsec_disabled;  /* Is IPSEC disabled ? */
  int                              family;   /* ipv4 or ipv6 or don't care */
  struct 
  {
    int pdp_profile_num;
  } umts;                              /* UMTS specific policy information */

  /* This field is significant, must be set by the Application */
  struct
  {
    int data_session_profile_id;
  } cdma;

  struct 
  {
    int cookie;
  } dss_netpolicy_private;                       /* Private cookie for internal purposes */
} dss_net_policy_info_type;

//#define DSS_GET_IFACE_ID_BY_POLICY_VERS (1)

#endif  /* FEATURE_DATA_PS */

#endif /* DSS_NETPOLICY_H */
