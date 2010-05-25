#ifndef PS_IFACE_IOCTL_H
#define PS_IFACE_IOCTL_H

/*===========================================================================

                          P S _ I F A C E _I O C T L. H

DESCRIPTION

  The Protocol Services interface file. This contains variables and 
  definitions used by the various modules to access interface definitions.

  Copyright (c)2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ioctl.h_v   1.1   12 Feb 2003 12:08:04   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface_ioctl.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h
11/10/02    aku     created file
===========================================================================*/

#include "comdef.h"
#ifdef FEATURE_DATA
#include "dss_iface_ioctl.h"
#include "ps_iface.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/                                                    
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

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
);
#endif /* FEATURE_DATA */
#endif /* PS_IFACE_IOCTL_H */

