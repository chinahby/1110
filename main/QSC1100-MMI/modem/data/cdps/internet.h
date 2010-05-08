#ifndef  _INTERNET_H
#define  _INTERNET_H

/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


   Copyright (c) 1995 by QUALCOMM Incorporated.  All Rights Reserved.
   Copyright (c) 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

#include "psglobal.h"
#include "customer.h"       /* Customer Specific Features */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/internet.h_v   1.0   01 Oct 2002 16:37:06   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/internet.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/03   om      Added IPSec ESP protocol type
11/14/96   jjw     Typecasted pseudo header type
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/* Global structures and constants pertaining to the interface between IP and
 * higher level protocols
 */

/* IP protocol field values */

#define  ICMP_PTCL   1  /* Internet Control Message Protocol */
#define  IP4_PTCL    4  /* IP inside IP */
#define  TCP_PTCL    6  /* Transmission Control Protocol */
#define  UDP_PTCL   17  /* User Datagram Protocol */
#define  ESP_PTCL   50  /* Encapsulated Secure Payload (IPSec) */
#define  IP_PTCL    94  /* JI's IP inside IP */
#define  SEC_PTCL  101  /* IP security protocol - NOT OFFICIAL */

#define  MAXTTL    254  /* Maximum possible IP time-to-live value, 
                           set per IS-99 requirement */

/* DoD-style precedences */

#define  IP_PRECEDENCE  0xe0  /* Mask for precedence field */
#define  ROUTINE        0x00
#define  PRIORITY       0x20
#define  IMMEDIATE      0x40
#define  FLASH          0x60
#define  FLASH_OVER     0x80
#define  CRITIC         0xa0
#define  INET_CTL       0xc0
#define  NET_CTL        0xe0

/* Amateur-style precedences */

#define  AM_ROUTINE    0x00
#define  AM_WELFARE    0x20
#define  AM_PRIORITY   0x40
#define  AM_EMERGENCY  0x60

/* Class-of-service bits */

#define  IP_COS       0x1c  /* Mask for class-of-service bits */
#define  LOW_DELAY    0x10
#define  THROUGHPUT   0x08
#define  RELIABILITY  0x04

/* IP TOS fields */

#define  PREC(x)     (((x)>>5) & 0x7)
#define  DELAY       0x10
#define  THRUPUT     0x8
#define  RELIABLITY  0x4

/* Pseudo-header for TCP and UDP checksumming */

typedef struct
{
  ip4a source;      /* IP source */
  ip4a dest;        /* IP destination */
  uint16 length;    /* Data field length */
  uint8  protocol;  /* Protocol */
} pseudo_header_type;

/* Format of a MIB entry for statistics gathering */
struct mib_entry {
  union {
    sint31 integer;
  } value;
};

#endif  /* _INTERNET_H */
