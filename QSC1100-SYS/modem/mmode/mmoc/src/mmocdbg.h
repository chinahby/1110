#ifndef MMOCDBG_H
#define MMOCDBG_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                     D E B U G      M O D U L E

GENERAL DESCRIPTION
   This module contains the debug related functionality of MMoC.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before DEBUG APIs are called.


Copyright (c) 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/mmoc/rel/08H1/src/mmocdbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/08   sv      Correcting Featurization for Memory Reduction
09/19/08   jd      Memory reduction changes
04/28/08   fj      Lint cleanup, CR 144389
03/12/08   aj      Edit header to reflect p4 path and revision
04/03/06   ka      Changing copyright.
12/30/05   ka      Initial revision to support string maps

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "customer.h" /* Customer configuration file.                      */
#include "comdef.h"   /* Definition for basic types and macros             */
#include "msg.h"      /* Message output services.                          */
#include "mmoci.h"    /* Internal interface definitions for MMoC.          */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MMOC DEBUG STRING NAMES ======================
=============================================================================
=============================================================================
===========================================================================*/

#ifndef FEATURE_MMOC_LOW_MEM_TARGET
/* Provide strings for  values mmoc_cmd_e_type */
extern const char* mmocdbg_cmd_e_type_string_map [MMOC_CMD_MAX];


  /* Provide strings for  values mmoc_rpt_e_type */
extern const char* mmocdbg_rpt_e_type_string_map [MMOC_RPT_MAX];


  /* Provide strings for  values mmoc_trans_state_e_type */
extern const char* mmocdbg_trans_state_e_type_string_map [MMOC_TRANS_STATE_MAX];

  /* Provide strings for  values mmoc_trans_e_type */
extern const char* mmocdbg_trans_e_type_string_map [MMOC_TRANS_MAX];

/* Provide strings for  values prot_state_e_type */
extern const char* mmocdbg_prot_state_e_type_string_map [PROT_STATE_MAX];



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MMOC DEBUG API's =============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION MMOCDBG_PRINT_MESSAGE

DESCRIPTION

 Behaves as printf() - takes the very same format string and any number of
 parameters and allows to print <stuff> to diagnostic monitor output
 screen.

 The problem is that %s in the format string does not work for
 MSG_HIGH (and other MSG_ macros) plus those macros support only
 maximum of 3 arguments, so this function was provided to remedy both problems.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mmocdbg_print_message
(
     
     const char *string,
       /* Format in which string needs to be printed */
     ...
);

#endif /* FEATURE_MMOC_LOW_MEM_TARGET */

#endif /* MMOCDBG_H */
