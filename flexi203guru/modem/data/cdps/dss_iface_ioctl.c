/*===========================================================================

                      D S S _ I F A C E _ I O C T L. C
                   
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

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_iface_ioctl.c_v   1.6   12 Feb 2003 17:54:54   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dss_iface_ioctl.c#2 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/24/08    pp     Fixed RVCT compiler warnings.
03/08/04    aku    Fixed potential access to NULL dss_errno pointer in call
                   to dss_iface_ioctl()
05/05/03    aku    Added support for accepting Null id_ptr in case of
                   DSS_IFACE_IOCTL_GET_ALL_IFACES.
04/30/03    aku    Moved dss_get_iface_id() from dssocket.c and code review
                   changes.
03/20/03    aku    Remove check for Null argval pointer in dss_iface_ioctl(). 
02/12/03    aku    Added checks for Null arguments in call to 
                   dss_iface_ioctl() to return appropriate error codes.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
02/10/03    aku    Added check for NULL dss_errno.  
01/17/03    usb    Removed support for specifying all interfaces to an ioctl
12/24/02    aku    Added support for specifying interface using name and 
                   instance.
11/23/02    aku    Removed support for sockfd as identifier.
11/10/02    aku    Initial version.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#include "dss_iface_ioctl.h"
#include "ps_iface_ioctl.h"
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

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()

DESCRIPTION
  This function dstermines the ps_iface_ptr associated with the passed in
  identifier. It then calls ps_iface_ioctl().

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
)
{
  ps_iface_type  *iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((id_ptr == NULL) && 
     (ioctl_name != DSS_IFACE_IOCTL_GET_ALL_IFACES))
  {
    MSG_ERROR("Null id_ptr", 0,0,0);
    *dss_errno = DS_EBADF;
    return -1;
  }
  if(dss_errno == NULL)
  {
    MSG_ERROR("Null dss_errno", 0,0,0);
    return -1;
  }

  MSG_MED("dss_iface_ioctl() called with %d", ioctl_name,0,0);

  /*-------------------------------------------------------------------------
    Determine the specific ps_iface_ptr based on the interface name and 
    instance passed in. 
    Note that we lookup the ps_iface_ptr only if the id_ptr is non-NULL. In 
    case of DSS_IFACE_IOCTL_GET_ALL_IFACES, a NULL id_ptr is a valid 
    parameter.
  -------------------------------------------------------------------------*/
  if(id_ptr != NULL)
  {
    iface_ptr = ps_iface_get_handle(id_ptr->name, id_ptr->instance);
    if(iface_ptr == NULL)
    {
      MSG_ERROR("Invalid name=0x%x and instance=0x%x", 
                 id_ptr->name, id_ptr->instance,0);
      *dss_errno = DS_EBADF;
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Call ps_iface_ioctl() with the ps_iface_ptr.
  -------------------------------------------------------------------------*/
  return ps_iface_ioctl(iface_ptr,
                        ioctl_name,
                        argval_ptr,
                        dss_errno);
  
} /* dss_iface_ioctl() */


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
)
{
  acb_type *acb_ptr=NULL;
  scb_type *scb_ptr=NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (iface_id == NULL)
  {
    ERR("NULL param iface_id", 0, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Switch on the id type and determine the associated interface. If the 
    appid is specified, the associated interface is stored in acb and if
    the sockfd is specified the associated interface is stored as the routing
    cache in the scb.
  -------------------------------------------------------------------------*/
  switch(id.type)
  {
    case DSS_APP_ID:
      acb_ptr = dssocki_appidtoacb(id.info.app_id);
      if(acb_ptr == NULL)
      {
        ERR("Null acb_ptr, appid=%d", id.info.app_id, 0, 0);
        return -1;
      }
      if(PS_IFACE_IS_VALID(acb_ptr->ps_iface_ptr) == FALSE)
      {
        ERR("Invalid ps_iface_ptr, appid=%d", id.info.app_id, 0, 0);
        return -1;
      }
      iface_id->name = (dss_iface_ioctl_name_enum_type)acb_ptr->ps_iface_ptr->name;
      iface_id->instance = acb_ptr->ps_iface_ptr->instance;
      break;

    case DSS_SOCK_FD:
      scb_ptr = dssocki_sockfdtoscb(id.info.sockfd);
      if(scb_ptr == NULL)
      {
        ERR("Null scb_ptr, sockfd=%d ", id.info.sockfd, 0, 0);
        return -1;
      }
      if(PS_IFACE_IS_VALID(scb_ptr->routing_cache) == FALSE)
      {
        ERR("Invalid routing_cache, sockfd=%d ", id.info.sockfd, 0, 0);
        return -1;
      }
      iface_id->name = (dss_iface_ioctl_name_enum_type)scb_ptr->routing_cache->name;
      iface_id->instance = scb_ptr->routing_cache->instance;
      break;

    default:
      ERR("Unknown id type %d", id.type, 0, 0);
      return -1;
  }

  return 0;

} /* dss_get_iface_name_and_instance() */
#endif /* FEATURE_DATA */
