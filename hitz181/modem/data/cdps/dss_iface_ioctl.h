#ifndef DSS_IFACE_IOCTL_H
#define DSS_IFACE_IOCTL_H

/*===========================================================================

                          D S S _ I F A C E_I O C T L . H

DESCRIPTION

  The Protocol Services interface file. This contains variables and 
  definitions used by the various modules to access interface definitions.

  Copyright (c)2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_iface_ioctl.h_v   1.14   25 Feb 2003 14:24:10   ajithp  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dss_iface_ioctl.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/12/04    gr     Added IOCTLs to Get and Set the 1x RLP Fill Rate
07/07/03    atp    Renamed SDB flags for SDB support query ioctl, as part of
                   SDB enhancements.
05/05/03    aku    Added support for DSS_IFACE_IOCTL_GET_ALL_IFACES and added
                   the loopback interface to list of interfaces.
04/30/03    aku    Moved dss_get_iface_id() prototype from dssocket.h
03/05/03    aku    Removed feature definitions from header file and added 
                   void typdef for all IOCTLs that do not need to pass in an 
                   arg.
02/25/03    atp    Added ioctl for enabling/disabling holddown timer for 707.
02/12/03    aku    Updated comment.
02/12/03    atp    Made GO_ACTIVE, GO_DORMANT, GO_NULL ioctls common, not
                   707-specific. 
02/12/03    atp    Introduced new definition for 707 RLP NAK policy to avoid
                   include of dsrlp.h.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
02/07/03    usb    Added ioctls for reg and dereg inbound IP filters
02/06/03    atp    Added SDB support query ioctl for 707.
02/06/03    atp    Moved functionality of dsiface (w.r.t 707 - RLP NAK policy
                   dorm timer, MDR, QOS) to ioctls. dsiface to be deprecated.
01/31/03    atp    Added GoNull ioctl for 707.
01/17/03    atp    Added support for 707 ioctls.
01/17/03    usb    Defined all enums to be 32 bit wide, max value of enums 
                   changed from 0xFFFFFFFF to 0x7FFFFFFF, removed 
                   DSS_IFACE_IOCTL_ALL - all interfaces currently not 
                   supported for ioctls.
12/24/02    aku    Added support for reg/dereg event callbacks and added
                   support for specifying interface using name and instance.
11/23/02    aku    Removed support for sockfd as identifier and iface stats.    
11/17/02    aku    added interface statistics IOCTL
11/10/02    aku    created file
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#include "dserrno.h"

#include "ps_iface_defs.h"
#include "ps_ip_addr.h"
#include "ps_ipfltr_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Definition of various interface control operations. Please do NOT change 
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with 
  corresponding names) that are passed in or returned for get/set operations. 
  Note that all GET operations are assigned even numbers and SET operations 
  are assigned odd numbers. Also ensure that symetric operations are 
  grouped.  
---------------------------------------------------------------------------*/
typedef enum
{
  /*-------------------------------------------------------------------------
  
                              COMMON IOCTLS
                              
    The following operations are common to all interfaces. 
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MIN                     = -1,
  DSS_IFACE_IOCTL_GET_IPV4_ADDR           = 0, /* Get IPV4 addr of iface   */
  DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR  = 2, /* Get Primary DNS (IPV4) addr 
                                                                  of iface */
  DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR  = 4, /* Get Secondary DNS (IPV4) 
                                                             addr of iface */
  DSS_IFACE_IOCTL_GET_MTU                 = 6, /* Get MTU of iface         */
  DSS_IFACE_IOCTL_GET_STATE               = 8, /* Get state of iface       */
  DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE     = 10,/* Get physical link state  */
  DSS_IFACE_IOCTL_REG_EVENT_CB            = 12,/* Register callback for 
                                                  events. Note that app can 
                                                  have only one callback per 
                                                  interface for each event */
  DSS_IFACE_IOCTL_DEREG_EVENT_CB          = 13,/* Deregister event callback*/
  DSS_IFACE_IOCTL_GET_ALL_IFACES          = 14,/* Get all enabled ifaces. 
                                                  Note for that this IOCTL,
                                                  a NULL id_ptr has to be 
                                                  passed as a parameter    */   


  /*-------------------------------------------------------------------------
    The following operations are common, but implementation is specific to
    a particular iface. Some may not even be supported by a particular iface. 
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_GO_ACTIVE               = 101,/* Go active from dormant  */
  DSS_IFACE_IOCTL_GO_DORMANT              = 103,/* Go dormant              */
  DSS_IFACE_IOCTL_GO_NULL                 = 105,/* Go NULL                 */
  

  /*-------------------------------------------------------------------------
    IOCTLs which are interface specific should be enumerated below this.
    Note that the enuerated values from 0 - 199 are reserved for the common
    IOCTLs. All interface specific IOCTLs should use values greater than 199. 
    It is recommended that each interface have a certain range of values 
    reserved. This would ensure that all the interface specific operations
    are grouped together. For intance, CDMA2000 could use value b/w 200 and
    399, WCDMA 400 to 599 etc.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
                                
                                707 IOCTLS
                                
    The following operations are specific to 707 (1x) interface. Start 200.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_707_GET_MDR             = 200,/* 1x QCMDR value          */
  DSS_IFACE_IOCTL_707_SET_MDR             = 201,/* 1x QCMDR value          */
  DSS_IFACE_IOCTL_707_GET_DORM_TIMER      = 202,/* 1x dorm timer value     */
  DSS_IFACE_IOCTL_707_SET_DORM_TIMER      = 203,/* 1x dorm timer value     */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK= 204,/* 1x RLP curr NAK policy  */
  DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK= 205,/* 1x RLP curr NAK policy  */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK = 206,/* 1x RLP  default NAK     */
  DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK = 207,/* 1x RLP  default NAK     */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK = 208,/* 1x RLP  negotiated NAK  */
  DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI  = 210,/* 1x QOS nonassured prio  */
  DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI  = 211,/* 1x QOS nonassured prio  */
  DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY   = 212,/* 1x SDB possible query   */
  DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN     = 215,/* 1x enable/disable
                                                   holddown timer          */

  /*-------------------------------------------------------------------------

                              UMTS IOCTLS
                                
    The following operations are specific to UMTS interface.Start from 300.
  -------------------------------------------------------------------------*/


  /*-------------------------------------------------------------------------
                             RESERVED IOCTLs
    The following IOCTLs are reserved and should NOT be used by external 
    applications.                         
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_RESERVED_MIN            = 1000,/* Min reserved value     */
  /*-------------------------------------------------------------------------
                             707 RESERVED IOCTLs
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_707_GET_RLP_FILL_RATE   = 1200,/* 1x RLP fill rate - get  */
  DSS_IFACE_IOCTL_707_SET_RLP_FILL_RATE   = 1201,/* 1x RLP fill rate - set  */
  DSS_IFACE_IOCTL_RESERVED_MAX            = 1499, /* Max reserved value    */

  /*-------------------------------------------------------------------------

                              MORE COMMON STUFF
                              
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MAX,
  DSS_IFACE_IOCTL_NAME_FORCE_32_BIT       = 0x7FFFFFFF /* Force 32bit enum */
} dss_iface_ioctl_type;



/*---------------------------------------------------------------------------
                                
                                COMMON DATA STRUCTURES
                                
              The following data structs are for common ioctls.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Definition of iface names.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_IOCTL_CDMA_SN      = CDMA_SN_IFACE,
  DSS_IFACE_IOCTL_CDMA_AN      = CDMA_AN_IFACE,
  DSS_IFACE_IOCTL_UMTS         = UMTS_IFACE,
  DSS_IFACE_IOCTL_SIO          = SIO_IFACE,
  DSS_IFACE_IOCTL_LO           = LO_IFACE,
  DSS_IFACE_IOCTL_IFACE_FORCE_32_BIT = 0x7FFFFFFF /* Force 32bit enum type */
} dss_iface_ioctl_name_enum_type;

/*---------------------------------------------------------------------------
  Definition of identifying struct.  
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_ioctl_name_enum_type  name;
  uint32                          instance;
} dss_iface_ioctl_id_type;

/*---------------------------------------------------------------------------
  Definition of iface events.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_IOCTL_MIN_EV            =  IFACE_MIN_EV,
  DSS_IFACE_IOCTL_DOWN_EV           =  IFACE_DOWN_EV,
  DSS_IFACE_IOCTL_UP_EV             =  IFACE_UP_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV =  IFACE_PHYS_LINK_DOWN_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_UP_EV   =  IFACE_PHYS_LINK_UP_EV,
  DSS_IFACE_IOCTL_ADDR_CHANGED_EV   =  IFACE_ADDR_CHANGED_EV,
  DSS_IFACE_IOCTL_EVENT_MAX,
  DSS_IFACE_IOCTL_EVENT_FORCE_32_BIT = 0x7FFFFFFF /* Force 32bit enum type */
} dss_iface_ioctl_event_enum_type; 


/*---------------------------------------------------------------------------
  Data type used for Iface IPV4 address (DSS_IFACE_IOCTL_GET_IPV4_ADDR)
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv4_addr_type;

/*---------------------------------------------------------------------------
  Data type used for Iface IPV4 Primary DNS address 
  (DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR)
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv4_prim_dns_addr_type;

/*---------------------------------------------------------------------------
  Data type used for Iface IPV4 Primary DNS address 
  (DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR)
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_ipv4_seco_dns_addr_type;

/*---------------------------------------------------------------------------
  Data type used for Iface MTU (DSS_IFACE_IOCTL_GET_MTU)
---------------------------------------------------------------------------*/
typedef uint32 dss_iface_ioctl_mtu_type;

/*---------------------------------------------------------------------------
  Data type used for Iface State (DSS_IFACE_IOCTL_GET_STATE)
---------------------------------------------------------------------------*/
typedef ps_iface_state_enum_type dss_iface_ioctl_state_type;

/*---------------------------------------------------------------------------
  Data type used for Phys link State (DSS_IFACE_IOCTL_PHYS_LINK_STATE)
---------------------------------------------------------------------------*/
typedef phys_link_state_type dss_iface_ioctl_phys_link_state_type;

/*---------------------------------------------------------------------------
  Data types used for registering/deregistering for event callbacks 
  (DSS_IFACE_IOCTL_REG_EVENT_CB/DSS_IFACE_IOCTL_DEREG_EVENT_CB)
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Event info for DSS_IFACE_IOCTL_DOWN_EV and DSS_IFACE_IOCTL_UP_EV
---------------------------------------------------------------------------*/
typedef ps_iface_state_enum_type dss_iface_ioctl_event_info_type;

/*---------------------------------------------------------------------------
  Event info for DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV and 
  DSS_IFACE_IOCTL_PHYS_LINK_UP_EV
---------------------------------------------------------------------------*/
typedef phys_link_state_type dss_iface_ioctl_phys_link_event_info_type;

/*---------------------------------------------------------------------------
  Event info for DSS_IFACE_IOCTL_ADDR_CHANGED_EV
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_addr_change_event_info_type;

typedef union
{
  dss_iface_ioctl_event_info_type             iface_state_info;
  dss_iface_ioctl_phys_link_event_info_type   phys_link_state_info;
  dss_iface_ioctl_addr_change_event_info_type addr_change_info;
} dss_iface_ioctl_event_info_union_type;

/*---------------------------------------------------------------------------
  Typedef for event callback function
  (DSS_IFACE_IOCTL_REG_EVENT_CB/ DSS_IFACE_IOCTL_DEREG_EVENT_CB)
---------------------------------------------------------------------------*/
typedef void (*dss_iface_ioctl_event_cb)
(
  dss_iface_ioctl_event_enum_type          event,
  dss_iface_ioctl_event_info_union_type    event_info,
  void                                     *user_data,
  sint15                                   app_id,
  dss_iface_ioctl_name_enum_type           name,
  uint32                                   instance 
);

/*---------------------------------------------------------------------------
  Typedef for struct used with DSS_IFACE_IOCTL_REG_EVENT_CB and
  DSS_IFACE_IOCTL_DEREG_EVENT_CB. Note that the application must specify a 
  valid APP_ID when registering/deregistering for event callbacks.
---------------------------------------------------------------------------*/
typedef struct 
{
  dss_iface_ioctl_event_cb        event_cb;
  dss_iface_ioctl_event_enum_type event;
  void                            *user_data_ptr;
  sint15                          app_id;
} dss_iface_ioctl_ev_cb_type;


/*---------------------------------------------------------------------------
  Typedef for struct used with DSS_IFACE_IOCTL_GET_ALL_IFACES (Note that for 
  this IOCTL, a NULL id_ptr has to be passed as a parameter).
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned int            number_of_ifaces;
  dss_iface_ioctl_id_type ifaces[MAX_SYSTEM_IFACES];
} dss_iface_ioctl_all_ifaces_type;

/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_GO_ACTIVE , DSS_IFACE_IOCTL_GO_DORMANT
  and DSS_IFACE_IOCTL_GO_NULL (Note that apps do not have to define an arg
  of this type. They can just pass in Null).  
---------------------------------------------------------------------------*/
typedef void dss_iface_ioctl_null_arg_type;

/*---------------------------------------------------------------------------
                                
                                707 DATA STRUCTURES
                                
    The following data structs are specific to 707 (1x) interface.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Data type used for 707 QCMDR value
  (DSS_IFACE_IOCTL_707_GET/SET_MDR)
---------------------------------------------------------------------------*/
typedef unsigned int dss_iface_ioctl_707_mdr_type;

/*---------------------------------------------------------------------------
  Data type used for 707 dorm timer value
  (DSS_IFACE_IOCTL_707_GET/SET_DORM_TIMER)
---------------------------------------------------------------------------*/
typedef unsigned int dss_iface_ioctl_707_dorm_timer_type;

/*---------------------------------------------------------------------------
  Data type used for 707 RLP3 NAK policies (current, default, negotiated)
  (DSS_IFACE_IOCTL_707_GET/SET_RLP_ALL_CURR_NAK)
  (DSS_IFACE_IOCTL_707_GET/SET_RLP_ALL_DEF_NAK)
  (DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK)
  IOCTL_707_MAX_NAK_ROUNDS should be same as DSRLP_MAX_NAK_ROUNDS. Duplicate
  definition to avoid exposing dsrlp.h  by including here.
---------------------------------------------------------------------------*/
#define IOCTL_707_MAX_NAK_ROUNDS 3

typedef struct
{
  byte rscb_index;
  byte nak_rounds_fwd;
  byte naks_per_round_fwd[IOCTL_707_MAX_NAK_ROUNDS];
  byte nak_rounds_rev;
  byte naks_per_round_rev[IOCTL_707_MAX_NAK_ROUNDS];
} dss_iface_ioctl_707_rlp_opt_type;

/*---------------------------------------------------------------------------
  Data type used for 707 QOS non-assured priority (DSS_IFACE_IOCTL_GET_STATE)
  (DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI)
---------------------------------------------------------------------------*/
typedef byte dss_iface_ioctl_707_qos_na_pri_type;

/*--------------------------------------------------------------------------
  Data type used for 707 SDB support query.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 flags;       /* Same flag as used in socket write to specify SDB
                         (Eg: MSG_EXPEDITE, MSG_FAST_EXPEDITE, etc)        */
  boolean can_do_sdb; /* On return from ioctl, this boolean field will say
                         whether SDB supported or not                      */
} dss_iface_ioctl_707_sdb_support_query_type;

/*---------------------------------------------------------------------------
  Data type used for 707 holddown timer (DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN)
  Specify true for enabling timer, false for disabling.
---------------------------------------------------------------------------*/
typedef boolean dss_iface_ioctl_enable_holddown_type;

/*---------------------------------------------------------------------------
  The following data types are used in dss_get_iface_id()  
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_APP_ID  = 0,
  DSS_SOCK_FD = 1
} dss_id_type;

typedef struct
{
  dss_id_type type;
  union
  {
    sint15 app_id;
    sint15 sockfd;
  } info;
} dss_id_info_type;
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()

DESCRIPTION
  This function dstermines the ps_iface_ptr associated with the passed in
  identifier (App ID). It then calls ps_iface_ioctl().

DEPENDENCIES
  None.

PARAMETERS

  dss_iface_ioctl_id_type   - Interface ID on which the specified operations 
                              is to be performed
                          
  dss_iface_ioctl_type      - The operation name
  
  void*                     - Pointer to operation specific structure
  
  sint15*                   - Error code returned in case of failure (Error 
                              values are those defined in dserrno.h)
                              
                              DS_EBADF - Returned by dss_iface_ioctl() if the
                              specified id_ptr is invalid (i.e. id_ptr does 
                              not map to a valid ps_iface_ptr).
                              
                              DS_EINVAL - Returned by dss_iface_ioctl() when 
                              the specified IOCTL does not belong to the 
                              common set of IOCTLs and there is no IOCTL mode
                              handler registered for the specified interface.
                              
                              DS_EOPNOTSUPP - Returned by the lower level 
                              IOCTL mode handler when specified IOCTL is not 
                              supported by the interface. For instance, this 
                              would be returned by interfaces that do not 
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the 
                              implementation is mode specific, for example, 
                              GO_DORMANT).
                              
                              DS_EFAULT - This error code is returned if the 
                              specified arguments for the IOCTL are incorrect 
                              or if dss_iface_ioctl() or a mode handler 
                              encounters an error while executing the IOCTL..
                              For instance, if the 1X interface cannot 
                              "GO_DORMANT" it would return this error.                               
                              
                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int dss_iface_ioctl
( 
  dss_iface_ioctl_id_type  *id_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *dss_errno
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID()

DESCRIPTION
  This function is used to get the interface name and instance of a specific
  interface. The interface can be specified using either an app_id or a 
  sockfd. Note that the interface would be the one that the app/socket is
  currently using.

DEPENDENCIES
  None.

PARAMETERS
  dss_id_info_type id - Specifies whether an app_id or sockfd is being 
    as an identifier.
  dss_iface_ioctl_id_type *iface_id - Output parameter which contains the 
    name and instance of the iface associated with specified id.
 
RETURN VALUE
  0 on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int dss_get_iface_id
(
  dss_id_info_type id,
  dss_iface_ioctl_id_type *iface_id 
);
#endif /* FEATURE_DATA */
#endif /* DSS_IFACE_IOCTL_H */
