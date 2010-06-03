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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.   http://www/jungo/com */
#ifndef _UW_HOST_ICCD_H_
#define _UW_HOST_ICCD_H_

/**
 * Note: "ICCD Class Specification" in this file refers to the:
 * "Universal Serial Bus, Device Class: Smart Card, ICCD Specification for
 * USB Integrated Circuit(s) Card Devices, Revision 1.0, April 22nd 2005"
 **/

/* Values of bResponseType field */
typedef juint8_t iccd_response_type_t;
#define UWH_ICCD_BRESPONSE_INFORMATION 0x00
#define UWH_ICCD_BRESPONSE_BSTATUS_ERROR 0x40
#define UWH_ICCD_BRESPONSE_POLLING 0x80

/* Values of bStatus bmIccStatus field */
typedef juint8_t iccd_status_t;
#define UWH_ICCD_BSTATUS_PRESENT_ACTIVE 0x0
#define UWH_ICCD_BSTATUS_PRESENT_NOT_ACTIVE 0x1
#define UWH_ICCD_BSTATUS_VIRTUALLY_NOT_PRESENT 0x2

/* abData type for polling */
typedef juint16_t delay_time_t;

/* Values that abData field may contain */
typedef union {
    iccd_status_t status;
    delay_time_t delay;
} ab_data_t;

/* Values of bError field */
typedef jint8_t iccd_error_t;
#define UWH_ICCD_BERROR_ICC_MUTE -2
#define UWH_ICCD_BERROR_XFER_OVERRUN -4
#define UWH_ICCD_BERROR_ICC_HW_ERROR -5

/* ICCD class driver context type */
typedef void *uwh_iccd_drv_h;

/* Application context type */
typedef void *uwh_iccd_app_h;

/* ICCD class descriptor */
typedef struct {
    juint8_t length;                      /* 0x36 */
    juint8_t descriptor_type;             /* 0x21 */
    juint16_t release_number;
    juint8_t max_slot_index;
    juint8_t voltage_support;
    juint32_t protocols;
    juint32_t default_clock;
    juint32_t maximum_clock;
    juint8_t num_clock_supported;
    juint32_t data_rate;
    juint32_t max_data_rate;
    juint8_t num_data_rate_supported;
    juint32_t max_ifsd;
    juint32_t synch_protocols;
    juint32_t mechanical;
    juint32_t features;
    juint32_t max_ccid_message_length;
    juint8_t class_get_response;
    juint8_t class_envelope;
    juint16_t lcd_layout;
    juint8_t pin_support;
    juint8_t max_ccid_busy_slots;
} uwh_iccd_descriptor_t;

/***************************************************************************/
/*                      Application Callbacks Structure                    */
/***************************************************************************/
typedef struct {
  /**
   * Callback field: attach
   * Description:    Device attach application callback.
   *                 Performs the required application initialization for
   *                 handling the new device, such as resources allocation.
   * Parameters:
   *     @drv_ctx  (IN):  Driver context.
   *     @app_ctx  (OUT): Pointer to private application context
   *                      (uwh_iccd_app_h), which should be used in calls to
   *                      the other application callbacks.
   *
   * Return value:   0 on success; error code on failure.
   * Scope:          Static
   **/
    jresult_t (*attach)(uwh_iccd_drv_h drv_ctx, uwh_iccd_app_h *app_ctx);
  /**
   * Callback field: detach
   * Description:    Device detach application callback.
   * Parameters:
   *     @app_ctx  (IN): Application context.
   *
   * Return value:   None
   * Scope:          Static
   **/
    void (*detach)(uwh_iccd_app_h app_ctx);
  /**
   * Callback field: slot_change
   * Description:    N/A -- currently not supported.
   **/
    void (*slot_change)(juint8_t message_type, juint8_t slot_status, 
        uwh_iccd_app_h app_ctx);
} uwh_iccd_callbacks_t;

/***************************************************************************/
/*                        ICCD Class Driver API                            */
/***************************************************************************/
typedef void (*uwh_iccd_cb)(uwh_iccd_app_h app_ctx, 
    void *data, juint16_t size, iccd_response_type_t response_type,
    ab_data_t ab_data, iccd_error_t error, void *arg, jresult_t usb_status);

/**
 * Function name:  uwh_iccd_register
 * Description:    Registers an ICCD application with the ICCD Class Driver.
 *                 The class driver will be operational only after the
 *                 registration function is called and returns successfully.
 *                 The application should call this function during its
 *                 initialization.
 * Parameters:
 *     @callbacks (IN): Pointer to a structure containing the application's
 *                      callback functions.
 *
 * Return value:   0 on success; error code on failure.
 * Scope:          Global
 **/
jresult_t uwh_iccd_register(uwh_iccd_callbacks_t *callbacks);

/**
 * Function name:  uwh_iccd_unregister
 * Description:    Unregisters an application from the ICCD Class Driver.
 * Parameters:     None
 * Return value:   None
 * Scope:          Global
 **/
void uwh_iccd_unregister(void);

/**
 * Function name:  uwh_iccd_power
 * Description:    Sends an ICC_POWER_ON or ICC_POWER_OFF request.
 *                 The ICC_POWER_ON and ICC_POWER_OFF requests are described in
 *                 the Table 6.2-9 and 6.2-10 of the ICCD Class Specification.
 * Parameters:
 *     @drv_ctx  (IN): Driver context.
 *     @power_on (IN): TRUE = send an ICC_POWER_ON request.
 *                     FALSE = send an ICC_POWER_OFF request.
 *     @cb       (IN): Callback function to call at the end of the transfer.
 *
 * Return value:  0 on success; error code on failure.
 * Scope:         Global
 **/
jresult_t uwh_iccd_power(uwh_iccd_drv_h drv_ctx, jbool_t power_on, 
    uwh_iccd_cb cb);

/**
 * Function name:  uwh_iccd_xfr_block
 * Description:    Transfers data from the host to the USB-ICC, using the.
 *                 XFER_BLOCK ICCD request -- see Table 6.2-11 of the ICCD
 *                 Class Specification.
 * Parameters:
 *     @drv_ctx (IN): Driver context.
 *     @data    (IN): Handle to a buffer containing the data to send.
 *     @size    (IN): Buffer size, in bytes.
 *     @cb      (IN): Callback function to call at the end of the transfer.
 *     @arg     (IN): Pointer to argument(s) for the callback function (@cb).
 *
 * Return value:  0 on success; error code on failure.
 * Scope:         Global
 **/
jresult_t uwh_iccd_xfr_block(uwh_iccd_drv_h drv_ctx, void *data, 
    juint16_t size, uwh_iccd_cb cb, void *arg);

/**
 * Function name:  uwh_iccd_data_block
 * Description:    Transfers data from the USB-ICC to the host, using the.
 *                 DATA_BLOCK ICCD request -- see Table 6.2-12 of the ICCD
 *                 Class Specification.
 * Parameters:
 *     @drv_ctx (IN): Driver context.
 *     @data    (IN): Handle to a buffer to be filled with the read data.
 *     @size    (IN): Buffer size, in bytes.
 *     @cb      (IN): Callback function to call at the end of the transfer.
 *     @arg     (IN): Pointer to argument(s) for the callback function (@cb).
 *
 * Return value:  0 on success; error code on failure.
 * Scope:         Global
 **/
jresult_t uwh_iccd_data_block(uwh_iccd_drv_h drv_ctx, void *data, 
    juint16_t size, uwh_iccd_cb cb, void *arg);

/**
 * Function name:  uwh_get_info
 * Description:    Returns ICCD descriptor information (uwh_iccd_descriptor_t).
 *                 The ICCD descriptor structure is defined in table 5.1-1 of
 *                 the ICCD Class Specification.
 * Parameters:     
 *     @drv_ctx (IN): Driver context.
 * Return value:   Pointer to an ICCD descriptor information structure
 *                 (uwh_iccd_descriptor_t).
 * Scope:          Global
 **/
uwh_iccd_descriptor_t *uwh_get_info(uwh_iccd_drv_h drv_ctx);

/**
 * Function name:  uwh_iccd_suspend
 * Description:    Suspends an ICCD (Selective Suspend).
 *                 After the device is suspended, the host stack will also
 *                 attempt to suspend all other inactive bus segments.

 * * Parameters:     
 *     @drv_ctx (IN): Driver context.
  * Return value:  0 on success; error code on failure.
 * Scope:          Global
 **/
jresult_t uwh_iccd_suspend(uwh_iccd_drv_h drv_ctx);

/**
 * Function name:  uwh_iccd_resume
 * Description:    Resumes normal operation of a suspended ICCD.
 * * Parameters:     
 *     @drv_ctx (IN): Driver context.
  * Return value:  0 on success; error code on failure.
 * Scope:          Global
 **/
jresult_t uwh_iccd_resume(uwh_iccd_drv_h drv_ctx);

#endif

