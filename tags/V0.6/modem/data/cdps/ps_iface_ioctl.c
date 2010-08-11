/*===========================================================================

                      P S _ I F A C E _ I O C T L. C
                   
DESCRIPTION
  This file contains functions used by various modules to access
  network interface. 

EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ioctl.c_v   1.8   13 Feb 2003 14:15:20   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface_ioctl.c#2 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/17/08    pp     Fixed RVCT compiler warnings.
05/05/03    aku    Aded support for DSS_IFACE_IOCTL_GET_ALL_IFACES
03/31/03    aku    Added NULL checks for argval_ptr before deferencing it.
03/19/03    aku    Set the IP address type to IPV4 for secondary DNS address.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
02/07/03    usb    Added ioctls for reg and dereg inbound IP filters.
01/17/03    atp    Added argument ps_iface_ptr to iface_ioctl_f_ptr.
01/17/03    usb    Added validation of iface_ptr in ps_iface_ioctl(), 
                   included file dssocki.h to remove compiler warnings
12/24/02    aku    Added support for reg/dereg event callbacks
11/24/02    aku    Removed interface statistics IOCTL.
11/19/02    aku    Added interface statisitics IOCTL
11/11/02    aku    Initial version.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#include "ps_iface_ioctl.h"
#include "ps_ip_addr.h"
#include "msg.h"
#include "assert.h"
#include "dssocket.h"
#include "dssocki.h"

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_IFACE_IOCTL()

DESCRIPTION
  This function performs various operations on the given ps interface. 
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  ps_iface_type             - ps_iface on which the specified operations 
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
                              specified arguments for the IOCTL are correct 
                              but dss_iface_ioctl() or a mode handler 
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
int ps_iface_ioctl
( 
  ps_iface_type            *ps_iface_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ip_addr_type *ip_addr;
  dss_iface_ioctl_mtu_type *mtu;
  dss_iface_ioctl_state_type *iface_state;
  dss_iface_ioctl_phys_link_state_type *phys_state;
  dss_iface_ioctl_ev_cb_type  *ev_cb_info;
  dss_iface_ioctl_all_ifaces_type  *iface_info;
  int iface_index = 0;

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *ps_errno = 0;

  /*-------------------------------------------------------------------------
    Note that in case of DSS_IFACE_IOCTL_GET_ALL_IFACES, a NULL ps_iface_ptr
    is a valid parameter.  
  -------------------------------------------------------------------------*/
  if((ioctl_name != DSS_IFACE_IOCTL_GET_ALL_IFACES) &&
     (PS_IFACE_IS_VALID(ps_iface_ptr) == FALSE))
  {
    MSG_ERROR("IOCTL failed, invalid iface 0x%x", ps_iface_ptr, 0, 0);
    *ps_errno = DS_EBADF;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Based on the interface io control option name, perform the appropriate
    actions. 
  -------------------------------------------------------------------------*/
  switch (ioctl_name)
  {
    /*-----------------------------------------------------------------------
      Get IPV4 addr of iface
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GET_IPV4_ADDR:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->type = IPV4_ADDR;
      ps_iface_get_addr(ps_iface_ptr,ip_addr); 
      /*---------------------------------------------------------------------
        Convert IP address to network byte order.
      ---------------------------------------------------------------------*/
      ip_addr->addr.v4 = dss_htonl(ip_addr->addr.v4);
      MSG_MED("IP addr = %d.%d", 
               (unsigned char)ip_addr->addr.v4,
               (unsigned char)(ip_addr->addr.v4>>8),
               0);
      MSG_MED("IP addr = %d.%d", 
               (unsigned char)(ip_addr->addr.v4>>16),
               (unsigned char)(ip_addr->addr.v4>>24),
               0);
      break;

    /*-----------------------------------------------------------------------
      Get IPV4 Primary DNS addr of iface
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->addr.v4 = ps_iface_ptr->v4_net_info.primary_dns;
      if(ip_addr->addr.v4 == 0)
      {
        ip_addr->type = IP_ADDR_INVALID;
      }
      else
      {
        ip_addr->type = IPV4_ADDR;
        /*-------------------------------------------------------------------
          Convert Primary DNS address to network byte order.
        -------------------------------------------------------------------*/
        ip_addr->addr.v4 = dss_htonl(ip_addr->addr.v4);
      }
      MSG_MED("Primary DNS addr = %d.%d", 
               (unsigned char)ip_addr->addr.v4,
               (unsigned char)(ip_addr->addr.v4>>8),
               0);
      MSG_MED("Primary DNS addr = %d.%d", 
               (unsigned char)(ip_addr->addr.v4>>16),
               (unsigned char)(ip_addr->addr.v4>>24),
               0);
      break;

    /*-----------------------------------------------------------------------
      Get IPV4 Secondary DNS addr of iface
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      ip_addr = (ip_addr_type *)argval_ptr;
      ip_addr->addr.v4 = ps_iface_ptr->v4_net_info.secondary_dns;
      if(ip_addr->addr.v4 == 0)
      {
        ip_addr->type = IP_ADDR_INVALID;
      }
      else
      {
        ip_addr->type = IPV4_ADDR;
        /*-------------------------------------------------------------------
          Convert Secondary DNS address to network byte order.
        -------------------------------------------------------------------*/
        ip_addr->addr.v4 = dss_htonl(ip_addr->addr.v4);
      }
      MSG_MED("Secondary DNS addr = %d.%d", 
               (unsigned char)ip_addr->addr.v4,
               (unsigned char)(ip_addr->addr.v4>>8),
               0);
      MSG_MED("Secondary DNS addr = %d.%d", 
               (unsigned char)(ip_addr->addr.v4>>16),
               (unsigned char)(ip_addr->addr.v4>>24),
               0);
      break;

    /*-----------------------------------------------------------------------
      Get MTU of iface
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GET_MTU:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      mtu = (dss_iface_ioctl_mtu_type *)argval_ptr;
      *mtu = ps_iface_ptr->v4_net_info.mtu;      
      MSG_MED("MTU is %d",*mtu,0,0); 
      break;

    /*-----------------------------------------------------------------------
      Get state of iface
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GET_STATE:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      iface_state = (dss_iface_ioctl_state_type *)argval_ptr;
      *iface_state = ps_iface_state(ps_iface_ptr);
      MSG_MED("Iface state is %d",*iface_state,0,0); 
      break;

    /*-----------------------------------------------------------------------
      Get iface physical link state
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      phys_state = (dss_iface_ioctl_phys_link_state_type *)argval_ptr;
      *phys_state = ps_iface_phys_link_state(ps_iface_ptr);
      MSG_MED("Phys link state is %d",*phys_state,0,0); 
      break;

    /*-----------------------------------------------------------------------
      Register for iface events 
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_REG_EVENT_CB:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      ev_cb_info = (dss_iface_ioctl_ev_cb_type *)argval_ptr;
      if((dssocki_reg_ev(ev_cb_info->app_id,
                         (ps_iface_event_enum_type)ev_cb_info->event,
                         ps_iface_ptr,
                         ev_cb_info->event_cb,
                         ev_cb_info->user_data_ptr)) == FALSE)
      {
        *ps_errno =  DS_EFAULT;
        return -1;
      }
      break;

    /*-----------------------------------------------------------------------
      Deregister for iface events
    -----------------------------------------------------------------------*/
    case DSS_IFACE_IOCTL_DEREG_EVENT_CB:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      ev_cb_info = (dss_iface_ioctl_ev_cb_type *)argval_ptr;
      if((dssocki_dereg_ev(ev_cb_info->app_id,
                         (ps_iface_event_enum_type)ev_cb_info->event,
                         ps_iface_ptr)) == FALSE)
      {
        *ps_errno =  DS_EFAULT;
        return -1;
      }
      break;

    case DSS_IFACE_IOCTL_GET_ALL_IFACES:
      if(argval_ptr == NULL)
      {
        MSG_ERROR("Null argval_ptr", 0,0,0);
        *ps_errno = DS_EFAULT;
        return -1;
      }
      iface_info = (dss_iface_ioctl_all_ifaces_type *)argval_ptr;      
      /*---------------------------------------------------------------------
        Set the number of ifaces to zero before starting the search.  
      ---------------------------------------------------------------------*/
      iface_info->number_of_ifaces = 0;
      
      /*---------------------------------------------------------------------
        Search the global iface array 
      ---------------------------------------------------------------------*/
      for(iface_index=0; iface_index<MAX_SYSTEM_IFACES; iface_index++)
      {
        /*-------------------------------------------------------------------
          Since ifaces are added sequentially and they are never deleted, as 
          soon as we get a NULL ptr there is no more iface pointers in the 
          list.
        -------------------------------------------------------------------*/
        if(global_iface_ptr_array[iface_index] == NULL)
        {
          break;
        }

        iface_info->ifaces[iface_index].name = (dss_iface_ioctl_name_enum_type)
                                   global_iface_ptr_array[iface_index]->name;
        iface_info->ifaces[iface_index].instance = 
                               global_iface_ptr_array[iface_index]->instance;
        iface_info->number_of_ifaces++;
      }
      break;

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

    default:
      /*---------------------------------------------------------------------
        Interface specific operation. Call the registered callback.
      ---------------------------------------------------------------------*/
      if(ps_iface_ptr->iface_ioctl_f_ptr != NULL)
      {
        MSG_MED("Interface specific ioctl=%d, calling callback",ioctl_name,0,0);
        return ps_iface_ptr->iface_ioctl_f_ptr(
                                               ps_iface_ptr,
                                               ioctl_name,
                                               argval_ptr,
                                               ps_errno
                                              );
      }
      else
      {                   
        ERR("Unsupported IOCTL %d on iface 0x%x",ioctl_name,ps_iface_ptr,0);
        *ps_errno = DS_EINVAL;
        return -1;
      }

  }/* switch */

  return 0;
} /* ps_iface_ioctl() */

#endif /* FEATURE_DATA */
