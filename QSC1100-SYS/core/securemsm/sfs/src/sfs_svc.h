#ifndef SFS_SVC_H
#define SFS_SVC_H

#ifdef FEATURE_SEC_USFS

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   SFS T A S K
                             
GENERAL DESCRIPTION
  This is the task level file for the secure file system service task

EXTERNALIZED FUNCTIONS
  sfs_task() - entry point for the SFS task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  MC must initialize the task and set the start signal for this task, 
  before any other functions can be called.

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/src/sfs_svc.h#1 $ 
  $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/07    rv     changed signal for SFS_IPC_SIG (colides with L4 signal)
12/15/06    df     created

===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------
  SFS Signals
---------------------------------------------------------------------------*/
/* The next 3 sigs are never used explicitly in the security services code.  
 * They are here as place-holders, so that when adding signals, the user does 
 * not re-defined signals already defined system-wide in task.h.  These 
 * signals are referred by their proper name (i.e., TASK_STOP_SIG) in this 
 * Security services code.
 */

#define SFS_START_SIG          0x8000   /* Reserved for TASK_START_SIG     */

#define SFS_STOP_SIG           0x4000   /* Reserved for TASK_STOP_SIG      */

#define SFS_OFFLINE_SIG        0x2000   /* Reserved for TASK_OFFLINE_SIG   */

/* The following sigs are all okay, and can be allocated freely */
#define SFS_DOG_RPT_TIMER_SIG  0x0001   /* signal set when watchdog expires*/

#define SFS_IPC_SIG            0x0100   /* sfs IPC signal */

#define SFS_NV_CMD_SIG         0x0004   /* signal when NV interaction done */


/*--------------------------------------------------------------------------
  SFS External Function
---------------------------------------------------------------------------*/
void sfs_task( dword task_ack_func_addr );

#ifdef __cplusplus
}
#endif
#endif  /* FEATURE_SEC_USFS */
#endif  /* SFS_SVC */
