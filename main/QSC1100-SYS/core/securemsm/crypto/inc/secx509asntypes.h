#ifndef SECX509TYPES_H
#define SECX509TYPES_H

#ifdef FEATURE_SEC

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

            A B S T R A C T   S Y N T A X   N O T A T I O N   

       S E C X 5 0 9    S P E C I F I C   T Y P E   D E F I N I T I O N S

GENERAL DESCRIPTION
  The Abstract Syntax Notation allows country and machine independent
  exchange of information.
  This file includes SSL specific type definitions.

  The following types are used by SSL to describe certificate
  contents:

  - ASN.1 Intergers
  - ASN.1 Strings
  - ASN.1 UTC Time

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secx509asntypes.h#1 $ 
  $DateTime: 2007/06/26 13:17:46 $ 
  $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/06   df      CR100945 Increase Max rec len to 64
05/30/01   om      Fixed pointer names
03/02/00   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM ASN.1

===========================================================================*/

#define SECSSLASN_MAX_REC_LEN       64

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/* ASN.1 Integer Type */
typedef struct 
{
  uint32   length;                 /* Length of the integer data           */
  uint8*   data_ptr;               /* Integer data                         */
} SECASN1_INTEGER;

/* ASN.1 String Type */
typedef struct 
{
  uint32   length;                 /* Length of the string data            */
  uint8*   data_ptr;               /* String data                          */
} SECASN1_STRING;

/* ASN.1 Universal Time Coordinated (UTC) Type */
typedef struct 
{
  uint32   length;                 /* Length of the time data              */
  uint8*   data_ptr;               /* Time data                            */
} SECASN1_UTCTIME;


#endif /* FEATURE_SEC */

#endif
