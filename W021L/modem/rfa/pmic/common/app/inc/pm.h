
#ifndef PM_H
#define PM_H
/*===========================================================================


                  P M    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs.

Copyright (c) 2003 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pm.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/09   wra     Removed the PM8028, PM8058, and Wookie enumerations.
01/31/09   jtn     Added #include "pm_xoadc.h"
10/21/08   aab     Added #include "pmapp_usb.h"
09/30/08   jtn     Added #include "pmapp_hsed.h"
08/01/08   jtn     Added #include "pmapp_pcil.h"
06/26/08   jtn     Merged from Kip branch
02/27/08   jtn     Added PMIC_IS_KIP to list of PMICs
01/07/08   jtn     Merged from QSC6270 branch
11/16/07   jtn     Added PM_ERR_FLAG__SECURITY_ERR
07/25/07   cng     Added PM6653 support 
07/03/07   jnoblet Added error flag for IRQ list error
02/27/07   cng     Added EPIC and PM6658 support
10/04/06   rmd     Removed VREG definitions that are not longer used within
                   the MSM6500 and MSM6550 RF GSM code.
09/08/06   cng     LINT warning fixes
07/10/06   Vish    Added LINT suppression for ERR_FATAL/ASSERT macro expansions
                   causing "Error (Info) 717: do ... while(0);" errors.
05/31/06   Vish    Added a section for suppressing LINT warnings arising
                   out of symbols defined outside of PMIC library. This will
                   help us get rid of LINT warnings in various PMIC lib files
                   without requiring suppressions in every one of our 
                   affected files. Refer to the following section below:
                        LINT ERROR SUPPRESSIONS
                                 FOR
                     EXTERNAL (to PMIC lib) SYMBOLS
02/10/06   vk      Fixed the order of enum items in pm_model_type 
01/24/06   vk      Modified pm_model_type enum type
11/09/05   cng     Added PM6620 support
11/08/05   cng     More support for Panoramix
10/24/05   cng     Added Panoramix support
09/20/05   fpe     Added MSM6050 support
09/06/05   ans     For MSM6100, PM6650 (ZRF6155 RF card) and PM6640 in software
                   use RFTX VREG as if it was the SYNTH LDO.
08/29/05   fpe     Added pm_model enum for PM6640 support 
07/25/05   cng     Added pm_model enum for PM7500 support 
07/01/05   cng     Added error code for ABUS busy for reservation contention 
06/23/04   rmd     Added VREG defines to support MSM6500 GSM and
                   MSM6100, MSM6025 and MSM6000 RFCMOS.
02/02/04   rmd     Added PMIC_IS_PM6610 in pm_model_type.
01/19/04   rmd     Added support for the PM6610.
01/12/04   rmd     Added definition for PM_GPS_LDO for the MSM6250.
10/14/03   rmd     Added VREG defines to support MSM6100 sleep code.
10/16/03   rmd     Added support for the pm_soft_reset().
10/14/03   rmd     Added VREG defines to support MSM6250 sleep code.
09/24/03   Vish    Added PM_ERR_FLAG__RTC_HALTED to RTC errors.
09/22/03   Vish    Added support for RTC application.
09/12/03   Vish    Added support for RTC.
09/10/03   Vish    Added support for USB-OTG application.
09/04/03   Vish    Added support for the USB-OTG module.
09/04/03   rmd     Added initial support for the charger.
08/28/03   rmd     Added support for the MPP library.
                   Added 2 more error IDs.
08/25/03   rmd     Defined PM_ALL_LDOS so that we can be backwards
                   compatible with the MSM6200 RF driver.
07/21/03   Vish    Added support for ADC (Analog MUX and scaling circuit)
07/21/03   rmd     Added support for PMIC IRQs.
05/29/03   rmd     Created.
===========================================================================*/

#include "comdef.h"


/*===========================================================================
                        LINT ERROR SUPPRESSIONS
                                 FOR
                     EXTERNAL (to PMIC lib) SYMBOLS
===========================================================================*/
//lint -emacro((578), INTLOCK, INTFREE) suppress "variable 'sav' on line#xx hides 'sav' on line#yy"
//lint -emacro({717}, ASSERT, ERR_FATAL) suppress "do ... while(0)"
//lint -emacro({713}, clk_busy_wait) suppress loss of precision
//lint -emacro({834}, clk_busy_wait) suppress confusing operators without parentheses


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef word pm_err_flag_type;

/* Specifies which PMIC we are using */
typedef enum
{
   PMIC_IS_PM6610,
   PMIC_IS_PM6620,
   PMIC_IS_PM6640,
   PMIC_IS_PM6650,
   PMIC_IS_PM7500,
   PMIC_IS_PANORAMIX,
   PMIC_IS_PM6652,
   PMIC_IS_PM6653,
   PMIC_IS_PM6658,
   PMIC_IS_EPIC,
   PMIC_IS_HAN,
   PMIC_IS_KIP,
   //? This is needed for the chips after KIP
   PMIC_IS_INVALID,
}pm_model_type;

/* Use these commands to enable or disable the different PMIC devices */
typedef enum
{
   PM_OFF_CMD,
   PM_ON_CMD,
   PM_INVALID_CMD
}pm_switch_cmd_type;

/*===========================================================================

                        ERROR FLAGS DEFINITIONS

===========================================================================*/

/*========================= Generic Flags =================================*/

/* There were not errors detected while the function was executed */
#define PM_ERR_FLAG__SUCCESS                0x0000
/* Input Parameter one is out of range */
#define PM_ERR_FLAG__PAR1_OUT_OF_RANGE      0x0001
/* Input Parameter two is out of range */
#define PM_ERR_FLAG__PAR2_OUT_OF_RANGE      0x0002
/* Input Parameter three is out of range */
#define PM_ERR_FLAG__PAR3_OUT_OF_RANGE      0x0004
/* Input Parameter four is out of range */
#define PM_ERR_FLAG__PAR4_OUT_OF_RANGE      0x0008
/* Input Parameter five is out of range */
#define PM_ERR_FLAG__PAR5_OUT_OF_RANGE      0x0010
/* The SBI operation Failed  */
#define PM_ERR_FLAG__SBI_OPT_ERR            0x0080
/* This feature is not supported by this PMIC */
/* Example: Speaker driver is not available on the PM6000 */
#define PM_ERR_FLAG__FEATURE_NOT_SUPPORTED  0x0100

/* The PMIC model (PM6000, PM6050, etc) was not init properly.
   Make sure you call pm_init_comm() before using any of the PMIC API!! */
#define PM_ERR_FLAG__INVALID_PMIC_MODEL     0x0200
#define PM_ERR_FLAG__SECURITY_ERR           0x0800
/*===================== End of Generic Error Flags ========================*/
/*==================== PM IRQ List error flags ==============================*/
#define PM_ERR_FLAG__IRQ_LIST_ERR           0x0400
/*==================== end PM IRQ List error flags ==========================*/


/*==================== PM SBI ID error flags ==============================*/
/* The SBI ID read from the PMIC does not match with the selected SBI ID.*/
#define PM_ERR_FLAG__DEV_MISMATCH           0x8000
/*===================== End of PM SBI error flags =========================*/


/*===================== PM HKADC error flags ==============================*/
/* The selected resolution is invalid */
#define PM_ERR_FLAG__ADC_INVALID_RES        0x8000
/* AD conversion not ready */
#define PM_ERR_FLAG__ADC_NOT_READY          0x4000
/* The selected signal is not connected to the PM chip */
#define PM_ERR_FLAG__ADC_SIG_NOT_SUPPORTED  0x2000
/*=================== End of PM HKADC error flags =========================*/

/*===================== PM RTC error flags ==============================*/
/* The RTC displayed mode read from the PMIC is invalid */
#define PM_ERR_FLAG__RTC_BAD_DIS_MODE       0x8000

/* Failed to read the time from the PMIC RTC*/
#define PM_ERR_FLAG__RTC_READ_FAILED        0x4000

/* Failed to write the time to the PMIC RTC*/
#define PM_ERR_FLAG__RTC_WRITE_FAILED       0x2000

/* RTC not running */
#define PM_ERR_FLAG__RTC_HALTED             0x1000

/*===================== PM MPP error flags ==============================*/
/* The DBUS is already in use by another MPP. */
#define PM_ERR_FLAG__DBUS_IS_BUSY_MODE      0x8000
/* The ABUS is already in use by another MPP. */
#define PM_ERR_FLAG__ABUS_IS_BUSY_MODE      0x4000

/*=================== End of PM RTC error flags =========================*/

/*=================== PM USB-OTG error flags ============================*/

/*================= End of PM USB-OTG error flags =======================*/


/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm66XXvregs.h"        /* VREGs API           */
#include "pm66XXirqs.h"         /* IRQ   API           */
#include "pm66XXadc.h"          /* ADC MUX API         */
#include "pm66XXmpps.h"         /* MPPS API            */
#include "pm66XXchg.h"          /* CHGer API           */
#include "pm66XXotg.h"          /* USB OTG API         */
#include "pm66XXrtc.h"          /* RTC API             */
#include "pmgen.h"              /* Generic API         */
#include "pmapp.h"              /* Generic apps API    */
#include "pmapp_otg.h"          /* USB-OTG apps API    */
#include "pmapp_rtc.h"          /* RTC apps API        */
#include "pmsbl.h"              /* Mini APIs for boot code or SBL */
#include "pmapp_pcil.h"         /* support for PMIC APIs via proc comm */
#include "pmapp_hsed.h"         /* support for one-touch headset controller */
#include "pmapp_usb.h"			/* support USB related functionality*/
#include "pm_xoadc.h"           /* support for PMIC XOADC */
/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION pm_init                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM60X0 for operation.

    It does the following:
    1)  It initializes the PMIC software driver memory
    2)  It initializes the LDOs voltages.
    3)  It enables and starts the 32Khz external sleep crystal

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  rflib_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while writing and reading to/from sbi.

===========================================================================*/
extern void pm_init(void);

/*===========================================================================

FUNCTION pm_init_delayed                            EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM60X0 ISR services. It disables all PMIC
  IRQs and it registers the PMIC ISR with the GPIO software driver.

INPUT PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  pm_init(), tramp_init() and gpio_int_init() must have been called.

SIDE EFFECTS
  None

===========================================================================*/
extern void pm_init_delayed(void);


/*===========================================================================

                        PUBLIC CONSTANT DEFINITIONS

===========================================================================*/

/* The following definitions are used to make the PMIC3 code backward 
   compatible with old PMIC2 RF drivers. It is recommended that the use 
   of this definitions becomes discontinue. */
   
#if defined(FEATURE_PMIC_MANAGED_LDO)

  #if defined(T_PMIC_MSM6250_CONFIG)
#error code not present
  #elif defined(T_QSC60X0)     

    #if defined(T_PMIC_PANORAMIX)
#error code not present
    #endif

  #elif (defined(T_MSM6100) && !defined(T_MSM6500))
#error code not present
  #elif (defined(T_MSM6025) || defined(T_MSM6000) || defined(T_MSM6050)) 
  
    #if defined(T_PMIC_PM6650)

      #if defined(T_RF_ZRF6155)

        /* Software uses the RFTX VREG as if it was the SYNTH LDO */
        #define PM_RFTX_LDO   0x0
        #define PM_SYNT_LDO   PM_VREG_RFTX_M
        #define PM_RFRX_LDO   PM_VREG_RFRX1_M
        
      #else
      
        #define PM_RFTX_LDO   PM_VREG_RFTX_M
        #define PM_SYNT_LDO   PM_VREG_SYNT_M
        #define PM_RFRX_LDO   PM_VREG_RFRX1_M

      #endif
      
    #elif defined(T_PMIC_PM6640)
#error code not present
    #elif defined(T_PMIC_PM6620)
#error code not present
    #elif defined(T_PMIC_PM6610)
#error code not present
    #endif

  #endif
  
#endif /* FEATURE_PMIC_MANAGED_LDO */

#endif /* PM_H */
