/*===========================================================================

                  PHYSICAL LAYER TESTER COMMAND PROCESSING
DESCRIPTION

  This file contains functions pertaining to FTM command processing.
  
    Copyright (c) 1999--2002
                  by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_cmd.c#1 $
  $DateTime: 2008/04/14 04:57:43 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/07   aak     Featurize the FTM Hard Handoff Feature (FTM_HAS_SRCH_HHO)
12/01/06   aak     Added support for FTM hard handoff feature
10/30/05   vm      Added new FTM APIs for DO RevA.
                   Modified the existing FTM Rev0 APIs
09/20/05   jac     Modified existing HDR FTM commands and added new ones to 
                   support rev A testing.
07/06/04   bmg     Converted release A test_* types to ftm_* types
11/11/03   bmg     Cougar Release A merge
09/23/03   wd      Added new commands for simultaneous FCH and DCCH (Release A)
09/05/03   bmg     Added FTM HDR commands
07/23/03   bmg     Converted test_* names to ftm_* names
07/09/03   bmg     Merged in Couger 3030->4040 changes:
05/08/03   wd      Added common fucntion to send command to MC and wait for signal.
05/07/03   wd      Changed signal waits so they will work with FTM task.
02/10/03   wd      Marge files for FTM Task, Also added p4 info and clean up.
01/24/03   wd      Remove obsolete functions (per ra changes).
02/21/02   rjr     Support RF commands in FTM mode.
08/22/01   bn      Support DCCH in FTM mode
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime choice.
11/16/00   bgc     Changed ftm_cmd_status to test_cmd_status.
10/13/00   bgc     Added loopback commands and deassign channels.
08/22/00   bgc     Initial revision.

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "customer.h"
#include "target.h"
#include "mc.h"
#include "mci.h"
#include "queue.h"
#include "rex.h"
#include "task.h"
#include "err.h"
#include "nv.h"
#include "diag.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm.h"
#include "ftm_cmdi.h"
#include "test_mc.h"
#include "memory.h"

#ifdef FTM_HAS_HDR_REV_A
#error code not present
#endif
/*===========================================================================
                              LOCAL DATA
===========================================================================*/

#ifdef FEATURE_FTM_MC
struct {
  word param_length;
  ftm_cmd_status_type (*function)(ftm_packed_param_type *);
} ftm_dispatch_table[FTM_MAX_CMD - FTM_FIRST_CMD] = {
    // FTM_ACQUIRE_SC = 100,
  {sizeof(ftm_cmd_acq_pilot_param_type), ftm_acquire},
    // FTM_SYNC_SC =    101,
  {0, ftm_sync},
    // FTM_FWD_FCH_SC = 102,
  {sizeof(ftm_cmd_demod_fch_param_type), ftm_assign_fwd_fch},
    // FTM_FWD_SCH_SC = 103,
  {sizeof(ftm_cmd_demod_sch_param_type), ftm_assign_fwd_sch},
    // FTM_REV_FCH_SC = 104,
  {sizeof(ftm_cmd_mod_fch_param_type), ftm_assign_rev_fch},
    // FTM_REV_SCH_SC = 105,
  {sizeof(ftm_cmd_mod_sch_param_type), ftm_assign_rev_sch},
    // FTM_FCH_LOOP_SC = 106,
  {0, ftm_loopback_fch},
    // FTM_SCH_LOOP_SC = 107,
  {0, ftm_loopback_sch},
    // FTM_RELEASE_SC = 108,
  {0, ftm_deassign_traffic},
    // FTM_SET_MODE_SC = 109,
  {1, ftm_set_mode},
    // FTM_FWD_DCCH_SC = 110,
  {sizeof(ftm_cmd_demod_dcch_param_type), ftm_assign_fwd_dcch},
    // FTM_REV_DCCH_SC = 111,
  {sizeof(ftm_cmd_mod_dcch_param_type), ftm_assign_rev_dcch},
    // FTM_DCCH_LOOP_SC = 112,
  {0, ftm_loopback_dcch},
  // 113 Reserved
  {0, 0},
  // 114 Reserved
  {0, 0},
  // 115 Reserved
  {0, 0},
  // 116 Reserved
  {0, 0},
  // 117 Reserved
  {0, 0},
  // 118 Reserved
  {0, 0},
#ifdef FTM_HAS_SRCH_HHO
  // 119 Reserved
  {0, 0},
  // 120 Reserved
  {0, 0},
  // 121 Reserved
  {0, 0},
  // 122 Reserved
  {0, 0},
  // 123 Reserved
  {0, 0},
  // 124 Reserved
  {0, 0},
  // 125 Reserved
  {0, 0},
  // 126 Reserved
  {0, 0},
  // 127 Reserved
  {0, 0},
  // FTM_FWD_HHO_SC = 128,
  {sizeof(ftm_cmd_fwd_hho_param_type), ftm_fwd_hho},
#endif
};
#endif /* FEATURE_FTM_MC */

rex_sigs_type diag_wait ( rex_sigs_type mask );

#ifdef FEATURE_FTM_TASK
rex_sigs_type ftm_sig_rex_wait ( rex_sigs_type mask );
#endif

LOCAL void ftm_send_cmd_and_wait ( ftm_cmd_type  *ftm_cmd_ptr );


/*===========================================================================

                              UTILITY FUNCTIONS

===========================================================================*/

/* Macro to kick the watchdog timer and set a timer to wake it again.      */

#define KICK_WATCHDOG() \
   dog_report(DOG_MC_RPT); \
   (void) rex_set_timer(&mc_rpt_timer, DOG_MC_RPT_TIME)


/*===========================================================================

FUNCTION      FTM_MC_CMD    

DESCRIPTION   This function will place a command structure
              on the PMC command queue for execution
DEPENDENCIES   

RETURN VALUE  TRUE if there was a buffer available and the
              command type is in range.  FALSE otherwise.

SIDE EFFECTS  A command will be placed on the PMC command queue
      
===========================================================================*/
void ftm_mc_cmd
(
   ftm_cmd_type *cmd_ptr
   /* pointer to a test command packet.  When the command has finished,
   ** the command structure is put on the "done queue". If there is no
   ** done queue", or this operation is not desirable, set the "done queue"
   ** pointer to NULL.  Also when the command has completed, the specified
   ** task is signaled with the sig parameter.  If no signaling is required,
   ** the task pointer parameter can be set to NULL.
   */
)
{
  (void) q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );      /* Initialize link field */

  q_put( &ftm_mc_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );       /* Put on command queue */

  (void) rex_set_sigs( &mc_tcb, FTM_MC_CMD_Q_SIG ); /* Signal a queue event */

} /* End ftm_mc_cmd */

/*===========================================================================

FUNCTION FTM_MC_GET_CMD

DESCRIPTION    
  This function gets a command, if any, from ftm_cmq_q for FTM_MCC .

DEPENDENCIES   ftm_mc_cmd_q

RETURN VALUE   Pointer to the command.

SIDE EFFECTS   ftm_mc_cmd_q

===========================================================================*/

ftm_cmd_type  *ftm_mc_get_cmd ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs ( &mc_tcb, FTM_MC_CMD_Q_SIG );
  /* Clear the new-command signal */

  return( (ftm_cmd_type  *) q_get ( &ftm_mc_cmd_q ) );
  /* Get the command and return it */

} /* ftm_mc_get_cmd */

/*===========================================================================

FUNCTION FTM_POST_CMD

DESCRIPTION    This function post-processes a command.  It reports
      errors to the diagnostic monitor.  It handles returning the
      command to a queue and/or signalling a task as specified in
      the command itself.  It maintains a count for debug purposes
      of how many times each return value was returned.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   A command is posted back to MC.

===========================================================================*/

void  ftm_post_cmd
(
  ftm_cmd_type  *cmd_ptr,
    /* Points to the command to perform post processing on. */

  ftm_cmd_status_type  status
    /* Status to post for command. */
)
{
  rex_sigs_type sigs;
  rex_tcb_type *tcb_ptr;
  /* Signal and task to notify */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr->hdr.status = status;
  /* Copy the status into the command */

  tcb_ptr = cmd_ptr->hdr.cmd_hdr.task_ptr;
  sigs    = cmd_ptr->hdr.cmd_hdr.sigs;
  /* Save task pointer and signals for using after the q_put() */

  /* Check to see if the command wants to be put on a queue when finished */
  if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
  {
    q_put ( cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link );
    /* Indeed it does. Do as the command asks */
  }

  /* Check to see if some task wants to be signalled when command is done. */
  if ( tcb_ptr != NULL )
  {
    (void) rex_set_sigs ( tcb_ptr, sigs );
    /* Someone does. Notify the task requesting signaling */
  }

} /* ftm_post_cmd */



/*============================================================================

FUNCTION      FTM_COMMAND_DISPATCHER

DESCRIPTION   This function will call the appropriate subcommand
              for a given FTM command.
DEPENDENCIES   

RETURN VALUE  TRUE if there was a buffer available and the
              command type is in range.  FALSE otherwise.

SIDE EFFECTS  A command will be placed on the FTM_MCC command queue
      
============================================================================*/
ftm_cmd_status_type ftm_cmd_dispatcher
(
  byte sub_cmd_code,
  word length,
  ftm_packed_param_type *param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_status_type status;

  if (sub_cmd_code < FTM_FIRST_CMD || sub_cmd_code >= FTM_MAX_CMD)
  {
    return TEST_INVALID_CMD;
  }
  if (ftm_dispatch_table[sub_cmd_code-FTM_FIRST_CMD].function == NULL)
  {
    return TEST_INVALID_CMD;
  }
  if (ftm_dispatch_table[sub_cmd_code-FTM_FIRST_CMD].param_length != length)
  {
    return TEST_INVALID_CMD;
  }
  return ftm_dispatch_table[sub_cmd_code-FTM_FIRST_CMD].function(param);

  return status;
#else
  return TEST_SUCCESS;
#endif 
} /* ftm_cmd_dispatcher */

/*===========================================================================

FUNCTION FTM_CMD_INIT_CDMA

DESCRIPTION
  This procedure processes a FTM command for initializing CDMA hardware and
  putting mobile into a stable state waiting for further commands.  
            
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_cmd_init_cdma( void )
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
  /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
    /* No command buffers available */

    return( TEST_NO_SUCCESS );
    /* Return FTM command status */
  } 
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */

    ftm_cmd_ptr->hdr.cmd = FTM_CMD_INIT_CDMA;
    /* Set command type */

    ftm_send_cmd_and_wait (ftm_cmd_ptr);
      /* send ftm command to mc queue and wait */

    return( ftm_cmd_ptr->hdr.status );
    /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_cmd_init_cdma */

/*===========================================================================

                              EXPORTED FUNCTIONS

===========================================================================*/



/*===========================================================================

FUNCTION FTM_ACQUIRE

DESCRIPTION
  This procedure processes a FTM command for acquiring a pilot. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_acquire
(
   ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */
  ftm_cmd_acq_pilot_param_type  *param = &p_param->acq;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_PILOT_ACQ;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->pilot_acq.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_acquire */

/*===========================================================================

FUNCTION FTM_SYNC

DESCRIPTION
  This procedure processes a FTM command for demodulating SYNC Channel. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_sync
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_DEMOD_SYNC;
      /* Set command type */

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_sync */

#ifdef FTM_HAS_SRCH_HHO
/*===========================================================================

FUNCTION FTM_FWD_HHO

DESCRIPTION
  This procedure processes a FTM command for Hard handoff on Traffic. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_fwd_hho
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */
  ftm_cmd_fwd_hho_param_type  *param = &p_param->f_hho;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_FWD_HHO;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->fwd_hho.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_fwd_hho */
#endif


/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_FCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Traffic Channel. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_fwd_fch
(
   ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
  ftm_cmd_demod_fch_param_type *param = &p_param->f_fch;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_DEMOD_FCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->demod_fch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif
}  /* ftm_assign_fwd_fch */

/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_DCCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Delicated Control Channel. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_fwd_dcch
(
   ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
  ftm_cmd_demod_dcch_param_type *param = &p_param->f_dcch;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_DEMOD_FDCCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->demod_dcch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_assign_fwd_dcch */

/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_SCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Suplimental Channel. 
          
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_assign_fwd_sch
( 
   ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
  ftm_cmd_demod_sch_param_type *param = &p_param->f_sch;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_DEMOD_FSCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->demod_sch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* else */
#else
  return TEST_SUCCESS;
#endif 
} /* ftm_assign_fwd_sch */

/*===========================================================================

FUNCTION FTM_ASSIGN_FWD_FCH_DCCH

DESCRIPTION
  This procedure processes a FTM command for demodulating Fundamental Channel 
  and Delicated Control Channel. 
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_fwd_fch_dcch
(
   ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
  ftm_cmd_demod_fch_dcch_param_type *param = &p_param->f_fch_dcch;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_DEMOD_FFCH_FDCCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->demod_fch_dcch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_assign_fwd_fch_dcch */

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_FCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  fundamental channel (R-FCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_rev_fch
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_mod_fch_param_type *param = &p_param->r_fch;
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_MOD_FCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->mod_fch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* else */
#else
  return TEST_SUCCESS;
#endif 
} /* ftm_assign_rev_fch */

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_DCCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  dedicated control channel (R-DCCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_rev_dcch
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_mod_dcch_param_type *param = &p_param->r_dcch;
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_MOD_DCCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->mod_dcch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* else */
#else
  return TEST_SUCCESS;
#endif
} /* ftm_assign_rev_dcch */

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_SCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  fundamental channel (R-SCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_rev_sch
(
  ftm_packed_param_type *p_param
)
{
#ifdef  FEATURE_FTM_MC
  ftm_cmd_mod_sch_param_type *param = &p_param->r_sch;
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_MOD_SCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->mod_sch.param = *param;
    
    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* else */
#else
  return TEST_SUCCESS;
#endif 
} /* ftm_assign_rev_sch */

/*===========================================================================

FUNCTION FTM_ASSIGN_REV_FCH_DCCH

DESCRIPTION
  This procedure processes a FTM command for modulating the reverse
  fundamental channel (R-FCH) and dedicated control channel (R-DCCH).
            
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  INVALID_STATE, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_assign_rev_fch_dcch
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_mod_fch_dcch_param_type *param = &p_param->r_fch_dcch;
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_MOD_FCH_DCCH;
      /* Set command type */

    /* Set command parameters */
    ftm_cmd_ptr->mod_fch_dcch.param = *param;

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* else */
#else
  return TEST_SUCCESS;
#endif
} /* ftm_assign_rev_fch_dcch */

/*===========================================================================

FUNCTION FTM_LOOPBACK_FCH

DESCRIPTION
  This procedure processes a FTM command for turning on FCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_loopback_fch
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_FCH_LOOPBACK;
      /* Set command type */

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_loopback_fch */

/*===========================================================================

FUNCTION FTM_LOOPBACK_DCCH

DESCRIPTION
  This procedure processes a FTM command for turning on DCCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_loopback_dcch
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_DCCH_LOOPBACK;
      /* Set command type */

    /* Send ftm command to mc queue and wait */
    ftm_send_cmd_and_wait (ftm_cmd_ptr);

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_loopback_dcch */

/*===========================================================================

FUNCTION FTM_LOOPBACK_SCH

DESCRIPTION
  This procedure processes a FTM command for turning on SCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_loopback_sch
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_SCH_LOOPBACK;
      /* Set command type */

    ftm_send_cmd_and_wait (ftm_cmd_ptr);
      /* send ftm command to mc queue and wait */

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_loopback_sch */

/*===========================================================================

FUNCTION FTM_DEASSIGN_TRAFFIC

DESCRIPTION
  This procedure processes a FTM command for turning on SCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_deassign_traffic
(
  ftm_packed_param_type *p_param
)
{
#ifdef FEATURE_FTM_MC
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to a FTM command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a command buffer */

  if ( (ftm_cmd_ptr = (ftm_cmd_type *) q_get( &ftm_mc_cmd_free_q )) == NULL )
  {
    ERR_FATAL ( "No buffers on ftm_mc_cmd_free_q", 0, 0, 0 );
      /* No command buffers available */
  
    return ( TEST_NO_SUCCESS );
      /* Return FTM command status */
  }
  else  /* A command buffer is available */
  {
    /* Pack and send a FTM command */
    
    ftm_cmd_ptr->hdr.cmd = FTM_CMD_RELEASE;
      /* Set command type */

    ftm_send_cmd_and_wait (ftm_cmd_ptr);
      /* send ftm command to mc queue and wait */

    return ( ftm_cmd_ptr->hdr.status );
      /* Return FTM command status */

  } /* A command buffer is available */
#else
  return TEST_SUCCESS;
#endif 
}  /* ftm_deassign_traffic */

/*===========================================================================

FUNCTION FTM_SET_MODE

DESCRIPTION
  This procedure processes a FTM command for turning on SCH loopback.
          
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_set_mode
(
  ftm_packed_param_type *p_param
)
{
  nv_cmd_type  ftm_nv_cmd_buf;   /* Command buffer to NV */
  ftm_mode_type mode = p_param->mode; /* New Mode */

  /* Check mode for valid values */
  switch(mode)
  {
    case DMSS_MODE:
    case FTM_MODE:
      /* mode is good */
      break;
    default:
      /* Unknown mode requested */
      return TEST_INVALID_CMD;
  }
  /* Prepare command buffer to NV. */

  ftm_nv_cmd_buf.cmd = NV_WRITE_F;               /* Read request         */
  ftm_nv_cmd_buf.tcb_ptr = rex_self();           /* Notify back to me    */
  ftm_nv_cmd_buf.sigs = DIAG_NV_CMD_SIG;           /* With this signal     */
  ftm_nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
  ftm_nv_cmd_buf.item = NV_FTM_MODE_I;           /* Item to get          */
  ftm_nv_cmd_buf.data_ptr = (nv_item_type *)&mode; /* Where to return it   */

  /* Clear signal, issue the command, and wait for the respone. */

  (void) rex_clr_sigs(rex_self(), DIAG_NV_CMD_SIG); /* Clear signal for NV  */
  nv_cmd(&ftm_nv_cmd_buf);                       /* Issue the request    */

  #ifdef FEATURE_FTM_TASK
    (void) ftm_sig_rex_wait( DIAG_NV_CMD_SIG );
      /* Wait the FTM command response signal to be set */
  #else
    (void) diag_wait( DIAG_NV_CMD_SIG );
      /* Wait the FTM command response signal to be set */
  #endif
  
  (void) rex_clr_sigs(rex_self(), DIAG_NV_CMD_SIG); /* Clear signal for NV  */

  /* Check and return status. */

  if (ftm_nv_cmd_buf.status != NV_DONE_S) {
    return TEST_NO_SUCCESS;
  }
  return TEST_SUCCESS;
}

#ifdef FEATURE_FTM_MC
/*===========================================================================

FUNCTION FTM_SEND_CMD_AND_WAIT

DESCRIPTION
  This procedure sends commands to mc queue and wait for mc to finish
  processing command.
          
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ftm_send_cmd_and_wait
(
  ftm_cmd_type  *ftm_cmd_ptr
)
{
  ftm_cmd_ptr->hdr.cmd_hdr.task_ptr = rex_self();        /* Reply to task */  
  ftm_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &ftm_mc_cmd_free_q; /* Free queue */

#ifdef FEATURE_FTM_TASK
  ftm_cmd_ptr->hdr.cmd_hdr.sigs = FTM_CMD_RSP_SIG;       /* Signal back */
    /* Set up header */

  (void) rex_clr_sigs( rex_self(), FTM_CMD_RSP_SIG );
    /* Clear the FTM command response signal */

  ftm_mc_cmd( ftm_cmd_ptr );
    /* Send the FTM command to FTM_MCC */

  (void) ftm_sig_rex_wait( FTM_CMD_RSP_SIG );
    /* Wait the FTM command response signal to be set */

  (void) rex_clr_sigs( rex_self(), FTM_CMD_RSP_SIG );
    /* Clear the FTM command response signal */
#else
  ftm_cmd_ptr->hdr.cmd_hdr.sigs = FTM_MC_CMD_RSP_SIG;     /* Signal back */
    /* Set up header */

  (void) rex_clr_sigs( rex_self(), FTM_MC_CMD_RSP_SIG );
    /* Clear the FTM command response signal */

  ftm_mc_cmd( ftm_cmd_ptr );
    /* Send the FTM command to FTM_MCC */

  (void) diag_wait( FTM_MC_CMD_RSP_SIG );
    /* Wait the FTM command response signal to be set */

  (void) rex_clr_sigs( rex_self(), FTM_MC_CMD_RSP_SIG );
    /* Clear the FTM command response signal */
#endif
}
#endif /* FEATURE_FTM_MC */

#ifdef FEATURE_FTM_MC
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
#endif /* FEATURE_FTM_MC */

#endif /* FEATURE_FACTORY_TESTMODE */
