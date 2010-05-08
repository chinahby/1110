/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     T E S T   M C   F O R   F T M

GENERAL DESCRIPTION
  This file contains the MC extensions to support Factory Test Mode (FTM).  It
  replaces the normal MC It is used only for FTM and should not be used for
  non-FTM builds.

Copyright (c) 1999 2000 2001 by QUALCOMM Incorporated. All Rights Reserved.
Copyright (c) 2002 2003 2004 by QUALCOMM Incorporated. All Rights Reserved.
Copyright (c) 2005 2006 2007 by QUALCOMM Incorporated. All Rights Reserved.
Copyright (c) 2008 2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/test_mc.c#2 $
  $DateTime: 2009/03/19 22:40:53 $ 
  $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/08   sr      Forced initialization of member pilot_rec_type
08/28/07   aak     Fix for Enabling Reverse Power Control during NS traffic and HHO 
05/01/07   grl     Removed parameter to hdrsrch_chan_and_aset_assign.
04/13/07   kss     Use HDR macros rather than direct DSP writes.
02/26/07   vm      Merge for Common archive
01/19/07   sar     Added call to rf_init_rf_nv_items() in test_mcc();
01/10/07   aak     Featurize the FTM Hard Handoff feature (FTM_HAS_SRCH_HHO)
03/09/07   et      merged changes from adk to fix WFQ tests- part of which
                   includes reading the ESN directly from this file rather
                   than using the passed in version.  The passed in version
                   comes in as 0 when in FTM mode (by design) which causes WFQ
                   to fail.
12/12/06   sst     Added band and chan info in slew message
12/01/06   aak      Added support for FTM hard handoff feature
12/07/06   bmg     Removed sync request when already in sync state.
11/22/06   vm      Added the support for Firmware TX AGC
11/06/06   ak      Updated functions ftm_cdma_enter_init() and test_mcc()
                   to fix the waveform quality test.
10/20/06   alb     Replaced ftm_nv_read_item to be available as a utility per
                   Anil K.
10/20/06   alb     Removed the reading of ESN from the FTM init function.  
                   This fix for 91611 has been moved to oemcm.c.
09/01/06   ak      Modifications needed since the global variable
                   hdrfmac_ack_mode has been removed from HDR.
08/18/06   ak      Minor changes plus a correction in hdr_ftmi_mod_rtc_reva().
08/17/06   ak      Removed the unused function ftm_nv_init().
                   Added a new function, ftm_nv_read_item(), to read an NV item
                   (e.g., ESN) directly from the NV. This fixed the FCH FER
                   problem in case of non-zero ESN (which was being set to 0).
                   Modified the functions hdr_ftmi_mod_rtc_reva(),
                   hdr_ftmi_mod_rtc(), and hdr_ftmi_deassign() to implement
                   mobile ACK, NAK, or not ACK every slot in the FTM
                   nonsignaling HDR mode.
06/15/06   ak      Modified the function hdr_ftmi_deassign(), so that
                   successive deassign commands do not lock the FTM.
06/14/06   ak/ra   Fixed RPC logic for CDMA2000 
05/23/06   vm      Fixed the reacquisition for NS HDR if signal is not connected
                   Added the support for enhanced access channel in NS HDR mode
12/07/05   vm      Fixed the unstable working of new HDR non signaling 
                   commands and FTM state machine issue
12/05/05   vm      Fixed the issue of FTM to online transition after
                   doing HDR non-signaling
11/14/05   vm      Fixed CDMA2K non signaling issues
11/07/05   vm      Added more support for DO RevA
10/30/05   vm      Added new APIs for supporting DO RevA.
                   Modified the existing FTM APIs
10/04/05   kss     Use macro for reading HDR max rri log.
09/26/05   vm	   Fixed FTM non signalling DO Rev0
09/23/05   jac     Fixed PLT build conflicts.
09/20/05   lp      Added T_MSM7500 define for hdrlmac_tc_assign
09/10/05   grl     Added tc_mode parameber to hdrlmac_tc_assign call.
09/08/05   ra      Wrapped ftm_mc_hdr_tx_done with appropriate feature check.
                   Interim DO REV A changes.
08/30/05   jac     Finished porting the PLT DO Rev A commands to FTM
08/08/05   ra      Continue to add DO REV A functions
08/01/05   ra      mc.h goes in test_mc.h
08/01/05   ra      include mc.h for mc_name_type
07/19/05   ra      Added more DO REV A support
07/11/05   dna     Add support for Rel. A TCA message field (DSC channel gain)
06/01/05   ra      Remove call to sam_unlock in hdr_ftmi_pil_acq. Removed sam.h
05/18/05   ejv     Remove srchi.h from include path.  Replace srch.h with srchmc.h.
05/03/05   ra      Added DO REV A support
03/21/05   sq      Removed improper featurization in HDR FTM
03/20/05   bmg     Switched to using rfm_tune_to_chan()
01/25/05   ra      DORA merge.
                   Removed tune to channel call in ftm_mc_sync. Re-tuning
                   while handling a synch message is not desired. Actual call
                   boxes will not send a retune during synch messaging. 
01/25/05   etv     Fixed the wrong use of enum from HDR_FTM_SYNC to 
                   HDR_FTM_SYNC_STATE
11/17/04   ra      Changed FTM preprocessor check at the top of the file
11/05/04   ra      Removed obsolete call to update UI when executing ftm_system_determination
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/20/04   ra      Removed call to ftm_unjump. This became obsolete when FEATURE_SRCH_INACTIVE_STATE
                   was enabled
08/17/04   bmg     Moved RF NV loading into test_mc where it can be called
                   in the correct task context.
06/01/04   fpe     Completed support for FEATURE_ENHANCED_BAND_CLASS
05/27/04   ra      Added support for FEATURE_ENHANCED_BAND_CLASS
05/18/04   rsr 	   Added changes under FEATURE_SRCH_INACTIVE_STATE. To be resolved
04/23/04   ra      Modified ftm_tc_init to turn off dcch report flag
04/12/04   ra      Modified ftm_init_cdma to put the searcher state machine in the 
                   correct state before acquiring.
03/10/04   ra      FTM_BAND_CLASS configured from CAI_BAND_CELL
12/30/03   sq      Added support of HDR receive diversity for FTM
12/10/03   bmg     DRC gating fix
12/05/03   ra      Updated FTM_BAND_CLASS definition to support correct BAND_CLASS
11/19/03   bmg     Updated to additional call parameter to hdrsrch_acquire()
11/11/03   bmg     Cougar Release A merges
  09/23/03 ka      Fixed featurization for Rel. A
  09/23/03 ka      Fixed bypass_lc behavior for simultaneous FCH and DCCH
  09/23/03 wd      Added new commands for simultaneous FCH and DCCH (Release A)
  07/24/03 bkm     Fixed P2 featurization wrt fch_info.active variable.
10/21/03   bmg     Fixed traffic on arbitrary system PN support
10/15/03   bmg     Merge to Cougar 5.0 release:
  07/21/03 wd      Fixed problem with FPC and Release A build.
10/08/03   bmg     HDR forward traffic command will use correct MAC index
09/17/03   bmg     Fixed HDR forward traffic command.
09/05/03   bmg     Force searcher into start state when entering CDMA mode.
                   Cleaned up HDR prototypes.
07/25/03   bkm     Fixed P2 featurization wrt fch_info.active variable.
07/23/03   bmg     Added calling support for multi-chain RF driver
                   Converted test_* names to ftm_* names
07/18/03   bmg     Imported cougar fixes
  07/15/03 wd      Added signaling code for FTM task to wait for MC to be 
                   FTM ready before executing FTM commands.
07/14/03   bmg     Merged in Cougar 3030->4040 changes
05/05/03   wd      Removed the use of cdma_active_set variable.
02/15/03   wd      Added ftm_wait function, moved from ftm.c since this ftm_wait
                   needs to work in mc_task space; ftm.c wait works for ftm_task.
02/15/03   wd      Removed FEATURE_FTM_MC in ftm_system_determination to be 
                   able to get in FTM happy mode.
02/10/03   wd      Marge files for FTM Task, Also added p4 info.
01/24/03   wd      Decouple FTM_RF (per ra changes)
12/12/02   ra      Added support for JCDMA in ftm_cdma_enter_init
10/04/02   sj      Added support for RF ftm.
09/16/02   sj      Featurized sending the handset command.
06/21/02   sj      Added support for multimode architecture.
05/28/02   ro      Use FEATURE_IS2000_R_SCH and FEATURE_IS2000_F_SCH.
02/22/02   fc      Changes to use new interfaces for support Release A common
                   channels.
02/21/02   rjr	   Support RF commands in FTM mode.
01/25/02   cml     Featureized for no SCH.
11/27/01   hrk     Changes for new definition of mctrans_fpc_olpc_param_type.
09/20/01   nxb     Fixed warning messages by wrapping with FEATURE_COMPLETE_SNM
08/22/01   bn      Support DCCH in FTM mode
07/06/01   ph      Added support for Callback Timers
06/11/01   fc      Changes to fix offline issue (SD20 only).
06/01/01   ejv     Added checking/setting of band_class in ACQ section.
04/10/01   fc      Added support for system determination 2.0.
04/10/01   lcc     Changes to use new interfaces for supporting P2 mode.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime choice.
01/16/01   bgc     Fixed dependencies with SRCH.
01/15/01   bgc     Removed unused code at end of test_mcc().
12/12/00   bgc     Removed use of hsig.h since the UI task turns off the call
                   led during init.
11/16/00   bgc     Added UI task offline and changed plt_cmd_status to
                   test_cmd_status.
11/07/00   bgc     #ifndef'd code that is obsolete for FEATURE_COMPLETE_SNM.
10/13/00   bgc     Added use of HS task for LCD display.
10/04/00   jq      Added FTM FCH and SCH loopback feature
08/22/00   bgc     Initial file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_sys_ctl.h"
#include "rfnv.h"
#include "caii.h"
#include "diag.h"
#include "db.h"
#include "err.h"
#include "hw.h"
#include "log.h"
#include "mar.h"
#include "mcc.h"
#include "mccdma.h"
#include "mccreg.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mcctcho.h"
#include "mcctci.h"
#include "mci.h"
#include "bio.h"
#ifdef FTM_HAS_SRCH_HHO
#include "srch_lib_ext.h"
#endif
#ifndef FEATURE_SD20
#include "mcsys.h"
#include "mcsyspr.h"
#endif /* !FEATURE_SD20 */
#include "parm.h"
#include "test_mc.h"
#include "queue.h"
#include "qw.h"
#include "ran.h"
#include "rex.h"
#include "rf.h"
#include "snm.h"
#include "srchmc.h"
#include "srv.h"
#include "ts.h"

#include "txc.h"
#include "enc.h"
#include "enci.h"

#include "msm_drv.h"
#include "hs.h"
#include "nv.h"
#include "ftm_task.h"

#include "ftm_1x_ctl.h"
#include "rficap.h"

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_MULTIMODE_ARCH
#include "mmoc.h"
#endif /* FEATURE_MULTIMODE_ARCH */
#include "ftm_1x_dispatch.h"
#include "ftm_cmdi.h"

#if !defined (FTM_HAS_HDR_REV_A)
extern void hdrsrchtc_drc_param_ctrl( void );
extern void hdrsrchtc_init_connected_state( void );
#endif

void ftm_sys_acc(void);
extern void mccidl_ftm_init(void);
extern void rf_init_rf_nv_items( void );
void mccsa_set_access_in_progress(void);
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* Acquire any SID+NID pair */
#define FTM_BLOCK_OR_SYS (nv_block_or_sys_enum_type) 0

/* band class (CELLULAR) for FTM */
#ifdef FEATURE_ENHANCED_BAND_CLASS
#define FTM_BAND_CLASS CAI_BAND_CLASS0
#else
#define FTM_BAND_CLASS CAI_BAND_CELL
#endif 

#define FTM_CDMA_CHAN 758
/* cdma channel we'll use for FTM */

#define FTM_TO_SRCH_CMD_MAP_BASE  ( SRCH_FTM_BASE_F + 1 )
  /* The difference between corresponding FTM and SRCH command codes */

#define FTM_CMD_TO_STATE_MAP_BASE  ( -FTM_CMD_INIT_CDMA + 1 )
  /* The difference between corresponding high-level FTM command codes and
     FTM state codes */

/* Macro to kick the watchdog timer and set a timer to wake it again.      */

#define KICK_WATCHDOG() \
   dog_report(DOG_MC_RPT); \
   (void) rex_set_timer(&mc_rpt_timer, DOG_MC_RPT_TIME)


void ftm_fwd_tc_release(void);
void ftm_rev_tc_release(void);

#ifdef FEATURE_IS2000_R_SCH
void ftm_rev_sch_init ( ftm_cmd_type  *cmd_ptr );
#endif /* FEATURE_IS2000_R_SCH */

void ftm_fch_loopback ( boolean lb_enable );
#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
void ftm_dcch_loopback ( boolean lb_enable );
/* End of FEATURE_FTM_DCCH */
#endif
#if (defined(FEATURE_IS2000_F_SCH) && defined(FEATURE_IS2000_R_SCH))
void ftm_sch_loopback ( boolean lb_enable );
#endif

#ifdef FEATURE_IS2000_REL_A
typedef struct {
  boolean fch_has_fpc;
  boolean dcch_has_fpc;
} ftm_fch_dcch_fpc_type;

ftm_fch_dcch_fpc_type g_ftm_fpc = {FALSE, FALSE};
#endif /* FEATURE_IS2000_REL_A */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

static nv_cmd_type  ftm_nv_cmd_buf;   /* Command buffer to NV */
static dword  ftm_nv_esn = 0x0L;      /* ESN read from NV */


ftm_cmd_status_type test_cmd_status;
  /* FTM command status */

ftm_state_enum_type ftm_fwd_curr_state;
  /* Current FTM state on the forward link */

ftm_state_enum_type ftm_rev_curr_state;
  /* Current FTM state on the reverse link */
enc_rate_type ftm_rev_fch_rate;

byte ftm_sframe_num;
  /* sframe number that Sync message was received in */

boolean ftm_bypass_lc = FALSE;
  /* bypass longcodes (all zero long code) */
#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
boolean ftm_bypass_dcch_lc = FALSE;
/* End of FEATURE_FTM_DCCH */
#endif
  

boolean ftm_wait_for_pc_msg = FALSE;
  /* Should rxc wait for a valid PC message? */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Local Data                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* buffers for FTM pseudo MC */
ftm_cmd_type      ftm_mc_cmd_bufs[FTM_CMD_BUFS_NUM];
q_type            ftm_mc_cmd_free_q;
q_type            ftm_mc_cmd_q;

#ifdef FEATURE_SD20
db_items_value_type ftm_db_item;
mcc_entry_type      ftm_cdma_entry; /* CDMA entry parameters */
#endif /* FEATURE_SD20 */


boolean ftm_cdma_enter_init_done = FALSE;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Data                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/* Global variable defined in mcctc */
extern mcc_exit_name_type tc_exit;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*               FUNCTIONS PROTOTYPES and IMPLEMENTATION                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

ftm_cmd_type  *ftm_mc_get_cmd ( void );

void  ftm_post_cmd (ftm_cmd_type  *cmd_ptr, test_cmd_status_type  status);

nv_stat_enum_type ftm_nv_read_item (nv_items_enum_type item_code,
                                    nv_item_type      *item_ptr);

#if defined(FEATURE_HDR) && defined(FTM_HAS_HDR_REV_A)
#error code not present
#endif /* defined(FEATURE_HDR) && defined(FTM_HAS_HDR_REV_A)*/
/*===========================================================================

FUNCTION FTM_MC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

word ftm_mc_wait
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{

  rex_sigs_type   sigs;       /* Signals returned by rex_get_sigs call */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for signals specified in mask and   */
  /* for watchdog signal.  If watchdog signal then kick it  */
  /* and loop again until some other signal in mask is set. */

  while (((sigs = rex_get_sigs(rex_self())) & mask) == 0) {
    KICK_WATCHDOG();
    (void) rex_wait(mask | MC_RPT_TIMER_SIG );
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* kick watchdog one last time and return the signals. */

  KICK_WATCHDOG();
  return(sigs);

} /* ftm_mc_wait */

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
)
{
#ifdef FEATURE_FTM_MC
  int i;

  /* Initialize queues. */
  (void) q_init(&ftm_mc_cmd_q);
  (void) q_init(&ftm_mc_cmd_free_q);

  /* Fill ftm_mc_cmd_free_q. */
  for (i = 0; i < FTM_CMD_BUFS_NUM; i++)
  {
    ftm_mc_cmd_bufs[i].hdr.cmd_hdr.done_q_ptr = &ftm_mc_cmd_free_q;
    q_put(&ftm_mc_cmd_free_q,
          q_link(&ftm_mc_cmd_bufs[i], &ftm_mc_cmd_bufs[i].hdr.cmd_hdr.link));
  }
#endif /* FEATURE_FTM_MC */
}

/*===========================================================================

FUNCTION ftm_nv_read_item

DESCRIPTION
  Read the specified item from NV.

DEPENDENCIES
  None.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
  Any other status causes ERR_FATAL and the function does not return.

SIDE EFFECTS
  While this function is running, all other task activities are
  suspended except for watchdog kicking, and until the NV item is
  read.  If any of read status other then the one described above
  is returned then an ERR_FATAL call is made.

===========================================================================*/

nv_stat_enum_type ftm_nv_read_item
(
  nv_items_enum_type  item_code,       /* Item to get */
  nv_item_type       *item_ptr         /* Pointer where to put the item */
)
{
  /* Prepare command buffer to NV */

  ftm_nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
  ftm_nv_cmd_buf.tcb_ptr = rex_self();           /* Notify back to me    */
  ftm_nv_cmd_buf.sigs = MC_NV_CMD_SIG;           /* With this signal     */
  ftm_nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
  ftm_nv_cmd_buf.item = item_code;               /* Item to get          */
  ftm_nv_cmd_buf.data_ptr = item_ptr;            /* Where to return item */

  /* Clear signal, issue command, and wait for respone. */

  (void) rex_clr_sigs(rex_self(), MC_NV_CMD_SIG); /* Clear signal for NV  */
  nv_cmd(&ftm_nv_cmd_buf);                        /* Issue request        */
  (void) ftm_mc_wait(MC_NV_CMD_SIG);              /* Wait for completion  */

  /* Check and return status. */
  if ((ftm_nv_cmd_buf.status != NV_DONE_S) &&
      (ftm_nv_cmd_buf.status != NV_NOTACTIVE_S))
  {
    ERR_FATAL("Bad NV read status %d", ftm_nv_cmd_buf.status, 0, 0);
  }
  return (ftm_nv_cmd_buf.status);
}

/*===========================================================================

FUNCTION FTM_CDMA_ENTER_INIT

DESCRIPTION
  This procedure initializes variables used in the entrance state for
  the CDMA state machine.  This is a special version just used for FTM.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  Certain variables used by the cdma_enter state will be initialized.

===========================================================================*/
ftm_state_enum_type ftm_cdma_enter_init
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
)
{
  FTM_MSG_HIGH( "MCC ACQUISITION ENTRY", 0, 0, 0 );

  /* store entry parameters for later use */
  cdma.entry_ptr = mcc_entry_reason_ptr;
  cdma.curr_state = CDMA_ENTER;

  if (cdma.entry_ptr->gen.new_mode_or_blksys)
  {
    /* Initialize registration variables */
    mccreg_acq_init();
  }

#ifdef FEATURE_SD20
  mcc_entry_reason_ptr->gen.block_or_sys = FTM_BLOCK_OR_SYS;
  mcc_entry_reason_ptr->gen.band_class = FTM_BAND_CLASS;
  mcc_entry_reason_ptr->gen.cdma_channel = FTM_CDMA_CHAN;
  FTM_MSG_HIGH( "BAND_CLASS %d CHANNEL %d",
             mcc_entry_reason_ptr->gen.band_class,
             mcc_entry_reason_ptr->gen.cdma_channel, 0 );

  /* Initialize rf for cdma */
  cdma_rf_init( mcc_entry_reason_ptr->gen.cdma_channel,
                mcc_entry_reason_ptr->gen.band_class );

  cdma.cdmach       = mcc_entry_reason_ptr->gen.cdma_channel;
  cdma.band_class   = mcc_entry_reason_ptr->gen.band_class;
  cdma.block_or_sys = mcc_entry_reason_ptr->gen.block_or_sys;

#else /* FEATURE_SD20 */
  mcc_entry_reason_ptr->param.acq.block_or_sys = FTM_BLOCK_OR_SYS;
  mcc_entry_reason_ptr->param.acq.band_class = FTM_BAND_CLASS;
  mcc_entry_reason_ptr->param.acq.cdma_channel = FTM_CDMA_CHAN;

  FTM_MSG_HIGH( "SYS_ACQ %d BAND_CLASS %d CHANNEL %d",
             mcc_entry_reason_ptr->param.acq.system_acq,
             mcc_entry_reason_ptr->param.acq.band_class,
             mcc_entry_reason_ptr->param.acq.cdma_channel );


  /* Initialize rf for cdma */
  cdma_rf_init( mcc_entry_reason_ptr->param.acq.cdma_channel,
                cdma.band_class );

  cdma.cdmach       = mcc_entry_reason_ptr->param.acq.cdma_channel;
  cdma.band_class   = mcc_entry_reason_ptr->param.acq.band_class;
  cdma.block_or_sys = mcc_entry_reason_ptr->param.acq.block_or_sys;

#endif /* FEATURE_SD20 */

  /* in FTM we force the CDMA_ENTER state */
  return (FTM_ENTER);
} /* cdma_enter_init */

/*===========================================================================

FUNCTION FTM_CDMA_ENTER

DESCRIPTION
  This procedure is the entrance state for the CDMA state machine.  It will
  request that the other tasks enter the CDMA mode.  This is a special version
  only for FTM.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
ftm_state_enum_type ftm_cdma_enter( void )
{
  /* -----------------------------
  ** Send other tasks to CDMA mode
  ** ----------------------------- */

  /* First of all initialise the BS Overhead Info */
  mcc_init_ovhd_info();

  
  #ifndef FEATURE_SRCH_INACTIVE_STATE
  /* Send request to Searcher task to go to CDMA mode */
  mcc_srch_buf.cdma.hdr.cmd = SRCH_START_F;
  mcc_srch_cmd( &mcc_srch_buf );
  #endif /* FEATURE_SRCH_INACTIVE_STATE */

  /* Send request to Transmit task to go to CDMA mode */
  mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
  mcc_txc_cmd( &mcc_tx_buf );

  /* Transmit task is in CDMA mode, send request to Receive task to
     go to CDMA mode */
  mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* Send request to Searcher task to go to CDMA mode */
  mcc_srch_buf.cdma.hdr.cmd = SRCH_CDMA_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* Ready to attempt Pilot Acquisition */
  return (FTM_INIT_CDMA);
} /* cdma_enter */

/*===========================================================================

FUNCTION FTM_IDLE

DESCRIPTION
  This function handles Idle state operation.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_idle
(
  void
)
{
#ifdef FEATURE_FTM_MC
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Go to jump to hyperspace (Timing Change Substate) */

  ftm_jump();

  if ( ftm_fwd_curr_state == FTM_IDLE )
  {
    /* Jump to hyperspace is completed and successfully direct tasks
       to Paging Channel */

    FTM_MSG_HIGH( "Demodulate Paging Channel", 0, 0, 0 );

    /* Calculate HASH_KEY (see IS-95 section 6.6.7.1) */
    cdma.imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);

    /* Initialize variables used for Idle State */
    mccidl_ftm_init();

  } /* Successfully jump to hyperspace and direct tasks to Paging Channel */

  else /* Fail to go to Paging Channel */
  {
    MSG_ERROR( "Fail to go to Paging Channel", 0, 0, 0 );
  }
#endif  /* FEATURE_FTM_MC */
} /* ftm_idle */

/*===========================================================================

FUNCTION FTM_INIT_CDMA

DESCRIPTION
  This function initializes mobile for CDMA mode.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_init_cdma( void )
{

  /* In FTM context, we have the Searcher go through START then CDMA within this function
  The Search task's state machine begins in START, and under control by Main Control, 
  proceeds generally through the following transitions:
                                    _____________________________
                                   |           /|\               |
                                   |            |                |
                                  \|/           |                |
           START -> CDMA -> ACQ -> SC -> SL -> PC -> TC -> UN -> +
            /|\                                /|\
             |                                  |
            \|/                                \|/
            DZ                                 ZZ 
   */
  #ifndef FEATURE_SRCH_INACTIVE_STATE
  mcc_srch_buf.cdma.hdr.cmd = SRCH_START_F;
  mcc_srch_cmd( &mcc_srch_buf );
  #endif /* FEATURE_SRCH_INACTIVE_STATE */

  
  /* Send request to TX task to go to CDMA mode */
  /* TXC can only take this command if it is beyond its initial state */
  if (ftm_rev_curr_state > FTM_IDLE)
  {
    /* Send request to Transmit task to go to CDMA mode */
    mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
    mcc_txc_cmd( &mcc_tx_buf );
  }

  /* Send request to Receive task to go to CDMA mode */
  mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* Send request to Searcher task to go to CDMA mode */
  mcc_srch_buf.cdma.hdr.cmd = SRCH_CDMA_F;
  mcc_srch_cmd( &mcc_srch_buf );

  ftm_fwd_curr_state = ftm_rev_curr_state = FTM_INIT_CDMA;
    /* Remember the new state */

} /* ftm_init_cdma */

/*===========================================================================

FUNCTION FTM_PIL_ACQ

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_pil_acq
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We already acquired a pilot, but we want to acquire again. Tell RX
     to go back to CDMA mode */

  if ( ftm_fwd_curr_state > FTM_PIL_ACQ )
  {
    FTM_MSG_HIGH( "Direct RX back to CDMA mode", 0, 0, 0 );

    /* Send request to Receive task to go to CDMA mode */

    mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
    mcc_rxc_cmd( &mcc_rx_buf );
  }

  /* If it is a different band or channel, retune */

  if ( (cmd_ptr->pilot_acq.param.cdma_freq != cdma.cdmach) ||
       (cmd_ptr->pilot_acq.param.band_class != cdma.band_class) )
  {

    cdma.band_class = cmd_ptr->pilot_acq.param.band_class;
    cdma.cdmach = cmd_ptr->pilot_acq.param.cdma_freq;
      /* Remember the band and channel we tuned to */
  }

  /* Request that Searcher task enter Acquire mode */
  mcc_srch_buf.acq.hdr.cmd   = SRCH_ACQ_F;
  mcc_srch_buf.acq.cdma_freq = cmd_ptr->pilot_acq.param.cdma_freq;
  mcc_srch_buf.acq.band_class = cmd_ptr->pilot_acq.param.band_class;
  mcc_srch_buf.acq.acq_mode = cmd_ptr->pilot_acq.param.acq_mode;

  mcc_srch_cmd( &mcc_srch_buf );

  FTM_MSG_HIGH( "Wait for SRCH ACQ rpt", 0, 0, 0 );

  (void)mcc_wait( MCC_SRCH_Q_SIG );
    /* Wait for SRCH acquisition report. mcc_wait() clears
       the signal after it gets the signal it waits for */

  /* Get SRCH acquisition report */

  if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
  {
    if (srch_ptr->hdr.rpt == SRCH_ACQ_R)
    {
      /* Searcher has acquired */

      test_cmd_status = TEST_SUCCESS;

      ftm_fwd_curr_state = FTM_PIL_ACQ;
        /* Remember the new state */
    }
    else if (srch_ptr->hdr.rpt == SRCH_ACQFAIL_R)
    {
      /* Searcher failed to acquire */

      FTM_MSG_HIGH( "CDMA pilot acquisition failed", 0, 0, 0 );
      test_cmd_status = TEST_NO_SUCCESS;
    }

    /* Put item back on queue if Searcher task requests it */
    cmd_done( &srch_ptr->hdr.rpt_hdr );
  }
  else
  {
    MSG_ERROR( "Cannot get SRCH acquisition report", 0, 0, 0 );

    test_cmd_status = TEST_NO_SUCCESS;
  }

} /* ftm_pil_acq */

/*===========================================================================

FUNCTION FTM_SYNC

DESCRIPTION
  This function attempts

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_mc_sync
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{

  mccrxtx_cmd_type *msg_ptr;  /* Pointer to msg received on the mcc_rxtx_q */
  bs_info_id_type  bs_info_id; /* Keys to identify BS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  FTM_MSG_HIGH( "Demod SC", 0, 0, 0 );

  /* Send request to Receive task to go to Sync mode */

  mcc_rx_buf.sc.hdr.command = RXC_SC_F;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* Send request to Searcher task to go to Sync mode. Wait for
     it to finish the command */

  mcc_srch_buf.sc.hdr.cmd = SRCH_SC_F;
  mcc_srch_cmd( &mcc_srch_buf );

  FTM_MSG_HIGH( "Wait for RXC message", 0, 0, 0 );

  (void)mcc_wait( MCC_RXTX_Q_SIG );

  if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q) ) != NULL)
  {
    /* Received a message --> process it */

    if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
        (msg_ptr->msg.chn == RXC_CDMA_SC))
    {
      /* Message from Sync Channel */
      parm_store_sync_time( msg_ptr->msg.msg.sync.sys_time );

      /* ---------------------------------------------------------
      ** We have got the sync channel message, which means we need
      ** to start storing information about this base station.
      ** Initialize a new entry in bs_info array (or find the old
      ** one if it exists.
      ** --------------------------------------------------------- */
      bs_info_id.ovhd_chan = CAIX_PC;
      bs_info_id.pilot_pn = msg_ptr->msg.msg.sync.pilot_pn;
      bs_info_id.cdma_ch = cdma.cdmach;
      bs_info_id.ovhd_walsh_ch = PRI_PAGECH;
      bs_info_id.band_class = cdma.band_class;
      mcc_entry_init(&bs_info_id, FALSE);

      /* Place the received Sync Channel msg into the entry for this BS */
      mcc_put_msg( MC_SYNC_MSG, &msg_ptr->msg.msg );

#ifdef FEATURE_ENHANCED_BAND_CLASS
      if ((cdma.band_class == CAI_BAND_CLASS0) ||
         ((cdma.band_class == CAI_BAND_CLASS1) &&
#else
      if ((cdma.band_class == CAI_BAND_CELL) ||
         ((cdma.band_class == CAI_BAND_PCS) &&
#endif
          (msg_ptr->msg.msg.sync.p_rev >= 3)))
      {
        /* The Base Station supports the Sync Channel Frequency */
        if (cdma.cdmach != msg_ptr->msg.msg.sync.cdma_freq)
        {
          cdma.cdmach = msg_ptr->msg.msg.sync.cdma_freq;
          FTM_MSG_HIGH(" Sync freq %d", msg_ptr->msg.msg.sync.cdma_freq,
                   0, 0);

        }
      }

      ftm_sframe_num = msg_ptr->msg.frm.sframe;
        /* Remember the subframe number that message is received in */

      test_cmd_status = TEST_SUCCESS;

      ftm_fwd_curr_state = FTM_SYNC;
        /* Remember the new forward state */
    }

    /* Unexpected message */

    else
    {
      MSG_ERROR( "Unexpected RXC message:cmd=%d,chn=%d",
         msg_ptr->hdr.command, msg_ptr->msg.chn, 0 );

      test_cmd_status = TEST_NO_SUCCESS;
    }

    /* Place message from layer 2 back on queue if requested */
    cmd_done( &msg_ptr->hdr.cmd_hdr );

  } /* end if ((msg_ptr = q_get( &mcc_rxtx_q) ) != NULL) */
  else
  {
    MSG_ERROR( "Cannot get RXC message", 0, 0, 0 );

    test_cmd_status = TEST_NO_SUCCESS;
  }

} /* ftm_mc_sync */

#ifdef FTM_HAS_SRCH_HHO
/*===========================================================================

FUNCTION FTM_MC_FWD_HHO

DESCRIPTION
  This function attempts a hard handoff

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_mc_fwd_hho
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
   FTM_MSG_HIGH( "Hard Handoff SC", 0, 0, 0 );

   if (cmd_ptr->fwd_hho.param.enable_rpc == TRUE) 
   {
     /* clear the 'disable closed loop power control' dip switch */
	 bio_set_sw(BIO_GET_SW(0xFFFFFFFF) & (~BIO_SW5_M)); 
   }
   else
   {
     /* set the 'disable closed loop power control' dip switch */
     bio_set_sw(BIO_SW5_M); 
   }

   srch_ftm_rx_tune (cmd_ptr->fwd_hho.param.band_class, cmd_ptr->fwd_hho.param.cdma_freq);
    
} /* ftm_mc_fwd_hho */
#endif

/*===========================================================================

FUNCTION FTM_HIGH_LEVEL_STATE_CMD_PROC

DESCRIPTION
  This function processes high-level FTM state commands and, based on the
  state, calls appropriate function that sends commands to low-level tasks.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_high_level_state_cmd_proc
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
#ifdef FEATURE_FTM_MC

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (cmd_ptr->hdr.cmd)
  {
  case FTM_CMD_INIT_CDMA:
	if (!ftm_cdma_enter_init_done)
	{
	  ftm_cdma_enter_init(&ftm_cdma_entry);
	  ftm_cdma_enter(); 
	  ftm_cdma_enter_init_done= TRUE;
	}
    ftm_init_cdma();
    test_cmd_status = TEST_SUCCESS;
    break;     
  case FTM_CMD_PILOT_ACQ:
	if (!ftm_cdma_enter_init_done)
	{
	  ftm_cdma_enter_init(&ftm_cdma_entry);
	  ftm_cdma_enter(); 
	  ftm_cdma_enter_init_done= TRUE;
	}
	ftm_init_cdma();
    ftm_pil_acq( cmd_ptr );
	ftm_rev_curr_state = FTM_INIT_CDMA;
	ftm_fwd_curr_state = FTM_PIL_ACQ;
    break;     
  case FTM_CMD_RELEASE:
    if(ftm_fwd_curr_state == FTM_TRAFFIC)
    {
      /* Issue a release command */
       ftm_fwd_tc_release();
    }
    if(ftm_rev_curr_state == FTM_REV_TRAFFIC)
    {
      /* Issue a release command */
       ftm_rev_tc_release();
    }
        ftm_init_cdma();
    ftm_fwd_curr_state = ftm_rev_curr_state = FTM_INIT_CDMA; 
    break;

  case FTM_CMD_DEMOD_SYNC:
    // this is only allowed from the previous or current state
    if ((ftm_fwd_curr_state == FTM_PIL_ACQ) || (ftm_fwd_curr_state == FTM_SYNC))
    {
      ftm_mc_sync( cmd_ptr );
        /* Demodulate Sync channel */
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state for this command: Curr fwd state=%d",
                   ftm_fwd_curr_state, 0, 0 );
      test_cmd_status = TEST_INVALID_STATE;
    }
    break;

#ifdef FTM_HAS_SRCH_HHO
  case 	FTM_CMD_FWD_HHO:
	  // this is only allowed from the previous or current state
    if ((ftm_fwd_curr_state == FTM_TRAFFIC) || (ftm_fwd_curr_state == FTM_REV_TRAFFIC))
    {
        FTM_MSG_HIGH( "Executing Hard Handoff SC", 0, 0, 0 );
		ftm_mc_fwd_hho( cmd_ptr );
        /* Attempt hard handoff */
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state for this command: Curr fwd state=%d",
                   ftm_fwd_curr_state, 0, 0 );
      test_cmd_status = TEST_INVALID_STATE;
    }
	break;     
#endif

  case FTM_CMD_DEMOD_FCH:
    // this is only allowed from the previous or current state
    if (ftm_fwd_curr_state == FTM_SYNC)
    {
      /* In FTM SYNC state we do not continue to update the LC state, so before
         idle we need to grab another sync message */
      ftm_mc_sync( cmd_ptr );
      ftm_idle();
    }
    if ((ftm_fwd_curr_state == FTM_IDLE) ||
        (ftm_fwd_curr_state == FTM_TRAFFIC))
    {
      ftm_fwd_fch( cmd_ptr );
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state fwd state=%d",
                   ftm_fwd_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  case FTM_CMD_DEMOD_FDCCH:
    // this is only allowed from the previous or current state
    if (ftm_fwd_curr_state == FTM_SYNC)
    {
      /* In FTM SYNC state we do not continue to update the LC state, so before
         idle we need to grab another sync message */
      ftm_mc_sync( cmd_ptr );
      ftm_idle();
    }
    if ((ftm_fwd_curr_state == FTM_IDLE) || (ftm_fwd_curr_state == FTM_TRAFFIC))
    {
      ftm_fwd_dcch(cmd_ptr);
    }
    else 
    {
      FTM_MSG_HIGH( "Invalid state for this command: Curr fwd state=%d", 
                   ftm_fwd_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
  /* End of FEATURE_FTM_DCCH */


  /* Support FEATURE_FTM_FCH_DCCH */
  case FTM_CMD_DEMOD_FFCH_FDCCH:
    // this is only allowed from the previous or current state
    if (ftm_fwd_curr_state == FTM_SYNC)
    {
      /* In FTM SYNC state we do not continue to update the LC state, so before
         idle we need to grab another sync message */
      ftm_mc_sync( cmd_ptr );
      ftm_idle();
    }
    if ((ftm_fwd_curr_state == FTM_IDLE) || (ftm_fwd_curr_state == FTM_TRAFFIC))
    {
      ftm_fwd_fch_dcch(cmd_ptr);
    }
    else 
    {
      FTM_MSG_HIGH( "Invalid state for this command: Curr fwd state=%d", 
                   ftm_fwd_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_F_SCH
  case FTM_CMD_DEMOD_FSCH:
    // this is only allowed from the previous or current state
    if ((ftm_fwd_curr_state == FTM_IDLE) || (ftm_fwd_curr_state == FTM_TRAFFIC))
    {
      /* Add Supplemental Channel */
      ftm_tc_add_f_sch(cmd_ptr);

    }
    else
    {
      FTM_MSG_HIGH( "Invalid state fwd state=%d",
                   ftm_fwd_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#endif /* FEATURE_IS2000_F_SCH */

  case FTM_CMD_MOD_FCH:
    // this is only allowed from the previous or current state
    if (ftm_rev_curr_state == FTM_INIT_CDMA )
    {
      ftm_sys_acc();
    }
    if ((ftm_rev_curr_state == FTM_SYS_ACC) || (ftm_rev_curr_state == FTM_REV_TRAFFIC))
    {
      ftm_rev_tc_init( cmd_ptr );
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state rev state=%d",
                   ftm_rev_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#ifdef FEATURE_IS2000_P2
   /* Support FEATURE_FTM_DCCH */
   case FTM_CMD_MOD_DCCH:
    // this is only allowed from the previous or current state
    if (ftm_rev_curr_state == FTM_INIT_CDMA )
    {
      ftm_sys_acc();
    }
    if ((ftm_rev_curr_state == FTM_SYS_ACC) || (ftm_rev_curr_state == FTM_REV_TRAFFIC))
    {
      ftm_rev_dcch_init( cmd_ptr );        
    }
    else 
    {
      FTM_MSG_HIGH( "Invalid state for this command: Curr fwd state=%d", 
                   ftm_fwd_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
  /* End of FEATURE_FTM_DCCH */

  case FTM_CMD_MOD_FCH_DCCH:
   // this is only allowed from the previous or current state
   if (ftm_rev_curr_state == FTM_INIT_CDMA )
   {
     ftm_sys_acc();
   }
   if ((ftm_rev_curr_state == FTM_SYS_ACC) || (ftm_rev_curr_state == FTM_REV_TRAFFIC))
   {
     ftm_rev_fch_dcch_init( cmd_ptr );        
   }
   else 
   {
     FTM_MSG_HIGH( "Invalid state for this command: Curr fwd state=%d", 
                  ftm_fwd_curr_state, 0, 0 );

     test_cmd_status = TEST_INVALID_STATE;
   }
   break;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_R_SCH
  case FTM_CMD_MOD_SCH:
    /* rev sch assignment is only allowed after rev fch is already assigned */
    if (ftm_rev_curr_state == FTM_REV_TRAFFIC)
    {
      ftm_rev_sch_init( cmd_ptr );
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state rev state=%d",
                   ftm_rev_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#endif /* FEATURE_IS2000_R_SCH */

  case FTM_CMD_FCH_LOOPBACK:
    if (ftm_rev_curr_state == FTM_REV_TRAFFIC)
    {
      ftm_fch_loopback( TRUE );
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state rev state=%d",ftm_rev_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  case FTM_CMD_DCCH_LOOPBACK:
    if (ftm_rev_curr_state == FTM_REV_TRAFFIC)
    {
      ftm_dcch_loopback( TRUE );
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state rev state=%d",ftm_rev_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
  /* End of FEATURE_FTM_DCCH */
#endif

#if (defined(FEATURE_IS2000_F_SCH) && defined(FEATURE_IS2000_R_SCH))
  case FTM_CMD_SCH_LOOPBACK:
    if ( (ftm_rev_curr_state == FTM_REV_TRAFFIC) &&
         (ftm_fwd_curr_state == FTM_TRAFFIC) )
    {
      ftm_sch_loopback( TRUE );
    }
    else
    {
      FTM_MSG_HIGH( "Invalid state rev state=%d",ftm_rev_curr_state, 0, 0 );

      test_cmd_status = TEST_INVALID_STATE;
    }
    break;
#endif /* FEATURE_IS2000_F_SCH && FEATURE_IS2000_R_SCH */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
   
  default:
     FTM_MSG_HIGH("Unknown FTM cmd %d, state %d %d", cmd_ptr->hdr.cmd,
             ftm_fwd_curr_state, ftm_rev_curr_state);
     test_cmd_status = TEST_INVALID_CMD;
  }

#endif /* FEATURE_FTM_MC */
} /* ftm_high_level_state_cmd_proc */

/*===========================================================================

FUNCTION TEST_MCC

DESCRIPTION
  This function contains the state machine and event loop of the FTM main
  control task.  FTM_MCC receives commands from the diag task as part of
  the overall RPC mechanism and processes those commands.

  In the case of "high level" commands, i.e. those commands that cause a
  state transition, FTM_MCC orchestrates the state transitions sending
  commands to SRCH, RXC, and TXC, as appropriate.  A high level command
  will have a target state associated with it.  FTM_MCC will continue
  through the state machine until the target state is reached at which
  point it will wait for further commands.

  FTM_MCC will wait for a report from the appropriate task to determine
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
)
{
  ftm_cmd_type  *ftm_cmd_ptr;
    /* Pointer to the current command */

  mc_msg_type *mc_cmd_ptr;
    /* Pointer to current mc command */

  #ifdef FEATURE_MULTIMODE_ARCH
  boolean      exit_ftm = FALSE;
  #endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read ESN from NV */
  if (ftm_nv_read_item (NV_ESN_I, (nv_item_type *)&ftm_nv_esn) != NV_DONE_S)
  {
    ftm_nv_esn = 0xdeadf00d;  /* Set ESN=0xdeadf00d if reading NV fails */
  }

  /* Initialize entrance states (= FTM_ENTER) */
  ftm_fwd_curr_state = ftm_cdma_enter_init (mcc_entry_reason_ptr);
  ftm_rev_curr_state = ftm_fwd_curr_state;

  /* Signal FTM task, indicating MC is FTM ready */
  ftm_sig_rex_set( FTM_MC_READY_SIG );

  /* Handle FTM commands and report command status to DIAG */

  while ((ftm_fwd_curr_state != FTM_EXIT ) &&
         (ftm_rev_curr_state != FTM_EXIT )
         #ifdef FEATURE_MULTIMODE_ARCH
         && !exit_ftm
         #endif
        )
  {
    test_cmd_status = TEST_SUCCESS;
      /* Initiliaze command status */

    /* Check if there is a new command */

    if ((ftm_cmd_ptr = (ftm_cmd_type *)ftm_mc_get_cmd()) != NULL)
    {

      /* High-level FTM state commands */
      ftm_high_level_state_cmd_proc( ftm_cmd_ptr );
      /* process High-level FTM state commands */

      FTM_MSG_HIGH( "Curr fwd=%d,rev=%d,cmd status=%d",
                   ftm_fwd_curr_state, ftm_rev_curr_state, test_cmd_status );


      ftm_post_cmd( ftm_cmd_ptr, test_cmd_status );
        /* Finish up the FTM command */

    }
    /* Check for MC commands on old MC command queue */
    else if ((mc_cmd_ptr = (mc_msg_type *)q_get(&mc_cmd_q)) != NULL)
    {
      switch (mc_cmd_ptr->hdr.cmd)
      {
        #ifdef FEATURE_MULTIMODE_ARCH
        case MC_CDMA_PROT_DEACT_CMD_F:
          exit_ftm = TRUE;
		  if (ftm_cdma_enter_init_done)
		  {
          cdma_exit( FALSE );
			ftm_cdma_enter_init_done= FALSE;
		  }
          ftm_rfmode_exit();
          rfnv_retrieve_nv_items( rex_self(), MC_NV_CMD_SIG, (void (*)( rex_sigs_type )) mcc_wait );  
          rf_init_rf_nv_items();
          mmoc_rpt_prot_deactd_cnf( mc_cmd_ptr->deact_prot.info.trans_id );
          mc_cmd_ptr->hdr.status = MC_DONE_S;
          break;

        #else  /* FEATURE_MULTIMODE_ARCH */
        case MC_RESET_F:
          mc_cmd_ptr->hdr.status = MC_DONE_S;
          hw_reset();
          /* We never get here */
          break;

        case MC_OFFLINE_DIGITAL_F:
#ifdef FEATURE_SD20
          mc_update_last_cdma_entry(MCC_OFFLINE);
#else
          mcsyspr_last_entry_params.cdma = MCC_ENTER_OFFLINE;
#endif /* FEATURE_SD20 */
          {
            db_items_value_type mc_db_item;
            mc_db_item.dmss_state = DB_DMSS_OFFLINE_DIGITAL_S;
            db_put(DB_DMSS_STATE, &mc_db_item);
          }
          (void) rex_set_sigs( &diag_tcb, DIAG_OFFLINE_COMPLETE_SIG );

          mc_cmd_ptr->hdr.status = MC_DONE_S;
          break;
        #endif /* FEATURE_MUTLIMODE_ARCH */

        default:
          FTM_MSG_HIGH("Ignoring MC cmd %d", mc_cmd_ptr->hdr.cmd, 0, 0);
      }
      cmd_done( &mc_cmd_ptr->hdr.cmd_hdr );
    }
    else /* No new command */
    {
      (void) ftm_mc_wait(FTM_MC_CMD_Q_SIG);
        /* Wait for signal to be set. When it is set, clear the signal. */
    }
  } /* End of while ( ftm_curr_state != FTM_EXIT ) */

  /* This point is unreachable, as there is not exit from FTM */

  return (&mcc_exit);

} /* test_mcc */

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

void ftm_jump
(
  void
)
{
#ifdef FEATURE_FTM_MC
  mccsrch_rpt_type *srch_ptr;  /* Pointer to report from Searcher task */
  mccrxtx_cmd_type *msg_ptr;   /* Pointer to msg received on the mcc_rxtx_q */
  boolean done_jump = FALSE;   /* Indicate if jump to hyperspace is done */
  db_items_value_type db_value;/* Buffer to send data to DB */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cdma.pagech = 1;

  /* Initialize PAGE_CHANs to 1 (IS-95 section 6.6.1.4) */
  cdma.page_chan_s = 1;

  /* Command Searcher task to Jump to slew */

  mcc_srch_buf.slew.hdr.cmd = SRCH_SLEW_F;

  mcc_srch_buf.slew.pilot_index = (int2) cur_bs_ptr->pilot_pn;
  mcc_srch_buf.slew.phase_80ms =  ftm_sframe_num;

  mcc_srch_buf.slew.cdma_freq = cdma.cdmach;

  mcc_srch_buf.slew.band_class = cdma.band_class;

  mcc_srch_cmd( &mcc_srch_buf );

  while ( !done_jump )
  {
    if ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
    {
      if ( srch_ptr->hdr.rpt == SRCH_SLEW_R )
      {
        done_jump = TRUE;
          /* finish Jump to hyperspace and go to idle */
      }

      /* Place item back on queue and set signal if requested */
      cmd_done( &srch_ptr->hdr.rpt_hdr );
    }
    else
    {
      /* Wait for report */
      (void) mcc_wait( MCC_SRCH_Q_SIG );
    }
  }

  /* Continue with jump to Hyperspace */

  /* Request that Transmit task begin Access Channel operations */
  mcc_tx_buf.pc.hdr.command = TXC_PC_F;
  mcc_txc_cmd( &mcc_tx_buf );

  /* Tell Receive task to go to idle state */
  mcc_rx_buf.idle.hdr.command = RXC_IDLE_F;
  mcc_rx_buf.idle.chan_info.chan_id = CAI_PCH;
  mcc_rx_buf.idle.chan_info.config.pch.rate = 0;

  /* Multiply time received in message by 4 to get
     frame number for Receive task */
  qw_mul(mcc_rx_buf.idle.frame, cur_bs_ptr->sy.sys_time, 4L);

  /* seed random number generator */
  ran_seed((ftm_nv_esn ^ qw_lo( mcc_rx_buf.idle.frame )) % 0x7FFFFFFF);

  mcc_rxc_cmd( &mcc_rx_buf );

  /* Time to tell Searcher to go to Idle state */
  mcc_srch_buf.pc.hdr.cmd = SRCH_IDLE_F;

  /* Place paging channel long code state @ next 80ms into buffer */
  qw_equ( mcc_srch_buf.idle.pn_state, cur_bs_ptr->sy.lc_state );

  /* Account for extra 80ms count in system time about to occur in TS */
  qw_equ( mcc_srch_buf.idle.sys_time, cur_bs_ptr->sy.sys_time );
  qw_dec( mcc_srch_buf.idle.sys_time, 1L );

  /* calculate Paging Channel or Broadcast Control Channel mask */

  qw_set( mcc_srch_buf.idle.pn_mask,  0x319L,  0xa0200000);

  mcc_srch_buf.idle.cdma_freq = cdma.cdmach;

  mcc_srch_buf.idle.band_class = cdma.band_class;

  mcc_srch_cmd( &mcc_srch_buf );


  if ( ftm_wait_for_pc_msg )
  {
    FTM_MSG_HIGH( "Wait for RXC message", 0, 0, 0 );

    mcc_wait( MCC_RXTX_Q_SIG );

    if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q) ) != NULL)
    {
      /* Received a message --> process it */

      if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
          (msg_ptr->msg.chn == RXC_CDMA_PC))
      {
        test_cmd_status = TEST_SUCCESS;

        ftm_fwd_curr_state = FTM_IDLE;
          /* Remember the new state */
      }

      /* Unexpected message */

      else
      {
        MSG_ERROR( "Unexpected RXC message:cmd=%d,chn=%d",
           msg_ptr->hdr.command, msg_ptr->msg.chn, 0 );

        test_cmd_status = TEST_NO_SUCCESS;
      }

      /* Place message from layer 2 back on queue if requested */
      cmd_done( &msg_ptr->hdr.cmd_hdr );

      /* Update code channel */
      db_value.code_channel = cdma.pagech;
      db_put( DB_CODE_CHANNEL, &db_value );

    } /* end if ((msg_ptr = q_get( &mcc_rxtx_q) ) != NULL) */
    else
    {
      MSG_ERROR( "Cannot get RXC message", 0, 0, 0 );

      test_cmd_status = TEST_NO_SUCCESS;
    }
  } /* Wait for a Paging Channel message */
  else
  {
    /* Don't wait for a PC message */
    srch_start_pc_state(FALSE);

    test_cmd_status = TEST_SUCCESS;

    ftm_fwd_curr_state = FTM_IDLE;
      /* Remember the new state */
  }
#endif /* FEATURE_FTM_MC */
} /* ftm_jump */

/*===========================================================================

FUNCTION FTM_SYS_ACC

DESCRIPTION
  This function sends an (request) Access command to the Transmit task.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the access attempt was aborted, else FALSE.

SIDE EFFECTS
  None

===========================================================================*/

void ftm_sys_acc
(
   void
)
{
   //
   // Build a dummy access message of arbit. length and send it to txc
   //
   // use the access paramters in cmd_ptr ato send_access_cmd() to txc

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  static byte acc_msg[33];

  mcc_tx_buf.acc.len = 33*8;

  mcc_tx_buf.acc.msg_ptr = acc_msg;

  /* Send command to transmit task to send access message */
  mcc_tx_buf.hdr.command = TXC_ACCESS_F;
  mcc_tx_buf.hdr.cmd_hdr.done_q_ptr = NULL;
  mcc_tx_buf.hdr.cmd_hdr.task_ptr = NULL;

  /* Signal when access attempt is finished */
  mcc_tx_buf.acc.tcb_ptr = rex_self();
  mcc_tx_buf.acc.sigs = MCC_TXC_DONE_SIG;

  mcc_tx_buf.acc.pam_sz = 0;
  mcc_tx_buf.acc.max_cap_sz = 0;
  mcc_tx_buf.acc.req = TRUE;
  mcc_tx_buf.acc.acc_tmo = 0;
  mcc_tx_buf.acc.bkoff = 1;
  mcc_tx_buf.acc.probe_bkoff = 0;
  mcc_tx_buf.acc.init_pwr = 0;

  mcc_tx_buf.acc.nom_pwr = 0;

  mcc_tx_buf.acc.acc_chan = 0;
  mcc_tx_buf.acc.use_chan = 0;

  mcc_tx_buf.acc.pwr_step = 1;
  mcc_tx_buf.acc.num_step = 1;

  /* ------------------------------------------------------------
  ** The following three parameters are used for constructing the
  ** Access Channel long code mask
  ** ------------------------------------------------------------ */
  mcc_tx_buf.acc.pagech   = 1;
  mcc_tx_buf.acc.base_id  = 200;
  mcc_tx_buf.acc.pilot_pn = 12;

  /* Hash the probe pn randomization even though we dont do it for now */
  mcc_tx_buf.acc.rn = 4;

  mcc_tx_buf.acc.seq_num = 1;
  mcc_tx_buf.acc.p = 0xffffffff;

  rex_clr_sigs(rex_self(), MCC_TXC_DONE_SIG);
  txc_cmd( &mcc_tx_buf );

  /* Indicate that we are starting a new access attempt */
  mccsa_set_access_in_progress();

  /* Wait for access probes to complete */
  (void) ftm_mc_wait(MCC_TXC_DONE_SIG);

  /* ------------------------------------------------------------
  ** Clear the state timer.  We set the timer when we complete
  ** an access attempt in Page Response or Origination Substates,
  ** in case a delayed response never comes.
  ** ------------------------------------------------------------ */
  (void) rex_clr_timer( &mcc_state_timer );
  (void) rex_clr_sigs( &mc_tcb,  MCC_STATE_TIMER_SIG );
  test_cmd_status = TEST_SUCCESS;
  ftm_rev_curr_state = FTM_SYS_ACC;
} /* ftm_sys_acc */

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
)
{
  srch_parms->hdr.cmd       = SRCH_PARM_F;
  /* Fill in the soft handoff parameters for search */
  srch_parms->win_a         = 8;
  srch_parms->win_n         = 8;
  srch_parms->win_r         = 8;
    /* "8" = 60 chips. A reasonable value */
  srch_parms->t_add         = 28;  /* -14 dB */
  srch_parms->t_drop        = 32;  /* -16 dB */
  srch_parms->t_comp        = 10;  /* 5 dB */
  srch_parms->t_tdrop       = 60;  /* 60 * 80 msec */
  srch_parms->nghbr_max_age = 0xFF;

#ifdef FEATURE_IS95B_SOFT_HO
  srch_parms->soft_slope     = 0;
  srch_parms->add_intercept  = 0;
  srch_parms->drop_intercept = 0;
#endif /* FEATURE_IS95B_SOFT_HO */

#ifdef FEATURE_IS95B_SYS_RESELEC
  /* disable system reselect thresholds */
  srch_parms->sys_reselect_included = FALSE;
  srch_parms->ec_thresh             = 0;
  srch_parms->ec_io_thresh          = 0;
#endif /* FEATURE_IS95B_SYS_RESELEC */
  /* slot cycle stuff */
  srch_parms->slot_cycle_index      = 0;
  srch_parms->pgslot           = 0;
  srch_parms->bcast_index      = 0;

} /* ftm_set_srch_parms */

/*===========================================================================

FUNCTION FTM_TC_INIT

DESCRIPTION
  This function directs the mobile to go to Traffic Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_tc_init
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report received from Receive task */
#ifndef FEATURE_COMPLETE_SNM
  caii_srv_cfg_type mcc_srv_cfg;
    /* status configuration for requested service option */
  snm_rate_set_pair_type rate_set;
#endif

  enum
  {
    TCI_1,  /* State in which the Search task is commanded to go to the
               Traffic Channel */
    TCI_2,  /* State in which frame quality metrics are checked waiting
               for a valid frame */
    TCI_3   /* State in which the Receive task is commanded to go to the
               Traffic Channel */
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_1;

  /* --------------------------------------------------------
  ** Initialize primary and secondary service options to NULL
  ** to indicate that there is no active service option
  ** -------------------------------------------------------- */
  cdma.so_cur_pri = CAI_SO_NULL;
  cdma.so_cur_sec = CAI_SO_NULL;

  /* Initialize the state of the Markov model to unsynchronized */
  mar_rate = MAR_VAR_RATE;
  mar_init_synch();

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  while( ftm_fwd_curr_state != FTM_TRAFFIC )
  {
    switch( sub_state )
    {
      case TCI_1:
      {
        ftm_set_srch_parms(&mcc_srch_buf.parm);
        mcc_srch_cmd(&mcc_srch_buf);
          /* Sends search parameters to searcher */

        cdma.code_chan = cmd_ptr->demod_fch.param.walsh.walsh;

        /* Send a message to Searcher to switch to Traffic Channel */
        mcc_srch_buf.tc.hdr.cmd = SRCH_TC_F;

        /* Send new active set to Searcher */
        mcc_srch_buf.tc.aset_cnt  = 1;

        mcc_srch_buf.tc.aset[0].pilot = cur_bs_ptr->pilot_pn;

        mcc_srch_buf.tc.aset[0].fch_info.active = TRUE;
#ifdef FEATURE_IS2000_P2
        mcc_srch_buf.tc.aset[0].dcch_info.active = FALSE;
#endif 

        mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_REL_A_TD
        MSG_MED("Pilot = 1X common", 0, 0, 0);
/*
        if ( cmd_ptr->demod_fch.param.td_mode == 0 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;
          MSG_MED("Pilot = 1X common", 0, 0, 0);
        }
        else if ( cmd_ptr->demod_fch.param.td_mode == 1 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_power_level =
              cmd_ptr->demod_fch.param.td_power_level;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_mode = CAI_TD_MODE_OTD;
          MSG_MED("Pilot = 1X common + OTD", 0, 0, 0);
        }
        else if ( cmd_ptr->demod_fch.param.td_mode == 2 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_power_level =
              cmd_ptr->demod_fch.param.td_power_level;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_mode = CAI_TD_MODE_STS;
          MSG_MED("Pilot = 1X common + STS", 0, 0, 0);
        }
        else
        {
          MSG_ERROR("Pilot = unknown", 0, 0, 0);
        }
*/
#endif /* FEATURE_IS2000_REL_A_TD */
#endif /* FEATURE_IS2000_REL_A */
        
        mcc_srch_buf.tc.aset[0].fch_info.walsh_chn = cdma.code_chan;
#ifdef T_MSM5000
        mcc_srch_buf.tc.aset[0].fch_info.qof_mask_id =
           cmd_ptr->demod_fch.param.walsh.qof_mask_id;
#endif /* T_MSM5000 */
        mcc_srch_buf.tc.aset[0].pwr_comb_ind = 0;
          /* Flag: nonzero if the power control subchannel on this pilot
             contains the same symbols as that on the previous pilot. */

        mcc_srch_buf.tc.cdma_freq = cdma.cdmach;

        mcc_srch_buf.tc.band_class = cdma.band_class;
        /* Calculate Traffic Channel Mask */
        if (cmd_ptr->demod_fch.param.bypass_lc)
        {
          qw_set( mcc_srch_buf.tc.pn_mask, 0L, 0L);
          ftm_bypass_lc = TRUE;
        } else {
          dword p_esn;
          qw_set(mcc_srch_buf.tc.pn_mask, 0x318L,
                 (p_esn = permute_esn(ftm_nv_esn)));
          FTM_MSG_HIGH("Using pn_mask 0x318 0x%x", p_esn, 0,0);
          ftm_bypass_lc = FALSE;
        }

        mcc_srch_cmd( &mcc_srch_buf );

        /* Tell Receieve task to go to Traffic channel Initialization */
        sub_state = TCI_2;
        break;
      } /* end of case TCI_1 */

      case TCI_2:
      {
        /* Send a message to the Receive task to tune to the traffic
           channel and begin sending frame quality metrics */
        mcc_rx_buf.tci.hdr.command = RXC_TCI_F;

#ifndef FEATURE_COMPLETE_SNM
        /* Consult SOs defined in Service Negotiation Manager
           (SNM).  If the SO is not in SNM, we use the old way.  Eventually,
           all SOs should be captured in SNM. */

        if ( snm_get_rate_set( 0, &rate_set ) == SNM_FAILURE )
        // The following will be obsolete when all SO are captured in SNM
        {
          (void) srv_set_def_cfg( 0, &mcc_srv_cfg, FALSE );
          rate_set.forward = (dec_rate_set_type)( mcc_srv_cfg.fwd_mux - 1 );
          rate_set.reverse = (dec_rate_set_type)( mcc_srv_cfg.rev_mux - 1 );
        }
#endif

        /* Set frame offset for RXC */
        // Sending the TXC_FRAME_OFF command to txc will set the encoder
        // up for fwd as well as reverse frame offsets
        //

        /* ------------------------------------------------------------
        ** Set the Forward and Reverse Traffic Channel frame offsets to
        ** the assigned frame offset as determined by FRAME_OFFSETs.
        ** ------------------------------------------------------------ */
        mcc_tx_buf.hdr.command     = TXC_FRAME_OFFSET_F;
        mcc_tx_buf.fo.frame_offset = cmd_ptr->demod_fch.param.frame_offset;

        /* -------------------------------------------------------------
        ** Set action_frame to zero so that command will be immediately
        ** carried out
        ** ------------------------------------------------------------ */
        qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
        mcc_txc_cmd( &mcc_tx_buf );

        #ifndef FEATURE_IS2000
        mcc_rx_buf.tci.rate_set = rate_set.forward;
        #else
        #ifdef FEATURE_IS2000_P2
        mcc_rx_buf.tci.for_fch.included = TRUE;
        mcc_rx_buf.tci.for_dcch.included = FALSE;
        #endif // FEATURE_IS2000_P2
        mcc_rx_buf.tci.for_fch.rc = (cai_radio_config_type)cmd_ptr->demod_fch.param.radio_cfg;
        mcc_rx_buf.tci.fpc.included = FALSE;
        #endif
        /* set up debug message control */
        mcc_rx_buf.tci.enable_skip_tci = TRUE;

#ifdef FEATURE_FFPC

#ifdef FEATURE_IS2000_REL_A
// if pri control chan is DCCH then DCCH is set up on channel 0.  this scenario works.
// if pri control chan is FCH then DCCH is set up on channel 1.  this scenario works 50% (failed)
//        mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_DCCH; 

        mcc_rx_buf.tci.fpc.included = TRUE;
        mcc_rx_buf.tci.fpc.fpc_mode = CAI_FPC_MODE_800_PRI;

        if (!g_ftm_fpc.dcch_has_fpc)
        {
          mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_FCH; 
          g_ftm_fpc.fch_has_fpc = TRUE;
          FTM_MSG_HIGH("FCH is FPC pri channel",0,0,0);
        }
#endif /* FEATURE_IS2000_REL_A */

        mcc_rx_buf.tci.ilpc_enabled = cmd_ptr->demod_fch.param.ffpc_enable;
        mcc_rx_buf.tci.olpc_enabled = cmd_ptr->demod_fch.param.ffpc_enable;
        mcc_rx_buf.tci.fpc.fch_olpc.init_setpt = cmd_ptr->demod_fch.param.init_setpt;
        mcc_rx_buf.tci.fpc.fpc_subchan_gain = cmd_ptr->demod_fch.param.subchan_gain;
        mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
                  cmd_ptr->demod_fch.param.olc_params.target_fer;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.set_to_current = FALSE;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value =
                  cmd_ptr->demod_fch.param.olc_params.min_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.set_to_current = FALSE;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value =
                  cmd_ptr->demod_fch.param.olc_params.max_setpt;
#endif /* FEATURE_FFPC */

        mcc_rxc_cmd( &mcc_rx_buf );

        mcc_wait( MCC_RX_RPT_Q_SIG );

        /* Check for frame reports from RXC task */

        if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
        {
          if (rpt_ptr->hdr.rpt == RXC_VALID_R)
          {
            /* Valid frame has been received */
            sub_state = TCI_3;
          }

          cmd_done( &rpt_ptr->hdr.rpt_hdr );
        }

        break;
      } /* TCI_2 */

      case TCI_3:
      {
        /* Send command to the Receive task to start receiving messages */
        mcc_rx_buf.tc.hdr.command = RXC_TC_F;
        mcc_rxc_cmd( &mcc_rx_buf );

        test_cmd_status = TEST_SUCCESS;

        ftm_fwd_curr_state = FTM_TRAFFIC;
          /* Remember the new state */

        break;
      } /* TCI_3 */

      default:
      {
        MSG_ERROR( "Invalid TCI substate %d", sub_state, 0, 0 );
      }

    } /* switch */
  } /* while */
} /* ftm_tc_init */

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
/*===========================================================================

FUNCTION FTM_DCCH_INIT

DESCRIPTION
  This function initializes the F-DCCH

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_dcch_init 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report received from Receive task */
#ifndef FEATURE_COMPLETE_SNM
  caii_srv_cfg_type mcc_srv_cfg;
    /* status configuration for requested service option */
  snm_rate_set_pair_type rate_set;
#endif /* FEATURE_COMPLET_SNM */

  enum
  {
    TCI_1,  /* State in which the Search task is commanded to go to the
               Traffic Channel */
    TCI_2,  /* State in which frame quality metrics are checked waiting
               for a valid frame */
    TCI_3   /* State in which the Receive task is commanded to go to the
               Traffic Channel */
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_1;

  /* --------------------------------------------------------
  ** Initialize primary and secondary service options to NULL
  ** to indicate that there is no active service option
  ** -------------------------------------------------------- */
  cdma.so_cur_pri = CAI_SO_NULL;
  cdma.so_cur_sec = CAI_SO_NULL;

  /* Initialize the state of the Markov model to unsynchronized */
  mar_rate = MAR_VAR_RATE;
  mar_init_synch();

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  while( ftm_fwd_curr_state != FTM_TRAFFIC )
  {
    switch( sub_state )
    {
      case TCI_1:
      {
        mcc_send_srch_parms();
          /* Sends search parameters to searcher */

        cdma.code_chan = cmd_ptr->demod_dcch.param.walsh.walsh;
        
        /* Send a message to Searcher to switch to Traffic Channel */
        mcc_srch_buf.tc.hdr.cmd = SRCH_TC_F;

        /* Send new active set to Searcher */
        mcc_srch_buf.tc.aset_cnt  = 1;

        mcc_srch_buf.tc.aset[0].pilot = cur_bs_ptr->pilot_pn;
        mcc_srch_buf.tc.aset[0].dcch_info.walsh_chn = cdma.code_chan;
        mcc_srch_buf.tc.aset[0].fch_info.active = FALSE;
        mcc_srch_buf.tc.aset[0].dcch_info.active = TRUE;

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_REL_A_TD
        mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;
        MSG_MED("Pilot = 1X common", 0, 0, 0);
/*
        if ( cmd_ptr->demod_fch.param.td_mode == 0 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;
          MSG_MED("Pilot = 1X common", 0, 0, 0);
        }
        else if ( cmd_ptr->demod_fch.param.td_mode == 1 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_power_level =
              cmd_ptr->demod_fch.param.td_power_level;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_mode = CAI_TD_MODE_OTD;
          MSG_MED("Pilot = 1X common + OTD", 0, 0, 0);
        }
        else if ( cmd_ptr->demod_fch.param.td_mode == 2 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_power_level =
              cmd_ptr->demod_fch.param.td_power_level;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_mode = CAI_TD_MODE_STS;
          MSG_MED("Pilot = 1X common + STS", 0, 0, 0);
        }
        else
        {
          MSG_ERROR("Pilot = unknown", 0, 0, 0);
        }
*/
#endif /* FEATURE_IS2000_REL_A_TD */
#endif /* FEATURE_IS2000_REL_A */

#ifdef T_MSM5000
        mcc_srch_buf.tc.aset[0].dcch_info.qof_mask_id = 
           cmd_ptr->demod_dcch.param.walsh.qof_mask_id;
#endif /* T_MSM5000 */
        mcc_srch_buf.tc.aset[0].pwr_comb_ind = 0;
          /* Flag: nonzero if the power control subchannel on this pilot
             contains the same symbols as that on the previous pilot. */

        mcc_srch_buf.tc.cdma_freq = cdma.cdmach;

        /* Calculate Traffic Channel Mask */
        if (cmd_ptr->demod_dcch.param.bypass_lc)
        {
          qw_set( mcc_srch_buf.tc.pn_mask, 0L, 0L);
          ftm_bypass_dcch_lc = TRUE;
        } 
        else 
        {
          dword p_esn;
          qw_set(mcc_srch_buf.tc.pn_mask, 0x318L,
                 (p_esn = permute_esn(ftm_nv_esn)));
          FTM_MSG_HIGH("Using pn_mask 0x318 0x%x", p_esn, 0,0);
          ftm_bypass_dcch_lc = FALSE;
        }

        mcc_srch_cmd( &mcc_srch_buf );

        /* Tell Receieve task to go to Traffic channel Initialization */
        sub_state = TCI_2;
        break;
      } /* end of case TCI_1 */

      case TCI_2:
      {
        /* Send a message to the Receive task to tune to the traffic
           channel and begin sending frame quality metrics */
        mcc_rx_buf.tci.hdr.command = RXC_TCI_F;

#ifndef FEATURE_COMPLETE_SNM
        /* Consult SOs defined in Service Negotiation Manager
           (SNM).  If the SO is not in SNM, we use the old way.  Eventually,
           all SOs should be captured in SNM. */

        if ( snm_get_rate_set( cmd_ptr->demod_dcch.param.so_pri, &rate_set ) 
             == SNM_FAILURE )
        // The following will be obsolete when all SO are captured in SNM
        {
          (void) srv_set_def_cfg( cmd_ptr->demod_dcch.param.so_pri, &mcc_srv_cfg, 
                                  FALSE );
          rate_set.forward = (dec_rate_set_type)( mcc_srv_cfg.fwd_mux - 1 );
          rate_set.reverse = (dec_rate_set_type)( mcc_srv_cfg.rev_mux - 1 );
        }
#endif
        
        /* Set frame offset for RXC */
        // rxc_set_frame_offset( cmd_ptr->demod_f_dcch.param.frame_offset );
        //
        // Sending the TXC_FRAME_OFF command to txc will set the encoder
        // up for fwd as well as reverse frame offsets
        //

        /* ------------------------------------------------------------
        ** Set the Forward and Reverse Traffic Channel frame offsets to
        ** the assigned frame offset as determined by FRAME_OFFSETs.
        ** ------------------------------------------------------------ */
        mcc_tx_buf.hdr.command     = TXC_FRAME_OFFSET_F;
        mcc_tx_buf.fo.frame_offset = cmd_ptr->demod_dcch.param.frame_offset;

        /* -------------------------------------------------------------
        ** Set action_frame to zero so that command will be immediately
        ** carried out
        ** ------------------------------------------------------------ */
        qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
        mcc_txc_cmd( &mcc_tx_buf );

        #ifndef FEATURE_IS2000
        mcc_rx_buf.tci.rate_set = rate_set.forward;
        #else
        mcc_rx_buf.tci.fpc.included = FALSE;
        mcc_rx_buf.tci.fpc.dcch_olpc.included = FALSE;
        mcc_rx_buf.tci.fpc.fch_olpc.included = FALSE;
        mcc_rx_buf.tci.for_fch.included = FALSE;
        mcc_rx_buf.tci.for_dcch.included = TRUE;
        mcc_rx_buf.tci.for_dcch.rc = (cai_radio_config_type)cmd_ptr->demod_dcch.param.radio_cfg;
        #endif
        /* set up debug message control */
        /* mcc_rx_buf.tci.enable_data_msg =
                cmd_ptr->demod_dcch.param.enable_data_msg;
        mcc_rx_buf.tci.enable_frmcnt_msg =
                cmd_ptr->demod_dcch.param.enable_frmcnt_msg;
        mcc_rx_buf.tci.enable_stats_msg =
                cmd_ptr->demod_dcch.param.enable_stats_msg;
        mcc_rx_buf.tci.enable_skip_tci =
                cmd_ptr->demod_dcch.param.disable_good_frame_check; */

#ifdef FEATURE_FFPC

#ifdef FEATURE_IS2000_REL_A
// if pri control chan is DCCH then DCCH is set up on channel 0.  this scenario works.
// if pri control chan is FCH then DCCH is set up on channel 1.  this scenario works 50% (failed)
//        mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_DCCH; 
        mcc_rx_buf.tci.fpc.included = TRUE;
        mcc_rx_buf.tci.fpc.fpc_mode = CAI_FPC_MODE_800_PRI;
          
        if (!g_ftm_fpc.fch_has_fpc)
        {
          mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_DCCH; 
          g_ftm_fpc.dcch_has_fpc = TRUE;
          FTM_MSG_HIGH("DCCH is FPC pri channel",0,0,0);
        }


#endif /* FEATURE_IS2000_REL_A */

        mcc_rx_buf.tci.ilpc_enabled = cmd_ptr->demod_dcch.param.ilpc_enabled;
        mcc_rx_buf.tci.olpc_enabled = cmd_ptr->demod_dcch.param.olpc_enabled;
        mcc_rx_buf.tci.fpc.dcch_olpc.included = mcc_rx_buf.tci.olpc_enabled;
        mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt = cmd_ptr->demod_dcch.param.init_setpt;
        mcc_rx_buf.tci.fpc.fpc_subchan_gain = cmd_ptr->demod_dcch.param.fpc_subchan_gain;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer =
                  cmd_ptr->demod_dcch.param.dcch_olc_params.target_fer;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.set_to_current = FALSE;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value =
                  cmd_ptr->demod_dcch.param.dcch_olc_params.min_setpt;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.set_to_current = FALSE;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value =
                  cmd_ptr->demod_dcch.param.dcch_olc_params.max_setpt;
#endif /* FEATURE_FFPC */

        mcc_rxc_cmd( &mcc_rx_buf );

        mcc_wait( MCC_RX_RPT_Q_SIG );

        /* Check for frame reports from RXC task */

        if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
        {
          if (rpt_ptr->hdr.rpt == RXC_VALID_R)
          {
            /* Valid frame has been received */
            sub_state = TCI_3;
          }

          cmd_done( &rpt_ptr->hdr.rpt_hdr );
        }
        
        break;
      } /* TCI_2 */

      case TCI_3:
      {
        /* Send command to the Receive task to start receiving messages */
        mcc_rx_buf.tc.hdr.command = RXC_TC_F;
        mcc_rxc_cmd( &mcc_rx_buf );

        test_cmd_status = TEST_SUCCESS;

        ftm_fwd_curr_state = FTM_TRAFFIC;
          /* Remember the new state */
                  
        break;
      } /* TCI_3 */

      default:
      {
        MSG_ERROR( "Invalid TCI substate %d", sub_state, 0, 0 );
      }

    } /* switch */
  } /* while */
} /* ftm_dcch_init */
/* End of FEATURE_FTM_DCCH */
#endif

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

void ftm_fwd_fch
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
#ifdef FEATURE_FTM_MC
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* Traffic channel initialization */
  ftm_tc_init( cmd_ptr );

  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* clear flag indicating wait for ack of start or stop continous dtmf */
  tc_ack_wait = FALSE;

  /* clear pending continous dtmf */
  tc_cont_dtmf = 0xFF;

  /* clear indicator that dtmf stop is in progress */
  tc_stop = FALSE;

  /* -------------------------------------------------
  ** Clear indicators that hard handoff is in progress
  ** ------------------------------------------------- */
  mcctcho_tc_init();
#endif /* FEATURE_FTM_MC */
} /* ftm_fwd_fch */

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
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

void ftm_fwd_dcch 
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* clear dtmf timer in case dtmf is pending */
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_CC_DTMF_TIMER_TYPE );

  /* Traffic channel initialization */
  ftm_dcch_init( cmd_ptr );

  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* clear dtmf timer in case dtmf is pending */
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_CC_DTMF_TIMER_TYPE );

  /* clear flag indicating wait for ack of start or stop continous dtmf */
  tc_ack_wait = FALSE;

  /* clear pending continous dtmf */
  tc_cont_dtmf = 0xFF;

  /* clear indicator that dtmf stop is in progress */
  tc_stop = FALSE;

  /* -------------------------------------------------
  ** Clear indicators that hard handoff is in progress
  ** ------------------------------------------------- */
  mcctcho_tc_init();

} /* ftm_fwd_dcch */
/* End of FEATURE_FTM_DCCH */
#endif

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
#ifdef FEATURE_IS2000_F_SCH
ftm_cmd_status_type ftm_tc_add_f_sch
(
   ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
#ifdef FEATURE_FTM_MC
  /* FTM should be in trafiic state */
  if(ftm_fwd_curr_state != FTM_TRAFFIC)
  {
    return TEST_INVALID_STATE;
  }

  /* Send the command to the searcher to demodulate variable rate
     spreading supplemental channel. (IS95C). */
  mcc_srch_buf.tc_sch.hdr.cmd = SRCH_TC_ADD_SCH_F;
  /* Number of supplemental channels. TIGER2 supports only one SCH */
  mcc_srch_buf.tc_sch.num_sch = 1;
  mcc_srch_buf.tc_sch.srch_tc_sch_type[0].pilot =  cur_bs_ptr->pilot_pn;
  mcc_srch_buf.tc_sch.srch_tc_sch_type[0].walsh_chn =
                                 cmd_ptr->demod_sch.param.walsh.walsh;
#ifdef T_MSM5000
  mcc_srch_buf.tc_sch.srch_tc_sch_type[0].qof_mask_id =
                                 cmd_ptr->demod_sch.param.walsh.qof_mask_id;
#endif /* T_MSM5000 */
  mcc_srch_cmd( &mcc_srch_buf);

  /* Send the command to the Receive task to set up the deinterleaver/decoder
     to decode supplemental channels. */
  mcc_rx_buf.sch.hdr.command = RXC_SCH_F;

  /* Pass in the Radio Configuration for  SCH */
  mcc_rx_buf.sch.rc = (deint_rc_type)cmd_ptr->demod_sch.param.radio_cfg;

  /* Pass in the SCH rate / spreading rate ( 2X, 4X, 8X , 16X */
  mcc_rx_buf.sch.sch_rate = (dec_sch_rate_type)cmd_ptr->demod_sch.param.sch_rate;
  mcc_rx_buf.sch.double_size_pdu = TRUE;
   mcc_rx_buf.sch.code_type = (dec_coding_type)
                              cmd_ptr->demod_sch.param.coding_type;
  mcc_rxc_cmd(&mcc_rx_buf);
#endif /* FEATURE_FTM_MC */
  return TEST_SUCCESS;
} /* ftm_tc_add_f_sch */
#endif /* FEATURE_IS2000_F_SCH */

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
)
{
#ifdef FEATURE_FTM_MC
  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */

  enum
  {
    TCI_4,  /* State in which the Transmit task is commanded to go to
               the Traffic Channel and transmit preambles */
    TCI_5   /* State in which preambles are transmitted waiting for
               a valid message from the base station */
  };

  /* Note that since the Traffic Channel must be completely initialized
     before we can send a dtmf message and we need to fully initialize before
     processing commands such as powerdown and reset we have chosen not
     to pull commands off the mc_cmd_q during this state.  We will pull
     them out in the next Traffic Channel substate.                        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_4;

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  while( ftm_rev_curr_state != FTM_REV_TRAFFIC )
  {
    switch( sub_state )
    {

      case TCI_4:
      {
        /* Set the reverse frame offset
        */
        mcc_tx_buf.hdr.command = TXC_FRAME_OFFSET_F;
        mcc_tx_buf.fo.frame_offset = cmd_ptr->mod_fch.param.frame_offset;

        /* -------------------------------------------------------------
        ** Set action_frame to zero so that command will be immediately
        ** carried out
        ** ------------------------------------------------------------ */
        qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
        mcc_txc_cmd( &mcc_tx_buf );

        /* Command TX to go to Traffic channel which will cause it to
           start sending Traffic Channel preambles */
        mcc_tx_buf.tc.hdr.command = TXC_TC_F;
        mcc_tx_buf.tc.sigs        = MCC_TXC_DONE_SIG;
        mcc_tx_buf.tc.tcb_ptr     = &mc_tcb;
        if( ftm_bypass_lc )
        {
          qw_set( mcc_tx_buf.tc.mask, 0L, 0L);
        } else {
          dword p_esn;
          qw_set(mcc_tx_buf.tc.mask, 0x318L,
                 (p_esn = permute_esn(ftm_nv_esn)));
          FTM_MSG_HIGH("Rev FCH pn_mask 0x318 0x%x", p_esn, 0,0);
        }
        #ifdef FEATURE_IS2000_P2
        mcc_tx_buf.tc.rev_fch.included = TRUE;
        /* Support FEATURE_FTM_DCCH */
        mcc_tx_buf.tc.rev_dcch.included = FALSE;
        /* End of FEATURE_FTM_DCCH */
        #endif // FEATURE_IS2000_P2
        /* Enable/Disable Reverse Power Control based on the FTM param enable_rpc */
        MSM_LOCK();    /* Lock interrupts during MSM register access */

        if (cmd_ptr->mod_fch_dcch.param.fch.enable_rpc)
        {
           /* clear the 'disable closed loop power control' dip switch */
		   bio_set_sw(BIO_GET_SW(0xFFFFFFFF) & (~BIO_SW5_M)); 

           #ifdef RF_HAS_MDSP_TX_AGC
           CDMA_MDSP_SET_TX_CLOSED_LOOP_NORMAL();
           #else    
           MSM_SET( TX_AGC_CTL2, PWR_CTL_EN );  /* Enable closed-loop power control */
           #endif /* RF_HAS_MDSP_TX_AGC */  
        }
        else
        {
           /* set the 'disable closed loop power control' dip switch */
		   bio_set_sw(BIO_SW5_M); 

           #ifdef RF_HAS_MDSP_TX_AGC
           CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE();
           #else
           MSM_CLEAR( TX_AGC_CTL2, PWR_CTL_EN );  /* Disable closed-loop power control */
           #endif /* RF_HAS_MDSP_TX_AGC */ 
        }
        MSM_UNLOCK();    /* Unlock interrupts */
        mcc_tx_buf.tc.rev_fch.rc = (cai_radio_config_type)
                                      cmd_ptr->mod_fch.param.radio_cfg;

        ftm_rev_fch_rate = 
			mcc_tx_buf.tc.fch_rate = (enc_rate_type)
                                    cmd_ptr->mod_fch.param.fch_rate;
        mcc_tx_buf.tc.sch_on = FALSE;
        mcc_tx_buf.tc.num_preambles = cmd_ptr->mod_fch.param.num_preambles;
        mcc_tx_buf.tc.tx_pattern = cmd_ptr->mod_fch.param.tx_pattern;

        (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
        mcc_txc_cmd( &mcc_tx_buf );

        /* Wait for TX to acquire timing & go to traffic state */
        (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
        (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

        FTM_MSG_HIGH( "Turned on Reverse Link traffic", 0, 0, 0 );

        sub_state = TCI_5;
        test_cmd_status = TEST_SUCCESS;
        ftm_rev_curr_state = FTM_REV_TRAFFIC;
        break;
      } /* case TCI_4 */

      default:
      {
        ERR_FATAL( "Invalid TCI substate %d", sub_state, 0, 0 );
      }

    } /* switch */
  } /* while */
#endif /* FEATURE_FTM_MC */
} /* ftm_rev_tc_init */

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
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
)
{

  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */

  enum
  {
    TCI_4,  /* State in which the Transmit task is commanded to go to
               the Traffic Channel and transmit preambles */
    TCI_5   /* State in which preambles are transmitted waiting for
               a valid message from the base station */
  };

  /* Note that since the Traffic Channel must be completely initialized
     before we can send a dtmf message and we need to fully initialize before
     processing commands such as powerdown and reset we have chosen not
     to pull commands off the mc_cmd_q during this state.  We will pull
     them out in the next Traffic Channel substate.                        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_4;

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  while( ftm_rev_curr_state != FTM_REV_TRAFFIC )
  {
    switch( sub_state )
    {

      case TCI_4:
      {
        /* Set the reverse frame offset
        */ 
        mcc_tx_buf.hdr.command = TXC_FRAME_OFFSET_F;
        mcc_tx_buf.fo.frame_offset = cmd_ptr->mod_dcch.param.frame_offset;

        /* -------------------------------------------------------------
        ** Set action_frame to zero so that command will be immediately
        ** carried out
        ** ------------------------------------------------------------ */
        qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
        mcc_txc_cmd( &mcc_tx_buf );

        /* Command TX to go to Traffic channel which will cause it to
           start sending Traffic Channel preambles */
        mcc_tx_buf.tc.hdr.command = TXC_TC_F;
        mcc_tx_buf.tc.sigs        = MCC_TXC_DONE_SIG;
        mcc_tx_buf.tc.tcb_ptr     = &mc_tcb;
        
        if( ftm_bypass_dcch_lc )
        {
          qw_set( mcc_tx_buf.tc.mask, 0L, 0L);
        } 
        else 
        {
          dword p_esn;
          qw_set(mcc_tx_buf.tc.mask, 0x318L,
                 (p_esn = permute_esn(ftm_nv_esn)));
          FTM_MSG_HIGH("Rev DCCH pn_mask 0x318 0x%x", p_esn, 0,0);
        }

        mcc_tx_buf.tc.rev_dcch.rc = (cai_radio_config_type)
                                      cmd_ptr->mod_dcch.param.dcch_rc;

        mcc_tx_buf.tc.dcch_rate = ENC_FULL_RATE;

        mcc_tx_buf.tc.rev_fch.included = FALSE;
        mcc_tx_buf.tc.rev_dcch.included = TRUE;

        mcc_tx_buf.tc.sch_on = cmd_ptr->mod_dcch.param.sch_on;
        if( mcc_tx_buf.tc.sch_on )
        {
           mcc_tx_buf.tc.rev_sch_rc = (cai_radio_config_type) cmd_ptr->mod_dcch.param.sch_rc;
           mcc_tx_buf.tc.sch_rate = (enc_sch_rate_type)
                                       cmd_ptr->mod_dcch.param.sch_rate;
           mcc_tx_buf.tc.sch_turbo = cmd_ptr->mod_dcch.param.sch_turbo;
           mcc_tx_buf.tc.sch_ltu_size = cmd_ptr->mod_dcch.param.sch_ltu_size;
        }
        mcc_tx_buf.tc.num_preambles = cmd_ptr->mod_dcch.param.num_preambles;
        mcc_tx_buf.tc.tx_pattern = cmd_ptr->mod_dcch.param.tx_pattern;
        mcc_tx_buf.tc.rev_fch_gating_on = FALSE;

        (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
        mcc_txc_cmd( &mcc_tx_buf );

        /* Wait for TX to acquire timing & go to traffic state */
        (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
        (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

        FTM_MSG_HIGH( "Turned on Reverse Link traffic", 0, 0, 0 );

        sub_state = TCI_5;
        test_cmd_status = TEST_SUCCESS;
        ftm_rev_curr_state = FTM_REV_TRAFFIC;
        break;
      } /* case TCI_4 */

      default:
      {
        ERR_FATAL( "Invalid TCI substate %d", sub_state, 0, 0 );
      }

    } /* switch */
  } /* while */
} /* ftm_rev_dcch_init */
/* End of FEATURE_FTM_DCCH */
#endif

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================

FUNCTION FTM_REV_SCH_INIT

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_rev_sch_init
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  /* Command TX to go to Traffic channel which will cause it to
     start sending Traffic Channel preambles */
  mcc_tx_buf.ftm_sch.hdr.command = TXC_FTM_SCH_F;
/*  mcc_tx_buf.ftm_sch.hdr.cmd_hdr.sigs        = MCC_TXC_DONE_SIG;
  mcc_tx_buf.ftm_sch.hdr.cmd_hdr.task_ptr     = rex_self(); */
  mcc_tx_buf.ftm_sch.radio_cfg = (cai_radio_config_type)
                                cmd_ptr->mod_sch.param.radio_cfg;

  mcc_tx_buf.ftm_sch.sch_rate = (enc_rate_type)
                              cmd_ptr->mod_sch.param.sch_rate;
  mcc_tx_buf.ftm_sch.sch_on = TRUE;
  mcc_tx_buf.ftm_sch.sch_turbo =
       ((ftm_coding_type)cmd_ptr->mod_sch.param.coding_type == FTM_DECODE_TURBO)?
       TRUE : FALSE;
  mcc_tx_buf.ftm_sch.tx_pattern = (dword)cmd_ptr->mod_sch.param.tx_pattern;

  (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
  mcc_txc_cmd( &mcc_tx_buf );

  /* Wait for TX to acquire timing & go to traffic state */
  // (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
  // (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

  FTM_MSG_HIGH( "Turned on Reverse SCH traffic", 0, 0, 0 );

  test_cmd_status = TEST_SUCCESS;

} /* ftm_rev_sch_init */
#endif /* FEATURE_IS2000_R_SCH */

/*===========================================================================

FUNCTION FTM_FCH_LOOPBACK

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_fch_loopback
(
  boolean lb_enable
    /* enable loopback */
)
{
  /* inform RXC of loopback status */
  mcc_rx_buf.loopback.hdr.command = RXC_FTM_FCH_LOOPBACK;
  mcc_rx_buf.loopback.loopback_on = lb_enable;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* inform TXC of loopback status */
  mcc_tx_buf.loopback.hdr.command = TXC_FTM_FCH_LOOPBACK;
  mcc_tx_buf.loopback.loopback_on = lb_enable;
  (void)rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
  mcc_txc_cmd( &mcc_tx_buf );

  test_cmd_status = TEST_SUCCESS;
}

#ifdef FEATURE_IS2000_P2
/* Support FEATURE_FTM_DCCH */
/*===========================================================================

FUNCTION FTM_DCCH_LOOPBACK

DESCRIPTION
  This function modulates Dedicated Control Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_dcch_loopback
(
  boolean lb_enable
    /* enable loopback */
)
{
  /* inform RXC of loopback status */
  mcc_rx_buf.loopback.hdr.command = RXC_FTM_DCCH_LOOPBACK;
  mcc_rx_buf.loopback.loopback_on = lb_enable;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* inform TXC of loopback status */
  mcc_tx_buf.loopback.hdr.command = TXC_FTM_DCCH_LOOPBACK;
  mcc_tx_buf.loopback.loopback_on = lb_enable;
  (void)rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
  mcc_txc_cmd( &mcc_tx_buf );

  test_cmd_status = TEST_SUCCESS;
}
/* End of FEATURE_FTM_DCCH */
#endif

#if (defined(FEATURE_IS2000_F_SCH) && defined(FEATURE_IS2000_R_SCH))
/*===========================================================================

FUNCTION FTM_SCH_LOOPBACK

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_sch_loopback
(
  boolean lb_enable
    /* enable loopback */
)
{
  /* inform RXC of loopback status */
  mcc_rx_buf.loopback.hdr.command = RXC_FTM_SCH_LOOPBACK;
  mcc_rx_buf.loopback.loopback_on = lb_enable;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* inform TXC of loopback status */
  mcc_tx_buf.loopback.hdr.command = TXC_FTM_SCH_LOOPBACK;
  mcc_tx_buf.loopback.loopback_on = lb_enable;
  (void)rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
  mcc_txc_cmd( &mcc_tx_buf );

  test_cmd_status = TEST_SUCCESS;
}
#endif /* FEATURE_IS2000_F_SCH && FEATURE_IS2000_R_SCH */

/*===========================================================================

FUNCTION FTM_ENTER_ONLINE_DIGITAL

DESCRIPTION
  This is a special much simplified version of online digital processing for
  FTM.

DEPENDENCIES
  None.

RETURN VALUE
  The exit state to go to.

SIDE EFFECTS
  None.

===========================================================================*/

#ifdef FEATURE_SD20
mc_state_type ftm_enter_online_digital(void)
{
  (void) rex_clr_timer(&mc_rpt_timer);
  (void) rex_clr_sigs(&mc_tcb, MC_RPT_TIMER_SIG);
  
  mc_update_last_cdma_entry(MCC_ONLINE);

  /* Print the acquisition mode and channel
   */
  db_get(DB_ACQ_MODE, &ftm_db_item);
  switch (ftm_db_item.acq_mode)
  {
    case DB_ACQ_FULL :
      MSG_LOW("Full scan of CDMA channel %d",
        ftm_cdma_entry.gen.cdma_channel, 0, 0);
      break;

    case DB_ACQ_MINI :
      MSG_LOW("Mini scan of CDMA channel %d",
        ftm_cdma_entry.gen.cdma_channel, 0, 0);
      break;

    case DB_ACQ_MICRO :
      MSG_LOW("Micro scan of CDMA channel %d",
        ftm_cdma_entry.gen.cdma_channel, 0, 0);
      break;

    default :
      ERR("Illegal acq_mode: %d", ftm_db_item.acq_mode, 0, 0);
      break;
  }

  /* Call the MCC subtask with the digital entry reason
   */
  (void) *test_mcc(&ftm_cdma_entry);

  /* we've got to return something, may as well make it a reset */
  return (MC_RESET_STATE);
} /* ftm_enter_online_digital */
#else /* FEATURE_SD20 */
mcsyspr_state_enum_type ftm_enter_online_digital(void)
{
  (void) rex_clr_timer(&mc_rpt_timer);
  (void) rex_clr_sigs(&mc_tcb, MC_RPT_TIMER_SIG);

  mcsyspr_last_entry_params.cdma = mcsyspr_cdma_entry.entry;

  /* Print the acquisition mode and channel
   */
  db_get(DB_ACQ_MODE, &mcsyspr_db_item);
  switch (mcsyspr_db_item.acq_mode)
  {
    case DB_ACQ_FULL :
      MSG_LOW("Full scan of CDMA channel %d",
        mcsyspr_cdma_entry.param.acq.cdma_channel, 0, 0);
      break;

    case DB_ACQ_MINI :
      MSG_LOW("Mini scan of CDMA channel %d",
        mcsyspr_cdma_entry.param.acq.cdma_channel, 0, 0);
      break;

    case DB_ACQ_MICRO :
      MSG_LOW("Micro scan of CDMA channel %d",
        mcsyspr_cdma_entry.param.acq.cdma_channel, 0, 0);
      break;

    default :
      ERR("Illegal acq_mode: %d", mcsyspr_db_item.acq_mode, 0, 0);
      break;
  }

  /* Show that a system hasn't been selected yet
   */
  mcsyspr_system_selected = FALSE;

  /* Call the MCC subtask with the digital entry reason
   */
  mcsyspr_cdma_exit = *test_mcc(&mcsyspr_cdma_entry);

  /* we've got to return something, may as well make it a reset */
  return (MCSYSPR_RESET_STATE);
} /* mcsysprd_enter_online_digital */
#endif /* FEATURE_SD20 */

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
)
{
#ifdef FEATURE_SD20
  mc_state_type next_state;     /* Next state to transition to */
#else
  mcsyspr_state_enum_type next_state; /* Next state to transition to */
#endif /* FEATURE_SD20 */



#ifdef FEATURE_SD20 
  ftm_db_item.acq_mode = DB_ACQ_MINI;
  db_put(DB_ACQ_MODE, &ftm_db_item);

  ftm_cdma_entry.entry = MCC_ACQUISITION;
  ftm_cdma_entry.gen.new_mode_or_blksys = TRUE;
  next_state = MC_ONLINE_DIGITAL_STATE;

  while (next_state == MC_ONLINE_DIGITAL_STATE)
  {
    FTM_MSG_HIGH("Going online digital", 0, 0, 0);
    ftm_db_item.dmss_state = DB_DMSS_ONLINE_DIGITAL_S;
    db_put(DB_DMSS_STATE, &ftm_db_item);
    next_state = ftm_enter_online_digital();
  } 
#else /* FEATURE_SD20 */
  /*Initialize the phone to the unlocked mode*/
  mcsyspr_cdma_locked = FALSE;

  /* Set the Mini_Scan to True */
  mcsyspr_use_mini_scan = TRUE;
  SET_DB_ACQ_MODE(DB_ACQ_MINI);

  mcsyspr_cdma_entry.entry = MCC_ACQUISITION;
  mcsyspr_cdma_entry.param.acq.new_mode_or_sys = TRUE;
  next_state = MCSYSPR_ONLINE_DIGITAL_STATE;

  /* Set the REDIRECTIONs variable to disabled
  */
  mcsyspr_redirection_enabled = FALSE;

  while (next_state == MCSYSPR_ONLINE_DIGITAL_STATE)
  {
    FTM_MSG_HIGH("Going online digital", 0, 0, 0);
    mcsyspr_db_item.dmss_state = DB_DMSS_ONLINE_DIGITAL_S;
    db_put(DB_DMSS_STATE, &mcsyspr_db_item);
    next_state = ftm_enter_online_digital();
  }
#endif /* FEATURE_SD20 */

  return(MC_RESET_F);
} /* ftm_system_determination */

/*===========================================================================

FUNCTION FTM_FWD_TC_RELEASE

DESCRIPTION
  This function issues RXC a release command when leaving the TC state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_fwd_tc_release(void)
{
  /* Issue RXC a RELEASE Command to release resources in TC state */
  mcc_rx_buf.rel.hdr.command = RXC_RELEASE_F;
  mcc_rxc_cmd( &mcc_rx_buf );

#ifdef FEATURE_IS2000_REL_A
  g_ftm_fpc.fch_has_fpc = FALSE;
  g_ftm_fpc.dcch_has_fpc = FALSE;
#endif /* FEATURE_IS2000_REL_A */
}

/*===========================================================================

FUNCTION FTM_REV_TC_RELEASE

DESCRIPTION
  This function issues TXC a release command when leaving the TC state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_rev_tc_release(void)
{
  /* Issue TXC a RELEASE Command to release resources in TC state */
  mcc_tx_buf.rel.hdr.command = TXC_RELEASE_F;
  mcc_txc_cmd( &mcc_tx_buf );
}

/*===========================================================================

FUNCTION ftm_tmc_init

DESCRIPTION
  ftm initializaiton that has to be done in TMC task.

DEPENDENCIES
  tmc task has already started.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tmc_init(void)
{
  ftmdiag_init();                     /* register ftm diag dispatch table. */
}

#ifdef FEATURE_IS2000_P2
/*===========================================================================

FUNCTION FTM_FCH_DCCH

DESCRIPTION
  This function demodulates the F-FCH and F-DCCH

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ftm_fch_dcch
(
  ftm_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report received from Receive task */
#ifndef FEATURE_COMPLETE_SNM
  caii_srv_cfg_type mcc_srv_cfg;
    /* status configuration for requested service option */
  snm_rate_set_pair_type rate_set;
#endif

  enum
  {
    TCI_1,  /* State in which the Search task is commanded to go to the
               Traffic Channel */
    TCI_2,  /* State in which frame quality metrics are checked waiting
               for a valid frame */
    TCI_3   /* State in which the Receive task is commanded to go to the
               Traffic Channel */
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_1;

  /* --------------------------------------------------------
  ** Initialize primary and secondary service options to NULL
  ** to indicate that there is no active service option
  ** -------------------------------------------------------- */
  cdma.so_cur_pri = CAI_SO_NULL;
  cdma.so_cur_sec = CAI_SO_NULL;

  /* Initialize the state of the Markov model to unsynchronized */
  mar_rate = MAR_VAR_RATE;
  mar_init_synch();

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  while( ftm_fwd_curr_state != FTM_TRAFFIC )
  {
    switch( sub_state )
    {
      case TCI_1:
      {
        mcc_send_srch_parms();
          /* Sends search parameters to searcher */

        /* vlechan what should cdma.code_chan be set up to for VP2? */
        cdma.code_chan = cmd_ptr->demod_fch_dcch.param.fch.walsh.walsh;
        
        /* Send a message to Searcher to switch to Traffic Channel */
        mcc_srch_buf.tc.hdr.cmd = SRCH_TC_F;

        /* Send new active set to Searcher */
        mcc_srch_buf.tc.aset_cnt  = 1;

        mcc_srch_buf.tc.aset[0].pilot = cur_bs_ptr->pilot_pn;

        mcc_srch_buf.tc.aset[0].fch_info.walsh_chn = 
          cmd_ptr->demod_fch_dcch.param.fch.walsh.walsh;
        mcc_srch_buf.tc.aset[0].dcch_info.walsh_chn = 
          cmd_ptr->demod_fch_dcch.param.dcch.walsh.walsh;
#ifdef FEATURE_IS2000_P2
        mcc_srch_buf.tc.aset[0].fch_info.active = TRUE;
        mcc_srch_buf.tc.aset[0].dcch_info.active = TRUE;
#endif 

#ifdef FEATURE_IS2000_REL_A
#ifdef FEATURE_IS2000_REL_A_TD
        mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;
        MSG_MED("Pilot = 1X common", 0, 0, 0);
/*
        if ( cmd_ptr->demod_fch_dcch.param.fch.td_mode == 0 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;
          FTM_MSG_HIGH("Pilot = 1X common", 0, 0, 0);
        }
        else if ( cmd_ptr->demod_fch_dcch.param.fch.td_mode == 1 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_power_level =
              cmd_ptr->demod_fch_dcch.param.fch.td_power_level;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_mode = CAI_TD_MODE_OTD;
          FTM_MSG_HIGH("Pilot = 1X common + OTD", 0, 0, 0);
        }
        else if ( cmd_ptr->demod_fch_dcch.param.fch.td_mode == 2 )
        {
          mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_power_level =
              cmd_ptr->demod_fch_dcch.param.fch.td_power_level;
          mcc_srch_buf.tc.aset[0].pilot_rec.rec0.td_mode = CAI_TD_MODE_STS;
          FTM_MSG_HIGH("Pilot = 1X common + STS", 0, 0, 0);
        }
        else
        {
          FTM_MSG_ERROR("Pilot = unknown", 0, 0, 0);
        }
*/        
#endif /* FEATURE_IS2000_REL_A_TD */
#endif /* FEATURE_IS2000_REL_A */

#ifdef T_MSM5000
        mcc_srch_buf.tc.aset[0].dcch_info.qof_mask_id = 
           cmd_ptr->demod_fch_dcch.param.dcch.walsh.qof_mask_id;
        mcc_srch_buf.tc.aset[0].fch_info.qof_mask_id = 
           cmd_ptr->demod_fch_dcch.param.fch.walsh.qof_mask_id;
#endif /* T_MSM5000 */
        mcc_srch_buf.tc.aset[0].pwr_comb_ind = 0;
          /* Flag: nonzero if the power control subchannel on this pilot
             contains the same symbols as that on the previous pilot. */

        mcc_srch_buf.tc.cdma_freq = cdma.cdmach;

        /* Calculate Traffic Channel Mask */
        if (cmd_ptr->demod_fch.param.bypass_lc || cmd_ptr->demod_dcch.param.bypass_lc)
        {
          qw_set( mcc_srch_buf.tc.pn_mask, 0L, 0L);
          ftm_bypass_lc = TRUE;
          ftm_bypass_dcch_lc = TRUE;
        } else {
          dword p_esn;
          qw_set(mcc_srch_buf.tc.pn_mask, 0x318L,
                 (p_esn = permute_esn(ftm_nv_esn)));
          FTM_MSG_HIGH("Using pn_mask 0x318 0x%x", p_esn, 0,0);
          ftm_bypass_lc = FALSE;
        }

        mcc_srch_cmd( &mcc_srch_buf );

        /* Tell Receieve task to go to Traffic channel Initialization */
        sub_state = TCI_2;
        break;
      } /* end of case TCI_1 */

      case TCI_2:
      {
        /* Send a message to the Receive task to tune to the traffic
           channel and begin sending frame quality metrics */
        mcc_rx_buf.tci.hdr.command = RXC_TCI_F;

#ifndef FEATURE_COMPLETE_SNM
        /* Consult SOs defined in Service Negotiation Manager
           (SNM).  If the SO is not in SNM, we use the old way.  Eventually,
           all SOs should be captured in SNM. */

        if (snm_get_rate_set( cmd_ptr->demod_fch_dcch.param.fch.so_pri, 
                              &rate_set
                            ) 
            == SNM_FAILURE 
           )
        // The following will be obsolete when all SO are captured in SNM
        {
          (void) srv_set_def_cfg
            ( 
              cmd_ptr->demod_fch_dcch.param.fch.so_pri, &mcc_srv_cfg, 
              FALSE 
            );
          rate_set.forward = (dec_rate_set_type)( mcc_srv_cfg.fwd_mux - 1 );
          rate_set.reverse = (dec_rate_set_type)( mcc_srv_cfg.rev_mux - 1 );
        }
#endif
        
        /* Set frame offset for RXC */
        // rxc_set_frame_offset( cmd_ptr->demod_f_dcch.param.frame_offset );
        //
        // Sending the TXC_FRAME_OFF command to txc will set the encoder
        // up for fwd as well as reverse frame offsets
        //

        /* ------------------------------------------------------------
        ** Set the Forward and Reverse Traffic Channel frame offsets to
        ** the assigned frame offset as determined by FRAME_OFFSETs.
        ** ------------------------------------------------------------ */
        mcc_tx_buf.hdr.command     = TXC_FRAME_OFFSET_F;
        mcc_tx_buf.fo.frame_offset = 
          cmd_ptr->demod_fch_dcch.param.fch.frame_offset;

        /* -------------------------------------------------------------
        ** Set action_frame to zero so that command will be immediately
        ** carried out
        ** ------------------------------------------------------------ */
        qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
        mcc_txc_cmd( &mcc_tx_buf );

        #ifndef FEATURE_IS2000
        mcc_rx_buf.tci.rate_set = rate_set.forward;
        #else
        mcc_rx_buf.tci.fpc.included = TRUE;
        mcc_rx_buf.tci.fpc.dcch_olpc.included = TRUE;
        mcc_rx_buf.tci.for_dcch.included = TRUE;
        mcc_rx_buf.tci.for_dcch.rc =
          (cai_radio_config_type)cmd_ptr->demod_fch_dcch.param.dcch.radio_cfg;
        mcc_rx_buf.tci.fpc.fch_olpc.included = TRUE;
        mcc_rx_buf.tci.for_fch.included = TRUE;
        mcc_rx_buf.tci.for_fch.rc =
          (cai_radio_config_type)cmd_ptr->demod_fch_dcch.param.fch.radio_cfg;
        #endif
        
        /* set up debug message control */
/*
        mcc_rx_buf.tci.enable_data_msg =
                cmd_ptr->demod_fch_dcch.param.fch. .enable_data_msg;
        mcc_rx_buf.tci.enable_frmcnt_msg =
                cmd_ptr->demod_fch_dcch.param.fch.enable_frmcnt_msg;
        mcc_rx_buf.tci.enable_stats_msg =
                cmd_ptr->demod_fch_dcch.param.fch.enable_stats_msg;
        mcc_rx_buf.tci.enable_skip_tci =
                cmd_ptr->demod_fch_dcch.param.fch.disable_good_frame_check;
*/                

#ifdef FEATURE_FFPC
        mcc_rx_buf.tci.ilpc_enabled = cmd_ptr->demod_fch_dcch.param.fch.ffpc_enable;
        mcc_rx_buf.tci.olpc_enabled = cmd_ptr->demod_fch_dcch.param.fch.ffpc_enable;
        mcc_rx_buf.tci.fpc.fpc_subchan_gain = 
          cmd_ptr->demod_fch_dcch.param.fch.subchan_gain;
        mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_FCH; 
        mcc_rx_buf.tci.fpc.fch_olpc.included = mcc_rx_buf.tci.olpc_enabled;
        mcc_rx_buf.tci.fpc.fch_olpc.init_setpt = 
          cmd_ptr->demod_fch_dcch.param.fch.init_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
          cmd_ptr->demod_fch_dcch.param.fch.olc_params.target_fer;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value =
          cmd_ptr->demod_fch_dcch.param.fch.olc_params.min_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value =
          cmd_ptr->demod_fch_dcch.param.fch.olc_params.max_setpt;

        mcc_rx_buf.tci.fpc.dcch_olpc.included = FALSE;
        mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt = 0;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer = 0;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value = 0;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value = 0;
#endif /* FEATURE_FFPC */

        mcc_rxc_cmd( &mcc_rx_buf );

        mcc_wait( MCC_RX_RPT_Q_SIG );

        /* Check for frame reports from RXC task */

        if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
        {
          if (rpt_ptr->hdr.rpt == RXC_VALID_R)
          {
            /* Valid frame has been received */
            sub_state = TCI_3;
          }

          cmd_done( &rpt_ptr->hdr.rpt_hdr );
        }
        
        break;
      } /* TCI_2 */

      case TCI_3:
      {
        /* Send command to the Receive task to start receiving messages */
        mcc_rx_buf.tc.hdr.command = RXC_TC_F;
        mcc_rxc_cmd( &mcc_rx_buf );

        test_cmd_status = TEST_SUCCESS;

        ftm_fwd_curr_state = FTM_TRAFFIC;
          /* Remember the new state */
                  
        break;
      } /* TCI_3 */

      default:
      {
        MSG_ERROR( "Invalid TCI substate %d", sub_state, 0, 0 );
      }

    } /* switch */
  } /* while */
} /* End ftm_fch_dcch */

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
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS2000_REL_A
  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID);

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID);

  /* clear dtmf timer in case dtmf is pending */
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID );

  /* Simultaneous FCH & DCCH channel demodulation */
  ftm_fch_dcch ( cmd_ptr );

  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID);

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID);

  /* clear dtmf timer in case dtmf is pending */
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID );
#endif /* FEATURE_IS2000_REL_A */

  /* clear flag indicating wait for ack of start or stop continous dtmf */
  tc_ack_wait = FALSE;

  /* clear pending continous dtmf */
  tc_cont_dtmf = 0xFF;

  /* clear indicator that dtmf stop is in progress */
  tc_stop = FALSE;

  /* -------------------------------------------------
  ** Clear indicators that hard handoff is in progress
  ** ------------------------------------------------- */
  mcctcho_tc_init();

} /* End ftm_fwd_fch_dcch */

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
)
{

  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */

  enum
  {
    TCI_4,  /* State in which the Transmit task is commanded to go to
               the Traffic Channel and transmit preambles */
    TCI_5   /* State in which preambles are transmitted waiting for
               a valid message from the base station */
  };

  /* Note that since the Traffic Channel must be completely initialized
     before we can send a dtmf message and we need to fully initialize before
     processing commands such as powerdown and reset we have chosen not
     to pull commands off the mc_cmd_q during this state.  We will pull
     them out in the next Traffic Channel substate.                        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_4;

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  while( ftm_rev_curr_state != FTM_REV_TRAFFIC )
  {
    switch( sub_state )
    {

      case TCI_4:
      {
        /* Set the reverse frame offset
        */ 
        mcc_tx_buf.hdr.command = TXC_FRAME_OFFSET_F;
        mcc_tx_buf.fo.frame_offset = cmd_ptr->mod_fch_dcch.param.fch.frame_offset;

        /* -------------------------------------------------------------
        ** Set action_frame to zero so that command will be immediately
        ** carried out
        ** ------------------------------------------------------------ */
        qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
        mcc_txc_cmd( &mcc_tx_buf );

        /* Command TX to go to Traffic channel which will cause it to
           start sending Traffic Channel preambles */
        mcc_tx_buf.tc.hdr.command = TXC_TC_F;
        mcc_tx_buf.tc.sigs        = MCC_TXC_DONE_SIG;
        mcc_tx_buf.tc.tcb_ptr     = &mc_tcb;
        
        if( ftm_bypass_dcch_lc || ftm_bypass_lc)
        {
          qw_set( mcc_tx_buf.tc.mask, 0L, 0L);
        } 
        else 
        {
          dword p_esn;
          qw_set(mcc_tx_buf.tc.mask, 0x318L,
                 (p_esn = permute_esn(ftm_nv_esn)));
          FTM_MSG_HIGH("Rev FCH_DCCH pn_mask 0x318 0x%x", p_esn, 0,0);
        }

        mcc_tx_buf.tc.rev_fch.included = TRUE;
        mcc_tx_buf.tc.rev_dcch.included = TRUE;

        mcc_tx_buf.tc.rev_fch.rc = (cai_radio_config_type) cmd_ptr->mod_fch_dcch.param.fch.radio_cfg;
        mcc_tx_buf.tc.rev_dcch.rc = (cai_radio_config_type) cmd_ptr->mod_fch_dcch.param.dcch.dcch_rc;

        ftm_rev_fch_rate = mcc_tx_buf.tc.fch_rate = (enc_rate_type) cmd_ptr->mod_fch_dcch.param.fch.fch_rate;
        mcc_tx_buf.tc.dcch_rate = ENC_FULL_RATE;

        mcc_tx_buf.tc.sch_on = cmd_ptr->mod_fch_dcch.param.dcch.sch_on;
        if( mcc_tx_buf.tc.sch_on )
        {
           mcc_tx_buf.tc.rev_sch_rc = (cai_radio_config_type) cmd_ptr->mod_dcch.param.sch_rc;
           mcc_tx_buf.tc.sch_rate = (enc_sch_rate_type)
                                       cmd_ptr->mod_dcch.param.sch_rate;
           mcc_tx_buf.tc.sch_turbo = cmd_ptr->mod_dcch.param.sch_turbo;
           mcc_tx_buf.tc.sch_ltu_size = cmd_ptr->mod_dcch.param.sch_ltu_size;
        }
        mcc_tx_buf.tc.num_preambles = cmd_ptr->mod_fch_dcch.param.fch.num_preambles;
        mcc_tx_buf.tc.tx_pattern = cmd_ptr->mod_fch_dcch.param.fch.tx_pattern;
        mcc_tx_buf.tc.rev_fch_gating_on = FALSE;

        (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
        mcc_txc_cmd( &mcc_tx_buf );

        /* Wait for TX to acquire timing & go to traffic state */
        (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
        (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

        FTM_MSG_HIGH( "Turned on Reverse Link traffic", 0, 0, 0 );

        sub_state = TCI_5;
        test_cmd_status = TEST_SUCCESS;
        ftm_rev_curr_state = FTM_REV_TRAFFIC;
        break;
      } /* case TCI_4 */

      default:
      {
        ERR_FATAL( "Invalid TCI substate %d", sub_state, 0, 0 );
      }

    } /* switch */
  } /* while */
} /* Edn ftm_rev_fch_dcch_init */

#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
#endif /* FEATURE_FACTORY_TESTMODE */

