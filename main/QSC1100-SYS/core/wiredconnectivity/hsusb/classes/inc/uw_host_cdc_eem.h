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

#ifndef _UW_HOST_CDC_EEM_H_
#define _UW_HOST_CDC_EEM_H_

#include "uw_host_cdc.h"

/*-------------------------------------
  CDC EEM Class Driver API Header File
  -------------------------------------*/

/* NOTE:
 * The EEM Class Driver functions declared in this file are described in the
 * jclass/jcdc/jcdc_eem.c file, which contains the functions' implementations.*/


/*******************************************************
 * EEM Device Attach/Detach Application Callback Types *
 *******************************************************/

/**
 * Function type:  uwh_eem_attach_cb_t
 * Description:    Attaches a CDC EEM device to the EEM class driver.   
 * Requirement:    Mandatory
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *     @app_ctx: (IN) Application context 
 *
 * Return value:   0 on success, otherwise an error code
 **/
typedef jresult_t (*uwh_eem_attach_cb_t)(uwh_cdc_dev_h dev_ctx, 
    uwh_cdc_app_h *app_ctx);

/**
 * Function type:  uwh_eem_detach_cb_t
 * Description:    Detaches a CDC EEM device from the EEM class driver.   
 * Requirement:    Mandatory
 * Parameters:
 *     @app_ctx: (IN) Application context 
 *
 * Return value:   None
 **/
typedef void (*uwh_eem_detach_cb_t)(uwh_cdc_app_h app_ctx);


/**********************************************************
 * Device-to-Host EEM Commands Application Callback Types *
 **********************************************************/

/**
 * Function type:  uwh_eem_echo_cb_t
 * Description:    Notifies the application that an Echo command or
 *                 Echo Response command was received from the device.
 * Requirement:    Optional
 * Parameters:
 *     @app_ctx:  (IN) Application context 
 *     @buffer:   (IN) Pointer to a buffer containing the received data
 *     @size:     (IN) Size of the received data, in bytes
 *     @response: (IN) TRUE  - Echo Response command received
 *                     FALSE - Echo command received
 *
 * Return value:   None
 **/
typedef void (*uwh_eem_echo_cb_t)(uwh_cdc_app_h app_ctx, 
    void *buffer, juint16_t size, jbool_t response);

/**
 * Function type:  uwh_eem_suspend_hint_cb_t
 * Description:    Notifies the application that the device sent a SuspendHint
 *                 command, indicating that it is safe to suspend the device.
 * Requirement:    Optional
 * Parameters:
 *     @app_ctx: (IN) Application context 
 *
 * Return value:   None
 **/
typedef void (*uwh_eem_suspend_hint_cb_t)(uwh_cdc_app_h app_ctx);

/**
 * Function type:  uwh_eem_response_hint_cb_t
 * Description:    Notifies the host that the device sent a 
 *                 ResponseCompleteHint or a ResponseHint command.
 *                 Notifies the application that the device sent one of the
 *                 following commands:
 *                 -- ResponseHint -- to inform the host of a suggested response
 *                    time interval in which the host may suspend polling of the
 *                    device.
 *                 -- ResponseCompleteHint -- to inform the host that the device
 *                    has no further information to transmit to the host at this
 *                    time.
 * Requirement:    Optional
 * Parameters:
 *     @app_ctx:  (IN) Application context 
 *     @interval: (IN) Meaning depends upon which command was received (see the
 *                     @complete parameter): 
 *                     ResponseCompleteHint - Timeout, in seconds
 *                     ResponseHint         - Recommended response time
 *                                            interval, in milliseconds
 *     @complete: (IN) TRUE  - ResponseCompleteHint command received
 *                     FALSE - ResponseHint command received
 *
 * Return value:   None
 **/
typedef void (*uwh_eem_response_hint_cb_t)(uwh_cdc_app_h app_ctx, 
    juint16_t interval, jbool_t complete);

/* EEM Application Callbacks Structure - supplied at registration */
typedef struct {
    uwh_eem_attach_cb_t attach;                 /* Mandatory */
    uwh_eem_detach_cb_t detach;                 /* Mandatory */
    uwh_eem_echo_cb_t echo;                     /* Optional */
    uwh_eem_suspend_hint_cb_t suspend_hint;     /* Optional */
    uwh_eem_response_hint_cb_t response_hint;   /* Optional */
} uwh_eem_callbacks_t;


/*********************************************
 * Data Transfer Application Callback Types *
 ********************************************/

/**
 * Function type:  uwh_eem_data_cb_t
 * Description:    General single-packet data transfer completion callback.
 * Parameters:
 *     @app_ctx:  (IN) Application context 
 *     @status:   (IN) Transfer status: 0 if successful, otherwise an error
 *     @buffer:   (IN) Pointer to a buffer containing a single received data
 *                     packet -- if called from uwh_eem_start_rx(); OR
 *                     pointer to the sent buffer -- if called  from
 *                     uwh_eem_send_data_packet() or uwh_eem_echo().
 *     @size:     (IN) Size of the received data, in bytes
 *     @app_priv: (IN) Private application data
 *
 * Return value:   None
 **/
typedef void (*uwh_eem_data_cb_t)(uwh_cdc_app_h app_ctx, jresult_t status,
    void *buffer, juint32_t size, void *app_priv);

/**
 * Function type:  uwh_eem_data_bundle_cb_t
 * Description:    General bundle-packet data transfer completion callback.
 * Parameters:
 *     @app_ctx:       (IN) Application context 
 *     @status:        (IN) Transfer status: 0 if successful, otherwise an error
 *     @buffers:       (IN) Pointer to an array of buffers containing the data
 *     @sizes:         (IN) Pointer to an array of values depicting the sizes,
 *                          in bytes, of the received data
 *     @num_data_pkts: (IN) Number of data packets in the buffers array
 *     @app_priv:      (IN) Private application data
 *
 * Return value:   None
 **/
typedef void (*uwh_eem_data_bundle_cb_t)(uwh_cdc_app_h app_ctx, 
    jresult_t status, void **buffers, juint32_t *sizes, juint16_t num_data_pkts, 
    void *app_priv);


/******************************
 * EEM Class Driver Functions *
 ******************************/

jresult_t uwh_eem_register(uwh_eem_callbacks_t *eem_cb);
void uwh_eem_unregister(void);

jresult_t uwh_eem_suspend(uwh_cdc_dev_h dev_ctx);
jresult_t uwh_eem_resume(uwh_cdc_dev_h dev_ctx);

jresult_t uwh_eem_enable(uwh_cdc_dev_h dev_ctx);
jresult_t uwh_eem_disable(uwh_cdc_dev_h dev_ctx);

jresult_t uwh_eem_echo(uwh_cdc_dev_h dev_ctx, void *buffer, juint16_t size,
    uwh_eem_data_cb_t cb, void *app_priv, jbool_t response);
jresult_t uwh_eem_tickle(uwh_cdc_dev_h dev_ctx);
jresult_t uwh_eem_send_data_packet(uwh_cdc_dev_h dev_ctx, void *buffer,
    juint32_t size, juint32_t crc, uwh_eem_data_cb_t cb, void *app_priv);
jresult_t uwh_eem_send_data_bundle(uwh_cdc_dev_h dev_ctx, void **buffers, 
    juint32_t *sizes, juint16_t num_data_pkts, uwh_eem_data_bundle_cb_t cb, 
    void *app_priv);
jresult_t uwh_eem_start_rx(uwh_cdc_dev_h dev_ctx, uwh_eem_data_cb_t cb, 
    void *app_priv, juint32_t rx_req_pool_size);
jresult_t uwh_eem_abort_tx(uwh_cdc_dev_h dev_ctx);
jresult_t uwh_eem_stop_rx(uwh_cdc_dev_h dev_ctx);

#endif

