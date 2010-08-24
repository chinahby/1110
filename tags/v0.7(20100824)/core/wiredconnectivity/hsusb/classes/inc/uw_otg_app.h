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
#ifndef __OTG_APP_H__ 
#define __OTG_APP_H__ 

/* OTG Core Context */
typedef void *otg_ctx_t;
/* Application Context */
typedef void *otg_appctx_t;

typedef enum {
    OTG_ERROR_OK,
    OTG_ERROR_VBUS,
    OTG_ERROR_SRP_FAILED
} otg_error_t;

typedef enum {
    OTG_HNP_DISABLED,
    OTG_HNP_SUPPORTED,
    OTG_HNP_ALT_SUPPORTED,
    OTG_HNP_ENABLED
} otg_hnp_state_t;

typedef enum {
    OTG_PERIPH_SUPPORTED,
    OTG_PERIPH_PARTIAL_SUPPORTED,
    OTG_PERIPH_NOT_SUPPORTED
} otg_peripheral_state_t;

typedef struct {
#define PERIPH_MATCH_BY_VENDOR_ID       0x01
#define PERIPH_MATCH_BY_PRODUCT_ID      0x02
#define PERIPH_MATCH_BY_MIN_BCD_DEVICE  0x04
#define PERIPH_MATCH_BY_MAX_BCD_DEVICE  0x08
#define PERIPH_MATCH_BY_USB_CLASS       0x10
#define PERIPH_MATCH_BY_USB_SUB_CLASS   0x20
#define PERIPH_MATCH_BY_USB_PROTOCOL    0x40
    juint8_t flags;

    /* Peripheral Vendor/Product Matching */
    juint16_t vendor_id;
    juint16_t product_id;
    juint16_t min_bcd_device;
    juint16_t max_bcd_device;

    /* Peripheral Class/Protocol Type Matching */
    juint8_t usb_class;
    juint8_t usb_sub_class;
    juint8_t usb_protocol;
} peripheral_matcher_t;

typedef enum {
    OTG_DEVICE_MODE_NONE,
    OTG_DEVICE_MODE_HOST,
    OTG_DEVICE_MODE_PERIPHERAL
} otg_device_mode_t;

/* ******************************************** *
 *      Application --> OTG Core Functions      *
 * ******************************************** */

/* OTG Application Callback Functions Structure */
typedef struct {
    /**
     * Function name:  notify_peripheral_state 
     * Description:    Informs the application of the host's degree of
     *                 support for the connected peripheral.
     * Requirement:    Mandatory
     * Parameters: 
     *     @appctx: (IN) Application context, as received from otg_app_init()
     *     @state:  (IN) The state of the connected peripheral (in terms of the
     *                   host's support)
     *
     * Return value:   None 
     * Scope:          Static
     **/
    void (*notify_peripheral_state)(otg_appctx_t appctx, 
        otg_peripheral_state_t state);

    /**
     * Function name:  notify_hnp_state
     * Description:    Informs the application of the current HNP state.
     * Requirement:    Mandatory
     * Parameters: 
     *     @appctx: (IN) Application context, as received from otg_app_init()
     *     @state:  (IN) The current HNP state
     *
     * Return value:   None 
     * Scope:          Static
     **/
    void (*notify_hnp_state)(otg_appctx_t appctx, otg_hnp_state_t state);

    /**
     * Function name:  notify_srp_state
     * Description:    Informs the application of the device's current SRP
     *                 state, which determines whether the device may start an
     *                 SRP process.
     * Requirement:    Mandatory
     * Parameters: 
     *     @appctx:     (IN) Application context, as received from
     *                       otg_app_init()
     *     @is_enabled: (IN) The current SRP state: 1 = Enabled / 0 = Disabled
     *
     * Return value:   None 
     * Scope:          Static
     **/
    void (*notify_srp_state)(otg_appctx_t appctx, jbool_t is_enabled);
    
    /**
     * Function name:  notify_error
     * Description:    Informs the application that an error of type
     *                 otg_error_t has occurred.
     * Requirement:    Mandatory
     * Parameters: 
     *     @appctx: (IN) Application context, as received from otg_app_init()
     *     @err:    (IN) The type of error that occurred
     *
     * Return value:   None 
     * Scope:          Static
     **/
    void (*notify_error)(otg_appctx_t appctx, otg_error_t err);

    /**
     * Function name:  notify_device_mode
     * Description:    Informs the application of the current device mode
     *                 (Host or Peripheral).
     * Requirement:    Mandatory
     * Parameters: 
     *     @appctx: (IN) Application context, as received from otg_app_init()
     *     @mode:   (IN) The current device mode
     *     
     * Return value:   None 
     * Scope:          Static
     **/
    void (*notify_device_mode)(otg_appctx_t appctx, otg_device_mode_t mode);

    /**
     * Function name: notify_device_connect_status
     * Description:   Informs the application of one of the following
     *                connection status changes:
     *                -- An A-type plug has been inserted and the device moved
     *                   from a B_IDLE to an A_IDLE state (status=TRUE).
     *                   After receiving this notification, VBus power-up may
     *                   be requested at any time until the A-type plug is
     *                   disconnected.
     *                -- An A-type plug has been disconnected and the device
     *                   moved from an A_IDLE to a B_IDLE state (status=FALSE).
     * Requirement:   Mandatory
     * Parameters: 
     *     @appctx:  (IN) Application context, as received from otg_app_init()
     *     @status:  (IN) True if an A-type plug has been inserted.
     *                    False if an A-type plug has been disconnected.
     *     
     * Return value:  None 
     * Scope:         Static
     **/
#define DEVICE_STATUS_DISCONNECTED 0
#define DEVICE_STATUS_CONNECTED 1
    void (*notify_device_connect_status)(otg_appctx_t appctx, jbool_t status);

    /**
     * Function name: respond_srp 
     * Description:   Informs the application that the connected B-device has
     *                initiated SRP after the Application had instructed the
     *                OTG Core to drop the VBUS.
     *                The Application must decide whether the OTG Core should
     *                respond to the SRP.
     * Requirement:   Mandatory
     * Parameters: 
     *     @appctx:  (IN) Application context, as received from otg_app_init()
     *     
     * Return value:  TRUE to respond to the SRP;
     *                FALSE to disregard the request
     * Scope:         Static
     **/ 
    jbool_t (*respond_srp)(otg_appctx_t appctx);

    /**
     * Function name:  otg_app_uninit
     * Description:    Un-initializes the OTG Application 
     * Requirement:    Optional
     * Parameters: 
     *     @appctx: (IN) Application context, as received from otg_app_init()
     *     
     * Return value:   0 on success, otherwise an error code 
     * Scope:          Static
     **/
    jresult_t (*otg_app_uninit)(otg_appctx_t appctx);
	 /*HSU TEMP FIX*/
	 /**
	  * Function name: notify_b_peripheral_state_transition
	  * Description:   Informs the application of a transition to b_peripheral_state
	  *                and that Vbus rised above B-Device Session Valid threshold.
	  * Requirement:   Mandatory
	  * Parameters:
	  *     @appctx:  (IN) Application context, as received from otg_app_init()
	  *    
	  * Return value:  None
	  * Scope:         Static
	  **/
	 void (*notify_b_peripheral_state_transition)(otg_appctx_t appctx);
	  /*END OF HSU TEMP FIX*/
} otg_app_callbacks_t;

typedef struct {
#define DROP_VBUS_TIMEOUT_INFINITE 0xFFFF
    juint16_t drop_vbus_timeout;
} otg_app_info_t;
/**
 * Function name:  otg_app_init
 * Description:    Initializes the OTG Application.
 *                 The function receives and stores the OTG Core context
 *                 required by the OTG Core functions, and returns to the OTG
 *                 Core the Application's callback functions and the
 *                 Application context to be passed to these callbacks.
 * Parameters: 
 *     @core_ctx:      (IN)  OTG Core context to be passed as an argument to the
 *                           OTG Core functions
 *     @otg_app_cbs:   (OUT) Pointer to a pre-allocated Application callback
 *                           functions structure to be filled by the function
 *     @otg_app_info:  (OUT) Pointer to a pre-allocated Application info data
 *                           structure to be filled by the function
 *     @appctx:        (OUT) Pointer to the Application context to be passed as
 *                           an argument to the Application's callbacks
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_app_init(otg_ctx_t core_ctx, otg_app_callbacks_t *otg_app_cbs, 
    otg_app_info_t *otg_app_info, otg_appctx_t *appctx);

/* ******************************************** *
 *      OTG Core --> Application Functions      *
 * ******************************************** */
/**
 * Function name:  otg_enable_hnp
 * Description:    Instructs the OTG Core, while acting as a host, to enable
 *                 role switching.
 *                 While acting as an A-device, the OTG Core will send a
 *                 SetFeature command for enabling HNP on the connected
 *                 B-device, and switch to suspend, in order for the connected
 *                 B-device to initiate HNP.
 *                 While acting as a B-device, the OTG Core will switch to
 *                 suspend in order for the connected A-device to revert back to
 *                 acting as host.
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received by otg_app_init()
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_enable_hnp(otg_ctx_t core_ctx);

/**
 * Function name:  otg_start_hnp
 * Description:    Instructs the OTG Core, while acting as a B-device, to start
 *                 HNP in order to become a host. Note that a B-device may 
 *                 start HNP only after it has been HNP enabled.
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received by otg_app_init()
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_start_hnp(otg_ctx_t core_ctx);

/**
 * Function name:  otg_start_srp
 * Description:    Initializes an SRP process.
 * Parameters: 
 *     @core_ctx: (IN) OTG Core context, as received by otg_app_init()
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_start_srp(otg_ctx_t core_ctx);

/**
 * Function name:  otg_set_whitelist
 * Description:    Sets the host's peripherals whitelist, which defines the
 *                 types of peripherals that the host supports.
 * Parameters: 
 *     @core_ctx:       (IN) OTG Core context, as received by otg_app_init()
 *     @whitelist_size: (IN) The number of elements in the whitelist array
 *     @whitelist:      (IN) Array of "peripheral matcher" structures, which
 *                           define the host's peripherals whitelist, i.e. the
 *                           type of peripherals that the host supports
 *     
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_set_whitelist(otg_ctx_t core_ctx, juint32_t whitelist_size,
    peripheral_matcher_t *whitelist);

/**
 * Function name:  otg_vbus_shutdown
 * Description:    Shuts down the VBUS to the connected device
 * Parameters: 
 *     @core_ctx:  (IN) OTG Core context, as received by otg_app_init()
 *     
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_vbus_shutdown(otg_ctx_t core_ctx);

/**
 * Function name:  otg_vbus_powerup
 * Description:    Powers-up the VBUS to the connected device
 * Parameters: 
 *     @core_ctx:  (IN) OTG Core context, as received by otg_app_init()
 *     
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Global
 **/
jresult_t otg_vbus_powerup(otg_ctx_t core_ctx);

#endif
