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
#include <controllers.h>

#include <transceiver_tcd.h>
#ifdef CONFIG_JSLAVE
#include <transceiver_device.h>
#endif
#ifdef CONFIG_JHOST
#include <jusb.h>
#endif

static jresult_t tcd_name_probe(jdevice_t dev)
{
    /* A TCD's probe driver method should always return 0 */

    return 0;
}

static jresult_t tcd_name_detach(jdevice_t dev)
{
    /* Release bus resources */

    return 0;
}

static jresult_t tcd_name_attach(jdevice_t dev)
{
    /* Allocate the needed bus resources */

    /* If attach failed, call detach */

    return 0;
}

static jresult_t tcd_name_suspend(jdevice_t dev)
{
    /* A TCD's suspend driver method should always return 0 */

    return 0;
}

static jresult_t tcd_name_resume(jdevice_t dev)
{
    /* A TCD's resume driver method should always return 0 */

    return 0;
}

/**
 * Function name:  tcd_intr
 * Description:    The TCD's H/W interrupt handler.
 * Parameters:
 *     @arg (IN):  TCD software context.
 *
 * Return value:   Instruct JOS whether to schedule a S/W interrupt handler.
 * Scope:          Local - This function is usually defined as static - 
 *                         removed for compilation.
 **/
/*static*/ jint_t tcd_intr(void *arg)
{    
    /**
     * Since read/write operations are done via the I2C, they must be done in
     * the Software ISR 
     **/
    return ISR_CALL_SOFT_INTR;
}

/**
 * Function name:  tcd_dsr
 * Description:    The TCD's S/W interrupt handler.
 * Parameters:
 *     @arg (IN):  TCD software context.
 *
 * Return value:   None
 * Scope:          Local - This function is usually defined as static - 
 *                         removed for compilation.
 **/
/*static*/ void tcd_dsr(void *arg)
{
    /**
     * Read the interrupt status register and acknowledge the interrupt
     * Inform the OCD on any interrupt by calling trcv_otg_intr_handle
     * Inform the HCD on any interrupt by calling trcv_host_intr_handle
     * Inform the DCD on any change the OTG status register (B_SESS_VLD or
     * B_SESS_END) by calling trcv_device_intr_handle 
     **/
}

#ifdef CONFIG_JOTG
/**
 * Function name:  tcd_srp_detection_enable
 * Description:    Enables/Disables SRP detection.
 * Parameters:
 *     @tcd_ctx (IN): TCD software context.
 *     @enable  (IN): TRUE = enable SRP detection;
 *                    False = disable SRP detection.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_srp_detection_enable(tcd_ctx_h tcd_ctx, jbool_t enable)
{
    /* SRP may be detected by monitoring DP status changing to High */
    return 0;
}

/**
 * Function name:  tcd_vbus_charge
 * Description:    Charges/Discharges the VBus.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @charge  (IN):   TRUE = charge the VBus;
 *                      FALSE = discharge the VBus.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_vbus_charge(tcd_ctx_h tcd_ctx, jbool_t charge)
{
    if (charge)
    {
        /**
         * Clear VBus Discharge.
         * Set VBus Charge.
         **/
    }
    else
    {
        /**
         * Clear VBus Charge.
         * Set VBus Discharge.
         **/
    }

    return 0;
}

/**
 * Function name:  tcd_state_get
 * Description:    Gets the transceiver's current H/W state.
 * Parameters:
 *     @tcd_ctx  (IN):  TCD software context.
 *     @otg_ctrl (OUT): Pointer to a data structure for containing the
 *                      transceiver's current H/W states.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_state_get(tcd_ctx_h tcd_ctx, otg_ctrl_t *otg_ctrl)
{
    /* Read the H/W state registers.
     * Set vbus_vld, sess_vld, b_sess_vld and b_sess_end according to
     * H/W state.
     * Set srp_detected according to the S/W state.
     * Set the ID according to the H/W state -
     *     When the H/W status ID is GRD - set the S/W ID to 0.
     *     When the H/W status ID is FLOAT - set the S/W ID to 1.
     **/
    return 0;
}

/**
 * Function name:  tcd_local_dplus_set
 * Description:    Enables/Disables the DP Pulldown.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @enable  (IN):   TRUE = enable DP Pulldown;
 *                      FALSE = disable DP Pulldown.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_local_dplus_set(tcd_ctx_h tcd_ctx, jbool_t enable)
{
    if (enable)
    {
        /* Enable the DP Pulldown */
    }
    else
    {
        /* Disable the DP Pulldown */
    }

    return 0;
}

/**
 * Function name:  tcd_local_dminus_set
 * Description:    Enables/Disables the DM Pulldown.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @enable  (OUT):  TRUE = enable DM Pulldown;
 *                      FALSE = disable DM Pulldown.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_local_dminus_set(tcd_ctx_h tcd_ctx, jbool_t enable)
{
    if (enable)
    {
        /* Enable the DM Pulldown */
    }
    else
    {
        /* Disable the DM Pulldown */
    }

    return 0;
}

/**
 * Function name:  tcd_vbus_drive
 * Description:    Drives/Shuts-down the VBus on the transceiver's port.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @enable  (IN):   TRUE = drive the VBus on the port;
 *                      FALSE = shutdown the VBus on the port.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_vbus_drive(tcd_ctx_h tcd_ctx, jbool_t drive)
{
    if (drive)
    {
        /* Drive VBus on the transceiver's port */
    }
    else
    {
        /* Shutdown VBus on the transceiver's port */
    }

    return 0;
}

/**
 * Function name:  tcd_srp_initiate
 * Description:    Initiates a full SRP sequence.
 * Parameters:
 *     @tcd_ctx (IN): TCD software context.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_srp_initiate(tcd_ctx_h tcd_ctx)
{
    /**
     * Enable the DP Pullup.
     * Wait for 5-10 ms.
     * Disable the DP Pullup.
     * Charge the VBus.
     * Wait 10-20 ms.
     * Discharge the VBus.
     **/

    return 0;
}
#endif

/**
 * Function name:  tcd_enable
 * Description:    Enables the transceiver.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @type    (IN):   Transceiver functionality to enable - OTG, Host or
 *                      Device (see the trcv_type_t enumeration).
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_enable(tcd_ctx_h tcd_ctx, trcv_type_t type)
{
    /**
     * Configure the type of connection between the transceiver and the USB
     * controller.
     * Register the ISR and Software ISR and enable the required interrupts.
     * In Host-Only mode there is no need for polling the ID pin.
     **/

    return 0;
}

/**
 * Function name:  tcd_disable
 * Description:    Disables the transceiver.
 * Description:    
 * Parameters:
 *     @tcd_ctx (IN): TCD software context.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void tcd_disable(tcd_ctx_h tcd_ctx)
{
    /**
     * Disable all H/W interrupts.
     * Tear down the registered interrupt handler.
     **/
}

/**
 * Function name:  tcd_mode_set
 * Description:    Sets the transceiver's mode of operation.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @mode    (IN):   The transceiver mode to set:
 *                      TRCV_MODE_HOST for Host;
 *                      TRCV_MODE_DEVICE for Device.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void tcd_mode_set(tcd_ctx_h tcd_ctx, jbool_t mode)
{
    /**
     * If needed, set the transceiver's mode of operation.
     * When setting to Device mode, check the device status and report to the
     * DCD if the status has changed.
     **/
}

#ifdef CONFIG_JHOST
/**
 * Function name:  tcd_host_port_set
 * Description:    Sets/Clears the specified transceiver port feature.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @value   (IN):   The port feature to set/clear.
 *     @set     (IN):   TRUE = set the port feature;
 *                      FALSE = clear the port feature.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_host_port_set(tcd_ctx_h tcd_ctx, jint_t value, jbool_t set)
{
    /* In most cases only PORT_POWER is handled by the TCD */

    if (value & UHF_PORT_POWER)
    {
        if (set)
        {
            /* Set Port Power */
        }
        else
        {
            /* Clear Port Power */
        }
    }

    return 0;
}

/**
 * Function name:  tcd_host_port_status_get
 * Description:    Gets the current transceiver port status.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @status  (OUT):  Pointer to a bit-mask of bits to set in the port status.
 *     @mask    (OUT):  Pointer to a bit-mask of bits to clear from port status.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_host_port_status_get(tcd_ctx_h tcd_ctx, juint16_t *status,
    juint16_t *mask)
{
    *status = 0;
    *mask = 0;
    return 0;
}

/**
 * Function name:  tcd_host_port_change_get
 * Description:    Gets the transceiver port's status change.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @change  (OUT):  Pointer to a bit-mask of bits to set in the port change
 *                      value.
 *     @mask    (OUT):  Pointer to a bit-mask of bits to clear from port change
 *                      value.
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t tcd_host_port_change_get(tcd_ctx_h tcd_ctx, juint16_t *change,
    juint16_t *mask)
{
    *change = 0;
    *mask = 0;
    return 0;
}
#endif

#ifdef CONFIG_JSLAVE
/**
 * Function name:  tcd_device_enable
 * Description:    Enables/Disables the work of the USB device by setting the DP
 *                 Pullup to the correct state.
 * Parameters:
 *     @tcd_ctx (IN):   TCD software context.
 *     @enable  (IN):   TRUE = enable the device;
 *                      FALSE = disable the device.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void tcd_device_enable(tcd_ctx_h tcd_ctx, jbool_t enable)
{
    if (enable)
    {
        /* Enable the DP Pullup */
    }
    else
    {
        /* Disable the DP Pullup */
    }
}
#endif

static tcd_ops_t ops =
{
#ifdef CONFIG_JOTG
    tcd_srp_detection_enable,
    tcd_vbus_charge,
    tcd_srp_initiate,
    tcd_state_get,
    tcd_local_dplus_set,
    tcd_local_dminus_set,
    tcd_vbus_drive,
#endif

#ifdef CONFIG_JHOST
    tcd_host_port_set,
    tcd_host_port_status_get,
    tcd_host_port_change_get,
#endif
    
#ifdef CONFIG_JSLAVE
    tcd_device_enable,
#endif

    tcd_enable,
    tcd_disable,
    tcd_mode_set
};

static jresult_t tcd_name_ioctl(jdevice_t dev, drv_ioctl_t ioctl, void *arg)
{
    switch (ioctl)
    {
    case DRV_IOCTL_TCD_SET_CTX:
        /**
         * Save the transceiver's context to be used in all calls to the
         * transceiver core's APIs 
         **/ 
        break;

    case DRV_IOCTL_TCD_GET_OPS:
        *(tcd_ops_t *)arg = ops;
        break;

    default:
        break;
    }
    
    return 0;
}

DRIVER_CONTROLLER_DECLARE(tcd_name, /* TCD's S/W context size */ 0)

