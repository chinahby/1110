/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 P M A P P S

GENERAL DESCRIPTION
  This file contains functions and corresponding variable 
  declarations to support applications for the Qualcomm Power Manager Chips
  Set.

Copyright (c) 2003,2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007,2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp.c#16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/09   aab     Added a check in pm_pwr_key_pressed_isr() and pm_pwr_key_released_isr()
                   so that for dual core targets, PMIC's Pwr/End key press/release ISRs 
                   does not get processed before Apps RPC completed its initialization.
                   This prevents proxy buffer overflow during RPC initialization.
11/21/09   jtn     Added FEATURE_PMIC_MPP4_USED_AS_AMUX_INPUT
11/06/08   jtn     Integrate 8200 target changes for MPP22 in pm_adc_init()
10/20/08   jtn     Add check for FEATURE_SLEEP_DISABLE_HS_OKTS_SIG in pm_pwr_key_pressed_isr()
10/04/08   jtn     Fix compiler warnings when FEATURE_KEYPAD is not defined
09/30/08   jtn     Moved headset send-end detect functions and vars to new
                   pmapp_hsed.c file
08/01/08   jtn     Move all proc comm functions to pm_pcil.c
07/29/08   jtn     Merge changes from Gobi2000/UNDP
07/11/08   jtn     Moved pm_vote_vreg_switch_pc() from library to common to
                   avoid link error in boot build.
07/11/08   jtn     Undid removal of pm_vreg_pa_dvs_work_around()--required for 
                   DZRF6500RF card
07/06/08   jtn     Added comments, API pm_vote_vreg_request_vote() for 
                   requesting VREG vote at run time.
06/27/08   jtn     Removed power key polling for QSC6270
(begin QSC1100 changes)
06/20/08   jtn     Added pm_pwr_key_diag_counter
06/20/08   jtn     Remove keypad polling for QSC1100, added 
                   FEATURE_PMIC_PWR_KEY_POLLING
(end QSC1100 changes)
06/13/08   jtn     Removed function pm_variable_pa_vdd_init()
05/07/08   vk      Added video load/unload detection.
03/01/08   jtn     Add #ifndef BUILD_BOOT_CHAIN with FEATURE_PMIC_HS_SEND_END_DETECT
                   to fix problem building boot image.
02/08/08   jtn     Wrap call to keypad_enable_polling() with #ifndef 
                   FEATURE_MULTIPROCESSOR and #if 
                   defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes)
11/14/07   jtn     Fixed compiler warnings.
11/07/07   jtn     Workaround for Han power key interrupt changes.
10/17/07   jtn     Changed parameters in call to pm_vreg_vote_switch()
(end QSC6270 changes)
10/17/07   cng     Raised current detection threshold for HSED key to 600uA
09/17/07   jhs     Fix issue where PWR and END key events could overflow
                   the buffer because the other processor was not fully
                   booted yet, only an issue when FEATURE_MULTIPROCESSOR 
                   is defined.
                   Fix issue where a PWR key release event was being sent
                   even when a PWR key pressed event had not been sent.
09/14/07   cng     Added UNDP specific action in pm_pwr_key_debounce_cb
06/28/07   cng     Added pm_register_vreg_switch_callback and 
                   pm_deregister_vreg_switch_callback APIs to allow callback
                   upon VREG state change 
                   Updated PMIC temperature alarm thresholds in comment 
                   section of pm_temp_state_changed_isr 
06/01/07   cng     Call keypad_enable_polling from pm_hsed_pressed_isr to let
                   keypad driver know the headset button status 
05/18/07   hs      Minor changes in pm_variable_pa_vdd_init()
03/12/07   cng     Featurize KEYPAD_PASS_KEY_CODE and keypad_set_pwr_key_state
                   with FEATURE_KEYPAD 
03/01/07   cng     Added API pm_hsed_get_key_pressed and
                   pm_hsed_clear_key_pressed to return latchd status of HSED
                   key
                   Changed default hystersis threshold for HSED button prress
                   Made sure clear IRQ first before unmasking for pressed and
                   released ISRs     
01/30/07   jhs     Made internal helper functions static.
01/02/07   jhs     Switched multiprocessor PWR/END key support to use an
                   internal buffer to remove dependency that ONCRPC PROXY
                   tasks be started.
01/03/07   hs      Replaced T_MSM7500 and FEATURE_ONCRPC with FEATURE_MULTIPROCESSOR
10/07/06   hs      Made changes in pm_perform_vbatdet_calibration()
07/24/06   giri    Core services build support. Featurized the RPC calls, for 
                   core build there is is no RPC support.
06/22/06   hs      Minor changes in pm_variable_pa_vdd_init()
06/14/06   cng     Added init routine and IRQ handler for headset send/end
                   detect feature 
06/06/06   vk      Suppressed lint warnings
05/12/06   VK      configured featurebased MPP2, MPP17 & MPP18 as Analog Input
12/21/05   Vish    Added two APIs - pm_get_latched_pwr_key_pressed_status() and
                   pm_clear_latched_pwr_key_pressed_status() for accessing the
                   latched status of "power key pressed" condition.
10/31/05   VK      Minor changes in vbatdet calibration routine
10/04/05   VK      added vbat calibration routine 
07/25/05   jhs     Support for 7500 PWR key.
07/01/05   cng     Added check for invalid vreg_id in pm_vote_vreg_switch()
03/17/05   Vish    Added FEATURE_PMIC_MPP2_USED_AS_AMUX_INPUT to selectively
                   configure MPP2 as a PM analog MUX input.                   
02/01/05   rmd     Added support for the power key pin pull up.
12/28/04   rmd     Disable pm_vreg_pa_dvs_config() by default. The RF driver 
                   is over-writing PA_R0 to ensure that the external FET is 
                   always open every time that the VREG_PA regulator is 
                   enabled.
12/16/04   rmd     Removed VREG PA initialization. Instead, updated the code 
                   within pm_vreg_control() to initialize the VREG PA for PA DVS if 
                   PA DVS is enabled and we are turning the VREG PA on. All other 
                   VREGs will unaffected by this change.
11/01/04   rmd     In pm_variable_pa_vdd_init() lower the wait time to 
                   200 usec.
10/26/04   rmd     Featurized correctly if FEATURE_SLEEP is not defined
10/26/04   rmd     Updated the PA DVS init code to make that VREG PA is 
                   initialized properly before we enable PA DVS.
08/30/04   rmd     * Removed obsolete UIM init function.
                   * For PA DVS, configured the PA BUCK VREG compensation
                     settings.
08/19/04   Vish    Fixed the power-on key hysteresis delay passed to 
                   pm_power_key_setup() in pm_pwr_key_init().
06/18/04   Vish    Wrapped inclusion of "keypad.h" inside of 
                   FEATURE_KEYPAD_OWNS_KEYPRESS_ISR to avoid compilation
                   errors on targets that don't have that file.
05/10/04   ajn     Inform the keypad of the pwr key press event, and let it
                   vote not ok to sleep and start its keypad polling again.
05/16/04   rmd     Added pm_app_clk_busy_wait().
04/26/04   rmd     Minor comment update.
04/20/04   rmd     Added delay before we clear and re-enable the PMIC's over 
                   temperature stage changed IRQ.
04/14/04   rmd     If we reach PMIC over temperature Stage 3. Added code to 
                   guarantee that the handset will shut down.
03/05/04   rmd     Added code that allows the end key to be detected 
                   while we are sleeping.
02/03/04   rmd     Added pm_vote_vreg_switch() which allows multiple 
                   applications to enable/disable the same VREG.
01/13/04   rmd     Configured MPP3 as a digital input if DVS is not defined.
11/14/03   rmd     Moved the temporary charger code into the charger VU.
11/12/03   Vish    Added sleep xtal osc management.
11/03/03   rmd     Added support for PMIC's internal over temperature
                   protection.
10/16/03   rmd     Configured PA_FET_N to be always low if PA DVS feature
                   is disable. Updated comments while i was at it.
10/13/03   rmd     - Configured the UIM code so that the PMIC driver
                     enables the RUIM VREG if the UIM application
                     does not do so.
                   - FEATURIZED the definition for PM_VREG_PA_VOLTAGE_SETTING
                     so that it only gets compiled if we are changing the
                     PA voltage depending on how much power we are TXing with. 
10/09/03   rmd     - Updated the Settings for UIM for the data line.
                   - If FEATURE_PMIC_MANAGED_LDO is not defined, i set the
                     voltage level and enable VREG PA. This is done for
                     DVS (PA dynamic voltage setting).
09/04/03   rmd     Added initial support for the charger.
                   Added support for variable PA voltage.
09/02/03   rmd     Added initialization for PM66XX AMUX.
08/28/03   rmd     Added init function for UIM.
07/29/03   rmd     Updated the power key code that it works even if 
                   IRQS are LOCKED while calling "pm_is_pwer_key_pressed(). 
07/18/03   rmd     Added support for the PWR key ISR.
06/22/03   rmd     Initial Revision.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"                   

#if defined (FEATURE_PMIC)

#include "comdef.h"
#include "pm.h"
#include "clk.h"
#include "err.h"
#include "adc.h"
#include "rex.h"
#include "sleep.h"
#include "hw.h"
#if defined(T_UNDP) && !defined(T_GOBI2000)
#include "cm.h"
#endif


#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
#include "keypad.h"
#endif //FEATURE_KEYPAD_OWNS_KEYPRESS_ISR

#if defined(FEATURE_PMIC_32K_XTAL)
   #if defined(FEATURE_SLEEP_CLK_STABILITY)
#include "sclk.h"
   #endif //FEATURE_SLEEP_CLK_STABILITY
#endif //FEATURE_PMIC_32K_XTAL

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

/*===========================================================================

                CONSTANT DEFINITIONS

===========================================================================*/
#define PM_VOTE_MAX_DYNAMIC_VOTES 16
#define PM_VOTE_REQUEST_START (PM_VOTE_MAX_SHIFT -1)

#define PM_PWR_KEY_CNT_BEFORE_PULL  200

#if !defined(FEATURE_PMIC_MANAGED_LDO)
/* PA VREG VOLTAGE SETTING */
#define PM_VREG_PA_VOLTAGE_SETTING    1750
#endif

#if defined(FEATURE_PMIC_INTERNAL_IC_TEMPERATURE)
#define PM_OVER_TEMP_CB_TIME          15000
#define PM_TEMP_STATE_CHANGED_TIME     1000
#endif

#if defined(FEATURE_PMIC_32K_XTAL)
#define PM_SLEEP_XTAL_WARMUP_TIME_MS   5000
#endif //FEATURE_PMIC_32K_XTAL
                       
#if defined(FEATURE_PMIC_VARIABLE_PA_VDD)
/* Error Amp Transconductance setting.     */
/* 1/((4 * 7) + 2000 Ohms) = 1/(2028 Ohms) */
#define PA_DVS_GM_CFG         7

/* Compensation Resistor setting. */
/* (10 + (20 * 3)) = 70 kOhms.    */
#define PA_DVS_RESISTOR_CFG   3

/* Sawtooth current setting.    */
/* 4.5uA - (6 * 0.5uA) = 1.5 uA */
#define PA_DVS_SAWTOOTH_CFG   6  
#endif /* FEATURE_PMIC_VARIABLE_PA_VDD */

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
static boolean     pm_pwr_key_status = FALSE;
static boolean     pm_pwr_key_pressed_latched_status = FALSE;


int pm_pwr_key_diag_counter = 0;

#if defined(FEATURE_PMIC_INTERNAL_IC_TEMPERATURE)
static clk_cb_type pm_over_temp_call_back;
static clk_cb_type pm_temp_state_changed_call_back;
#endif

#if defined(FEATURE_PMIC_32K_XTAL)
// timer client used for waiting for xtal osc to warmup
static clk_cb_type  pm_sleep_xtal_osc_timer_client;
#endif //FEATURE_PMIC_32K_XTAL

/* This table allows multiple application to share the same VREG. Each item 
   represents a VREG, each bit within an item represents an application. If
   all bits within an item are equal to 0 then no application requires the VREG
   to stay on so it is OK to turn off the VREG. */
static uint32 pm_vote_vreg_table[PM_VREG_ID_INVALID]={0};

/* function pointer type associated with a VREG */
typedef void (*pm_vreg_cb_type)(pm_vreg_id_type, pm_switch_cmd_type);

/* struct for callback function table to keep track of all registered callbacks */
typedef struct
{
    pm_vreg_cb_type callback_func; /* registered callback */
    pm_vreg_cb_when_type when_to_call; /* to call before or after state change */
} pm_vreg_cb_table_type;

/* Queue size of callback - maximum number of callback functions that can be
 * registered per VREG
 */
#define PM_VREG_CB_QUEUE_SIZE   3

/* Callback function table that keep track of all callback functions of all VREGs */
static pm_vreg_cb_table_type pm_vote_vreg_cb[PM_VREG_ID_INVALID][PM_VREG_CB_QUEUE_SIZE];

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
static void    pm_pwr_key_pressed_isr(void *arg);

static void    pm_pwr_key_released_isr(void *arg);


#if defined(FEATURE_PMIC_INTERNAL_IC_TEMPERATURE)

static void    pm_over_temp_cb( int4 dummy );
static void    pm_over_temp_isr(void);
static void    pm_temp_state_changed_isr(void);
static void    pm_temp_state_changed_cb( int4 dummy );

#endif

#if defined(FEATURE_PMIC_32K_XTAL)
static void pm_sleep_xtal_warmup_timer_expiry_cb(int32 dummy);
static void pm_sleep_xtal_osc_failure_hdlr(void);
#endif //FEATURE_PMIC_32K_XTAL

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

#if defined(FEATURE_PMIC_VIDEO_AMPLIFIER)
static void pm_vid_load_detect_isr(void);
static void pm_vid_unload_detect_isr(void);
#endif /* FEATURE_PMIC_VIDEO_AMPLIFIER */


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

                POWER KEY APP FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION pm_pwr_key_init                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the power key ISR.
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
#if defined (FEATURE_PMIC_PWR_KEY_POLLING)
static clk_cb_type pm_pwron_key_clk_call_back;
static void pm_pwr_key_han_timer(int4 dummy);
#endif
/**
 * This ISR happens when the key is pressed
 * When it happens,
 * 1. disable the IRQ
 * 2. Enable a timer callback to poll for key release
 *
 */
#if defined (FEATURE_PMIC_PWR_KEY_POLLING)
void pm_pwr_key_pressed_han_isr(void)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    /* clear the IRQ */
    errFlag = pm_clear_irq(PM_KPD_PWRON_EVENT_IRQ_HDL);
    /* disable the IRQ handler */
    errFlag |= pm_clear_irq_multi_handle(PM_KPD_PWRON_EVENT_IRQ_HDL, pm_pwr_key_pressed_han_isr);
    pm_pwr_key_status = TRUE;
    /* Setup up a periodic timer to poll the power key  */
    clk_reg( &pm_pwron_key_clk_call_back, pm_pwr_key_han_timer, 
             50, 50, TRUE ); 
    #ifndef FEATURE_MULTIPROCESSOR
    #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
    /* Make sure the keypad driver knows there has been a keypress */
    keypad_enable_polling();
    #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
    #endif /* FEATURE_MULTIPROCESSOR */

}
#endif
/**
 * 
 * When the power key is pressed, call keypad_enable_polling.
 * The keypad driver will then poll
 * the power key status using pm_is_power_key_pressed() API.
 * Keypad driver polling interval is 20 ms.
 * 
 * @param dummy
 * 
 * @return clk_cb_type
 */
#if defined (FEATURE_PMIC_PWR_KEY_POLLING)
static void pm_pwr_key_han_timer(int4 dummy)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    boolean status;
    errFlag = pm_get_rt_status(PM_KPD_PWRON_EVENT_RT_ST, &status);
    if (status == TRUE)  //key is pressed
    {
        errFlag |= pm_clear_irq(PM_KPD_PWRON_EVENT_IRQ_HDL);
        pm_pwr_key_status = TRUE;
    }
    else
    {
        /* turn off polling timer */
        clk_dereg( &pm_pwron_key_clk_call_back );
        /* Enable the power key ISR */
        errFlag |= pm_set_irq_handle(PM_KPD_PWRON_EVENT_IRQ_HDL, 
                                     pm_pwr_key_pressed_han_isr);
        pm_pwr_key_status = FALSE;
    }
}
#endif

void pm_pwr_key_init(void)
{   
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

#if defined (FEATURE_PMIC_PWR_KEY_POLLING)

    clk_def( &pm_pwron_key_clk_call_back );
    
    /* Clear the power key pressed IRQ. */
    errFlag |= pm_clear_irq(PM_KPD_PWRON_EVENT_IRQ_HDL);
    /* Enable the power key ISR */
    errFlag |= pm_set_irq_handle(PM_KPD_PWRON_EVENT_IRQ_HDL, 
                                 pm_pwr_key_pressed_han_isr);

#else
    /* Setup the PMIC power key hysteresis and enable the pull up. */
    errFlag  = pm_power_key_setup(PM_PWR_KEY_DELAY_EQ_0p97_msecs,PM_ON_CMD);
    /* Clear the power key pressed IRQ. */
    errFlag |= pm_clear_irq(PM_KPD_PWR_KEY_ON_IRQ_HDL);
    /* Enable the power key ISR */
    errFlag |= pm_set_irq_multi_handle(PM_KPD_PWR_KEY_ON_IRQ_HDL, 
                                       pm_pwr_key_pressed_isr, NULL);
#endif

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM) && !defined(FEATURE_PMIC_PWR_KEY_POLLING)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

    /* Find out if there was an PMIC API error */
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_ERROR("PMIC API ERROR(0x%x) DETECTED in pm_pwr_key_init",errFlag,0,0);
    }
}

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

/*===========================================================================

FUNCTION pm_is_power_key_pressed                    EXTERNAL FUNCTION

DESCRIPTION
    Use this function to find out if the power key is pressed 
       
PARAMETERS
  None.  

RETURN VALUE
  Return type: Boolean.
    
        TRUE  = Power Key is pressed.
        FALSE = Power Key is not pressed.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean pm_is_power_key_pressed(void)
{   
    uint32  intsave;
    static   uint16  counter = 0;
    boolean status  = FALSE;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
#if defined (FEATURE_PMIC_PWR_KEY_POLLING)
    int4 dummy = 0;
#endif
    INTLOCK_SAV(intsave); //lint !e774 (Boolean within 'if' always evaluates to True)
             //Reason : Check done within the macro and not here */
    
    /* To power down the handset, "hw_power_off()" function waits in a 
       while loop for the power key to be released. The problem is that 
       "hw_power_off()" has interrupts LOCKed, this means that the
       power key released IRQ will never be detected. To avoid this 
       situation, check if interrupts are locked, if they are and the power 
       key was pressed then poll the status of the power key instead 
       of waiting for the interrupt. */

    /* Check if interrupts were locked before we locked them.*/
    if (intsave) //lint !e774 (Boolean within 'if' always evaluates to True)
                 //Reason : above                                            
    {
        /* Interrupts were locked, checked the previous status of the power 
           key */
        if (pm_pwr_key_status == TRUE) 
        {
            
            counter++;

            /* Do not pull all the time. If we are not powering off
               then the Power key Release IRQ will have a chance to
               trigger. */
            if (counter > PM_PWR_KEY_CNT_BEFORE_PULL) 
            {
                counter = 0;
                /* If the power key was pressed then poll to see if it got
                   released */
            #if defined (FEATURE_PMIC_PWR_KEY_POLLING)
                pm_pwr_key_han_timer(dummy);
                /* next two lines are to resolve compiler warnings for FEATURE_PMIC_PWR_KEY_POLLING */
                errFlag = PM_ERR_FLAG__SUCCESS;
                status = TRUE;
            #else
                errFlag = pm_get_rt_status(PM_KPD_PWR_KEY_ON_RT_ST,&status);

                if ((errFlag == PM_ERR_FLAG__SUCCESS) && (status == FALSE))
                {
                   /* Ok, the power key got released, call the power key
                         released ISR yourself. */
                   pm_pwr_key_released_isr(NULL);
                }
                else
                {
                   MSG_ERROR("PMIC API ERROR(0x%x) in pm_is_power_key_pressed",errFlag,0,0);
                }
             #endif
            }
        }
        else
        {
            counter = 0;
        }
    }

    INTFREE_SAV(intsave); 

    return pm_pwr_key_status;
}


/*===========================================================================
FUNCTION   pm_get_latched_pwr_key_pressed_status           EXTERNAL FUNCTION

DESCRIPTION
  Use this function to query the current latched status of "power key
  pressed" condition. Since this a latched status, it means that it will
  indicate if the power key was pressed any time prior to this fn call and
  so the latched status may not be the same as the real-time status returned 
  by pm_is_power_key_pressed(). After the power key pressed condition has been
  detected, this function will continue to indicate the latched condition
  until the latched condition is explicitly cleared using 
  pm_clear_latched_pwr_key_pressed_status().
       
PARAMETERS
  None.  

RETURN VALUE
  Return type: Boolean
    TRUE  --> Power Key pressed condition was detected            
    FALSE --> Power Key pressed condition has not been detected yet (or since
              the last pm_clear_latched_pwr_key_pressed_status() invocation)

DEPENDENCIES
  The following functions must have been invoked before:
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  
SIDE EFFECTS
  Interrupts are locked in this fn.  
===========================================================================*/
boolean pm_get_latched_pwr_key_pressed_status(void)
{
   boolean curr_status = FALSE;

   INTLOCK();
   curr_status = pm_pwr_key_pressed_latched_status;
   INTFREE();

   return curr_status;
}


/*===========================================================================
FUNCTION   pm_clear_latched_pwr_key_pressed_status         EXTERNAL FUNCTION

DESCRIPTION
  Use this function to clear the latched status of power key pressed
  condition. This will result in subsequent pm_get_latched_pwr_key_pressed_status()
  calls to return FALSE until the power key is pressed again.
       
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
  The following functions must have been invoked before:
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  
SIDE EFFECTS
  Interrupts are locked in this fn.
===========================================================================*/
void pm_clear_latched_pwr_key_pressed_status(void)
{
   INTLOCK();
   pm_pwr_key_pressed_latched_status = FALSE;
   INTFREE();
}


/*===========================================================================

FUNCTION pm_pwr_key_pressed_isr                        INTERNAL FUNCTION

DESCRIPTION
  This is the power key pressed ISR.
    
PARAMETERS
  void *arg, added for compatibility with ISR manager  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_pwr_key_pressed_isr(void *arg)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

    #if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
    #endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

    /* Disable the power key pressed ISR */
    pm_pwr_key_diag_counter++;
    errFlag  = pm_clear_irq_multi_handle(PM_KPD_PWR_KEY_ON_IRQ_HDL, pm_pwr_key_pressed_isr);
    
    /* Inform the user that the pwr key has been pressed */
    pm_pwr_key_status = TRUE;

    // remember the latched status
    INTLOCK();
    pm_pwr_key_pressed_latched_status = TRUE;
    INTFREE();

    /* Clear the power key released IRQ */
    errFlag |= pm_clear_irq(PM_KPD_PWR_KEY_OFF_IRQ_HDL);
    /* re-enable the power key released ISR */
    errFlag |= pm_set_irq_multi_handle(PM_KPD_PWR_KEY_OFF_IRQ_HDL, 
                                        pm_pwr_key_released_isr, NULL);
    
    #if !defined (FEATURE_SLEEP_DISABLE_HS_OKTS_SIG)
      #if !defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
#error code not present
      #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
    #endif /* FEATURE_SLEEP_DISABLE_HS_OKTS_SIG */

    #ifdef FEATURE_REX_PREEMPT_INTS
    clk_unexpected_enable();
    /* Re-enable the 5msec tick */
    #endif /* FEATURE_REX_PREEMPT_INTS */
    
    #ifndef FEATURE_MULTIPROCESSOR
    #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
    /* Make sure the keypad driver knows there has been a keypress */
    keypad_enable_polling();
    #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
    #endif /* FEATURE_MULTIPROCESSOR */
    
    /* Find out if there was an PMIC API error */
    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR DETECTED",0,0,0);
    }
}

/*===========================================================================

FUNCTION pm_pwr_key_released_isr                        INTERNAL FUNCTION

DESCRIPTION
  This is the power key released ISR.
    
PARAMETERS
  void *arg, added for compatibility with ISR manager  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_pwr_key_released_isr(void *arg)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

    #if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
    #endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */
    
    /* Disable the power key released ISR */
    errFlag  = pm_clear_irq_multi_handle(PM_KPD_PWR_KEY_OFF_IRQ_HDL, pm_pwr_key_released_isr);
    /* Inform the user that the pwr key has been released */
    pm_pwr_key_status = FALSE;

    /* Clear the power key pressed IRQ */
    errFlag |= pm_clear_irq(PM_KPD_PWR_KEY_ON_IRQ_HDL);
    /* re-enable the power key pressed ISR */
    errFlag |= pm_set_irq_multi_handle(PM_KPD_PWR_KEY_ON_IRQ_HDL,
                                 pm_pwr_key_pressed_isr, NULL);

    /* Find out if there was an PMIC API error */
    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",errFlag,0,0);
    }
}

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */

/*===========================================================================

FUNCTION pm_adc_init                                     EXTERNAL FUNCTION

DESCRIPTION
  Initializes the PM66XX ADC. 
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
void pm_adc_init(void)
{
    pm_err_flag_type    err   = PM_ERR_FLAG__SUCCESS;

    /* Configure MPP1 and MPP2 to be analog inputs. */

    /*     MPP1: Analog Input   =>      AMUX5  */
    err  = pm_mpp_config_analog_input(PM_MPP_1,PM_MPP__AIN__CH_AMUX5);

    #ifdef FEATURE_PMIC_MPP2_USED_AS_AMUX_INPUT
    /*     MPP2: Analog Input   =>      AMUX6  */
    err |= pm_mpp_config_analog_input(PM_MPP_2,PM_MPP__AIN__CH_AMUX6);
    #endif

    #ifdef FEATURE_PMIC_MPP4_USED_AS_AMUX_INPUT
    /*     MPP4: Analog Input   =>      AMUX7  */
    err |= pm_mpp_config_analog_input(PM_MPP_4,PM_MPP__AIN__CH_AMUX7);
    #endif        


    #ifdef FEATURE_PMIC_MPP17_USED_AS_AMUX_INPUT
   /*     MPP17: Analog Input   =>      AMUX8  */
    err |= pm_mpp_config_analog_input(PM_MPP_17,PM_MPP__AIN__CH_AMUX8);
    #endif

    #ifdef FEATURE_PMIC_MPP18_USED_AS_AMUX_INPUT
    /*     MPP18: Analog Input   =>      AMUX9  */
    err |= pm_mpp_config_analog_input(PM_MPP_18,PM_MPP__AIN__CH_AMUX9);
    #endif        

    #ifdef FEATURE_PMIC_MPP22_USED_AS_AMUX_INPUT
#error code not present
    #endif        

    /* Find out if there was an PMIC API error */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
    }
}

/*===========================================================================

FUNCTION pm_app_clk_busy_wait                            EXTERNAL FUNCTION

DESCRIPTION
  Halts the microprocessor execution for the specified amount of time. 
    
PARAMETERS
  1) Parameter name: dvs_enable.
     - How long we want to halt

     Parameter type: uint32
     - Valid Inputs: micro secounds.
     
RETURN VALUE
  None.

DEPENDENCIES
  THIS FUNCTION IS USED BY THE PMIC LIBRARY. PLEASE, DO NOT MODIFY!!!.
  
SIDE EFFECTS
  Halts the microprocessor execution for the specified amount of time. 
  
===========================================================================*/
void pm_app_clk_busy_wait(uint32 us)
{
    /* Halts the Microprocessor for the specified amount of usecs. */
    clk_busy_wait(us);
}

#if defined(FEATURE_PMIC_VARIABLE_PA_VDD)
/*===========================================================================

FUNCTION pm_variable_pa_vdd_init                     EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM66XX to support the
 PA Dynamic Voltage Setting.
  
  This function will:
  1) If dvs_enable (user parameter) == FALSE then
     it will configure PA_FET_N to low to keep the PA 
     in HIGH power mode.   
  2) If dvs_enable (user parameter) == TRUE then
     it will configure PA_FET_N to follow PA_R0.
     See figure bellow.
  3) If dvs_enable (user parameter) == TRUE, FEATURE_RF_PA_DVS 
     is defined and the RF driver is not managing the VREGs then
     this function will configure and enable the VREG PA. 
     
          
                **************************               ******
                *       PM66XX IC        *               *    *
                *                        *               *    *
      PA_R0 --->* PMIC MPPX     PMIC MPPX*- PA_FET_N --> * PA *
                * (INPUT)       (OUTPUT) *               *    *
                *                        *               *    *
                **************************               ******
                
     where:
           PA_FET_N * PA GAIN   * PA VDD
           ---------*-----------*-------
             HIGH   * LOW  GAIN * VREG PA
             LOW    * HIGH GAIN * VPH POWER
                
PARAMETERS
  1) Parameter name: dvs_enable.
     - Enable/Disable PMIC's support to PA DVS.

     Parameter type: boolean.
     - TRUE  = Enable.
     - FALSE = Disable.
     
RETURN VALUE
 None.
    
DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
void pm_variable_pa_vdd_init( boolean dvs_enable )
{  
  pm_err_flag_type           err         = PM_ERR_FLAG__SUCCESS;
  pm_vreg_buck_cfg_cp_type   pa_buck_cfg;
  pm_vreg_pa_dvs_config_type config_tb;

  /* VREG PA internal logic settings. */
  config_tb.discharge_t = 200;
  config_tb.ton         = 5;
  /* Disable by default if the RF driver is over-writing PA_R0 to
     ensure that the external FET is always open every time that the 
     VREG_PA regulator is enabled. */
  config_tb.cmd         = PM_OFF_CMD;  
                                          
  /* PA BUCK VREG compensation parameters. */
  pa_buck_cfg.gm_set       = PA_DVS_GM_CFG;
  pa_buck_cfg.resistor_set = PA_DVS_RESISTOR_CFG;
  pa_buck_cfg.sawtooth_set = PA_DVS_SAWTOOTH_CFG;
  
  if ( dvs_enable )
  {                                   
    /* Configure the PA BUCK VREG compensation parameters. */
    err |= pm_vreg_buck_config_comp(PM_VREG_BUCK_PA_ID, &pa_buck_cfg);
      
    /*                 Configure MPPs                   */

    /*     MPP3: PA_R0/PA_R1  ==>  MPP4: PA_FET_N       */
    /*     VREF = MSMP VREG        VREF = VDD VREG      */
    err |= pm_mpp_config_digital_input( PM_MPP_3, 
                                         PM_MPP__DLOGIC__LVL_MSMP,
                                          PM_MPP__DLOGIC_IN__DBUS_NONE);
    
    err |= pm_mpp_config_digital_output(PM_MPP_4, 
                                         PM_MPP__DLOGIC__LVL_VDD, 
                                          PM_MPP__DLOGIC_OUT__CTRL_MPP);

    /* Configure the VREG PA internal logic and ensure that the external FET is 
       always open every time that the VREG_PA regulator is enabled.  Please refer 
       to the following document for more information.
    
       PM6650 Device Revision Guide, 80-V5773-6
    */
    pm_vreg_pa_dvs_config(&config_tb);

  }
  else
  {
    /*                 Configure MPPs                   */

    /* MPP3: Input, VREF = MSMP VREG. */   
    err |= pm_mpp_config_digital_input( PM_MPP_3, 
                                         PM_MPP__DLOGIC__LVL_MSMP,
                                          PM_MPP__DLOGIC_IN__DBUS_NONE);  
  
    /* Set MPP4: PA_FET_N to low to keep the PA in HIGH mode. */
    err |= pm_mpp_config_digital_output(PM_MPP_4, 
                                         PM_MPP__DLOGIC__LVL_VDD, 
                                          PM_MPP__DLOGIC_OUT__CTRL_LOW);
  }  

  /* If the RF driver is not managing the VREG PA but it wants
     to use the PMIC's support to PA DVS then enable/conf
     the VREG PA. */
  #if (!defined(FEATURE_PMIC_MANAGED_LDO) && \
        defined(FEATURE_RF_PA_DVS))

  /* Configure the VREG PA Voltage. */
  err |= pm_vreg_set_level(PM_VREG_PA_ID, 
                             PM_VREG_PA_VOLTAGE_SETTING);

  /* Enable the VREG PA if the RF driver is not going 
     to enable it. */
  err |= pm_vreg_control(PM_ON_CMD, PM_VREG_PA_M);

  #endif /* FEATURE_PMIC_MANAGED_LDO */

  /* Find out if there was an PMIC API error */
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
    MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
  }
}
#endif /* FEATURE_PMIC_VARIABLE_PA_VDD */

#if defined(FEATURE_PMIC_INTERNAL_IC_TEMPERATURE)
/*===========================================================================

FUNCTION pm_int_temp_protection_init                   EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PMIC's Over Temperature Driver.
                     
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
void pm_int_temp_protection_init(void)
{
  pm_err_flag_type         err   = PM_ERR_FLAG__SUCCESS;

  /* Enable the Over Temperature IRQ. This IRQ will be active if the handset
     power down because the temperature within the PMIC reached stage 3.
     Interrupt will trigger the next time we power the handset on.*/
  err =  pm_set_irq_handle(PM_OVER_TEMP_IRQ_HDL,
                                pm_over_temp_isr);

  /* Enable the over temperature stage changed IRQ. */
  err |= pm_set_irq_handle(PM_T_STAT_CHANGED_IRQ_HDL,
                                pm_temp_state_changed_isr);

  /* Find out if there was an PMIC API error. */
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
    MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
  }
}

/*===========================================================================

FUNCTION pm_over_temp_isr                 INTERNAL FUNCTION

DESCRIPTION
   This function is called if the internal PMIC temperature 
  reaches over-temp stage 3.
    
  NOTE: This IRQ will be active if the handset
        powers down because the temperature within the PMIC reached stage 3.
        Interrupt will trigger the next time we power the handset on.
                     
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_int_temp_protection_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
static void pm_over_temp_isr(void)
{
  pm_err_flag_type         err         = PM_ERR_FLAG__SUCCESS;

  /* Configure a call back to be called after the Handset boots up
     so that we can print in DM warning messages. */
  clk_def( &pm_over_temp_call_back );
  /* Setup up the over temperature callback function. */
  clk_reg( &pm_over_temp_call_back,       /* Pointer to the call back struct*/
            pm_over_temp_cb,              /* Addr of the callback func      */
            (int4)PM_OVER_TEMP_CB_TIME,   /* Delay till first call back     */
            (int4)0,                      /* Delay between repeat call backs*/
            FALSE);                       /* Call once or Periodic Call 
                                             backs?                         */
  /* Clear the over temperature IRQ. */
  err |= pm_clear_irq(PM_OVER_TEMP_IRQ_HDL);

  /* Find out if there was an PMIC API error. */
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
    MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
  }
}

/*===========================================================================

FUNCTION pm_over_temp_cb                              INTERNAL FUNCTION

DESCRIPTION
   This function is a clock called back function set up by pm_over_temp_isr()
  to be called after boot is done to inform the user through DM that a PMIC
  over temperature shut down occurred.
                     
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_int_temp_protection_init()
  5) pm_over_temp_isr()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
static void    pm_over_temp_cb( int4 dummy )
{  //lint --e{715} suppress "dummy not referenced"

    adc_result_type          pm_ic_temp;

    /* De-register the clock call back. */
    clk_dereg( &pm_over_temp_call_back ); 

    pm_ic_temp.degC = 0;     

    /* Get the Internal PMIC temperature. */
    if (adc_read_channel(ADC_PMIC_THERM_DEGC, &pm_ic_temp) != ADC_ERR_FLAG__SUCCESS)
    {
          MSG_ERROR("FAILED TO READ PMIC TEMP",0,0,0);
    }

    /* Inform the user that we detected a Over temperature shut down. */
    MSG_FATAL("OVER TEMP(%dC) SHUT DOWN DETECTED.",pm_ic_temp.degC,0,0);
    
}
/*===========================================================================

FUNCTION pm_temp_state_changed_isr                    INTERNAL FUNCTION

DESCRIPTION
      This function is called if the over temperature stage within the 
    PMIC changed.
    
    Temperature Protection Stages:
                                                                         
        - STAGE0:
            * Temperature Range = T < 105.
            * Normal Condition.
            
        - STAGE1:
            * Temperature Range = 105 > T < 125.         
            * Over Temperature Warning. No action.
        
        - STAGE2:
            * Temperature Range = 125 > T < 145.         
            * Shuts down high current drivers such as speaker and LED 
              drivers automatically. Can be override through
              "pm_itemp_stage_override()".

        - STAGE3:
            * Temperature Range = T > 145.         
            * Powers down the PMIC.    
                     
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_int_temp_protection_init()
  
SIDE EFFECTS
  Disables the PMIC's over temperature stage changed IRQ.
  
===========================================================================*/
static void pm_temp_state_changed_isr(void)
{
  pm_err_flag_type         err         = PM_ERR_FLAG__SUCCESS;
  pm_item_stage_type       itemp_stage = PM_ITEMP_STAGE0;
  adc_result_type          pm_ic_temp;

  pm_ic_temp.degC = 0;

  /* Disable the over temperature stage changed IRQ. */
  err |= pm_set_irq_handle(PM_T_STAT_CHANGED_IRQ_HDL, NULL);

  /* Get the over temperature Stage from the PMIC. */
  err = pm_itemp_get_stage(&itemp_stage);

  /* Get the internal PMIC temperature. */
  if (adc_read_channel(ADC_PMIC_THERM_DEGC, &pm_ic_temp) != ADC_ERR_FLAG__SUCCESS)
  {
        MSG_ERROR("FAIL TO READ PMIC TEMP",0,0,0);
  }

  /* Inform the user which over temperature stage did the PMIC switch to. */
  switch(itemp_stage)
  {                                                              
    case PM_ITEMP_STAGE0:
        MSG_HIGH("PMIC TEMP(%dC) is OK",pm_ic_temp.degC,0,0);
        break;

    case PM_ITEMP_STAGE1:
        MSG_ERROR("STAGE(%d, %dC): PMIC IS GETTING TOO HOT!!",
                  itemp_stage,pm_ic_temp.degC,0);
        break;

    case PM_ITEMP_STAGE2:
        MSG_FATAL("STAGE(%d, %dC): PMIC GOT TOO HOT!!",
                  itemp_stage,pm_ic_temp.degC,0);
        break;

    case PM_ITEMP_STAGE3:
        ERR("STAGE(%d, %dC): TOO HOT, SHUTTING DOWN",
                  itemp_stage,pm_ic_temp.degC,0);
        /* Shut off the handset now!!.*/
        hw_power_off();
        break;

    default:
        MSG_ERROR("INVALID PMIC TEMP STAGE.",0,0,0);
  }

  /* The interrupt is level triggered it can't be cleared for 50ms without it 
     triggering again. So set up a callback to clear and re-enable this IRQ later 
     in time. */
  clk_def( &pm_temp_state_changed_call_back );
  /* Setup up the Temperature Status Changed callback function. */
  clk_reg( &pm_temp_state_changed_call_back, /* Pointer to the call back struct*/
            pm_temp_state_changed_cb,        /* Addr of the callback func      */
            (int4)PM_TEMP_STATE_CHANGED_TIME,/* Delay till first call back     */
            (int4)0,                         /* Delay between repeat call backs*/
            FALSE);                          /* Call once or Periodic Call 
                                                backs? */

  /* Find out if there was an PMIC API error. */
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
    MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
  }
}

/*===========================================================================

FUNCTION pm_temp_state_changed_cb                    INTERNAL FUNCTION

DESCRIPTION
      This function is called to clear and enable the PMIC's over 
  temperature stage changed IRQ. This function is a clock call back
  function set up in pm_temp_state_changed_isr(). 
  
  Note:
       The interrupt is level triggered, it can't be cleared for 50ms without it 
     triggering again. So we set up this callback to clear and re-enable this 
     IRQ later in time.  
  
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_int_temp_protection_init()
  5) pm_temp_state_changed_isr()
  
SIDE EFFECTS
  Clears and Enables the PMIC's over temperature stage changed IRQ.  
  
===========================================================================*/
static void    pm_temp_state_changed_cb( int4 dummy )
{ //lint --e{715} suppress "dummy not referenced"

  pm_err_flag_type         err         = PM_ERR_FLAG__SUCCESS;

  /* De-register the clock call back. */
  clk_dereg( &pm_temp_state_changed_call_back );

  /* Clear the Temperature Status Changed IRQ. */
  err |= pm_clear_irq(PM_T_STAT_CHANGED_IRQ_HDL);
  
  /* Re-Enable the over temperature stage changed IRQ. */
  err |= pm_set_irq_handle(PM_T_STAT_CHANGED_IRQ_HDL,
                                    pm_temp_state_changed_isr);
  /* Find out if there was an PMIC API error. */
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
    MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
  }
}

#endif /* FEATURE_PMIC_INTERNAL_IC_TEMPERATURE */


#if defined(FEATURE_PMIC_32K_XTAL)

void pm_sleep_xtal_halted_isr(void)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

   // Sleep xtal osc has halted and the RC osc has switched on
   // automatically. Inform the sleep clock driver about this
   // as RC osc is not as accurate as xtal osc.
   #if defined(FEATURE_SLEEP_CLK_STABILITY)
   sclk_slow_clock_unstable(TRUE);
   #endif //FEATURE_SLEEP_CLK_STABILITY
   
   // IRQ shouldn't be cleared until we have given xtal osc
   // sufficient time to warm-up. Until the time IRQ is cleared,
   // it needs to be masked though.
   errFlag = pm_set_irq_handle(PM_OSC_HALT_IRQ_HDL, NULL);

   // enable the xtal osc
   errFlag |= pm_xtal_sleep_osc_cmd(PM_ON_CMD);

   // and wait for it to warm-up
   clk_def(&pm_sleep_xtal_osc_timer_client);
   clk_reg(
      &pm_sleep_xtal_osc_timer_client,          // timer client
      pm_sleep_xtal_warmup_timer_expiry_cb,     // timer expiry callback fn
      (int32)PM_SLEEP_XTAL_WARMUP_TIME_MS,      // timer delay until first callback
      0,                                        // timer delay for successive callbacks
      FALSE);                                   // NO successive callbacks

   /* Find out if there was an PMIC API error */
   if (errFlag != PM_ERR_FLAG__SUCCESS) 
   {
       MSG_ERROR("PMIC API ERROR(0x%x)DETECTED in pm_sleep_xtal_halted_isr",errFlag,0,0);
   }
}


static void pm_sleep_xtal_warmup_timer_expiry_cb(int32 dummy)
{  //lint --e{715} suppress "dummy not referenced"

   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   boolean xtal_halted = TRUE;

   // first, de-register the timer client
   clk_dereg(&pm_sleep_xtal_osc_timer_client);

   // we have given enough time for the sleep xtal osc to
   // warm-up; clear the osc halted IRQ. If it is still halted, then 
   // we have a real xtal problem.
   errFlag = pm_clear_irq(PM_OSC_HALT_IRQ_HDL);
   errFlag |= pm_get_rt_status(PM_OSC_HALT_RT_ST, &xtal_halted);
   if ( xtal_halted )
   {
      pm_sleep_xtal_osc_failure_hdlr();
   }
   else
   {
      MSG_HIGH("32KHz sleep xtal was started successfully", 0, 0, 0);

      #if defined(FEATURE_SLEEP_CLK_STABILITY)
      // inform the sleep clock driver that the sleep clock is stable
      sclk_slow_clock_unstable(FALSE);
      #endif //FEATURE_SLEEP_CLK_STABILITY
      
      // Any future failure of xtal osc should be considered a real problem
      errFlag |= pm_set_irq_handle(PM_OSC_HALT_IRQ_HDL, pm_sleep_xtal_osc_failure_hdlr);
   }
   /* Find out if there was an PMIC API error */
   if (errFlag != PM_ERR_FLAG__SUCCESS) 
   {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED in pm_sleep_xtal_warmup_timer_expiry_cb",errFlag,0,0);
   }
}


static void pm_sleep_xtal_osc_failure_hdlr(void)
{
   ERR_FATAL("32KHz sleep xtal failure", 0, 0, 0);
}

#endif //FEATURE_PMIC_32K_XTAL


/*===========================================================================

FUNCTION pm_vote_vreg_switch                          EXTERNAL FUNCTION

DESCRIPTION
      Use this function if 1 or more applications need to use a single
  voltage regulator. This function keeps track of which apps require
  a specific VREG to be enable. This function will only allow the VREG 
  to be disable, if all the apps that have previously enabled the VREG
  through this function, call this function to disable it. 
  
  WARNING!!!!: 
      Do not use this function in conjunction with "pm_vreg_control()". 
    
      Example: if you enable MMC through pm_vreg_control(), you should not 
               use this function to disable it.
  
                     
PARAMETERS
  1) Parameter name: cmd.
     - Turn the VREG ON or OFF

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = Disable the VREG.
            PM_ON_CMD  = Enable the VREG.

  2) Parameter name: vreg_id. 
     - Which VREG needs to be turn ON/OFF.

     Parameter type: pm_vreg_id_type (enum).
     - Valid Inputs:
        PM_VREG_MSMA_ID
        PM_VREG_MSMP_ID
        PM_VREG_MSME_ID
        PM_VREG_MSMC1_ID
        PM_VREG_MSMC2_ID
        PM_VREG_MSMS_ID
        PM_VREG_MEM_ID
        PM_VREG_GP_ID
        PM_VREG_CAM_ID
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
 
  3) Parameter name: vreg_id. 
     - Which Application wants to enable/disable the VREG.

     Parameter type: pm_vote_vreg_app_type (enum).
     
RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
void pm_vote_vreg_switch( pm_switch_cmd_type cmd, 
                          pm_vreg_id_type vreg_id,
                          pm_vote_vreg_app_type app_mask )
{
    pm_err_flag_type      err = PM_ERR_FLAG__SUCCESS;
    uint8 idx;

    /* make sure that this is a valid vreg_id */
    if( vreg_id < PM_VREG_ID_INVALID )
    {
        INTLOCK();

        switch(cmd)
        {
            case PM_ON_CMD:
                /* if no application has voted to turned on this VREG yet */
                if ( pm_vote_vreg_table[vreg_id] == 0x0 )
                {
                    /* walk through the table to see if any callback needs to be called 
                     * BEFORE this VREG turns on
                     */
                    for ( idx = 0; idx < PM_VREG_CB_QUEUE_SIZE; idx++ )
                    {
                        if ( (pm_vote_vreg_cb[vreg_id][idx].callback_func != NULL) &&
                             (pm_vote_vreg_cb[vreg_id][idx].when_to_call == PM_VREG_CB_BEFORE) )
                        {
                            (*pm_vote_vreg_cb[vreg_id][idx].callback_func)(vreg_id, PM_ON_CMD);
                        }
                    }

                    /* Enable the corresponding VREG. */
                    err = pm_vreg_control(PM_ON_CMD, 
                                          (pm_vreg_masked_type)(1ULL << (uint32)vreg_id));

                    /* walk through the table to see if any callback needs to be called 
                     * AFTER this VREG turns on
                     */
                    for ( idx = 0; idx < PM_VREG_CB_QUEUE_SIZE; idx++ )
                    {
                        if ( (pm_vote_vreg_cb[vreg_id][idx].callback_func != NULL) &&
                             (pm_vote_vreg_cb[vreg_id][idx].when_to_call == PM_VREG_CB_AFTER) )
                        {
                            (*pm_vote_vreg_cb[vreg_id][idx].callback_func)(vreg_id, PM_ON_CMD);
                        }
                    }

                }

                /* Set the app bit mask. */
                pm_vote_vreg_table[vreg_id] |= (uint32)app_mask;

                break;
        
            case PM_OFF_CMD:
                /* Clear the app bit mask.*/
                pm_vote_vreg_table[vreg_id] &= ~((uint32)app_mask);
                /* if no other application needs this VREG enable then go 
                   ahead and disable it. */
                if ( pm_vote_vreg_table[vreg_id] == 0x0 )
                {
                    /* walk through the table to see if any callback needs to be called 
                     * BEFORE this VREG turns off
                     */
                    for ( idx = 0; idx < PM_VREG_CB_QUEUE_SIZE; idx++ )
                    {
                        if ( (pm_vote_vreg_cb[vreg_id][idx].callback_func != NULL) &&
                             (pm_vote_vreg_cb[vreg_id][idx].when_to_call == PM_VREG_CB_BEFORE) )
                        {
                            (*pm_vote_vreg_cb[vreg_id][idx].callback_func)(vreg_id, PM_OFF_CMD);
                        }
                    }

                    err = pm_vreg_control(PM_OFF_CMD, 
                                          (pm_vreg_masked_type)(1ULL << (uint32)vreg_id));

                    /* walk through the table to see if any callback needs to be called 
                     * AFTER this VREG turns off
                     */
                    for ( idx = 0; idx < PM_VREG_CB_QUEUE_SIZE; idx++ )
                    {
                        if ( (pm_vote_vreg_cb[vreg_id][idx].callback_func != NULL) &&
                             (pm_vote_vreg_cb[vreg_id][idx].when_to_call == PM_VREG_CB_AFTER) )
                        {
                            (*pm_vote_vreg_cb[vreg_id][idx].callback_func)(vreg_id, PM_OFF_CMD);
                        }
                    }
                }
                break;

            default:
                MSG_ERROR("Parm 1 is out of range = %d.",cmd,0,0);
        }

        INTFREE();

        /* Find out if there was an PMIC API error. */
        if (err != PM_ERR_FLAG__SUCCESS) 
        {
          MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
        }
    }
    else
    {
        MSG_ERROR("PMIC ERROR pm_vote_vreg_switch - invalid vreg_id: 0x%x",
                    vreg_id,0,0);
    }
}

/**
 * pm_vote_vreg_request_vote(pm_vreg_id_type       vreg_id,
 *                           pm_vote_vreg_app_type *vote)
 * 
 * This function should be used to request a vote for use
 * with the pm_vote_vreg_switch() API.
 * 
 * Many votes are already defined in the pm_vote_vreg_app_type
 * enumeration, but as new votes are required, this API
 * allows the application to request votes dynamically
 * instead of requesting additional (constant) votes
 * in the PMIC driver.
 * 
 * @param vreg_id pm_vreg_id_type, this is the VREG for which the application
 *                code is requesting a vote
 * @param vote    A pointer to the vote.  If this function returns TRUE,
 *                vote will be set to the next available vote for vreg_id.
 * 
 * @return TRUE if a vote was available
 *         FALSE if a vote was not available or if vreg_id
 *         was out of range
 * @example 
 * ...
 *  pm_vote_vreg_app_type vote;
 *  boolean result;
 * 
 *  //request a vote for controlling VREG_GP2
 *  result = pm_vote_vreg_request_vote(PM_VREG_GP2_ID,
 *                                     &vote);
 * ...
 *  //vote to turn VREG_GP2 on
 *  (void) pm_vote_vreg_switch(PM_ON_CMD,
 *                             PM_VREG_GP2_ID,
 *                             vote);
 * ...
 */
boolean pm_vote_vreg_request_vote(pm_vreg_id_type       vreg_id, 
                                  pm_vote_vreg_app_type *vote)
{

    /**************************************************************************
    * This table keeps track of which votes have already been provided.
    * The counter starts at 0.  The votes that are returned are 
    * 1 << (PM_VOTE_REQUEST_START - vote_table[vreg_id])
    * The votes that this API provides should not conflict with votes that
    * are already defined in the pm_vote_vreg_app_type enumeration.
    **************************************************************************/
    static uint8 vote_table[PM_VREG_ID_INVALID] = {0};
	boolean err_flag = FALSE;

    /************************************************************
    * check to make sure vreg_id is valid
    * note:  we are not checking here to see if this VREG is
    * supported by this PMIC model
    ************************************************************/
    if (vreg_id < PM_VREG_ID_INVALID)
	{
        /*******************************************************************
        * Lower votes are already assigned in pm_vote_vreg_app_type
        * enumeration .  There are PM_VOTE_MAX_DYNAMIC_VOTES
        * available votes.
        *******************************************************************/
        if (vote_table[vreg_id] < PM_VOTE_MAX_DYNAMIC_VOTES)
		{
			*vote = (pm_vote_vreg_app_type) 
                    (1 << (PM_VOTE_REQUEST_START - vote_table[vreg_id]));
			vote_table[vreg_id]++;
			err_flag = TRUE;
		}
		else
		{
			MSG_ERROR("PMIC ERROR pm_vote_vreg_request_vote() out of votes",0,0,0);
		}
	}
	else
	{
		MSG_ERROR("PMIC ERROR VREG_ID pm_vote_vreg_request_vote() out of range",0,0,0);
	}
	return err_flag;
}

/*===========================================================================

FUNCTION pm_register_vreg_switch_callback                    EXTERNAL FUNCTION

DESCRIPTION
    This function allows registration of a callback to be called upon state
change of a specified VREG. The callback can also specified to call before
or after the VREG state change happens.
  
                     
PARAMETERS

  1) Parameter name: vreg_id. 
     - Which VREG to be registered with a callback function 

     Parameter type: pm_vreg_id_type (enum).
 
  2) Parameter name: when_to_call 
     - whether the callback to be called before or after the VREG changes state

     Parameter type: pm_vreg_cb_when_type (enum).
     - Valid Inputs:
        PM_VREG_CB_BEFORE
        PM_VREG_CB_AFTER

  2) Parameter name: vreg_cb 
     - the function pointer of the callback to be registered

     
RETURN VALUE
  boolean: 
    TRUE - callback registeration succeeded
    FALSE - callback registeration failed

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
boolean pm_register_vreg_switch_callback( pm_vreg_id_type vreg_id,  
                                          pm_vreg_cb_when_type when_to_call,
                                          void (*vreg_cb)(pm_vreg_id_type, pm_switch_cmd_type) )
{
    boolean cb_registered = FALSE;
    uint8 idx;
    
    if( vreg_id < PM_VREG_ID_INVALID )
    {
        if ( vreg_cb != NULL )
        {
            if ( when_to_call < PM_VREG_CB_INVALID )
            {
                for ( idx = 0; idx < PM_VREG_CB_QUEUE_SIZE; idx++ )
                {
                    if ( pm_vote_vreg_cb[vreg_id][idx].callback_func == NULL )
                    {
                        /* found a free space in the registration table, register the callback */
                        pm_vote_vreg_cb[vreg_id][idx].callback_func = vreg_cb;

                        pm_vote_vreg_cb[vreg_id][idx].when_to_call = when_to_call;

                        cb_registered = TRUE;
                        /* successfully registered the callback, quit searching right away */
                        break;
                    }
                }

                if ( cb_registered == FALSE )
                {
                    MSG_ERROR("PMIC ERROR pm_register_vreg_switch_callback failed to register callback - \
                               registration queue overflow",0,0,0);
                }
            }
            else
            {
                MSG_ERROR("PMIC ERROR pm_register_vreg_switch_callback - invalid call type: 0x%x",
                           when_to_call,0,0);
            }
        }
        else
        {
            MSG_ERROR("PMIC ERROR pm_register_vreg_switch_callback failed to register NULL callback", 0,0,0);
        }
    }
    else
    {
        MSG_ERROR("PMIC ERROR pm_register_vreg_switch_callback - invalid vreg_id: 0x%x",
                    vreg_id,0,0);
    }

    return cb_registered;
}


/*===========================================================================

FUNCTION pm_deregister_vreg_switch_callback                 EXTERNAL FUNCTION

DESCRIPTION
    This function allows deregistration of a callback to associated with a 
specified VREG. 
  
PARAMETERS

  1) Parameter name: vreg_id. 
     - Which VREG to be deregistered with a callback function 

     Parameter type: pm_vreg_id_type (enum).
 
  2) Parameter name: when_to_call 
     - whether the callback  is called before or after the VREG changes state

     Parameter type: pm_vreg_cb_when_type (enum).
     - Valid Inputs:
        PM_VREG_CB_BEFORE
        PM_VREG_CB_AFTER

  2) Parameter name: vreg_cb 
     - the function pointer of the callback to be deregistered

     
RETURN VALUE
  boolean: 
    TRUE - callback deregisteration succeeded
    FALSE - callback deregisteration failed

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
boolean pm_deregister_vreg_switch_callback( pm_vreg_id_type vreg_id,  
                                            pm_vreg_cb_when_type when_to_call,
                                            void (*vreg_cb)(pm_vreg_id_type, pm_switch_cmd_type) )
{
    boolean cb_deregistered = FALSE;
    uint8 idx;
    
    if( vreg_id < PM_VREG_ID_INVALID )
    {
        if ( vreg_cb != NULL )
        {
            for ( idx = 0; idx < PM_VREG_CB_QUEUE_SIZE; idx++ )
            {
                if ( (pm_vote_vreg_cb[vreg_id][idx].callback_func == vreg_cb) &&
                     (pm_vote_vreg_cb[vreg_id][idx].when_to_call == when_to_call) )
                {
                    /* found the callback function that matches, deregister it */
                    pm_vote_vreg_cb[vreg_id][idx].callback_func = NULL;

                    cb_deregistered = TRUE;
                    /* successfully deregistered the callback, quit searching right away */
                    break;
                }
            }

            if ( cb_deregistered == FALSE )
            {
                MSG_ERROR("PMIC ERROR pm_deregister_vreg_switch_callback failed to deregister callback",
                           0,0,0);
            }
        }
        else
        {
            MSG_ERROR("PMIC ERROR pm_deregister_vreg_switch_callback failed to deregister NULL callback", 0,0,0);
        }
    }
    else
    {
        MSG_ERROR("PMIC ERROR pm_deregister_vreg_switch_callback - invalid vreg_id: 0x%x",
                    vreg_id,0,0);
    }

    return cb_deregistered;
}


/*===========================================================================

FUNCTION pm_perform_vbatdet_calibration                        EXTERNAL FUNCTION

DESCRIPTION
      This function is used for calibrating vbatdet
  
PARAMETERS
  None   
 
RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  1) Enable TX WAIT state( disable pulse charging) before calling calibration 
  routine and disable TX WAIT state( enable pulse charging) after calibration.
  eg.  //Enable TX WAIT state, disable pulse charging
       chg_tx_wait_vote(CHG_CMD_ENABLE, CHG_TX_VOTE__VBATDET);

       pm_perform_vbatdet_calibration();

       //disable TX WAIT state, enable pulse charging
       chg_tx_wait_vote(CHG_CMD_DISABLE, CHG_TX_VOTE__VBATDET);
  
  2) During the calibration procedure, VMAXSEL has been changed to 4.2V. So 
  make sure to restore the to original value after the calibration is finished.
  
SIDE EFFECTS
  None  
===========================================================================*/
pm_err_flag_type pm_perform_vbatdet_calibration(void)
{
    pm_err_flag_type    errFlag                 = PM_ERR_FLAG__SUCCESS;
    boolean                  vbatdet_rt_st_curr = TRUE;
    boolean             vbatdet_trim_successful = FALSE;
    pm_switch_cmd_type            batt_Q_status = PM_INVALID_CMD;

    const pm_chg_q_vmax_type  vmaxsel_cal_val   = PM_CHG_Q_VMAXSEL_4200mV;
    const uint16        vbatdet_cal_val         = 4200; /* mV */

    uint8               vbatdet_trim_val_min    = 0;
    uint8               vbatdet_trim_val_max    = 0;
    uint8               vbatdet_trim_val_temp   = 0;

    /* get vbatdet trim limits */
    errFlag |= pm_get_vbatdet_trim_limits(&vbatdet_trim_val_min, &vbatdet_trim_val_max);

    /* get battery transistor status */
    errFlag |= pm_chg_get_battery_transistor_status(&batt_Q_status);

   /************************************
   *     start of calibration routine  *
   ************************************/

    /* open battery FET if needed */
    if(PM_ON_CMD == batt_Q_status)
    {
        errFlag |= pm_chg_battery_transistor_switch(PM_OFF_CMD);
    }
    
    /* set the VMAXSEL and VBATDET voltage */
    errFlag |= pm_chg_pulse_charger_vbatdet_config(vbatdet_cal_val);
    errFlag |= pm_chg_charger_transistor_vmaxsel(vmaxsel_cal_val);
    
    /* enable VBATDET trim test mode */
    errFlag |= pm_vbatdet_trim_test_mode_switch(PM_ON_CMD);
    
    /* increase trim value until STATUS_RTVBATDET goes from 1 to 0 
       within the trim limits */
    vbatdet_trim_val_temp  = vbatdet_trim_val_min;
    while ( !vbatdet_trim_successful )
    {
        errFlag |= pm_set_vbatdet_trim(vbatdet_trim_val_temp);
        errFlag |= pm_get_rt_status(PM_VBAT_DET_RT_ST, &vbatdet_rt_st_curr);
        
        if( ( FALSE == vbatdet_rt_st_curr) || 
            ( vbatdet_trim_val_max == vbatdet_trim_val_temp) )
        {
             vbatdet_trim_successful = TRUE;
        }
        else
        {
            vbatdet_trim_val_temp ++;
        }
    }

    /* disable the VBATDET trim test */
    errFlag |= pm_vbatdet_trim_test_mode_switch(PM_OFF_CMD);

    /* Restore the batt FET to the original status */
    if(PM_ON_CMD == batt_Q_status)
    {
        errFlag |= pm_chg_battery_transistor_switch(PM_ON_CMD);
    }

     /**********************************
     *     end of calibration routine  *
     **********************************/

    /* Find out if there was an PMIC API error. */
    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API VBATDET CALIBRATION ERROR(0x%x) DETECTED", errFlag, 0, 0);
    }
    
    return errFlag;
    
}



#if defined(FEATURE_PMIC_VIDEO_AMPLIFIER)

/*===========================================================================

FUNCTION pm_vid_init                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the video amplifier load/unload detect settings and ISR 
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
void pm_vid_init(void)
{   
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

    /* Disable Video amplifier */
    errFlag |= pm_vid_en(FALSE);
        
   /*Disable load/unload IRQs through masking to avoid false interrupt*/
    errFlag |= pm_set_irq_handle(PM_VIDEOAMP_LOAD_DET_IRQ_HDL,NULL); 
    errFlag |= pm_set_irq_handle(PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL,NULL); 

    /* Enable load detect */
    errFlag |= pm_vid_load_detect_en(PM_ON_CMD);

    /*Clear the load detect IRQ */
    errFlag |= pm_clear_irq(PM_VIDEOAMP_LOAD_DET_IRQ_HDL);

    /* Unmask the LOAD interrupt*/
    errFlag |= pm_set_irq_handle(PM_VIDEOAMP_LOAD_DET_IRQ_HDL, pm_vid_load_detect_isr); 
        
    
    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR DETECTED",0,0,0);
    }
}
/*===========================================================================

FUNCTION pm_vid_load_detect_isr                        INTERNAL FUNCTION

DESCRIPTION
  This is the video load detection ISR.
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  
  
SIDE EFFECTS
  None
  
===========================================================================*/

static void pm_vid_load_detect_isr(void)
{
    pm_err_flag_type    err       = PM_ERR_FLAG__SUCCESS;
    boolean             rt_status;
   
   /* First debounce the IRQ. */
   err |= pm_get_rt_status(PM_VIDEOAMP_LOAD_DET_RT_ST, &rt_status);

   if (rt_status) 
   {
       MSG_FATAL("TV OUT:  Load Detect",0,0,0);

       // Enable the video amplifier; at this point the video content should be pushed out
       pm_vid_en(TRUE);

       /* Disable the load detect IRQ through masking */
       pm_set_irq_handle(PM_VIDEOAMP_LOAD_DET_IRQ_HDL, NULL); 
       /*Clear the load detect IRQ */
       pm_clear_irq(PM_VIDEOAMP_LOAD_DET_IRQ_HDL); 

       /* Reset the unload detect IRQ */
       err |= pm_clear_irq(PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL);
       /* Enable the unload detect IRQ through unmasking */
       err |= pm_set_irq_handle(PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL, pm_vid_unload_detect_isr);

   }
    else
    {
       /* Reset/clear the load detect IRQ */
       err |= pm_clear_irq(PM_VIDEOAMP_LOAD_DET_IRQ_HDL); 
       MSG_FATAL("FALSE Load detect IRQ.",0,0,0);
       
   }
   
   /* Find out if there was an PMIC API error */
   if (err != PM_ERR_FLAG__SUCCESS) 
   {
       MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
   }
       
}
/*===========================================================================

FUNCTION pm_vid_unload_detect_isr                        INTERNAL FUNCTION

DESCRIPTION
  This is the video unload detection ISR.
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_vid_unload_detect_isr(void)
{

    pm_err_flag_type    err       = PM_ERR_FLAG__SUCCESS;
    boolean             rt_status;

   /* First debounce the IRQ. */
   err |= pm_get_rt_status(PM_VIDEOAMP_UNLOAD_DET_RT_ST, &rt_status);

   if (rt_status) 
   {
       MSG_FATAL("TV OUT:  Unload Detect",0,0,0);

       // Disable the video amplifier; at this point the video content should be pushed out
       pm_vid_en(FALSE);
       /* Disable the unload detect IRQ through masking */
       pm_set_irq_handle(PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL, NULL); 
       /*Clear the unload detect IRQ */
       pm_clear_irq(PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL); 

       /* Reset the load detect IRQ */
       err |= pm_clear_irq(PM_VIDEOAMP_LOAD_DET_IRQ_HDL);
       /* Enable the load detect IRQ through unmasking */
       err |= pm_set_irq_handle(PM_VIDEOAMP_LOAD_DET_IRQ_HDL, pm_vid_load_detect_isr);

   }
    else
    {
       /* Reset/clear the load detect IRQ */
       err |= pm_clear_irq(PM_VIDEOAMP_UNLOAD_DET_IRQ_HDL); 
       MSG_FATAL("FALSE Unload detect IRQ.",0,0,0);
       
   }

   /* Find out if there was an PMIC API error */
   if (err != PM_ERR_FLAG__SUCCESS) 
   {
       MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
   }
}
#endif /* FEATURE_PMIC_VIDEO_AMPLIFIER */


#endif /* FEATURE_PMIC */

