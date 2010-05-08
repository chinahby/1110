#ifndef PS_PPP_SLHC_H
#define PS_PPP_SLHC_H

/*===========================================================================

                          P S _ P P P _ S L H C . H

DESCRIPTION
  Header file for the VJ header compression module.

 Redistribution and use in source and binary forms are permitted
 provided that the above copyright notice and this paragraph are
 duplicated in all such forms and that any documentation,
 advertising materials, and other materials related to such
 distribution and use acknowledge that the software was developed
 by the University of California, Berkeley.  The name of the
 University may not be used to endorse or promote products derived
 from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 Compressed packet format:

 The first octet contains the packet type (top 3 bits), TCP
 'push' bit, and flags that indicate which of the 4 TCP sequence
 numbers have changed (bottom 5 bits).  The next octet is a
 conversation number that associates a saved IP/TCP header with
 the compressed packet.  The next two octets are the TCP checksum
 from the original datagram.  The next 0 to 15 octets are
 sequence number changes, one change per bit set in the header
 (there may be no changes and there are two special cases where
 the receiver implicitly knows what changed -- see below).

 There are 5 numbers which can change (they are always inserted
 in the following order): TCP urgent pointer, window,
 acknowlegement, sequence number and IP ID.  (The urgent pointer
 is different from the others in that its value is sent, not the
 change in value.)  Since typical use of SLIP links is biased
 toward small packets (see comments on MTU/MSS below), changes
 use a variable length coding with one octet for numbers in the
 range 1 - 255 and 3 octets (0, MSB, LSB) for numbers in the
 range 256 - 65535 or 0.  (If the change in sequence number or
 ack is more than 65535, an uncompressed packet is sent.)

 Packet types (must not conflict with IP protocol version)

 The top nibble of the first octet is the packet type.  There are
 three possible types: IP (not proto TCP or tcp with one of the
 control flags set); uncompressed TCP (a normal IP/TCP packet but
 with the 8-bit protocol field replaced by an 8-bit connection id --
 this type of packet syncs the sender & receiver); and compressed
 TCP (described above).

 LSB of 4-bit field is TCP "PUSH" bit (a worthless anachronism) and
 is logically part of the 4-bit "changes" field that follows.  Top
 three bits are actual packet type.  For backward compatibility
 and in the interest of conserving bits, numbers are chosen so the
 IP protocol version number (4) which normally appears in this nibble
 means "IP packet".

 Copyright (c) 1989 Regents of the University of California.
    All rights reserved.
 Redistribution and use in source and binary forms are permitted
 provided that the above copyright notice and this paragraph are
 duplicated in all such forms and that any documentation,
 advertising materials, and other materials related to such
 distribution and use acknowledge that the software was developed
 by the University of California, Berkeley.  The name of the
 University may not be used to endorse or promote products derived
 from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

 Copyright (c) 1995 by QUALCOMM Incorporated.  All Rights Reserved.
 Copyright (c) 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
 Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
 Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_slhc.h_v   1.1   13 Feb 2003 14:12:00   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_slhc.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/02/03    usb    Added device type to slhc_init().
07/25/02    mvl    Renamed file, file cleanup.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

#include "customer.h"       /* Customer Specific Features */
#include "dsm.h"
#include "ps_ip.h"
#include "ps_tcp.h"
#include "ps_ppp_defs.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/* SLIP compression masks for len/vers byte */

#define SL_TYPE_IP 0x40
#define SL_TYPE_UNCOMPRESSED_TCP 0x70
#define SL_TYPE_COMPRESSED_TCP 0x80
#define SL_TYPE_ERROR 0x00

/* Bits in first octet of compressed packet */

#define NEW_C  0x40  /* flag bits for what changed in a packet */
#define NEW_I  0x20
#define NEW_S  0x08
#define NEW_A  0x04
#define NEW_W  0x02
#define NEW_U  0x01

/* reserved, special-case values of above */

#define SPECIAL_I (NEW_S|NEW_W|NEW_U)        /* echoed interactive traffic */
#define SPECIAL_D (NEW_S|NEW_A|NEW_W|NEW_U)  /* unidirectional data */
#define SPECIALS_MASK (NEW_S|NEW_A|NEW_W|NEW_U)

#define TCP_PUSH_BIT 0x10

/*
 * data type and sizes conversion assumptions:
 *
 *  VJ code   KA9Q style      generic
 *  u_char    uint8         unsigned char    8 bits
 *  u_short   uint16        unsigned short  16 bits
 *  u_int     uint16        unsigned short  16 bits
 *  u_long    uint32        unsigned long   32 bits
 *  int       sint31        long            32 bits
 */

/*
 * "state" data for each active tcp conversation on the wire.  This is
 * basically a copy of the entire IP/TCP header from the last packet
 * we saw from the conversation together with a small identifier
 * the transmit & receive ends of the line use to locate saved header.
 */
struct cstate 
{
  uint8   this;         /* connection id number (xmit) */
  struct cstate *next;  /* next in ring (xmit) */
  struct ip cs_ip;      /* ip/tcp hdr from most recent packet */
  struct tcp cs_tcp;
};

/*
 * all the state data for one serial line (we need one of these per line).
 */
struct slcompress 
{
  struct cstate *tstate;  /* transmit connection states (array)*/
  struct cstate *rstate;  /* receive connection states (array)*/

  uint8  tslot_limit;  /* highest transmit slot id (0-l)*/
  uint8  rslot_limit;  /* highest receive slot id (0-l)*/

  uint8  xmit_oldest;   /* oldest xmit in ring */
  uint8  xmit_current;  /* most recent xmit id */
  uint8  recv_current;  /* most recent rcvd id */

  uint8  flags;

#define SLF_TOSS  0x01  /* tossing rcvd frames until id received */

  uint32 sls_o_nontcp;        /* outbound non-TCP packets */
  uint32 sls_o_tcp;           /* outbound TCP packets */
  uint32 sls_o_uncompressed;  /* outbound uncompressed packets */
  uint32 sls_o_compressed;    /* outbound compressed packets */
  uint32 sls_o_searches;      /* searches for connection state */
  uint32 sls_o_misses;        /* times couldn't find conn. state */

  uint32 sls_i_uncompressed;  /* inbound uncompressed packets */
  uint32 sls_i_compressed;    /* inbound compressed packets */
  uint32 sls_i_error;         /* inbound error packets */
  uint32 sls_i_tossed;        /* inbound packets tossed because of error */
};

#define SLHC_ARRAY_SIZE 16    /* Maximum slots to hold for TCP/IP headers */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/* In slhc.c: */

struct slcompress *slhc_init
(
  ppp_dev_enum_type  dev,
  int rslots, 
  int tslots
);

void slhc_free
(
  struct slcompress *comp
);

int slhc_compress
(
  struct slcompress *comp,
  struct dsm_item_s **bpp, 
  int compress_cid
);

int slhc_uncompress
(
  struct slcompress *comp,
  struct dsm_item_s **bpp
);

int slhc_remember
(
  struct slcompress *comp,
  struct dsm_item_s **bpp
);

int slhc_toss
(
  struct slcompress *comp
);

void slhc_i_status
(
  struct slcompress *comp
);

void slhc_o_status
(
  struct slcompress *comp
);

#endif  /* PS_PPP_SLHC_H */

