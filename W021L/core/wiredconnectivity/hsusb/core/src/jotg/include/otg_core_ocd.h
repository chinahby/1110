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
#ifndef __OTG_CORE_OCD_H__
#define __OTG_CORE_OCD_H__

/* OTG Core Context */
typedef void *otg_core_ctx_t;
/* OCD Context */
typedef void *ocd_ctx_t;

/* State Machine Hardware Parameters */
typedef struct {
    /* OTG related flags */
    juint_t     id               :1;     /* Device type:
                                           0 = A-device; 1 = B-device */
    /* For A-device */
    juint_t     a_sess_vld       :1;     /* 1 = valid session */
    juint_t     a_srp_det        :1;     /* 1 = SRP session detected */
    juint_t     a_vbus_vld       :1;     /* 1 = valid VBus */
    /* For B-device */
    juint_t     b_se0_srp        :1;     /* 1 = B-device detected SE0 state for
                                               a time period of at least
                                               TBSE0_SRP */
    juint_t     b_sess_end       :1;     /* 1 = session ended */
    juint_t     b_sess_vld       :1;     /* 1 = valid session */

    /* Port related flags */
    /* Host related flags */
    juint_t     connect          :1;     /* 1 = Device connected */
    /* Peripheral related flags */
    juint_t     resume           :1;     /* 1 = Resume was detected */
} otg_hw_smp_t;


/* ******************************************** *
 *      OCD --> OTG Core Functions
 * ******************************************** */

/* OCD Callbacks Structure */
typedef struct {
    /* ******************************
     *   A-device related callbacks *
     * ******************************/
    /**
     * Function name:  ocd_enable_srp_detection
     * Description:    Enables/disables the OTG controller's ability to detect
     *                 SRP from the connected peripheral. 
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD Context, as received by register_ocd()
     *     @enable:  (IN) TRUE  = Enable SRP detection
     *                    FALSE = Disable SRP detection
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_enable_srp_detection)(ocd_ctx_t ocd_ctx, jbool_t enable);

    /**
     * Function name:  ocd_drive_vbus
     * Description:    Drives/shuts down the VBus.
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @drive:   (IN) TRUE  = Drive the VBus;
     *                    FALSE = Shut down the VBus
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_drive_vbus)(ocd_ctx_t ocd_ctx, jbool_t drive);

    /* ******************************
     *   B-device related callbacks *
     * ******************************/
    /**
     * Function name:  ocd_charge_vbus
     * Description:    Charges/discharges the VBus.
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @charge:  (IN) TRUE  = Charge the VBus
     *                    FALSE = Discharge the VBus
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_charge_vbus)(ocd_ctx_t ocd_ctx, jbool_t charge);

    /**
     * Function name:  ocd_initiate_srp 
     * Description:    Instruct the OCD to start SRP 
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_initiate_srp)(ocd_ctx_t);

    /* *********************************************
     *   Callbacks related to both A and B devices *
     * *********************************************/
    /**
     * Function name:  ocd_start_timer
     * Description:    Starts a new OTG controller timer.
     *                 (The OTG core uses the OTG controller's timer
     *                  capabilities).
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @ms:      (IN) Timeout period for the timer, in milliseconds
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_start_timer)(ocd_ctx_t ocd_ctx, juint32_t ms);
    
    /**
     * Function name:  ocd_stop_timer
     * Description:    Stops the currently running OTG controller timer.
     *                 (The OTG core uses the OTG controller's timer
     *                  capabilities).
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_stop_timer)(ocd_ctx_t ocd_ctx);

    /**
     * Function name:  ocd_get_state
     * Description:    Gets the state of OTG controller's hardware registers.
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN)  OCD context, as received by register_ocd()
     *     @hw_smp:  (OUT) Pointer to a pre-allocated OTG state machine hardware
     *                     parameters structure to to be filled by the OCD
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_get_state)(ocd_ctx_t ocd_ctx, otg_hw_smp_t *hw_smp);

    /**
     * Function name:  ocd_set_local_dplus
     * Description:    Enables/disables the local pulldown register on the
     *                 D+ line.
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @enable:  (IN) TRUE  = Enable the local D+ pulldown register
     *                    FALSE = Disable the local D+ pulldown register
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_set_local_dplus)(ocd_ctx_t ocd_ctx, jbool_t enable);
    
    /**
     * Function name:  ocd_set_local_dminus
     * Description:    Enables/disables the local pulldown register on the
     *                 D- line.
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @enable:  (IN) TRUE  = Enable the local D- pulldown register
     *                    FALSE = Disable the local D- pulldown register
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_set_local_dminus)(ocd_ctx_t ocd_ctx, jbool_t enable);

    /**
     * Function name:  ocd_invoke_ioctl
     * Description:    Invokes an IOCTL sent from the OTG Core to the
     *                 host/device controller.
     *                 The OCD performs any required initial handling of the
     *                 IOCTL and then passes it to the device or host
     *                 controller, as needed.
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @ioctl:   (IN) OTG controller IOCTL code.
     *                    The possible IOCTLs are:
     *                    Device controller IOCTLs:
     *                    DRV_IOCTL_DEV_ENABLE
     *                    DRV_IOCTL_DEV_DISABLE
     *                    Host controller IOCTLs:
     *                    DRV_IOCTL_HOST_ENABLE
     *                    DRV_IOCTL_HOST_DISABLE
     *     @args:    (IN) Arguments for the IOCTL
     *
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*ocd_invoke_ioctl)(ocd_ctx_t ocd_ctx, drv_ioctl_t ioctl,
        void *args);

    /**
     * Function name:  ocd_set_mode 
     * Description:    Sets the controller's mode to either Host or Device 
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @device:  (IN) TRUE  = Set the Controller's mode to Device
     *                    FALSE = Set the Controller's mode to Host
     *
     * Return value:   None 
     * Scope:          Static
     **/
    void (*ocd_set_mode)(ocd_ctx_t ocd_ctx, jbool_t device);

    /**
     * Function name:  ocd_set_auto_reset 
     * Description:    Instruct the Controller to reset the port on device
     *                 connect. Used when transitioning in and from b_wait_acon
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *     @set:     (IN) TRUE  = Set the Auto-Reset 
     *                    FALSE = Unset the Auto-Reset 
     *
     * Return value:   None 
     * Scope:          Static
     **/
    void (*ocd_set_auto_reset)(ocd_ctx_t ocd_ctx, jbool_t set);

    /**
     * Function name:  ocd_get_bus 
     * Description:    Get the OCD's BUS handle for jnotify_os 
     * Requirement:    Mandatory
     * Parameters: 
     *     @ocd_ctx: (IN) OCD context, as received by register_ocd()
     *
     * Return value:   None 
     * Scope:          Static
     **/
    jbus_h (*ocd_get_bus)(ocd_ctx_t ocd_ctx);
} ocd_ops_t;


/* ******************************************** *
 *      OTG Core --> OCD Functions
 * ******************************************** */
/**
 * Function name:  register_ocd
 * Description:    Registers the OCD with the OTG Core.
 *                 The function receives and stores a list of the OCD callbacks
 *                 and the OCD context to be passed to these callbacks, and
 *                 returns to the OCD the OTG Core's context to be passed to the
 *                 other OTG Core functions.
 * Parameters: 
 *     @ocd_ctx:        (IN)  OCD context to be be passed as an argument to 
 *                            the OCD callbacks 
 *     @ops:            (IN)  Pointer to a structure containing the OCD 
 *                            callbacks 
 *     @core_ctx:       (OUT) Pointer to the OTG Core context to be passed as 
 *                            an argument calls to the other OTG Core functions
 *     @full_speed_only (OUT) OTG core fills this value according to the 
 *                            argument received from uw_args
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t register_ocd(ocd_ctx_t ocd_ctx, ocd_ops_t *ops, 
    otg_core_ctx_t *core_ctx, jbool_t *full_speed_only);

/**
 * Function name:  unregister_ocd
 * Description:    Unregisters a previously registered OCD from the OTG Core.
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received from register_ocd()
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t unregister_ocd(otg_core_ctx_t core_ctx);

/**
 * Function name:  jotg_timer_done
 * Description:    Informs the OTG core that the timeout period of the currently
 *                 running OTG controller timer has expired.
 *                 (The OTG core uses the OTG controller's timer capabilities).
 *                 
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received from register_ocd()
 *
 * Return value:   None
 * Scope:          Global
 **/
void jotg_timer_done(otg_core_ctx_t core_ctx);

/**
 * Function name:  jotg_ioctl_invoked
 * Description:    Handles an invoked IOCTL sent to the OTG controller.
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received from register_ocd()
 *     @ioctl:    (IN) OTG controller IOCTL code.
 *                     The possible IOCTLs are:
 *                     From the device (peripheral):
 *                     DRV_IOCTL_DEV_SUSPEND
 *                     DRV_IOCTL_DEV_RESUME
 *                     DRV_IOCTL_DEV_RESET
 *                     DRV_IOCTL_DEV_HNP_ENBL
 *                     DRV_IOCTL_DEV_HNP_ALT_SUPPORTED
 *                     From the host:
 *                     DRV_IOCTL_HOST_WAIT_FOR_HNP
 *                     DRV_IOCTL_HOST_SUSPEND
 *                     DRV_IOCTL_HOST_RESUME
 *                     DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED
 *                     DRV_IOCTL_HOST_REMOTE_CONNECTED
 *                     DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED
 *                     DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED
 *     @args:     (IN) Arguments for the IOCTL
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t jotg_ioctl_invoked(otg_core_ctx_t core_ctx, drv_ioctl_t ioctl, 
    void *args);

/**
 * Function name:  jotg_handle_intr
 * Description:    Handles an OTG interrupt.
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received from register_ocd()
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t jotg_handle_intr(otg_core_ctx_t core_ctx);

/**
 * Function name:  jotg_srp_done
 * Description:   Notify the OTG Core that the OCD finished executing the SRP
 *                protocol 
 * Parameters:
 *     @core_ctx: (IN) OTG Core context 
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t jotg_srp_done(otg_core_ctx_t core_ctx);

#endif
