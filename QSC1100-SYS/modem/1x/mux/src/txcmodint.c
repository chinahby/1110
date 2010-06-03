/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         C D M A   MODULATOR INTERRUPT   M O D U L E

GENERAL DESCRIPTION
  Item related to the setup the MOD interrupt

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:                                                                              $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txcmodint.c#2 $ $ $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Reduced debug string sizes for ULC effort.
09/26/07   vlc     Changed #include "msmhwio.h" to #include "msm.h".
06/09/06   trc     Remove T_MSM6700 featurization
07/25/05   rkc     Fixed lint errors.
02/23/05   bn      Let tramp handle the second level int
07/29/04   bn      Optimize the register and de-register logic
07/29/04   bn      Clear PCG and second level status bits during initialization/de-reg
07/20/04   bn      Used its own variable to store the frame offset
07/01/04   bn      Put txcmodint.c under feature T_MSM6700
06/18/04   bn      Corrected the number of registered func., add the de-registered func.
06/04/04   bn      Fixed bug in the computation of number of register function per PCG
05/20/04   bn      Sync up the derived PCG from MOD TX time with the PCG status
04/29/04   bn      Created the file

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "msg.h"
#include "txcmodint.h"
#include "misc.h"
#include "hw.h"
#include "tramp.h"
#include "msm.h"                   /* Target specific definitions          */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
byte txcmod_int_frame_offset;
static txcmod_int_pcg_registry_type txcmod_int_pcg_registry[TXMOD_INT_NUM_PCG];

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
void txcmod_int_init (void)
{
  uint16 pcg, reg;

  for (pcg=0; pcg < TXMOD_INT_NUM_PCG; pcg++)
  {
    txcmod_int_pcg_registry[pcg].num_regis_func_per_pcg = 0;
    for (reg = 0; reg < TXMOD_INT_TOTAL_CLT; reg++)
    {
      txcmod_int_pcg_registry[pcg].callbacks[reg].fptr = NULL;
    }
  }
  txcmod_int_frame_offset = 0;
  /* disable all the PCG interrupt */
  HWIO_OUTM(TX_PCG_INT_ENABLE, 0x0, 0x0);
  /* clear the PCG INT status bits */
  HWIO_OUTM(TX_PCG_INT_CLEAR,0xffff,0xffff);
}

/*lint -save -esym(578,sav) -e502
 *     578 - Multiple macros happen to declare variable sav
 *     502 - Expected unsigned type.  Too dangerous to fix for now. */
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
static boolean txmod_set_int_handler_to_tramp = FALSE;
boolean txcmod_int_regis_for_pcgint(
  uint8                          pcg,    /* PCG on which call should be given */
  txcmod_int_pcg_callback_type   fptr,   /* function to be called in ISR */
  uint32                         arg,    /* argument to be passed to function in ISR */
  txmod_int_client_id_enum_type  clt_id  /* client id */
)
{

  uint16 index;
  uint16 total_num_clt_reg_function = 0;

  if ((pcg >= TXMOD_INT_NUM_PCG) || (clt_id >= TXMOD_INT_TOTAL_CLT))
  {
    MSG_ERROR("Bad input arg val",0,0,0);
    return(FALSE);
  }

  INTLOCK();

  if (fptr != NULL)
  {
    if (txcmod_int_pcg_registry[pcg].callbacks[clt_id].fptr == NULL)
    {
      txcmod_int_pcg_registry[pcg].num_regis_func_per_pcg++;
    }
    txcmod_int_pcg_registry[pcg].callbacks[clt_id].fptr = fptr;
    txcmod_int_pcg_registry[pcg].callbacks[clt_id].arg = arg;
    /* if the we have not set the INT handler to TRAMP, then do it */
    if (!txmod_set_int_handler_to_tramp)
    {
      txmod_set_int_handler_to_tramp = TRUE;
      /* There were no registered functions prior to this call */
      tramp_set_isr( TRAMP_1X_TX_FR_ISR, txcmod_int_handler );
    }

    /* There were no registered functions for this PCG prior to this call */
    if (txcmod_int_pcg_registry[pcg].num_regis_func_per_pcg == 1)
    {
      /* There were no registrations for this pcg before.
      * Enable it in hardware. */
      HWIO_OUTM(TX_PCG_INT_ENABLE, 1<<pcg, 0xffff);
    }
  }
  else
  {
    if (txcmod_int_pcg_registry[pcg].callbacks[clt_id].fptr != NULL)
    {
      txcmod_int_pcg_registry[pcg].num_regis_func_per_pcg--;
    }

    txcmod_int_pcg_registry[pcg].callbacks[clt_id].fptr = NULL;
    txcmod_int_pcg_registry[pcg].callbacks[clt_id].arg = 0;

    if (txcmod_int_pcg_registry[pcg].num_regis_func_per_pcg == 0)
    {
      /* There were no registrations for this pcg anymore.
      * Disable it in hardware. */
      HWIO_OUTM(TX_PCG_INT_ENABLE, 1<<pcg, 0x0);
    }

    /* check for all the PCG, to see if there is no register function, then
    reset the interrupt handler in TRAMP and clear all the second
    level MOD 1x INT bit*/
    for (index = 0; index < TXMOD_INT_NUM_PCG; index++)
    {
      total_num_clt_reg_function = total_num_clt_reg_function
          + txcmod_int_pcg_registry[index].num_regis_func_per_pcg;
    }

    if (total_num_clt_reg_function == 0)
    {
      txmod_set_int_handler_to_tramp = FALSE;
      /* There were no registered functions prior to this call */
      tramp_set_isr( TRAMP_1X_TX_FR_ISR, NULL );
      /* disable all the PCG interrupt */
      HWIO_OUTM(TX_PCG_INT_ENABLE, 0x0, 0x0);
      /* clear the PCG INT status bits */
      HWIO_OUTM(TX_PCG_INT_CLEAR,0xffff,0xffff);

    }
  }

  INTFREE();
  return(TRUE);
} /* txcint_register_for_pcg_interrupt */
/*lint -restore */

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
void txcmod_int_handler (void)
{
  uint16 mod_int_bits,index, curr_pcg_slot, clt_index;
  uint32 mod_current_chipx8time;


  /* get the PCG INT status bit */
  mod_int_bits = HWIO_IN(TX_PCG_INT_STATUS);

  /* clear the PCG INT status bits */
  HWIO_OUTM(TX_PCG_INT_CLEAR,mod_int_bits,mod_int_bits);

  /* tell the modulator to latch the current system time in chipx8*/
  HWIO_OUTM(MOD_TX_TIME_LATCH,0x1,0x1);

  /* read the current system time in chipx8, only 21 bits */
  mod_current_chipx8time = (HWIO_IN(MOD_TX_TIME_RD)) & 0x1FFFFF;

  /* conver from chipx8 into PCG. There are 1228800 chips per second,
  and (1228800 / 16*50) chip per PCG or (1228800 / 100) chipx8 per PCG*/
  curr_pcg_slot = (uint16) (mod_current_chipx8time / 12288);

  /* since the mod tx time is accumulated over 80 ms, then we need to mod it
  by 16 to get the PCG number in 20 ms frame and the MOD_TX_TIME_RD is based on
  the TX time with zero offset, and PCG status is based on the TX time with
  offset. there is txcmod_int_frame_offset PCG offset. To sysn up, we need to do this */

  curr_pcg_slot = (curr_pcg_slot + 16 - (txcmod_int_frame_offset & 0x0F)) % 16;

  /* check for all the PCG from curr_pcg_slot + 1 to the end PCG first */
  for (index = curr_pcg_slot + 1; index < TXMOD_INT_NUM_PCG; index++)
  {
    if (mod_int_bits & (0x1 << index))
    {
      if (txcmod_int_pcg_registry[index].num_regis_func_per_pcg > 0)
      {
        for(clt_index = 0;clt_index < TXMOD_INT_TOTAL_CLT; clt_index++)
        {
          if((txcmod_int_pcg_registry[index].callbacks[clt_index].fptr) != NULL)
          {
            /* Call the ISR */
            (*(txcmod_int_pcg_registry[index].callbacks[clt_index].fptr))
              (txcmod_int_pcg_registry[index].callbacks[clt_index].arg);
          }
        }
      }
      else
      {
        MSG_ERROR("MOD INT, NO regisfunc in PCG %d!",index,0,0);
      }
    }
  }
  /* then check for all the slot 0 to curr_pcg_slot */
  for (index = 0; index <= curr_pcg_slot; index++)
  {
    if (mod_int_bits & (0x1 << index))
    {
      if (txcmod_int_pcg_registry[index].num_regis_func_per_pcg > 0)
      {
        for(clt_index = 0;clt_index < TXMOD_INT_TOTAL_CLT; clt_index++)
        {
          if((txcmod_int_pcg_registry[index].callbacks[clt_index].fptr) != NULL)
          {
            /* Call the ISR */
            (*(txcmod_int_pcg_registry[index].callbacks[clt_index].fptr))
              (txcmod_int_pcg_registry[index].callbacks[clt_index].arg);
          }
        }
      }
      else
      {
        MSG_ERROR("MOD INT, NO regisfunc in PCG %d!",index,0,0);
      }
    }
  }
}


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
)
{
  int pcg;

  for(pcg=0;pcg<=15;++pcg)
  {
    (void) txcmod_int_regis_for_pcgint(pcg, NULL, 0, clt_id);
  }
}
