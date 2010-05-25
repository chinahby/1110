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

Copyright (c) 2001-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/03/05    vs     commented the use of QOS feature in this file 
                    since its usage resulted in connection failure
                    while streaming for targets not supporting the QOS feature
11/12/04    wcl    merged back in msr changes under featurization:
                     dssocki_indextoscb() is called in order to get sockfd
                     instead of directly accessing array maintained by
                     dssocki.c
10/09/03     ss    Brought in sync with the DS API changes
12/11/02     mt    Fix dss_multiple_async_select_deselect() where it
                   deselects a socket event on a socket that does not belong
                   to the app.
06/05/02     na    Fixed bug where app was not being notified when events
                   occured.
06/20/01     mt    Create file.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#ifdef FEATURE_DS_SOCKETS_BSD
#include "target.h"
#include "dssocket.h"
#include "dssbsdh.h"
#ifdef PLATFORM_LTK
#error code not present
#else
#include "dssocki.h"
#endif /* PLATFORM_LTK */
#include "assert.h"
#include "msg.h"

/*===========================================================================

                              FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSS_ASYNC_SELECT_NO_NOTIFY

DESCRIPTION
  Enables the events to be notified of through the asynchronous notification
  mechanism.  Application specifies a bitmask of events that it is interested
  in, for which it will receive asynchronous notification via its application
  callback function. This function also performs a real-time check to
  determine if any of the socket events have already occurred, and if so
  returns a pointer to the socket's control block.  It is up to the caller to
  notify the application.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to a socket control block or NULL with *dss_errno = 0 on
  success.

  On error, returns NULL and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Sets the relevant event mask in the socket control block.

===========================================================================*/
static scb_type* dss_async_select_no_notify
(
  sint15 sockfd,                                      /* socket descriptor */
  uint32 interest_mask,                        /* bitmask of events to set */
  uint32 *happened_mask,                       /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    ASSERT(0);
    return NULL;
  }
  *dss_errno = 0;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return NULL;
  }

  INTLOCK();
  /*-------------------------------------------------------------------------
    Reset the notified callback flag, so that the callback can be called
    again.
  -------------------------------------------------------------------------*/
  scb_ptr->acb_ptr->notified_cb_flag = FALSE;

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Add it to existing requested
    mask.
  -------------------------------------------------------------------------*/
  scb_ptr->requested_event_mask |= interest_mask;

  INTFREE();

  /*-------------------------------------------------------------------------
    Do a realtime check to see if any of the events are true. If there are
    any active events, return the pointer to the socket control block so that
    the caller can perform the callback later.
  -------------------------------------------------------------------------*/
  if ( (*happened_mask =
          dssocki_socket_event_check( scb_ptr,
                                      scb_ptr->requested_event_mask )) != 0 )
  {
    return scb_ptr;
  }

  return NULL;

} /* dss_async_select_no_notify() */

/*===========================================================================

FUNCTION DSSBSD_GETNEXTEVENT()

DESCRIPTION
  Function based on dss_getnextevent() with the following changes.  The
  function only checks events of one specific socket whose descriptor is
  passed in by the application.  The DS_WRITE_EVENT is ignored and cleared if
  the socket is a TCP socket and the connection attempt failed.

DEPENDENCIES
  None.

PARAMETERS
   app_id           application ID
   sockfd           socket descriptor
  *dss_errno        address of error condition code

RETURN VALUE

SIDE EFFECTS
  Clears the bits in the socket control block event mask, corresponding to
  the events that have occurred.

===========================================================================*/
static sint31 dssbsd_getnextevent
(
  sint15 app_id,
  sint15 sockfd,
  sint15 *dss_errno
)
{
  sint31 event_mask=0;                              /* returned event mask */
  struct scb_s *scb_ptr;                    /* ptr to socket control block */
  acb_type *acb_ptr;         /* ptr to application control block structure */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    ASSERT(0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Check for invalid application ID
  -------------------------------------------------------------------------*/
  if ( (acb_ptr = dssocki_appidtoacb(app_id)) == NULL)
  {
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Clear the notified flag.
  -------------------------------------------------------------------------*/
  INTLOCK();
  acb_ptr->notified_cb_flag = FALSE;
  INTFREE();

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ( (scb_ptr=dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Make sure that the sockfd belongs to this app.
  -------------------------------------------------------------------------*/
  if (acb_ptr != scb_ptr->acb_ptr)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Get the event mask for the socket and if the requested events for that
    socket are enabled, return the event mask, and remove the events
    from the requested_event_mask.
  -------------------------------------------------------------------------*/
  event_mask = dssocki_socket_event_check( scb_ptr, scb_ptr->requested_event_mask );

  if ( event_mask & scb_ptr->requested_event_mask)
  {
    INTLOCK();

     scb_ptr->requested_event_mask &= ~(uint32)(event_mask);
    
    INTFREE();

    /*-----------------------------------------------------------------------
      Return the DS_WRITE_EVENT for a TCP socket only if there is a TCP
      connection.
    -----------------------------------------------------------------------*/
    if (event_mask & DS_WRITE_EVENT &&
        scb_ptr->type == SOCK_STREAM &&
        scb_ptr->socket_state != DSSOCKI_OPEN)
    {
      event_mask &= ~DS_WRITE_EVENT;
    }

    return(event_mask);
  }
  /*-------------------------------------------------------------------------
    No events have occurred, so just return 0.
  -------------------------------------------------------------------------*/
  return(DSS_SUCCESS);

} /* dssbsd_getnextevent() */

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
)
{
  struct scb_s *scb_ptr;                    /* Ptr to socket control block */
  uint32 event_mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Assert if the pointer to dss_errno is invalid.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    ASSERT(0);
    return DSS_ERROR;
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Convert socket descriptor to SCB
  -------------------------------------------------------------------------*/
  if ((scb_ptr = dssocki_sockfdtoscb(sockfd)) == NULL)
  {
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  INTLOCK();
  /*-------------------------------------------------------------------------
    Reset the notified callback flag, so that the callback can be called
    again.
  -------------------------------------------------------------------------*/
  scb_ptr->acb_ptr->notified_cb_flag = FALSE;

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket by ORing with the select bit
    mask and ANDing it with the complement of the deselect_mask.
  -------------------------------------------------------------------------*/
  scb_ptr->requested_event_mask |= select_mask;
  scb_ptr->requested_event_mask &= ~(uint32)(deselect_mask);
  INTFREE();

  /*-------------------------------------------------------------------------
    Do a realtime check to see if any of the events are true. If there are
    any active events, call the callback.
  -------------------------------------------------------------------------*/
  if ((event_mask =
         dssocki_socket_event_check(scb_ptr,
                                    scb_ptr->requested_event_mask)) != 0 )
  {
    dssocki_notify_app(scb_ptr, event_mask);
  }

  return DSS_SUCCESS;

} /* dss_async_select_deselect() */

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

  This function assumes that all the sockets represented in the socket bit
  array arguments are created by the caller of this function.

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
)
{
  int       i;              /* Loop index                                  */
  scb_type *scb_ptr = NULL; /* Socket control block pointer                */
  scb_type *ret_ptr;        /* dss_async_select_no_notify() return pointer */
//#ifdef FEATURE_QTV_QOS_SELECTION
  scb_type *index_ptr;      /* Pointer returned by dssocki_indextoscb()    */
//#endif
  int       ret_val;        /* Return value                                */
  uint32    event_mask;
  uint32    happened_mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret_val = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Examine each bit of the readfds and writefds bit arrays.  Each bit is
    mapped to a socket descriptor via the scb_array[] array.  When a bit in
    the readfds array is set, it indicates that the application wants to know
    when the corresponding socket is read-ready.  Similarly, when a bit in
    the writefds array is set, it indicates that the application wants to
    know when the corresponding socket can be written to.
  -------------------------------------------------------------------------*/
  for (i = 0; i < FD_SET_SIZE; i++)
  {
    event_mask = 0;
    if (_ISSET(i, readfds))
    {
      /*---------------------------------------------------------------------
        The application wants to know when the socket corresponding to bit i
        has data available.  Set the DS Sock code to notify the BSD to DS
        layer when the socket gets a read event.  Note that when remote TCP
        peer ends the connection, the local TCP socket gets a read event.
        Hence, there is no need to register the DS_CLOSE_EVENT for the
        socket.
      ---------------------------------------------------------------------*/
//#ifdef FEATURE_QTV_QOS_SELECTION
      if (NULL == (index_ptr = dssocki_indextoscb(i)))
      {
        MSG_MED("Event notification is registered for an invalid sockfd "
                "(index is %d)", i, 0, 0);

        *dss_errno = DS_EBADF;
        ret_val = DSS_ERROR;
        break;
      }
      if ((ret_ptr = dss_async_select_no_notify(index_ptr->sockfd,
                                                DS_READ_EVENT,
                                                &happened_mask,
                                                dss_errno)) != NULL)
//#else
     /* if ((ret_ptr = dss_async_select_no_notify((sint15)scb_array[i].sockfd,
                                                DS_READ_EVENT,
                                                &happened_mask,
                                                dss_errno)) != NULL)*/
//#endif
      {
        /*-------------------------------------------------------------------
          A real-time check of the events by dss_async_select_no_notify()
          indicates that the socket is read-ready.  Store the returned
          socket control block so that the owner of the socket can be
          notified later.
        -------------------------------------------------------------------*/
        event_mask |= happened_mask;
        scb_ptr = ret_ptr;
      }
      else if (*dss_errno != DSS_SUCCESS)
      {
        ret_val = DSS_ERROR;
        break;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Either there is no valid socket to bit i or the application is not
        interested when the socket corresponding to bit i is read ready.  For
        the latter case, ensure that the DS Sock code does not call the
        socket callback function when the socket receives data or when the
        remote TCP peer ends the connection.
      ---------------------------------------------------------------------*/
      if (_ISSET(i, validfds))
      {
//#ifdef FEATURE_QTV_QOS_SELECTION
        if (NULL == (index_ptr = dssocki_indextoscb(i)))
        {
          MSG_MED("Event notification is registered for an invalid sockfd "
                  "(index is %d)", i, 0, 0);

          *dss_errno = DS_EBADF;
          ret_val = DSS_ERROR;
          break;
        }
        if (dss_async_deselect(index_ptr->sockfd,
                               DS_READ_EVENT | DS_CLOSE_EVENT,
                               dss_errno) == DSS_ERROR)
//#else
        /*if (dss_async_deselect((sint15)scb_array[i].sockfd,
                               DS_READ_EVENT | DS_CLOSE_EVENT,
                               dss_errno) == DSS_ERROR)*/
//#endif
        {
          ret_val = DSS_ERROR;
          break;
        }
      }
    }

    if (_ISSET(i, writefds))
    {
      /*---------------------------------------------------------------------
        The application wants to know when the socket corresponding to bit i
        can be written to.  Set the DS Sock code to notify the BSD to DS
        layer when the socket can accept data or when its TCP peer closes the
        connection.
      ---------------------------------------------------------------------*/
//#ifdef FEATURE_QTV_QOS_SELECTION

      if (NULL == (index_ptr = dssocki_indextoscb(i)))
      {
        MSG_MED("Event notification is registered for an invalid sockfd "
                "(index is %d)", i, 0, 0);

        *dss_errno = DS_EBADF;
        ret_val = DSS_ERROR;
        break;
      }
      if ((ret_ptr = dss_async_select_no_notify(index_ptr->sockfd,
                                             DS_WRITE_EVENT | DS_CLOSE_EVENT,
                                             &happened_mask,
                                             dss_errno)) != NULL)
//#else
     /* if ((ret_ptr = dss_async_select_no_notify((sint15)scb_array[i].sockfd,
                                             DS_WRITE_EVENT | DS_CLOSE_EVENT,
                                             &happened_mask,
                                             dss_errno)) != NULL)*/
//#endif
      {
        /*-------------------------------------------------------------------
          A real-time check of the events by dss_async_select_no_notify()
          indicates that the socket is write-ready.  Store the returned
          socket control block so that the owner of the socket can be
          notified later.
        -------------------------------------------------------------------*/
        event_mask |= happened_mask;
        scb_ptr = ret_ptr;
      }
      else if (*dss_errno != DSS_SUCCESS)
      {
        ret_val = DSS_ERROR;
        break;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Either there is no valid socket to bit i or the application is not
        interested when the socket corresponding to bit i is write ready.
        For the latter case, ensure that the DS Sock code does not call the
        socket callback function when the socket can accept data or when its
        TCP peer socket terminates the connection.
      ---------------------------------------------------------------------*/
      if (_ISSET(i, validfds))
      {
///#ifdef FEATURE_QTV_QOS_SELECTION
        if (NULL == (index_ptr = dssocki_indextoscb(i)))
        {
          MSG_MED("Event notification is registered for an invalid sockfd "
                  "(index is %d)", i, 0, 0);

          *dss_errno = DS_EBADF;
          ret_val = DSS_ERROR;
          break;
        }
        if (dss_async_deselect(index_ptr->sockfd,
                               DS_WRITE_EVENT | DS_CLOSE_EVENT,
                               dss_errno) == DSS_ERROR)
//#else
       /* if (dss_async_deselect((sint15)scb_array[i].sockfd,
                               DS_WRITE_EVENT | DS_CLOSE_EVENT,
                               dss_errno) == DSS_ERROR)*/
//#endif
        {
          ret_val = DSS_ERROR;
          break;
        }
      }
    }
  } /* for */

  if (scb_ptr != NULL)
  {
    dssocki_notify_app(scb_ptr, event_mask);
  }

  return ret_val;

} /* dss_multiple_async_select_deselect() */

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
   app_id           Application ID
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
  sint15 app_id,
  fd_set *readfds,
  fd_set *writefds,
  short  *dss_errno
)
{
  uint32    i;                       /* Loop index                         */
  sint31 events_state;               /* Events bit array of a socket       */
  int    fd_ready;                   /* Socket read or write-ready flag    */
  int    num_ready = 0;              /* Number of socket descriptors ready */
//#ifdef FEATURE_QTV_QOS_SELECTION
  scb_type *index_ptr;         /* Pointer returned by dssocki_indextoscb() */
//#endif
  sint15 dss_sockfd;                 /* Socket file descriptor             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check if any of the socket descriptors the application wants monitored
    for read and/or write readiness are indeed ready.  Loop through each bit
    in the readfds and writefds bit arrays.  If the bit is set in either
    array and corresponds to a socket, dss_getnextevent() is called to get
    the event state of the socket.  If the event state indicates a read event
    on the socket and the application wanted to know when that socket has
    data, the bit in the readfds bit array that corresponds to the socket is
    left set.  Otherwise, the bit is reset.  Likewise, if the event state
    indicates a write event on the socket and the application wanted to know
    when the socket can accept data, the bit in the writefds bit array that
    corresponds to the socket is left set.  Otherwise, the bit is reset.
    When a bit in the readfds and/or writefds bit array(s) is left set, a
    count that counts the number of descriptors ready is incremented.
  -------------------------------------------------------------------------*/
  for (i = 0; i < FD_SET_SIZE; i++)
  {
    fd_ready = 0;
//#ifdef FEATURE_QTV_QOS_SELECTION
    if ((_ISSET(i, readfds) || _ISSET(i, writefds)))
    {
      if (NULL == (index_ptr = dssocki_indextoscb(i)))
      {
        *dss_errno = DS_EBADF;
        MSG_HIGH("dssbsd_getnextevent() failed! error %d", dss_errno, 0, 0);
        return -1;
      }

      dss_sockfd = index_ptr->sockfd;

//#else
  /*  if ((_ISSET(i, readfds) || _ISSET(i, writefds)) &&
        scb_array[i].sockfd != 0)
    {
      dss_sockfd = scb_array[i].sockfd;*/
//#endif
      if ((events_state = dssbsd_getnextevent(app_id,
                                              dss_sockfd,
                                              dss_errno)) != DSS_ERROR)
      {
        if ((events_state & DS_READ_EVENT) && (_ISSET(i, readfds)))
        {
          num_ready++;
          fd_ready = 1;
          MSG_LOW("Socket %d read-ready", dss_sockfd, 0, 0);
        }
        else
        {
          if (readfds != NULL)
          {
            _RESET(i, readfds);
          }
        }

        if (events_state & DS_WRITE_EVENT && _ISSET(i, writefds))
        {
          MSG_LOW("Socket %d write-ready", dss_sockfd, 0, 0);
          if (!fd_ready)
          {
            num_ready++;
          }
        }
        else
        {
          if (writefds != NULL)
          {
            _RESET(i, writefds);
          }
        }
      }
      else
      {
        MSG_HIGH("dssbsd_getnextevent() failed! error %d", dss_errno, 0, 0);
        return -1;
      }
    }
  }
  return num_ready;

} /* dss_get_sockets_event() */
#endif /* FEATURE_DS_SOCKETS_BSD */
