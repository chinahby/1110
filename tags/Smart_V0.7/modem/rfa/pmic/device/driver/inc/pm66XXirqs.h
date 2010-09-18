#ifndef PM66XXIRQS_H
#define PM66XXIRQS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M 6 6 X X   I R Q   S E R V I C E S
                      H E A D E R    F I L E

GENERAL DESCRIPTION
    This file contains functions and variable prototypes as well as type
  definitions to support interrupt services for Qualcomm's Power Manager ICs.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003-2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm66XXirqs.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/08   jtn/apu Integrate 8200 target branch changes
08/22/08   jtn     Added 2 new IRQ IDs for PMIC4 HSED module
06/27/08   jtn     Merge changes from QSC1100 branch
06/20/08   jtn     Added new IRQ IDs for PMIC4 charger
03/01/08   jtn     Changed PM_NUM_OF_IRQ_HDL_HAN 
02/27/08   jtn     Added support for Kip PMIC
02/22/08   jtn     Added typedef void (*pm_isr_ptr_multi_type)(void *)
                   externs for multiple ISR code
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
10/11/07   jnoblet Added Han MPP IRQs
(end QSC6270 changes) 

06/08/07   cng     Added OVP IRQ enum type
04/23/07    th     Addition of pm_get_wake_up_status()
06/12/05   cng     Added enums for headset send/end detect interrupt for PM6640
11/09/05   cng     Added PM6620 support
10/24/05   cng     Added Panoramix support
09/20/05   fpe     Added 6050 support
07/25/05   cng     PM7500 support 
10/26/04   rmd     Updated the comments for the MPP real time status.
06/24/04   rmd     Added IRQ debug ISR debug services.
05/04/04   rmd     Added support for the MSM6000.
04/13/04   rmd     Added support for the VCP IRQ.
02/13/04   rmd     Added comments to the IRQ table.
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
01/19/04   rmd     Added support for the PM6610.
11/11/03   Vish    Changed USB related interrupt assignments for B1 version.
10/13/03   rmd     Added support for the MSM6100.
07/07/03   rmd     Created.
===========================================================================*/

/* which gpio is the PMIC IRQ connected to */
#if defined(T_MDM8200)
#define PM_INT                  GPIO_INT_134
#elif defined(T_MSM7500)
#define PM_INT                  GPIO_INT_24
#elif defined(T_MSM6500)
#define PM_INT                  GPIO_INT_40
#elif defined(T_PMIC_MSM6250_CONFIG)
#define PM_INT                  GPIO_INT_40
#elif defined(T_MSM6100)
#define PM_INT                  GPIO_INT_40
#elif defined(T_MSM6025) || defined(T_MSM6050)
#define PM_INT                  GPIO_INT_43
#elif defined(T_MSM6000)
#define PM_INT                  GPIO_INT_10
#else
#error:  PMIC irq GPIO has not been defined for this target.
#endif           

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* IRQ handler type definition */
typedef void (*pm_isr_ptr_type)(void);
/* new type for multi-handler IRQ scheme */
typedef void (*pm_isr_ptr_multi_type)(void *);

/* PMIC IRQ blocks definition */
typedef enum
{
    PM_IRQ_BLK1,
    PM_IRQ_BLK2,
    PM_IRQ_BLK3,
    PM_IRQ_BLK4,
    PM_IRQ_BLK5,
    PM_IRQ_BLK6,
    PM_IRQ_BLK7, 
    PM_IRQ_BLK0,
    PM_IRQ_BLK8,
    PM_IRQ_BLK9,
    PM_IRQ_BLK10,
    PM_IRQ_BLK11,

    PM_IRQ_NUM_MSTR_BLK_PM6620 = PM_IRQ_BLK4 + 1,
    PM_IRQ_NUM_MSTR_BLK_PANORAMIX = PM_IRQ_BLK7 + 1,
    PM_IRQ_NUM_MSTR_BLK_PM6640 = PM_IRQ_NUM_MSTR_BLK_PANORAMIX,
    PM_IRQ_NUM_MSTR_BLK_PM6650 = PM_IRQ_NUM_MSTR_BLK_PANORAMIX,
    PM_IRQ_NUM_MSTR_BLK_PM6658 = PM_IRQ_NUM_MSTR_BLK_PANORAMIX,
    PM_IRQ_NUM_MSTR_BLK_EPIC = PM_IRQ_NUM_MSTR_BLK_PANORAMIX,
    PM_IRQ_NUM_MSTR_BLK_PM7500 = PM_IRQ_BLK9 + 1,
    PM_IRQ_NUM_MSTR_BLK_KIP = PM_IRQ_BLK11 + 1,
    PM_IRQ_NUM_MSTR_BLK_HAN = PM_IRQ_BLK11 + 1
} pm_irq_blk_type;

/* PMIC IRQs */
typedef enum
{
    /* 0) Charger valid IRQ.*/
    PM_VALID_CHG_IRQ_HDL,
    /* 1) Charger Invalid IRQ. */
    PM_INVALID_CHG_IRQ_HDL,
    /* 2) USB Charger valid IRQ. */
    PM_VALID_USB_CHG_IRQ_HDL,
    /* 2) USB OVP VBUS valid IRQ (level triggered) */
    PM_USB_OVP_VBUS_VALID_IRQ_HDL = PM_VALID_USB_CHG_IRQ_HDL,
    /* 3) USB Charger Invalid IRQ. */
    PM_INVALID_USB_CHG_IRQ_HDL,
    /* 3) USB OVP VBUS Invalid IRQ (level triggered) */
    PM_USB_OVP_VBUS_INVALID_IRQ_HDL = PM_INVALID_USB_CHG_IRQ_HDL,
    /* 4) Charger in current limiting  IRQ.*/
    PM_CHG_I_LIMITING_IRQ_HDL,
    /* 5) Charger in Power limiting IRQ. */
    PM_CHG_P_LIMITING_IRQ_HDL,
    /* 6) Pulse Charger done IRQ. */
    PM_PULSE_CHG_DONE_IRQ_HDL,
    /* 7) VCP (voltage collapse protection) IRQ. */
    PM_VCP_IRQ_HDL,
    /* 8) Temperature Status Changed IRQ. */
    PM_T_STAT_CHANGED_IRQ_HDL,
    /* 9) Battery Status Changed IRQ. */
    PM_BAT_STAT_CHANGED_IRQ_HDL,
    /* 10) Upper VBATT max IRQ. */
    PM_VBAT_DET_IRQ_HDL,
    /* 11)Battery fet turn on IRQ. */
    PM_BAT_FET_ON_IRQ_HDL,
    /* 12)Keypad power key pressed IRQ. */
    PM_KPD_PWR_KEY_ON_IRQ_HDL,
    /* 13)Keypad power key released IRQ. */
    PM_KPD_PWR_KEY_OFF_IRQ_HDL,
    /* 14 Headset switch pressed IRQ */
    PM_HS_SED_OUT_IRQ_HDL,
    /* 15 Headset switch released IRQ */
    PM_HS_SED_OUTB_IRQ_HDL,
    /* 16)Real time clock alarm IRQ. */
    PM_RTC_ALRM_IRQ_HDL,
    /* 17)Crystal oscillator halted IRQ. */
    PM_OSC_HALT_IRQ_HDL,
    /* 18)Power reset IRQ. */
    PM_PWR_RST_IRQ_HDL,
    /* 19)Sudden momentary pwr loss IRQ. */
    PM_SMPL_IRQ_HDL,
    /* 20)Cable insertion IRQ. */
    PM_CABLE_IN_IRQ_HDL,
    /* 21)Device over-temperature IRQ. */
    PM_OVER_TEMP_IRQ_HDL,
    /* 22)Watchdog Timeout IRQ. */
    PM_WDOG_TOUT_IRQ_HDL,
    /* 23)Keypad power on event occurred IRQ. */
    PM_KPD_PWRON_EVENT_IRQ_HDL,
    /* 24)USB bus valid (Rising Edge) IRQ. */
    PM_USB_BUS_VALID_RE_IRQ_HDL,
    /* 25)USB session valid (Rising Edge) IRQ. */
    PM_USB_SN_VALID_RE_IRQ_HDL,
    /* 26)USB D+ HI (Rising Edge) IRQ. */
    PM_USB_D_PLUS_HI_RE_IRQ_HDL,
    /* 27)USB ID grounded (Rising Edge) IRQ. */
    PM_USB_ID_GND_RE_IRQ_HDL,
    /* 28)USB D- HI (Rising Edge) IRQ. */
    PM_USB_D_MINUS_HI_RE_IRQ_HDL,
    /* 29)USB ID floating (Rising Edge) IRQ. */
    PM_USB_ID_FLOATING_RE_IRQ_HDL,
    /* 30)USB B disconnect A connect (Rising Edge) IRQ. */
    PM_USB_BOFF_AON_RE_IRQ_HDL,
    /* 31)USB carkit interrupt (Rising Edge) IRQ. */
    PM_USB_CARKIT_INT_RE_IRQ_HDL,
    /* 32)USB bus valid (Falling Edge) IRQ. */
    PM_USB_BUS_VALID_FE_IRQ_HDL,
    /* 33)USB session valid (Falling Edge) IRQ. */
    PM_USB_SN_VALID_FE_IRQ_HDL,
    /* 34)USB D+ HI (Falling Edge) IRQ. */
    PM_USB_D_PLUS_HI_FE_IRQ_HDL,
    /* 35)USB ID grounded (Falling Edge) IRQ. */
    PM_USB_ID_GND_FE_IRQ_HDL,
    /* 36)USB D- HI (Falling Edge) IRQ. */
    PM_USB_D_MINUS_HI_FE_IRQ_HDL,
    /* 37)USB ID floating (Falling Edge) IRQ. */
    PM_USB_ID_FLOATING_FE_IRQ_HDL,
    /* 38)USB B disconnect A connect (Falling Edge) IRQ. */
    PM_USB_BOFF_AON_FE_IRQ_HDL,
    /* 39)USB carkit interrupt (Falling Edge) IRQ. */
    PM_USB_CARKIT_INT_FE_IRQ_HDL,
    /* 40)MPP1 State Changed */
    PM_MPP01_CHGED_ST_IRQ_HDL,
    /* 41)MPP2 State Changed */ 
    PM_MPP02_CHGED_ST_IRQ_HDL,
    /* 42)MPP3 State Changed */ 
    PM_MPP03_CHGED_ST_IRQ_HDL,
    /* 43)MPP4 State Changed */ 
    PM_MPP04_CHGED_ST_IRQ_HDL,
    /* 44)MPP5 State Changed */ 
    PM_MPP05_CHGED_ST_IRQ_HDL,
    /* 45)MPP6 State Changed */ 
    PM_MPP06_CHGED_ST_IRQ_HDL,
    /* 46)MPP7 State Changed */ 
    PM_MPP07_CHGED_ST_IRQ_HDL,
    /* 47)MPP8 State Changed */ 
    PM_MPP08_CHGED_ST_IRQ_HDL,
    /* 48)MPP9 State Changed */ 
    PM_MPP09_CHGED_ST_IRQ_HDL,
    /* 49)MPP10 State Changed */ 
    PM_MPP10_CHGED_ST_IRQ_HDL,
    /* 50)MPP11 State Changed */ 
    PM_MPP11_CHGED_ST_IRQ_HDL,
    /* 51)MPP12 State Changed */ 
    PM_MPP12_CHGED_ST_IRQ_HDL,
    /* 52)MPP13 State Changed */ 
    PM_MPP13_CHGED_ST_IRQ_HDL,
    /* 53)MPP14 State Changed */ 
    PM_MPP14_CHGED_ST_IRQ_HDL,
    /* 54)MPP15 State Changed */ 
    PM_MPP15_CHGED_ST_IRQ_HDL,
    /* 55)MPP16 State Changed */ 
    PM_MPP16_CHGED_ST_IRQ_HDL,
    /* 56)MPP17 State Changed */ 
    PM_MPP17_CHGED_ST_IRQ_HDL,
    /* 57)MPP18 State Changed */ 
    PM_MPP18_CHGED_ST_IRQ_HDL,
    /* 58)MPP19 State Changed */ 
    PM_MPP19_CHGED_ST_IRQ_HDL,
    /* 59)MPP20 State Changed */ 
    PM_MPP20_CHGED_ST_IRQ_HDL,
    /* 60)MPP21 State Changed */ 
    PM_MPP21_CHGED_ST_IRQ_HDL,
    /* 61)MPP22 State Changed */ 
    PM_MPP22_CHGED_ST_IRQ_HDL,
    /* 62 normally open headset switch change */
    PM_DETOUT_NO_CHGED_IRQ_HDL,
    /* 63 normally closed headset switch change */
    PM_DETOUT_NC_CHGED_IRQ_HDL,
    /* 64)Video Amp Load Detection */
    PM_VIDEOAMP_LOAD_DET_IRQ_HDL,
    /* 65)Video Amp Unload Detection */
    PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL,
    /* PMIC4 charger interrupts */
    /* 66 auto trickle charging completed successfully */
    PM_ATCDONE_IRQ_HDL, 
    /* 67 auto trickle charigng failed */
    PM_ATCFAIL_IRQ_HDL,
    /* 68 auto charging has completed successfully */
    PM_CHGDONE_IRQ_HDL,
    /* 69 auto charging has failed */
    PM_CHGFAIL_IRQ_HDL,
    /* 70 charger state machine has changed states */
    PM_CHGSTATE_IRQ_HDL,
    /* 71 charger is fast charging */
    PM_FASTCHG_IRQ_HDL,
    /* 72 charge termination current has been reached */
    PM_ICHGEND_IRQ_HDL,
    /* 73 thermistor enabled but is either shorted or open */
    PM_BATTCONNECT_IRQ_HDL,
    /* 74 thermistor is enabled and temperature is either too hot or too cold */
    PM_BATTTEMP_IRQ_HDL,
    /* 75 charger temperature exceeded pd_temp_high2 */
    PM_CHGHOT_IRQ_HDL,
    /* 76 charger in temperature limit */
    PM_CHGTLIMIT_IRQ_HDL,
    /* 77 charger removal detected */
    PM_CHGGONE_IRQ_HDL,
    /* 78 major VDD collapse detected */
    PM_VCPMAJOR_IRQ_HDL,
    /* 79 */
    PM_NUM_OF_IRQ_HDL,

    /* Use this enum to set all the IRQs to one isr */
    /* Number of IRQ handlers for PM6650 */ 
    PM_NUM_OF_IRQ_HDL_PM6650 = PM_MPP12_CHGED_ST_IRQ_HDL + 1,
    PM_NUM_OF_IRQ_HDL_PM6658 = PM_NUM_OF_IRQ_HDL_PM6650,
    /* Number of IRQ handlers for PANORAMIX, PM6640 and PM6620 */ 
    PM_NUM_OF_IRQ_HDL_PANORAMIX = PM_MPP02_CHGED_ST_IRQ_HDL + 1,
    PM_NUM_OF_IRQ_HDL_PM6640 = PM_NUM_OF_IRQ_HDL_PANORAMIX,
    PM_NUM_OF_IRQ_HDL_PM6620 = PM_NUM_OF_IRQ_HDL_PANORAMIX,
    PM_NUM_OF_IRQ_HDL_EPIC   =  PM_MPP04_CHGED_ST_IRQ_HDL + 1,
    /* Number of IRQ handlers for PM75XX */
    PM_NUM_OF_IRQ_HDL_PM7500 = PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL + 1,
    PM_NUM_OF_IRQ_HDL_HAN = PM_VCPMAJOR_IRQ_HDL + 1,
    PM_NUM_OF_IRQ_HDL_KIP = PM_VCPMAJOR_IRQ_HDL + 1  
}pm_irq_hdl_type;

/* IRQ debug ISR type. */
typedef void (*pm_isr_debug_func_type)(pm_irq_hdl_type);

/* PMIC REAL TIME STATUSs */
typedef enum
{
    /* Charger valid REAL TIME STATUS.*/
    PM_VALID_CHG_RT_ST          = PM_VALID_CHG_IRQ_HDL,
    /* Charger Invalid REAL TIME STATUS. */
    PM_INVALID_CHG_RT_ST        = PM_INVALID_CHG_IRQ_HDL,
    /* USB Charger valid REAL TIME STATUS. */
    PM_VALID_USB_CHG_RT_ST      = PM_VALID_USB_CHG_IRQ_HDL,
    /* USB OVP VBUS valid REAL TIME STATUS. */
    PM_USB_OVP_VBUS_VALID_RT_ST = PM_USB_OVP_VBUS_VALID_IRQ_HDL,
    /* USB Charger Invalid REAL TIME STATUS. */
    PM_INVALID_USB_CHG_RT_ST    = PM_INVALID_USB_CHG_IRQ_HDL,
    /* USB OVP VBUS Invalid REAL TIME STATUS. */
    PM_USB_OVP_VBUS_INVALID_RT_ST = PM_USB_OVP_VBUS_INVALID_IRQ_HDL,
    /* Charger in current limiting  REAL TIME STATUS.*/
    PM_CHG_I_LIMITING_RT_ST     = PM_CHG_I_LIMITING_IRQ_HDL,
    /* Charger in Power limiting REAL TIME STATUS. */
    PM_CHG_P_LIMITING_RT_ST     = PM_CHG_P_LIMITING_IRQ_HDL,
    /* Pulse Charger done REAL TIME STATUS. */
    PM_PULSE_CHG_DONE_RT_ST     = PM_PULSE_CHG_DONE_IRQ_HDL,
    /* Voltage collapse protection (VCP) REAL TIME STATUS. */
    PM_VCP_IRQ_RT_ST            = PM_VCP_IRQ_HDL,
    /* Temperature Status Changed REAL TIME STATUS. */
    PM_T_STAT_CHANGED_RT_ST     = PM_T_STAT_CHANGED_IRQ_HDL,
    /* Battery Status Changed REAL TIME STATUS. */
    PM_BAT_STAT_CHANGED_RT_ST   = PM_BAT_STAT_CHANGED_IRQ_HDL,
    /* Upper VBATT max REAL TIME STATUS. */
    PM_VBAT_DET_RT_ST           = PM_VBAT_DET_IRQ_HDL,
    /* Battery fet turn on REAL TIME STATUS. */
    PM_BAT_FET_ON_RT_ST         = PM_BAT_FET_ON_IRQ_HDL,
    /* Keypad power key pressed REAL TIME STATUS. */
    PM_KPD_PWR_KEY_ON_RT_ST     = PM_KPD_PWR_KEY_ON_IRQ_HDL,
    /* Keypad power key released REAL TIME STATUS. */
    PM_KPD_PWR_KEY_OFF_RT_ST    = PM_KPD_PWR_KEY_OFF_IRQ_HDL,
    /* Headset switch pressed REAL TIME STATUS */
    PM_HS_SED_OUT_IRQ_HDL_RT_ST = PM_HS_SED_OUT_IRQ_HDL,
    /* Headset switch released REAL TIME STATUS */
    PM_HS_SED_OUTB_IRQ_HDL_RT_ST = PM_HS_SED_OUTB_IRQ_HDL,
    /* Real time clock alarm REAL TIME STATUS. */
    PM_RTC_ALRM_RT_ST           = PM_RTC_ALRM_IRQ_HDL,
    /* Crystal oscillator halted REAL TIME STATUS. */
    PM_OSC_HALT_RT_ST           = PM_OSC_HALT_IRQ_HDL,
    /* Power reset REAL TIME STATUS. */
    PM_PWR_RST_RT_ST            = PM_PWR_RST_IRQ_HDL,
    /* Sudden momentary pwr loss REAL TIME STATUS. */
    PM_SMPL_RT_ST               = PM_SMPL_IRQ_HDL,
    /* Cable insertion REAL TIME STATUS. */
    PM_CABLE_IN_RT_ST           = PM_CABLE_IN_IRQ_HDL,
    /* Device over-temperature REAL TIME STATUS. */
    PM_OVER_TEMP_RT_ST          = PM_OVER_TEMP_IRQ_HDL,
    /* Watchdog Timeout REAL TIME STATUS. */
    PM_WDOG_TOUT_RT_ST          = PM_WDOG_TOUT_IRQ_HDL,
    /* Keypad power on event occurred REAL TIME STATUS. */
    PM_KPD_PWRON_EVENT_RT_ST    = PM_KPD_PWRON_EVENT_IRQ_HDL,
    /* USB bus valid REAL TIME STATUS. */
    PM_USB_BUS_VALID_RT_ST      = PM_USB_BUS_VALID_RE_IRQ_HDL,
    /* USB session valid REAL TIME STATUS. */
    PM_USB_SN_VALID_RT_ST       = PM_USB_SN_VALID_RE_IRQ_HDL,
    /* USB D+ HI REAL TIME STATUS. */
    PM_USB_D_PLUS_HI_RT_ST      = PM_USB_D_PLUS_HI_RE_IRQ_HDL,
    /* USB ID grounded REAL TIME STATUS. */
    PM_USB_ID_GND_RT_ST         = PM_USB_ID_GND_RE_IRQ_HDL,
    /* USB D- HI REAL TIME STATUS. */
    PM_USB_D_MINUS_HI_RT_ST     = PM_USB_D_MINUS_HI_RE_IRQ_HDL,
    /* USB ID floating REAL TIME STATUS. */
    PM_USB_ID_FLOATING_RT_ST    = PM_USB_ID_FLOATING_RE_IRQ_HDL,
    /* USB B disconnect A connect REAL TIME STATUS. */
    PM_USB_BOFF_AON_RT_ST       = PM_USB_BOFF_AON_RE_IRQ_HDL,
    /* USB carkit interrupt REAL TIME STATUS. */
    PM_USB_CARKIT_INT_RT_ST     = PM_USB_CARKIT_INT_RE_IRQ_HDL,
    /* MPP State Changed */
    PM_MPP01_CHGED_RT_ST        = PM_MPP01_CHGED_ST_IRQ_HDL,
    PM_MPP02_CHGED_RT_ST        = PM_MPP02_CHGED_ST_IRQ_HDL,
    PM_MPP03_CHGED_RT_ST        = PM_MPP03_CHGED_ST_IRQ_HDL,
    PM_MPP04_CHGED_RT_ST        = PM_MPP04_CHGED_ST_IRQ_HDL,
    PM_MPP05_CHGED_RT_ST        = PM_MPP05_CHGED_ST_IRQ_HDL,
    PM_MPP06_CHGED_RT_ST        = PM_MPP06_CHGED_ST_IRQ_HDL,
    PM_MPP07_CHGED_RT_ST        = PM_MPP07_CHGED_ST_IRQ_HDL,
    PM_MPP08_CHGED_RT_ST        = PM_MPP08_CHGED_ST_IRQ_HDL,
    PM_MPP09_CHGED_RT_ST        = PM_MPP09_CHGED_ST_IRQ_HDL,
    PM_MPP10_CHGED_RT_ST        = PM_MPP10_CHGED_ST_IRQ_HDL,
    PM_MPP11_CHGED_RT_ST        = PM_MPP11_CHGED_ST_IRQ_HDL,
    PM_MPP12_CHGED_RT_ST        = PM_MPP12_CHGED_ST_IRQ_HDL,
    PM_MPP13_CHGED_RT_ST        = PM_MPP13_CHGED_ST_IRQ_HDL,
    PM_MPP14_CHGED_RT_ST        = PM_MPP14_CHGED_ST_IRQ_HDL,
    PM_MPP15_CHGED_RT_ST        = PM_MPP15_CHGED_ST_IRQ_HDL,
    PM_MPP16_CHGED_RT_ST        = PM_MPP16_CHGED_ST_IRQ_HDL,
    PM_MPP17_CHGED_RT_ST        = PM_MPP17_CHGED_ST_IRQ_HDL,
    PM_MPP18_CHGED_RT_ST        = PM_MPP18_CHGED_ST_IRQ_HDL,
    PM_MPP19_CHGED_RT_ST        = PM_MPP19_CHGED_ST_IRQ_HDL,
    PM_MPP20_CHGED_RT_ST        = PM_MPP20_CHGED_ST_IRQ_HDL,
    PM_MPP21_CHGED_RT_ST        = PM_MPP21_CHGED_ST_IRQ_HDL,
    PM_MPP22_CHGED_RT_ST        = PM_MPP22_CHGED_ST_IRQ_HDL,
    PM_DETOUT_NO_CHGED_RT_ST    = PM_DETOUT_NO_CHGED_IRQ_HDL,
    PM_DETOUT_NC_CHGED_RT_ST    = PM_DETOUT_NC_CHGED_IRQ_HDL,

    /* Video Amp Load/Unload Detection */
    PM_VIDEOAMP_LOAD_DET_RT_ST  = PM_VIDEOAMP_LOAD_DET_IRQ_HDL,
    PM_VIDEOAMP_UNLOAD_DET_RT_ST= PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL,
    PM_ATCDONE_RT_ST            = PM_ATCDONE_IRQ_HDL, 
    PM_ATCFAIL_RT_ST            = PM_ATCFAIL_IRQ_HDL,
    PM_CHGDONE_RT_ST            = PM_CHGDONE_IRQ_HDL,
    PM_CHGFAIL_RT_ST            = PM_CHGFAIL_IRQ_HDL,
    PM_CHGSTATE_RT_ST           = PM_CHGSTATE_IRQ_HDL,
    PM_FASTCHG_RT_ST            = PM_FASTCHG_IRQ_HDL,
    PM_ICHGEND_RT_ST            = PM_ICHGEND_IRQ_HDL,
    PM_BATTCONNECT_RT_ST        = PM_BATTCONNECT_IRQ_HDL,
    PM_BATTTEMP_RT_ST           = PM_BATTTEMP_IRQ_HDL,
    PM_CHGHOT_RT_ST             = PM_CHGHOT_IRQ_HDL,
    PM_CHGTLIMIT_RT_ST          = PM_CHGTLIMIT_IRQ_HDL,
    PM_CHGGONE_RT_ST            = PM_CHGGONE_IRQ_HDL,
    PM_VCPMAJOR_RT_ST           = PM_VCPMAJOR_IRQ_HDL,

    PM_NUM_OF_RT_ST             = PM_NUM_OF_IRQ_HDL

}pm_rt_status_type;

/* Event definition for wake up from power collapse */
#define PM_WAKE_UP_NO_EVENT             0x00
#define PM_WAKE_UP_EVENT_CHG            0x01
#define PM_WAKE_UP_EVENT_USB_CHG        0x02
#define PM_WAKE_UP_EVENT_RTC_ALRM       0x04
#define PM_WAKE_UP_EVENT_CABLE_IN       0x08

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
/*   PMIC interrupt service routine. */
extern  void          pm_isr(void);

/*===========================================================================

FUNCTION pm_set_irq_handle                            EXTERNAL FUNCTION

DESCRIPTION
    Use this function:

    1) Enable the desired PMIC IRQ and assign to it an
       interrupt service routine.

    2) Disable the desired PMIC IRQ by assigning to it a "NULL"
       interrupt service routine.

INPUT PARAMETERS
  1) Parameter name: pm_irq_hdl.
     - Which IRQ.

     Parameter type: pm_irq_hdl_type (enum).
        PM_VALID_CHG_IRQ_HDL          =  Charger valid IRQ.
        PM_INVALID_CHG_IRQ_HDL        =  Charger Invalid IRQ.
        PM_VALID_USB_CHG_IRQ_HDL      =  USB Charger valid IRQ.
        PM_INVALID_USB_CHG_IRQ_HDL    =  USB Charger Invalid IRQ.
        PM_CHG_I_LIMITING_IRQ_HDL     =  Charger in current limiting IRQ.
        PM_CHG_P_LIMITING_IRQ_HDL     =  Charger in Power limiting IRQ.
        PM_PULSE_CHG_DONE_IRQ_HDL     =  Pulse Charger done IRQ.
        PM_VCP_IRQ_HDL                =  VCP(voltage collapse protection) IRQ.    
        PM_T_STAT_CHANGED_IRQ_HDL     =  Temperature Status Changed IRQ.
        PM_BAT_STAT_CHANGED_IRQ_HDL   =  Battery Status Changed IRQ.
        PM_VBAT_DET_IRQ_HDL           =  Upper VBATT max IRQ.
        PM_BAT_FET_ON_IRQ_HDL         =  Battery fet turn on IRQ.
        PM_KPD_PWR_KEY_ON_IRQ_HDL     =  Keypad power key pressed IRQ.
        PM_KPD_PWR_KEY_OFF_IRQ_HDL    =  Keypad power key released IRQ.
        PM_RTC_ALRM_IRQ_HDL           =  Realtime clock alarm IRQ.
        PM_OSC_HALT_IRQ_HDL           =  Crystal oscillator halted IRQ.
        PM_PWR_RST_IRQ_HDL            =  Power reset IRQ.
        PM_SMPL_IRQ_HDL               =  Sudden momentary pwr loss IRQ.
        PM_CABLE_IN_IRQ_HDL           =  Cable insertion IRQ.
        PM_OVER_TEMP_IRQ_HDL          =  Device over-temperature IRQ.
        PM_WDOG_TOUT_IRQ_HDL          =  Watchdog Timeout IRQ.
        PM_KPD_PWRON_EVENT_IRQ_HDL    =  Keypad power on event occurred IRQ.

        PM_USB_BUS_VALID_RE_IRQ_HDL   =  USB VBUS valid (RE) IRQ.
        PM_USB_SN_VALID_RE_IRQ_HDL    =  USB session valid (RE) IRQ.
        PM_USB_D_PLUS_HI_RE_IRQ_HDL   =  USB D+ HI (RE) IRQ.
        PM_USB_ID_GND_RE_IRQ_HDL      =  USB ID grounded (RE) IRQ.
        PM_USB_D_MINUS_HI_RE_IRQ_HDL  =  USB D- HI (RE) IRQ.
        PM_USB_ID_FLOATING_RE_IRQ_HDL =  USB ID floatingt (RE) IRQ.
        PM_USB_BOFF_AON_RE_IRQ_HDL    =  USB B disconnect A connect (RE) IRQ.
        PM_USB_CARKIT_INT_RE_IRQ_HDL  =  USB carkit interrupt (RE) IRQ.

        Note: RE = Rising Edge.

        PM_USB_BUS_VALID_FE_IRQ_HDL   =  USB bus valid (FE) IRQ.
        PM_USB_SN_VALID_FE_IRQ_HDL    =  USB session valid (FE) IRQ.
        PM_USB_D_PLUS_HI_FE_IRQ_HDL   =  USB D+ HI (FE) IRQ.
        PM_USB_ID_GND_FE_IRQ_HDL      =  USB ID grounded (FE) IRQ.
        PM_USB_D_MINUS_HI_FE_IRQ_HDL  =  USB D- HI (FE) IRQ.
        PM_USB_ID_FLOATING_FE_IRQ_HDL =  USB ID floatingt (FE) IRQ.
        PM_USB_BOFF_AON_FE_IRQ_HDL    =  USB B disconnect A connect (FE) IRQ.
        PM_USB_CARKIT_INT_FE_IRQ_HDL  =  USB carkit interrupt (FE) IRQ.

        Note: FE = Falling Edge.


        PM_MPP01_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP02_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP03_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP04_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP05_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP06_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP07_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP08_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP09_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP10_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP11_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP12_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP13_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP14_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP15_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP16_CHGED_ST_IRQ_HDL     =  MPP State Changed


  2) Parameter Name: Handler.
     - The function that will service the IRQ.

     Parameter type: pm_isr_type (function pointer).

RETURN VALUE
  Return value type: pm_err_flag_type

    PM_ERR_FLAG__SUCCESS              = We were successful Enabling/Disabling
                                        the desired PMIC IRQ..
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one "pm_irq_hdl" is
                                        out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC.                                       
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
                                        with the PMIC.

DEPENDENCIES
  The following functions need to be call before calling this function:
  1) pm_init()
  2) pm_init_delayed()

SIDE EFFECTS
  None.

===========================================================================*/
pm_err_flag_type pm_set_irq_handle
(
  pm_irq_hdl_type       irq_id,     /* Which IRQ */
  pm_isr_ptr_type       handler     /* The function that will service
                                       the IRQ */
);
/*=============================================================================
FUNCTION:
  pm_set_irq_multi_handle
  Sets up the PMIC interrupt handler for a given IRQ ID.  This function
  may be called more than once which allows for multiple callback functions
  for the same IRQ ID.

PARAMETERS:
  pm_irq_hdl_type       irq_id,     // Which IRQ 
  pm_isr_ptr_multi_type handler,    // The function that will service
                                       the IRQ 
  void *handler_arg                 // argument to use with handler 
  
RETURNS:
  pm_err_flag_type
==============================================================================*/
pm_err_flag_type pm_set_irq_multi_handle
(
  pm_irq_hdl_type       irq_id,     /* Which IRQ */
  pm_isr_ptr_multi_type handler,    /* The function that will service
                                       the IRQ */
  void *handler_arg                 /* argument to use with handler */
);
/**
 * See description for pm_set_irq_handle.
 * This function clears the handler for a given irq_id.
 * @param irq_id
 * @param handler
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_clear_irq_multi_handle
(
  pm_irq_hdl_type       irq_id,     /* Which IRQ */
  pm_isr_ptr_multi_type handler     /* The function that will service
                                       the IRQ */
);

/*===========================================================================

FUNCTION pm_get_rt_status                            EXTERNAL FUNCTION

DESCRIPTION
    This function returns the current bit-wise (not latch) status of the select
  PMIC IRQ. Use this function for polling the desired PMIC real time status bit
  if the IRQ is not enabled. An interrupt will not be trigger until the
  corresponding IRQ is enable.

INPUT PARAMETERS
  1) Parameter name: rt_status_id.
     - Which IRQ Real Time Status we want to read.

     Parameter type: pm_rt_status_type (enum).
       PM_VALID_CHG_RT_ST       =  Charger valid RT STATUS.
       PM_INVALID_CHG_RT_ST     =  Charger Invalid RT STATUS.
       PM_VALID_USB_CHG_RT_ST   =  USB Charger valid RT STATUS.
       PM_INVALID_USB_CHG_RT_ST =  USB Charger Invalid RT STATUS.
       PM_CHG_I_LIMITING_RT_ST  =  Charger in current limiting RT STATUS.
       PM_CHG_P_LIMITING_RT_ST  =  Charger in Power limiting RT STATUS.
       PM_PULSE_CHG_DONE_RT_ST  =  Pulse Charger done RT STATUS.
       PM_VCP_IRQ_RT_ST         =  (VCP)Voltage Collapse Protection RT STATUS.
       PM_T_STAT_CHANGED_RT_ST  =  Temperature Status Changed RT STATUS.
       PM_BAT_STAT_CHANGED_RT_ST=  Battery Status Changed RT STATUS.
       PM_VBAT_DET_RT_ST        =  Upper VBATT max RT STATUS.
       PM_BAT_FET_ON_RT_ST      =  Battery fet turn on RT STATUS.
       PM_KPD_PWR_KEY_ON_RT_ST  =  Keypad power key pressed RT STATUS.
       PM_KPD_PWR_KEY_OFF_RT_ST =  Keypad power key released RT STATUS.
       PM_RTC_ALRM_RT_ST        =  Realtime clock alarm RT STATUS.
       PM_OSC_HALT_RT_ST        =  Crystal oscillator halted RT STATUS.
       PM_PWR_RST_RT_ST         =  Power reset RT STATUS.
       PM_SMPL_RT_ST            =  Sudden momentary pwr loss RT STATUS.
       PM_CABLE_IN_RT_ST        =  Cable insertion RT STATUS.
       PM_OVER_TEMP_RT_ST       =  Device over-temperature RT STATUS.
       PM_WDOG_TOUT_RT_ST       =  Watchdog Timeout RT STATUS.
       PM_KPD_PWRON_EVENT_RT_ST =  Keypad power on event occurred RT STATUS.
       PM_USB_BUS_VALID_RT_ST   =  USB VBUS valid RT STATUS.
       PM_USB_SN_VALID_RT_ST    =  USB session valid RT STATUS.
       PM_USB_D_PLUS_HI_RT_ST   =  USB D+ HI RT STATUS.
       PM_USB_ID_GND_RT_ST      =  USB ID grounded RT STATUS.
       PM_USB_D_MINUS_HI_RT_ST  =  USB D- HI RT STATUS.
       PM_USB_ID_FLOATING_RT_ST =  USB ID floatingt RT STATUS.
       PM_USB_BOFF_AON_RT_ST    =  USB B disconnect A connect RT STATUS.
       PM_USB_CARKIT_INT_RT_ST  =  USB carkit interrupt RT STATUS.

       PM_MPP01_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP02_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP03_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP04_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP05_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP06_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP07_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP08_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP09_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP10_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP11_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP12_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP13_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP14_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP15_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.
       PM_MPP16_CHGED_RT_ST     =  Real Time State of MPP, when config as 
                                   digital input.

  2) Parameter name: rt_status.
     - Returns the real time status of the selected IRQ.

     Parameter type: boolean pointer.

     TRUE  = HIGH
     FALSE = LOW

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
  Call the following functions before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_get_rt_status(pm_rt_status_type rt_status_id,
                                         boolean           *rt_status);

/*===========================================================================

FUNCTION pm_get_irq_status                            EXTERNAL FUNCTION

DESCRIPTION
    This function returns the status of the different PMIC
  IRQs. An interrupt will not be trigger until the corresponding IRQ
  is enabled. The IRQ status bit will stay active until the corresponding
  IRQ is clear.

INPUT PARAMETERS
  1) Parameter name: pm_irq_hdl.
     - Which IRQ.

     Parameter type: pm_irq_hdl_type (enum).
        PM_VALID_CHG_IRQ_HDL          =  Charger valid IRQ.
        PM_INVALID_CHG_IRQ_HDL        =  Charger Invalid IRQ.
        PM_VALID_USB_CHG_IRQ_HDL      =  USB Charger valid IRQ.
        PM_INVALID_USB_CHG_IRQ_HDL    =  USB Charger Invalid IRQ.
        PM_CHG_I_LIMITING_IRQ_HDL     =  Charger in current limiting IRQ.
        PM_CHG_P_LIMITING_IRQ_HDL     =  Charger in Power limiting IRQ.
        PM_PULSE_CHG_DONE_IRQ_HDL     =  Pulse Charger done IRQ.
        PM_VCP_IRQ_HDL                =  VCP(voltage collapse protection) IRQ.
        PM_T_STAT_CHANGED_IRQ_HDL     =  Temperature Status Changed IRQ.
        PM_BAT_STAT_CHANGED_IRQ_HDL   =  Battery Status Changed IRQ.
        PM_VBAT_DET_IRQ_HDL           =  Upper VBATT max IRQ.
        PM_BAT_FET_ON_IRQ_HDL         =  Battery fet turn on IRQ.
        PM_KPD_PWR_KEY_ON_IRQ_HDL     =  Keypad power key pressed IRQ.
        PM_KPD_PWR_KEY_OFF_IRQ_HDL    =  Keypad power key released IRQ.
        PM_RTC_ALRM_IRQ_HDL           =  Realtime clock alarm IRQ.
        PM_OSC_HALT_IRQ_HDL           =  Crystal oscillator halted IRQ.
        PM_PWR_RST_IRQ_HDL            =  Power reset IRQ.
        PM_SMPL_IRQ_HDL               =  Sudden momentary pwr loss IRQ.
        PM_CABLE_IN_IRQ_HDL           =  Cable insertion IRQ.
        PM_OVER_TEMP_IRQ_HDL          =  Device over-temperature IRQ.
        PM_WDOG_TOUT_IRQ_HDL          =  Watchdog Timeout IRQ.
        PM_KPD_PWRON_EVENT_IRQ_HDL    =  Keypad power on event occurred IRQ.

        PM_USB_BUS_VALID_RE_IRQ_HDL   =  USB VBUS valid (RE) IRQ.
        PM_USB_SN_VALID_RE_IRQ_HDL    =  USB session valid (RE) IRQ.
        PM_USB_D_PLUS_HI_RE_IRQ_HDL   =  USB D+ HI (RE) IRQ.
        PM_USB_ID_GND_RE_IRQ_HDL      =  USB ID grounded (RE) IRQ.
        PM_USB_D_MINUS_HI_RE_IRQ_HDL  =  USB D- HI (RE) IRQ.
        PM_USB_ID_FLOATING_RE_IRQ_HDL =  USB ID floatingt (RE) IRQ.
        PM_USB_BOFF_AON_RE_IRQ_HDL    =  USB B disconnect A connect (RE) IRQ.
        PM_USB_CARKIT_INT_RE_IRQ_HDL  =  USB carkit interrupt (RE) IRQ.

        Note: RE = Rising Edge.

        PM_USB_BUS_VALID_FE_IRQ_HDL   =  USB bus valid (FE) IRQ.
        PM_USB_SN_VALID_FE_IRQ_HDL    =  USB session valid (FE) IRQ.
        PM_USB_D_PLUS_HI_FE_IRQ_HDL   =  USB D+ HI (FE) IRQ.
        PM_USB_ID_GND_FE_IRQ_HDL      =  USB ID grounded (FE) IRQ.
        PM_USB_D_MINUS_HI_FE_IRQ_HDL  =  USB D- HI (FE) IRQ.
        PM_USB_ID_FLOATING_FE_IRQ_HDL =  USB ID floatingt (FE) IRQ.
        PM_USB_BOFF_AON_FE_IRQ_HDL    =  USB B disconnect A connect (FE) IRQ.
        PM_USB_CARKIT_INT_FE_IRQ_HDL  =  USB carkit interrupt (FE) IRQ.

        Note: FE = Falling Edge.


        PM_MPP01_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP02_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP03_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP04_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP05_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP06_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP07_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP08_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP09_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP10_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP11_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP12_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP13_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP14_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP15_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP16_CHGED_ST_IRQ_HDL     =  MPP State Changed


  2) Parameter name: status.
     - Returns the status of the selected IRQ.

     Parameter type: boolean pointer.

     TRUE  = HIGH
     FALSE = LOW

RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC.     
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
                                        with the PMIC.
    PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  Call the following functions before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_get_irq_status(pm_irq_hdl_type irq_id,
                                          boolean         *status);

/*===========================================================================

FUNCTION pm_clear_irq                            EXTERNAL FUNCTION

DESCRIPTION
    This function resets/clears the selected IRQ.

INPUT PARAMETERS
  1) Parameter name: pm_irq_hdl.
     - Which IRQ.

     Parameter type: pm_irq_hdl_type (enum).
        PM_VALID_CHG_IRQ_HDL          =  Charger valid IRQ.
        PM_INVALID_CHG_IRQ_HDL        =  Charger Invalid IRQ.
        PM_VALID_USB_CHG_IRQ_HDL      =  USB Charger valid IRQ.
        PM_INVALID_USB_CHG_IRQ_HDL    =  USB Charger Invalid IRQ.
        PM_CHG_I_LIMITING_IRQ_HDL     =  Charger in current limiting IRQ.
        PM_CHG_P_LIMITING_IRQ_HDL     =  Charger in Power limiting IRQ.
        PM_PULSE_CHG_DONE_IRQ_HDL     =  Pulse Charger done IRQ.
        PM_VCP_IRQ_HDL                =  VCP(voltage collapse protection) IRQ.
        PM_T_STAT_CHANGED_IRQ_HDL     =  Temperature Status Changed IRQ.
        PM_BAT_STAT_CHANGED_IRQ_HDL   =  Battery Status Changed IRQ.
        PM_VBAT_DET_IRQ_HDL           =  Upper VBATT max IRQ.
        PM_BAT_FET_ON_IRQ_HDL         =  Battery fet turn on IRQ.
        PM_KPD_PWR_KEY_ON_IRQ_HDL     =  Keypad power key pressed IRQ.
        PM_KPD_PWR_KEY_OFF_IRQ_HDL    =  Keypad power key released IRQ.
        PM_RTC_ALRM_IRQ_HDL           =  Realtime clock alarm IRQ.
        PM_OSC_HALT_IRQ_HDL           =  Crystal oscillator halted IRQ.
        PM_PWR_RST_IRQ_HDL            =  Power reset IRQ.
        PM_SMPL_IRQ_HDL               =  Sudden momentary pwr loss IRQ.
        PM_CABLE_IN_IRQ_HDL           =  Cable insertion IRQ.
        PM_OVER_TEMP_IRQ_HDL          =  Device over-temperature IRQ.
        PM_WDOG_TOUT_IRQ_HDL          =  Watchdog Timeout IRQ.
        PM_KPD_PWRON_EVENT_IRQ_HDL    =  Keypad power on event occurred IRQ.

        PM_USB_BUS_VALID_RE_IRQ_HDL   =  USB VBUS valid (RE) IRQ.
        PM_USB_SN_VALID_RE_IRQ_HDL    =  USB session valid (RE) IRQ.
        PM_USB_D_PLUS_HI_RE_IRQ_HDL   =  USB D+ HI (RE) IRQ.
        PM_USB_ID_GND_RE_IRQ_HDL      =  USB ID grounded (RE) IRQ.
        PM_USB_D_MINUS_HI_RE_IRQ_HDL  =  USB D- HI (RE) IRQ.
        PM_USB_ID_FLOATING_RE_IRQ_HDL =  USB ID floatingt (RE) IRQ.
        PM_USB_BOFF_AON_RE_IRQ_HDL    =  USB B disconnect A connect (RE) IRQ.
        PM_USB_CARKIT_INT_RE_IRQ_HDL  =  USB carkit interrupt (RE) IRQ.

        Note: RE = Rising Edge.

        PM_USB_BUS_VALID_FE_IRQ_HDL   =  USB bus valid (FE) IRQ.
        PM_USB_SN_VALID_FE_IRQ_HDL    =  USB session valid (FE) IRQ.
        PM_USB_D_PLUS_HI_FE_IRQ_HDL   =  USB D+ HI (FE) IRQ.
        PM_USB_ID_GND_FE_IRQ_HDL      =  USB ID grounded (FE) IRQ.
        PM_USB_D_MINUS_HI_FE_IRQ_HDL  =  USB D- HI (FE) IRQ.
        PM_USB_ID_FLOATING_FE_IRQ_HDL =  USB ID floatingt (FE) IRQ.
        PM_USB_BOFF_AON_FE_IRQ_HDL    =  USB B disconnect A connect (FE) IRQ.
        PM_USB_CARKIT_INT_FE_IRQ_HDL  =  USB carkit interrupt (FE) IRQ.

        Note: FE = Falling Edge.


        PM_MPP01_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP02_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP03_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP04_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP05_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP06_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP07_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP08_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP09_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP10_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP11_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP12_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP13_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP14_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP15_CHGED_ST_IRQ_HDL     =  MPP State Changed
        PM_MPP16_CHGED_ST_IRQ_HDL     =  MPP State Changed

RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
    PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
                                        with the PMIC.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                        version of the PMIC.                                      
    PM_ERR_FLAG__SUCCESS              = SUCCESS.

DEPENDENCIES
  Call the following functions before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_clear_irq(pm_irq_hdl_type irq);


/*===========================================================================

FUNCTION pm_register_isr_logging_func                    EXTERNAL FUNCTION

DESCRIPTION
    This function sets the debug function that will be called every time a
  PMIC is triggered.
  
    This function was written to facilitate debugging.
  
INPUT PARAMETERS
  1) Parameter name: isr_debug_func.
     - The debug function that will be called every time a PMIC IRQ is 
       triggered.

     Parameter type: pm_isr_debug_func_type (function pointer)..

RETURN VALUE
  None.

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) pm_init_delayed()

SIDE EFFECTS
  None.
===========================================================================*/
extern void pm_register_isr_logging_func( \
                                     pm_isr_debug_func_type isr_debug_func);

/*===========================================================================

FUNCTION pm_get_wake_up_status                          EXTERNAL FUNCTION

DESCRIPTION
    This function returns what interrupt events have triggered since the last 
    call. The interrupt status will be cleared after this call. The return 
    status can be used to match particular wake up events in interest.

INPUT PARAMETERS
  1) Parameter name: *pwr_on_status
     - pointer to 32-bit unsigned integer that stores the wake up status
    including RTC alarm trigger, cable, wall charger and USB charger.

RETURN VALUE
  Return Value Type: pm_err_flag_type.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.


DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type pm_get_wake_up_status(uint32* status);


#endif /* PM66XXIRQS */
