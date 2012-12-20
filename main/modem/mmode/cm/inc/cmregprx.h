#ifndef CMREGPRX_H
#define CMREGPRX_H
/*===========================================================================

          R E G P R O X Y    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Multi-Mode
  Controller ( MMoC ) and CM/REG to interface with the RegProxy module.

Copyright (c) 2002 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmregprx.h#1 $

when       who     what, where, why
--------   ---    -------------------------------------------------------
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning 
                   pattern to re-acquire service
07/03/06   ic     Cleaned up leftovers from mainlining FEATURE_MULTIMODE_ARCH
04/01/06   ka     Adding sys to reg mapping function for net_list_cnf status.
11/30/05   ka     Adding api declaration cmregprx_send_hplmn_srch_req().
11/14/05   ic     Lint80 cleanup 
11/08/05   sk     Separated CM reports and commands.
05/20/05   ic     Mainlined FEATURE_SD20
03/03/05   ka     Adding support to let mmoc know of protocol that has a
                  subs change when calling mmoc_cmd_subscription_chg
10/21/04   sj     Changed throttling mechanism for CM_SERVICE_CNF.
                  Removed handling of unused sanity timer mechanism.
02/06/04   ws     Initial jaguar/mainline merge.
08/20/03   SJ     Added srv cnf hold period, if srv cnf comes back within
                  1 second.
01/21/03   ws     Updated copyright information for 2003
04/22/02   sj     Initial release.

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "cmd.h"
#include "sd.h"
#include "prot.h"
#include "cmwll.h"


/*===========================================================================
=============================================================================
=============================================================================
============================== MMoC -> REGPROXY commands ====================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_cmd_gw_activate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA
  needs to be activated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gw_activate_protocol
(
        prot_act_e_type                act_reason
           /* Reason for activating the protocol.
           */

);


/*===========================================================================

FUNCTION cmregprx_cmd_gw_deactivate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA
  needs to be deactivated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gw_deactivate_protocol
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the protocol.
           */

        prot_trans_type                trans_id
           /* Trans_id for the deactivate protocol request.
           */

);


/*===========================================================================

FUNCTION cmregprx_cmd_gw_generic_cmd

DESCRIPTION
  This function should be called when the generic command needs to be
  processed by the RegProxy. The generic command could be because of Pref.
  system selection change, reselection time out, Get GSM/WCDMA networks list
  or Terminate Get GSM/WCDMA networks list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gw_generic_cmd
(
        const prot_gen_cmd_s_type            *gen_cmd_ptr
           /* Pointer to the generic protocol command.
           */
);


/*===========================================================================

FUNCTION cmregprx_cmd_gw_ph_stat_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the phone status
  changed. The phone status change could be because of operating mode change
  or subscription change.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gw_ph_stat_chgd
(

        prot_ph_stat_chg_e_type        chg_type,
           /* Type of the phone status change.
           */

        boolean                        is_gw_subs_avail,
           /* Current GSM/WCDMA subscription available flag.
           */

        prot_oprt_mode_e_type          oprt_mode,
           /* Current operating mode of the phone.
           */

        prot_trans_type                trans_id,
           /* Transaction id for the phone status change command.
           */

        prot_subsc_chg_e_type          prot_subsc_chg
           /* Enumeration of protocols that have a change in
           ** subscription available status
           */
);


/*===========================================================================

FUNCTION cmregprx_cmd_proc

DESCRIPTION
  This function should be called when the REX signal corresponding to
  RegProxy's command queue is set. This function will be responsible for
  dequeuing and getting the command processed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_proc
(
        void

);




/*===========================================================================
=============================================================================
=============================================================================
============================== REG -> REGPROXY reports ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_rpt_proc

DESCRIPTION
  This function should be called to process the reports from REG like
  cm_service_cnf, cm_service_ind , cm_network_list_cnf etc. This function
  would be responsible for processing the REG report by calling the
  appropriate state handlers.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                        cmregprx_rpt_proc
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */

);


/*===========================================================================
=============================================================================
=============================================================================
============================== Timer handler ================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_timer_proc

DESCRIPTION
  This function should be called to process the CM's timer tick.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
     void

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                        cmregprx_timer_proc
(
        void
);



/*===========================================================================
=============================================================================
=============================================================================
============================== CM -> REGPROXY functional interface ==========
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_proc_gw_resel_not_ok

DESCRIPTION
  This function should be called to inform the RegProxy that the GSM/WCDMA
  protocol is busy and RegProxy should not perform any reselection.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_proc_gw_resel_not_ok
(
        void
);


/*===========================================================================

FUNCTION cmregprx_proc_gw_resel_ok

DESCRIPTION
  This function should be called to inform the RegProxy that the GSM/WCDMA
  protocol has completed the system access and RegProxy can perform
  reselection if needed.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_proc_gw_resel_ok
(
        void
);




/*===========================================================================

FUNCTION cmregprx_init

DESCRIPTION
  This function should be called to initialize the CM RegProxy.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_init
(
        rex_tcb_type                   *tcb_ptr,
           /* Pointer to the task control block for the task in which
           ** RegProxy runs.
           */

        rex_sigs_type                  cmd_q_sig
           /* Signal to be set when a command is posted in the RegProxy's
           ** command queue.
           */
);



/*===========================================================================

FUNCTION cmregprx_sanity_timeout

DESCRIPTION
  This function should be called when the RegProxy's sanity timer expires or
  in other words, when the RegProxy's sanity timer REX signal is set.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_sanity_timeout
(
        void
);


/*===========================================================================

FUNCTION cmregprx_send_hplmn_srch_req

DESCRIPTION
  Function is called to inform REG of user initiated HPLMN search request.
  As a result of this request REG initiates a PLMN search. IF HPLMN is found
  in the return result of this search REG attemps to register on the HPLMN.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void               cmregprx_send_hplmn_srch_req 
(
        void
);

/*===========================================================================

FUNCTION cmregprx_send_exit_from_pwr_save_ind

DESCRIPTION
  This function sends the cm_exit_from_pwr_save  to REG.
  CMREGPRXY substate will not change with this ind.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            cmregprx_send_exit_from_pwr_save_ind
(
        void
);


#if defined(FEATURE_GW_GET_NET_ABORT)
#error code not present
#endif /* FEATURE_GW_GET_NET_ABORT */

#endif /* CMREGPRX_H */

