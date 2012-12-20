#ifndef CMWLL_H
#define CMWLL_H
/*===========================================================================

              C A L L   M A N A G E R   L O W E R   L A Y E R

                       H E A D E R   F I L E

DESCRIPTION
  This header file contains all the message definitions necessary for
  Lower Layers to interface with the Call Manager module.

Copyright (c) 2001 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmwll.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/09   aj      UMA GAN related changes
11/21/08   am      Fixing compilation error for Running off-target tests
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/18/08   aj      Add support for UMA-GAN feature
07/24/08   rn      Send regn reject ind to clients
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_PH_STAT_CHGD
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
02/15/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
02/15/08   ks      Adding SIR, Pathloss Implementation
01/24/08   ks      Display of UE signal strength indication
10/03/07   rn      Added smreg_ps_sig_rel_cnf_T in cm_rpt_type
08/17/07   dm      Featurised to report HSPA call status indication
08/17/07   ka/dm   New field to report HS call in progress
05/15/07   rp      Adding ecio to cm_signal_status_type
05/02/07   rp      Adding FEATURE_CM_LCS
04/15/07   rp      Removing FEATURE_MMGPS
04/04/07   rp      Changed type for cell_change_ind
02/18/07   rp      Removed cm_wms_bc_gw_cb_filter
04/17/06   ka      Adding structure for cell services ind.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
11/30/05   ka      Adding HPLMN srch cnf
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/02/05   ic      Mainlined FEATURE_CIPHER_IND
05/18/05   ka      Merging CIPHER_IND feature.
12/16/04   ic      In cm_rpt_type, put cb_data_ind and cb_duplicate_ind fields
                   under FEATURE_GSM_CB
12/03/04   ws      Support for Remote WMS.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
09/22/04   ka      Removing duplicate declaration for pdp activate sec conf
07/28/04   ka      Added support for Secodnary PDP context
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dk      Support for smreg_ps_sig_rel_ind_T primitive.
05/21/04   ka      Added new structure for cm_camped_ind
02/06/04   ws      Initial jaguar/mainline merge.
09/18/03   RI      Added cs_sig_rel_ind to cm_rpt_type.
07/08/03   sbs/lt  Added support for Location Services related SS messages
05/01/03   jar     Added gsdi_refresh_fcn_ind_type refresh_fcn_ind to
                   cm_rpt_type
04/11/03   prk     Removed references to FEATURE_SUPS and FEATURE_MMGSDI.
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   [CS]Added support for invalid SIM handling. Receive and send
                   GSDI events to CM task.
10/23/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/07/02   vt      Added sim state changed report.
07/17/02   vt      Updated it for multimode.
05/04/02   PRK     Modified cm_name_type and cmd to support new CM-REG interface.
04/25/02   RI      Added process_uss_data_conf field to cm_rpt_type.
04/24/02   RI      Added CM_RAB_REESTAB_FAIL and renamed CM_RAB_ESTABLISH_IND to
                   CM_RAB_REESTABLISH_IND in cm_name_type.
                   Added  rabm_rab_reestablish_fail_T message and
                   renamed rabm_rab_establish_ind_T to
                   rabm_rab_reestablish_ind_T in cm_rpt_type.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary Services.
                     - removed mn_cm_exp_t.h
11/21/01   RI      Added rabm_rab_release_ind_T in cm_rpt_type
                   for PS Data.
10/15/01   RI      Added Supplementary Services declarations.
08/23/01   HD      NAS header file reorg changes + RSSI reporting for GSM/WCDMA.
04/27/01   HD      Initial release.

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */
#endif /* CMWLL_H */

