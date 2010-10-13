#ifndef DSSBSDH_H
#define DSSBSDH_H

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_DS_SOCKETS_BSD
/*===========================================================================

                   DATA SERVICES SOCKET BSD HELPER API FILE

DESCRIPTION

  API functions designed to limit the number of DS Sock calls necessary to
  implement the BSD socket interface.  These functions optimize system
  performance when the BSD and DS socket APIs run on different processors
  and communicate with each other via RPC.    

EXTERNALIZED FUNCTIONS

  dss_async_select_deselect()
    Specifies the events to be or not to be notified for on a single socket.

  dss_multiple_async_select_deselect()
    Specifies the events to be or not to be notified for on a set of sockets. 

  dss_get_socket_events()
    Get the events that have occurred on a set of sockets.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/20/01     mt    Create file.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dssocket.h"

/*===========================================================================
                          MACROS AND TYPE DEFINITIONS
===========================================================================*/
#define BITS_PER_MASK   8                /* dependent on fd_set array type */

/*---------------------------------------------------------------------------
  Macros used for accessing bits in a variable of type fd_set.
---------------------------------------------------------------------------*/
#define _BIT_MASK(bit)     ((uint8)1 << (bit % BITS_PER_MASK))
#define _ISSET(bit, fdset) \
  ((fdset != NULL) ? (fdset)->set[bit/BITS_PER_MASK] & _BIT_MASK(bit) : 0)
#define _RESET(bit, fdset) \
  ((fdset)->set[bit/BITS_PER_MASK] &= ~ (uint32) _BIT_MASK(bit))
#define _SET(bit, fdset) \
  ((fdset)->set[bit/BITS_PER_MASK] |= _BIT_MASK(bit))

/*---------------------------------------------------------------------------
  fd_set type is a socket descriptor bit array.  Each array element holds
  8 bits since it is a uint8.  Therefore, the required number of array
  entries is (FD_SET_SIZE + BITS_PER_MASK - 1) / BITS_PER_MASK, where
  FD_SET_SIZE is the maximum number of sockets,
  (BITS_PER_MASK - 1) is the addend for rounding up to the next array entry,
  BITS_PER_MASK is the number of bits per array element.
---------------------------------------------------------------------------*/
#define FD_SET_SIZE     DSS_MAX_SOCKS

typedef struct
{
  uint8 set[(FD_SET_SIZE + BITS_PER_MASK - 1) / BITS_PER_MASK];
} fd_set;

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSS_ASYNC_SELECT_DESELECT()

DESCRIPTION
  Enables and disables asynchronous event(s) notification on one socket
  descriptor.  Application specifies a bit mask of events of interest --
  events for which the application will receive an asynchronous notification
  via a callback function.  This function also performs a real-time event
  check and invokes the callback if any of the events have already occurred.
  Application also specifies a bit mask of events that it wishes to clear --
  events for which the application will no longer receive notification.

  NOTE: if the application mistakenly specifies the same event to be
  selected as well as deselected, the event is deselected.

DEPENDENCIES
  None.

PARAMETERS
   sockfd           socket descriptor
   select_mask      bit mask of events to select
   delect_mask      bit mask of events to deselect
  *dss_errno        address of error condition code

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, returns DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF          invalid socket descriptor is specified

SIDE EFFECTS
  None.

===========================================================================*/
long dss_async_select_deselect
(
  int    sockfd,                       /* socket descriptor                */
  long   select_mask,                  /* bit mask of events to select     */
  long   deselect_mask,                /* bit mask of events to deselect   */
  short *dss_errno                     /* address of error condition value */
);

/*===========================================================================

FUNCTION DSS_MULTIPLE_ASYNC_SELECT_DESELECT()

DESCRIPTION
  Enables and disables asynchronous event(s) notification on multiple socket
  descriptors.  Application passes in the addresses of two socket bit arrays,
  where a bit maps to a socket descriptor.  The first bit array is the "read"  
  bit array.  All sockets interested in receiving the DS_READ_EVENT
  notification have their corresponding bit set in this array.  The second
  bit array is the "write" bit array.  All sockets interested in receiving 
  DS_WRITE_EVENT notification have their corresponding bit set in this array.
  Sockets that do now want to receive the DS_READ_EVENT and/or the
  DS_WRITE_EVENT notification have their corresponding bit in the respective
  bit array reset.

DEPENDENCIES
  None.

PARAMETERS
  *readfds          address of a socket bit array specifying sockets that are
                    interested and not interested in receiving a
                    DS_READ_EVENT notification 
  *writefds         address of a socket bit array specifying sockets that are
                    interested and not interested in receiving a
                    DS_WRITE_EVENT notification
  *validfds         address of a socket bit array specifying sockets that are
                    owned by the task
  *dss_errno        address of error condition code

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, returns DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF          invalid socket descriptor is specfied

SIDE EFFECTS
  None.

===========================================================================*/
int dss_multiple_async_select_deselect
(
  fd_set *readfds,
  fd_set *writefds,
  fd_set *validfds,
  short  *dss_errno
);

/*===========================================================================

FUNCTION DSS_GET_SOCKETS_EVENTS()

DESCRIPTION
  Checks a set of socket descriptors for those who have data and another set
  for those who can accept data.  The sets of socket descriptors are bit
  arrays where each bit correspond to a socket descriptor and are hereon
  referred to as readfds and writefds.  When a bit in readfds is set, the 
  the application is interested in the corresponding socket's data
  availability.  If that socket has data, the socket is said to be ready and
  its bit in readfds is left set.  Otherwise, it is reset.  When a bit in
  writefds is set, the application is interested in the corresponding
  socket's ability to accept data.  If that socket can accept data, the
  socket is aid to be ready and its bit in writefds is left set.  Otherwise,
  it is reset.  A count of ready sockets is maintained to be returned to the
  caller.

DEPENDENCIES
  None.

PARAMETERS
  *readfds          address of a socket bit array specifying sockets that are
                    to be checked for data
  *writefds         address of a socket bit array specifying sockets that are
                    to be checked for whether or not they can accept data 
  *dss_errno        address of error condition code

RETURN VALUE
  Returns the number of ready socket descriptors on success.

  On error, returns DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF          invalid socket descriptor is specfied

SIDE EFFECTS
  None.

===========================================================================*/
int dss_get_sockets_events
(
  sint15  app_id,
  fd_set *readfds,
  fd_set *writefds,
  short  *dss_errno
);
#endif /* FEATURE_DS_SOCKETS_BSD */
#endif /* DSSBSDH_H */
