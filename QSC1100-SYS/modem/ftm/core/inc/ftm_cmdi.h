#ifndef __FTM_CMDI_H
#define __FTM_CMDI_H

/*===========================================================================

                  FACTORY TEST INTERNAL HEADER
DESCRIPTION

  This file contains prototypes, #defines, and enums used internally by FTM.
  
    Copyright (c) 1999--2007, 2009
                  by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_cmdi.h#2 $
  $DateTime: 2009/01/28 13:21:28 $ 
  $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   lp      Move FTM_EXIT state to ftm_state_enum_type to resolve
                   Critical Lint error
01/10/07   aak     Featurize the FTM Hard handoff feature (FTM_HAS_SRCH_HHO) 
12/01/06   aak     Added support for FTM hard handoff feature
10/30/05   vm      Added support for new FTM DO Reva APIs
08/30/05   jac     Finished the DO Rev A changes
07/19/05   ra      Added more DO REV A support
05/03/05   ra      Added DO REV A support
10/20/04   ra      Removed ftm_unjump. This became obsolete when FEATURE_SRCH_INACTIVE_STATE
                   was enabled
11/11/03   bmg     Cougar Release A merge
09/23/03   wd      Added new types and defs for simultaneous FCH and DCCH (Release A)
09/05/03   bmg     Added FTM HDR commands
07/23/03   bmg     Converted test_* names to ftm_* names
07/14/03   bmg     Cougar 3030->4040 port
02/10/03   wd      Marge files for FTM Task, Also added p4 info and clean up.
01/24/03   wd      Decouple legacy FTM from FTM_HWTC and FTM_RF
02/21/02   rjr     Support for RF commands
08/22/01   bn      Support DCCH in FTM mode
11/15/00   bgc     ftm_cmd_status has been changed to test_cmd_status.
08/22/00   bgc     Initial revision.

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "cmd.h"
#include "qw.h"
#include "ftm_cmd.h"


/*===========================================================================

                 REGIONAL DEFINITIONS AND DECLARATIONS

This section contains regional definitions for constants, macros, types,
variables and other items needed by FTM.

===========================================================================*/
/*--------------------------------------------------------------------------
                            FTM COMMAND Conversions
--------------------------------------------------------------------------*/
/* convert FTM command interface RC numbering to Modulator RC numbering */
#define FTM_RC_TO_MOD_RC(x) \
   ((enc_fch_radio_config_type)( x ))

/* convert the reverse link RC specifier to a rateset */
#define REV_RC_TO_RS(x) \
   ((enc_rate_set_type)((x-1)%2))


/*--------------------------------------------------------------------------
                            NUM_SCCH_PER_FINGER
--------------------------------------------------------------------------*/
#ifdef T_MSM5000
#define NUM_SCCH_PER_FINGER 7
#else
#error code not present
#endif /* T_MSM5000 */



/*--------------------------------------------------------------------------
                            FTM COMMAND ENUM TYPE
--------------------------------------------------------------------------*/
/* these are the command codes that are sent from the diag task to the
   ftm_mcc task as part of the overall command chain for executing FTM 
   user commands */
   
typedef enum 
{
  FTM_CMD_INIT_CDMA,
  FTM_CMD_PILOT_ACQ,      
  FTM_CMD_DEMOD_SYNC, 
  FTM_CMD_MOD_FCH, 
  FTM_CMD_MOD_DCCH,
  FTM_CMD_MOD_SCH,
  FTM_CMD_DEMOD_FCH,
  FTM_CMD_DEMOD_FDCCH,
  FTM_CMD_DEMOD_FSCH, 
  FTM_CMD_FCH_LOOPBACK, 
  FTM_CMD_DCCH_LOOPBACK,
  FTM_CMD_SCH_LOOPBACK,
  FTM_CMD_RELEASE,
#ifdef FTM_HAS_SRCH_HHO
    FTM_CMD_FWD_HHO,
#endif
#ifdef FEATURE_HDR
#error code not present
#else
  FTM_CMD_RESERVED1,
  FTM_CMD_RESERVED2,
  FTM_CMD_RESERVED3,
  FTM_CMD_RESERVED4,
  FTM_CMD_RESERVED5,
  FTM_CMD_RESERVED6,
#ifndef FTM_HAS_HDR_REV_A
  FTM_CMD_RESERVED7,
  FTM_CMD_RESERVED8,
  FTM_CMD_RESERVED9,
  FTM_CMD_RESERVED10,
  FTM_CMD_RESERVED11,
  FTM_CMD_RESERVED12,
  FTM_CMD_RESERVED13,
#endif
#endif
  FTM_CMD_MOD_FCH_DCCH, 
  FTM_CMD_DEMOD_FFCH_FDCCH, 
  FTM_CMD_NUM_CMDS    /* must be the last member */
}
ftm_cmd_enum_type;

/*--------------------------------------------------------------------------
                            FTM STATE ENUM TYPE
--------------------------------------------------------------------------*/

typedef enum
{
  FTM_ENTER,                  /* FTM entry state */
  FTM_INIT_CDMA,              /* FTM CDMA initialization */
  FTM_PIL_ACQ,                /* FTM pilot channel acquisition */
  FTM_SYNC,                   /* FTM sync channel processing, combines
                                 CDMA_SYNC_ACQ, CDMA_SYNC_MSG */
  FTM_IDLE,                   /* FTM idle state, we have acquired system
                                 time and are demodulating the paging 
                                 channel. */
  FTM_TRAFFIC,                /* FTM traffic state */
  FTM_SYS_ACC,                /* FTM System Access state */
  FTM_REV_TRAFFIC,            /* FTM reverse traffic state */
  FTM_EXIT,                   /* FTM exit state */
  FTM_MAX_STATE
} ftm_state_enum_type;

#ifdef FEATURE_HDR
#error code not present
#endif

/*--------------------------------------------------------------------------
                        COMMAND HEADER STRUCT TYPE
--------------------------------------------------------------------------*/

typedef struct
{
  cmd_hdr_type         cmd_hdr;
    /* A header for queue control */
  ftm_cmd_enum_type    cmd;            
    /* Which command to run */
  ftm_cmd_status_type  status; 
    /* The return status of the command */
}
ftm_cmd_hdr_type;


/*--------------------------------------------------------------------------
                          FTM COMMAND STRUCT TYPES
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                          PERFORM CDMA INITIALIZATION

This command instructs the FTM to initialize CDMA hardware and go to a
stable state for further commands.  
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
ftm_init_cdma_cmd_type;


/*--------------------------------------------------------------------------
                          PERFORM PILOT ACQUISITION

This command instructs the FTM to perform pilot acquisition.  FTM will
continue to attempt to acquire a pilot until it does so successfully or
until new behavior is commanded.  There are no timeouts.
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_acq_pilot_param_type param;
}
ftm_pilot_acq_cmd_type;


/*--------------------------------------------------------------------------
                         DEMODULATE SYNC CHANNEL
                         
This command instructs FTM to demodulate the sync channel                         
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
ftm_demod_sync_cmd_type;

/*--------------------------------------------------------------------------
                         DEMODULATE TRAFFIC CHANNEL
                         
This command instructs FTM to demodulate a 95A/B traffic channel
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_demod_fch_param_type param;
}
ftm_demod_fch_cmd_type;

/* DEMODULATE DEDICATED CONTROL CHANNEL */
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_demod_dcch_param_type param;
} ftm_demod_dcch_cmd_type;

/*
** Reverse link commands
*/

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_fch_param_type param;
}
ftm_mod_fch_cmd_type;

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_dcch_param_type param;
} ftm_mod_dcch_cmd_type;

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_sch_param_type param;
}
ftm_mod_sch_cmd_type;
/*--------------------------------------------------------------------------
                         DEMODULATE SUPPLEMENTAL CHANNEL (IS95C SCH)

This command instructs FTM to demodulate a 95C Supplemental Channel
--------------------------------------------------------------------------*/
typedef struct
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_demod_sch_param_type param;
}
ftm_demod_sch_cmd_type;

/*--------------------------------------------------------------------------
                         DEMODULATE SIMULTANEOUS FCH & DCCH 

This command instructs FTM to demodulate FCH & DCCH channels (Release A)
--------------------------------------------------------------------------*/
typedef struct
{
   ftm_cmd_hdr_type  hdr;
   /* Common header */

   ftm_cmd_demod_fch_dcch_param_type param;
}
ftm_demod_fch_dcch_cmd_type;

/*--------------------------------------------------------------------------
                         MODULATE SIMULTANEOUS FCH & DCCH 

This command instructs FTM to modulate FCH & DCCH channels (Release A)
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_fch_dcch_param_type param;
}
ftm_mod_fch_dcch_cmd_type;

#ifdef FTM_HAS_SRCH_HHO
/*--------------------------------------------------------------------------
                          PERFORM HARD HANDOFF

This command instructs the FTM to perform Hard handoff.  FTM will
continue to attempt to complete hard handoff until it does so successfully or
until new behavior is commanded.  There are no timeouts.
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_fwd_hho_param_type param;
}
ftm_fwd_hho_cmd_type;
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*--------------------------------------------------------------------------
                             UNION OF ALL FTM COMMANDS
--------------------------------------------------------------------------*/
typedef union
{
  ftm_cmd_hdr_type      hdr;
  ftm_init_cdma_cmd_type init_cdma_cmd;
  ftm_pilot_acq_cmd_type pilot_acq;
  ftm_demod_sync_cmd_type demod_sync;
  ftm_demod_fch_cmd_type demod_fch;
  ftm_demod_dcch_cmd_type demod_dcch;
  ftm_demod_sch_cmd_type demod_sch;
  ftm_mod_fch_cmd_type mod_fch;
  ftm_mod_dcch_cmd_type mod_dcch;
  ftm_mod_sch_cmd_type mod_sch;
  ftm_demod_fch_dcch_cmd_type demod_fch_dcch;
  ftm_mod_fch_dcch_cmd_type mod_fch_dcch;
#ifdef FTM_HAS_SRCH_HHO
  ftm_fwd_hho_cmd_type fwd_hho;
#endif

#ifdef FEATURE_HDR
#error code not present
#endif
}
ftm_cmd_type;


/*===========================================================================

                 EXTERNAL DEFINITIONS AND DECLARATIONS

This section contains external definitions for constants, macros, types,
variables and other items needed by FTM.

===========================================================================*/

/*--------------------------------------------------------------------------
                         Constant Definitions
--------------------------------------------------------------------------*/

#define  FTM_CMD_BUFS_NUM   6
  /* Number of buffers on the FTM command queue */

/*--------------------------------------------------------------------------
                          Type and Variables 
--------------------------------------------------------------------------*/

extern ftm_cmd_type      ftm_mc_cmd_bufs[FTM_CMD_BUFS_NUM];
extern q_type            ftm_mc_cmd_free_q;
extern q_type            ftm_mc_cmd_q;

extern ftm_cmd_status_type test_cmd_status;
  /* FTM command status */

extern ftm_state_enum_type ftm_fwd_curr_state;
  /* Current FTM state on the forward link */

extern ftm_state_enum_type ftm_rev_curr_state;
  /* Current FTM state on the reverse link */

//EFY HACK
//extern qword parm_oth_sys_time;
extern uint64 parm_oth_sys_time;
  /* Storage for Sync Channel message time parameter */

extern byte ftm_sframe_num;       
  /* sframe number that Sync message was received in */

extern ftm_acq_mode_type ftm_acq_mode;
  /* The acquisition mode */

extern ftm_band_class_type ftm_band_class;
  /* The band class */

extern boolean ftm_wait_for_pc_msg;
  /* Flag indicating if the user wants to wait for a PC message */

#ifdef FEATURE_HDR
#error code not present
#endif

/*--------------------------------------------------------------------------
                         Function Prototypes 
--------------------------------------------------------------------------*/

#ifdef FEATURE_FTM_MC
/*============================================================================

FUNCTION      FTM_MC_COMMAND    

DESCRIPTION   This function will place a command structure
              on the PMC command queue for execution
DEPENDENCIES   

RETURN VALUE  TRUE if there was a buffer available and the
              command type is in range.  FALSE otherwise.

SIDE EFFECTS  A command will be placed on the PMC command queue
      
============================================================================*/
extern void ftm_mc_cmd
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to searcher command. Upon queueing, the 'status'
       field of the command block is set to 'SRCH_BUSY_S'. */
); /* ftm_mc_cmd */

/*===========================================================================

FUNCTION FTM_MC_GET_CMD

DESCRIPTION    
  This function gets a command, if any, from ftm_cmq_q for FTM_MCC .

DEPENDENCIES   ftm_mc_cmd_q

RETURN VALUE   Pointer to the command.

SIDE EFFECTS   ftm_mc_cmd_q

===========================================================================*/

extern ftm_cmd_type  *ftm_mc_get_cmd ( void ); /* ftm_mc_get_cmd */


/*===========================================================================

FUNCTION FTM_MC_POST_CMD

DESCRIPTION    This function post-processes a command.  It reports
      errors to the diagnostic monitor.  It handles returning the
      command to a queue and/or signalling a task as specified in
      the command itself.  It maintains a count for debug purposes
      of how many times each return value was returned.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   A command is posted back to MC.

===========================================================================*/

extern void ftm_post_cmd
(
  ftm_cmd_type  *cmd_ptr,
    /* Points to the command to perform post processing on. */

  ftm_cmd_status_type  status
    /* Status to post for command. */
); /* ftm_post_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION FTM_JUMP

DESCRIPTION
  This function controls the "jump to hyperspace".

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ftm_jump
(   
  void
); /* ftm_jump */


/*===========================================================================

FUNCTION FTM_FWD_FCH

DESCRIPTION
  This function demodulates Forward Fundamental Channel.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

extern void ftm_fwd_fch 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
); /* ftm_fwd_fch */

/*===========================================================================

FUNCTION FTM_FWD_DCCH

DESCRIPTION
  This function demodulates Forward Dedicated Control Channel.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

extern void ftm_fwd_dcch 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
); /* ftm_fwd_dcch */

/*===========================================================================

FUNCTION FTM_FWD_FCH_DCCH

DESCRIPTION
  This function demodulates simultaneously the Forward Fundamental Channel
  and the Forward Dedicated Control Channel.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void ftm_fwd_fch_dcch 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
);

/*===========================================================================

FUNCTION FTM_REV_TC_INIT

DESCRIPTION
  This function modulates Reverse Fundamental Channel.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void ftm_rev_tc_init 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
);

/*===========================================================================

FUNCTION FTM_REV_DCCH_INIT

DESCRIPTION
  This function modulates Reverse DCCH
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void ftm_rev_dcch_init 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
);

/*===========================================================================

FUNCTION FTM_REV_FCH_DCCH_INIT

DESCRIPTION
  This function modulates Reverse FCH and DCCH
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void ftm_rev_fch_dcch_init 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
);

/*===========================================================================

FUNCTION FTM_TC_ADD_F_SCH

DESCRIPTION
  This function sends message to RXC & SRCH to demodulate/decode SCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

ftm_cmd_status_type ftm_tc_add_f_sch
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#endif /* FEATURE_FTM_MC */
#endif /* __FTM_CMDI_H */

