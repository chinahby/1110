#ifndef AEEDNSTYPES_H
#define AEEDNSTYPES_H
/*=============================================================================

FILE:         AEEDNSTypes.h

SERVICES:     None

DESCRIPTION:  Common DNS types and definitions

===============================================================================
   Copyright (c) 2008 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// DNS message flags
typedef uint16 AEEDNSFlag;

#define AEEDNSFLAG_QR            0x8000    // Query/Response bit;  1 => response
#define AEEDNSFLAG_OPCODE_MASK   0x7800    // 4-bit field: opcode (type of query)
#define AEEDNSFLAG_AA            0x0400    // authoritative answer
#define AEEDNSFLAG_TC            0x0200    // message was truncated
#define AEEDNSFLAG_RD            0x0100    // recursion desired
#define AEEDNSFLAG_RA            0x0080    // recursion available
#define AEEDNSFLAG_Z             0x0070    // 3-bit field: should be zero
#define AEEDNSFLAG_RCODE_MASK    0x000F    // 4-bit field: error codes


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEDNSFlag

Description:
   This is an unsigned 16-bit integer used to define the DNS flags.

Definition:
   #define AEEDNSFLAG_QR            0x8000
   #define AEEDNSFLAG_OPCODE_MASK   0x7800
   #define AEEDNSFLAG_AA            0x0400
   #define AEEDNSFLAG_TC            0x0200
   #define AEEDNSFLAG_RD            0x0100
   #define AEEDNSFLAG_RA            0x0080
   #define AEEDNSFLAG_Z             0x0070
   #define AEEDNSFLAG_RCODE_MASK    0x000F
   
   typedef uint16 AEEDNSFlag;

Members:
   AEEDNSFLAG_QR:          Query/Response bit;  1 => response
   AEEDNSFLAG_OPCODE_MASK: 4-bit field: opcode (type of query)
   AEEDNSFLAG_AA:          authoritative answer
   AEEDNSFLAG_TC:          message was truncated
   AEEDNSFLAG_RD:          recursion desired
   AEEDNSFLAG_RA:          recursion available
   AEEDNSFLAG_Z:           3-bit field: should be zero
   AEEDNSFLAG_RCODE_MASK:  4-bit field: error codes

Comments:
   None

See Also:
   None.

=======================================================================*/

#endif  // AEEDNSTYPES_H
