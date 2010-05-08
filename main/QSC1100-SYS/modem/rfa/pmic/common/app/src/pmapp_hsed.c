/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 P M A P P _ H S E D . C

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

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_hsed.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/08   jtn     Added software workaround for type 2 OTHC
10/23/08   jtn     Added run-time detection and configuration of headset driver 
                   depending on PMIC hardware model/revision
09/30/08   jtn     New file containing headset send-end detect functions and vars to new
                   pmapp_hsed.c file
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "pm.h"
#include "err.h"
#include "clk.h"

#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
#include "keypad.h"
#endif //FEATURE_KEYPAD_OWNS_KEYPRESS_ISR


#if defined(FEATURE_PMIC_HS_SEND_END_DETECT) && !defined(BUILD_BOOT_CHAIN)

static boolean     pm_hsed_key_pressed = FALSE;
static boolean     pm_hsed_inserted    = FALSE;

static void pm_hsed_init_type_0(void);
static void pm_hsed_released_isr_type_0(void);
static void pm_hsed_pressed_isr_type_0(void);

static void pm_hsed_init_type_1(void);
static void pm_hsed_no_hdlr_type_1(void);

static void pm_hsed_init_type_2(void);
static void pm_hsed_detout_no_isr_type_2(void);
static void pm_hsed_detout_nc_isr_type_2(void);

/* variables and declarations needed for PMIC type 2 software workaround */
static clk_cb_type pm_hsed_key_clk_call_back;
static void pm_hsed_key_release_timer(int4 dummy);

#define PM_HSED_TYPE2_RELEASE_POLL_TIME 50
#define PM_HSED_TYPE2_RELEASE_POLL_COUNT 3

struct 
{
    /* the time between polls for switch release */
    int                         poll_time;
    /* number of times the poll must return "released" before deciding that the switch
    *  is released */
    int                         release_count;  
    /* saved setting for hysteresis pre-divider */
    pm_hsed_hyst_pre_div_type   hyst_pre_div;
    /* saved setting for hysteresis time */
    pm_hsed_hyst_time_type      hyst_time; 
} pm_hsed_type2_settings;

/*===========================================================================

FUNCTION pm_hsed_released_isr_type_0                        INTERNAL FUNCTION

DESCRIPTION
  This is the headset button released ISR. It simply prints out an F3 message
and re-enables the pressed ISR.
    
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
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_hsed_released_isr_type_0(void)
{
    (void)pm_set_irq_handle(PM_HS_SED_OUTB_IRQ_HDL, NULL);
    (void)pm_clear_irq(PM_HS_SED_OUTB_IRQ_HDL);

    (void)pm_clear_irq(PM_HS_SED_OUT_IRQ_HDL);
    (void)pm_set_irq_handle(PM_HS_SED_OUT_IRQ_HDL, pm_hsed_pressed_isr_type_0);

    MSG_FATAL("Headset switch released!", 0, 0, 0);
}

/*===========================================================================

FUNCTION pm_hsed_pressed_isr_type_0                         INTERNAL FUNCTION

DESCRIPTION
  This is the headset button pressed ISR. It simply prints out an F3 message
and re-enables the released ISR.
    
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
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_hsed_pressed_isr_type_0(void)
{
    (void)pm_set_irq_handle(PM_HS_SED_OUT_IRQ_HDL, NULL);
    (void)pm_clear_irq(PM_HS_SED_OUT_IRQ_HDL);
    pm_hsed_key_pressed = TRUE;

    #ifndef FEATURE_MULTIPROCESSOR
    #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
    /* Make sure the keypad driver knows there has been a keypress */
    keypad_enable_polling();
    #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
    #endif /* FEATURE_MULTIPROCESSOR */
    
    (void)pm_clear_irq(PM_HS_SED_OUTB_IRQ_HDL);
    (void)pm_set_irq_handle(PM_HS_SED_OUTB_IRQ_HDL, pm_hsed_released_isr_type_0);

    MSG_FATAL("Headset switch pressed!", 0, 0, 0);
}


/*===========================================================================

FUNCTION pm_hsed_init_type_0                               EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the headset send/end detect settings and ISR 
    
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
static void pm_hsed_init_type_0(void)
{   
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

    errFlag |= pm_hsed_set_switch_type(PM_HSED_SC_SWITCH_TYPE, PM_HSED_SC_IDAC_600uA);
    errFlag |= pm_hsed_enable(PM_HSED_ENABLE_PWM_TCXO);
    errFlag |= pm_hsed_set_hysteresis(PM_HSED_HYST_PRE_DIV_2, PM_HSED_HYST_TIME_10_CLK_CYCLES);
    errFlag |= pm_hsed_set_period(PM_HSED_PERIOD_PRE_DIV_2, PM_HSED_PERIOD_TIME_10_CLK_CYCLES);

    errFlag |= pm_clear_irq(PM_HS_SED_OUT_IRQ_HDL);
    errFlag |= pm_set_irq_handle(PM_HS_SED_OUT_IRQ_HDL, pm_hsed_pressed_isr_type_0);

    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR DETECTED",0,0,0);
    }
}


/*******************************************************************************
* Type 2 HSED module notes:
* The module starts with a default assumption of nothing inserted, if something
* is already inserted, there should be a 0 -> 1 transition on the NC_IR
* interrupt bit.  This will happen very fast, so I suppose there might need
* to be a one time check at startup for the NC_IR bit being 1.  This is only
* important for the IR portion of the NC_IR.  For the use case of supporting
* both button press/release for NO and NC type headsets, the one time check
* isn’t needed since the insertion/removal needs to be done via an alternate
* method.
*******************************************************************************/
/********************************************************
* The insertion event caused a NC “even” number result.
* The removal event caused a NC “odd” number result.
*
* The press event caused a NO “even” number result.
* The release event caused a NO “odd” number result.
********************************************************/

/**
 * Function:  pm_hsed_key_release_timer
 * 
 * This function is registered with the clk_reg() API.  It is called 
 * periodically to poll the real-time status of the HSED/OTHC switch.
 * 
 * When the software polling has determined that the switch is
 * released, the function de-registers itself and re-enables the switch 
 * press interrupt.
 * 
 * @param dummy  This parameter is used for compatibility with the clk_reg() API
 */
static void pm_hsed_key_release_timer(int4 dummy)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    static int poll_counter = 0;

    boolean status;
    errFlag = pm_get_rt_status(PM_DETOUT_NO_CHGED_IRQ_HDL, &status);
    if (status == TRUE)  //switch is still pressed
    {
        poll_counter = 0;  //reset the counter
    }
    else
    {
        if (++poll_counter >= pm_hsed_type2_settings.release_count)
        {
            pm_hsed_key_pressed = FALSE;
            MSG_FATAL("HSED NO type key released", 0, 0, 0);
            #ifndef FEATURE_MULTIPROCESSOR
            #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
            /* Make sure the keypad driver knows there has been a keypress */
            keypad_enable_polling();
            #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
            #endif /* FEATURE_MULTIPROCESSOR */
            poll_counter = 0;  //reset the counter
            /* turn off polling timer */
            clk_dereg( &pm_hsed_key_clk_call_back);
            //set the hysteresis back to "normal" 
            errFlag |= pm_hsed_set_hysteresis(pm_hsed_type2_settings.hyst_pre_div, pm_hsed_type2_settings.hyst_time);
            /* Enable the HSED interrupt */
            errFlag |= pm_clear_irq(PM_DETOUT_NO_CHGED_IRQ_HDL);
            errFlag |= pm_set_irq_handle(PM_DETOUT_NO_CHGED_IRQ_HDL, pm_hsed_detout_no_isr_type_2);
        }
    }
}



/*===========================================================================

FUNCTION pm_hsed_detout_no_isr_type_2                       INTERNAL FUNCTION

DESCRIPTION
  This is the headset button pressed ISR. It prints out an F3 message
and notifies the keypad driver.
    
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
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_hsed_detout_no_isr_type_2(void)
{

    pm_clear_irq(PM_DETOUT_NO_CHGED_IRQ_HDL);

    MSG_FATAL("HSED NO type key pressed", 0, 0, 0);

    pm_hsed_key_pressed = TRUE;

    #ifndef FEATURE_MULTIPROCESSOR
    #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
    /* Make sure the keypad driver knows there has been a keypress */
    keypad_enable_polling();
    #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
    #endif /* FEATURE_MULTIPROCESSOR */

    //disable the DETOUT_NO interrupt
    pm_set_irq_handle(PM_DETOUT_NO_CHGED_IRQ_HDL, NULL);
    //set the hysteresis to minimum <note:  need a variable to keep track of this>
    pm_hsed_set_hysteresis(PM_HSED_HYST_PRE_DIV_1, PM_HSED_HYST_TIME_1_CLK_CYCLES);

    //enable polling timer for release event
    /* Setup up a periodic timer to poll the HSED key for a release event */
    clk_reg( &pm_hsed_key_clk_call_back, pm_hsed_key_release_timer, 
             pm_hsed_type2_settings.poll_time, pm_hsed_type2_settings.poll_time, TRUE ); 
}

/*===========================================================================
FUNCTION pm_hsed_detout_nc_isr_type_2                       INTERNAL FUNCTION

DESCRIPTION
  This is the headset insert/remove ISR. It prints out an F3 message
and notifies the keypad driver.
  VREG_NCP is enabled when a headset is inserted and disabled when
  a headset is removed if FEATURE_AUDIO_HEADSET_CAPLESS_MODE is defined.    
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
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_hsed_detout_nc_isr_type_2(void)
{
    pm_clear_irq(PM_DETOUT_NC_CHGED_IRQ_HDL);

    if (pm_hsed_inserted == FALSE)
    {
        MSG_FATAL("HSED NO type inserted", 0, 0, 0);
//        MSG_FATAL("HSED NC type key pressed", 0, 0, 0);
        pm_hsed_inserted = TRUE;
        #if defined (FEATURE_AUDIO_HEADSET_CAPLESS_MODE)
        /* turn on the negative charge pump VREG */
        pm_vreg_control(PM_ON_CMD, PM_VREG_NCP_M);
        #endif

    }
    else
    {
        MSG_FATAL("HSED NO type removed", 0, 0, 0);
//        MSG_FATAL("HSED NC type key released", 0, 0, 0);
        pm_hsed_inserted = FALSE;
        #if defined (FEATURE_AUDIO_HEADSET_CAPLESS_MODE)
        /* turn off the negative charge pump VREG */
        pm_vreg_control(PM_OFF_CMD, PM_VREG_NCP_M);
        #endif
    }
    #ifndef FEATURE_MULTIPROCESSOR
    #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
    /* Make sure the keypad driver knows there has been a change in HSED 
     * insert state */
    keypad_enable_polling();
    #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
    #endif /* FEATURE_MULTIPROCESSOR */

}
/*===========================================================================
FUNCTION pm_hsed_no_hdlr_type_1                             INTERNAL FUNCTION

DESCRIPTION
  This is the headset insert/remove ISR. It prints out an F3 message
and notifies the keypad driver.

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
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void pm_hsed_no_hdlr_type_1(void)
{
    pm_hsed_state_type_1 hsed_status = PM_HSED_STATE_TYPE1_INVALID;

    (void) pm_hsed_status_type_1_get(&hsed_status);

    pm_clear_irq(PM_HS_SED_OUT_IRQ_HDL);
    pm_clear_irq(PM_HS_SED_OUTB_IRQ_HDL);


    switch (hsed_status)
    {
    case PM_HSED_STATE_TYPE1_PRESS:
        MSG_FATAL("Headset switch pressed!", 0, 0, 0);
        pm_hsed_key_pressed = TRUE;
        break;
    case PM_HSED_STATE_TYPE1_RELEASE:
        MSG_FATAL("Headset switch released!", 0, 0, 0);
        break;
    case PM_HSED_STATE_TYPE1_INSERT:
        MSG_FATAL("Headset switch inserted!", 0, 0, 0);
        pm_hsed_inserted = TRUE;
        #if defined (FEATURE_AUDIO_HEADSET_CAPLESS_MODE)
        /* turn on the negative charge pump VREG */
        pm_vreg_control(PM_ON_CMD, PM_VREG_NCP_M);
        #endif
        break;
    case PM_HSED_STATE_TYPE1_REMOVE:
        MSG_FATAL("Headset switch removed!", 0, 0, 0);
        pm_hsed_inserted = FALSE;
        #if defined (FEATURE_AUDIO_HEADSET_CAPLESS_MODE)
        /* turn off the negative charge pump VREG */
        pm_vreg_control(PM_OFF_CMD, PM_VREG_NCP_M);
        #endif
        break;
    case PM_HSED_STATE_TYPE1_INVALID: default:
        MSG_FATAL("Headset switch UNDETERMINED!", 0, 0, 0);
        break;

    }
    switch (hsed_status)
    {
    case PM_HSED_STATE_TYPE1_PRESS:
    case PM_HSED_STATE_TYPE1_INSERT:
    case PM_HSED_STATE_TYPE1_REMOVE:
        #ifndef FEATURE_MULTIPROCESSOR
        #if defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
        /* Make sure the keypad driver knows there has been a keypress */
        keypad_enable_polling();
        #endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */
        #endif /* FEATURE_MULTIPROCESSOR */
        break;
    case PM_HSED_STATE_TYPE1_RELEASE:
    case PM_HSED_STATE_TYPE1_INVALID: default:
        break;
    }
}
/*===========================================================================

FUNCTION pm_hsed_init_type_2                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the headset send/end detect settings and ISR 
    
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
static void pm_hsed_init_type_1(void)
{   
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;

    errFlag |= pm_hsed_set_switch_type(PM_HSED_SC_SWITCH_TYPE, PM_HSED_SC_IDAC_600uA);
    errFlag |= pm_hsed_enable(PM_HSED_ENABLE_PWM_TCXO);
    errFlag |= pm_hsed_set_hysteresis(PM_HSED_HYST_PRE_DIV_2, PM_HSED_HYST_TIME_10_CLK_CYCLES);
    errFlag |= pm_hsed_set_period(PM_HSED_PERIOD_PRE_DIV_2, PM_HSED_PERIOD_TIME_10_CLK_CYCLES);

    errFlag |= pm_clear_irq(PM_HS_SED_OUT_IRQ_HDL);
    errFlag |= pm_clear_irq(PM_HS_SED_OUTB_IRQ_HDL);
    errFlag |= pm_set_irq_handle(PM_HS_SED_OUT_IRQ_HDL, pm_hsed_no_hdlr_type_1);
    errFlag |= pm_set_irq_handle(PM_HS_SED_OUTB_IRQ_HDL, pm_hsed_no_hdlr_type_1);

    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR DETECTED",0,0,0);
    }
}

/*===========================================================================

FUNCTION pm_hsed_init_type_2                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the headset send/end detect settings and ISR 
    
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
static void pm_hsed_init_type_2(void)
{   
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    boolean irq_status;

    pm_hsed_type2_settings.hyst_pre_div = PM_HSED_HYST_PRE_DIV_16;
    pm_hsed_type2_settings.hyst_time = PM_HSED_HYST_TIME_16_CLK_CYCLES;


    errFlag |= pm_hsed_set_current_threshold(PM_HSED_SC_SWITCH_TYPE, 700);
    errFlag |= pm_hsed_enable(PM_HSED_ENABLE_PWM_TCXO);

    errFlag |= pm_hsed_set_hysteresis(pm_hsed_type2_settings.hyst_pre_div, pm_hsed_type2_settings.hyst_time);
    errFlag |= pm_hsed_set_period(PM_HSED_PERIOD_PRE_DIV_64, PM_HSED_PERIOD_TIME_6_CLK_CYCLES);

    //read the NC IRQ status to see if the headset is already inserted?
    errFlag |= pm_get_rt_status(PM_DETOUT_NC_CHGED_IRQ_HDL, &irq_status);
    if (irq_status == TRUE)
    {
        pm_hsed_inserted = TRUE;
    }
    pm_hsed_type2_settings.poll_time = PM_HSED_TYPE2_RELEASE_POLL_TIME;
    pm_hsed_type2_settings.release_count = PM_HSED_TYPE2_RELEASE_POLL_COUNT;

    errFlag |= pm_clear_irq(PM_DETOUT_NO_CHGED_IRQ_HDL);
    errFlag |= pm_set_irq_handle(PM_DETOUT_NO_CHGED_IRQ_HDL, pm_hsed_detout_no_isr_type_2);
    errFlag |= pm_clear_irq(PM_DETOUT_NC_CHGED_IRQ_HDL);
    errFlag |= pm_set_irq_handle(PM_DETOUT_NC_CHGED_IRQ_HDL, pm_hsed_detout_nc_isr_type_2);

    if (errFlag != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR DETECTED",0,0,0);
    }
}
/*===========================================================================

FUNCTION pm_hsed_init                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the headset send/end detect settings and ISR 
    
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
void pm_hsed_init()
{
    pm_hsed_module_type module_type = PM_HSED_MODULE_TYPE__INVALID;

    pm_hsed_get_module_type(&module_type);

    switch (module_type)
    {
    case PM_HSED_MODULE_TYPE__0:
        pm_hsed_init_type_0();
        break;
    case PM_HSED_MODULE_TYPE__1:
        pm_hsed_init_type_1();
        break;
    case PM_HSED_MODULE_TYPE__2:
        pm_hsed_init_type_2();
        break;
    default:
        break;
    }
}

/* the APIs below are called from HS to return the current state of the HSED */
/*===========================================================================

FUNCTION pm_hsed_get_inserted                               EXTERNAL FUNCTION

DESCRIPTION
  This funcion returns true if the headset is inserted, false if 
  it is removed

PARAMETERS
  None.  

RETURN VALUE
  Boolean: TRUE = there is a headset inserted 
           FALSE = there is no headset inserted 

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_hsed_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean pm_hsed_get_inserted(void)
{
    boolean curr_status = FALSE;

    INTLOCK();
    curr_status = pm_hsed_inserted;
    INTFREE();

    return curr_status;
    
}


/*===========================================================================

FUNCTION pm_hsed_get_key_pressed                           EXTERNAL FUNCTION

DESCRIPTION
  This funcion returns the latched status of the HSED button press prior to
calling this function. The latched status requires pm_hsed_clear_key_pressed to
clear
    
PARAMETERS
  None.  

RETURN VALUE
  Boolean: TRUE = a HSED button has been pressed prior to function call 
           FALSE = no HSED button has been pressed prior to function call 

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_hsed_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean pm_hsed_get_key_pressed(void)
{
   boolean curr_status = FALSE;

   INTLOCK();
   curr_status = pm_hsed_key_pressed;
   INTFREE();

   return curr_status;
}

/*===========================================================================

FUNCTION pm_hsed_clear_key_pressed                           EXTERNAL FUNCTION

DESCRIPTION
  This funcion clears the latched status of the HSED button press 
    
PARAMETERS
  None.  

RETURN VALUE
  None  

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_hsed_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
void pm_hsed_clear_key_pressed(void)
{
   INTLOCK();
   pm_hsed_key_pressed = FALSE;
   INTFREE();
}

#endif /* FEATURE_PMIC_HS_SEND_END_DETECT */

