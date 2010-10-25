#ifndef PM66XXVREG_H
#define PM66XXVREG_H
/*===========================================================================


            P M 6 6 X X V R E G   H E A D E R    F I L E

DESCRIPTION
    This file contains functions prototypes and variable/type/constant 
  declarations to support the different voltage regulators inside the 
  Qualcomm Power Manager ICs.
  
Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007                   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                                                                         
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm66XXvregs.h#16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/08   jtn     Added PM_VREG_LP_USB3P3_ID to pm_vreg_lp_id_type enum
11/13/08   jtn     Added pm_vreg_smps_pulse_skipping_enable() API
10/20/08   jtn/bt  New API to configure VREG_NCP for sample-comparator mode
10/08/08   jtn     Added APIs for UVLO voltage threshold and delay
08/27/08   jtn     Fix SMPS mapping for Han forced PWM/PFM
08/03/08   jtn     Fix problem with QSC1100 S1/S2 in pm_vreg_smps_config()
08/01/08   jtn     Move all proc comm functions to pm_pcil.c
07/25/08   jtn     Added PM_VREG_EXTCDC1_ID and PM_VREG_EXTCDC2_ID for 
                   QSD8x50 external PMIC.
07/11/08   jtn     Moved pm_vote_vreg_switch_pc() from library to common to
                   avoid link error in boot build.
07/11/08   jtn     Undid removal of pm_vreg_pa_dvs_work_around()--required for 
                   DZRF6500RF card
07/01/08   jtn     Added pm_vreg_pull_down_switch_pc()
06/27/08   jtn     Merge changes from QSC1100 branch
06/16/08   jtn     Remove pm_vreg_pa_dvs_config_type definition and 
                   prototype for pm_vreg_pa_dvs_config()
05/26/08   jtn     Move Han-specific initialization to pm_init_comm()
05/14/08   jtn     Removed extraneous commas in enums
04/01/08   jtn     Added APIs for setting SMPS switch size
2/05/08    jtn     Added proc comm wrapper functions pm_vreg_set_level_pc
                   and pm_vote_vreg_switch_pc
02/22/08   jtn     Added PM_VREG_RF1_M
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
12/06/07   jtn     Clean up Han LP mode and pulldown registers
10/18/07   jnoblet Added temporary fix for untrimmed HAN PMICs
10/11/07   jnoblet Changed vreg_id mask shifts to 1ULL
10/10/07   jnoblet Changed pm_vreg_masked_type from uint32 to unit64
09/21/07   jnoblet Added vreg RF1 ID
09/07/07   jnoblet Added API for LDO bypass
08/03/07   jnoblet Added support for HAN PMIC
(end QSC6270 changes)
12/06/07   jtn     Added API pm_vreg_ldo_current_limit_enable
08/17/07   jnoblet Added API pm_vreg_status() to return real-time state
                   of all vregs 
                   pm_vreg_status_initialize()
06/18/07   cng     Added meta comments to pm_vreg_pull_down_switch
06/08/07   cng     Added API pm_vreg_smps_low_batt_stable
06/01/07   cng     Added APIs pm_vreg_smps_individual_clock_enable and
                   pm_vreg_smps_individual_clock_div_sel for controlling 
                   individual clocks to SMPS VREGs   
02/27/07   cng     Added VREG support for EPIC and PM6658 (HT2)
01/31/07   vk      Added metacomments for remoted APIs
12/08/06   cng     Updated pm_vreg_lp_id_type to support all LDOs with LPM
09/28/06   cng     LINT warning fixes
09/14/06   cng     Updated enum pm_vreg_smps_tcxo_div_type to cover full range
                   of SMPS switching frequency
08/30/06   cng     Added enum for active pull down for VREG_MDDI and VREG_BT on PM7500
08/04/06   ab      Added get vreg value ftm api and get vreg enum ftm api support
05/31/06   Vish    Fixed LINT warnings.
02/14/06   cng     Added VREG_BT for PM7500
11/09/05   cng     Added PM6620 support for LDO MSMC
10/24/05   cng     Added Panoramix support for LDO MSME1
10/10/05   cng     Removed function calls that should belong to pm.c
07/25/05   cng     PM7500 support
12/28/04   rmd     Updated the comments for pm_vreg_pa_dvs_config().
12/16/04   rmd     In pm_vreg_control(), updated the code so that the VREG PA
                   gets initialized for PA DVS if PA DVS is enabled and
                   we are turning the VREG PA on. All other VREGs will 
                   unaffected by this change.
10/26/04   rmd     Updated the VREG active pull down API to reflect that the
                   BUCK VREGs also support the active pull down resistor.
08/09/04   rmd     Added support to all LDOs that support LPM mode.
05/16/04   rmd     Added support for the BUCK VREGs compensation settings.
05/04/04   rmd     Added support for the PM6610 MSMC0 VREG.
02/17/04   rmd     Added the ability to select and configure the SMPS VREGs 
                   clock source.
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
01/12/04   rmd     Added support for the LDO pull down resistor. see
                   "pm_vreg_pull_down_switch()".
11/21/03   rmd     Corrected the list of LDOs that support low power mode
                   and updated the list of VREGs that can be turned ON/OFF.
10/09/03   rmd     Changed pm_vreg_set_level() so that it takes mV as a 
                   voltage settings instead of a value calculated from an 
                   equation.
06/26/03   rmd     Created.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum 
{
   PM_VREG_MSMA_ID = 0,
   PM_VREG_MSMP_ID,
   PM_VREG_MSME1_ID,    /* Not supported in Panoramix */
   PM_VREG_MSMC1_ID,    /* Not supported in PM6620 */
   PM_VREG_MSMC2_ID,    /* Supported in PM7500 only */
   PM_VREG_GP3_ID,      /* Supported in PM7500 only */   
   PM_VREG_MSME2_ID,    /* Supported in PM7500 and Panoramix only */
   PM_VREG_GP4_ID,      /* Supported in PM7500 only */
   PM_VREG_GP1_ID,      /* Supported in PM7500 and Han only */
   PM_VREG_TCXO_ID,
   PM_VREG_PA_ID,
   PM_VREG_RFTX_ID,
   PM_VREG_RFRX1_ID,
   PM_VREG_RFRX2_ID,
   PM_VREG_SYNT_ID,
   PM_VREG_WLAN_ID,
   PM_VREG_USB_ID,
   PM_VREG_BOOST_ID,
   PM_VREG_MMC_ID,
   PM_VREG_RUIM_ID,
   PM_VREG_MSMC0_ID,                    /* Supported in PM6610 only */
   PM_VREG_GP2_ID,     /* Supported in PM7500 and Han only */
   PM_VREG_GP5_ID,     /* Supported in PM7500 only */
   PM_VREG_GP6_ID,       /* Supported in PM7500 only */
   PM_VREG_RF_ID,
   PM_VREG_RF_VCO_ID,
   PM_VREG_MPLL_ID,
   PM_VREG_S2_ID,
   PM_VREG_S3_ID,
   PM_VREG_RFUBM_ID,
   PM_VREG_NCP_ID, 
   PM_VREG_RF2_ID,       
   PM_VREG_RFA_ID,    
   PM_VREG_CDC2_ID,   
   PM_VREG_RFTX2_ID,  
   PM_VREG_USIM_ID,   
   PM_VREG_USB2P6_ID, 
   PM_VREG_USB3P3_ID, 
   PM_VREG_EXTCDC1_ID,   //Used for external TI SMPS
   PM_VREG_EXTCDC2_ID,   //Used for external TI SMPS
   PM_VREG_ID_INVALID,

   /* backward compatible enums only */ 
   PM_VREG_MSME_ID = PM_VREG_MSME1_ID,
   PM_VREG_MSME_BUCK_SMPS_ID = PM_VREG_MSME1_ID,
   PM_VREG_MSME1_LDO_ID = PM_VREG_MSME1_ID,

   PM_VREG_MSMC_ID = PM_VREG_MSMC1_ID,
   PM_VREG_MSMC_LDO_ID = PM_VREG_MSMC1_ID,
   PM_VREG_MSMC1_BUCK_SMPS_ID = PM_VREG_MSMC1_ID,

   PM_VREG_MSME2_LDO_ID = PM_VREG_MSME2_ID,

   PM_VREG_CAM_ID = PM_VREG_GP1_ID,      /* Supported in PM7500 only */
   PM_VREG_MDDI_ID = PM_VREG_GP2_ID,     /* Supported in PM7500 only */
   PM_VREG_RUIM2_ID = PM_VREG_GP3_ID,    /* Supported in PM7500 only */   
   PM_VREG_AUX_ID = PM_VREG_GP4_ID,      /* Supported in PM7500 only */
   PM_VREG_AUX2_ID = PM_VREG_GP5_ID,     /* Supported in PM7500 only */
   PM_VREG_BT_ID = PM_VREG_GP6_ID,       /* Supported in PM7500 only */
   PM_VREG_RF1_ID = PM_VREG_RF_ID,       /* added for Han compatibility */
   PM_VREG_S1_ID = PM_VREG_RF1_ID,  
   PM_VREG_5V_ID = PM_VREG_BOOST_ID,
   PM_VREG_RFA1_ID = PM_VREG_RFRX2_ID,
   PM_VREG_RFA2_ID = PM_VREG_RFTX2_ID,
   PM_VREG_XO_ID = PM_VREG_TCXO_ID

}pm_vreg_id_type;

/* PM logical VREG definitions
** These definitions are used with the PM VREG access routines
*/
//lint -esym(641, pm_vreg_mask_type) : Converting enum to int
// Reason: 'pm_vreg_masked_type' is a UINT32 mask which is
//         expected to be assigned a value from 'pm_vreg_mask_type'.
//         So it could be suppressed by a single lint suppression
//         as shown above instead of in various source files where
//         pm_vreg_control(pm_switch_cmd_type vreg_cmd, pm_vreg_masked_type vregs)
//         is invoked.
// bitmask used to control several regulators at once
typedef uint64 pm_vreg_mask_type;
/* pm_vreg_masked_type was uint32 */
typedef uint64 pm_vreg_masked_type;

#define PM_VREG_MSMA_M     (pm_vreg_mask_type)(1ULL << PM_VREG_MSMA_ID) 
#define PM_VREG_MSMP_M     (pm_vreg_mask_type)(1ULL << PM_VREG_MSMP_ID) 
#define PM_VREG_MSME1_M    (pm_vreg_mask_type)(1ULL << PM_VREG_MSME1_ID)   /* Supported in Panoramix only */
#define PM_VREG_MSMC1_M    (pm_vreg_mask_type)(1ULL << PM_VREG_MSMC1_ID)   /* Not supported in PM6620 */
#define PM_VREG_MSMC2_M    (pm_vreg_mask_type)(1ULL << PM_VREG_MSMC2_ID)   /* Supported in PM7500 only */
#define PM_VREG_GP3_M      (pm_vreg_mask_type)(1ULL << PM_VREG_GP3_ID)   /* Supported in PM7500 only */   
#define PM_VREG_MSME2_M    (pm_vreg_mask_type)(1ULL << PM_VREG_MSME2_ID)   /* Supported in PM7500 and Panoramix only */
#define PM_VREG_GP4_M      (pm_vreg_mask_type)(1ULL << PM_VREG_GP4_ID)     /* Supported in PM7500 only */
#define PM_VREG_GP1_M      (pm_vreg_mask_type)(1ULL << PM_VREG_GP1_ID)     /* Supported in PM7500 only */
#define PM_VREG_TCXO_M     (pm_vreg_mask_type)(1ULL << PM_VREG_TCXO_ID) 
#define PM_VREG_PA_M       (pm_vreg_mask_type)(1ULL << PM_VREG_PA_ID) 
#define PM_VREG_RFTX_M     (pm_vreg_mask_type)(1ULL << PM_VREG_RFTX_ID) 
#define PM_VREG_RFRX1_M    (pm_vreg_mask_type)(1ULL << PM_VREG_RFRX1_ID) 
#define PM_VREG_RFRX2_M    (pm_vreg_mask_type)(1ULL << PM_VREG_RFRX2_ID) 
#define PM_VREG_SYNT_M     (pm_vreg_mask_type)(1ULL << PM_VREG_SYNT_ID) 
#define PM_VREG_WLAN_M     (pm_vreg_mask_type)(1ULL << PM_VREG_WLAN_ID) 
#define PM_VREG_USB_M      (pm_vreg_mask_type)(1ULL << PM_VREG_USB_ID) 
#define PM_VREG_BOOST_M    (pm_vreg_mask_type)(1ULL << PM_VREG_BOOST_ID) 
#define PM_VREG_MMC_M      (pm_vreg_mask_type)(1ULL << PM_VREG_MMC_ID) 
#define PM_VREG_RUIM_M     (pm_vreg_mask_type)(1ULL << PM_VREG_RUIM_ID) 
#define PM_VREG_MSMC0_M    (pm_vreg_mask_type)(1ULL << PM_VREG_MSMC0_ID)   /* Supported in PM6610 only*/
#define PM_VREG_GP2_M      (pm_vreg_mask_type)(1ULL << PM_VREG_GP2_ID)    /* Supported in PM7500 only */
#define PM_VREG_GP5_M      (pm_vreg_mask_type)(1ULL << PM_VREG_GP5_ID)    /* Supported in PM7500 only */
#define PM_VREG_GP6_M      (pm_vreg_mask_type)(1ULL << PM_VREG_GP6_ID)       /* Supported in PM7500 only */
#define PM_VREG_RF_M       (pm_vreg_mask_type)(1ULL << PM_VREG_RF_ID)   
#define PM_VREG_RF_VCO_M   (pm_vreg_mask_type)(1ULL << PM_VREG_RF_VCO_ID)   
#define PM_VREG_MPLL_M     (pm_vreg_mask_type)(1ULL << PM_VREG_MPLL_ID)   
#define PM_VREG_S2_M       (pm_vreg_mask_type)(1ULL << PM_VREG_S2_ID) 
#define PM_VREG_S3_M       (pm_vreg_mask_type)(1ULL << PM_VREG_S3_ID)   
#define PM_VREG_RFUBM_M    (pm_vreg_mask_type)(1ULL << PM_VREG_RFUBM_ID)   
#define PM_VREG_NCP_M      (pm_vreg_mask_type)(1ULL << PM_VREG_NCP_ID)
#define PM_VREG_RF1_M      (pm_vreg_mask_type)(1ULL << PM_VREG_RF1_ID)      
#define PM_VREG_RF2_M      (pm_vreg_mask_type)(1ULL << PM_VREG_RF2_ID)      
#define PM_VREG_RFA_M      (pm_vreg_mask_type)(1ULL << PM_VREG_RFA_ID)
#define PM_VREG_CDC2_M     (pm_vreg_mask_type)(1ULL << PM_VREG_CDC2_ID)
#define PM_VREG_RFTX2_M    (pm_vreg_mask_type)(1ULL << PM_VREG_RFTX2_ID)
#define PM_VREG_USIM_M     (pm_vreg_mask_type)(1ULL << PM_VREG_USIM_ID)
#define PM_VREG_USB2P6_M   (pm_vreg_mask_type)(1ULL << PM_VREG_USB2P6_ID)
#define PM_VREG_USB3P3_M   (pm_vreg_mask_type)(1ULL << PM_VREG_USB3P3_ID)   
#define PM_VREG_S1_M       (pm_vreg_mask_type)(1ULL << PM_VREG_S1_ID)       
#define PM_VREG_5V_M       (pm_vreg_mask_type)(1ULL << PM_VREG_5V_ID)
#define PM_VREG_RFA1_M     (pm_vreg_mask_type)(1ULL << PM_VREG_RFA1_ID)
#define PM_VREG_RFA2_M     (pm_vreg_mask_type)(1ULL << PM_VREG_RFA2_ID)
#define PM_VREG_XO_M       (pm_vreg_mask_type)(1ULL << PM_VREG_XO_ID)
#define PM_VREG_EXTCDC1_M  (pm_vreg_mask_type)(1ULL << PM_VREG_EXTCDC1_ID) 
#define PM_VREG_EXTCDC2_M  (pm_vreg_mask_type)(1ULL << PM_VREG_EXTCDC2_ID) 

#define PM_VREG_INVALID_M   0x0 

/* backward compatible enums only */ 
#define PM_VREG_MSMC_M     PM_VREG_MSMC1_M   /* Supported in PM6620 only */
#define PM_VREG_MSME_M     PM_VREG_MSME1_M   /* Not supported in Panoramix */
#define PM_VREG_CAM_M      PM_VREG_GP1_M     /* Supported in PM7500 only */
#define PM_VREG_MDDI_M     PM_VREG_GP2_M     /* Supported in PM7500 only */
#define PM_VREG_RUIM2_M    PM_VREG_GP3_M     /* Supported in PM7500 only */   
#define PM_VREG_AUX_M      PM_VREG_GP4_M     /* Supported in PM7500 only */
#define PM_VREG_AUX2_M     PM_VREG_GP5_M     /* Supported in PM7500 only */
#define PM_VREG_BT_M       PM_VREG_GP6_M     /* Supported in PM7500 only */

typedef enum 
{
   PM_VREG_LP_MSMA_ID,
   PM_VREG_LP_MSMP_ID,
   PM_VREG_LP_MSME1_ID,     /* Not supported in Panoramix */
   PM_VREG_LP_GP3_ID,    /* Supported in PM7500 only */   
   PM_VREG_LP_MSMC_ID,     /* Supported in PM6620 only */
   PM_VREG_LP_MSME2_ID,    /* Supported in PM7500 and Panoramix only */
   PM_VREG_LP_GP4_ID,      /* Supported in PM7500 only */
   PM_VREG_LP_GP1_ID,      /* Supported in PM7500 only */
   PM_VREG_LP_RFTX_ID,
   PM_VREG_LP_RFRX1_ID,
   PM_VREG_LP_RFRX2_ID,
   PM_VREG_LP_WLAN_ID,
   PM_VREG_LP_MMC_ID,
   PM_VREG_LP_RUIM_ID,
   PM_VREG_LP_MSMC0_ID,    /* Supported in PM6610 only */
   PM_VREG_LP_GP2_ID,     /* Supported in PM7500 only */
   PM_VREG_LP_GP5_ID,     /* Supported in PM7500 only */
   PM_VREG_LP_GP6_ID,       /* Supported in PM7500 only */
   PM_VREG_LP_MPLL_ID,
   PM_VREG_LP_RFUBM_ID,
   PM_VREG_LP_RFA_ID,    
   PM_VREG_LP_CDC2_ID,   
   PM_VREG_LP_RFTX2_ID,  
   PM_VREG_LP_USIM_ID,   
   PM_VREG_LP_USB2P6_ID, 
   PM_VREG_LP_TCXO_ID,   
   PM_VREG_LP_USB3P3_ID, 
   PM_VREG_LP_ID_INVALID,

   PM_VREG_LP_MSME_ID = PM_VREG_LP_MSME1_ID,
   /* backward compatible enums only */ 
   PM_VREG_LP_CAM_ID = PM_VREG_LP_GP1_ID,
   PM_VREG_LP_MDDI_ID = PM_VREG_LP_GP2_ID,
   PM_VREG_LP_RUIM2_ID = PM_VREG_LP_GP3_ID,
   PM_VREG_LP_AUX_ID = PM_VREG_LP_GP4_ID,
   PM_VREG_LP_AUX2_ID = PM_VREG_LP_GP5_ID,
   PM_VREG_LP_BT_ID = PM_VREG_LP_GP6_ID,

   PM_VREG_LP_MSMC_LDO_ID = PM_VREG_LP_MSMC_ID,
   PM_VREG_LP_MSME1_LDO_ID = PM_VREG_LP_MSME1_ID, 
   PM_VREG_LP_MSME2_LDO_ID = PM_VREG_LP_MSME2_ID,
   PM_VREG_LP_RFA1_ID = PM_VREG_LP_RFRX2_ID,
   PM_VREG_LP_RFA2_ID = PM_VREG_LP_RFTX2_ID,
   PM_VREG_LP_XO_ID = PM_VREG_LP_TCXO_ID
}pm_vreg_lp_id_type;

typedef enum
{
   PM_VREG_SMPS_PA_ID,
   PM_VREG_SMPS_MSME_ID,
   PM_VREG_SMPS_MSMC1_ID,
   PM_VREG_SMPS_MSMC2_ID,  /* Supported in PM7500 only */
   PM_VREG_SMPS_BOOST_ID,
   PM_VREG_SMPS_S2_ID,
   PM_VREG_SMPS_S3_ID,
   PM_VREG_SMPS_RF_ID,
   PM_VREG_SMPS_S1_ID,   
   PM_VREG_SMPS_ID_INVALID,
   PM_VREG_SMPS_MSMC_ID = PM_VREG_SMPS_MSMC1_ID,
   PM_VREG_SMPS_RF1_ID = PM_VREG_SMPS_S1_ID,
   PM_VREG_SMPS_RF2_ID = PM_VREG_SMPS_S2_ID,   //HAN
   PM_VREG_SMPS_5V_ID = PM_VREG_SMPS_BOOST_ID
}pm_vreg_smps_id_type;

/* this table represents the 12 LDOs available for HAN */
/* and Kip */
typedef enum
{
   PM_VREG_LDO_GP1_ID,
   PM_VREG_LDO_MSME1_ID,
   PM_VREG_LDO_RFA_ID,
   PM_VREG_LDO_MSMP_ID,  
   PM_VREG_LDO_CDC2_ID,
   PM_VREG_LDO_RFTX2_ID,
   PM_VREG_LDO_RFRX2_ID,
   PM_VREG_LDO_GP2_ID,
   PM_VREG_LDO_TCXO_ID,
   PM_VREG_LDO_MPLL_ID,
   PM_VREG_LDO_USIM_ID,
   PM_VREG_LDO_USB2P6_ID,
   PM_VREG_LDO_USB3P3_ID,
   PM_VREG_LDO_ID_INVALID,
   PM_VREG_LDO_MSMC_ID = PM_VREG_LDO_MSMP_ID,
   PM_VREG_LDO_MSMA_ID = PM_VREG_LDO_RFA_ID,
   PM_VREG_LDO_MSME2_ID = PM_VREG_LDO_GP2_ID,
   PM_VREG_LDO_RFA1_ID = PM_VREG_LDO_RFRX2_ID,
   PM_VREG_LDO_RFA2_ID = PM_VREG_LDO_RFTX2_ID,
   PM_VREG_LDO_XO_ID = PM_VREG_LDO_TCXO_ID,
   PM_VREG_LDO_RUIM_ID = PM_VREG_LDO_USIM_ID
}pm_vreg_ldo_id_type;

/* BUCK VREGs. */
typedef enum
{
   PM_VREG_BUCK_PA_ID,
   PM_VREG_BUCK_MSME_ID,
   PM_VREG_BUCK_MSMC1_ID,
   PM_VREG_BUCK_MSMC2_ID,
   PM_VREG_BUCK_S2_ID,
   PM_VREG_BUCK_S3_ID,
   PM_VREG_BUCK_RF_ID,
   PM_VREG_BUCK_RF2_ID,   
   PM_VREG_BUCK_ID_INVALID,
   PM_VREG_BUCK_MSMC_ID = PM_VREG_BUCK_MSMC1_ID,
   PM_VREG_BUCK_RF1_ID = PM_VREG_BUCK_RF_ID,
   PM_VREG_BUCK_S1_ID = PM_VREG_BUCK_RF1_ID
}pm_vreg_buck_id_type;

/* BUCK VREGS Compensation settings. */
typedef struct
{
   uint8 sawtooth_set;
   uint8 gm_set;
   uint8 resistor_set;
}pm_vreg_buck_cfg_cp_type;

/* SMPS VREGs clock source. */
typedef enum
{
   PM_VREG_SMPS_CLK_SEL__TCXO,
   PM_VREG_SMPS_CLK_SEL__RC,
   PM_VREG_SMPS_CLK_SEL__INVALID
}pm_vreg_smps_clk_sel_type;

/* SMPS VREGs clock phase shift. */
typedef enum
{
   PM_VREG_SMPS_PHASE_SEL__0_0_0_0,
   PM_VREG_SMPS_PHASE_SEL__0_0_180_180,
   PM_VREG_SMPS_PHASE_SEL__0_90_0_90,
   PM_VREG_SMPS_PHASE_SEL__0_90_180_270,
   PM_VREG_SMPS_PHASE_SEL__INVALID
}pm_vreg_smps_phase_sel_type;

/* SMPS VREGs TCXO dividers. */
typedef enum
{
   PM_VREG_SMPS_TCXO_DIV__2,     
   PM_VREG_SMPS_TCXO_DIV__4,     
   PM_VREG_SMPS_TCXO_DIV__6,     
   PM_VREG_SMPS_TCXO_DIV__8,     
   PM_VREG_SMPS_TCXO_DIV__10,
   PM_VREG_SMPS_TCXO_DIV__12,
   PM_VREG_SMPS_TCXO_DIV__14,
   PM_VREG_SMPS_TCXO_DIV__16,
   PM_VREG_SMPS_TCXO_DIV__18,
   PM_VREG_SMPS_TCXO_DIV__20,
   PM_VREG_SMPS_TCXO_DIV__22,
   PM_VREG_SMPS_TCXO_DIV__24,
   PM_VREG_SMPS_TCXO_DIV__26,
   PM_VREG_SMPS_TCXO_DIV__28,
   PM_VREG_SMPS_TCXO_DIV__30,
   PM_VREG_SMPS_TCXO_DIV__32,
   PM_VREG_SMPS_TCXO_DIV__INVALID
}pm_vreg_smps_tcxo_div_type;

/* List of VREGs that support the Pull Down Resistor setting. */
typedef enum
{
   PM_VREG_PDOWN_MSMA_ID,
   PM_VREG_PDOWN_MSMP_ID,
   PM_VREG_PDOWN_MSME1_ID,     /* Not supported in Panoramix */
   PM_VREG_PDOWN_MSMC1_ID,    /* Not supported in PM6620 */
   PM_VREG_PDOWN_MSMC2_ID,    /* Supported in PM7500 only */
   PM_VREG_PDOWN_GP3_ID,    /* Supported in PM7500 only */   
   PM_VREG_PDOWN_MSME2_ID,    /* Supported in PM7500 and Panoramix only */
   PM_VREG_PDOWN_GP4_ID,      /* Supported in PM7500 only */
   PM_VREG_PDOWN_GP1_ID,      /* Supported in PM7500 only */
   PM_VREG_PDOWN_TCXO_ID,
   PM_VREG_PDOWN_PA_ID,
   PM_VREG_PDOWN_RFTX_ID,
   PM_VREG_PDOWN_RFRX1_ID,
   PM_VREG_PDOWN_RFRX2_ID,
   PM_VREG_PDOWN_SYNT_ID,
   PM_VREG_PDOWN_WLAN_ID,
   PM_VREG_PDOWN_USB_ID,
   PM_VREG_PDOWN_MMC_ID,
   PM_VREG_PDOWN_RUIM_ID,
   PM_VREG_PDOWN_MSMC0_ID,                    /* Supported in PM6610 only */
   PM_VREG_PDOWN_GP2_ID,     /* Supported in PM7500 only */
   PM_VREG_PDOWN_GP5_ID,     /* Supported in PM7500 only */
   PM_VREG_PDOWN_GP6_ID,       /* Supported in PM7500 only */
   PM_VREG_PDOWN_RF_ID,
   PM_VREG_PDOWN_RF_VCO_ID,
   PM_VREG_PDOWN_MPLL_ID,
   PM_VREG_PDOWN_S2_ID,
   PM_VREG_PDOWN_S3_ID,
   PM_VREG_PDOWN_RFUBM_ID,
    /* new for HAN */
   PM_VREG_PDOWN_RF1_ID,
   PM_VREG_PDOWN_RF2_ID,       
   PM_VREG_PDOWN_RFA_ID,    
   PM_VREG_PDOWN_CDC2_ID,   
   PM_VREG_PDOWN_RFTX2_ID,  
   PM_VREG_PDOWN_USIM_ID,   
   PM_VREG_PDOWN_USB2P6_ID, 
   PM_VREG_PDOWN_USB3P3_ID,
   PM_VREG_PDOWN_INVALID_ID,

    /* backward compatible enums only */ 
   PM_VREG_PDOWN_CAM_ID = PM_VREG_PDOWN_GP1_ID,
   PM_VREG_PDOWN_MDDI_ID = PM_VREG_PDOWN_GP2_ID,
   PM_VREG_PDOWN_RUIM2_ID = PM_VREG_PDOWN_GP3_ID,
   PM_VREG_PDOWN_AUX_ID = PM_VREG_PDOWN_GP4_ID,
   PM_VREG_PDOWN_AUX2_ID = PM_VREG_PDOWN_GP5_ID,
   PM_VREG_PDOWN_BT_ID = PM_VREG_PDOWN_GP6_ID,

   PM_VREG_PDOWN_MSME_ID = PM_VREG_PDOWN_MSME1_ID,
   PM_VREG_PDOWN_MSMC_ID = PM_VREG_PDOWN_MSMC1_ID,
   PM_VREG_PDOWN_RFA1_ID = PM_VREG_PDOWN_RFRX2_ID,
   PM_VREG_PDOWN_RFA2_ID = PM_VREG_PDOWN_RFTX2_ID,
   PM_VREG_PDOWN_XO_ID = PM_VREG_PDOWN_TCXO_ID
}pm_vreg_pdown_id_type;

typedef enum
{
   PM_VREG_SMPS_MODE__TCXO_EN,
   PM_VREG_SMPS_MODE__PWM,
   PM_VREG_SMPS_MODE__PBM,
   PM_VREG_SMPS_MODE__AUTOMATIC,
   PM_VREG_SMPS_MODE__INVALID
}pm_vreg_smps_mode_type;

/* Low power mode */
typedef enum 
{
    PM_MSMA_LP,
    PM_MSMP_LP,
    PM_MSMC_LP,
    PM_ALL_LP
}pm_lp_type;

/* NCP switching frequency */
typedef enum
{
  PM_NCP_FREQ_9_6MHZ,
  PM_NCP_FREQ_4_8MHZ,
  PM_NCP_FREQ_3_2MHZ,  
  PM_NCP_FREQ_2_4MHZ,
  PM_NCP_FREQ_1_92MHZ,
  PM_NCP_FREQ_1_6MHZ, //Default
  PM_NCP_FREQ_1_37MHZ,
  PM_NCP_FREQ_1_2MHZ, 
  PM_NCP_FREQ_OUT_OF_RANGE 
}pm_ncp_switching_freq_type;

/* PA DVS VREG PA Config settings. */
typedef struct
{
    uint32              discharge_t; /* Load capacitor discharge time.  */
    uint32              ton;         /* Internal PFET is turned on.     */
    pm_switch_cmd_type  cmd;         /* Enable/Disable config settings. */
}pm_vreg_pa_dvs_config_type;

typedef enum
{
    PM_SMPS_SWITCH_SIZE_1_8,
    PM_SMPS_SWITCH_SIZE_1_4,
    PM_SMPS_SWITCH_SIZE_1_2,
    PM_SMPS_SWITCH_SIZE_1_1,
    PM_SMPS_SWITCH_SIZE_INVALID
} pm_vreg_smps_switch_size_type;

#define PM_VREG_ENUM_NUM_OF_DIGITS (3)
#define PM_VREG_MAX_ENUM_NAME_STR_SIZE (200)
/*===========================================================================

                        MANAGED LDO DRIVER

===========================================================================*/
/*===========================================================================

FUNCTION pm_lp_mode_control                           EXTERNAL FUNCTION

DESCRIPTION
  This function enables or disable the Voltage Regulators low power mode.
                                                                             
INPUT PARAMETERS
  1) Parameter name: vreg_cmd.
     - Indicates if we want to turn low power mode ON or OFF.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = Disable the VREG low power mode.
            PM_ON_CMD  = Enable the VREG low power mode.

  2) Parameter name: which_vreg. 
     - Which VREG low power mode will be turn ON/OFF.

     Parameter type: pm_vreg_lp_id_type (enum).
     - Valid Inputs:
        
            PM_VREG_LP_MSMA_ID
            PM_VREG_LP_MSMP_ID
            PM_VREG_LP_MMC_ID
            PM_VREG_LP_RFRX1_ID
            PM_VREG_LP_RFRX2_ID
            PM_VREG_LP_RFTX_ID
            PM_VREG_LP_RUIM_ID
            PM_VREG_LP_WLAN_ID
            PM_VREG_LP_MSMC0_ID
            
RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC.     
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                        with the PMIC.      
    PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init() 
  2) pm_init()  

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_lp_mode_control(pm_switch_cmd_type vreg_cmd, 
                                           pm_vreg_lp_id_type which_vreg);
/*~ FUNCTION pm_lp_mode_control */

/*===========================================================================

FUNCTION pm_vreg_control                           EXTERNAL FUNCTION

DESCRIPTION
    This function enables or disables each or a combination 
  Voltage Regulators.
  
  example:
     This function call will enable the WLAN VREG 
     "pm_vreg_control(PM_ON_CMD,PM_VREG_WLAN_M);"
     
     or 

     This function call will enable both the WLAN and SYNTH VREG.
     "pm_vreg_control(PM_ON_CMD,PM_VREG_WLAN_M | PM_VREG_SYNT_M);"
     
     
     NOTE: This function replaces "pm_ldo_control()". 
                                                                                  
INPUT PARAMETERS
  1) Parameter name: vreg_cmd.
     - Indicates if we want to turn the VREG ON or OFF

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = Disable the VREG.
            PM_ON_CMD  = Enable the VREG.

  2) Parameter name: ldo. 
     - Which LDO needs to be turn ON/OFF.

     Parameter type: pm_vreg_mask_type (uint64).
     - Valid Inputs:
            PM_VREG_MSME_M  
            PM_VREG_MSMC1_M 
            PM_VREG_MSMC2_M   
            PM_VREG_MSMS_M    
            PM_VREG_MEM_M     
            PM_VREG_GP_M      
            PM_VREG_CAM_M     
            PM_VREG_TCXO_M  
            PM_VREG_PA_M    
            PM_VREG_RFTX_M  
            PM_VREG_RFRX1_M 
            PM_VREG_RFRX2_M 
            PM_VREG_SYNT_M  
            PM_VREG_WLAN_M
            PM_VREG_USB_M
            PM_VREG_BOOST_M
            PM_VREG_MMC_M
            PM_VREG_RUIM_M
            PM_VREG_MSMC0_M   
            
RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC.     
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                        with the PMIC.          
    PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_vreg_control ( pm_switch_cmd_type vreg_cmd, 
                                          pm_vreg_mask_type vregs);

/*===========================================================================

FUNCTION pm_vreg_set_level                           EXTERNAL FUNCTION

DESCRIPTION
    This function programs the voltage levels for the specified 
  Voltage Regulator.
  
  Example:
    // Program RFRX1 VREG to 2.85 Volts. 
    err = pm_vreg_set_level(PM_VREG_RFRX1_ID, 2850); 
 
 
INPUT PARAMETERS
  1) Parameter name: vreg. 
     - One of the Voltage Regulators.

     Parameter type: pm_vreg_id_type (enum).
     - Valid Inputs:

        PM_VREG_MSMA_ID
        PM_VREG_MSMP_ID
        PM_VREG_MSME_ID
        PM_VREG_MSMC1_ID
        PM_VREG_MSMC2_ID     Not supported in PM6650 
        PM_VREG_MSMS_ID      Not supported in PM6650    
        PM_VREG_MEM_ID       Not supported in PM6650 
        PM_VREG_GP_ID        Not supported in PM6650 
        PM_VREG_CAM_ID       Not supported in PM6650 
        PM_VREG_TCXO_ID
        PM_VREG_PA_ID
        PM_VREG_RFTX_ID
        PM_VREG_RFRX1_ID
        PM_VREG_RFRX2_ID
        PM_VREG_SYNT_ID
        PM_VREG_WLAN_ID
        PM_VREG_USB_ID
        PM_VREG_BOOST_ID
        PM_VREG_MMC_ID
        PM_VREG_RUIM_ID

  2) Parameter name: voltage.
     - Voltage regulator level in mV.

     Parameter type: uint16 (16 bits).
     - Valid Voltage settings for the different VREGs: 
            
            VREG: PA, MSME, MSMC1 and MSMC2            
              750  mV to 1525 mV, in 25 mV steps.
              1550 mV to 3050 mV, in 50 mV steps.
              
            VREG: BOOST, USB
              3000 mV to 6100 mV in 100 mV steps.
              
            VREG: MSMA, MSMP, MSMS, MEM, GP, CAM, TCXO, PA, RFTX, RFRX1
                  RFRX2, SYNTH, WLAN, MMC, RUIM.
              1500 mV to 3050 mV in 50 mV steps.
              
     NOTE: Voltage values between voltage steps will be rounded up to
           the next voltage setting. ex:          
           
            // Because 1760 mV is not a multiple of 25 mV, the
            // PA VREG voltage setting will be rounded up to
            // the next setting (1775 mV).
            pm_vreg_set_level(PM_VREG_PA_ID, 1760);
                   
RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                        with the PMIC.                      
    PM_ERR_FLAG__SUCCESS              = SUCCESS.    

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
  
===========================================================================*/
extern pm_err_flag_type  pm_vreg_set_level ( pm_vreg_id_type vreg, 
                                             uint16 voltage );
/*~ FUNCTION pm_vreg_set_level */

/*===========================================================================
FUNCTION pm_vreg_get_level                           EXTERNAL FUNCTION

DESCRIPTION
    This function gets the voltage level of the specified Voltage Regulator.
  Example:
    // Get the voltage level of RFRX1 VREG 
    err = pm_vreg_get_level(PM_VREG_RFRX1_ID, *vreg_level); 
    vreg_level will hold the voltage level of RFRX1 vreg in mV
    
INPUT PARAMETERS
  1) Parameter name: vreg. 
     - One of the Voltage Regulators.

     Parameter type: pm_vreg_id_type (enum).
     use pm_vreg_get_enum() API to get the list of supported vregs of 
       the target and their enumration

  2) Parameter name:  *vreg_level.
     - holds the Voltage regulator level in mV.
     Parameter type: uint16* (16 bits).
                   
RETURN VALUE
  Return Value Type: pm_err_flag_type.
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                        with the PMIC.                    
    PM_ERR_FLAG__SUCCESS              = SUCCESS.    
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC. 
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type  pm_vreg_get_level ( pm_vreg_id_type vreg,
 											 uint16 *vreg_level  );



#define pm_ldo_control(vreg_cmd, vregs) (pm_vreg_control(vreg_cmd, vregs))


/*===========================================================================

FUNCTION  pm_vreg_smps_config                       EXTERNAL FUNCTION

DESCRIPTION
    Use this function to select the different modes available for the 
  SMPS (switch mode power supply) Voltage Regulators.
 
INPUT PARAMETERS
  1) Parameter name: vreg_smps. 
     - Which SMPS Voltage Regulator we want to configure.

     Parameter type: pm_vreg_smps_id_type (enum).
     - Valid Inputs:
          PM_VREG_SMPS_PA_ID,
          PM_VREG_SMPS_MSME_ID
          PM_VREG_SMPS_MSMC1_ID
          PM_VREG_SMPS_MSMC2_ID         Note: Not supported in PM6650.
          PM_VREG_SMPS_BOOST_ID

  2) Parameter name: mode.
     - The mode that the selected SMPS VREG will be configure to.

     Parameter type: pm_vreg_smps_mode_type (enum).
     - Valid inputs: 
          PM_VREG_SMPS_MODE__TCXO_EN    Note:Not available for the BOOST VREG.
          PM_VREG_SMPS_MODE__PWM        Note:Not available for the BOOST VREG.
          PM_VREG_SMPS_MODE__PBM
          PM_VREG_SMPS_MODE__AUTOMATIC

                   
RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC.     
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                        with the PMIC.                      
    PM_ERR_FLAG__SUCCESS              = SUCCESS.    
    
DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
  
===========================================================================*/
extern pm_err_flag_type  pm_vreg_smps_config (pm_vreg_smps_id_type vreg_smps, 
                                              pm_vreg_smps_mode_type mode);

/*===========================================================================

FUNCTION pm_vreg_smps_clock_sel                          EXTERNAL FUNCTION

DESCRIPTION
    Use this function to select the SMPS VREGs clock selection. 
  
  example:
     
     // Select RC as the SMPS VREGs clock source.          
     err |= pm_vreg_smps_clock_sel(PM_VREG_SMPS_CLK_SEL__RC);
     
     if (err != PM_ERR_FLAG__SUCCESS)
     {
        MSG_ERROR("API ERROR(0x%x) DETECTED.",err,0,0);
     }
                                                                                  
INPUT PARAMETERS
  1) Parameter name: clk_sel. 
     - SMPS VREGs clock selection.

     Parameter type: pm_vreg_smps_clk_sel_type (enum).
     - Valid Inputs:
        PM_VREG_SMPS_CLK_SEL__TCXO
        PM_VREG_SMPS_CLK_SEL__RC   (default)


RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init() 
  2) pm_init()
  3) pm_vreg_smps_tcxo_div_sel(), if we want to select TCXO as the 
     clock source.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_smps_clock_sel (
                                         pm_vreg_smps_clk_sel_type clk_sel);

/*===========================================================================

FUNCTION pm_vreg_smps_phase_sel                          EXTERNAL FUNCTION

DESCRIPTION
This function configures the phase splitter that splits the TCXO clock to the SMPS
regulators into four phases.  This distributes the transient current pulses
applied to VDD by the SMPS regulators and makes filtering easier.

INPUT PARAMETERS
  1) Parameter name: phase_sel. 
     - SMPS VREGs clock phase shift

     Parameter type: pm_vreg_smps_phase_sel_type (enum).
     - Valid Inputs:
        PM_VREG_SMPS_PHASE_SEL__0_0_0_0      = {0, 0, 0, 0} degrees phase shift
        PM_VREG_SMPS_PHASE_SEL__0_0_180_180  = {0, 0, 180, 180} degrees phase shift
        PM_VREG_SMPS_PHASE_SEL__0_90_0_90    = {0, 90, 0, 90} degrees phase shift
        PM_VREG_SMPS_PHASE_SEL__0_90_180_270 = {0, 90, 180, 270} degrees phase shift

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_smps_phase_sel (
                                            pm_vreg_smps_phase_sel_type phase_sel);

/*===========================================================================

FUNCTION pm_vreg_smps_tcxo_div_sel                       EXTERNAL FUNCTION

DESCRIPTION
      Use this function to select the SMPS VREGs TCXO clock source 
    divider. 
  
  example:

     // Configure the SMPS VREGs TCXO clock source divider to
     // 16.          
     err |= pm_vreg_smps_tcxo_div_sel(PM_VREG_SMPS_TCXO_DIV__16);

     // Select TCXO as the SMPS VREGs clock source.          
     err |= pm_vreg_smps_clock_sel(PM_VREG_SMPS_CLK_SEL__TCXO);
     
     if (err != PM_ERR_FLAG__SUCCESS)
     {
        MSG_ERROR("API ERROR(0x%x) DETECTED.",err,0,0);
     }
                                                                                  
INPUT PARAMETERS
  1) Parameter name: div. 
     - SMPS VREGs TCXO divider.

     Parameter type: pm_vreg_smps_tcxo_div_type (enum).
     - Valid Inputs:
        PM_VREG_SMPS_TCXO_DIV__2,     
        PM_VREG_SMPS_TCXO_DIV__4,     
        PM_VREG_SMPS_TCXO_DIV__6,     
        PM_VREG_SMPS_TCXO_DIV__8,     
        PM_VREG_SMPS_TCXO_DIV__10,
        PM_VREG_SMPS_TCXO_DIV__12,   (default)
        PM_VREG_SMPS_TCXO_DIV__14,
        PM_VREG_SMPS_TCXO_DIV__16,
        PM_VREG_SMPS_TCXO_DIV__18,
        PM_VREG_SMPS_TCXO_DIV__20,
        PM_VREG_SMPS_TCXO_DIV__22,
        PM_VREG_SMPS_TCXO_DIV__24,
        PM_VREG_SMPS_TCXO_DIV__26,
        PM_VREG_SMPS_TCXO_DIV__28,
        PM_VREG_SMPS_TCXO_DIV__30,
        PM_VREG_SMPS_TCXO_DIV__32,


RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_smps_tcxo_div_sel(
                                             pm_vreg_smps_tcxo_div_type div);

/*===========================================================================

FUNCTION pm_vreg_smps_individual_clock_enable           EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables individual clock to a SMPS voltage
regulator
                                                                                  
INPUT PARAMETERS
  1) Parameter name: which_vreg
     - which SMPS voltage with individual clocking capability 

     Parameter type: pm_vreg_smps_id_type (enum).
     - Valid Inputs:
        PM_VREG_SMPS_RF_ID
        PM_VREG_SMPS_BOOST_ID

  2) Parameter name: enable

     Parameter type: boolean
    - Valid Inputs:
        TRUE = enable individual clock to the SMPS
        FALSE = disable individual clock to the SMPS

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_smps_individual_clock_enable 
(
    pm_vreg_smps_id_type which_vreg,
    boolean enable
);

/*===========================================================================

FUNCTION pm_vreg_smps_individual_clock_div_sel           EXTERNAL FUNCTION

DESCRIPTION
    This function sets the switching frequency of a SMPS voltage regulator
with individual clocking capability 
                                                                                  
INPUT PARAMETERS
  1) Parameter name: which_vreg
     - which SMPS voltage with individual clocking capability 

     Parameter type: pm_vreg_smps_id_type (enum).
     - Valid Inputs:
        PM_VREG_SMPS_RF_ID
        PM_VREG_SMPS_BOOST_ID

  2) Parameter name: div
    - the SMPS switching frequency is equal to the TCXO frequency divided by
    the specified value

     Parameter type: pm_vreg_smps_tcxo_div_type
    - Valid Inputs:
       PM_VREG_SMPS_TCXO_DIV__2     
       PM_VREG_SMPS_TCXO_DIV__4     
       PM_VREG_SMPS_TCXO_DIV__6     
       PM_VREG_SMPS_TCXO_DIV__8     
       PM_VREG_SMPS_TCXO_DIV__10
       PM_VREG_SMPS_TCXO_DIV__12
       PM_VREG_SMPS_TCXO_DIV__14
       PM_VREG_SMPS_TCXO_DIV__16
       PM_VREG_SMPS_TCXO_DIV__18
       PM_VREG_SMPS_TCXO_DIV__20
       PM_VREG_SMPS_TCXO_DIV__22
       PM_VREG_SMPS_TCXO_DIV__24
       PM_VREG_SMPS_TCXO_DIV__26
       PM_VREG_SMPS_TCXO_DIV__28
       PM_VREG_SMPS_TCXO_DIV__30
       PM_VREG_SMPS_TCXO_DIV__32

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_smps_individual_clock_div_sel 
(
    pm_vreg_smps_id_type    which_vreg,
    pm_vreg_smps_tcxo_div_type  div
);

/*===========================================================================

FUNCTION pm_vreg_smps_low_batt_stable                  EXTERNAL FUNCTION

DESCRIPTION
   This function checks if the PMIC chip has improved stability for SMPS
regulators when the battery level is low 
  
INPUT PARAMETERS
    None

RETURN VALUE
  Return Value Type: boolean.
  
    TRUE  = PMIC has improved SMPS stability
    FALSE = PMIC does not have improved SMPS stability

DEPENDENCIES
  The following function must have been called:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
    None
===========================================================================*/
extern boolean  pm_vreg_smps_low_batt_stable( void );

/*===========================================================================

FUNCTION pm_vreg_buck_config_comp                       EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to configure the BUCK VREGs compensation 
  parameters.                                                           
  
INPUT PARAMETERS
  1) Parameter name: vreg_buck. 
     - Which BUCK Voltage Regulator we want to configure.

     Parameter type: pm_vreg_buck_id_type (enum)
     - Valid Inputs:
        PM_VREG_BUCK_PA_ID
        PM_VREG_BUCK_MSME_ID
        PM_VREG_BUCK_MSMC1_ID
        PM_VREG_BUCK_MSMC2_ID            Not supported in PM6650 

  2) Parameter name: cfg (pointer).
     - It contains the user's BUCK VREGs compensation parameters.

     Parameter type: pm_vreg_buck_cfg_cp_type (struct).
     - Valid Inputs: 
          1.1) Structure member: sawtooth_set.
               - Sawtooth current setting.
                         
               Type: uint8
               - Valid Inputs: 
                    Integer from 0 to 7, where:
                    
                    Sawtooth I = 4.5uA - (sawtooth_set * 0.5uA)                    
                       
          1.2) Structure member: gm_set.
               - Error Amp Transconductance setting.
                         
               Type: uint8
               - Valid Inputs: 
                    Integer from 0 to 15, where:
                    
                    gm = 1/((4 * gm_set) + 2000 Ohms)
               
          1.3) Structure member: resistor_set.
               - Compensation Resistor setting.
                         
               Type: uint8
               - Valid Inputs: 
                    Integer from 0 to 15, where:
                    
                    resistance = (10 + (20 * resistor_set)) kOhms.

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = cfg pointer is NULL.  
     PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = One of the compensation settings
                                         within the cfg pointer is out of 
                                         range.
     
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are LOCKED.
===========================================================================*/
extern pm_err_flag_type pm_vreg_buck_config_comp(
                                          pm_vreg_buck_id_type     vreg_buck,
                                          const pm_vreg_buck_cfg_cp_type *cfg);

/*===========================================================================

FUNCTION pm_vreg_pull_down_switch                     EXTERNAL FUNCTION

DESCRIPTION
    This function Enable/Disable the VREG Active Pull Down. Note, the
  active pull down will only become active when the corresponding VREG
  is disabled.
  
  example:

     // Configure the RUIM VREG pull down to be active when the RUIM VREG 
     // is disabled.
     err |= pm_vreg_pull_down_switch(PM_ON_CMD,PM_VREG_PDOWN_RUIM_ID);
     
     if (err != PM_ERR_FLAG__SUCCESS)
     {
        MSG_ERROR("API ERROR(0x%x) DETECTED.",err,0,0);
     }
                                                                                  
INPUT PARAMETERS
  1) Parameter name: vreg_cmd.
     - Enable/Disable the VREG Active Pull Down.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
        PM_OFF_CMD = Pull down is always inactive.
        PM_ON_CMD  = Pull down is active when the VREG is disabled.

  2) Parameter name: vreg. 
     - Which VREG pull down we want to Enable/Disable.

     Parameter type: pm_vreg_pdown_id_type (enum).
     - Valid Inputs:
        PM_VREG_PDOWN_MSMP_ID
        PM_VREG_PDOWN_MSMA_ID
        PM_VREG_PDOWN_WLAN_ID
        PM_VREG_PDOWN_RFRX2_ID
        PM_VREG_PDOWN_RFRX1_ID
        PM_VREG_PDOWN_RFTX_ID
        PM_VREG_PDOWN_SYNT_ID
        PM_VREG_PDOWN_TCXO_ID
        PM_VREG_PDOWN_USB_ID
        PM_VREG_PDOWN_MMC_ID
        PM_VREG_PDOWN_RUIM_ID
        PM_VREG_PDOWN_MSMS_ID
        PM_VREG_PDOWN_MEM_ID
        PM_VREG_PDOWN_CAM_ID
        PM_VREG_PDOWN_GP_ID
        PM_VREG_PDOWN_MSMC0_ID
        PM_VREG_PDOWN_PA_ID
        PM_VREG_PDOWN_MSME_ID
        PM_VREG_PDOWN_MSMC1_ID
        PM_VREG_PDOWN_MSMC2_ID        

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_pull_down_switch (
                                                pm_switch_cmd_type    vreg_cmd,
                                                pm_vreg_pdown_id_type vreg);
/*~ FUNCTION pm_vreg_pull_down_switch */

/*===========================================================================

FUNCTION pm_vreg_pa_dvs_config                           EXTERNAL FUNCTION

DESCRIPTION
      This function Configure the VREG PA internal logic and ensure that the 
  external FET is always open every time that the VREG_PA regulator is 
  enabled. Please refer to the following document for more information.
        
    PM6650 Device Revision Guide, 80-V5773-6
   
INPUT PARAMETERS
  1) Parameter name: . 
     - PA DVS VREG PA configuration settings.

     Parameter type: pm_vreg_pa_dvs_config_type (struct, pointer).
     - Valid Inputs:
           discharge_t = Load capacitor discharge time.
           ton         = Internal PFET turned on time (usecs).
           cmd         = Enable/Disable config settings.
                PM_OFF_CMD
                PM_ON_CMD


RETURN VALUE
  None.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  None.  
===========================================================================*/
extern void pm_vreg_pa_dvs_config(const pm_vreg_pa_dvs_config_type* config_tb);

/*===========================================================================

FUNCTION pm_vreg_status_get                     EXTERNAL FUNCTION

DESCRIPTION
    This function returns the real-time status of the voltage regulators.
    The return value is a bit-mapped variable where each bit represents 
    the on (1) or off (0) state of the regulators enumerated in 
    pm_vreg_mask_type
    vreg_rt_status is initialized during PMIC init through a call
    to pm_vreg_status_initialize()
    bits are set or cleared in pm_vreg_control
                                                                                  
INPUT PARAMETERS
  none
RETURN VALUE
  Return Value Type: pm_vreg_mask_type
  each bit represents one voltage regulator
DEPENDENCIES
  pm_init() must have been called.

SIDE EFFECTS
  none
===========================================================================*/
extern pm_vreg_mask_type pm_vreg_status_get(void);

/*=========================================================================*/
/**
 * This API supports both bypass and subregulation modes.  One or more LDOs
 * can be connected to an SMPS.  The PMIC driver needs to be told what the 
 * associations are so that the SMPS can be managed.
 * 
 * If all of the LDOs that are connected to an SMPS are turned off, then
 * the SMPS can be turned off.
 * 
 * Likewise, if one of the LDOs connected to an SMPS is turned on, then
 * the SMPS must be turned on.
 * 
 * @param ldo    The LDO that is to be associated with parameter smps
 * @param smps
 * 
 * @return pm_err_flag_type
 *
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
 *         with the PMIC.
 *         PM_ERR_FLAG__SUCCESS              = SUCCESS.
 *
 * @example To associate LDO_RFTX2 with SMPS_RF2:
 *          pm_vreg_ldo_smps_group(PM_VREG_LDO_RFTX2_ID, PM_VREG_SMPS_RF2_ID);
*/
/*=========================================================================*/
pm_err_flag_type pm_vreg_ldo_smps_group(pm_vreg_ldo_id_type ldo,
                                        pm_vreg_smps_id_type smps);
/*=========================================================================*/
/**
 * Set bypass mode on for a given LDO.
 * 
 * @param bypass_ldo  Which LDO is being set to bypass mode
 * @param bypass_smps Which SMPS is used to supply the LDO
 * 
 * @return pm_err_flag_type
 *         
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
 *         with the PMIC.
 *         PM_ERR_FLAG__SUCCESS              = SUCCESS.
 * 
 * @example To enable bypass mode with LDO_RFTX2:
 *          pm_vreg_ldo_bypass_set(PM_VREG_LDO_RFTX2_ID);
 */
/*=========================================================================*/
extern pm_err_flag_type pm_vreg_ldo_bypass_set(pm_vreg_ldo_id_type bypass_ldo);
/*=========================================================================*/
/**
 * Clear bypass mode for the given LDO and set the associated SMPS to
 * PM_VREG_SMPS_ID_INVALID.
 * 
 * @param bypass_ldo
 * 
 * @return pm_err_flag_type
 *         
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
 *         with the PMIC.
 *         PM_ERR_FLAG__SUCCESS              = SUCCESS.
 * 
 * @example To disable bypass mode for the RFTX2 LDO:
 *          pm_vreg_ldo_bypass_clear(PM_VREG_LDO_RFTX2_ID);
 */
/*=========================================================================*/
extern pm_err_flag_type pm_vreg_ldo_bypass_clear(pm_vreg_ldo_id_type bypass_ldo);

/*===========================================================================
FUNCTION pm_vreg_ldo_current_limit_disable                     EXTERNAL FUNCTION

DESCRIPTION
    This function will Enable/Disable an LDO VREG output current limit. Note, this
  API only supports PM6658/VREG_MSME
    Current limit is enabled by default.
    Disable by calling  
        pm_vreg_ldo_current_limit_enable (PM_OFF_CMD, PM_VREG_MSME_ID)                                                                           
                                                                                  
INPUT PARAMETERS
  1) Parameter name: vreg_cmd.
     - Enable/Disable the VREG Active Pull Down.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
        PM_OFF_CMD = Disable the LDO current limit
        PM_ON_CMD  = Enable the LDO current limit

  2) Parameter name: vreg. 
     - Which VREG current limit we want to Enable/Disable.

     Parameter type: pm_vreg_pdown_id_type (enum).
     - Valid Inputs:
        PM_VREG_PDOWN_MSME_ID

RETURN VALUE
  Return Value Type: pm_err_flag_type.
  
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
     PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.  
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                         version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                         with the PMIC.          
     PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_ldo_current_limit_enable (
                                                pm_switch_cmd_type    vreg_cmd,
                                                pm_vreg_id_type vreg);



/**
 * FUNCTION pm_vreg_smps_switch_size_set
 * 
 * This API will set the SMPS switch size.  Smaller switch
 * size is used to improve SMPS efficiency at
 * low current loads.  Larger switch size is required for
 * higher current loads.
 * 
 * @param smps        PM_VREG_SMPS_RF1_ID
 *                    PM_VREG_SMPS_RF2_ID
 *                    PM_VREG_SMPS_MSMC_ID
 * @param switch_size PM_SMPS_SWITCH_SIZE_1_8 1/8 of switch size
 *                    PM_SMPS_SWITCH_SIZE_1_4 1/4 of switch size
 *                    PM_SMPS_SWITCH_SIZE_1_2 1/2 of switch size
 *                    PM_SMPS_SWITCH_SIZE_1_1 1/1 of switch size
 * 
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
 *         PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
 *         with the PMIC.
 *         PM_ERR_FLAG__SUCCESS              = SUCCESS.
 * @example To set the SMPS switch size to 1/2 for VREG_RF2:
 * pm_vreg_smps_switch_size(PM_VREG_SMPS_RF2_ID, PM_SMPS_SWITCH_SIZE_1_2);
 */
extern pm_err_flag_type pm_vreg_smps_switch_size_set(
                                                pm_vreg_smps_id_type smps,
                                                pm_vreg_smps_switch_size_type switch_size);
/**
 * Function pm_uvlo_threshold_set()
 * 
 * This API is used to set the undervoltage lockout threshold 
 * voltage.
 * 
 * @param mVolts undervoltage lockout threshold voltage.  Valid values range from
 *               1500 to 3050.
 * 
 * @return pm_err_flag_type
 *         
 *         PM_ERR_FLAG__SUCCESS  - The function completed
 *         successfully.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED - UVLO is not
 *         supported by this version or model of PMIC
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE - The requested
 *         voltage is too high or too low.
 *         PM_ERR_FLAG__SBI_OPT_ERR - Error communicating with
 *         the PMIC
 * 
 * @example pm_err_flag_type err;
 * *set the UVLO threshold to 2550 mV
 * err = pm_uvlo_threshold_set(2550);
 */
pm_err_flag_type pm_uvlo_threshold_set(
    uint16 mVolts);
/**
 * Function pm_uvlo_delay_set()
 * 
 * This API is used to set the undervoltage lockout detection delay.
 * 
 * @param mSeconds Time delay for UVLO detection.  Valid values range from
 *                 0 to 64 mS (0, 1, 2, 4, 8, 16, 32, 64)
 * 
 * @return pm_err_flag_type
 *         
 *         PM_ERR_FLAG__SUCCESS  - The function completed
 *         successfully.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED - UVLO is not
 *         supported by this version or model of PMIC
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE - The requested
 *         delay is too high.
 *         PM_ERR_FLAG__SBI_OPT_ERR - Error communicating with
 *         the PMIC
 * @example pm_err_flag_type err;
 * *set the UVLO delay to 32 mS
 * err = pm_uvlo_delay_set(32);
 */
pm_err_flag_type pm_uvlo_delay_set(
    uint16 mSeconds);

/*===========================================================================
FUNCTION pm_vreg_ncp_sample_comp_mode_enable                    EXTERNAL FUNCTION

DESCRIPTION
  Use this API to enable the sample-comparator mode for VREG_NCP. This function will 
  enable the comparator mode and allow us to set the switching frequency.
                                          
PARAMETERS
  1) Parameter name: OnOff
     - Enable or Disable the sample comparator output.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
     
            PM_OFF_CMD
            PM_ON_CMD

  2) Parameter name: freq
     - Select switching frequency.

     Parameter type: pm_ncp_switching_freq_type (enum).
     - Valid Inputs:
            PM_NCP_FREQ_9_6MHZ,
            PM_NCP_FREQ_4_8MHZ,
            PM_NCP_FREQ_3_2MHZ,  
            PM_NCP_FREQ_2_4MHZ,
            PM_NCP_FREQ_1_92MHZ,
            PM_NCP_FREQ_1_6MHZ, //Default
            PM_NCP_FREQ_1_37MHZ,
            PM_NCP_FREQ_1_2MHZ,
   

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_vreg_ncp_sample_comp_mode_enable(
                                             pm_switch_cmd_type           OnOff,
                                             pm_ncp_switching_freq_type      freq);


/**
 * Function: pm_vreg_smps_pulse_skipping_enable(
 *                                         pm_vreg_smps_id_type smps_vreg,
 *                                         boolean              enable)
 * 
 * Description:  Enable or disable SMPS pulse skipping
 * 
 * @param smps_vreg The SMPS that is to be controlled:
 *                  PM_VREG_SMPS_S1_ID
 *                  PM_VREG_SMPS_S2_ID
 * @param enable    TRUE--enable pulse skipping
 *                  FALSE--disable pulse skipping
 * 
 * @return pm_err_flag_type
 * @example pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID, TRUE);
 * pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID, FALSE);
 * pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID, TRUE);
 * pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID, FALSE);
 */
extern pm_err_flag_type pm_vreg_smps_pulse_skipping_enable(
                                          pm_vreg_smps_id_type smps_vreg,
                                          boolean              enable);

#endif /* PM60X0VLT_REG_H */
