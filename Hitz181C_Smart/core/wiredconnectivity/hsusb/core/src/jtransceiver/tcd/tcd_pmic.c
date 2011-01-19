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

typedef struct {
    /* resources */
    jbus_resource_h reg_res;
    jbus_resource_h irq_res;
    void *intr_handle;
    
    trcv_ctx_h trcv_core;
} pmic_sc_t;

static jresult_t pmic_probe(jdevice_t dev)
{
    /* A TCD's probe driver method should always return 0 */

    return 0;
}

static jresult_t pmic_detach(jdevice_t dev)
{
    pmic_sc_t *sc = (pmic_sc_t *)j_device_get_softc(dev);
    jbus_h bus = j_device_get_bus(dev);
    DECLARE_FNAME("pmic_detach");

    DBG_I(DTRCV_TCD, ("%s started\n", fname));
    
    if (sc->reg_res)
        jbus_release_resource(bus, sc->reg_res);

    if (sc->irq_res)
        jbus_release_resource(bus, sc->irq_res);

    return 0;
}

static jresult_t pmic_attach(jdevice_t dev)
{
    pmic_sc_t *sc = (pmic_sc_t *)j_device_get_softc(dev);
    jbus_h bus = j_device_get_bus(dev);
    jresult_t rc = JEINVAL;
    DECLARE_FNAME("pmic_attach");

    DBG_I(DTRCV_TCD, ("%s started\n", fname));

    sc->irq_res = j_bus_alloc_resource(bus, SYS_RES_IRQ, 0);
    if (!sc->irq_res)
    {
        DBG_E(DTRCV_TCD, ("%s: Failed alloc irq resource\n", fname));
        goto Error;
    }

    sc->reg_res = j_bus_alloc_resource(bus, SYS_RES_MEMORY, 0);
    if (!sc->reg_res)
    {
        DBG_E(DTRCV_TCD, ("%s: Failed alloc memory resource\n", fname));
        goto Error;
    }

    return 0;

Error:
    pmic_detach(dev);
    return rc;
}

static jresult_t pmic_suspend(jdevice_t dev)
{
    /* A TCD's suspend driver method should always return 0 */

    return 0;
}

static jresult_t pmic_resume(jdevice_t dev)
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
    
static void tcd_interface_type_get(tcd_ctx_h tcd_ctx, 
    trcv_interface_type_t *type)
{
    /* Set the Transceiver Interace Type */
    *type = TRCV_INTRFACE_TYPE_DEFAULT;
}

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
    DBG_V(DTRCV_TCD, ("In TCD Device %sable\n", enable ? "en" : "dis"));

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
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif

#ifdef CONFIG_JHOST
    NULL,
    NULL,
    NULL,
#endif
    
#ifdef CONFIG_JSLAVE
    tcd_device_enable,
#endif

    tcd_enable,
    tcd_disable,
    NULL,
    tcd_interface_type_get
};

static jresult_t pmic_ioctl(jdevice_t dev, drv_ioctl_t ioctl, void *arg)
{
    pmic_sc_t *sc = (pmic_sc_t *)j_device_get_softc(dev);

    switch (ioctl)
    {
    case DRV_IOCTL_TCD_SET_CTX:
        sc->trcv_core = (trcv_ctx_h)arg;
        break;

    case DRV_IOCTL_TCD_GET_OPS:
        *(tcd_ops_t *)arg = ops;
        break;

    default:
        return JEINVAL;
    }
    
    return 0;
}

DRIVER_CONTROLLER_DECLARE(pmic, sizeof(pmic_sc_t))

