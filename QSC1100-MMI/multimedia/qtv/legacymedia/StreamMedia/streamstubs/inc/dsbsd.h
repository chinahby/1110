#ifndef DSBSD_H
#define DSBSD_H
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/streammedia/streamstubs/rel/1.0/inc/dsbsd.h#2 $
$DateTime: 2008/07/29 21:17:21 $
$Change: 712884 $
========================================================================== */

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rex.h"
#include "dssbsdh.h"
#include "ps_iface_defs.h"

#define INADDR_NONE     0xFFFFFFFFUL

typedef struct
{
  rex_sigs_type select;
  rex_sigs_type socket;
  rex_sigs_type net;
  rex_sigs_type iface_ioctl;
  rex_sigs_type dns;
  rex_sigs_type abort;
  rex_sigs_type tcp_close_timeout;
} default_sigs_type;

struct timeval
{
  uint32 tv_sec;                                           /* seconds      */
  uint32 tv_usec;                                          /* microseconds */
};

int bsd_shutdown
(
  void *ptr
);

int bind
(
  int sockfd,                                         /* socket descriptor */
  struct sockaddr *localaddr,                             /* local address */
  int addrlen                                         /* length of address */
);
int close
(
  int sockfd
);
int connect
(
  int sockfd,
  struct sockaddr *addr,
  int addrlen
);
int read
(
  int   sockfd,
  void *buf,
  unsigned int len                    /* number of bytes to read, can be 0 */
);

int recv
(
  int   sockfd,
  void *buf,
  int   len,
  int   flags
);
int recvfrom
(
  int   sockfd,
  void *buf,
  int   len,
  int   flags,
  struct sockaddr *fromaddr,
  int  *fromlen
);

int select
(
  int     n,                                                 
  fd_set *readfds,
  fd_set *writefds,
  fd_set *exceptfds,
  struct timeval *timeout
);

int send
(
  int   sockfd,
  const void *buf,
  int   len,
  int   flags
);

int sendto
(
  int   sockfd,
  const void *buf,
  int   len,
  int   flags,
  struct sockaddr *toaddr,
  int   tolen
);

int socket
(
  int family,                             /* Address family - AF_INET only */
  int type,                                                 /* socket type */
  int protocol                                          /* protocol number */
);
int bsd_init
(
  default_sigs_type* defaults_sigs,
  rex_sigs_type   watchdog_sig,
  rex_timer_type *watchdog_timer,
  rex_timer_cnt_type watchdog_rpt_time,
  unsigned long   watchdog_rpt_mask
);
unsigned long inet_addr(const char *addr);
#endif /* DSBSD_H */
