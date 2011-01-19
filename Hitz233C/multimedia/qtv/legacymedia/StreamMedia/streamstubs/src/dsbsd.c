/*========================================================================== */


/* =======================================================================
D A T A  S E R V I C E S  B S D  S O C K E T  A P I  F I L E

DESCRIPTION

The Data Services mobile BSD socket API file. Contains basic API functions
forsocket manipulation.

========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/src/dsbsd.c#3 $
$DateTime: 2008/09/18 21:24:40 $
$Change: 746805 $

========================================================================== */

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "dsbsd.h"
#include "qtvsystem.h"

int bsd_shutdown
(
  void *ptr
)
{
  return 0;
}

int bind
(
int sockfd,                                         /* socket descriptor */
struct sockaddr *localaddr,                             /* local address */
int addrlen                                         /* length of address */
)
{
  return 0;
}

int close
(
int sockfd
)
{
  return 0;
}

int connect
(
int sockfd,
struct sockaddr *addr,
int addrlen
)
{
  return 0;
}

int read
(
int   sockfd,
void *buf,
unsigned int len                    /* number of bytes to read, can be 0 */
)
{
  return 0;
}


int recv
(
int   sockfd,
void *buf,
int   len,
int   flags
)
{
  return 0;
}

int recvfrom
(
int   sockfd,
void *buf,
int   len,
int   flags,
struct sockaddr *fromaddr,
int  *fromlen
)
{
  return 0;
}

int select
(
int     n,                                                    /* ignored */
fd_set *readfds,
fd_set *writefds,
fd_set *exceptfds,
struct timeval *timeout
)
{
  QTV_USE_ARG1(timeout);
  return -1;
}

int send
(
int   sockfd,
const void *buf,
int   len,
int   flags
)
{
  return 0;
}

int sendto
(
int   sockfd,
const void *buf,
int   len,
int   flags,
struct sockaddr *toaddr,
int   tolen
)
{
  return 0;
}

int socket
(
int family,                             /* Address family - AF_INET only */
int type,                                                 /* socket type */
int protocol                                          /* protocol number */
)
{
  return 0;
}
int bsd_init
(
default_sigs_type* defaults_sigs,
rex_sigs_type   watchdog_sig,
rex_timer_type *watchdog_timer,
rex_timer_cnt_type watchdog_rpt_time,
unsigned long   watchdog_rpt_mask
)
{
  return 0;
}
unsigned long inet_addr(const char *addr)
{
  return 0;
}
