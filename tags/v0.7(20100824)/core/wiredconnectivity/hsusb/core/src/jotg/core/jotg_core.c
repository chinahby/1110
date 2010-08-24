/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* *  Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com          */
#include <jos.h>
#include <otg_core_ocd.h>
#include <uw_otg_app.h>
#include <otg_core_host.h>
#include "jotg_core.h"

/* HSU addition */
#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC
#include "ocd_tdi_4x.h"

#define TDI_4X_OTG_WRITE_REG(sc, offset, value) \
  j_bus_space_write_4((sc)->reg_res, offset, value)
#define TDI_4X_OTG_READ_REG(sc, offset) \
  j_bus_space_read_4((sc)->reg_res, offset)
#endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */

#define OTG_BUILD_TIMER_TIME_ARRAY(arr) \
    do { \
        (arr)[0]  = OTG_TIMER_TIME_NONE;            \
        (arr)[1]  = OTG_TIMER_TIME_A_WAIT_VRISE;    \
        (arr)[2]  = OTG_TIMER_TIME_A_WAIT_BCON;     \
        (arr)[3]  = OTG_TIMER_TIME_A_AIDL_BDIS;     \
        (arr)[4]  = OTG_TIMER_TIME_B_ASE0_BRST;     \
        (arr)[5]  = OTG_TIMER_TIME_B_DATA_PLS;      \
        (arr)[6]  = OTG_TIMER_TIME_B_VBUS_PLS;      \
        (arr)[7]  = OTG_TIMER_TIME_B_BUS_REQ;       \
        (arr)[8]  = OTG_TIMER_TIME_A_SUSPEND_DET;   \
        (arr)[9]  = OTG_TIMER_TIME_A_BCON_DEBOUNCE; \
        (arr)[10] = OTG_TIMER_TIME_B_AIDL_BDIS;     \
        (arr)[11] = OTG_TIMER_TIME_A_WAIT_VFALL;    \
    } while(0)

#define OTG_START_TIMER(sc, type) \
    do { \
        (sc)->state_machine.timeout = 0; \
        (sc)->state_machine.timer_running = 1; \
        (sc)->state_machine.timer_type = (type); \
        (sc)->ocd_ops->ocd_start_timer((sc)->ocd_ctx, \
            (sc)->otg_timer_times[(jint_t)(type)]); \
    } while(0)

#define OTG_TIMEOUT_OCCURRED(sc, type) \
    ((sc)->state_machine.timeout && \
        ((sc)->state_machine.timer_type == type))

#define OTG_STOP_TIMER(sc) \
    do { \
        (sc)->ocd_ops->ocd_stop_timer((sc)->ocd_ctx); \
        (sc)->state_machine.timeout = 0; \
        (sc)->state_machine.timer_running = 0; \
        (sc)->state_machine.timer_type = OTG_TIMER_TYPE_NONE; \
    } while(0)

#define OTG_CHANGE_STATE(sc, new_state, change) \
    do { \
        (sc)->next_state_handler = (new_state); \
        (change) = 1; \
    } while(0)

#define OTG_MOVE_TO_A_WAIT_VFALL(sc, change) \
    do { \
        OTG_START_TIMER((sc), OTG_TIMER_TYPE_A_WAIT_VFALL); \
        OTG_CHANGE_STATE((sc), handle_a_wait_vfall_state, (change)); \
    } while(0)

/* State Handlers */
/* Handlers for A Device */
static jbool_t handle_a_idle_state(jotg_core_t *jotg_core);
static jbool_t handle_a_wait_vrise_state(jotg_core_t *jotg_core);
static jbool_t handle_a_wait_bcon_state(jotg_core_t *jotg_core);
static jbool_t handle_a_host_state(jotg_core_t *jotg_core);
static jbool_t handle_a_suspend_state(jotg_core_t *jotg_core);
static jbool_t handle_a_peripheral_state(jotg_core_t *jotg_core);
static jbool_t handle_a_wait_vfall_state(jotg_core_t *jotg_core);
/* HSU addition - remove unused function to avoid compilation warning */
/* static jbool_t handle_a_vbus_err_state(jotg_core_t *jotg_core); */
/* HSU addition */
/* Handlers for B Device */
static jbool_t handle_b_idle_state(jotg_core_t *jotg_core);
static jbool_t handle_b_srp_init_state(jotg_core_t *jotg_core);
static jbool_t handle_b_peripheral_state(jotg_core_t *jotg_core);
static jbool_t handle_b_wait_acon_state(jotg_core_t *jotg_core);
static jbool_t handle_b_host_state(jotg_core_t *jotg_core);

/* Function for running the state machine changes */
static void handle_state_machine_change(jotg_core_t *jotg_core);
static void reset_state_machine_software_params(jotg_core_t *jotg_core);
static void update_hw_smp(jotg_core_t *jotg_core);

/* General Functions */
static jresult_t is_peripheral_supported(jotg_core_t *jotg_core, 
    periph_support_t *periph_sup);

/* Core handles */
static jotg_core_t *cores;
static juint8_t ncores;

/* OTG State Machine handling functions */
/**
 * Function name:  handle_a_idle_state
 * Description: Handle the OTG state machine A_IDLE State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_idle_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_idle_state");
    /* HSU addition */
#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC
    int  loop_count = 0;
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)jotg_core->ocd_ctx;
#endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */

    DBG_V(DJOTG_CORE, ("%s: Handling A_IDLE State\n", fname));

    jotg_core->ocd_ops->ocd_enable_srp_detection(jotg_core->ocd_ctx, 1);
    jotg_core->is_idle = 1;

    if (jotg_core->state_machine.hw_smp.id)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_IDLE State\n", fname));        
        jotg_core->app_cbs.notify_device_connect_status(jotg_core->app_ctx,
            DEVICE_STATUS_DISCONNECTED);
        jotg_core->ocd_ops->ocd_enable_srp_detection(jotg_core->ocd_ctx, 0);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
        OTG_CHANGE_STATE(jotg_core, handle_b_idle_state, state_changed);
        goto Exit;
    }
   
    if (jotg_core->state_machine.a_bus_drop && 
        jotg_core->state_machine.hw_smp.a_srp_det)
    {
        jbool_t connect = jotg_core->app_cbs.respond_srp(jotg_core->app_ctx);
        if (connect)        
            jotg_core->state_machine.a_bus_drop = 0;
    }

    if (!jotg_core->state_machine.a_bus_drop && 
        (jotg_core->state_machine.bus_req || 
        jotg_core->state_machine.hw_smp.a_srp_det))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_VRISE State\n", fname));

        /* HSU addition */
        #ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC

        /* Due to limitation, ULPI viewport access to PHY is denied during 
         * charger is connected which results in SE1 (D+/D- both high) 
         * Line State.  When USB charger is connected, do not access the
         * PHY via ULPI viewport.
         */
        if ((TDI_4X_OTG_READ_REG(sc, TDI_4X_PORT_SC_REG) & TDI_4X_PORT_SC_LS) != TDI_4X_PORT_SC_LS_SE1)
        {       
            /* Set the Vbus valid rise and Vbus valid fall interrupts 
            * in the PHY, using the ULPI viewport.
            * On device mode we reset these bits for the PHY stuck workaround.
            * On host mode we need to set them since these bits are required for
            * host connection.
            */
            TDI_4X_OTG_WRITE_REG(sc, TDI_4X_ULPI_VIEWPORT_REG, 
              TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK
              | TDI_4X_ULPI_VIEWPORT_ULPIRW_BMSK
              | (0xe << TDI_4X_ULPI_VIEWPORT_ULPIADDR_SHFT)
              | (0x2 << TDI_4X_ULPI_VIEWPORT_ULPIDATWR_SHFT)
              );
            while (TDI_4X_OTG_READ_REG(sc, TDI_4X_ULPI_VIEWPORT_REG) & TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK)
            {
              HSU_ASSERT(loop_count < 10000);
              loop_count++;
            }

            loop_count = 0;
            TDI_4X_OTG_WRITE_REG(sc, TDI_4X_ULPI_VIEWPORT_REG, 
              TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK
              | TDI_4X_ULPI_VIEWPORT_ULPIRW_BMSK
              | (0x11 << TDI_4X_ULPI_VIEWPORT_ULPIADDR_SHFT)
              | (0x2 << TDI_4X_ULPI_VIEWPORT_ULPIDATWR_SHFT)
              );
            while (TDI_4X_OTG_READ_REG(sc, TDI_4X_ULPI_VIEWPORT_REG) & TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK)
            {
              HSU_ASSERT(loop_count < 10000);
              loop_count++;
            }
        }
        #endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */

        /* If session started by SRP mark it for later */
        if (jotg_core->state_machine.hw_smp.a_srp_det)
            jotg_core->state_machine.a_pend_hnp_en = 1;
        jotg_core->ocd_ops->ocd_enable_srp_detection(jotg_core->ocd_ctx, 0);
        jotg_core->ocd_ops->ocd_drive_vbus(jotg_core->ocd_ctx, 1);
        OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_WAIT_VRISE); 
        OTG_CHANGE_STATE(jotg_core, handle_a_wait_vrise_state, state_changed);
    }

Exit:
    return state_changed;
}

/**
 * Function name: handle_a_wait_vrise_state 
 * Description: Handle the OTG state machine A_WAIT_VRISE State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_wait_vrise_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_wait_vrise_state");

    DBG_V(DJOTG_CORE, ("%s: Handling A_WAIT_VRISE State\n", fname));
    
    jotg_core->is_idle = 0;
    
    if (jotg_core->state_machine.hw_smp.id ||
        jotg_core->state_machine.a_bus_drop || 
        jotg_core->state_machine.hw_smp.a_vbus_vld ||
        OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_WAIT_VRISE))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_BCON State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        if (jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON] != 
            DROP_VBUS_TIMEOUT_INFINITE)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_WAIT_BCON); 
        }
        OTG_CHANGE_STATE(jotg_core, handle_a_wait_bcon_state, state_changed);
    }

    return state_changed;
}

/**
 * Function name: handle_a_wait_bcon_state 
 * Description: Handle the OTG state machine A_WAIT_BCON State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_wait_bcon_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_wait_bcon_state");

    DBG_V(DJOTG_CORE, ("%s: Handling A_WAIT_BCON State\n", fname));
    
    jotg_core->is_idle = 1;
    
    if (jotg_core->state_machine.hw_smp.id ||
        jotg_core->state_machine.a_bus_drop || 
        OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_WAIT_BCON))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_VFALL State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->state_machine.bus_req = 0;
        jotg_core->ocd_ops->ocd_drive_vbus(jotg_core->ocd_ctx, 0);
        OTG_MOVE_TO_A_WAIT_VFALL(jotg_core, state_changed);
    }
	/*HSU ADDITION: Cancel V_BUS_ERR state */
 /*   else if (!jotg_core->state_machine.hw_smp.a_vbus_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_VBUS_ERR State\n", fname));
        jotg_core->app_cbs.notify_error(jotg_core->app_ctx, OTG_ERROR_VBUS);
        OTG_CHANGE_STATE(jotg_core, handle_a_vbus_err_state, state_changed);
    }*/
    else if (jotg_core->state_machine.b_hw_conn)
    {
        if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_BCON_DEBOUNCE))
        {
            DBG_X(DJOTG_CORE, ("%s: Switching to A_HOST State\n", fname));
            OTG_STOP_TIMER(jotg_core);
            jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
                DRV_IOCTL_HOST_PORT_ENABLE, (juint32_t *)HOST_PORT_ENABLE_RESET);
            /* Notify the Application about its device state */
            jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
                OTG_DEVICE_MODE_HOST);
            OTG_CHANGE_STATE(jotg_core, handle_a_host_state, state_changed);
        }
        else if (jotg_core->state_machine.timer_type != 
            OTG_TIMER_TYPE_A_BCON_DEBOUNCE)
        {
            /* TODO stop timer if b_conn==0 during ioctl */
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_BCON_DEBOUNCE);
        }
    }
    else if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_BCON_DEBOUNCE))
    {
        if (jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON] != 
            DROP_VBUS_TIMEOUT_INFINITE)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_WAIT_BCON); 
        }
    }

    return state_changed;
}

/**
 * Function name: handle_a_host_state 
 * Description: Handle the OTG state machine A_HOST State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_host_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_host_state");

    DBG_V(DJOTG_CORE, ("%s: Handling A_HOST State\n", fname));

    jotg_core->is_idle = 0;
    
    if (jotg_core->state_machine.hw_smp.id ||
        !jotg_core->state_machine.b_hw_conn ||
        jotg_core->state_machine.a_bus_drop)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_BCON State\n", fname));
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);         
        if (jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON] != 
            DROP_VBUS_TIMEOUT_INFINITE)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_WAIT_BCON);
        }
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
        OTG_CHANGE_STATE(jotg_core, handle_a_wait_bcon_state, state_changed);
    }
    /*HSU ADDITION: Cancel V_BUS_ERR state 
    else if (!jotg_core->state_machine.hw_smp.a_vbus_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_VBUS_ERR State\n", fname));
        jotg_core->app_cbs.notify_error(jotg_core->app_ctx, OTG_ERROR_VBUS);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);         
        Notify the Application about its device state 
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
        OTG_CHANGE_STATE(jotg_core, handle_a_vbus_err_state, state_changed);
    }*/
    else if (jotg_core->state_machine.b_sw_conn &&(
        !jotg_core->state_machine.bus_req || 
        jotg_core->state_machine.a_suspend_req))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_SUSPEND State\n", fname));
        /* If sessions started by an SRP, HNP must be enabled */
        if (jotg_core->state_machine.a_pend_hnp_en)
        {
            if (!jotg_core->state_machine.a_set_b_hnp_en)
            {
                if (!jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
                    DRV_IOCTL_HOST_ENABLE_HNP, NULL))
                {
                    jotg_core->state_machine.a_set_b_hnp_en = 1;
                }
            }
            jotg_core->state_machine.a_pend_hnp_en = 0;
        }
        /* Start the timer only if not INFINITE */
        if (jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_AIDL_BDIS] != 
            DROP_VBUS_TIMEOUT_INFINITE)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_AIDL_BDIS);
        }
        OTG_CHANGE_STATE(jotg_core, handle_a_suspend_state, state_changed);
    }

    return state_changed;
}

/**
 * Function name: handle_a_suspend_state 
 * Description: Handle the OTG state machine A_SUSPEND State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_suspend_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_suspend_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling A_SUSPEND State\n", fname));
    
    jotg_core->state_machine.a_suspend_req = 0;
    jotg_core->is_idle = 1;

    if (jotg_core->state_machine.hw_smp.id ||
        jotg_core->state_machine.a_bus_drop ||
        OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_AIDL_BDIS))
    {
        OTG_STOP_TIMER(jotg_core);
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_VFALL State\n", fname));
        jotg_core->ocd_ops->ocd_drive_vbus(jotg_core->ocd_ctx, 0);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);         
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
        OTG_MOVE_TO_A_WAIT_VFALL(jotg_core, state_changed);
    }
    /*HSU ADDITION: Cancel V_BUS_ERR state
    else if (!jotg_core->state_machine.hw_smp.a_vbus_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_VBUS_ERR State\n", fname));
        jotg_core->app_cbs.notify_error(jotg_core->app_ctx, OTG_ERROR_VBUS);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);         
        Notify the Application about its device state 
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
        OTG_CHANGE_STATE(jotg_core, handle_a_vbus_err_state, state_changed);
    }*/
    else if (!jotg_core->state_machine.b_hw_conn &&
        jotg_core->state_machine.a_set_b_hnp_en)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_PERIPHERAL State\n", fname));
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_ENABLE, NULL);         
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_PERIPHERAL);
        OTG_CHANGE_STATE(jotg_core, handle_a_peripheral_state, state_changed);
    }
    else if (!jotg_core->state_machine.b_hw_conn &&
        !jotg_core->state_machine.a_set_b_hnp_en)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_BCON State\n", fname));
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);         
        if (jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON] != 
            DROP_VBUS_TIMEOUT_INFINITE)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_WAIT_BCON);
        }
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
        OTG_CHANGE_STATE(jotg_core, handle_a_wait_bcon_state, state_changed);
    }
    else if (jotg_core->state_machine.bus_req ||
        jotg_core->state_machine.b_bus_resume)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_HOST State\n", fname));
        jotg_core->state_machine.b_bus_resume = 0;
        OTG_CHANGE_STATE(jotg_core, handle_a_host_state, state_changed);
    }

    return state_changed;
}

/**
 * Function name: handle_a_peripheral_state 
 * Description: Handle the OTG state machine A_PERIPHERAL State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_peripheral_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_peripheral_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling A_PERIPHERAL State\n", fname));

    jotg_core->is_idle = 0;
    
    if (jotg_core->state_machine.hw_smp.id ||
        jotg_core->state_machine.a_bus_drop)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_VFALL State\n", fname));
        jotg_core->ocd_ops->ocd_drive_vbus(jotg_core->ocd_ctx, 0);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_DISABLE, NULL);         
        OTG_MOVE_TO_A_WAIT_VFALL(jotg_core, state_changed);
    }
	/*HSU ADDITION: Cancel V_BUS_ERR state */
/*    else if (!jotg_core->state_machine.hw_smp.a_vbus_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_VBUS_ERR State\n", fname));
        jotg_core->app_cbs.notify_error(jotg_core->app_ctx, OTG_ERROR_VBUS);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_DISABLE, NULL);                 
        OTG_CHANGE_STATE(jotg_core, handle_a_vbus_err_state, state_changed);
    }*/
    else if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_SUSPEND_DET) && 
        jotg_core->state_machine.b_bus_suspend)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_BCON State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->state_machine.b_bus_suspend = 0;
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_DISABLE, NULL);         
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 0);
        if (jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON] != 
            DROP_VBUS_TIMEOUT_INFINITE)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_WAIT_BCON);
        }
        OTG_CHANGE_STATE(jotg_core, handle_a_wait_bcon_state, state_changed);
    }
    else if (jotg_core->state_machine.b_bus_suspend)
        OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_A_SUSPEND_DET);

    if (state_changed)
    {
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
    }

    return state_changed;
}

/**
 * Function name: handle_a_wait_vfall_state 
 * Description: Handle the OTG state machine A_WAIT_VFALL State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_a_wait_vfall_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_wait_vfall_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling A_WAIT_VFALL State\n", fname));

    jotg_core->is_idle = 0;
    
    if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_A_WAIT_VFALL) ||
        jotg_core->state_machine.bus_req ||
        (!jotg_core->state_machine.hw_smp.a_sess_vld &&
        !jotg_core->state_machine.b_hw_conn))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_IDLE State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 0);
        reset_state_machine_software_params(jotg_core);
        update_hw_smp(jotg_core);
        OTG_CHANGE_STATE(jotg_core, handle_a_idle_state, state_changed);
    }

    return state_changed;
}

/**
 * Function name: handle_a_vbus_err_state 
 * Description: Handle the OTG state machine A_VBUS_ERR State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
/* HSU Addition, remove the implementation of this function to fix compilation warnings */
/*
static jbool_t handle_a_vbus_err_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_a_vbus_err_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling A_VBUS_ERR State\n", fname));

    jotg_core->is_idle = 0;
    
    if (jotg_core->state_machine.hw_smp.id ||
        jotg_core->state_machine.a_bus_drop ||
        jotg_core->state_machine.a_clr_err)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_WAIT_VFALL State\n", fname));
        OTG_MOVE_TO_A_WAIT_VFALL(jotg_core, state_changed);
    }

    return state_changed;
}
*/
/* HSU addition */

/**
 * Function name: handle_b_idle_state 
 * Description: Handle the OTG state machine B_IDLE State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_b_idle_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_b_idle_state");
    /* HSU addition */
#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC
    int  loop_count;
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)jotg_core->ocd_ctx;
#endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */
    
    DBG_V(DJOTG_CORE, ("%s: Handling B_IDLE State\n", fname));

    jotg_core->is_idle = 1;

    jotg_core->state_machine.b_hnp_en = 0;

    if (!jotg_core->state_machine.hw_smp.id)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to A_IDLE State\n", fname));
        reset_state_machine_software_params(jotg_core);
        jotg_core->app_cbs.notify_device_connect_status(jotg_core->app_ctx,
            DEVICE_STATUS_CONNECTED);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 0);
        OTG_CHANGE_STATE(jotg_core, handle_a_idle_state, state_changed);
    }
    else if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_B_BUS_REQ))
    {
        DBG_W(DJOTG_CORE, ("%s: Failed to get response on SRP\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->state_machine.b_host_pending = 0;
        jotg_core->state_machine.bus_req = 0;

        jotg_core->app_cbs.notify_error(jotg_core->app_ctx, 
            OTG_ERROR_SRP_FAILED); 
    }
    else if (jotg_core->state_machine.hw_smp.b_sess_end &&
        jotg_core->state_machine.hw_smp.b_se0_srp)
    {
        /* If the BUS is requested start SRP else inform the OTG Application
         * that it may requset SRP initiation */
        if (jotg_core->state_machine.bus_req)
        {
            DBG_X(DJOTG_CORE, ("%s: Switching to B_SRP_INIT State\n", fname));
            jotg_core->ocd_ops->ocd_initiate_srp(jotg_core->ocd_ctx);
            jotg_core->state_machine.bus_req = 0;        
            OTG_CHANGE_STATE(jotg_core, handle_b_srp_init_state, state_changed);
        }
        else                    
            jotg_core->app_cbs.notify_srp_state(jotg_core->app_ctx, 1);
    }
    else if (jotg_core->state_machine.hw_smp.b_sess_vld)
    {
	    /* HSU TEMP FIX */
	    /* call b_peripheral_state_transition_cb */
	    jotg_core->app_cbs.notify_b_peripheral_state_transition(jotg_core->app_ctx);
	    /* END OF HSU TEMP FIX */

        DBG_X(DJOTG_CORE, ("%s: Switching to B_PERIPHERAL State\n", fname));
        OTG_STOP_TIMER(jotg_core);

        /* HSU addition */
        #ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC

        /* Due to limitation, ULPI viewport access to PHY is denied during 
         * charger is connected which results in SE1 (D+/D- both high) 
         * Line State.  When USB charger is connected, do not access the
         * PHY via ULPI viewport.
         */
        if ((TDI_4X_OTG_READ_REG(sc, TDI_4X_PORT_SC_REG) & TDI_4X_PORT_SC_LS) != TDI_4X_PORT_SC_LS_SE1)
        {       
            /* Clear the Vbus valid rise and Vbus valid fall interrupts 
            * in the PHY, using the ULPI viewport, for the PHY stuck workaround.
            */
            loop_count = 0;
            TDI_4X_OTG_WRITE_REG(sc, TDI_4X_ULPI_VIEWPORT_REG, 
              TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK
              | TDI_4X_ULPI_VIEWPORT_ULPIRW_BMSK
              | (0xf << TDI_4X_ULPI_VIEWPORT_ULPIADDR_SHFT)
              | (0x2 << TDI_4X_ULPI_VIEWPORT_ULPIDATWR_SHFT)
              );
            while (TDI_4X_OTG_READ_REG(sc, TDI_4X_ULPI_VIEWPORT_REG) & TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK)
            {
              HSU_ASSERT(loop_count < 10000);
              loop_count++;
            }

            loop_count = 0;
            TDI_4X_OTG_WRITE_REG(sc, TDI_4X_ULPI_VIEWPORT_REG, 
              TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK
              | TDI_4X_ULPI_VIEWPORT_ULPIRW_BMSK
              | (0x12 << TDI_4X_ULPI_VIEWPORT_ULPIADDR_SHFT)
              | (0x2 << TDI_4X_ULPI_VIEWPORT_ULPIDATWR_SHFT)
              );
            while (TDI_4X_OTG_READ_REG(sc, TDI_4X_ULPI_VIEWPORT_REG) & TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK)
            {
              HSU_ASSERT(loop_count < 10000);
              loop_count++;
            }
        }
        #endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */

        /* Inform the OTG Application that it may not request for SRP 
         * initiation */
        jotg_core->app_cbs.notify_srp_state(jotg_core->app_ctx, 0);
        /* Enable Slave */
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_ENABLE, NULL);         
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_PERIPHERAL);
        OTG_CHANGE_STATE(jotg_core, handle_b_peripheral_state, state_changed);
    }

    return state_changed;
}

/**
 * Function name: handle_b_srp_init_state 
 * Description: Handle the OTG state machine B_SRP_INIT State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_b_srp_init_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_b_srp_init_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling B_SRP_INIT State\n", fname));

    jotg_core->is_idle = 0;
    
    if (!jotg_core->state_machine.hw_smp.id ||
        jotg_core->state_machine.b_srp_done)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_IDLE State\n", fname));        
        jotg_core->state_machine.b_srp_done = 0;
        jotg_core->state_machine.b_host_pending = 1;
        OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_B_BUS_REQ);
        OTG_CHANGE_STATE(jotg_core, handle_b_idle_state, state_changed);
    }
       
    return state_changed;        
}

/**
 * Function name: handle_b_peripheral_state 
 * Description: Handle the OTG state machine B_PERIPHERAL State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_b_peripheral_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_b_peripheral_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling B_PERIPHERAL State\n", fname));

    jotg_core->is_idle = 0;
    
    if (!jotg_core->state_machine.hw_smp.id ||
        !jotg_core->state_machine.hw_smp.b_sess_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_IDLE State\n", fname));
        /* Disable the JSlave before switching to b_idle state */
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_DISABLE, NULL);         
        OTG_CHANGE_STATE(jotg_core, handle_b_idle_state, state_changed);
    }
    else if ((jotg_core->state_machine.bus_req || 
        jotg_core->state_machine.b_host_pending) &&
        jotg_core->state_machine.b_hnp_en &&
        jotg_core->state_machine.a_bus_suspend)
    {
        if (jotg_core->state_machine.timer_type != OTG_TIMER_TYPE_B_AIDL_BDIS)
        {
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_B_AIDL_BDIS); 
        }        
        else if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_B_AIDL_BDIS))
        {
            DBG_X(DJOTG_CORE, ("%s: Switching to B_WAIT_ACON State\n", fname));
            OTG_STOP_TIMER(jotg_core);
            jotg_core->state_machine.b_hnp_en = 0;
            jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
                DRV_IOCTL_DEV_DISABLE, NULL);         
            jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 0);
            update_hw_smp(jotg_core);
            jotg_core->ocd_ops->ocd_set_auto_reset(jotg_core->ocd_ctx, 1);
            OTG_START_TIMER(jotg_core, OTG_TIMER_TYPE_B_ASE0_BRST);
            OTG_CHANGE_STATE(jotg_core, handle_b_wait_acon_state, 
                state_changed);
        }
    }
    else if (OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_B_AIDL_BDIS))
    {
        OTG_STOP_TIMER(jotg_core);
    }

    if (state_changed)
    {
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
    }

    return state_changed;
}

/**
 * Function name: handle_b_wait_acon_state 
 * Description: Handle the OTG state machine B_WAIT_ACON State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_b_wait_acon_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_b_wait_acon_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling B_WAIT_ACON State\n", fname));

    jotg_core->is_idle = 0;
    
    if (!jotg_core->state_machine.hw_smp.id ||
        !jotg_core->state_machine.hw_smp.b_sess_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_IDLE State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->state_machine.b_host_pending = 0;
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
        OTG_CHANGE_STATE(jotg_core, handle_b_idle_state, state_changed);
    }
    else if (jotg_core->state_machine.a_conn)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_HOST State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_ENABLE, (juint32_t *)HOST_PORT_ENABLE_NO_RESET);
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_HOST);
        OTG_CHANGE_STATE(jotg_core, handle_b_host_state, state_changed);
    }
    else if (jotg_core->state_machine.a_bus_resume ||
        OTG_TIMEOUT_OCCURRED(jotg_core, OTG_TIMER_TYPE_B_ASE0_BRST))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_PERIPHERAL State\n", fname));
        OTG_STOP_TIMER(jotg_core);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_ENABLE, NULL);         
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_PERIPHERAL);
        OTG_CHANGE_STATE(jotg_core, handle_b_peripheral_state, state_changed);
    }

    /* Unset auto reset when leaving state */
    if (state_changed)
        jotg_core->ocd_ops->ocd_set_auto_reset(jotg_core->ocd_ctx, 0);

    return state_changed;
}

/**
 * Function name: handle_b_host_state 
 * Description: Handle the OTG state machine B_HOST State
 * Parameters: 
 *     @jotg_core: OTG Core Context
 *
 * Return value: 1 if State changed 0 if not
 * Scope: local
 **/
static jbool_t handle_b_host_state(jotg_core_t *jotg_core)
{
    jbool_t state_changed = 0;
    DECLARE_FNAME("handle_b_host_state");
    
    DBG_V(DJOTG_CORE, ("%s: Handling B_HOST State\n", fname));

    jotg_core->is_idle = 0;
    
    if (!jotg_core->state_machine.hw_smp.id ||
        !jotg_core->state_machine.hw_smp.b_sess_vld)
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_IDLE State\n", fname));
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_NONE);
        OTG_CHANGE_STATE(jotg_core, handle_b_idle_state, state_changed);
    }
    else if (!jotg_core->state_machine.a_conn ||
        (!jotg_core->state_machine.bus_req &&
        !jotg_core->state_machine.b_host_pending))
    {
        DBG_X(DJOTG_CORE, ("%s: Switching to B_PERIPHERAL State\n", fname));
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_HOST_PORT_DISABLE, NULL);
        jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
            DRV_IOCTL_DEV_ENABLE, NULL);         
        /* Notify the Application about its device state */
        jotg_core->app_cbs.notify_device_mode(jotg_core->app_ctx, 
            OTG_DEVICE_MODE_PERIPHERAL);
        OTG_CHANGE_STATE(jotg_core, handle_b_peripheral_state, state_changed);
    }

    return state_changed;
}

static void update_hw_smp(jotg_core_t *jotg_core)
{
    DECLARE_FNAME("update_hw_smp");

    DBG_X(DJOTG_CORE, ("%s: Getting current hardware state.\n", fname));

    jotg_core->ocd_ops->ocd_get_state(jotg_core->ocd_ctx, 
        &jotg_core->state_machine.hw_smp);

    if (jotg_core->state_machine.hw_smp.id)
    {
        jotg_core->state_machine.a_conn = 
            jotg_core->state_machine.hw_smp.connect;
        jotg_core->state_machine.a_bus_resume = 
            jotg_core->state_machine.hw_smp.resume;
        if (jotg_core->state_machine.a_bus_resume)
            jotg_core->state_machine.a_bus_suspend = 0;
    }
    else
    {
        jotg_core->state_machine.b_hw_conn = 
            jotg_core->state_machine.hw_smp.connect;
        jotg_core->state_machine.b_bus_resume = 
            jotg_core->state_machine.hw_smp.resume;
        if (jotg_core->state_machine.b_bus_resume)
            jotg_core->state_machine.b_bus_suspend = 0;
    }
}

static void update_smp(jotg_core_t *jotg_core, smp_notif_t notif)
{
    DECLARE_FNAME("update_smp");

    switch(notif)
    {
    case SMP_NOTIF_HW:
        update_hw_smp(jotg_core);
        break;
    case SMP_NOTIF_DEV_SUSPEND:
        if (jotg_core->state_machine.hw_smp.id)
        {
            jotg_core->state_machine.a_bus_suspend = 1;
            jotg_core->state_machine.a_bus_resume  = 0;
        }
        else
        {
            jotg_core->state_machine.b_bus_suspend = 1;
            jotg_core->state_machine.b_bus_resume  = 0;
        }
        break;
    case SMP_NOTIF_DEV_RESUME:
        if (jotg_core->state_machine.hw_smp.id)
        {
            jotg_core->state_machine.a_bus_suspend = 0;
            jotg_core->state_machine.a_bus_resume  = 1;
        }
        else
        {
            jotg_core->state_machine.b_bus_suspend = 0;
            jotg_core->state_machine.b_bus_resume  = 1;
        }
        break;
    case SMP_NOTIF_HOST_SUSPEND:
        if (jotg_core->state_machine.hw_smp.id)
            jotg_core->state_machine.b_bus_suspend = 1;
        else
        {
            jotg_core->state_machine.a_suspend_req = 1;
            jotg_core->state_machine.bus_req = 0;
        }
        break;
    case SMP_NOTIF_HOST_RESUME:
        if (jotg_core->state_machine.hw_smp.id)
            jotg_core->state_machine.a_bus_resume = 1;
        else
        {
            jotg_core->state_machine.b_bus_resume = 1;
            jotg_core->state_machine.bus_req = 1;
        }
        break;
    case SMP_NOTIF_HOST_CONNECT:
        if (jotg_core->state_machine.hw_smp.id)
            jotg_core->state_machine.a_conn = 1;
        else
            jotg_core->state_machine.b_sw_conn = 1;
        break;
    case SMP_NOTIF_HOST_DISCONNECT:
        if (jotg_core->state_machine.hw_smp.id)
            jotg_core->state_machine.a_conn = 0;
        else
            jotg_core->state_machine.b_sw_conn = 0;
        break;
    case SMP_NOTIF_START_SRP:
        jotg_core->state_machine.bus_req = 1;
        break;
    case SMP_NOTIF_VBUS_SHUTDOWN:
        jotg_core->state_machine.a_bus_drop = 1;
        jotg_core->state_machine.bus_req = 0;
        break;
    case SMP_NOTIF_VBUS_POWERUP:
        jotg_core->state_machine.a_bus_drop = 0;
        jotg_core->state_machine.bus_req = 1;
        break;
    case SMP_NOTIF_START_HNP:
        jotg_core->state_machine.bus_req = 1;
        break;
    default:
        DBG_E(DJOTG_CORE, ("%s: Got unknown notification - %d\n", fname, 
            notif));
        break;
    }
}

/**
 * Function name:  handle_state_machine_change
 * Description:    Handle a change in State Machine Parameters and run the state
 *                 machine as needed
 * Parameters:
 *     @jotg_core: OTG Core Context
 *
 * Return value:   None
 * Scope:          Local
 **/
static void handle_state_machine_change(jotg_core_t *jotg_core)
{
    jbool_t state_changed;
    otg_smp_notif_t *cur_notif;
    jbus_h bus = jotg_core->ocd_ops->ocd_get_bus(jotg_core->ocd_ctx);
    DECLARE_FNAME("handle_state_machine_change");

    jmutex_lock(jotg_core->state_machine_mtx);
    cur_notif = TAILQ_FIRST(&jotg_core->notif_queue);
    jmutex_unlock(jotg_core->state_machine_mtx);

    if (cur_notif && jotg_core->is_idle)
    {
        jnotify_os(NOTIFY_OTG_BUSY, bus); 
        jotg_core->is_idle = 0;
    }

    while (cur_notif)
    {
        update_smp(jotg_core, cur_notif->notif);
        do 
        {
            KASSERT(jotg_core->next_state_handler, ("%s: Next state handler is "
                "not assigned\n", fname));
            state_changed = jotg_core->next_state_handler(jotg_core);
        } while (state_changed);
        
        jmutex_lock(jotg_core->state_machine_mtx);
        TAILQ_REMOVE(&jotg_core->notif_queue, cur_notif, next_notif);
        jfree(cur_notif);
        cur_notif = TAILQ_FIRST(&jotg_core->notif_queue);
        jmutex_unlock(jotg_core->state_machine_mtx);
    }
    
    if (jotg_core->is_idle)
        jnotify_os(NOTIFY_OTG_IDLE, bus); 
}

/**
 * Function name:  reset_state_machine_software_params 
 * Description:   Reset all state machine software parameters, should be used
 *                before switching to A_IDLE state 
 * Parameters:
 *     @jotg_core: OTG Core Context
 *
 * Return value: None  
 * Scope:          Local
 **/
static void reset_state_machine_software_params(jotg_core_t *jotg_core)    
{
    /* Stop currently running timer */
    OTG_STOP_TIMER(jotg_core);

    /* for A-device */
    jotg_core->state_machine.b_hw_conn = 0;
    jotg_core->state_machine.b_sw_conn = 0;
    jotg_core->state_machine.b_bus_suspend = 0;
    jotg_core->state_machine.b_bus_resume  = 0;
    jotg_core->state_machine.b_hnp_support = 1;
    jotg_core->state_machine.b_srp_support = 1;
    jotg_core->state_machine.a_set_b_hnp_en = 0;
    jotg_core->state_machine.a_suspend_req = 0;
    jotg_core->state_machine.a_clr_err = 0;
    jotg_core->state_machine.a_bus_drop = 0;
    jotg_core->state_machine.a_pend_hnp_en = 0;

    /* for B-device */
    jotg_core->state_machine.a_conn = 0;
    jotg_core->state_machine.a_bus_suspend = 0;
    jotg_core->state_machine.a_bus_resume = 0;
    jotg_core->state_machine.b_srp_done = 0;
    jotg_core->state_machine.b_hnp_en = 0;
    jotg_core->state_machine.a_hnp_support = 0;
    jotg_core->state_machine.a_alt_hnp_support = 0;
    jotg_core->state_machine.b_host_pending = 0;

    /* for both A-device and B-device */
    jotg_core->state_machine.bus_req = 0;

    jotg_core->state_machine.b_srp_pending = 0;
}

static jresult_t add_smp_notif(jotg_core_t *jotg_core, smp_notif_t notif)
{
    otg_smp_notif_t *new_notif;
    jbool_t run_state_machine = 0;
    DECLARE_FNAME("add_smp_notif");

    DBG_V(DJOTG_CORE, ("%s: Adding new State Machine parameters notification "
        "- %d\n", fname, notif));

    new_notif = (otg_smp_notif_t *)jmalloc(sizeof(otg_smp_notif_t), M_ZERO);
    if (!new_notif)
    {
        DBG_E(DJOTG_CORE, ("%s: Failed to allocate memory for new "
            "notification\n", fname));
        return JENOMEM;
    }
    new_notif->notif = notif;

    jmutex_lock(jotg_core->state_machine_mtx);

    if (!TAILQ_FIRST(&jotg_core->notif_queue))
    {
        DBG_X(DJOTG_CORE, ("%s: There are no new notifications waiting for "
            "processing\n", fname));
        run_state_machine = 1;
    }
    
    TAILQ_INSERT_TAIL(&jotg_core->notif_queue, new_notif, next_notif);
    
    jmutex_unlock(jotg_core->state_machine_mtx);

    if (run_state_machine)
        handle_state_machine_change(jotg_core);

    return 0;
}

/* OCD --> OTG Core APIs implementation */
/**
 * Function name:  register_ocd
 * Description: Register the OCD with the OTG Core be exchanging contexts and
 *              callbacks struct
 * Parameters:
 *     @ocd_ctx:  (IN)  OCD Context to be passed as argument when calling one of
 *                      the OCD callbacks 
 *     @ops:      (IN)  Function Pointers for all OCD callbacks 
 *     @ocd_dev:  (IN)  Bus Device of the OCD
 *     @core_ctx: (OUT) OTG Core context to be passed as argument when
 *                      calling one of the OTG Core APIs 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t register_ocd(ocd_ctx_t ocd_ctx, ocd_ops_t *ops, 
    otg_core_ctx_t *core_ctx, jbool_t *full_speed_only)
{
    jresult_t rc = 0;
    jotg_core_t *jotg_core = NULL;
    jint_t i;
    DECLARE_FNAME("register_ocd");

    for (i = 0; i < ncores; i++)
    {
        if (!cores[i].ocd_ctx)
        {
            jotg_core = &cores[i];
            break;
        }
    }

    if (!jotg_core)
    {      
        DBG_E(DJOTG_CORE, ("%s: OTG core not found\n", fname));
        return JEINVAL;
    }

    jotg_core->ocd_ops = ops;
    jotg_core->ocd_ctx = ocd_ctx;
    jmutex_init(&jotg_core->state_machine_mtx);
    TAILQ_INIT(&jotg_core->notif_queue);
    jotg_core->is_idle = 1;                     
    *full_speed_only = jotg_core->full_speed_only;

    reset_state_machine_software_params(jotg_core);
    jotg_core->ocd_ops->ocd_set_mode(jotg_core->ocd_ctx, 1);
    jotg_core->next_state_handler = handle_b_idle_state;
    OTG_BUILD_TIMER_TIME_ARRAY(jotg_core->otg_timer_times);
    
    rc = otg_app_init(jotg_core, &jotg_core->app_cbs, &jotg_core->app_info,
        &jotg_core->app_ctx);
    if (rc)
    {
        DBG_E(DJOTG_CORE, ("%s: Failed initializing the OTG Application %d\n",
            fname, rc));
        goto Error;
    }

    if (jotg_core->app_info.drop_vbus_timeout > 
        jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_AIDL_BDIS])
    {
        jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_AIDL_BDIS] = 
            jotg_core->app_info.drop_vbus_timeout;
    }
    
    if (jotg_core->app_info.drop_vbus_timeout > 
        jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON])
    {
        jotg_core->otg_timer_times[(jint_t)OTG_TIMER_TYPE_A_WAIT_BCON] = 
            jotg_core->app_info.drop_vbus_timeout;
    }

    *core_ctx = jotg_core;

    return 0;

Error:
    *core_ctx = NULL;
    return rc;
}

/**
 * Function name:  unregister_ocd
 * Description: Unregister previously registered OCD from the OTG Core 
 * Parameters:
 *     @core_ctx: (IN) OTG Core context 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t unregister_ocd(otg_core_ctx_t core_ctx)
{
    jotg_core_t *jotg_core = (jotg_core_t *)core_ctx;
    jresult_t rc = 0;

    if (!jotg_core)
        goto Exit;

    if (!jotg_core->state_machine.hw_smp.id && 
        jotg_core->state_machine.hw_smp.a_vbus_vld)
    {
        rc = otg_vbus_shutdown(jotg_core);
        if (rc)
        {            
            DBG_E(DJOTG_CORE, ("%s: Failed to shutdown the VBus - %d\n", rc));
            goto Clean;
        }
        
        /* Wait for tasks to finish */
        while(!jotg_core->state_machine.hw_smp.id && 
            jotg_core->state_machine.hw_smp.a_vbus_vld)
        {
            jdelay_ms(300);
        }
    }

Clean:

    if (jotg_core->app_cbs.otg_app_uninit)
        rc = jotg_core->app_cbs.otg_app_uninit(jotg_core->app_ctx);
    
    if (jotg_core->state_machine_mtx)
        jmutex_uninit(jotg_core->state_machine_mtx);
    
    /* HSU Addition (based on B65470). Now that core is static, 
     * we need to mark the white list as NULL */
    if (jotg_core->white_list)
    {
        jfree(jotg_core->white_list);
        jotg_core->white_list = NULL;
    }

    if (jotg_core->ocd_ctx)
        jotg_core->ocd_ctx = NULL;
    /* HSU Addition */

Exit:
    return rc;
}

/**
 * Function name:  is_peripheral_supported
 * Description:   Checks the OTG Host white list to see if the connected
 *                peripheral is supported
 * Parameters:
 *     @jotg_core: OTG Core Context 
 *     @periph_sup: IN and OUT fields of DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED
 *                  ioctl
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Local
 **/
static jresult_t is_peripheral_supported(jotg_core_t *jotg_core, 
    periph_support_t *periph_sup)
{
    jint_t i;
    jbool_t support = 0;
    device_details_t *dev_details  = &periph_sup->dev_details;
    DECLARE_FNAME("is_peripheral_supported");

    DBG_V(DJOTG_CORE, ("%s: Asking for peripheral support on device with "
        "VendorID = 0x%x, ProductID = 0x%x\n", fname, dev_details->vendor_id,
        dev_details->product_id));
#ifdef CONFIG_JOTG_OPT
    if (dev_details->product_id == 0x1234)
    {
        periph_sup->is_supported = 0;
        return 0;
    }
#endif

    if (!jotg_core->white_list_size)
        support = 1;

    for (i = 0; i < jotg_core->white_list_size && !support; i++)
    {
        peripheral_matcher_t *periph_match = &jotg_core->white_list[i];

        if ((periph_match->flags & PERIPH_MATCH_BY_VENDOR_ID) &&
            (dev_details->vendor_id != periph_match->vendor_id))
        {
            continue;
        }
        
        if ((periph_match->flags & PERIPH_MATCH_BY_PRODUCT_ID) &&
            (dev_details->product_id != periph_match->product_id))
        {
            continue;
        }
        
        if ((periph_match->flags & PERIPH_MATCH_BY_MIN_BCD_DEVICE) &&
            (dev_details->bcd_device < periph_match->min_bcd_device))
        {
            continue;
        }
        
        if ((periph_match->flags & PERIPH_MATCH_BY_MAX_BCD_DEVICE) &&
            (dev_details->bcd_device > periph_match->max_bcd_device))
        {
            continue;
        }

        if ((periph_match->flags & PERIPH_MATCH_BY_USB_CLASS) &&
            (dev_details->usb_class != periph_match->usb_class))
        {
            continue;
        }

        if ((periph_match->flags & PERIPH_MATCH_BY_USB_SUB_CLASS) &&
            (dev_details->usb_sub_class != periph_match->usb_sub_class))
        {
            continue;
        }

        if ((periph_match->flags & PERIPH_MATCH_BY_USB_PROTOCOL) &&
            (dev_details->usb_protocol != periph_match->usb_protocol))
        {
            continue;
        }

        support = 1;        
    }

    periph_sup->is_supported = support;

    return 0;
}

/**
 * Function name:  jotg_ioctl_invoked
 * Description: Handle the IOCTL which was invoked. The possible IOCTLs are
 *              described in the API declaration in "otg_core_host.h" file.
 * Parameters:
 *     @core_ctx: (IN) OTG Core Context 
 *     @ioctl: (IN) The IOCTL type 
 *     @args: (IN) Arguments for the IOCTL
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t jotg_ioctl_invoked(otg_core_ctx_t core_ctx, drv_ioctl_t ioctl, 
    void *args)
{
    jresult_t rc = 0;
    jotg_core_t *jotg_core = (jotg_core_t *)core_ctx;
    smp_notif_t notif = SMP_NOTIF_NONE;
    DECLARE_FNAME("jotg_ioctl_invoked");

    KASSERT(jotg_core, ("%s: Invalid Context passed to OTG Core\n", fname));

    DBG_V(DJOTG_CORE, ("%s: OTG Core received an IOCTL - %d\n", fname, ioctl));

    switch(ioctl)
    {
    case DRV_IOCTL_DEV_SUSPEND:
        notif = SMP_NOTIF_DEV_SUSPEND;
        break;
    case DRV_IOCTL_DEV_RESUME:
        notif = SMP_NOTIF_DEV_RESUME;
        break;
    case DRV_IOCTL_DEV_RESET:
        break;
    case DRV_IOCTL_DEV_HNP_ENBL:
        jotg_core->state_machine.b_hnp_en = 1;
        jotg_core->app_cbs.notify_hnp_state(jotg_core->app_ctx, 
            OTG_HNP_ENABLED);
        break;
    case DRV_IOCTL_DEV_HNP_ALT_SUPPORTED:
        jotg_core->state_machine.a_alt_hnp_support = 1;
        jotg_core->app_cbs.notify_hnp_state(jotg_core->app_ctx, 
            OTG_HNP_ALT_SUPPORTED);
        break;
    case DRV_IOCTL_HOST_SUSPEND:
        notif = SMP_NOTIF_HOST_SUSPEND;
        break;
    case DRV_IOCTL_HOST_RESUME:
        notif = SMP_NOTIF_HOST_RESUME;
        break;
    case DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED:
        rc = is_peripheral_supported(jotg_core, (periph_support_t *)args);
        break;
    case DRV_IOCTL_HOST_REMOTE_CONNECTED:
        jotg_core->app_cbs.notify_peripheral_state(jotg_core->app_ctx, 
            OTG_PERIPH_SUPPORTED);
        notif = SMP_NOTIF_HOST_CONNECT;
        break;
    case DRV_IOCTL_HOST_REMOTE_DISCONNECTED:
        notif = SMP_NOTIF_HOST_DISCONNECT;
        break;
    case DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED:
        jotg_core->app_cbs.notify_peripheral_state(jotg_core->app_ctx, 
            OTG_PERIPH_PARTIAL_SUPPORTED);
        notif = SMP_NOTIF_HOST_CONNECT;
        break;
    case DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED:
        jotg_core->app_cbs.notify_peripheral_state(jotg_core->app_ctx, 
            OTG_PERIPH_NOT_SUPPORTED);
        notif = SMP_NOTIF_HOST_CONNECT;
        break;
    case DRV_IOCTL_DEV_SET_PARAMS:
    case DRV_IOCTL_HOST_SET_PARAMS:
        jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, ioctl, args);
        break;
    default:
        rc = JENOTSUP;
        goto Exit;
    }

    if (notif != SMP_NOTIF_NONE)
        rc = add_smp_notif(jotg_core, notif);
        
Exit:
    return rc;
}

/**
 * Function name:  jotg_timer_done
 * Description: The OTG core uses the OTG controller's timer capabilities.
 *              Currently running timer has reached its timeout.
 *              This function returns immediately
 * Parameters:
 *     @core_ctx: (IN) OTG Core Context 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
void jotg_timer_done(otg_core_ctx_t core_ctx)
{
    jotg_core_t *jotg_core = (jotg_core_t *)core_ctx;
    DECLARE_FNAME("jotg_timer_done");

    KASSERT(jotg_core, ("%s: Invalid Context passed to OTG Core\n", fname));

    jotg_core->state_machine.timeout = 1;
    jotg_core->state_machine.timer_running = 0;
}

/**
 * Function name:  jotg_handle_intr
 * Description: Handle an OTG interrupt 
 * Parameters:
 *     @core_ctx: (IN) OTG Core context 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t jotg_handle_intr(otg_core_ctx_t core_ctx)
{
    DECLARE_FNAME("jotg_handle_intr");

    KASSERT(core_ctx, ("%s: Invalid Context passed to OTG Core\n", fname));

    return add_smp_notif((jotg_core_t *)core_ctx, SMP_NOTIF_HW);
}

/**
 * Function name:  jotg_srp_done
 * Description:   Notify the OTG Core that the OCD finished executing the SRP
 *                protocol.
 *                This function returns immediately 
 * Parameters:
 *     @core_ctx: (IN) OTG Core context 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t jotg_srp_done(otg_core_ctx_t core_ctx)
{
    jotg_core_t *jotg_core = (jotg_core_t *)core_ctx;

    jotg_core->state_machine.b_srp_done = 1;

    return 0;
}

/* OTG App --> OTG Core APIs implementation */
/**
 * Function name:  otg_set_white_list
 * Description: Set the hosts current whitelist 
 * Parameters:
 *     @ctx: The OTG Core context 
 *     @whitelist_size: The amount of matchers in the whitelist 
 *     @whitelist: Array of matchers to define the OTG Host whitelist
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_set_whitelist(otg_ctx_t ctx, juint32_t whitelist_size,
    peripheral_matcher_t *whitelist)
{
    jotg_core_t *jotg_core = (jotg_core_t *)ctx;
    juint32_t whitelist_mem_size = whitelist_size * sizeof(peripheral_matcher_t);
    DECLARE_FNAME("otg_set_white_list");

    KASSERT(jotg_core, ("%s: Invalid Context passed to OTG Core\n", fname));

    if (jotg_core->white_list)
        jfree(jotg_core->white_list);

    jotg_core->white_list = (peripheral_matcher_t *)jmalloc(whitelist_mem_size, 
        M_ZERO);
    if (!jotg_core->white_list)
        return JENOMEM;

    j_memcpy(jotg_core->white_list, whitelist, whitelist_mem_size);
    jotg_core->white_list_size = whitelist_size;

    return 0;
}

jresult_t otg_start_srp(otg_ctx_t core_ctx)
{
    return add_smp_notif((jotg_core_t *)core_ctx, SMP_NOTIF_START_SRP);
}

jresult_t otg_enable_hnp(otg_ctx_t core_ctx)
{
    jotg_core_t *jotg_core = (jotg_core_t *)core_ctx;
    jresult_t rc;
    
    rc = jotg_core->ocd_ops->ocd_invoke_ioctl(jotg_core->ocd_ctx, 
        DRV_IOCTL_HOST_ENABLE_HNP, NULL);
    if (!rc)
        jotg_core->state_machine.a_set_b_hnp_en = 1;

    return rc;
}

jresult_t otg_vbus_shutdown(otg_ctx_t core_ctx)
{
    return add_smp_notif((jotg_core_t *)core_ctx, SMP_NOTIF_VBUS_SHUTDOWN);
}

jresult_t otg_vbus_powerup(otg_ctx_t core_ctx)
{
    return add_smp_notif((jotg_core_t *)core_ctx, SMP_NOTIF_VBUS_POWERUP);
}

jresult_t otg_start_hnp(otg_ctx_t core_ctx)
{
    return add_smp_notif((jotg_core_t *)core_ctx, SMP_NOTIF_START_HNP);
}

/* OTG Initialization and Uninitialization Functions */
void jotg_uninit(void)
{
    DBG_I(DJOTG_CORE, ("Starting core un-init...\n"));

    if (cores)
    {
        jfree(cores);
        cores = NULL;
        ncores = 0;
    }
}

jresult_t jotg_init(uw_args_t *args)
{
    jint_t i;

    DECLARE_FNAME("jotg_init");

    DBG_I(DJOTG_CORE, ("%s: Starting JOTG Core init...\n", fname));

    ncores =  args->num_of_cores;
    cores = (jotg_core_t *)jmalloc(sizeof(jotg_core_t) * ncores, M_ZERO);
    if (!cores)
    {
        DBG_E(DJOTG_CORE, ("%s: Failed to allocate cores list\n", fname));
        return JENOMEM;
    }

    for (i = 0; i < ncores; i++)
    {
#ifdef CONFIG_FULL_SPEED_ONLY
        cores[i].full_speed_only = 1;
#else
      /* HSU temp fix */
      if (args->hw_core_params[i].init_opt != UW_INIT_HOST)
      {
      /* HSU temp fix - end */
        cores[i].full_speed_only = 
            args->hw_core_params[i].device_param->full_speed_only;
      /* HSU temp fix */
      }
      else
      {
        cores[i].full_speed_only = 0;
      }
      /* HSU temp fix - end */
#endif
    }

    DBG_V(DJOTG_CORE, ("%s: JOTG Core initialized\n", fname));

    return 0;
}
