#ifndef PS_IPHDR_H 
#define PS_IPHDR_H
/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


   Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iphdr.h_v   1.1   09 Sep 2002 16:26:20   mlioy  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iphdr.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/09/02    mvl    removed typedef for ip4a, and its use in file.        
07/25/02    om     Created module from ip.h

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include "target.h"
#include "comdef.h"

/* IP header, INTERNAL representation */

#define IPLEN      20  /* Length of standard IP header */
#define IP_MAXOPT  40  /* Largest option field, bytes */

typedef enum
{
  IP_V4 = 4,
  IP_V6 = 6
} ip_version_enum_type;

struct ip 
{
  uint32 source;      /* Source address */
  uint32 dest;        /* Destination address */
  uint16 length;    /* Total length */
  uint16 id;        /* Identification */
  uint16 offset;    /* Fragment offset in bytes */
  uint16 checksum;  /* Header checksum */

  struct 
  {
    unsigned int congest:1; /* Congestion experienced bit (exp) */
    unsigned int df:1;      /* Don't fragment flag */
    unsigned int mf:1;      /* More Fragments flag */
  } flags;

  uint8 version;            /* IP version number */
  uint8 tos;                /* Type of service */
  uint8 ttl;                /* Time to live */
  uint8 protocol;           /* Protocol */
  uint8 optlen;             /* Length of options field, bytes */
  uint8 options[IP_MAXOPT]; /* Options field */
};

/* Fields in option type byte */

#define OPT_COPIED  0x80  /* Copied-on-fragmentation flag */
#define OPT_CLASS   0x60  /* Option class */
#define OPT_NUMBER  0x1f  /* Option number */

/* IP option numbers */

#define IP_EOL    0      /* End of options list */
#define IP_NOOP   1      /* No Operation */
#define IP_SECURITY  2   /* Security parameters */
#define IP_LSROUTE  3    /* Loose Source Routing */
#define IP_TIMESTAMP  4  /* Internet Timestamp */
#define IP_RROUTE  7     /* Record Route */
#define IP_STREAMID  8   /* Stream ID */
#define IP_SSROUTE  9    /* Strict Source Routing */

/* Timestamp option flags */

#define TS_ONLY   0      /* Time stamps only */
#define TS_ADDRESS  1    /* Addresses + Time stamps */
#define TS_PRESPEC  3    /* Prespecified addresses only */


#endif  /* PS_IPHDR_H */
