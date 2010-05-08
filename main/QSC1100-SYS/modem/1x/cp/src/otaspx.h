#ifdef FEATURE_OTASP
#ifndef OTASPX_H
#define OTASPX_H
/*===========================================================================

     O T A S P   T R A N S L A T I O N   D A T A   S T R U C T U R E S

DESCRIPTION
  This file contains data structures and functions for OTASP translation
  services.  Translating OTASP messages to and from external and internal
  formats.

Copyright (c) 1996-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/otaspx.h_v   1.0.2.0   30 Nov 2001 17:57:00   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/otaspx.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/96   dna     Checked in initial version.
04/03/96   day     Created file. 

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "otaspi.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*     TYPES FOR OTASP PARAMETER VALUES                                    */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/* OTASP Translation Status Codes                       */
/* Used to indicate status of OTASP message translation */
typedef enum
{
  OTASPX_DONE_S,           /* Successful translation completed           */
  OTASPX_INV_MSG_S,        /* Invalid OTASP message type detected        */
  OTASPX_INV_LEN_S         /* Invalid OTASP Data Message length detected */
} otaspx_status_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION OTASPX_EXT_TO_INT

DESCRIPTION
  Procedure which translates OTASP messages from external to internal
  format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern word otaspx_ext_to_int
(
  word otasp_len,
    /* Length of OTASP message in bytes */
  byte *rx_otasp_ptr,    
    /* Pointer to received message to be translated */
  otaspi_ftc_msg_type *int_otasp_ptr
    /* Pointer to place where translated message should be placed */
);

/* <EJECT> */
/*===========================================================================

FUNCTION OTASPX_INT_TO_EXT

DESCRIPTION
  Procedure which translates OTASP messages from internal to external
  format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern word otaspx_int_to_ext
(
  word *len_ptr,
    /* Pointer to where message length, in bits should be placed */
  otaspi_rtc_msg_type *int_msg_ptr,  
    /* Pointer to internal message to be translated */
  byte *tx_otasp_ptr
    /* Pointer to place where translated message should be placed */
);

#endif /* OTASPX_H */
#endif /* FEATURE_OTASP */


