#ifndef FTM_MC_H
#define FTM_MC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     M C   P L T   S U P P O R T 

GENERAL DESCRIPTION
  This file contains the MC extensions to support FTM.  It is used only
  for FTM and should not be used for non-FTM builds.
  
Copyright (c) 1999 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/test_mc.h#1 $
  $DateTime: 2008/04/14 04:57:43 $ 
  $Author: ppottier $
  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/06   ak      Removed the unused function prototype ftm_nv_init()
10/30/05   vm      Added new function ftm_hdrlmac_tc_assign_complete_cb()
09/23/05   jac     Fixed more PLT build conflicts.
09/23/05   jac     Fixed PLT build conflicts.
08/30/05   jac     Finished porting the PLT DO Rev A commands to FTM
08/08/05   ra      added HDR Rev A support
08/01/05   ra      include mc.h for mc_name_type
07/14/03   bmg     Merged in Cougar 3030->4040 changes
02/10/03   wd      Marge files for FTM Task, Also added p4 info.
12/06/02   ra      added ftm_tmc_init to register the diag subcommand dispatch table
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime choice.
10/19/00   bgc     Added ftm_set_srch_parms to fix warnings.
10/26/99   lcc     First cut.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "srch.h"
#include "ftm.h"
#include "mc.h"
#ifndef FEATURE_FACTORY_TESTMODE
#error FEATURE_FACTORY_TESTMODE must be defined to used this file
#endif

#include "mcc.h"
/* FTM_HAS_HDR_REV_A */ 
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FTM signals                                 */
/*                              ----------                                  */

                      
#define  FTM_MC_CMD_Q_SIG         MC_CMD_Q_SIG 
  /* This signal is set when an item is placed on any FTM_MCC queue */

#define  FTM_MC_CMD_RSP_SIG       MC_CMD_SIG          
  /* This signal is set when FTM reports to DIAG */

#define  FTM_MC_CMD_VOC_SIG       MC_CMD_SIG << 1
  /* This signal is set when the vocoder goes to DFM mode */


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Local Data                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION ftm_mc_init

DESCRIPTION
  Perform task initialization pertaining to FTM only.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_mc_init
(
  void
);

/*===========================================================================

FUNCTION TEST_MCC

DESCRIPTION
  This function contains the state machine and event loop of the FTM main
  control task.  TEST_MCC receives commands from the diag task as part of
  the overall RPC mechanism and processes those commands.
  
  In the case of "high level" commands, i.e. those commands that cause a
  state transition, TEST_MCC orchestrates the state transitions sending
  commands to SRCH, RXC, and TXC, as appropriate.  A high level command
  will have a target state associated with it.  TEST_MCC will continue
  through the state machine until the target state is reached at which
  point it will wait for further commands.
  
  In the case of "low level" commands, i.e. those commands that do not
  cause a state transition, TEST_MCC will repackage the command into the
  appropriate command structure and issue the command to the appropriate
  task, SRCH, RXC, or TXC.  The bulk of the FTM low level commands are
  handled by the SRCH task.
  
  TEST_MCC will wait for a report from the appropriate task to determine
  when a command has completed and what it's status was and send a
  status report back to the diag task.  The diag task will then return
  from the RPC function.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

mcc_exit_type *test_mcc
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
);

/*===========================================================================

FUNCTION FTM_SET_SRCH_PARMS

DESCRIPTION
  This function set the default parameters to be sent to searcher.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ftm_set_srch_parms
(
  srch_parm_cmd_type *srch_parms
);

/*===========================================================================

FUNCTION FTM_SYSTEM_DETERMINATION

DESCRIPTION
  This is a special simplified version of system determination used only in
  FTM (hardcoded conditions used).

DEPENDENCIES
  None.

RETURN VALUE
  The action code causing the exit.

SIDE EFFECTS
  None.

===========================================================================*/

mc_name_type ftm_system_determination
(
  mc_name_type entry_action     /* Initial action to take */
);

/*===========================================================================

FUNCTION ftm_tmc_init

DESCRIPTION
  ftm initialization that has to be done in tmc task.

DEPENDENCIES
  TMC task has already started.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tmc_init(void);

/*===========================================================================

FUNCTION TEST_HDRLMAC_TC_ASSIGN_COMPLETE_CB

DESCRIPTION
  Callback function passed to HDR TC assignment command.

DEPENDENCIES

RETURN VALUE
  Will be called when HDR traffic channel is assigned.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_hdrlmac_tc_assign_complete_cb ( void );

#endif /* FTM_MC_H */
