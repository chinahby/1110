#ifndef DSSINTERNAL_H
#define DSSINTERNAL_H

/*===========================================================================

         I N T E R N A L  D A T A  S E R V I C E  S O C K E T S  
                       H E A D E R  F I L E
                   
DESCRIPTION

 The Internal Data Services Sockets Header File. Contains shared variables 
 and enums, as well as declarations for functions for each protocol.


Copyright (c) 2001 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssinternal.h_v   1.3   12 Feb 2003 19:34:06   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssinternal.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/05/03    jd     Removed dependency on dsiface.h
04/25/03    aku    Added dss_set_specified_iface() and removed unused 
                   dss_is_mip_up_on_um() and dssi_force_dorm() prototypes.
02/10/03    aku    Added support for flow control of non system sockets on a 
                   per interface basis.
12/13/02    ifk    Renamed function parameter from errno to dss_errno.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/27/02    ss     Moved socket options related declarations to dssocket.h
08/16/02    ss     Added DSS_SO_SILENT_CLOSE socket option.
07/24/02    aku    Featurized dss_ppp_in_use() with FEATURE_DATA_MM.
01/02/02    aku    dss_ppp_in_use() now returns uint instead of a boolean.
11/26/01    js     Added prototype for dssi_force_dorm().
11/15/01    aku    Added prototype for dss_is_ppp_in_use().
07/28/01    na     Added prototype for dss_is_mip_up_on_um().
06/26/01    na     Added the prototype for dssocki_set_non_sys_socket_flow()
06/26/01    sjy    Added system socket option DSS_SO_SYS_SOCK, and deleted
                   DSS_SO_ZERO_IP_OK option.
04/06/01    js     Added definitinon for socket option DSS_SO_ZERO_IP_OK.
04/06/01    js     Added support for socket level socket options. 
03/28/01    js     Created module. 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "dss_iface_ioctl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSS_WRITE_DSM_CHAIN

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

  On error, return DSS_ERROR and places the error condition value in 
  *dss_errno.

  Errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.

===========================================================================*/
extern sint15 dss_write_dsm_chain
(
  sint15          sockfd,            /* socket descriptor                  */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15         *dss_errno          /* error condition value              */
);


/*===========================================================================

FUNCTION DSS_READ_DSM_CHAIN

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File 
      condition has occurred.

  On error, return DSS_ERROR and places the error condition value in 
  *dss_errno.

  Errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           application buffer no valid part of address space
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_read_dsm_chain
(
  sint15           sockfd,            /* socket descriptor                 */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15          *dss_errno          /* error condition value             */
);

/*===========================================================================
FUNCTION DSSOCKI_SET_NON_SYS_SOCKET_FLOW()

DESCRIPTION
  This function is used to enable and disable flow control for non system
  sockets on a specific interface.

PARAMETERS
  mask:    the flow control mask (defined in dsi.h)
  enabled: whether flow should be enabled
  iface_id_ptr: Interface on which to operate.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_set_non_sys_socket_flow
(
  uint32 mask,
  boolean enabled,
  dss_iface_ioctl_id_type *iface_id_ptr
);



/*===========================================================================
FUNCTION DSS_SET_SPECIFIED_IFACE()

DESCRIPTION
  This function is used to set the app specified interface for an 
  application. For instance, MIP and GPS which uses only the 1X interface,
  use this API to set the specified interface in the acb.

DEPENDENCIES
  None.

PARAMETERS
  sint15 appid - Application ID.
  dss_iface_ioctl_id_type *iface_id - Parameter which contains the 
  name and instance of the iface that application would like to use.
  sint15 *ds_errno - DS_EBADAPP if invalid appid is specified.
                     DS_EBADF if invalid interface is specified.
 
RETURN VALUE
  0 on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
int dss_set_specified_iface
(
  sint15                  appid,
  dss_iface_ioctl_id_type *iface_id,
  sint15                  *ds_errno
);

#endif
