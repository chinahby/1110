#ifndef TXCMODINT_H
#define TXCMODINT_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         C D M A   MODULATOR INTERRUPT   M O D U L E

GENERAL DESCRIPTION
  Item related to the setup the MOD interrupt

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 through 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:                                                                              $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcmodint.h#1 $ $ $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/06   vlc     Fixed RVCT compiler warnings.
06/09/06   trc     Remove T_MSM6700 featurization
07/01/04   bn      Put txcmodint.h under feature T_MSM6700
06/18/04   bn      Added function header for de-registered func.
04/29/04   bn      Created the file

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define TXMOD_INT_NUM_PCG 16

/* all the identified client IDs uses TX INT */
typedef enum
{
  TXMOD_INT_MUX_CLT,
  TXMOD_INT_RF_CLT,
  TXMOD_INT_SRCH_CLT,
  TXMOD_INT_TOTAL_CLT
} txmod_int_client_id_enum_type;

typedef void (*txcmod_int_pcg_callback_type) (uint32);

typedef struct
{
  uint16  num_regis_func_per_pcg; /* total number of registered function for this PCG */
  struct
  {
   txcmod_int_pcg_callback_type fptr;
    uint32 arg; /* this can be a pointer to a input structure */
  } callbacks [TXMOD_INT_TOTAL_CLT];

} txcmod_int_pcg_registry_type;

/*===========================================================================
 FUNCTION TXCMOD_INT_INIT

DESCRIPTION
  Initialize the interrupt registry.

DEPENDENCIES
  This must be called at startup, and never again.

RETURN VALUE
  None.

SIDE EFFECTS
  Modifies the registry
===========================================================================*/
void txcmod_int_init (void);

/*===========================================================================
 FUNCTION TXCMOD_INT_REGIS_FOR_PCGINT

DESCRIPTION
  Add an ISR registration for a pcg.

DEPENDENCIES
  txcmod_int_init() must have been called previously.

RETURN VALUE

SIDE EFFECTS
  Modifies the registry
===========================================================================*/
boolean txcmod_int_regis_for_pcgint(
  uint8                          pcg,    /* PCG on which call should be given, from 0 to 15 */
  txcmod_int_pcg_callback_type   fptr,   /* function to be called in ISR */
  uint32                         arg,    /* argument to be passed to function in ISR */
  txmod_int_client_id_enum_type  clt_id  /* client id */
);

/*===========================================================================
 FUNCTION TXCMOD_INT_HANDLER

DESCRIPTION
  Handles the txcmod_int_handler interrupt and calls registered callbacks for the
  pcgs in TX_PCG_INT_STATUS.

DEPENDENCIES
  txcint_init() must have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txcmod_int_handler (void);



/*===========================================================================
 FUNCTION TXCMOD_INT_DEREGISTER_ALL

DESCRIPTION
  Deregisters all pcg interrupts registered for a client.

DEPENDENCIES
  txcint_init() must have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txcmod_int_deregister_all
(
  txmod_int_client_id_enum_type  clt_id  /* client id */
);
#endif /* TXCMODINT_H */

