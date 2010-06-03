#ifndef _DSMIP_IO_H
#define _DSMIP_IO_H

/*===========================================================================

 D A T A   S E R V I C E S   M O B I L E   I P   I / O   A P I   H E A D E R
                   
DESCRIPTION

 The Data Services Mobile IP I/O Header File.  Contains exported function
 prototypes, definitions and data.

Copyright (c) 2000,2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_io.h_v   1.11   29 Dec 2002 16:34:18   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/dsmip_io.h#1 $ $DateTime: 2008/04/11 07:14:56 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/02   jd      Fixed includes for multimode builds.
                   Changed mipio_handle_*_event prototypes to match multimode 
                   sig handler signature, ret val. ignored in old arch.
05/10/02   jd      includes dsmip.h instead of dsmip_regsm.h
                   (moved definition of mip_session_info_type)
01/02/02   aku     Added new macro MIPIO_OPEN_IFACE().
11/09/01   aku     Added enum type for mipio_pppopen_ret_val and 
                   mipio_ppp_open() now returns mipio_pppopen_ret_val instead 
                   of a boolean
08/14/01   aku     Added prototype for accessor function mipioi_get_app_id
07/25/01   jd      added mipio_handle_network_event()
07/20/01   sjy     added function declarations for mipio_ppp_open() and 
                   mipio_ppp_close().
07/12/01   jd      code cleanup (linting and code review)
07/06/01   jd      removed mipio_shutdown as we changed design to initialize
                   on startup and never call shutdown.
06/07/01   jd      replaced conn_cb_type with socket descriptor array
05/25/01   jd      Fixed mipio_handle_sockET_event prototype
09/25/00   jd      Created module.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "dsmip.h"
#ifdef FEATURE_DATA_MM
#include "ps_svc.h"
#endif /* FEATURE_DATA_MM */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Definitions of connection mapping table handles for ICMP receive and 
  transmit sockets.

  The connection mapping table maps mip i/o connections to socket 
  descriptors returned by dss_socket().  The logical connections are 
  numbered as follows:
 
  connection          purpose
  -----------         ----------
   0                  UDP socket for Mobile IP session #1
   1                  UDP socket for Mobile IP session #2
   ...                ...
   MIP_MAX_SESSIONS-1 UDP socket for Mobile IP session #(MIP_MAX_SESSIONS)
   MIP_MAX_SESSIONS   ICMP socket to rx ICMP msg type 9 (AAM)
   MIP_MAX_SESSIONS+1 ICMP socket to tx ICMP msg type 10 (ASM)
---------------------------------------------------------------------------*/
#define MIPIO_ICMP_RX_HANDLE  (MIP_MAX_SESSIONS)    /* ICMP rx socket  */
#define MIPIO_ICMP_TX_HANDLE  (MIP_MAX_SESSIONS+1)  /* ICMP tx socket  */

/*---------------------------------------------------------------------------
  Enum for mipio_pppopen return value.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_OPEN_FAIL = 0,
  PPP_OPEN_SUCCESS = 1,
  PPP_IN_SOCKET_CALL = 2
} mipio_pppopen_ret_val;


/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================

MACRO MIPIO_OPEN_IFACE()

DESCRIPTION
  Calls dss_pppopen(). But this effectively sets the app_iface_state to 
  IFACE_OPENING for MIP socket apps

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#define MIPIO_OPEN_IFACE()  \
        dss_pppopen(mipio_get_app_id(), &l_errno);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MIPIO_INIT

DESCRIPTION

  Opens and configures all sockets required for Mobile IP messaging.

  One UDP socket is required for each session, over which registration
  request messages (RRQ's) are sent and registration reply messages (RRP's)
  are received.
 
  Two ICMP sockets total are required for Mobile IP to handle mobility 
  agent (i.e., foreign or home agent) advertisements (AAM's) and 
  solicitations (ASM's).  

  The DMSS ICMP implementation avoids wasting memory on extraneous messages
  by assigning a type filter to each socket.  The socket will receive ICMP
  messages of that type only, and will assign that type to any messages sent
  on that socket.  Thus 2 sockets are required since ASM and AAM have unique
  types. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - initialized ok (or already initialized)
  FALSE - initialization failed

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mipio_init
(
  void
);



/*===========================================================================

FUNCTION MIPIO_WRITE

DESCRIPTION
  Writes data to a mipio connection.  

PARAMETERS
  idx       - connection # to write to (see table above for semantics)
  data      - pointer to data to be written
  data_len  - length of data pointed to by 'data'
  dest_ip   - destination IP address
  dest_port - destination port number (ignored for ICMP messages)

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mipio_write
(
  uint8    idx,                            /* session number to write to   */
  void *   data,                           /* data to be written           */
  uint16   data_len,                       /* length of above data         */
  uint32   dest_ip,                        /* destination IP address       */
  uint16   dest_port                       /* destination IP port          */
);


/*===========================================================================

FUNCTION MIPIO_HANDLE_SOCKET_EVENT

DESCRIPTION
  Called by PS to process socket event on a mobile IP socket

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mipio_handle_socket_event
(
#ifdef FEATURE_DATA_MM
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
#else
  void
#endif /* FEATURE_DATA_MM */
);


/*===========================================================================

FUNCTION MIPIO_HANDLE_NETWORK_EVENT

DESCRIPTION
  Called by PS to perform MIP related processing related to network event

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mipio_handle_network_event
(
#ifdef FEATURE_DATA_MM
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
#else
  void
#endif /* FEATURE_DATA_MM */
);


/*===========================================================================

FUNCTION MIPIO_PPP_OPEN

DESCRIPTION
  Starts up the network subsystem (CDMA data service and PPP) over the Um 
  interface for all sockets using the M.IP application ID.  

PARAMETERS
  None.

RETURN VALUE
  TRUE  - PPP brought up
  FALSE - unable to bring up PPP, PPP open failed

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
extern mipio_pppopen_ret_val mipio_ppp_open
(
  void  
);


/*===========================================================================

FUNCTION MIPIO_PPP_CLOSE

DESCRIPTION
  Initiates termination to bring down PPP and the traffic channel opened
  by M.IP.  Upon successful close of the network subsystem, invokes the 
  network callback function.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - PPP brought down
  FALSE - unable to bring down PPP, PPP close failed

DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mipio_ppp_close
(
  void  
);


/*===========================================================================

FUNCTION MIPIO_GET_APP_ID

DESCRIPTION
  Accessor function used to retrieve socket appId

PARAMETERS
  None.

RETURN VALUE
  app_id           application ID
  
DEPENDENCIES
  mipio_init() must have been called previously.

SIDE EFFECTS
  None.

===========================================================================*/
sint15 mipio_get_app_id
(
  void  
);

#endif  /* _DSMIP_IO_H */
