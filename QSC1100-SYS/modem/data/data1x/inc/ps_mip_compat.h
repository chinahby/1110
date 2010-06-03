#ifndef _PS_MIP_COMPAT_H
#define _PS_MIP_COMPAT_H

/*===========================================================================

                        P S _ M I P _ C O M P A T . H

DESCRIPTION

 The Data Services Mobile IP Multimode compatibility file

 Provides translation from old constants and functions to new MM equivalents

 Will be deprecated once software releases are complete for non-MM targets

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/ps_mip_compat.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/05/03    jd     Wrapped in FEATURE_DATA_MM
04/11/03    ak     Updated for new 707 RMSM interfaces.
12/29/02    jd     Added ds_atcop_qcmipp_val for DMU builds
11/14/02    jd     Created module
===========================================================================*/

#ifdef FEATURE_DATA_MM

/*===========================================================================

                         DEFINITION TRANSLATIONS

===========================================================================*/

#define PS_MIP_SOCK_EVENT_SIG     ((rex_sigs_type) 1 << ( PS_MIP_SOCK_EVENT_SIGNAL ))
#define PS_MIP_NET_EVENT_SIG      ((rex_sigs_type) 1 << ( PS_MIP_NET_EVENT_SIGNAL ))
#define mip_rm_sm_post_event      ds707_rmsm_post_event
#define RMSM_UM_MIP_DOWN          DS707_RMSM_UM_MIP_DOWN_EV
#define RMSM_UM_MIP_UP            DS707_RMSM_UM_MIP_UP_EV
#define RMSM_UM_RM_UP_STATE       DS707_RMSM_UM_RM_UP_MIP_STATE 

#define netmdl_set_mode           ppp_set_mode
#define IFACE_UM_PPP_STACK        PPP_UM_SN_DEV
#define ds_atcop_erase_shadow_nv  dsat707_erase_shadow_nv
#define ds_atcop_fill_mip_vals    dsat707_fill_mip_vals
#define ds_atcop_qcmipt_val       dsat707_qcmipt_val
#define ds_atcop_qcmipp_val       dsat707_qcmipp_val
#define ds_atcop_qcmdr_val        dsat707_qcmdr_val
#define psi_get_nv_item           ps_get_nv_item
#define psi_put_nv_item           ps_put_nv_item
#define ds_socfg_get_p_rev_index  ds707_p_rev_get_index

#define DS_BS_PREV_6              DS707_P_REV_6

#endif /* FEATURE_DATA_MM */

#endif /* _PS_MIP_COMPAT_H */
