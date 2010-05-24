#ifndef MCI_H
#define MCI_H
/*===========================================================================

          M A I N   C O N T R O L   T A S K    H E A D E R

DESCRIPTION
  This file contains global declarations and external references
  required by internal MC units and subtasks.

Copyright (c) 1990-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mci.h_v   1.10   08 Oct 2002 12:21:36   hexinw  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mci.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/08/08   ag      Mainlined FEATURE_LPM
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
10/08/02   hxw     Changed FEATURE_GPSONE_MSBASED to FEATURE_GPSONE_OOS.
09/30/02   sb      Mainlined FEATURE_SD20.
09/06/02   va      Cleaned up lint errors.
06/07/02   sj      Added support for multimode architecture.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/14/02   fc      Merged from 1.5 in MSM archive.
 02/06/02  abh     Added code to go into GPS state when Phone is in Acquisition state.
 02/06/02  sh      Added hybird operation support
 05/25/01  ts      Added UIM queue for new UIM server interface.
06/26/01   sb      Merged the following from common archive:
           05/25/01   ts      Added UIM queue for new UIM server interface.
05/01/01   dna     Added support for HDR.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
05/12/99   kmp     Changed T_AUTH to FEATURE_AUTH
09/04/98   pms     Neared the variables to save ROM space.
08/25/98   ck      Externalized mc_dh_q
01/29/98   sk      Added nv_get_item, nv_put_item, mcid_chk as externs.
02/15/96   dna     Included target.h to provide T_AUTH define for extern.
06/07/95   dna     Added External Authentication Task Report Queue
07/09/92   ip      Initial release.

===========================================================================*/

#include "queue.h"
#include "target.h"
#include "rex.h"

/* Is vendor id validation supported ? */
#ifdef FEATURE_ID
#error code not present
#endif

#include "prot.h"           /* Common type declarations for multimode. */
#include "mcc.h"



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* The MC command queue is declared external here so that the MCC and      */
/* ACPMC subtasks can access it to get commands arriving to it.  The       */
/* report timer is regional so that MC and MCSYS can use it.               */

extern  q_type mc_cmd_q;
extern  rex_timer_type    mc_rpt_timer;

#ifdef FEATURE_AUTH
/* The auth report queue is used by MCC and ACPMC.                         */
extern  q_type mc_auth_q;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
/* The uim report queue is used by MCC and ACPMC.                          */
extern  q_type mc_uim_q;
#endif /* FEATURE_UIM_RUIM */

#if (defined (FEATURE_DH) && defined (FEATURE_DH_EXP))
/* The dh report queue is used by MCC.                                     */
extern  q_type            mc_dh_q;
#endif /* FEATURE_DH and FEATURE_DH_EXP*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  States of the MC                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
typedef enum {
  MC_ONLINE_DIGITAL_STATE,   /* CDMA online state                  */
  MC_OFFLINE_DIGITAL_STATE,  /* CDMA offline state                 */
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#ifdef FEATURE_GPSONE_OOS
#error code not present
#endif /* FEATURE_GPSONE_OOS */
  MC_RESET_STATE,            /* Reset state                        */
  MC_POWERDOWN_STATE,        /* Power down state                   */
  MC_POWERSAVE_STATE,        /* Power save state                   */
  MC_POWERUP_STATE,          /* Power up state                     */
  MC_LPM_STATE,              /* Lower power mode state             */
  MC_MEAS_DED_STATE,         /* Dedicated system measurement state */
#ifdef FEATURE_GPSONE_ACQ_MODE
#error code not present
#endif
  MC_EXIT_STATE              /* Exit state                         */
} mc_state_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_ID
#error code not present
#endif /* FEATURE_ID */


/*===========================================================================
FUNCTION mc_map_deact_to_mcc_exit_reason

DESCRIPTION
  This function maps the deact_reason received in MMoC's deactivate request
  to mcc_exit_name_type.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_exit_reason

SIDE EFFECTS
  None.
===========================================================================*/
extern mcc_exit_name_type              mc_map_deact_to_mcc_exit_reason
(
       prot_deact_e_type               deact_reason
          /* MMoC's protocol deactivate reason.
          */

);


/*===========================================================================
FUNCTION mc_map_gen_cmd_to_mcc_exit_reason

DESCRIPTION
  This function maps the MMOC's generic command received to
  mcc_exit_name_type.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_exit_reason

SIDE EFFECTS
  None.
===========================================================================*/
extern mcc_exit_name_type              mc_map_gen_cmd_to_mcc_exit_reason
(
       prot_gen_cmd_s_type             *gen_cmd_ptr
          /* Pointer to the MMoC's generic command.
          */

);

#endif /* MCI_H */

