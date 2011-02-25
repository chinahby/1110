#ifndef  _NETUSER_H
#define  _NETUSER_H

/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


   Copyright (c) 1995 by QUALCOMM Incorporated.  All Rights Reserved.
   Copyright (c) 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/netuser.h_v   1.1   08 Oct 2002 23:17:44   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/netuser.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/02   aku     Removed FEATURE_DATA_MM featurization.
09/10/02   aku     Removed global Ip_addr if FEATURE_DATA_MM is defined.
===========================================================================*/

#include "customer.h"       /* Customer Specific Features */

/* Global structures and constants needed by an Internet user process */

#ifndef _PSGLOBAL_H
#include "psglobal.h"
#endif

#define  NCONN  20    /* Maximum number of open network connections */
extern int Net_error;  /* Error return code */
extern char Inet_eol[];

#define  NONE        0  /* No error */
#define  CON_EXISTS  1  /* Connection already exists */
#define  NO_CONN     2  /* Connection does not exist */
#define  CON_CLOS    3  /* Connection closing */
#define  NO_MEM      4  /* No memory for TCB creation */
#define  WOULDBLK    5  /* Would block */
#define  NOPROTO     6  /* Protocol or mode not supported */
#define  INVALID     7  /* Invalid arguments */

/* Codes for the tcp_open call */
#define  TCP_PASSIVE  0
#define  TCP_ACTIVE   1
#define  TCP_SERVER   2  /* Passive, clone on opening */

/* Local IP wildcard address */
#define  INADDR_ANY  0x0L

/* Socket structure */
struct socket {
  ip4a address;    /* IP address */
  uint16 port;     /* port number */
};

/* Connection structure (two sockets) */
struct connection {
  struct socket local;
  struct socket remote;
};

/* In netuser.c: */
ip4a aton(char *s);
char *inet_ntoa(ip4a a);
char *pinet(struct socket *s);

#endif  /* _NETUSER_H */
