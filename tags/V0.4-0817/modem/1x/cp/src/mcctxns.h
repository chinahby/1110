#ifndef MCCTXNS_H
#define MCCTXNS_H
/*===========================================================================

    M C   T R A N S A C T I O N   C L I E N T   F U N C T I O N S

GENERAL DESCRIPTION
  This module contains functions to support the handoff and service
  configuration transactions.

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctxns.h_v   1.4   06 Sep 2002 09:24:48   varavamu  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mcctxns.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/19/04   sb       Added support for Release C Decoupling PLCM from ESN
01/03/05   va      mcctxns_begin_tx_on_off now takes an argument for ON/OFF
06/30/04   bkm     Featurized prototye for mcctxns_init_txc_f_dv_params.
06/29/04   pg      Added function mcctxns_init_txc_f_dv_params().
06/21/04   va      Changes for HHO/CFS/GPS clean up
06/16/04   pg      Added support for Release D Rate Change Message.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
09/06/02   va      Cleaned up lint errors.
08/02/02   va      Removed obsolete mcctxns_begin_control_hold_trans
                   function declaration.
06/20/02   az      Added function prototypes for mcctxns_begin_ext_rel_trans
04/10/01   jq      Added function prototypes for control hold transaction.
07/17/00   va      Added interface for mcctxns_begin_cfs_trans function.
05/30/00   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "qw.h"
#include "mctrans.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*===========================================================================

FUNCTION mcctxns_begin_long_code_trans

DESCRIPTION
  Starts a Long Code Mask transaction.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_long_code_trans
(
 qword trans_time,
 mctrans_pending_action_msg_type* msg
);


/*===========================================================================

FUNCTION mcctxns_begin_scr_trans

DESCRIPTION
  Starts a SCR transaction.

DEPENDENCIES
  Pending service configuration must be valid.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_scr_trans
(
  qword trans_time
);

/*===========================================================================

FUNCTION mcctxns_begin_dv_tdso_trans

DESCRIPTION
  Starts a DV-TDSO transaction.  This transaction is used to control DV-TDSO 
  channel testing and counter collections.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_dv_tdso_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
);

/*===========================================================================

FUNCTION mcctxns_begin_ho_trans

DESCRIPTION
  Starts a HO transaction.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_ho_trans
(
  qword trans_time
);

/*===========================================================================

FUNCTION mcctxns_begin_subchan_gain_trans

DESCRIPTION
  Starts a FPC_CHAN_GAIN transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_subchan_gain_trans
(
  qword trans_time
);

/*===========================================================================

FUNCTION mcctxns_begin_pwr_ctrl_trans

DESCRIPTION
  Starts a PWR_CTRL_MSG transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_pwr_ctrl_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
);


#if defined(FEATURE_IS2000_CHS) || (defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A_SVD))
/*===========================================================================

FUNCTION mcctxns_begin_ext_rel_trans

DESCRIPTION
  Starts a Extended Release transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_ext_rel_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
);
#endif

/*===========================================================================

FUNCTION mcctxns_begin_cfs_trans

DESCRIPTION
  Starts a CFS transaction.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_cfs_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
);

/*===========================================================================

FUNCTION mcctxns_init_ffpc

DESCRIPTION
  Initializes the action packet for FFPC to included nothing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_ffpc
(
  mctrans_ffpc_action_type* ffpc
);

/*===========================================================================

FUNCTION mcctxns_init_rlgc

DESCRIPTION
  Initializes the action packet for RLGC to included nothing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_rlgc
(
  mctrans_rlgc_action_type* rlgc
);

#ifdef FEATURE_IS2000_CHS

/*===========================================================================

FUNCTION mcctxns_begin_active_trans

DESCRIPTION
  Starts a control hold to active transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_active_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
);
#endif /* FEATURE_IS2000_CHS */

/*===========================================================================

FUNCTION mcctxns_begin_hho_begin_trans

DESCRIPTION
  Starts a "HHO Begin" transaction.
  This is the transaction that gives ASET to SRCH for a HHO and tells
  RXC (& SRCH in case of RIF-HHO) to start looking for good frame.
  This txn is scheduled 1 frame after the action time of the actual
  HO transaction

DEPENDENCIES
 The actual "HO transaction that sets up params for this trans shd
 have already started. The assumption is the RF is fully tuned to the
 new freq by the time we act on this txn.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_hho_begin_trans
(
  qword trans_time
);

/*===========================================================================

FUNCTION mcctxns_begin_return_trans

DESCRIPTION
  Starts a "Return" transaction after a failed HHO

DEPENDENCIES
 The actual "HO transaction that sets up params for this trans shd
 have already started. The assumption is the RF is fully tuned to the
 new freq by the time we act on this txn.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_return_trans
(
  qword trans_time
);

/*===========================================================================

FUNCTION mcctxns_begin_tx_on_off_trans

DESCRIPTION
  Starts a "TX on or off" transaction 

DEPENDENCIES
  This can transmitter to be turned on or off; This txn is always
  "immediate". If TX on/off is to be synced up with anything else
  it will be an action under a different txn (Eg: TX_ON_OFF_ACTION as part of
  HO txn )
  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_tx_on_off_trans
(
  boolean tx_on_off
);

/*===========================================================================

FUNCTION mcctxns_begin_cfs_gps_prep_trans

DESCRIPTION
  Starts a "CFS GPS Prep" transaction 

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_cfs_gps_prep_trans
(
  qword trans_time
);

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

#ifdef FEATURE_IS2000_REL_D_DV
#error code not present
#endif /* FEATURE_IS2000_REL_D_DV */

#endif // MCCTXNS_H

