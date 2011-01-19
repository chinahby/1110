#ifndef DSIFACE_DEF_H
#define DSIFACE_DEF_H
/*===========================================================================

                          D S I F A C E _ D E F . H

DESCRIPTION

  The Data Services interface file. This contains variables and definitions
  used by the various modules to access interface definitions.
  
  Copyright (c)2001-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsiface_def.h_v   1.1   13 Nov 2002 14:39:18   rchar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dsiface_def.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/13/02    rc     Wrapped include of dsrlp.h in !FEATURE_GSM_GPRS.
04/17/02    rc     Wrapped include of dsrlp.h in !FEATURE_DATA_WCDMA_PS.
03/13/02    aku    Added new iface_state APP_IFACE_OPEN_PENDING 
02/25/02    atp    Added support for non-assured QOS API.
12/21/01    dwp    Wrap include of dsrlp.h in !FEATURE_DATA_PS.
12/18/01    snn    Added FEATURE_DS_RLP3 under dsiface_rlp_opt_type.
12/16/01    snn    Modified the way in which dsrlp_opt structure is accessed.
12/03/01    snn    Added a new define for RLP option
12/01/01    hcg    Added IFACE_UM_HDR_AN_PPP_STACK for 1xEV Access Network
                   interface.
11/29/01    mvl    moved all public dsiface interface function declarations
                   into this file from the various places they were.
07/25/01    mvl    modified so RM value is only defined if net model is on.
07/19/01    sjy    created file
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_DS_RLP3
#include "dsrlp.h"
#endif


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Enum for the PPP stack - on Rm interface or Um interface.
---------------------------------------------------------------------------*/
typedef  enum
{
  IFACE_NO_STACK     = -1,           /* the lower bound value              */
  IFACE_UM_PPP_STACK = 0,            /* The Um interface                   */
#ifdef FEATURE_DS_NET_MODEL
  IFACE_RM_PPP_STACK = 1,            /* the Rm interface                   */
#endif /* FEATURE_DS_NET_MODEL */
#ifdef FEATURE_HDR_AN_AUTH
#error code not present
#endif /* FEATURE_HDR_AN_AUTH */
  IFACE_MAX_STACK                    /* max value of enum, bounds checking */
} iface_stack_enum_type;

/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
typedef enum
{
  DSIFACE_BAD_OPT_NAME = -1, /* Option is not defined. */
  DSIFACE_BAD_IFACE    = -2, /* Action not supported at the specified */
                             /* iface, or iface not defined.          */
  DSIFACE_BAD_PARAM    = -3, /* Pointers to parameters not defined.   */
  DSIFACE_BAD_LEN      = -4, /* Invalid length.                       */
  DSIFACE_BAD_ARG_VAL  = -5, /* Arg has invalid value (or if the arg  */
                             /* value causes other internal errors.   */
  DSIFACE_SUCCESS      = 0
}dsiface_error_type;

/*---------------------------------------------------------------------------
  Definition for various options that affect the behaviors of data service
  interface. Only the options whose names are defined here are supported.
  They can be modified and retrieved by dsiface_set_opt() and 
  dsiface_get_opt(), respectively. The following definition is also used as 
  index to the table of all supported options.
---------------------------------------------------------------------------*/
typedef enum
{
  DSIFACE_OPT_MIN = -1,  
  DSIFACE_OPT_MDR = 0,             
  DSIFACE_OPT_DORM_TIMER = 1,
  DSIFACE_OPT_RLP_ALL_CUR_NAK = 2,
  DSIFACE_OPT_RLP_ALL_DEF_NAK = 3,
  DSIFACE_OPT_RLP_ALL_NEG_NAK = 4,
  DSIFACE_OPT_CTLHLD_TIMER = 5,
#ifdef FEATURE_IS2000_REL_A
  DSIFACE_OPT_RLP_QOS_PRI =6,
#endif /* FEATURE_IS2000_REL_A */
  DSIFACE_OPT_MAX             /* determine upper bound and array size */
}dsifacei_opt_names_type;

/*---------------------------------------------------------------------------
  Definition for various interface I/O control options that can be modified
  through dsiface_ioctl(). Only the options whose names are defined here 
  are supported. The following definition is also used as index to the 
  reference table of all supported options.
---------------------------------------------------------------------------*/
typedef enum
{
  DSIFACE_IOCTL_MIN = -1,
  DSIFACE_IOCTL_FORCEDORM = 0,
  DSIFACE_IOCTL_TRYCTLHLD = 1,
  DSIFACE_IOCTL_MAX              /* determines upper bound and array size */
}dsifacei_ioctl_type;

/*---------------------------------------------------------------------------
  Structure for modifying RLP setup
  
  rscb_index:        Value of the SR_ID. For now fixed so always program to  
                     a constant value. Currently its value should be only 0
  nak_rounds_fwd:    Contains forward nak rounds. This is a byte. 
                     Range: 0 to DSRLP_MAX_NAK_ROUNDS (3).
  naks_per_rnds_fwd: Contains forward Naks/round. Each element of 
                     the array corresponds to the round 1,2,3 respectively. 
                     *(naks_per_rnds_fwd+0) gives NAKs in 1st round
                     *(naks_per_rnds_fwd+1) gives NAKs in 2nd round
                     *(naks_per_rnds_fwd+2) gives NAKs in 3rd round
  nak_rounds_rev:    Contains forward nak rounds. This is a byte. 
                     Range: 0 to DSRLP_MAX_NAK_ROUNDS.
  naks_per_rnds_rev: Contains reverse Naks/round. This is to be a 
                     DSRLP_MAX_NAK_ROUNDS byte array. Each element of the 
                     array corresponds to the round 1,2,3 respectively. 
                     If nak_rounds_rev is 0 then donot read/write to this. 
                     To obtain the values always read nak_rnds_rev_ptr value 
                     and obtain values from this array.
                     *(naks_per_rnds_fwd+0) gives NAKs in 1st round
                     *(naks_per_rnds_fwd+1) gives NAKs in 2nd round
                     *(naks_per_rnds_fwd+2) gives NAKs in 3rd round
---------------------------------------------------------------------------*/
#ifdef FEATURE_DS_RLP3
typedef struct
{
    byte rscb_index; 
    byte nak_rounds_fwd;
    byte naks_per_round_fwd[DSRLP_MAX_NAK_ROUNDS];
    byte nak_rounds_rev;
    byte naks_per_round_rev[DSRLP_MAX_NAK_ROUNDS];
} dsiface_rlp_opt_type;
#endif /* FEATURE_DS_RLP3 */


/*---------------------------------------------------------------------------
  Enumerated type for call_state                      
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_NOT_DORMANT = 0,             /* Iface is not dormant               */
  IFACE_DORMANT     = 1,             /* Iface is dormant                   */
  IFACE_CTRL_HOLD   = 2              /* Iface is in control hold           */
} iface_call_state_type;

/*---------------------------------------------------------------------------
                      Interface Control Block Structure
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_OPEN    = 0,                                 /* Interface open     */
  IFACE_OPENING = 1,                                 /* Interface opening  */
  IFACE_CLOSED  = 2,                                 /* Interface closed   */
  IFACE_CLOSING = 3,                                 /* Interface closing  */
  APP_IFACE_OPEN_PENDING = 4                     /* Interface Open pending */
} iface_state;

/*---------------------------------------------------------------------------
  Enumerated type enlisting the various close reasons
  INVALID_PPP_CLOSE - Initial value 
  PDSN_INIT_PPP_CLOSE - PPP closure initiated by PDSN. 
  APP_INIT_PPP_CLOSE - Applcaition initated PPP closure.
  MIP_FAIL_PPP_CLOSE - This close reason indicates that MIP failed to come
  due to some reason. One of the reasons could be that the PDSN that the 
  mobile connected to does not support MIP. 
  MIP_BAD_NV_PPP_CLOSE - This closure reason is returned if the MIP related
  NV parameters are not properly configured.
  SO_REJECT_PPP_CLOSE - This closusre reason indicates that the mobile 
  received a SO_REJECT from the base station. The application could then 
  change the QCMDR value and reoriginate.
  RADIO_LINK_INIT_PPP_CLOSE - This close reason is returned if the mobile 
  failed to establish the traffic channel. This usually indicates a bad 
  coverage area. The application could attempt to setup a call sometime 
  later.
  MIP_UNSUPPORTED_PPP_CLOSE - This close reason is returned if one of the
  following conditions 
     1) P_REV is less than 6
     2) QCMDR is not set to 3
     3) The MIP meta sm on the phone is not initialized 
---------------------------------------------------------------------------*/
typedef enum 
{
  INVALID_PPP_CLOSE         = 0,    /* Start of PPP close types            */
  PDSN_INIT_PPP_CLOSE       = 1,    /* PDSN initiated PPP close            */
  APP_INIT_PPP_CLOSE        = 2,    /* APP initiated PPP close             */
  MIP_FAIL_PPP_CLOSE        = 3,    /* MIP initiated PPP close             */
  MIP_BAD_NV_PPP_CLOSE      = 4,    /* MIP BAD NV PPP close                */
  SO_REJECT_PPP_CLOSE       = 5,    /* Recd Service Option Rej from BS     */
  RADIO_LINK_INIT_PPP_CLOSE = 6,    /* Bad Radio link initiated PPP close  */
  MIP_UNSUPPORTED_PPP_CLOSE = 7,    /* MIP not supported in current config */
  END_PPP_CLOSE             = 8     /* End of PPP close types              */
} ppp_close_reason;


typedef struct
{
  iface_call_state_type call_state;   /* indicates if dormant/control hold */
  iface_state           state;        /* state of the interface            */
  ppp_close_reason      close_reason; /* Reason for the last PPP closure   */
  boolean               iface_has_mip;/* indicates if interface has M.IP   */
} dsiface_status_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             MACRO  DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#ifdef FEATURE_DS_NET_MODEL
#define SWAP_IFACE(iface) \
  ((iface_stack_enum_type) \
   ((iface) == IFACE_UM_PPP_STACK ? IFACE_RM_PPP_STACK : IFACE_UM_PPP_STACK))
#endif /* FEATURE_DS_NET_MODEL */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSIFACE_GET_STATUS()

DESCRIPTION
  This function retrieves interface specific information and stores it in 
  the given interface control block.
 
  NOTE : This function is wrapper function for the dssnet_get_iface_status
  function implemented in dssnet.c
    
DEPENDENCIES
  None. 

PARAMETERS  
  iface             - the interface in question
  iface_status_type - the interface control block to be filled in   

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void dsiface_get_status
(
  iface_stack_enum_type  iface,
  dsiface_status_type   *iface_status
);


/*===========================================================================
FUNCTION DSIFACE_SET_OPT()

DESCRIPTION
  This function can be called by applications or various internal modules
  (e.g. AT Command Processor) to modify values of data service interface
  options.

DEPENDENCIES
  None.

PARAMETERS
  iface_stack_enum_type iface      - interface where the option is to be set
  dsifacei_opt_names_type optname  - Option name.
  void *optval                     - Pointer to the option value.
  int optlen                       - Size of the option value.

RETURN VALUE
  DSIFACE_BAD_OPT_NAME - Option is not defined. 
  DSIFACE_BAD_IFACE    - Action not supported at the specified iface, or 
                         iface not defined.          
  DSIFACE_BAD_PARAM    - Pointers to parameters not defined.  
  DSIFACE_BAD_LEN      - Invalid length.                      
  DSIFACE_BAD_ARG_VAL  - Arg has invalid value (or if the arg value causes 
                         other internal errors.   
  DSIFACE_SUCCESS      - otherwise


SIDE EFFECTS
  None.

===========================================================================*/
dsiface_error_type dsiface_set_opt
(
  iface_stack_enum_type iface,  /* interface stack type (PPP Um or PPP Rm) */
  dsifacei_opt_names_type optname,       /* option name                    */
  void *optval,                          /* value of the option            */
  int optlen                             /* size of the option legth       */
);

/*===========================================================================
FUNCTION DSIFACE_GET_OPT()
  
DESCRIPTION
  This function can be called by applications or various internal modules
  (e.g. AT Command Processor) to retrieve the values of data service 
  interface options.

DEPENDENCIES
  None.

PARAMETERS
  iface_stack_enum_type iface     - interface where the option is to be set
  dsifacei_opt_names_type optname - Option name.
  void *optval                    - Pointer to the buffer where the return
                                    value is to be stored.
  int *optlen                     - Size of the buffer going in;
                                    size of the return value going out.

RETURN VALUE
  DSIFACE_BAD_OPT_NAME - Option is not defined.
  DSIFACE_BAD_IFACE    - Action not supported at the specified iface, or
                         iface not defined.
  DSIFACE_BAD_PARAM    - Pointers to parameters not defined.
  DSIFACE_BAD_LEN      - Invalid length.
  DSIFACE_BAD_ARG_VAL  - Arg has invalid value (or if the arg value causes
                         other internal errors.
  DSIFACE_SUCCESS      - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
dsiface_error_type dsiface_get_opt
(
  iface_stack_enum_type iface,  /* interface stack type (PPP Um or PPP Rm) */
  dsifacei_opt_names_type optname,       /* option name                    */
  void *optval,                          /* value of the option            */
  int *optlen                            /* size of the option legth       */
);

/*===========================================================================
FUNCTION DSIFACE_IOCTL()

DESCRIPTION
  This API performs various control functions on data service interface.

DEPENDENCIES
  None.

PARAMETERS
  iface_stack_enum_type iface - interface where the ioctl option is to be 
                                set
  int ioctlname,              - Option name.
  void *argval                - Pointer to the argument
  int arglen                  - Size of the argument

RETURN VALUE
  DSIFACE_BAD_OPT_NAME - Option is not defined.
  DSIFACE_BAD_IFACE    - Action not supported at the specified iface, or
                         iface not defined.
  DSIFACE_BAD_PARAM    - Pointers to parameters not defined.
  DSIFACE_BAD_LEN      - Invalid length.
  DSIFACE_BAD_ARG_VAL  - Arg has invalid value (or if the arg value causes
                         other internal errors.
  DSIFACE_SUCCESS      - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
dsiface_error_type dsiface_ioctl
(
  iface_stack_enum_type iface,
  dsifacei_ioctl_type ioctlname,
  void *argval,
  int arglen
);
#endif /* DSIFACE_DEF_H */

