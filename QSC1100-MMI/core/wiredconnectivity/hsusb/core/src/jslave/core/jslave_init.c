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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jslave_init.h>
#include <jusb_common.h>
#include <jusb_chp9.h>

/**
 * Function name:  jslave_init
 * Description: Initialize the device stack 
 * Parameters: 
 *     @args: Stack initialization arguments
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t jslave_init(uw_args_t *args)
{
    jresult_t rc = 0;

    DBG_I(DSLAVE_CORE, ("\n\nStarting JSLAVE init\n"));
 
    rc = core_init(args->hw_core_params, args->num_of_cores);
    if (!rc)
        DBG_I(DSLAVE_CORE, ("Finished JSLAVE init (%d)\n", rc));

    return rc;
}

/**
 * Function name:  jslave_uninit
 * Description: Un-initializae the device stack
 * Parameters: 
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
void jslave_uninit(void)
{
    core_uninit();
}
 
/**
 * Function name:  jslave_enable
 * Description: Enable the device stack for a specific controller
 * Parameters: 
 *     @core_number: (IN) Number of the core to enable
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t jslave_enable(juint8_t core_number)
{
    return core_enable_by_idx(core_get_controller_idx(core_number));
}

/**
 * Function name:  jslave_disable
 * Description: Disable the device stack for a specific controller
 * Parameters: 
 *     @core_number: (IN) Number of the core to enable
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
void jslave_disable(juint8_t core_number)
{
    core_disable_by_idx(core_get_controller_idx(core_number));
}

/**
 * Function name:  jslave_reconfig
 * Description: Change function driver configuration for a specific controller.
 *              Disable and uninitialize all running function driver, and then
 *              initialize 
 * Parameters: 
 *     @core_number(IN) Number of the reconfigured core
 *     @new_config (IN) Struct holding the controller's new configurations
 *     @cb         (IN) completion callback for the reconfig task
 *     @arg        (IN) Argument for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: global
 **/
jresult_t jslave_reconfig(juint8_t core_number, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg)
{
    return core_reconfig(core_get_controller_idx(core_number), new_config, cb, 
        arg);
}

/**
* Function name:  jcore_get_curr_configuration
* Description: Returns the configuration number that the device was configured 
* Parameters: 
*     @core_number(IN) Number of the reconfigured core
*
* Return value: Current configuration number
* Scope: global
**/
juint8_t jcore_get_curr_configuration(juint8_t core_number)
{
   return core_get_curr_configuration(core_number);
}
