/*===========================================================================

                      D S S _ I F A C E. C
                   
DESCRIPTION
  This file contains functions used by various modules to access
  network interface. 

  NOTE: these functions are DEPRICATED and should not be used, instead the
  equivalient dss_iface functions (or combinations) should be used.

EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2008 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsiface.c_v   1.2   08 Nov 2002 11:57:48   ubabbar  $ 
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/dsiface.c#2 $  $DateTime: 2008/06/24 01:30:38 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/18/08    sn     Fixed compiler warnings.
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
08/09/06    az     Fixed compiler warnings (Checkin for Anuj)
05/05/03    jd     Moved logic for dsiface_get_status into this file to free
                   ps707_dssnet.c of dependencies on dsiface
10/07/02    atp    Fixed compilation bugs in REL_A.
10/03/02    atp    Renamed file to dsiface.c, removed get_ip_addr to avoid
                   clash with ds_shim.c
09/04/02    aku    Added support for dsiface_get_status(), dsiface_set_opt(),
                   dsiface_get_opt() and dsiface_ioctl().
09/02/02    mvl    created module with subset of dsiface.c
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgr.h"
#include "ps_iface.h"
#include "assert.h"
#include "ps_iface.h"
#include "ps_ppp_defs.h"
#include "dsiface.h"
#include "dsiface_def.h"
#include "dsmip_regsm.h"
#include "ds707.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsat707extctab.h"
#include "dsat707vend.h"
#include "dsat707vendctab.h"
#else
#include "dsatprofile.h"
#endif
#include "dssocket.h"


/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
boolean dsifacei_notify_iface_down = FALSE;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                      Interface Options Definitions
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
/*---------------------------------------------------------------------------
  Bit-field to indicate if the specified option is defined on a particular
  interface. Used in the interface options table.  New bit fileds may be
  added for future interface supports.
  {1, 1} means the option is defined on both Um and Rm ifaces
  {1, 0} indicates an Um only option
  {0, 1} indicates an Rm only option
  {0, 0} means that the option is not currently supported
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned int iface_um : 1;
  unsigned int iface_rm : 1;
}dsifacei_flag;

/*---------------------------------------------------------------------------
  Encapsulation of socket option.
---------------------------------------------------------------------------*/
typedef struct
{
  int optlen;                             /* length of the option value    */
  dsifacei_flag iface_support;            /* support for iface             */
}dsifacei_opt_ref_type;

/*---------------------------------------------------------------------------
                 DATA SERVICE INTERFACE OPTIONS TABLE

  Interface options reference table. This table is referenced when an 
  application is attempting to change the value of any data service
  interface options. NOTE: the table is indexed by the definition of 
  dsiface_opt_names_type. So any additions to the table must reflect the
  same ordering of dsiface_opt_names_type. 

         size    +   supported iface
                 |    {um, rm}       
     ------------|-------------------
---------------------------------------------------------------------------*/
const dsifacei_opt_ref_type dsifacei_opt_ref[DSIFACE_OPT_MAX] = 
{
  sizeof(int), {1,0},                  /* DSIFACE_OPT_MDR                  */
  sizeof(int), {1,0},                  /* DSIFACE_OPT_DORM_TIMER           */
  sizeof(dsiface_rlp_opt_type), {1,0}, /* DSIFACE_OPT_RLP_ALL_CUR_NAK      */
  sizeof(dsiface_rlp_opt_type), {1,0}, /* DSIFACE_OPT_RLP_ALL_DEF_NAK      */
  sizeof(dsiface_rlp_opt_type), {1,0}, /* DSIFACE_OPT_RLP_ALL_NEG_NAK      */
  sizeof(int), {0,0},                  /* DSIFACE_OPT_CTLHLD               */
#ifdef FEATURE_IS2000_REL_A
  sizeof(int), {1,0}                   /* DSIFACE_OPT_RLP_QOS_PRI          */
#endif /* FEATURE_IS2000_REL_A */
};
/*---------------------------------------------------------------------------
  Encapsulation of ioctl option. 
---------------------------------------------------------------------------*/
typedef struct
{
  int arglen;
  dsifacei_flag iface_support;
}dsifacei_ioctl_ref_type;

/*---------------------------------------------------------------------------
                 DATA SERVICE INTERFACE IOCTL OPTIONS TABLE

  Interface options reference table. This table is referenced when an 
  application is attempting to change the value of any data service
  interface options. NOTE: the table is indexed by the definition of 
  dsiface_ioctl_type. 

         size    +   supported iface
                 |    {um, rm}       
     ------------|-------------------
---------------------------------------------------------------------------*/
const dsifacei_ioctl_ref_type dsifacei_ioctl_ref[DSIFACE_IOCTL_MAX] =
{
  sizeof(sint15), {1,0},  /* DSIFACE_IOCTL_FORCEDORM */
  sizeof(int), {0,0}             /* DSIFACE_IOCTL_TRYCTLHLD */
};




/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/
#ifdef FEATURE_DS_SOCKETS
/*===========================================================================
FUNCTION DSIFACE_GET_STATUS

DESCRIPTION

  THIS FUNCTION IS DEPRECATED.  THE EXISTING FUNCTIONALITY FOR THE 1X UM /
  1XEVDO SN INTERFACE ONLY IS MAINTAINED FOR BACKWARDS COMPATIBILITY.  

  APPLICATIONS SHOULD USE DSS_IFACE_IOCTL() CALLS IN PLACE OF THIS FUNCTION.

  This function retrieves interface specific information and stores it in 
  the given interface control block.
 
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
  iface_stack_enum_type iface,
  dsiface_status_type *iface_status
)
{
  dss_iface_ioctl_id_type     ioctl_id;
  dss_iface_ioctl_state_type  iface_state;
  sint15                      ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a iface_status variable was specified -
    Prevents dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (iface_status == NULL)
  {
    MSG_ERROR("NULL iface ptr",0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Assert that the iface specifed is the Um iface.
  -------------------------------------------------------------------------*/
  if (iface != IFACE_UM_PPP_STACK)
  {
    MSG_ERROR("Invalid iface %d",iface,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Check if using MIP.  

    Note this will only be valid if the interface is currently OPEN.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_MOBILE_IP
  iface_status->iface_has_mip = mip_reg_sm_session_active(NULL);
#else
  iface_status->iface_has_mip = FALSE;
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Check 1x Um interface state.  

    Note that this is hard coded to CDMA_SN for backwards compatibility.
    This function is deprecated and will never support other modes -
    Use dss_iface_ioctl()'s instead.
  -------------------------------------------------------------------------*/
  ioctl_id.name     = DSS_IFACE_IOCTL_CDMA_SN;
  ioctl_id.instance = 0;

  if( dss_iface_ioctl( &ioctl_id,
                       DSS_IFACE_IOCTL_GET_STATE,
                       &iface_state,
                       &ps_errno ) < 0 )
  {
    ERR( "Unable to get CDMA_SN iface state", 0,0,0 );
    iface_state = IFACE_DISABLED;
  }

  switch (iface_state)
  {
    case IFACE_DISABLED:
    case IFACE_DOWN:
      iface_status->state = IFACE_CLOSED;
      break;
    case IFACE_COMING_UP:
      iface_status->state = IFACE_OPENING;
      break;
    case IFACE_UP:
    case IFACE_ROUTEABLE:
      iface_status->state = IFACE_OPEN;
      break;
    case IFACE_GOING_DOWN:
      iface_status->state = IFACE_CLOSING;
      break;
    default:
      ASSERT (0);
      break;
  };

  /*-------------------------------------------------------------------------
    Check Um dormant state
  -------------------------------------------------------------------------*/
  if( ds707_pkt_is_dormant(ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE)) )
  {
    iface_status->call_state = IFACE_DORMANT;
    MSG_MED("Call is dormant",0,0,0);
  }
  else
  {
    iface_status->call_state = IFACE_NOT_DORMANT;
    MSG_MED("Call is NOT dormant",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Close reason is not used (always report 0, was INVALID_PPP_CLOSE)
  -------------------------------------------------------------------------*/
  iface_status->close_reason = INVALID_PPP_CLOSE;

} /* dsiface_get_status() */
#endif /* FEATURE_DS_SOCKETS */

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
)
{
  dsiface_rlp_opt_type* rlp_opt_ptr;
  dsiface_error_type    err_type = DSIFACE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Generic checking. 
  -------------------------------------------------------------------------*/  
  if ((optname <= DSIFACE_OPT_MIN) || (optname >= DSIFACE_OPT_MAX))
  {
     MSG_HIGH ("Unsupported interface option.", 0,0,0);
     return DSIFACE_BAD_OPT_NAME;
  }
  /*-------------------------------------------------------------------------
     Check iface/option correspondence. Not all options are supported at
     a given level
  -------------------------------------------------------------------------*/
  switch (iface)
  {
    case IFACE_UM_PPP_STACK:
      if (dsifacei_opt_ref[optname].iface_support.iface_um != 1)
      {
        MSG_HIGH("Not supported at this level!", 0,0,0);
        err_type = DSIFACE_BAD_IFACE;
      }
      break;
      
#ifdef FEATURE_DS_NET_MODEL
    case IFACE_RM_PPP_STACK: 
      if (dsifacei_opt_ref[optname].iface_support.iface_rm != 1)
      {
        MSG_HIGH("Not supported at this level!", 0,0,0);
        err_type = DSIFACE_BAD_IFACE;
      }
      break;
#endif /* FEATURE_DS_NET_MODEL */
    default:
      MSG_HIGH("INVALID iface!",0,0,0);
      err_type = DSIFACE_BAD_IFACE;
      break;
  }/*switch()*/

  if( err_type != DSIFACE_SUCCESS )
  {
    return err_type;
  }

  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (optval == NULL) 
  {
    MSG_HIGH ("Null option value.",0,0,0);
    return DSIFACE_BAD_PARAM; 
  }

  /*-------------------------------------------------------------------------
     Check option length
  -------------------------------------------------------------------------*/
  if (optlen != dsifacei_opt_ref[optname].optlen)
  {
    MSG_HIGH("Invlid option length.", 0,0,0);
    return DSIFACE_BAD_LEN;
  } 
  /*-------------------------------------------------------------------------
    Setting the interface option with the new option value.  
  -------------------------------------------------------------------------*/
  switch (optname)
  {
    /*-----------------------------------------------------------------------
       QCMDR value
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_MDR:
      if ( (*(int*)optval < DS_MDR_MODE_MDR_ONLY ) || 
           (*(int*)optval > (DS_MDR_MODE_MAX -1)) ) 
      {
        MSG_HIGH("Invalid MDR value!", 0,0,0);
        err_type = DSIFACE_BAD_ARG_VAL;
      } 
      else
      {
        if (dsat707_set_mdr_val(*(unsigned int*)optval)==TRUE)
        {
          err_type = DSIFACE_SUCCESS;
        }
        else
        {
          err_type = DSIFACE_BAD_ARG_VAL; 
        }
      }
      break;
    /*-----------------------------------------------------------------------
       DORM_TIMER value
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_DORM_TIMER:
      if ( (*(int *)optval < 0 ) || (*(int *)optval > 255))
      {
        MSG_HIGH("Invalid dorm timer val  value!",0,0,0);
        err_type = DSIFACE_BAD_ARG_VAL;
      }
      else
      {
        dsat707_cta_val = (*(int *)optval);
        err_type = DSIFACE_SUCCESS;
      }
      break;
    /*-----------------------------------------------------------------------
       SET_ALL_CURRENT_NAK: This option sets the current NAK RLP policy that
       the MS will use. This option assumes that MS is in a RLP3 based Data
       call. 
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_RLP_ALL_CUR_NAK:
      rlp_opt_ptr = (dsiface_rlp_opt_type *)optval;

      if (dsrlp_set_all_cur_naks( (rlp_opt_ptr->rscb_index),
                                  (byte *)&(rlp_opt_ptr->nak_rounds_fwd),
                                  (byte *)&(rlp_opt_ptr->naks_per_round_fwd),
                                  (byte *)&(rlp_opt_ptr->nak_rounds_rev),
                                  (byte *)&(rlp_opt_ptr->naks_per_round_rev)
                                ) == TRUE)
      {
        err_type = DSIFACE_SUCCESS;
      }
      else
      {
        err_type = DSIFACE_BAD_ARG_VAL;
      }
      break;

    /*-----------------------------------------------------------------------
      SET_ALL_DEFAULT_NAK: This option sets the default NAK RLP policy that
      the MS will use. These parameters will take effect only after the 
      Service negotiation.
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_RLP_ALL_DEF_NAK:
      rlp_opt_ptr = (dsiface_rlp_opt_type *)optval;
      if ( dsrlp_set_all_def_naks((byte *)&(rlp_opt_ptr->nak_rounds_fwd),
                                  (byte *)&(rlp_opt_ptr->naks_per_round_fwd),
                                  (byte *)&(rlp_opt_ptr->nak_rounds_rev),
                                  (byte *)&(rlp_opt_ptr->naks_per_round_rev)
                                 ) == TRUE)
      {
        err_type = DSIFACE_SUCCESS;
      }
      else
      {
        err_type = DSIFACE_BAD_ARG_VAL;
      }
      break;

#ifdef FEATURE_IS2000_REL_A
    /*-----------------------------------------------------------------------
      DSIFACE_OPT_RLP_QOS_PRI: This option sets the desired QOS non-assured
      priority adjustment.
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_RLP_QOS_PRI:
      if ( (*(int*)optval < DS707_QOS_DESIRED_PRI_MIN ) ||
           (*(int*)optval > DS707_QOS_DESIRED_PRI_MAX ) )
      {
        MSG_HIGH("Invalid QOS_NON_ASSUR_PRI value!", 0,0,0);
        err_type = DSIFACE_BAD_ARG_VAL;
      }
      else
      {
        dsat707_qcqospri_val = *(unsigned int*)optval;
        dsrlp_set_desired_qos_non_assur_pri
              ( (byte) *(unsigned int*)optval );
        err_type = DSIFACE_SUCCESS;
      }
      break;
#endif /* FEATURE_IS2000_REL_A */

      /*-----------------------------------------------------------------------
         Add support for new options here
      -----------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------
      SET_ALL_NEGOTIATED_NAK: This option cannot be used for setting. So return
      bad NAK parameter.
    -------------------------------------------------------------------------*/
    case DSIFACE_OPT_RLP_ALL_NEG_NAK:
    default:
      /* shouldn't get here*/
      err_type = DSIFACE_BAD_PARAM;
      break;
  }/* switch */

  return err_type;
} /*dsiface_set_opt()*/


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
)
{
  dsiface_rlp_opt_type* rlp_opt_ptr;
  dsiface_error_type    err_type = DSIFACE_SUCCESS;
#ifdef FEATURE_IS2000_REL_A
  byte dsrlp_qos;
#endif /* FEATURE_IS2000_REL_A */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Generic checking.
  -------------------------------------------------------------------------*/
  if ((optname <= DSIFACE_OPT_MIN) || (optname >= DSIFACE_OPT_MAX))
  {
     MSG_HIGH ("Unsupported interface option.", 0,0,0);
     return DSIFACE_BAD_OPT_NAME;
  }
  
  /*-------------------------------------------------------------------------
     Check iface/option correspondence. Not all options are supported at
     a given level
  -------------------------------------------------------------------------*/
  switch (iface)
  {
    case IFACE_UM_PPP_STACK:
      if (dsifacei_opt_ref[optname].iface_support.iface_um != 1)
      {
        MSG_HIGH("Not supported at this level!", 0,0,0);
        err_type = DSIFACE_BAD_IFACE;
      }
      break;

#ifdef FEATURE_DS_NET_MODEL
    case IFACE_RM_PPP_STACK:
      if (dsifacei_opt_ref[optname].iface_support.iface_rm != 1)
      {
        MSG_HIGH("Not supported at this level!", 0,0,0);
        err_type = DSIFACE_BAD_IFACE;
      }
      break;
#endif /* FEATURE_DS_NET_MODEL */
    default:
      MSG_HIGH("INVALID iface!",0,0,0);
      err_type = DSIFACE_BAD_IFACE;
      break;
  }/*switch()*/

  if( err_type != DSIFACE_SUCCESS )
  {
    return err_type;
  }
 
  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if ((optval == NULL) || (optlen == NULL) )
  {
    return DSIFACE_BAD_PARAM;
  }

  /*-------------------------------------------------------------------------
      Getting the interface option with the new option value.
  -------------------------------------------------------------------------*/
  switch (optname)
  {
    /*-----------------------------------------------------------------------
       QCMDR value
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_MDR:
      if ( *optlen != sizeof(int))
      {
        MSG_HIGH("Buffer has invalid length!", 0,0,0);
        err_type = DSIFACE_BAD_LEN;
      }
      else
      {
        (*(int *)optval) = dsat707_qcmdr_val;
        err_type = DSIFACE_SUCCESS;
      }
    break;
    /*-----------------------------------------------------------------------
       DORM TIMER value
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_DORM_TIMER:
      if ( *optlen != sizeof(int))
      {
        MSG_HIGH("Buffer has invalid length!", 0,0,0);
        err_type = DSIFACE_BAD_LEN;
      }
      else
      {
        (*(int *)optval) = dsat707_cta_val;
        err_type = DSIFACE_SUCCESS;
      }
    break;
    /*-----------------------------------------------------------------------
       Add support for new options here
     
       Name of the option:  GET_ALL_CURRENT_NAK
       Name of the Option:  GET_ALL_DEFAULT_NAK
    -----------------------------------------------------------------------*/
  case DSIFACE_OPT_RLP_ALL_CUR_NAK:
      rlp_opt_ptr = (dsiface_rlp_opt_type *)optval;
      if (dsrlp_get_all_cur_naks(rlp_opt_ptr->rscb_index,
                                 (byte *)&(rlp_opt_ptr->nak_rounds_fwd),
                                 (byte *)&(rlp_opt_ptr->naks_per_round_fwd),
                                 (byte *)&(rlp_opt_ptr->nak_rounds_rev),
                                 (byte *)&(rlp_opt_ptr->naks_per_round_rev)
                                 ) == TRUE)
      {
        err_type = DSIFACE_SUCCESS;
      }
      else
      {
        err_type = DSIFACE_BAD_ARG_VAL;
      }
      break;

  case DSIFACE_OPT_RLP_ALL_DEF_NAK:
       rlp_opt_ptr = (dsiface_rlp_opt_type *)optval;
       if ( dsrlp_get_all_def_naks((byte *)&(rlp_opt_ptr->nak_rounds_fwd),
                                   (byte *)&(rlp_opt_ptr->naks_per_round_fwd),
                                   (byte *)&(rlp_opt_ptr->nak_rounds_rev),
                                   (byte *)&(rlp_opt_ptr->naks_per_round_rev)
                                   ) == TRUE)
       {
         err_type = DSIFACE_SUCCESS;
       }
       else
       {
         err_type = DSIFACE_BAD_ARG_VAL;
       }
       break;

  case DSIFACE_OPT_RLP_ALL_NEG_NAK:
      rlp_opt_ptr = (dsiface_rlp_opt_type *)optval;
       if ( dsrlp_get_all_neg_naks((byte *)&(rlp_opt_ptr->nak_rounds_fwd),
                                   (byte *)&(rlp_opt_ptr->naks_per_round_fwd),
                                   (byte *)&(rlp_opt_ptr->nak_rounds_rev),
                                   (byte *)&(rlp_opt_ptr->naks_per_round_rev)
                                   ) == TRUE)
     {
       err_type = DSIFACE_SUCCESS;
     }
     else
     {
       err_type = DSIFACE_BAD_ARG_VAL;
     }
     break;

#ifdef FEATURE_IS2000_REL_A
    /*-----------------------------------------------------------------------
      DSIFACE_OPT_RLP_QOS_PRI: This option gets the desired QOS non-assured
      priority adjustment.
    -----------------------------------------------------------------------*/
    case DSIFACE_OPT_RLP_QOS_PRI:
      /*---------------------------------------------------------------------
        If desired values have been set in QOS, the API should return the
        current desired value. But if it hasn't been set at all, the API
        returns the default desired value, which in this case is the same
        as standard default value.
      ---------------------------------------------------------------------*/
      if ((dsrlp_get_desired_qos_non_assur_pri(&dsrlp_qos)) == TRUE)
      {
        (*(byte *)optval) = dsrlp_qos;
      }
      else
      {
        (*(byte *)optval) = DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ;
      }
      err_type = DSIFACE_SUCCESS;
      break;
#endif /* FEATURE_IS2000_REL_A */

    /*-----------------------------------------------------------------------
       Add support for new options here
    -----------------------------------------------------------------------*/
    default:
      /* shouldn't get here*/
      err_type = DSIFACE_BAD_PARAM;
      break;
  }/* switch */  

  return err_type;
} /* dsiface_get_opt() */


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
)
{
  dsiface_error_type err_type = DSIFACE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
     Generic checking.
  -------------------------------------------------------------------------*/
  if ((ioctlname <= DSIFACE_IOCTL_MIN) || (ioctlname >= DSIFACE_IOCTL_MAX))
  {
     MSG_HIGH ("Unsupported interface IO ctl.", 0,0,0);
     return DSIFACE_BAD_OPT_NAME;
  }

  /*-------------------------------------------------------------------------
     Check iface/option correspondence. Not all options are supported at
     a given level
  -------------------------------------------------------------------------*/
  switch (iface)
  {
    case IFACE_UM_PPP_STACK:
      if (dsifacei_ioctl_ref[ioctlname].iface_support.iface_um != 1)
      {
        MSG_HIGH("Not supported at this level!", 0,0,0);
        err_type = DSIFACE_BAD_IFACE;
      }
      break;

#ifdef FEATURE_DS_NET_MODEL
    case IFACE_RM_PPP_STACK:
      if (dsifacei_ioctl_ref[ioctlname].iface_support.iface_rm != 1)
      {
        MSG_HIGH("Not supported at this level!", 0,0,0);
        err_type = DSIFACE_BAD_IFACE;
      }
      break;
#endif /* FEATURE_DS_NET_MODEL */
    default:
      MSG_HIGH("INVALID iface!",0,0,0);
      err_type = DSIFACE_BAD_IFACE;
      break;
  }/*switch()*/

  if( err_type != DSIFACE_SUCCESS )
  {
    return err_type;
  }

  /*-------------------------------------------------------------------------
    Assert all arguments are valid - Prevents dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if ((argval == NULL))
  {
    return DSIFACE_BAD_PARAM;
  }

  /*-------------------------------------------------------------------------
    Based on the interface io control option name, perform the appropriate
    actions. 
  -------------------------------------------------------------------------*/
  switch (ioctlname)
  {
    /*-----------------------------------------------------------------------
       Force dormancy.
    -----------------------------------------------------------------------*/
    case DSIFACE_IOCTL_FORCEDORM:
      if ( arglen != dsifacei_ioctl_ref[ioctlname].arglen)
      {
        MSG_HIGH("Argument has invalid length!", 0,0,0);
        err_type = DSIFACE_BAD_LEN;
      }
      else
      {
        dss_force_dormancy (* (sint15 *)argval);
        err_type = DSIFACE_SUCCESS;
      }
      break;
    /*-----------------------------------------------------------------------
       Try control hold. Does nothing currently. 
    -----------------------------------------------------------------------*/
    case DSIFACE_IOCTL_TRYCTLHLD:
      break;
    /*-----------------------------------------------------------------------
       Add support for new interface control options here
    -----------------------------------------------------------------------*/
    default:
      /* shouldn't get here*/
      err_type = DSIFACE_BAD_PARAM;
      break;
  }/* switch */

  return err_type;
} /* dsiface_ioctl() */

/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSIFACE_GET_DNS_ADDR()

DESCRIPTION
  This function returns the DNS sever IP address - either the primary or
  secondsry depending on the enum pasesed in.

PARAMETERS
  dummy: a dummy parameter for backwards compatibility
  addr_type: the type of IP address to fetch (primary, secondary) 

RETURN VALUE
  The DNS servers IP address, in HOST order.

DEPENDENCIES
  Will only work for the first instance (instace 0) of the ds707 pkt_mgr.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsiface_get_dns_addr
(
  iface_stack_enum_type iface,  /* which interface?                        */
  iface_addr_type addr_type     /* Which IP address to set                 */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     NOTE: The parameter iface is being ignored presently. Its presence 
     enables the ease of future expansions of this function.
  -------------------------------------------------------------------------*/
  switch(addr_type) 
  {
    case IFACE_IPv4_DNS_PRIMARY:
      return ds707_pkt_get_iface_ptr(0)->v4_net_info.primary_dns;

    case IFACE_IPv4_DNS_SECONDARY:
      return ds707_pkt_get_iface_ptr(0)->v4_net_info.secondary_dns;

    default:
      ASSERT(0); /* Application should never get here */
      return 0;
  }
} /* dsiface_get_dns_addr() */

#endif /* FEATURE_DATA_IS707 */
