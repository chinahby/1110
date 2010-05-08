#ifndef PSGLOBAL_H
#define PSGLOBAL_H

/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


   Copyright (c) 1995-2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/psglobal.h_v   1.0   08 Aug 2002 11:20:00   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/psglobal.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/02   rc      Define NBBY if not already defined elsewhere.
08/19/99   ak      Redefined MAXINT to be the largest positive value that
                   can be fit into a 32-bit signed int.
08/09/99   mvl     Moved the various htonl() etc macro's into this file to
                   support network model development.
09/15/98   jct     Replaced inclusion of types.h with comdef.h
09/12/98   ldg     For T_ARM added #include "types.h".
09/01/98   ldg     For T_ARM omitted some typedefs.
11/13/95   jjw     Removed redundant min/max macros
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/



#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

/* Global definitions used by every source file.
 * Some may be compiler dependent.
 *
 * This file depends only on internal macros or those defined on the
 * command line, so it may be safely included first.
 */

/* These lines assume that your compiler's longs are 32 bits and
 * shorts are 16 bits. It is already assumed that chars are 8 bits,
 * but it doesn't matter if they're signed or unsigned.
 */
typedef unsigned long int ip4a;         /* IPv4 address */

#define MAXINT16 65535U            /* Largest 16-bit unsigned integer */
#define MAXINT32 0x7FFFFFFF        /* Largest 32-bit signed integer */
#ifndef NBBY
#define NBBY  8                    /* 8 bits/byte */
#endif

/* Since not all compilers support structure assignment, the ASSIGN()
 * macro is used. This controls how it's actually implemented.
 */
#ifdef  NOSTRUCTASSIGN  /* Version for old compilers that don't support it */
#define ASSIGN(a,b)  memcpy((char *)&(a),(char *)&(b),sizeof(b))
#else      /* Version for compilers that do */
#define ASSIGN(a,b)  ((a) = (b))
#endif

/* Define null object pointer in case stdio.h isn't included */
#ifndef NULL
/* General purpose NULL pointer */
#define NULL 0
#endif

/* Define End-Of-Line */
#ifndef EOL
/* char EOL */
#define EOL '\0'
#endif

#define CTLA 0x1
#define CTLB 0x2
#define CTLC 0x3
#define CTLD 0x4
#define CTLE 0x5
#define CTLF 0x6
#define CTLG 0x7
#define CTLH 0x8
#define CTLI 0x9
#define CTLJ 0xa
#define CTLK 0xb
#define CTLL 0xc
#define CTLM 0xd
#define CTLN 0xe
#define CTLO 0xf
#define CTLP 0x10
#define CTLQ 0x11
#define CTLR 0x12
#define CTLS 0x13
#define CTLT 0x14
#define CTLU 0x15
#define CTLV 0x16
#define CTLW 0x17
#define CTLX 0x18
#define CTLY 0x19
#define CTLZ 0x1a

#define ccNUL  0x00
#define ccBEL  CTLG
#define ccBS  CTLH
#define ccTAB  CTLI
#define ccLF  CTLJ
#define ccFF  CTLL
#define ccCR  CTLM
#define ccXON  CTLQ
#define ccXOFF  CTLS
#define ccESC  0x1b
#define ccDEL  0x7f

/* string equality shorthand */
#define STREQ(x,y) (strcmp(x,y) == 0)

/* Extract a short from a long */
#define hiword(x)  ((uint16)((x) >> 16))
#define loword(x)  ((uint16)(x))

/* Extract a byte from a short */
#define hibyte(x)  ((uint8)((x) >> 8))
#define lobyte(x)  ((uint8)(x))

/* Extract nibbles from a byte */
#define hinibble(x)  (((x) >> 4) & 0xf)
#define lonibble(x)  ((x) & 0xf)

/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO HTONL

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

/*===========================================================================

MACRO HTONS

DESCRIPTION
 Converts host-to-network short integer.  Handles potential byte order
 differences between different computer architectures and different network
 protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define htons(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))

/*===========================================================================

MACRO NTOHL

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

/*===========================================================================

MACRO NTOHS

DESCRIPTION
 Converts network-to-host short integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define ntohs(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))


#endif  /* PSGLOBAL_H */
